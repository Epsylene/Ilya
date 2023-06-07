
use std::sync::Arc;
use crate::hittable::{Hittable, Sphere};
use glam::Vec3;

pub struct Scene {
    pub objects: Vec<Arc<dyn Hittable>>,
    pub dimensions: (u32, u32),
}

impl Scene {
    pub fn new(dimensions: (u32, u32)) -> Self {
        Self {
            objects: Vec::new(),
            dimensions,
        }
    }

    pub fn add_sphere(&mut self) {
        self.objects.push(Arc::new(Sphere::new(Vec3::new(0.0, 0.0, -1.0), 0.5)));
    }
}