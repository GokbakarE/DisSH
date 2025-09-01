# DisSH - SuperH Disassembler

DisSH is a professional, lightweight SuperH architecture disassembler written in C++20.  
It compiles with modern C++ standards and is designed to be easily extensible and efficient.

## Features

- Supports multiple SuperH instruction sets (SuperH1, SuperH2, SuperH3, SuperH4 variants)  
- Modular source code structure with separate libraries  
- Static library support for reusable components  
- Cross-platform build support for Linux and macOS  
- Simple and clean Makefile-based build system  

## Requirements

- C++20 compatible compiler (e.g., g++ 10 or newer)  
- GNU Make  
- Linux or macOS environment  

## Build and Installation

To build the project, clone the repository and run:

```bash
git clone https://github.com/GokbakarE/DisSH.git
cd DisSH
make            
cd lib
make            
```

- After building and installation, the executable `DisSH` will be available in your system.

## Usage

Run the executable with the desired options and input files:

```bash
DisSH [options] <16-bit-input>
```

Options are:
```bash
--SuperH1

--SuperH2

--SuperH3

--SuperH3E

--SuperH3DSP

--SuperH4

--SuperH4A

--SuperHDSP
```
## License

This project is licensed under the GNU AGPLv3 - see the [LICENSE.md](LICENSE.md) file for details.

## Author

GokbakarE
© 2025 GokbakarE. All rights reserved.