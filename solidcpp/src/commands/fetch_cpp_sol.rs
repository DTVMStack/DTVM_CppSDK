// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

/// Extract Solidity code strings from C++ contract source code
/// that start with SOLIDITY(contractName, R"""( and end with )""")
use regex::Regex;

#[derive(Debug)]
pub struct SolidityFromCpp {
    pub contract_name: String,
    pub solidity_code: String,
}

pub fn extract_solidity_from_cpp(s: &str) -> Vec<SolidityFromCpp> {
    let re_str = r#"SOLIDITY\(([\w_]+),\s*R"""\(((.|\n)*)\)"""\)"#;
    let re = Regex::new(re_str).unwrap();
    re.captures_iter(s)
        .map(|cap| SolidityFromCpp {
            contract_name: cap.get(1).map_or("", |m| m.as_str()).to_string(),
            solidity_code: cap.get(2).map_or("", |m| m.as_str()).to_string(),
        })
        .collect()
}

#[test]
fn test_extract_solidity_from_cpp() {
    let s = r#"
        SOLIDITY(ContractName, R"""(
            pragma solidity ^0.8.0;

            contract ContractName {
                // ...
            }
          )""")

        void main() {
          printf("Hello, world!");
        }
    "#;
    let sols = extract_solidity_from_cpp(s);
    println!("extracted: {sols:?}");
    assert_eq!(sols.len(), 1);
    assert_eq!(sols[0].contract_name, "ContractName");
    assert!(sols[0].solidity_code.contains("contract ContractName {"))
}
