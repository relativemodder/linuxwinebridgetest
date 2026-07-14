use serde::{Deserialize, Serialize};

#[derive(Deserialize)]
pub struct Request {
    pub cmd: String,
    #[serde(default)]
    pub args: serde_json::Value,
}

#[derive(Serialize)]
pub struct Response {
    pub ok: bool,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub result: Option<serde_json::Value>,
    #[serde(skip_serializing_if = "Option::is_none")]
    pub error: Option<String>,
}

impl Response {
    pub fn ok(result: serde_json::Value) -> Self {
        Self { ok: true, result: Some(result), error: None }
    }

    pub fn err(msg: &str) -> Self {
        Self { ok: false, result: None, error: Some(msg.to_string()) }
    }
}
