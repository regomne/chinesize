pub use ilhook::x86::{HookPoint, HookType, Registers};
use ilhook::{x86::CallbackOption, x86::HookFlags, x86::Hooker, HookError};
use std::iter::once;
use thiserror::Error;
use winapi::um::errhandlingapi::GetLastError;
use winapi::um::libloaderapi::{GetModuleHandleW, GetProcAddress};

#[derive(Error, Debug)]
pub enum HookErr {
    #[error("internal lib error")]
    LibErr(#[from] HookError),

    #[error("load library failed: {0}")]
    LoadLibErr(u32),
    //#[error("other error")]
    //OtherErr,
}

enum HookPosition<'a> {
    Off(usize),
    Name(&'a str),
}

pub struct HookStruct<'a> {
    mod_name: &'a str,
    pos: HookPosition<'a>,
    hook: HookType,
}

impl<'a> HookStruct<'a> {
    pub fn new_off(mod_name: &'a str, off: usize, hook: HookType) -> Self {
        Self {
            mod_name,
            pos: HookPosition::Off(off),
            hook,
        }
    }
    pub fn new_func_name(mod_name: &'a str, func_name: &'a str, hook: HookType) -> Self {
        Self {
            mod_name,
            pos: HookPosition::Name(func_name),
            hook,
        }
    }
    pub fn hook(self) -> Result<HookPoint, HookErr> {
        let mod_name = self
            .mod_name
            .encode_utf16()
            .chain(once(0))
            .collect::<Vec<_>>();
        let lib = unsafe { GetModuleHandleW(mod_name.as_ptr()) };
        if lib == std::ptr::null_mut() {
            return Err(HookErr::LoadLibErr(unsafe { GetLastError() }));
        }
        let addr = match self.pos {
            HookPosition::Off(off) => lib as usize + off,
            HookPosition::Name(func_name) => {
                let name: Vec<_> = func_name.bytes().chain(once(0)).collect();
                let addr = unsafe { GetProcAddress(lib, name.as_ptr() as *const i8) };
                if addr == std::ptr::null_mut() {
                    return Err(HookErr::LoadLibErr(0));
                }
                addr as usize
            }
        };
        let h = Hooker::new(addr, self.hook, CallbackOption::None, HookFlags::empty());
        unsafe { h.hook() }.map_err(|e| HookErr::LibErr(e))
    }
}

pub fn hook_functions(hooks: Vec<HookStruct>) -> Result<Vec<HookPoint>, HookErr> {
    hooks.into_iter().map(|h| h.hook()).collect()
}
