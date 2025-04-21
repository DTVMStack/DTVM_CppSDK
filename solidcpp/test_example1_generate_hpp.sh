#!/bin/bash
cargo run -- generate-hpp --input ../examples/example1/generated/ITokenService_meta.json --input ../examples/example1/generated/MyToken_meta.json \
 --input ../examples/example1/generated/ITokenService_storage.json --input ../examples/example1/generated/MyToken_storage.json \
 --output ../examples/example1/generated/my_token_decl.hpp

