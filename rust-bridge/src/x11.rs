use std::ffi::CString;
use x11_dl::xlib::Xlib;
use x11_dl::xtest::Xf86vmode as Xtest;

pub fn press_key(keysym_name: &str) -> Result<(), String> {
    let xlib = Xlib::open().map_err(|e| format!("failed to open xlib: {}", e))?;
    let xtst = Xtest::open().map_err(|e| format!("failed to open xtst: {}", e))?;

    unsafe {
        let dpy = (xlib.XOpenDisplay)(std::ptr::null());
        if dpy.is_null() {
            return Err("XOpenDisplay failed".to_string());
        }

        let name = CString::new(keysym_name).map_err(|e| e.to_string())?;
        let keysym = (xlib.XStringToKeysym)(name.as_ptr());
        if keysym == 0 {
            (xlib.XCloseDisplay)(dpy);
            return Err(format!("unknown keysym: {}", keysym_name));
        }

        let keycode = (xlib.XKeysymToKeycode)(dpy, keysym);
        if keycode == 0 {
            (xlib.XCloseDisplay)(dpy);
            return Err(format!("no keycode for keysym: {}", keysym_name));
        }

        (xtst.XTestFakeKeyEvent)(dpy, keycode as u32, 1, 0); // press
        (xtst.XTestFakeKeyEvent)(dpy, keycode as u32, 0, 0); // release
        (xlib.XFlush)(dpy);
        (xlib.XCloseDisplay)(dpy);
    }

    Ok(())
}
