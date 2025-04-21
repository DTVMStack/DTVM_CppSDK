# Installation Guide

This document outlines the steps to install the DTVM_CppSDK toolkit and its dependencies.

## System Requirements

- **Operating System**: Ubuntu 22.04 (recommended)
- **Disk Space**: At least 1GB free space
- **Memory**: 4GB RAM or more recommended

## Installing Dependencies

### 1. Basic Tools

Install the required basic tools:

```bash
apt update -y
apt install -y git python3 xz-utils
```

### 2. Emscripten SDK

Install Emscripten version 3.1.69 (recommended for compatibility):

```bash
mkdir -p /opt
cd /opt
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install 3.1.69
./emsdk activate 3.1.69

# Activate in current shell
source "/opt/emsdk/emsdk_env.sh"

# Configure for shell startup (optional)
echo 'source "/opt/emsdk/emsdk_env.sh"' >> $HOME/.bash_profile
```

### 3. DTVM_CppSDK Toolkit

#### Option 1: From Release Package

Download and install the toolkit:

```bash
# Copy the toolkit package to /opt directory
cp path/to/DTVM_CppSDK.linux.latest.tar.gz /opt
cd /opt
tar -xzvf DTVM_CppSDK.linux.latest.tar.gz

# Verify the binaries are working
./solc --version
./solidcpp --version

# Add toolkit to PATH (optional)
echo 'export PATH="/opt:$PATH"' >> $HOME/.bash_profile
source $HOME/.bash_profile
```

#### Option 2: From Source (for contributors)

Clone the repository and build:

```bash
git clone https://github.com/DTVMStack/DTVM_CppSDK.git
cd DTVM_CppSDK
bash build_lib.sh
```

## Docker Installation (Alternative)

If you prefer using Docker, we provide a Docker image with all dependencies pre-installed:

```bash
# Pull the Docker image
docker pull yourorg/dtvmcppcompiler:latest

# Run a container
docker run -it --name dtvmcpp-dev yourorg/dtvmcppcompiler:latest

# For development with volume mounting
docker run -it -v $(pwd):/workspace --name dtvmcpp-dev yourorg/dtvmcppcompiler:latest
```

## Verifying Installation

To verify that DTVM_CppSDK is correctly installed, try building one of the example contracts:

```bash
cd examples/example1
bash build_wasm.sh
```

If the build completes successfully, you should see a file named `my_token.wasm` in the current directory.

## Next Steps

After installation, refer to the [Quick Start Guide](quick_start.md) to begin developing your first smart contract with DTVM_CppSDK.
