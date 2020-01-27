use clap::{App, Arg};
use encoding_rs::{GBK, SHIFT_JIS, UTF_16LE, UTF_8};
use std::error::Error;
use std::fs::{self, File};
use std::io::Write;
use thiserror::Error as ThisError;

fn read_u32_checked(v: &[u8]) -> usize {
    u32::from_le_bytes([v[0], v[1], v[2], v[3]]) as usize
}
fn get_str(stm: &[u8], idx: usize) -> &[u8] {
    let len = stm[idx..]
        .iter()
        .position(|x| *x == 0)
        .unwrap_or(stm.len() - idx);
    &stm[idx..idx + len]
}
fn is_need_include(stm: &[u8]) -> bool {
    stm.len() != 0 && stm.iter().any(|c| *c >= 0x80)
}
fn ext_text(stm: &[u8]) -> Result<Vec<(&[u8], usize)>, &'static str> {
    if !stm.starts_with("BurikoCompiledScriptVer1.00\0".as_bytes()) {
        return Err("not a valid script");
    }
    let mut code_end_off = stm[..stm.len() & (!3)]
        .windows(4)
        .rposition(|x| x == &[3, 0, 0, 0])
        .ok_or("seems not having texts")?;
    let hdr_size = read_u32_checked(&stm[0x1c..]) + 0x1c;
    if hdr_size >= code_end_off {
        return Err("not a valid script");
    }

    let mut ret: Vec<(&[u8], usize)> = vec![];
    let mut i = hdr_size;
    while i < code_end_off {
        if read_u32_checked(&stm[i..]) == 3 {
            let off = read_u32_checked(&stm[i + 4..]) + hdr_size;
            if off < stm.len() {
                if off < code_end_off {
                    code_end_off = off;
                }
                let s = get_str(stm, off);
                if is_need_include(s) {
                    ret.push((s, i + 4));
                }
                i += 4;
            } else {
                println!("exceeded, idx:{:x}", off);
            }
        }
        i += 4;
    }
    Ok(ret)
}

fn write_u32_checked(stm: &mut [u8], v: u32) {
    stm[0..4].copy_from_slice(&v.to_le_bytes());
}

fn pack_text(stm: &[u8], text: Vec<Vec<u8>>) -> Result<Box<[u8]>, &'static str> {
    let ext = ext_text(stm)?;
    let empty_cnt = text
        .iter()
        .rev()
        .position(|l| l.len() != 0)
        .unwrap_or(text.len());
    if ext.len() != text.len() - empty_cnt {
        //println!("need:{}, given:{}", ext.len(), text.len());
        return Err("lines not match!");
    }
    let mut out: Vec<_> = stm.iter().cloned().collect();
    let hdr_size = read_u32_checked(&stm[0x1c..]) + 0x1c;
    for (idx, l) in text[..text.len() - empty_cnt].iter().enumerate() {
        let off = out.len();
        out.extend_from_slice(l);
        out.push(0);
        write_u32_checked(&mut out[ext[idx].1..], (off - hdr_size) as u32);
    }
    Ok(out.into_boxed_slice())
}

fn process_ext_file(in_name: &str, out_name: &str) -> Result<(), String> {
    let stm = fs::read(in_name).map_err(|e| e.description().to_string())?;
    let txt = ext_text(&stm[..]).map_err(|e| e.to_string())?;
    let mut out = File::create(out_name).map_err(|e| e.description().to_string())?;
    txt.into_iter().for_each(|(l, _)| {
        out.write(l).unwrap_or_default();
        out.write(&[0x0d, 0x0a]).unwrap_or_default();
    });
    Ok(())
}

struct PackOption {
    txt_enc: TextEnc,
    scr_enc: TextEnc,
}

