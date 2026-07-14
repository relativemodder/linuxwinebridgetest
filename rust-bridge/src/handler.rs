use crate::protocol::{Request, Response};
use crate::x11;

pub fn handle_request(req: Request) -> Response {
    match req.cmd.as_str() {
        "ping" => Response::ok(serde_json::json!("pong")),

        "zenity" => {
            let text = req.args
                .get("text")
                .and_then(|v| v.as_str())
                .unwrap_or("hello from rust-bridge")
                .to_string();

            match std::process::Command::new("zenity")
                .args(["--info", "--text", &text])
                .status()
            {
                Ok(s) if s.success() => Response::ok(serde_json::json!(null)),
                Ok(s) => Response::err(&format!("zenity exited with {}", s)),
                Err(e) => Response::err(&format!("failed to run zenity: {}", e)),
            }
        }

        "key_press" => {
            let keysym = match req.args.get("keysym").and_then(|v| v.as_str()) {
                Some(k) => k.to_string(),
                None => return Response::err("missing args.keysym"),
            };

            match x11::press_key(&keysym) {
                Ok(()) => Response::ok(serde_json::json!(null)),
                Err(e) => Response::err(&e),
            }
        }

        other => Response::err(&format!("unknown command: {}", other)),
    }
}
