use std::env;

mod producer;
mod consumer;

#[tokio::main]
async fn main() {
    let args: Vec<String> = env::args().collect();
    
    if args.len() < 2 {
        eprintln!("Usage: {} [producer|consumer]", args[0]);
        std::process::exit(1);
    }

    match args[1].as_str() {
        "producer" => producer::run().await,
        "consumer" => consumer::run().await,
        _ => {
            eprintln!("Invalid option. Use 'producer' or 'consumer'");
            std::process::exit(1);
        }
    }
}