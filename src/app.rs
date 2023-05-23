use crate::gui::Framework;

use pixels::{SurfaceTexture, Pixels};
use winit::{window, event_loop::{EventLoop, ControlFlow}, dpi::LogicalSize, window::WindowBuilder, event::{Event, VirtualKeyCode}};
use winit_input_helper::WinitInputHelper;

pub struct Window
{
    handle: window::Window,
    input: WinitInputHelper,
    texture: Pixels,
    gui: Framework,
}

pub struct AppOptions {
    pub width: u32,
    pub height: u32,
    pub name: &'static str,
}

pub struct App;

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

impl App {
    pub fn run(options: AppOptions) {
        let event_loop = EventLoop::new();
        let mut window = Window::new(options.width, options.height, options.name, &event_loop);

        event_loop.run(move |event, _, control_flow| {
            // Handle input events
            if window.input.update(&event) {
                // Close events
                if window.input.key_pressed(VirtualKeyCode::Escape) || window.input.close_requested() || window.input.destroyed() {
                    *control_flow = ControlFlow::Exit;
                    return;
                }
    
                // Resize the window
                if let Some(scale) = window.input.scale_factor() {
                    window.gui.scale_factor(scale);
                }
    
                // Resize the window
                if let Some(size) = window.input.window_resized() {
                    if let Err(err) = window.texture.resize_surface(size.width, size.height) {
                        println!("pixels.resize_surface error, {:?}", err);
                        *control_flow = ControlFlow::Exit;
                        return;
                    }
                    window.gui.resize(size.width, size.height);
                }
    
                // Update internal state and request a redraw
                window.handle.request_redraw();
            }
    
            match event {
                Event::WindowEvent { event, .. } => {
                    // Update egui inputs
                    window.gui.handle_event(&event);
                }
                // Draw the current frame
                Event::RedrawRequested(_) => {
                    // Draw the world
                    App::render(window.texture.frame_mut(), &options);
    
                    // Prepare egui
                    window.gui.prepare(&window.handle);
    
                    // Render everything together
                    let render_result = window.texture.render_with(|encoder, render_target, context| {
                        // Render the world texture
                        context.scaling_renderer.render(encoder, render_target);
    
                        // Render egui
                        window.gui.render(encoder, render_target, context);
    
                        Ok(())
                    });
    
                    // Basic error handling
                    if let Err(err) = render_result {
                        println!("pixels.render error, {:?}", err);
                        *control_flow = ControlFlow::Exit;
                    }
                }
                _ => (),
            }
        });
    }

    fn render(frame: &mut [u8], options: &AppOptions) {
        let width = options.width;
        let height = options.height;

        for (i, pixel) in frame.chunks_exact_mut(4).enumerate() {
            let x = i % width as usize;
            let y = i / width as usize;
    
            let r = x as f64 / (width-1) as f64;
            let g = y as f64 / (height-1) as f64;
            let b = 0.25;
    
            let ir = (255.999 * r) as u8;
            let ig = (255.999 * g) as u8;
            let ib = (255.999 * b) as u8;
    
            pixel.copy_from_slice(&[ir, ig, ib, 255]);
        }
    }
}