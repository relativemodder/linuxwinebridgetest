use std::ffi::CString;
use x11_dl::xlib::Xlib;
use x11_dl::xtest::Xf86vmode as Xtest;

pub struct XContext {
    xlib: Xlib,
    xtst: Xtest,
    dpy: *mut x11_dl::xlib::Display,
}

unsafe impl Send for XContext {}

impl XContext {
    pub fn init() -> Result<Self, String> {
        let xlib = Xlib::open().map_err(|e| format!("failed to open xlib: {}", e))?;
        let xtst = Xtest::open().map_err(|e| format!("failed to open xtst: {}", e))?;

        let dpy = unsafe { (xlib.XOpenDisplay)(std::ptr::null()) };
        if dpy.is_null() {
            return Err("XOpenDisplay failed".to_string());
        }

        Ok(Self { xlib, xtst, dpy })
    }

    pub fn key_down(&self, keysym_name: &str) -> Result<(), String> {
        let keycode = self.resolve_keycode(keysym_name)?;
        unsafe {
            (self.xtst.XTestFakeKeyEvent)(self.dpy, keycode as u32, 1, 0);
            (self.xlib.XFlush)(self.dpy);
        }
        Ok(())
    }

    pub fn key_up(&self, keysym_name: &str) -> Result<(), String> {
        let keycode = self.resolve_keycode(keysym_name)?;
        unsafe {
            (self.xtst.XTestFakeKeyEvent)(self.dpy, keycode as u32, 0, 0);
            (self.xlib.XFlush)(self.dpy);
        }
        Ok(())
    }

    fn resolve_keycode(&self, keysym_name: &str) -> Result<u8, String> {
        let name = CString::new(keysym_name).map_err(|e| e.to_string())?;
        let keysym = unsafe { (self.xlib.XStringToKeysym)(name.as_ptr()) };
        if keysym == 0 {
            return Err(format!("unknown keysym: {}", keysym_name));
        }
        let keycode = unsafe { (self.xlib.XKeysymToKeycode)(self.dpy, keysym) };
        if keycode == 0 {
            return Err(format!("no keycode for keysym: {}", keysym_name));
        }
        Ok(keycode)
    }
}

impl Drop for XContext {
    fn drop(&mut self) {
        unsafe {
            (self.xlib.XCloseDisplay)(self.dpy);
        }
    }
}
