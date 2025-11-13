use clap::Parser;

#[derive(Parser, Debug)]
struct Args {
    port: u8
}

fn main() {
    
    let args = Args::parse();

    println!("Port: {}", args.port);
}
