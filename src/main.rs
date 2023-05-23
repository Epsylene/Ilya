
mod app;
mod gui;

use crate::app::{App, AppOptions};

fn main() {
    let options = AppOptions {
        width: 600,
        height: 500,
        name: "Ilya",
    };
    
    App::run(options);
}