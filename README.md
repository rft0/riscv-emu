
# RV64GC Emulator
riscv-emu is a RV64GC(I, M, A, F, D Extensions) emulator made with purpose of running linux.
It emulates UART, CLINT and PLIC devices too. (virtio yet to be implemented)
Currently only supports sv39 MMU.

### Requirements & Building
#### Requirements
* clang
* Makefile
#### Building
```bash
$ git clone https://github.com/rft0/riscv-emu
$ cd riscv-emu
$ git submodule update --init --recursive
$ make
```

### Testing
Build `riscv-tests`
Run tests.py with arguments `<emulator_path>` `<tests_dir>`

### Usage
Arguments:
* `--dtb <path>`, Path to device tree blob to load
* `--sbi <path>`, Path to SBI elf file to load
* `--kernel <path>`, Path to kernel image to load
* `--initrd <path>`, Path to initial file system to load
* `--test <path>`, Path to `riscv-tests` elf file to load

**Note:** `--test` argument is only expected if program is compiled with `EMU_TESTS_ENABLED`.
**Another Note:** Emulator currently doesn't support virtio devices so kernel and initial filesystem be compatible with this.

### Todos
* Possibly add tlb instruction caches for address translation.
* Mimic GPU behavior (emulate VRAM in system memory, then transfer via SDL_Texture)
