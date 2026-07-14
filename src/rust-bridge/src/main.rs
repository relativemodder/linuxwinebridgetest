mod handler;
mod protocol;
mod server;
mod x11;

use std::fs;
use std::thread;
use std::time::Duration;

fn watch_parent(ppid: u32) {
    thread::spawn(move || loop {
        thread::sleep(Duration::from_secs(2));
        if fs::metadata(format!("/proc/{}", ppid)).is_err() {
            std::process::exit(0);
        }
    });
}

fn main() {
    if let Some(ppid) = std::env::args().nth(1).and_then(|s| s.parse::<u32>().ok()) {
        watch_parent(ppid);
    }
    server::run();
}
