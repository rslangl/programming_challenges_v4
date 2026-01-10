use std::{
    fs::{self, File},
    path::PathBuf,
};

use clap::{ArgGroup, Parser};

const CHUNK_SIZE_BYTES: u64 = 256;

#[derive(Parser, Debug)]
#[command(group(ArgGroup::new("operation").args(&["encrypt", "decrypt"]).required(true)))]
struct Args {
    #[arg(short, long)]
    input_file: String,

    #[arg(short, long)]
    output_file: String,

    #[arg(short, long)]
    encrypt: bool,

    #[arg(short, long)]
    decrypt: bool,
}

fn load(path: PathBuf) -> Result<Vec<u8>, Box<dyn std::error::Error>> {
    let file = File::open(path.as_os_str())?;
    let metadata = file.metadata()?;

    let bytes: Vec<u8> = fs::read(path.as_os_str())?;

    Ok(bytes)
}

fn encrypt() {}

fn decrypt() {}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    let args = Args::parse();

    let bytes = load(args.input_file.into())?;

    Ok(())
}
