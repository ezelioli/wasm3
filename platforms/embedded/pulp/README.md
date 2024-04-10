# Wasm3 on PULP

## Directory structure

- Makefile: entry point to compile / execute wasm3 on control-pulp. Configuration options can be found at the top of it.
- runtime: contains stripped down pulp-runtime. Few modifications to the minimal libc and the linker script. 
- src/wasm3: link to the wasm3 source tree.
- src/wasm3/extra: contains few C headers with some simple .wasm examples.
- srt/main.c: actual application that calls the wasm3 functions to run the interpreter.

## Setup

### Somewhere in the file system


1. Get control-pulp
```bash
git clone https://github.com/pulp-platform/control-pulp.git
git submodule update --init --recursive
```

2. Build RTL sources for simulation (check control-pulp readme)
```bash
source env/env.sh
make all
```

### Within this repo

1. Set up environment

Pointer to control-pulp:
```bash
export VSIM_PATH=<path/to/control-pulp>/sim
```

Pointer to riscv toolchain (only one of the following):
```bash
export PULP_RUNTIME_GCC_TOOLCHAIN=<riscv/toolchain/path>
export PULP_RISCV_GCC_TOOLCHAIN=<riscv/toolchain/path>
export PATH=$PATH:<riscv/toolchain/path>/bin
```

Go to pulp target directory:

```bash
cd platforms/embedded/pulp
```

2. Build wasm3 library + custom application
 
```bash
make all
```

3. Run simulation

```bash
make run
```

Good luck :')