#[derive(ThisError, Debug)]
enum EncodingError {
    #[error("io error")]
    IoFail(#[from] std::io::Error),

    #[error("decoding error")]
    Decoding,

    #[error("wrong encoding specified")]
    WrongEncoding,
}

enum TextEnc {
    U16,
    Utf8Bom,
    Utf8,
    Gbk,
    Sjis,
    Unknown,
}
impl TextEnc {
    fn parse_from(s: &str) -> Option<Self> {
        match s {
            "u16" => Some(Self::U16),
            "utf8bom" => Some(Self::Utf8Bom),
            "utf8" => Some(Self::Utf8),
            "gbk" => Some(Self::Gbk),
            "sjis" => Some(Self::Sjis),
            "unk" => Some(Self::Unknown),
            _ => None,
        }
    }
}

fn read_text_file_with_encoding(name: &str, enc: &TextEnc) -> Result<String, EncodingError> {
    let stm = fs::read(name)?;
    if stm.len() >= 2 && stm[0] == 0xff && stm[1] == 0xfe {
        match enc {
            TextEnc::Unknown | TextEnc::U16 => {
                let (s, _, err) = UTF_16LE.decode(&stm[2..]);
                if err {
                    Err(EncodingError::Decoding)
                } else {
                    Ok(s.to_string())
                }
            }
            _ => Err(EncodingError::WrongEncoding),
        }
    } else if stm.len() >= 3 && stm[0] == 0xef && stm[1] == 0xbb && stm[2] == 0xbf {
        match enc {
            TextEnc::Unknown | TextEnc::Utf8Bom => {
                let (s, _, err) = UTF_8.decode(&stm[3..]);
                if err {
                    Err(EncodingError::Decoding)
                } else {
                    Ok(s.to_string())
                }
            }
            _ => Err(EncodingError::WrongEncoding),
        }
    } else {
        let enc = match enc {
            TextEnc::Utf8 => UTF_8,
            TextEnc::Gbk => GBK,
            TextEnc::Sjis => SHIFT_JIS,
            _ => return Err(EncodingError::WrongEncoding),
        };
        let (s, _, err) = enc.decode(&stm);
        if err {
            Err(EncodingError::Decoding)
        } else {
            Ok(s.to_string())
        }
    }
}

fn process_pack_file(
    in_name: &str,
    text_name: &str,
    out_name: &str,
    opt: &PackOption,
) -> Result<(), String> {
    let stm = fs::read(in_name).map_err(|e| e.description().to_string())?;
    let txt =
        read_text_file_with_encoding(text_name, &opt.txt_enc).map_err(|e| format!("{}", e))?;
    let enc_line_func: fn(&str) -> Vec<u8> = match opt.scr_enc {
        TextEnc::Sjis => |l: &str| SHIFT_JIS.encode(l).0.into_owned(),
        TextEnc::Gbk => |l: &str| GBK.encode(l).0.into_owned(),
        _ => return Err("encoding error".to_string()),
    };
    let x: Vec<Vec<u8>> = txt
        .replace("\r\n", "\n")
        .replace("\r", "\n")
        .split("\n")
        .map(enc_line_func)
        .collect();

    let v = pack_text(&stm[..], x)?;
    fs::write(out_name, v).map_err(|e| e.description().to_string())?;
    Ok(())
}

fn main() {
    let args = App::new("BGI extractor")
        .version("1.0")
        .author("regomne <fallingsunz@gmail.com>")
        .arg(Arg::with_name("pack").short("p").conflicts_with("extract"))
        .arg(Arg::with_name("extract").short("e").conflicts_with("pack"))
        .arg(
            Arg::with_name("text")
                .short("t")
                .value_name("FILE")
                .required_if("pack", ""),
        )
        .arg(
            Arg::with_name("input")
                .short("i")
                .value_name("FILE")
                .required(true),
        )
        .arg(
            Arg::with_name("output")
                .short("o")
                .value_name("FILE")
                .required(true),
        )
        .arg(
            Arg::with_name("text_enc")
                .long("text-enc")
                .value_name("ENCODING")
                .default_value("unk")
                .possible_values(&["unk", "gbk", "sjis", "utf8"]),
        )
        .arg(
            Arg::with_name("scr_enc")
                .long("scr-enc")
                .value_name("ENCODING")
                .default_value("gbk")
                .possible_values(&["gbk", "sjis", "utf8"]),
        )
        .get_matches();
    let input = args.value_of("input").unwrap();
    let output = args.value_of("output").unwrap();
    if args.is_present("extract") {
        match process_ext_file(input, output) {
            Err(e) => println!("failed:{}", e),
            _ => {}
        }
    } else {
        let text_name = args.value_of("text").unwrap();
        let txt_enc = TextEnc::parse_from(args.value_of("text_enc").unwrap()).unwrap();
        let scr_enc = TextEnc::parse_from(args.value_of("scr_enc").unwrap()).unwrap();
        match process_pack_file(input, text_name, output, &PackOption { txt_enc, scr_enc }) {
            Err(e) => println!("failed:{}", e),
            _ => {}
        }
    }
}
