fn main() {
    match std::env::args().len() {
        // one argument passed
        2 => {
            let input: u16 = match std::env::args()
                .nth(1)
                .expect("error: could not get argument")
                .parse()
            {
                Ok(n) => n,
                Err(_) => {
                    eprintln!("error: need a positive numeric input");
                    std::process::exit(2)
                }
            };

            for n in 0..input {
                if n % 15 == 0 {
                    println!("fizzbuzz");
                } else if n % 5 == 0 {
                    println!("fizz");
                } else if n % 3 == 0 {
                    println!("buzz");
                } else {
                    println!("{}", n);
                }
            }
        }
        // none, and all other arguments passed
        _ => {
            eprintln!("error: only one input allowed");
            std::process::exit(2)
        }
    }
}
