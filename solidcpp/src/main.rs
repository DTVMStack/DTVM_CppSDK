// Copyright (C) 2024-2025 the DTVM authors. All Rights Reserved.
// SPDX-License-Identifier: Apache-2.0

use std::collections::HashSet;
use std::io::Read;

use clap::{Parser, Subcommand};
use solidcpp::commands::fetch_cpp_sol::extract_solidity_from_cpp;
use solidcpp::commands::generate_hpp::SolHppWriter;

#[derive(Parser, Debug)]
#[command(version, about, long_about = None)]
struct Args {
    #[command(subcommand)]
    command: Option<Commands>,
}

#[derive(Subcommand, Debug)]
enum Commands {
    /// generate .hpp from solidity meta jsons
    GenerateHpp {
        // eg. --input a_meta.json --input b_storage.json
        /// input of solidity .json filepaths
        #[arg(short, long)]
        input: Vec<String>,

        /// output .hpp filepath
        #[arg(short, long)]
        output: String,
    },
    /// fetch solidity code from cpp file by SOLIDITY macro in .cpp files
    FetchCppSol {
        // eg. --input a.cpp --input b.cpp
        /// multiple input files of cpp contract source files which maybe contains SOLIDITY macro
        #[arg(short, long)]
        input: Vec<String>,

        /// output dir to store solidity code fetched
        #[arg(short, long)]
        output: String,
    },
    /// chain tools to build from origin cpp to wasm
    Build {
        // eg. --input a.cpp --input b.cpp
        /// multiple input files of cpp contract source files which maybe contains SOLIDITY macro
        #[arg(short, long)]
        input: Vec<String>,
        /// tmp generated dir to store generated files. maybe "generated"
        #[arg(short, long)]
        generated_dir: String,
        /// dir path to the cpp contract sdk
        #[arg(short, long)]
        contractlib_dir: String,
        /// output wasm filepath
        #[arg(short, long)]
        output: String,
    },
}

fn read_input_path_with_glob(input: &str) -> Vec<String> {
    let base_dir = ".";
    let walker = globwalk::GlobWalkerBuilder::from_patterns(base_dir, &[input])
        .max_depth(4)
        .follow_links(true)
        .build()
        .unwrap()
        .filter_map(Result::ok);

    let mut result: Vec<String> = vec![];
    for fp in walker {
        let fp = fp.path();
        result.push(fp.to_str().unwrap().to_string());
    }
    result
}

// merge input path(maybe have same filepath)
fn merge_all_filepaths(input_filepaths: &[String]) -> Vec<String> {
    let mut result: Vec<String> = vec![];
    let mut result_set: HashSet<String> = HashSet::new();
    for input_filepath in input_filepaths {
        if !result_set.contains(input_filepath) {
            result.push(input_filepath.to_string());
            result_set.insert(input_filepath.to_string());
        }
    }
    result
}

fn read_multiple_input_path_with_glob_and_merge(inputs: &[String]) -> Vec<String> {
    let mut result: Vec<String> = vec![];
    for input in inputs {
        result.append(&mut read_input_path_with_glob(input));
    }
    merge_all_filepaths(&result)
}

fn process_generate_hpp_command(input_filepaths: &[String], output_filepath: &str) {
    println!("output_filepath: {}", output_filepath);
    println!("input_filepaths: {:?}", input_filepaths);

    let input_filepaths = &read_multiple_input_path_with_glob_and_merge(input_filepaths);

    println!("input solidity meta json files: {input_filepaths:?}");

    let input_meta_filepaths = input_filepaths
        .iter()
        .filter(|filepath| filepath.ends_with("_meta.json"))
        .collect::<Vec<&String>>();
    let _input_storage_filepaths = input_filepaths
        .iter()
        .filter(|filepath| filepath.ends_with("_storage.json"))
        .collect::<Vec<&String>>();

    let mut writer = SolHppWriter::new();

    for intput_filepath in &input_meta_filepaths {
        match std::fs::read_to_string(intput_filepath) {
            Ok(content) => {
                if content.is_empty() || !content.contains("{") {
                    println!("invalid input file json of solidity meta: {intput_filepath}");
                    continue;
                } else {
                    // find storage json filepath by meta json filepath
                    let storage_filepath = intput_filepath.replace("_meta.json", "_storage.json");
                    let storage_json_str =
                        if let Ok(content_storage) = std::fs::read_to_string(storage_filepath) {
                            Some(content_storage)
                        } else {
                            None
                        };
                    writer.add_solidity_interface(&content, storage_json_str);
                }
            }
            Err(_e) => {}
        };
    }

    writer.write_to_file(output_filepath);
    println!("writen to {output_filepath}");
}

