
use std::sync::Arc;
use crate::hittable::{Hittable, Sphere};
use glam::Vec3;

pub struct Scene {
    pub objects: Vec<Arc<dyn Hittable>>,
}

impl Scene {
    pub fn new() -> Self {
        Self {
            objects: Vec::new(),
        }
    }

    pub fn add_sphere(&mut self) {
        self.objects.push(Arc::new(Sphere::new(Vec3::new(0.0, 0.0, -1.0), 0.5)));
    }
}