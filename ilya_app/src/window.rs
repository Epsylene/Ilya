use pixels::{SurfaceTexture, Pixels};
use winit::{
    window, 
    event_loop::EventLoop, 
    dpi::LogicalSize, 
    window::WindowBuilder,
};
use winit_input_helper::WinitInputHelper;

use crate::gui::Framework;

pub struct Window
{
    pub handle: window::Window,
    pub input: WinitInputHelper,
    pub texture: Pixels,
    pub gui: Framework,
}

#[derive(Copy, Clone)]
pub struct WindowOptions {
    pub width: u32,
    pub height: u32,
    pub name: &'static str,
}

impl Window {
    pub fn new(width: u32, height: u32, name: &str, event_loop: &EventLoop<()>) -> Self {
        let window = {
            let size = LogicalSize::new(width as f64, width as f64);
            WindowBuilder::new()
                .with_title(name)
                .with_inner_size(size)
                .with_min_inner_size(size)
                .build(event_loop)
                .unwrap()
        };

        let input = WinitInputHelper::new();

        let (texture, gui) = {
            let window_size = window.inner_size();
            let scale_factor = window.scale_factor() as f32;
            let surface_texture = SurfaceTexture::new(window_size.width, window_size.height, &window);
            let texture = Pixels::new(width, height, surface_texture).unwrap();
            let gui = Framework::new(
                event_loop,
                window_size.width,
                window_size.height,
                scale_factor,
                &texture,
            );
    
            (texture, gui)
        };

        Self {
            handle: window,
            input,
            texture,
            gui,
        }
    }
}