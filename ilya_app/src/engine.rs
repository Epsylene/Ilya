use winit::{
    event_loop::EventLoop,
    event::{Event, WindowEvent, VirtualKeyCode}
};

use ilya::{
    scene::Scene,
    renderer::Renderer,
};

use crate::window::{Window, WindowOptions};

pub struct EngineOptions {
    pub window: WindowOptions,
}

pub struct Engine {
    pub scene: Scene,
    pub options: EngineOptions,
}

impl Engine {
    pub fn new(options: EngineOptions) -> Self {
        let wo = options.window;
        let mut scene = Scene::new((wo.width, wo.height));
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
                    Renderer::render(&self.scene, window.texture.frame_mut());

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
}