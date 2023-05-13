use image::{RgbImage, ImageBuffer, Rgb};

fn main() {
    const IMG_WIDTH: u32 = 256;
    const IMG_HEIGHT: u32 = 256;

    let mut buffer: RgbImage = ImageBuffer::new(IMG_WIDTH, IMG_HEIGHT);

    for (x, y, pixel) in buffer.enumerate_pixels_mut() {
        let r = x as f64 / (IMG_WIDTH-1) as f64;
        let g = y as f64 / (IMG_HEIGHT-1) as f64;
        let b = 0.25;

        let ir = (255.999 * r) as u8;
        let ig = (255.999 * g) as u8;
        let ib = (255.999 * b) as u8;

        *pixel = Rgb([ir, ig, ib]);
    }

    match buffer.save("image.png") 
    {
        Err(e) => eprintln!("Error writing file: {e}"),
        Ok(()) => println!("Done.")
    };
}
