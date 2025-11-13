use clap::Parser;

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

fn main() {

    let args = Args::parse();

    println!("{:?}", args);


}
