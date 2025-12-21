use std::{
    io::Read,
    net::{Shutdown, TcpListener},
    str,
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
            std::process::exit(1)
        }
    };

    for conn in server.incoming() {
        match conn {
            Ok(mut c) => {
                let client_addr = String::from(c.peer_addr()?.ip().to_string());

                // input buffer
                let mut buf: [u8; 255] = [0x0; 255];

                loop {
                    if let Ok(message_bytes) = c.read(&mut buf[..]) {
                        if message_bytes > 0 {
                            let system_time = SystemTime::now();
                            let datetime: DateTime<Utc> = system_time.into();

                            print!(
                                "[{}:{}]: {}",
                                datetime.format("%d/%m/%Y %T"),
                                client_addr,
                                str::from_utf8(&buf)?
                            );
                        }
                    }

                    buf.fill(0);
                }

                //c.shutdown(Shutdown::Both)?
            }
            Err(e) => {
                eprintln!("error: could not handle connection: {}", e);
                return Err(Box::new(e));
            }
        }
    }

    Ok(())
}
