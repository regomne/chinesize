#[cfg(not(target_os = "windows"))]
compile_error!("this only works for windows");

mod hook_helper;

use encoding_rs::{GBK, SHIFT_JIS};
use hook_helper::*;
//use std::os::windows::ffi::OsStringExt;
use std::iter;
use std::slice;
use winapi::shared::d3d9;
use winapi::shared::minwindef::{DWORD, HINSTANCE, LPVOID};
use winapi::um::consoleapi::AllocConsole;
use winapi::um::wingdi::LOGFONTA;
use winapi::um::winnt::{DLL_PROCESS_ATTACH, DLL_PROCESS_DETACH};

unsafe fn wcslen(s: *const u16) -> usize {
    let mut p = s;
    while *p != 0 {
        p = p.offset(1)
    }
    (p as usize - s as usize) / 2
}
unsafe fn from_wide_str_ptr(p: *const u16) -> String {
    let s = slice::from_raw_parts(p, wcslen(p));
    String::from_utf16(s).unwrap_or(String::from("<error converting>"))
}
unsafe fn strlen(s: *const u8) -> usize {
    let mut p = s;
    while *p != 0 {
        p = p.offset(1)
    }
    p as usize - s as usize
}
unsafe fn from_c_str_ptr(p: *const u8) -> String {
    let s = slice::from_raw_parts(p, strlen(p));
    GBK.decode(s).0.into_owned()
}

unsafe extern "cdecl" fn on_create_file_w(regs: *mut Registers, _: usize) {
    let str_ptr = (*regs).get_arg(1) as usize;
    let s = from_wide_str_ptr(str_ptr as *const u16);
    println!("{}", s);
}

unsafe extern "cdecl" fn on_create_file_a(regs: *mut Registers, _: usize) {
    let str_ptr = (*regs).get_arg(1) as usize;
    let s = from_c_str_ptr(str_ptr as *const u8);
    println!("{}", s);
}

unsafe extern "cdecl" fn on_cwe(regs: *mut Registers, _: usize) {
    let str_ptr = (*regs).get_arg(2) as usize;
    let s = if str_ptr >= 0x10000 {
        from_wide_str_ptr(str_ptr as *const u16)
    } else {
        str_ptr.to_string()
    };
    println!("className: {}", s);
    let str_ptr = (*regs).get_arg(3) as usize;
    if str_ptr != 0 {
        let s = from_wide_str_ptr(str_ptr as *const u16);
        println!("windowName: {}", s);
    }
}
unsafe extern "cdecl" fn on_cfi(regs: *mut Registers, _: usize) {
    let font = (*regs).get_arg(1);
    let font = font as usize as *mut LOGFONTA;
    (*font).lfCharSet = 0x86;
    let ptr = (*font).lfFaceName.as_mut_ptr();
    ptr.copy_from("SimHei\0".as_ptr() as *const i8, 7);
}
unsafe extern "cdecl" fn on_setwindowtext(regs: *mut Registers, _: usize) {
    let s = "『神明的尾巴_十二支神们的报恩』中文版";
    let s: Vec<u16> = s.encode_utf16().chain(iter::once(0)).collect();
    let s = s.into_boxed_slice();
    let text_ptr = ((*regs).esp as usize + 8) as usize as *mut usize;
    *text_ptr = s.as_ptr() as usize;
    std::mem::forget(s);
}

#[no_mangle]
extern "stdcall" fn DllMain(_: HINSTANCE, reason: DWORD, _: LPVOID) -> i32 {
    match reason {
        DLL_PROCESS_ATTACH => {
            //unsafe { AllocConsole() };
            let h1 = vec![
                /*HookStruct::new_func_name(
                    "kernel32.dll",
                    "CreateFileW",
                    HookType::JmpBack(on_create_file_w),
                ),*/
                HookStruct::new_func_name(
                    "gdi32.dll",
                    "CreateFontIndirectA",
                    HookType::JmpBack(on_cfi),
                ),
                HookStruct::new_func_name(
                    "user32.dll",
                    "SetWindowTextW",
                    HookType::JmpBack(on_setwindowtext),
                ),
            ];
            match hook_functions(h1) {
                Err(err) => println!("hook error: {}", err),
                Ok(p) => std::mem::forget(p),
            };
            1
        }
        DLL_PROCESS_DETACH => 1,
        _ => 1,
    }
}

#[no_mangle]
pub unsafe extern "system" fn D3dCreate9(v: u32) -> *mut d3d9::IDirect3D9 {
    d3d9::Direct3DCreate9(v)
}
