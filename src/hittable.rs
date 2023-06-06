
use glam::Vec3;
use crate::scene::Scene;
use crate::point::Point3;
use crate::ray::Ray;

pub struct HitRecord {
    pub point: Point3,
    pub normal: Vec3,
    pub t: f32
}

impl HitRecord {
    fn new(point: Point3, normal: Vec3, t: f32) -> Self {
        Self { point, normal, t }
    }
}

pub trait Hittable {
    fn hit(&self, ray: &Ray, t_min: f32, t_max: f32) -> Option<HitRecord>;
}

pub struct Sphere {
    pub center: Vec3,
    pub radius: f32,
}

impl Sphere {
    pub fn new(center: Vec3, radius: f32) -> Self {
        Self { center, radius }
    }
}

impl Hittable for Sphere {
    fn hit(&self, ray: &Ray, t_min: f32, t_max: f32) -> Option<HitRecord> {
        let oc = ray.origin - self.center;
        let a = Vec3::length_squared(ray.direction);
        let b = 2.0 * Vec3::dot(oc, ray.direction);
        let c = Vec3::length_squared(oc) - self.radius*self.radius;
        let discriminant = b*b - 4.0*a*c;

        if discriminant > 0.0 {
            let sqrt = discriminant.sqrt();
            let r1 = (-b - sqrt)/(2.0*a);
            let r2 = (-b + sqrt)/(2.0*a);
            let mut temp = 0.0;

            if r1 > t_min { temp = r1; }
            else if r2 > t_min { temp = r2; }
            else { return None; }
            
            if temp < t_max && temp > t_min {
                let t = temp;
                let p = ray.at(t);
                let normal = (p - self.center) / self.radius;
                
                return Some(HitRecord::new(p, normal, t));
            }
        }

        None
    }
}

impl Hittable for Scene {
    fn hit(&self, ray: &Ray, t_min: f32, t_max: f32) -> Option<HitRecord> {
        let mut closest_so_far = t_max;
        let mut hit_record = None;

        for object in &self.objects {
            if let Some(record) = object.hit(ray, t_min, closest_so_far) {
                closest_so_far = record.t;
                hit_record = Some(record);
            }
        }

        hit_record
    }
}