// Copyright (C) 2024-2025 the DTVM authors. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

use std::collections::HashMap;

fn parse_type(solidity_type: &str) -> String {
    let storage_base_type_in_cpp_mapping: HashMap<String, String> = [
        ("t_string_storage".to_string(), "std::string".to_string()),
        ("t_bytes32".to_string(), "dtvm::bytes32".to_string()),
        ("t_bytes_storage".to_string(), "dtvm::Bytes".to_string()),
        ("t_uint128".to_string(), "__uint128_t".to_string()),
        ("t_int128".to_string(), "__int128_t".to_string()),
        ("t_uint256".to_string(), "dtvm::uint256".to_string()),
        ("t_int256".to_string(), "dtvm::int256".to_string()),
        ("t_uint64".to_string(), "uint64_t".to_string()),
        ("t_int64".to_string(), "int64_t".to_string()),
        ("t_uint32".to_string(), "uint32_t".to_string()),
        ("t_int32".to_string(), "int32_t".to_string()),
        ("t_uint16".to_string(), "uint16_t".to_string()),
        ("t_int16".to_string(), "int16_t".to_string()),
        ("t_uint8".to_string(), "uint8_t".to_string()),
        ("t_int8".to_string(), "int8_t".to_string()),
        ("t_bool".to_string(), "bool".to_string()),
        ("t_address_storage".to_string(), "dtvm::Address".to_string()),
        ("t_address".to_string(), "dtvm::Address".to_string()),
    ]
    .into_iter()
    .collect();
    if let Some(cpp_type) = storage_base_type_in_cpp_mapping.get(solidity_type) {
        return cpp_type.to_string();
    }
    if let Some((key_type, value_type)) = parse_mapping_type(solidity_type) {
        return format!(
            "dtvm::StorageMap<{}, {}>",
            parse_type(key_type),
            parse_type(value_type)
        );
    }
    if let Some(element_type) = parse_array_type(solidity_type) {
        return format!("dtvm::StorageArray<{}>", parse_type(element_type));
    }
    panic!("unknown solidity type: {}", solidity_type);
}

fn parse_mapping_type(solidity_type: &str) -> Option<(&str, &str)> {
    if !solidity_type.starts_with("t_mapping(") || !solidity_type.ends_with(")") {
        return None;
    }
    // key type will have no comma, so we can split by the first comma
    let first_comma_pos = solidity_type.find(',').unwrap();
    Some((
        &solidity_type[("t_mapping(".len())..first_comma_pos],
        &solidity_type[first_comma_pos + 1..(solidity_type.len() - 1)],
    ))
}

fn parse_array_type(solidity_type: &str) -> Option<&str> {
    if !solidity_type.starts_with("t_array(") || !solidity_type.ends_with(")dyn_storage") {
        return None;
    }
    Some(&solidity_type[("t_array(".len())..solidity_type.len() - ")dyn_storage".len()])
}

pub fn transform_solidity_storage_type_to_cpp(solidity_type: &str) -> String {
    parse_type(solidity_type)
}

#[test]
fn test_nested_mapping_transform() {
    let input = "t_mapping(t_address,t_mapping(t_address,t_uint256))";
    let output = transform_solidity_storage_type_to_cpp(input);
    assert_eq!(
        output,
        "dtvm::StorageMap<dtvm::Address, dtvm::StorageMap<dtvm::Address, dtvm::uint256>>"
    );
}

#[test]
fn test_simple_type_transform() {
    {
        let solidity_type = "t_address";
        let output = transform_solidity_storage_type_to_cpp(solidity_type);
        assert_eq!(output, "dtvm::Address");
    }
    {
        let solidity_type = "t_uint256";
        let output = transform_solidity_storage_type_to_cpp(solidity_type);
        assert_eq!(output, "dtvm::uint256");
    }
    {
        let solidity_type = "t_string_storage";
        let output = transform_solidity_storage_type_to_cpp(solidity_type);
        assert_eq!(output, "std::string");
    }
    {
        let solidity_type = "t_mapping(t_address,t_uint256)";
        let output = transform_solidity_storage_type_to_cpp(solidity_type);
        assert_eq!(output, "dtvm::StorageMap<dtvm::Address, dtvm::uint256>");
    }
    {
        let solidity_type = "t_array(t_uint32)dyn_storage";
        let output = transform_solidity_storage_type_to_cpp(solidity_type);
        assert_eq!(output, "dtvm::StorageArray<uint32_t>");
    }
}
