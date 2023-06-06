use crate::gui::Framework;
use crate::ray::Ray;

use pixels::{SurfaceTexture, Pixels};
use winit::{window, event_loop::{EventLoop, ControlFlow}, dpi::LogicalSize, window::WindowBuilder, event::{Event, WindowEvent, VirtualKeyCode}};
use winit_input_helper::WinitInputHelper;

use crate::point::Point3;
use glam::Vec3;

use crate::scene::Scene;

struct Window
{
    handle: window::Window,
    input: WinitInputHelper,
    texture: Pixels,
    gui: Framework,
}

#[derive(Copy, Clone)]
pub struct WindowOptions {
    pub width: u32,
    pub height: u32,
    pub name: &'static str,
}

pub struct EngineOptions {
    pub window: WindowOptions,
}



pub struct Engine {
    pub scene: Scene,
    pub options: EngineOptions,
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

impl Engine {
    pub fn new(options: EngineOptions) -> Self {
        let mut scene = Scene::new();

        scene.add_sphere();
        
        Self {
            scene,
            options,
        }
    }

    pub fn run(self) {
        let event_loop = EventLoop::new();
        let wo = self.options.window;
        let mut window = Window::new(wo.width, wo.height, wo.name, &event_loop);

        // Main app loop
        event_loop.run(move |event, _, control_flow| {
            // Update the input
            if window.input.update(&event) {
                // Close events
                if window.input.key_pressed(VirtualKeyCode::Escape) || window.input.close_requested() || window.input.destroyed() {
                    control_flow.set_exit();
                    return;
                }

                window.handle.request_redraw();
            }
            
            // Update the window
            match event {
                Event::WindowEvent { event, .. } => {
                    // Update egui inputs
                    window.gui.handle_event(&event);
                
                    if event == WindowEvent::CloseRequested {
                        control_flow.set_exit();
                    }
                }

                // Draw the current frame
                Event::RedrawRequested(_) => {
                    // Draw the world
                    Engine::render(&self, window.texture.frame_mut());

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
                        control_flow.set_exit();
                    }
                }

                _ => (),
            }
        });
    }

    fn render(&self, frame: &mut [u8]) {
        let wo = self.options.window;
        let width = wo.width;
        let height = wo.height;
        let aspect = width as f32 / height as f32;
        let focal_length = 1.0;

        let origin = Point3::default();
        let vertical = 2.0*Vec3::Y;
        let horizontal = 2.0*aspect*Vec3::X;
        let llc = Vec3::from(origin) - horizontal/2.0 - vertical/2.0 - Vec3::new(0.0, 0.0, focal_length);

        for (i, pixel) in frame.chunks_exact_mut(4).enumerate() {
            let x = i % width as usize;
            let y = i / width as usize;
    
            let u = x as f32 / (width-1) as f32;
            let v = 1.0 - (y as f32 / (height-1) as f32);
    
            let ray = Ray::new(
                origin,
                llc + u*horizontal + v*vertical - Vec3::from(origin),
            );
    
            pixel.copy_from_slice(&ray.color(&self.scene).to_255());
        }
    }
}