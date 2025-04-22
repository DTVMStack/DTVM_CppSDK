# Contributing to DTVM_CppSDK

Thank you for considering contributing to DTVM_CppSDK! This document provides guidelines and instructions for contributing to this project.

## Code of Conduct

Please be respectful and considerate of others when contributing to this project.

## How to Contribute

1. **Fork the repository**
2. **Create a branch**: `git checkout -b feature/your-feature-name`
3. **Make your changes**
4. **Test your changes**: Ensure that your changes don't break existing functionality
5. **Submit a pull request**

## Development Environment Setup

1. **Install dependencies**:
   ```bash
   # Ubuntu 22.04 (recommended)
   apt update -y
   apt install -y git python3 xz-utils

   # Install Emscripten 3.1.69
   mkdir -p /opt
   cd /opt
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   ./emsdk install 3.1.69
   ./emsdk activate 3.1.69
   source "/opt/emsdk/emsdk_env.sh"
   ```

2. **Clone the repository**:
   ```bash
   git clone https://github.com/DTVMStack/DTVM_CppSDK.git
   cd DTVM_CppSDK
   ```

3. **Build the library**:
   ```bash
   bash build_lib.sh
   ```

## Coding Standards

- Follow the C++17 standard
- Use consistent indentation (4 spaces)
- Provide clear and concise comments
- Write unit tests for new functionality
- Document public API changes

## Pull Request Process

1. Ensure your code follows the coding standards
2. Update documentation if necessary
3. Include test cases for new functionality
4. Your pull request will be reviewed by maintainers

## Reporting Bugs

Please report bugs using the issue tracker with the following information:
- A clear, descriptive title
- Steps to reproduce the issue
- Expected behavior
- Actual behavior
- Environment details (OS, compiler version, etc.)

## Feature Requests

Feature requests are welcome. Please provide:
- A clear, descriptive title
- Detailed description of the proposed feature
- Any relevant examples or use cases

Thank you for contributing to DTVM_CppSDK! 
