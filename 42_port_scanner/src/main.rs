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

    let mut stream: TcpStream = TcpStream::connect(args.target + ":" + args.port.to_string().as_str())?;
    stream.write(&[1])?;
    Ok(())
}
