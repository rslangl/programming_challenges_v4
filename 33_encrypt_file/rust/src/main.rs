use std::{
    fs::{self, File},
    io::Read,
    path::{Path, PathBuf},
};

const CHUNK_SIZE_BYTES: u64 = 256;

fn load(path: PathBuf) -> Result<Vec<u8>, Box<dyn std::error::Error>> {
    let file = File::open(path.as_os_str())?;
    let metadata = file.metadata()?;

    let bytes: Vec<u8> = fs::read(path.as_os_str())?;

    Ok(bytes)
}

fn encrypt() {}

fn decrypt() {}

fn main() -> Result<(), Box<dyn std::error::Error>> {
    match std::env::args().len() {
        // two argument passed: input file, output file
        3 => {
            let input_file = std::env::args()
                .nth(1)
                .map(PathBuf::from)
                .expect("error: no input file path provided");

            let output_file = std::env::args()
                .nth(2)
                .map(PathBuf::from)
                .expect("error: no output file provided");

            let bytes = load(input_file)?;
        }
        _ => {
            eprintln!("error: input and output file arguments required");
            std::process::exit(2)
        }
    };

    Ok(())
}
