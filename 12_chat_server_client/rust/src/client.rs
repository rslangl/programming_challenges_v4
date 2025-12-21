use std::{
    io::{Read, Write, stdin},
    net::TcpStream,
};

use clap::Parser;

#[derive(Parser)]
struct Cli {
    address: String,
}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let cli = Cli::parse();

    let mut server = match TcpStream::connect(cli.address) {
        Ok(s) => {
            println!("info: connection established");
            s
        }
        Err(e) => {
            eprintln!("error: could not establish connection: {}", e);
            return Err(Box::new(e));
        }
    };

    let mut buf = String::new();

    loop {
        print!("[Send to {}]: ", server.peer_addr()?.ip().to_string());
        std::io::stdout().flush()?;

        // returns the read input in usize
        let _ = stdin().read_line(&mut buf)?;

        server.write(buf.as_bytes())?;

        buf.clear();
    }
}
