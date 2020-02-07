use std::env;
use std::error::Error;
use std::fs::{self, File};
use std::io::{self, Cursor, ErrorKind, Write};

fn find_same_alpha(pixel: &[u8]) -> (usize, u8) {
    let a = pixel[3];
    (
        pixel[4..]
            .chunks(4)
            .position(|p| p[3] != a)
            .unwrap_or(pixel.len() / 4 - 1),
        a,
    )
}

fn compress_rgba(width: u32, height: u32, dib: &[u8]) -> Vec<u8> {
    let mut buf = Cursor::new(Vec::with_capacity(dib.len()));
    buf.write("GCGK".as_bytes()).unwrap();
    buf.write(&(width as u16).to_le_bytes()).unwrap();
    buf.write(&(height as u16).to_le_bytes()).unwrap();

    let buf_begin = height * 4 + 12;
    buf.set_position(buf_begin as u64);
    let mut off_index = 12;
    let line_size = (width * 4) as usize;
    for i in 0..height as usize {
        let cur_off = buf.position();
        buf.set_position(off_index);
        buf.write(&(cur_off as u32 - buf_begin).to_le_bytes())
            .unwrap();
        buf.set_position(cur_off);
        off_index += 4;

        let line_start = (line_size * i) as usize;
        let mut cur = 0;
        while cur < line_size {
            let searching_end = if line_size - cur >= 0x400 {
                cur + 0x400
            } else {
                line_size
            };
            let (cnt, alpha) = find_same_alpha(&dib[line_start + cur..line_start + searching_end]);
            let cnt_byte = match cnt {
                0xff => 0,
                0..=0xfe => (cnt + 1) as u8,
                _ => panic!(format!("internal error, cnt:{}", cnt)),
            };
            buf.write(&[alpha]).unwrap();
            buf.write(&[cnt_byte]).unwrap();
            if alpha != 0 {
                for j in 0..cnt + 1 {
                    buf.write(&dib[line_start + cur + j * 4..line_start + cur + j * 4 + 3])
                        .unwrap();
                }
            }
            cur += (cnt + 1) * 4;
        }
    }
    let buf_size = buf.position() as u32 - buf_begin;
    buf.set_position(8);
    buf.write(&buf_size.to_le_bytes()).unwrap();
    buf.into_inner()
}

fn png_to_kg(pngname: &String, kgname: &String) -> Result<(), Box<Error>> {
    let mut decoder = png::Decoder::new(File::open(pngname)?);
    decoder.set_transformations(png::Transformations::IDENTITY);
    let (info, mut reader) = decoder.read_info()?;
    if info.bit_depth != png::BitDepth::Eight {
        return Err(Box::new(io::Error::new(
            ErrorKind::Other,
            "png bit depth error",
        )));
    }
    if info.color_type != png::ColorType::RGBA {
        return Err(Box::new(io::Error::new(
            ErrorKind::Other,
            "png color type error",
        )));
    }
    let mut dib = vec![0; info.buffer_size()];
    reader.next_frame(&mut dib)?;
    let kg = compress_rgba(info.width, info.height, &dib);
    fs::write(kgname, kg)?;
    Ok(())
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        println!("usage: {} <png> <kg>", args[0]);
        return;
    }
    match png_to_kg(&args[1], &args[2]) {
        Err(e) => println!("error:{}", e),
        _ => println!("complete."),
    }
}
