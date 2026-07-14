use std::io::{BufRead, BufReader, Write};
use std::os::unix::net::{UnixListener, UnixStream};
use std::thread;
use std::fs;

use crate::handler::handle_request;
use crate::protocol::Request;

pub const SOCK_PATH: &str = "/tmp/gdbridge.sock";

fn handle_client(stream: UnixStream) {
    let mut reader = BufReader::new(&stream);
    let mut writer = &stream;
    let mut line = String::new();

    loop {
        line.clear();
        match reader.read_line(&mut line) {
            Ok(0) | Err(_) => break,
            Ok(_) => {
                let resp = match serde_json::from_str::<Request>(&line) {
                    Ok(req) => handle_request(req),
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
    let _ = fs::remove_file(SOCK_PATH);
    let listener = UnixListener::bind(SOCK_PATH).expect("failed to bind socket");

    for stream in listener.incoming() {
        match stream {
            Ok(stream) => { thread::spawn(|| handle_client(stream)); }
            Err(_) => break,
        }
    }
}