#[derive(Clone)]
struct SolCodeFile {
    contract_name: String,
    solidity_filepath: String,
}

/// output generated solidity file paths
fn process_fetch_cpp_sol_command(inputs: &[String], output_dir: &str) -> Vec<SolCodeFile> {
    if !std::fs::exists(output_dir).unwrap() {
        std::fs::create_dir(output_dir).unwrap();
    }

    let inputs = &read_multiple_input_path_with_glob_and_merge(inputs);
    let mut result_solidity_filepaths: Vec<SolCodeFile> = vec![];
    for input_filepath in inputs {
        let input_cpp_code = std::fs::read_to_string(input_filepath).unwrap();
        let extracted_solidity_codes = extract_solidity_from_cpp(&input_cpp_code);
        for item in &extracted_solidity_codes {
            let output_filepath = &format!("{output_dir}/{}.sol", item.contract_name);
            std::fs::write(output_filepath, &item.solidity_code).unwrap();
            println!("writen solidity code to {}", &output_filepath);
            result_solidity_filepaths.push(SolCodeFile {
                contract_name: item.contract_name.to_string(),
                solidity_filepath: output_filepath.to_string(),
            });
        }
    }
    result_solidity_filepaths
}

fn main() {
    // parse command line options to get 1 output filepath and multiple input filepaths
    let args = Args::parse();
    match &args.command {
        Some(Commands::GenerateHpp { input, output }) => {
            let output_filepath = output;
            let input_filepaths = input;
            process_generate_hpp_command(input_filepaths, output_filepath);
        }
        Some(Commands::FetchCppSol { input, output }) => {
            process_fetch_cpp_sol_command(input, output);
        }
        Some(Commands::Build {
            input,
            contractlib_dir,
            generated_dir,
            output,
        }) => {
            // fetch solidity from cpp

            // for generate-hpp sub-command, glob path needs to be relative path without './' prefix, like 'generated' instead of './generated'
            let generated_dir = if generated_dir.starts_with("./") && generated_dir.len() > 2 {
                generated_dir[2..].to_string()
            } else {
                generated_dir.to_string()
            };
            let generated_dir = &generated_dir;

            let solidity_file_infos = &process_fetch_cpp_sol_command(input, generated_dir);
            let solidity_filepaths = solidity_file_infos
                .iter()
                .map(|item| item.solidity_filepath.to_string())
                .collect::<Vec<String>>();
            println!("generated solidity filepaths: {:?}", solidity_filepaths);
            if !solidity_file_infos.is_empty() {
                let first_contract_name = &solidity_file_infos[0].contract_name;
                // solidity to meta json
                let solc_path = "solc"; // solc must be in $PATH env
                let mut sub_proc = std::process::Command::new(solc_path)
                    .arg("--metadata")
                    .arg("--storage-layout")
                    .arg("--overwrite")
                    .arg("--output-dir")
                    .arg(generated_dir)
                    .arg("--")
                    .args(solidity_filepaths.clone())
                    .stdout(std::process::Stdio::piped())
                    .stderr(std::process::Stdio::piped())
                    .spawn()
                    .expect("find solc failed");
                let mut stdout = sub_proc.stdout.take().unwrap();
                let mut stderr = sub_proc.stderr.take().unwrap();
                let mut stdout_buf = Vec::new();
                let _ = stdout.read_to_end(&mut stdout_buf);
                let mut stderr_buf = Vec::new();
                let _ = stderr.read_to_end(&mut stderr_buf);
                println!("{}", String::from_utf8_lossy(&stdout_buf));
                println!("{}", String::from_utf8_lossy(&stderr_buf));
                let exit_status = sub_proc
                    .wait()
                    .expect("Failed to wait for child process solc compile sol to meta json");
                if exit_status.success() {
                    println!("solidity metadata json built successfully in {generated_dir}");
                } else {
                    panic!("solc compile solc to metadata failed");
                }

                // solidity meta json to hpp
                process_generate_hpp_command(
                    &[
                        format!("{generated_dir}/*_meta.json"),
                        format!("{generated_dir}/*_storage.json"),
                    ],
                    &format!("{generated_dir}/{first_contract_name}_decl.hpp"),
                );
            } else {
                println!("not SOLIDITY macro found in cpp input files");
            }

            // cpp and hpp to wasm
            // em++ -std=c++17 -o my_token.wasm -O3 my_token.cpp -I ../.. -s 'EXPORTED_FUNCTIONS=["_call","_deploy"]' --no-entry -Wl,--allow-undefined -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
            // -s WASM=1 -s STANDALONE_WASM=0 -s PURE_WASI=0 -s INITIAL_MEMORY=1048576 -s TOTAL_STACK=307200
            let emcc_path = "em++";
            let mut build_wasm_proc = std::process::Command::new(emcc_path)
                .arg("--std=c++17")
                .arg("-o")
                .arg(output)
                .arg("-O3")
                .args(input)
                .arg(format!("{contractlib_dir}/contractlib/v1/contractlib.o"))
                .arg("-I")
                .arg(contractlib_dir)
                .arg("-s")
                .arg("EXPORTED_FUNCTIONS=[\"_call\",\"_deploy\"]")
                .arg("--no-entry")
                .arg("-Wl,--allow-undefined")
                .arg("-s")
                .arg("ERROR_ON_UNDEFINED_SYMBOLS=0")
                .arg("-s")
                .arg("WASM=1")
                .arg("-s")
                .arg("STANDALONE_WASM=0")
                .arg("-s")
                .arg("PURE_WASI=0")
                .arg("-s")
                .arg("INITIAL_MEMORY=1048576")
                .arg("-s")
                .arg("TOTAL_STACK=307200")
                .stdout(std::process::Stdio::piped())
                .stderr(std::process::Stdio::piped())
                .spawn()
                .expect("find em++ failed");
            let mut stdout = build_wasm_proc.stdout.take().unwrap();
            let mut stderr = build_wasm_proc.stderr.take().unwrap();
            let mut stdout_buf = Vec::new();
            let _ = stdout.read_to_end(&mut stdout_buf);
            let mut stderr_buf = Vec::new();
            let _ = stderr.read_to_end(&mut stderr_buf);
            println!("{}", String::from_utf8_lossy(&stdout_buf));
            println!("{}", String::from_utf8_lossy(&stderr_buf));
            let exit_status = build_wasm_proc
                .wait()
                .expect("Failed to wait for child process cpp to wasm");
            if exit_status.success() {
                println!("wasm built successfully to {output}");
            } else {
                panic!("build cpp to wasm failed");
            }
            // generate .cbin (4bytes-big-endian-length + wasm-bytes)
            let mut wasm_bytes = std::fs::read(output).unwrap();
            let mut cbin_bytes = Vec::new();
            let wasm_len = wasm_bytes.len() as u32;
            cbin_bytes.extend_from_slice(&wasm_len.to_be_bytes());
            cbin_bytes.append(&mut wasm_bytes);
            let cbin_output_filepath = format!("{output}.cbin");
            std::fs::write(&cbin_output_filepath, &cbin_bytes).unwrap();
            println!("contract cbin(prefix+wasm) built successfully to {cbin_output_filepath}");
            // generate .cbin.hex with is hex format of .cbin bytes
            let cbin_hex_output_filepath = format!("{output}.cbin.hex");
            let cbin_hex_bytes = hex::encode(&cbin_bytes);
            std::fs::write(&cbin_hex_output_filepath, cbin_hex_bytes).unwrap();
            println!("cbin hex built successfully to {cbin_hex_output_filepath}");
        }
        _ => {
            println!("no command");
        }
    }
}
