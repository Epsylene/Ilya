
mod engine;
mod gui;
mod color;
mod ray;

use crate::engine::{Engine, EngineOptions, WindowOptions};

fn main() {
    let options = EngineOptions {
        window: WindowOptions {
            width: 600,
            height: 500,
            name: "Ilya", 
        }
    };
    
    Engine::run(options);
}