use std::{
    io::{Read, Write, stdin},
    net::{Shutdown, TcpListener, TcpStream},
    str,
    sync::mpsc::{Sender, channel},
    thread,
    time::SystemTime,
};

use chrono::{DateTime, Utc};
use clap::{Parser, Subcommand, arg};

/// ==== CLI layer ====
/// Examples:
/// chat server --listener 0.0.0.0:1337
/// chat client --address 127.0.0.1:1337

#[derive(Parser)]
#[command(version, about, long_about = None)]
struct Cli {
    #[command(subcommand)]
    role: RoleCmd,
}

#[derive(Subcommand)]
enum RoleCmd {
    Client {
        #[arg(long)]
        address: String,
    },
    Server {
        #[arg(long)]
        listener: String,
    },
}

/// ==== Domain layer ====

struct ClientRole {
    server: TcpStream,
}

struct ServerRole {
    listener: TcpListener,
}

enum Role {
    Client(ClientRole),
    Server(ServerRole),
}

trait Runnable {
    fn run(&mut self) -> Result<(), RoleError>;
}

impl Runnable for Role {
    fn run(&mut self) -> Result<(), RoleError> {
        match self {
            Role::Client(c) => c.run(),
            Role::Server(s) => s.run(),
        }
    }
}

impl Runnable for ClientRole {
    fn run(&mut self) -> Result<(), RoleError> {
        // shared channel for writing to stdout
        let (tx, rx) = channel();

        let server_addr = match self.server.local_addr() {
            Ok(sa) => sa.to_string(),
            Err(e) => return Err(RoleError::Tcp(e)),
        };

        let reader = self.server.try_clone()?;

        // worker thread for incoming messages
        {
            let remote_addr = match reader.peer_addr() {
                Ok(ra) => String::from(ra.ip().to_string()),
                Err(e) => return Err(RoleError::Tcp(e)),
            };

            reader_thread(reader, tx.clone(), remote_addr);
        }

        // stdout handling
        let ui = thread::spawn(move || {
            let mut out = std::io::stdout();
            let prompt = format_args!("[Send to {}]:", server_addr);

            loop {
                for msg in &rx {
                    // clear current prompt line in order to write incoming on the line above
                    write!(out, "\x1b[2K\r").unwrap();
                    write!(out, "{msg}").unwrap();
                    write!(out, "{prompt}").unwrap();
                    out.flush().unwrap();
                }
            }
        });

        let mut buffer = String::new();

        // input handling
        loop {
            if let Ok(_) = stdin().read_line(&mut buffer) {
                match buffer.as_str() {
                    "/quit" => break,
                    _ => {
                        let _ = self.server.write(buffer.as_bytes());
                    }
                }
            };

            buffer.clear();
        }

        self.server
            .shutdown(Shutdown::Both)
            .map_err(RoleError::Tcp)?;

        ui.join().unwrap();

        drop(tx);

        Ok(())
    }
}

impl Runnable for ServerRole {
    fn run(&mut self) -> Result<(), RoleError> {
        for conn in self.listener.incoming() {
            match conn {
                Ok(c) => {
                    // shared channel for writing to stdout
                    let (tx, rx) = channel();

                    let server_addr = match self.listener.local_addr() {
                        Ok(sa) => sa.to_string(),
                        Err(e) => return Err(RoleError::Tcp(e)),
                    };

                    // handles for I/O
                    let mut writer = c.try_clone().map_err(RoleError::Tcp)?;
                    let reader = c;

                    // worker thread for incoming messages
                    {
                        let remote_addr = match reader.peer_addr() {
                            Ok(ra) => String::from(ra.ip().to_string()),
                            Err(e) => return Err(RoleError::Tcp(e)),
                        };

                        reader_thread(reader, tx.clone(), remote_addr);
                    }

                    // stdout handling
                    let ui = thread::spawn(move || {
                        let mut out = std::io::stdout();
                        let prompt = format_args!("[Send to {}]:", server_addr);

                        loop {
                            for msg in &rx {
                                // clear current prompt line in order to write incoming on the line above
                                write!(out, "\x1b[2K\r").unwrap();
                                write!(out, "{msg}").unwrap();
                                write!(out, "{prompt}").unwrap();
                                out.flush().unwrap();
                            }
                        }
                    });

                    let mut buffer = String::new();

                    // input handling
                    loop {
                        if let Ok(_) = stdin().read_line(&mut buffer) {
                            match buffer.as_str() {
                                "/quit" => break,
                                _ => {
                                    let _ = writer.write(buffer.as_bytes());
                                }
                            }
                        };

                        buffer.clear();
                    }

                    writer.shutdown(Shutdown::Both).map_err(RoleError::Tcp)?;

                    ui.join().unwrap();

                    drop(tx);
                }
                Err(e) => {
                    eprintln!("error: could not handle connection: {}", e);
                    return Err(RoleError::Tcp(e));
                }
            }
        }
        Ok(())
    }
}

#[derive(Debug)]
enum RoleError {
    Tcp(std::io::Error),
}

impl From<std::io::Error> for RoleError {
    fn from(e: std::io::Error) -> Self {
        RoleError::Tcp(e)
    }
}

impl TryFrom<RoleCmd> for Role {
    type Error = RoleError;

    fn try_from(cmd: RoleCmd) -> Result<Self, RoleError> {
        match cmd {
            RoleCmd::Client { address } => {
                let connection = TcpStream::connect(address).map_err(RoleError::Tcp)?;
                Ok(Role::Client(ClientRole { server: connection }))
            }
            RoleCmd::Server { listener } => {
                let host = TcpListener::bind(listener).map_err(RoleError::Tcp)?;
                Ok(Role::Server(ServerRole { listener: host }))
            }
        }
    }
}

fn reader_thread(
    mut stream: TcpStream,
    tx: Sender<String>,
    remote_addr: String,
) -> thread::JoinHandle<Result<(), Box<dyn std::error::Error + Send>>> {
    thread::spawn(move || {
        let mut buffer = [0u8; 255];

        loop {
            if let Ok(message_bytes) = stream.read(&mut buffer) {
                if message_bytes > 0 {
                    if let Ok(message) = str::from_utf8(&buffer[..message_bytes]) {
                        let payload = format_incoming(message, remote_addr.as_str());
                        tx.send(payload).unwrap()
                    };
                }
                buffer.fill(0);
            }
        }
    })
}

fn format_incoming(message: &str, remote_addr: &str) -> String {
    let datetime: DateTime<Utc> = SystemTime::now().into();
    let datetime_str = datetime.format("%d/%m/%Y %T").to_string();
    let mut payload = String::new();

    payload.push_str("[");
    payload.push_str(datetime_str.as_str());
    payload.push_str(" ");
    payload.push_str(remote_addr);
    payload.push_str("]: ");
    payload.push_str(message);

    payload
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let cli = Cli::parse();

    let mut role = Role::try_from(cli.role).unwrap_or_else(|e| {
        eprintln!("error: could not initialize: {e:?}");
        std::process::exit(1)
    });

    match role.run() {
        Ok(_) => return Ok(()),
        Err(e) => {
            eprintln!("error: runtime error: {e:?}");
            std::process::exit(1)
        }
    };
}
