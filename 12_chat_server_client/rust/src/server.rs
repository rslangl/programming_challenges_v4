use std::{
    io::{Read, Write, stdin},
    net::{Shutdown, TcpListener, TcpStream},
    str,
    sync::mpsc::channel,
    thread,
    time::SystemTime,
};

use chrono::{DateTime, Utc};
use clap::Parser;

#[derive(Parser)]
#[command(version, about, long_about = None)]
struct Cli {
    listen_address: String,
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let cli = Cli::parse();

    let server = match TcpListener::bind(cli.listen_address) {
        Ok(s) => {
            println!("info: connection established");
            s
        }
        Err(e) => {
            eprintln!("error: could not establish connection: {}", e);
            return Err(Box::new(e));
        }
    };

    for conn in server.incoming() {
        match conn {
            Ok(mut c) => {
                // shared channel for writing to stdout
                let (tx, rx) = channel();

                let server_addr = match server.local_addr() {
                    Ok(sa) => sa.to_string(),
                    Err(e) => return Err(Box::new(e)),
                };

                // client for outgoing traffic
                let mut client = match TcpStream::connect(server.local_addr().unwrap()) {
                    Ok(c) => c,
                    Err(e) => return Err(Box::new(e)),
                };

                // worker thread for incoming messages
                {
                    let tx = tx.clone();
                    let mut buffer: [u8; 255] = [0x0; 255];

                    thread::spawn(move || -> Result<(), Box<dyn std::error::Error + Send>> {
                        let remote_addr = match c.peer_addr() {
                            Ok(ra) => String::from(ra.ip().to_string()),
                            Err(e) => return Err(Box::new(e)),
                        };

                        loop {
                            if let Ok(message_bytes) = c.read(&mut buffer[..]) {
                                // TODO: check if size is at least equal to
                                // the expected wire-format (peer address +
                                // payload)
                                if message_bytes > 0 {
                                    if let Ok(message) = str::from_utf8(&buffer) {
                                        let datetime: DateTime<Utc> = SystemTime::now().into();
                                        let datetime_str =
                                            datetime.format("%d/%m/%Y %T").to_string();
                                        let mut payload = String::new();

                                        payload.push_str("[");
                                        payload.push_str(datetime_str.as_str());
                                        payload.push_str(" ");
                                        payload.push_str(remote_addr.as_str());
                                        payload.push_str("]: ");
                                        payload.push_str(message);

                                        tx.send(payload).unwrap()
                                    };
                                }
                                buffer.fill(0);
                            }
                        }
                    });
                }

                // stdout handling
                let ui = thread::spawn(move || {
                    // stdout handle
                    let mut out = std::io::stdout();
                    let prompt = format_args!("[Send to {}]:", server_addr);

                    loop {
                        for msg in &rx {
                            // clear current prompt line
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
                                let _ = client.write(buffer.as_bytes());
                                tx.send(buffer.clone()).unwrap();
                            }
                        }
                    };

                    // clear output buffer
                    buffer.clear();
                }

                client.shutdown(Shutdown::Both)?;

                ui.join().unwrap();

                drop(tx);
            }
            Err(e) => {
                eprintln!("error: could not handle connection: {}", e);
                return Err(Box::new(e));
            }
        }
    }

    Ok(())
}
