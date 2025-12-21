use std::{io::Read, net::TcpListener, str};

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
                // input buffer
                let mut buf: [u8; 255] = [0x0; 255];

                loop {
                    c.read(&mut buf[..])?;

                    let message = str::from_utf8(&buf)?;

                    print!("{}", message);

                    buf.fill(0);
                }
            }
            Err(e) => {
                eprintln!("error: could not handle connection: {}", e);
                return Err(Box::new(e));
            }
        }
    }

    Ok(())
}
