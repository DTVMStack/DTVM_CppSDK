// Copyright (C) 2024-2025 the DTVM authors. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

pub fn with_prefix_comma_if_not_empty(s: &str) -> String {
    if !s.is_empty() {
        format!(", {s}")
    } else {
        s.to_string()
    }
}
