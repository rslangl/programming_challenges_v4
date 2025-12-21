use std::{
    io::{Write, stdin},
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

    let remote_addr = String::from(server.peer_addr()?.ip().to_string());

    let mut input_buffer: [u8; 255] = [0x0; 255];
    let mut output_buffer = String::new();

    loop {
        // if let Ok(message_bytes) = server.read(&mut input_buffer[..]) {
        //     if message_bytes > 0 {
        //         let system_time = SystemTime::now();
        //         let datetime: DateTime<Utc> = system_time.into();
        //
        //         print!(
        //             "[{}:{}]: {}",
        //             datetime.format("%d/%m/%Y %T"),
        //             remote_addr,
        //             str::from_utf8(&input_buffer)?
        //         );
        //     }
        //
        //     // clear input buffer
        //     input_buffer.fill(0);
        // }

        print!("[Send to {}]: ", server.peer_addr()?.ip().to_string());
        std::io::stdout().flush()?;

        // returns the read input in usize
        let _ = stdin().read_line(&mut output_buffer)?;

        server.write(output_buffer.as_bytes())?;

        output_buffer.clear();
    }
}
