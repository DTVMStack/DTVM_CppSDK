// Copyright (C) 2024-2025 Ant Group Co., Ltd. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

use crate::commands::sol_types_utils::transform_solidity_storage_type_to_cpp;
use crate::commands::utils::with_prefix_comma_if_not_empty;
use byteorder::ByteOrder;
use serde_json::Value;
use sha3::Digest;
use std::collections::HashMap;

pub struct SolHppWriter {
    buf: String,
}

impl Default for SolHppWriter {
    fn default() -> Self {
        Self::new()
    }
}

impl SolHppWriter {
    pub fn new() -> Self {
        let prefix = r#"
#pragma once
#include "contractlib/v1/contractlib.hpp"
"#;
        SolHppWriter {
            buf: prefix.to_string(),
        }
    }

    pub fn add_solidity_interface(
        &mut self,
        solidity_json: &str,
        storage_json_str: Option<String>,
    ) {
        let meta_json: Value = serde_json::from_str(solidity_json).unwrap();

        let storage_json: Value = if let Some(storage_json_str) = &storage_json_str {
            serde_json::from_str(storage_json_str).unwrap()
        } else {
            serde_json::from_str("{\"storage\":[]}").unwrap()
        };
        let contract_storages_json = storage_json["storage"].as_array().unwrap();

        // There is no inheritance relationship in the metadata, so we do not add it in the hpp

        let compilation_target = meta_json["settings"]["compilationTarget"]
            .as_object()
            .unwrap();
        let cls_name = compilation_target.values().collect::<Vec<_>>()[0]
            .as_str()
            .unwrap();
        println!("sol cls name: {cls_name}");
        let mut cls_buf: String = format!("\nclass {cls_name} : public dtvm::Contract {{");

        // Generate a proxy subclass for each Solidity interface
        let mut proxy_cls_buf: String = format!(
            r#"
// Generate a proxy subclass for each Solidity interface
class {cls_name}Proxy : public {cls_name} {{
public:
  inline {cls_name}Proxy(const dtvm::Address& addr) {{
      addr_ = addr;
  }}
protected:
   dtvm::CResult constructor(dtvm::CallInfoPtr call, dtvm::Input &input) override {{
      // Since it's just a proxy, there's no need to execute the contract's constructor
      // do nothing
      return dtvm::Revert("unreachable");
   }}
private:
  dtvm::Address addr_;
public:
  // abi proxyes
"#
        );

        // abis and events
        let abis = meta_json["output"]["abi"].as_array().unwrap();

        let mut abi_selector_map: HashMap<String, u32> = HashMap::new();
        let mut event_signature_map: HashMap<String, u32> = HashMap::new();

        for abi in abis {
            let abi_name = abi["name"].as_str().unwrap();
            // println!("abi name: {abi_name}");
            let state_mutability = abi["stateMutability"].as_str().unwrap_or(""); // nonpayable or payable or "" ...
            let abi_type = abi["type"].as_str().unwrap(); // function or event or ...
                                                          // println!("abi_type: {abi_type}");
            let mut inputs: &Vec<Value> = &vec![];
            if abi["inputs"].is_array() {
                inputs = abi["inputs"].as_array().unwrap();
            }
            let mut _outputs: &Vec<Value> = &vec![];
            if abi["outputs"].is_array() {
                _outputs = abi["outputs"].as_array().unwrap();
            }
            let mut args_cpp_buf: String = "".to_string(); // Function signature parameter part
            let mut args_decode_cpp_buf: String = "".to_string(); // Decoding parameters section
            let mut call_abi_args_cpp_buf: String = "".to_string(); // , arg0, arg1, ... part in xxx(msg, arg0, arg1, ...). Each parameter is a variable decoded from the function body.
            let mut all_params_name_list_cpp_buf: String = "".to_string(); // Comma-separated list of function parameters

            let mut abi_method_signature: String = format!("{abi_name}(");

            for (input_index, input) in inputs.iter().enumerate() {
                if input_index > 0 {
                    args_cpp_buf += ", ";
                    call_abi_args_cpp_buf += ", ";
                    all_params_name_list_cpp_buf += ", ";
                    abi_method_signature += ",";
                }

                let internal_type = input["internalType"].as_str().unwrap(); // address/uint256/(t1,t2...)...
                abi_method_signature += internal_type;
                let arg_type = input["type"].as_str().unwrap();
                // TODO: If the ABI parameter is a custom struct, we also need to generate the corresponding C++ type
                // for this custom struct, as well as the encoding and decoding functions.
                let mut arg_name: String = input["name"].as_str().unwrap().to_string();
                if arg_name.is_empty() {
                    arg_name = format!("annoy_arg{input_index}");
                }
                let arg_type_cpp_mapping = HashMap::from([
                    ("address", "Address"),
                    ("uint256", "uint256"),
                    ("uint128", "__uint128_t"),
                    ("uint64", "uint64_t"),
                    ("uint32", "uint32_t"),
                    ("uint8", "uint8_t"),
                    ("int256", "int256"),
                    ("int128", "__int128_t"),
                    ("int64", "int64_t"),
                    ("int32", "int32_t"),
                    ("int8", "int8_t"),
                    ("bool", "bool"),
                    ("string", "std::string"),
                ]);
                let arg_type_in_cpp = &format!("dtvm::{}", arg_type_cpp_mapping[&arg_type]);
                args_cpp_buf += &format!("const {arg_type_in_cpp} &{arg_name}");

                args_decode_cpp_buf += &format!(
                    "{arg_type_in_cpp} arg{input_index} = input.read<{arg_type_in_cpp}>();\n        "
                );
                call_abi_args_cpp_buf += &format!("arg{input_index}");
                all_params_name_list_cpp_buf += &arg_name;
            }

            abi_method_signature += ")";
            // calculate selector(uint32)
            let mut hasher = sha3::Keccak256::new();
            hasher.update(abi_method_signature.as_bytes());
            let sig_bytes = hasher.finalize();
            let selector_bytes: [u8; 4] = sig_bytes[0..4].try_into().unwrap();
            let selector_bytes_cpp_vector_code = &format!(
                "{{ {}, {}, {}, {} }}",
                selector_bytes[0], selector_bytes[1], selector_bytes[2], selector_bytes[3]
            );
            // println!("selector hex: {}", hex::encode(selector_bytes));
            let selector: u32 = byteorder::BigEndian::read_u32(&selector_bytes);
            // println!("method {abi_method_signature} selector {selector}");

            if abi_type == "function" {
                abi_selector_map.insert(abi_name.to_string(), selector);
            } else if abi_type == "event" {
                event_signature_map.insert(abi_name.to_string(), selector);
            }

            let payable_check_code = if state_mutability == "nonpayable" {
                "dtvm::require(dtvm::get_msg_value() == 0, \"not payable method\");"
            } else {
                ""
            };
            let args_cpp_buf_with_prefix_comma = with_prefix_comma_if_not_empty(&args_cpp_buf);
            let call_abi_args_cpp_buf_with_prefix_comma =
                with_prefix_comma_if_not_empty(&call_abi_args_cpp_buf);
            let all_params_name_list_cpp_buf_with_prefix_comma =
                with_prefix_comma_if_not_empty(&all_params_name_list_cpp_buf);
            match abi_type {
                "function" => {
                    cls_buf += &format!(
                        r#"
public:
  virtual dtvm::CResult {abi_name}({args_cpp_buf}) = 0;
  virtual dtvm::CResult {abi_name}(dtvm::CallInfoPtr call_info {args_cpp_buf_with_prefix_comma}) {{
      return {abi_name}({all_params_name_list_cpp_buf});
  }}
protected:
  inline dtvm::CResult interface_{abi_name}(dtvm::CallInfoPtr call_info, dtvm::Input &input) {{
      {payable_check_code}
      {args_decode_cpp_buf}
      return {abi_name}(call_info{call_abi_args_cpp_buf_with_prefix_comma});
  }}
"#
                    );
                    proxy_cls_buf += &format!(
                        r#"
  inline dtvm::CResult {abi_name}(dtvm::CallInfoPtr call_info {args_cpp_buf_with_prefix_comma}) override {{
      std::vector<uint8_t> encoded_input = {selector_bytes_cpp_vector_code}; // {selector}, function selector bytes, {abi_method_signature}
      std::vector<uint8_t> encoded_args = dtvm::abi_encode(std::make_tuple({all_params_name_list_cpp_buf}));
      encoded_input.insert(encoded_input.end(), encoded_args.begin(), encoded_args.end());
      return dtvm::call(addr_, encoded_input, call_info->value, call_info->gas);
  }}
  inline dtvm::CResult {abi_name}({args_cpp_buf}) override {{
      return {abi_name}(dtvm::default_call_info(){all_params_name_list_cpp_buf_with_prefix_comma});
  }}
"#
                    );
                }
                "event" => {
                    // Generate the hash of this event's function signature as topic1
                    let mut event_topic_cpp_code: String = "".to_string();
                    for (i, b) in selector_bytes.iter().enumerate() {
                        if i > 0 {
                            event_topic_cpp_code += ", ";
                        }
                        let b_str = format!("{b}");
                        event_topic_cpp_code += &b_str;
                    }
                    // TODO: Each indexed event field should also be added to topics, not to data
                    cls_buf += &format!(
                        r#"
protected:
  inline void emit{abi_name}({args_cpp_buf}) {{
      const std::vector <uint8_t> topic1 = {{ {event_topic_cpp_code} }}; // {selector}, {abi_method_signature}
      const std::vector <uint8_t> data = dtvm::abi_encode(std::make_tuple({all_params_name_list_cpp_buf}));
      dtvm::hostio::emit_log({{topic1}}, data);
  }}
"#
                    );
                }
                _ => {
                    unreachable!("unknown abi type: {abi_type}");
                }
            }
        }

        // fields
        let mut constructor_cpp_buf: String = "".to_string();

        if !contract_storages_json.is_empty() {
            cls_buf += "protected:"; // fields are set to protected to allow subclasses to access them

            constructor_cpp_buf += &format!(
                r#"
  inline {cls_name}() {{
"#
            );
        }
        for storage_info in contract_storages_json {
            let label = storage_info["label"].as_str().unwrap();
            let slot = storage_info["slot"].as_str().unwrap();
            let slot: i32 = slot.parse().unwrap();
            let offset: i64 = storage_info["offset"].as_i64().unwrap();
            let offset: u32 = offset as u32;
            // storage type eg. t_mapping(t_address,t_uint256), t_string_storage
            let storage_type = storage_info["type"].as_str().unwrap();
            // now support some types
            let mut storage_type_in_cpp: String =
                transform_solidity_storage_type_to_cpp(storage_type);
            if !storage_type.starts_with("t_mapping") && !storage_type.starts_with("t_array") {
                storage_type_in_cpp = format!("dtvm::StorageValue<{}>", storage_type_in_cpp);
            }

            // TODO: If the storage has the same name as an ABI, it indicates a public field.
            // Generate a virtual ABI implementation function for it automatically.

            // Append an underscore to the storage variable name in C++ to avoid conflicts
            // between storage and ABI names.
            cls_buf += &format!(
                r#"
  dtvm::StorageSlot {label}_slot;
  std::shared_ptr<{storage_type_in_cpp}> {label}_;
"#
            );
            constructor_cpp_buf += &format!(
                r#"
      {label}_slot = dtvm::StorageSlot({slot}, {offset});
      {label}_ = std::make_shared<{storage_type_in_cpp}>({label}_slot);
"#
            );
        }

        // If storages are not empty, add a constructor to initialize slot variables
        if !contract_storages_json.is_empty() {
            constructor_cpp_buf += "    }\n\n";
        }
        cls_buf += &constructor_cpp_buf;

        // from address to proxy method signature(implementation is after proxy class)
        cls_buf += &format!(
            r#"
public:
  static std::shared_ptr<{cls_name}> from(const dtvm::Address& addr);
"#
        );

        // dispatch and dispatch_constructor
        // Dispatch code for each ABI method ID
        let mut abi_selectors_switch_cpp_code: String = "".to_string();
        for (abi_name, selector) in abi_selector_map {
            abi_selectors_switch_cpp_code += &format!(
                r#"
          case {selector}: {{ // selector of {abi_name}
              return interface_{abi_name}(call_info, input);
          }}
              break;
"#
            );
        }
        cls_buf += &format!(
            r#"
public:
  dtvm::CResult dispatch(dtvm::CallInfoPtr call_info, dtvm::Input &input_with_selector) {{
      const uint32_t selector = input_with_selector.read_selector();
      // ignore first 4 bytes selector, read the rest as contract abi input
      dtvm::Input input(input_with_selector.data() + 4, input_with_selector.size() - 4);
      switch (selector) {{
          {abi_selectors_switch_cpp_code}
          default: {{
              fallback();
              return dtvm::Ok();
          }}
      }}
  }}

  dtvm::CResult dispatch_constructor(dtvm::CallInfoPtr call_info, dtvm::Input &input) {{
      return constructor(call_info, input);
  }}
"#
        );

        // tail '}'
        cls_buf += "};\n";
        proxy_cls_buf += "};\n";
        // add from address to proxy class implementation
        proxy_cls_buf += &format!(
            r#"
std::shared_ptr<{cls_name}> {cls_name}::from(const dtvm::Address& addr) {{
  return std::make_shared<{cls_name}Proxy>(addr);
}}
"#
        );
        self.buf += &cls_buf;

        self.buf += &proxy_cls_buf;
    }

    pub fn write_to_file(&self, filepath: &str) {
        std::fs::write(filepath, self.buf.as_bytes()).unwrap();
    }
}
