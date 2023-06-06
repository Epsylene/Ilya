use std::ops;

pub struct Color {
    pub r: f32,
    pub g: f32,
    pub b: f32,
    pub a: f32,
}

impl Color {
    pub const WHITE: Self = Self::rgb(1.0, 1.0, 1.0);
    pub const RED: Self = Self::rgb(1.0, 0.0, 0.0);
    pub const GREEN: Self = Self::rgb(0.0, 1.0, 0.0);
    pub const BLUE: Self = Self::rgb(0.0, 0.0, 1.0);
    pub const BLACK: Self = Self::rgb(0.0, 0.0, 0.0);

    pub const fn rgb(r: f32, g: f32, b: f32) -> Self {
        Self { r, g, b, a: 1.0 }
    }

    pub const fn rgba(r: f32, g: f32, b: f32, a: f32) -> Self {
        Self { r, g, b, a }
    }

    pub fn to_255(&self) -> [u8; 4] {
        let r = f32::clamp(self.r * 255.99, 0.0, 255.99) as u8;
        let g = f32::clamp(self.g * 255.99, 0.0, 255.99) as u8;
        let b = f32::clamp(self.b * 255.99, 0.0, 255.99) as u8;
        let a = f32::clamp(self.a * 255.99, 0.0, 255.99) as u8;

        [r, g, b, a]
    }
}

impl ops::Mul<f32> for Color {
    type Output = Self;

    fn mul(self, rhs: f32) -> Self {
        Self::rgb(
            self.r * rhs,
            self.g * rhs,
            self.b * rhs,
        )
    }
}

impl ops::Mul<Color> for f32 {
    type Output = Color;

    fn mul(self, rhs: Color) -> Color {
        Color::rgb(
            self * rhs.r,
            self * rhs.g,
            self * rhs.b,
        )
    }
}

impl ops::Add<Color> for Color {
    type Output = Self;

    fn add(self, rhs: Color) -> Self {
        Self::rgb(
            self.r + rhs.r,
            self.g + rhs.g,
            self.b + rhs.b,
        )
    }
}