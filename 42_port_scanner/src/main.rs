use clap::Parser;
use std::io::prelude::*;
use std::net::TcpStream;

#[derive(Parser, Debug)]
#[command(version, about)]
struct Args {
    #[arg(short = 't', long)]
    target: String,
    #[arg(long)]
    protocol: Option<String>,
    #[arg(short, long)]
    port: u8
}

fn main() -> std::io::Result<()> {

    let args = Args::parse();

    println!("{:?}", args);

    let mut stream: TcpStream = match TcpStream::connect(args.target + ":" + args.port.to_string().as_str()) {
        Ok(ts) => ts,
        Err(e) => return Err(e)
    };
    let buf: &[u8] = &[1];
    let ret_bytes = match stream.write(buf) {
        Ok(bytes) => bytes,
        Err(e) => return Err(e)
    };
    if ret_bytes == std::mem::size_of_val(buf) {
        println!("same amount of bytes returned, so must be working, right?");
    }
    Ok(())
}
