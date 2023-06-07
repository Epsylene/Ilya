
use crate::{
    ray::Ray,
    utils::point::Point3,
    scene::Scene,
};
use glam::Vec3;

pub struct Renderer;

impl Renderer {
    pub fn render(scene: &Scene, frame: &mut [u8]) {
        let (width, height) = scene.dimensions;
        for (i, pixel) in frame.chunks_exact_mut(4).enumerate() {
            let x = i % width as usize;
            let y = i / width as usize;

            let u = x as f32 / (width-1) as f32;
            let v = 1.0 - (y as f32 / (height-1) as f32);
            
            pixel.copy_from_slice(&Renderer::single(u, v, scene));
        }
    }

    fn single(u: f32, v: f32, scene: &Scene) -> [u8; 4] {
        let (width, height) = scene.dimensions;
        let aspect = width as f32 / height as f32;
        let focal_length = 1.0;

        let origin = Point3::default();
        let vertical = 2.0*Vec3::Y;
        let horizontal = 2.0*aspect*Vec3::X;
        let llc = Vec3::from(origin) - horizontal/2.0 - vertical/2.0 - Vec3::new(0.0, 0.0, focal_length);

        let ray = Ray::new(
            origin,
            llc + u*horizontal + v*vertical - Vec3::from(origin),
        );

        ray.color(scene).to_255()
    }
}