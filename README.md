# RV64GC Emulator
riscv-emu is a RV64GC(I, M, A, F, D Extensions) emulator made with purpose of running linux (debian riscv64 port to be more spefisific).

**This project is still work in progress cpu emulation is finished but there is still many things to do (look at todos section)**

### Requirements & Building
#### Requirements
* clang (msvc sucks)
#### Building
```bash
make
```

**It is not ready yet.**

### Todos
* Add TLB caches for mmu address translation
* Handle DTB and kernel load address
* Initialize MMIO sections
* Mimic GPU behavior (emulate VRAM in system memory, then transfer via SDL_Texture)
