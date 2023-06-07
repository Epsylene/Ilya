
mod engine;
mod window;
mod gui;

use crate::engine::{Engine, EngineOptions};
use crate::window::WindowOptions;

fn main() {
    let options = EngineOptions {
        window: WindowOptions {
            width: 600,
            height: 500,
            name: "Ilya", 
        },
    };

    let app = Engine::new(options);
    app.run();
}