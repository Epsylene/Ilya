
use std::ops;
use std::convert::From;
use glam::Vec3;

#[derive(Copy, Clone, Debug)]
pub struct Point3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

impl Point3 {
    pub const fn new(x: f32, y: f32, z: f32) -> Self {
        Self { x, y, z }
    }
}

impl Default for Point3 {
    fn default() -> Self {
        Self::new(0.0, 0.0, 0.0)
    }
}

impl ops::Add<Vec3> for Point3 {
    type Output = Self;

    fn add(self, rhs: Vec3) -> Self {
        Self::new(
            self.x + rhs.x,
            self.y + rhs.y,
            self.z + rhs.z,
        )
    }
}

impl ops::Sub<Vec3> for Point3 {
    type Output = Vec3;

    fn sub(self, rhs: Vec3) -> Vec3 {
        Vec3::new(
            self.x - rhs.x,
            self.y - rhs.y,
            self.z - rhs.z,
        )
    }
}

impl ops::Div<f32> for Point3 {
    type Output = Self;

    fn div(self, rhs: f32) -> Self {
        Self::new(
            self.x / rhs,
            self.y / rhs,
            self.z / rhs,
        )
    }
}

impl From<Point3> for Vec3 {
    fn from(point: Point3) -> Self {
        Self::new(point.x, point.y, point.z)
    }
}