
mod engine;
mod gui;
mod color;
mod ray;
mod point;
mod hittable;
mod scene;

use crate::engine::{Engine, EngineOptions, WindowOptions};

fn main() {
    let options = EngineOptions {
        window: WindowOptions {
            width: 600,
            height: 500,
            name: "Ilya", 
        }
    };

    let app = Engine::new(options);
    app.run();
}