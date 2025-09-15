from datetime import datetime
import re

fields = {
    "opcode": (0, 6),
    "rd": (7, 11),
    "funct3": (12, 14),
    "rs1": (15, 19),
    "rs2": (20, 24),
    "funct7": (25, 31),
}

def genmask(l, h):
    return ((1 << (h - l + 1)) - 1) << l

def parse_field_spec(fs):
    field, val = fs.split("=")
    val = int(val, 16)

    if field in fields:
        l, h = fields[field]
        return l, h, val

    m = re.match(r"\[(\d+):(\d+)\]", field)
    if m:
        l, h = int(m.group(2)), int(m.group(1))
        return l, h, val

    m = re.match(r"\[(\d+)\]", field)
    if m:
        l = h = int(m.group(1))
        return l, h, val

    raise ValueError(f"Invalid field specifier: {fs}")

if __name__ == "__main__":
    with open("table.txt", "r") as f:
        lines = f.readlines()

    instr_table = []
    for line in lines:
        line = line.strip()
        if line and not line.startswith("#"):
            parts = line.split()
            if len(parts) >= 2:
                name = parts[0]
                mask = 0
                match = 0
                for fs in parts[1:]:
                    l, h, val = parse_field_spec(fs)
                    m = genmask(l, h)
                    v = val << l
                    mask |= m
                    match |= v & m

                instr_table.append((mask, match, name.replace(".", "_")))

    with open("table.c", "w") as f:
        f.write(f'// This file is auto-generated. ({datetime.now().strftime("%Y-%m-%d %H")})\n\n')
        f.write('#include "table.h"\n')
        f.write('#include "../cpu.h"\n\n')
        for _, _, name in instr_table:
            f.write(f'extern void exec_{name}(cpu_t *cpu, uint32_t instr);\n')

        f.write('\ninstr_dispatch_table_t instr_dispatch_table[] = {\n')
        for mask, match, name in instr_table:
            f.write(f'    {{0x{mask:08X}, 0x{match:08X}, exec_{name}}},\n')

        f.write('};\n')