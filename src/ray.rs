
use crate::scene::Scene;
use crate::hittable::Hittable;
use crate::color::Color;
use crate::point::Point3;
use glam::Vec3;

pub struct Ray {
    pub origin: Point3,
    pub direction: Vec3,
}

impl Ray {
    pub fn new(origin: Point3, direction: Vec3) -> Self {
        Self { origin, direction }
    }

    pub fn at(&self, t: f32) -> Point3 {
        self.origin + t * self.direction
    }

    pub fn color(&self, world: &Scene) -> Color {
        let t = world.hit(self, 0.0001, f32::INFINITY);

        match t {
            Some(record) => {
                0.5 * (Color::WHITE + Color::from(record.normal.normalize()))
            }

            None => {
                let unit_dir = Vec3::normalize(self.direction);
                let t = 0.5 * (unit_dir.y + 1.0);

                (1.0 - t)*Color::WHITE + t*Color::rgb(0.5, 0.7, 1.0)
            }
        }
    }
}