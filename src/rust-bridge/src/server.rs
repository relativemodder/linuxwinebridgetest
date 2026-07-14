use std::io::{BufRead, BufReader, Write};
use std::os::unix::net::{UnixListener, UnixStream};
use std::sync::{Arc, Mutex};
use std::thread;
use std::fs;

use crate::handler::handle_request;
use crate::protocol::Request;
use crate::x11::XContext;

pub const SOCK_PATH: &str = "/tmp/gdbridge.sock";

fn handle_client(stream: UnixStream, x11: Arc<Mutex<XContext>>) {
    let mut reader = BufReader::new(&stream);
    let mut writer = &stream;
    let mut line = String::new();

    loop {
        line.clear();
        match reader.read_line(&mut line) {
            Ok(0) | Err(_) => break,
            Ok(_) => {
                let resp = match serde_json::from_str::<Request>(&line) {
                    Ok(req) => {
                        let ctx = x11.lock().unwrap();
                        handle_request(req, &ctx)
                    }
                    Err(e) => crate::protocol::Response::err(&format!("parse error: {}", e)),
                };
                let mut out = serde_json::to_string(&resp).unwrap_or_default();
                out.push('\n');
                let _ = writer.write_all(out.as_bytes());
            }
        }
    }
}

pub fn run() {
    let x11 = match XContext::init() {
        Ok(ctx) => Arc::new(Mutex::new(ctx)),
        Err(e) => {
            eprintln!("rust-bridge: failed to init X11 context: {}", e);
            std::process::exit(1);
        }
    };

    let _ = fs::remove_file(SOCK_PATH);
    let listener = UnixListener::bind(SOCK_PATH).expect("failed to bind socket");

    for stream in listener.incoming() {
        match stream {
            Ok(stream) => {
                let x11 = Arc::clone(&x11);
                thread::spawn(move || handle_client(stream, x11));
            }
            Err(_) => break,
        }
    }
}
