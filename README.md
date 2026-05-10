# Assembler - Custom Assembly Language Processor

A two-pass assembler written in C for a custom assembly language (based on the Imaginary Computer architecture from the Open University). It reads `.as` source files, expands macros, validates syntax, and outputs object (`.ob`), entry (`.ent`), and extern (`.ext`) files.

---

## Project Structure

```
assembler/
├── src/                  # All .c source files
│   ├── main.c            # Entry point
│   ├── assembler.c       # Two-pass assembler logic
│   ├── lexer.c           # Lexical analysis / AST construction
│   ├── preprosesor.c     # Macro expansion and preprocessing
│   ├── function.c        # Shared utility functions
│   ├── tree.c            # Trie (prefix tree) implementation
│   └── vector.c          # Dynamic array implementation
├── include/              # All .h header files
│   ├── assembler.h
│   ├── lexer.h
│   ├── preprosesor.h
│   ├── function.h
│   ├── tree.h
│   └── vector.h
├── tests/                # Sample .as input files for testing
├── makefile              # Build system
└── README.md
```

---

## How It Works

### Pipeline

```
source.as
    │
    ▼
[ Preprocessor ]   — expands macros, strips comments → source.am
    │
    ▼
[ Lexer / Parser ] — tokenizes lines into AST nodes
    │
    ▼
[ First Pass ]     — builds symbol table, calculates addresses
    │
    ▼
[ Second Pass ]    — resolves labels, encodes machine words
    │
    ▼
source.ob / source.ent / source.ext
```

### Output Files

| File | Description |
|------|-------------|
| `.am` | Preprocessed assembly (macros expanded) |
| `.ob` | Object file — machine words in base64, with instruction/data counts |
| `.ent` | Entry symbols and their addresses |
| `.ext` | Extern symbol references and the addresses that use them |

---

## Building

### Requirements

- GCC (via [MinGW](https://winlibs.com) on Windows, or system GCC on Linux/macOS)
- GNU Make

### Build

```bash
make
```

This produces `assembler.exe` (Windows) or `assembler` (Linux/macOS).

### Clean

```bash
make clean
```

---

## Usage

```bash
assembler <file1> [file2] [file3] ...
```

Pass one or more base filenames (without extension). The assembler looks for `<name>.as` in the current directory.

**Example:**

```bash
assembler myprogram
```

Reads `myprogram.as`, writes `myprogram.am`, `myprogram.ob`, and (if applicable) `myprogram.ent` / `myprogram.ext`.

**Multiple files:**

```bash
assembler prog1 prog2 prog3
```

Each file is assembled independently.

---

## Supported Instructions

| Instruction | Operands | Description |
|-------------|----------|-------------|
| `mov`       | src, dst | Copy src to dst |
| `cmp`       | op1, op2 | Compare two operands |
| `add`       | src, dst | Add src to dst |
| `sub`       | src, dst | Subtract src from dst |
| `lea`       | label, dst | Load effective address |
| `not`       | dst      | Bitwise NOT |
| `clr`       | dst      | Clear (set to 0) |
| `inc`       | dst      | Increment by 1 |
| `dec`       | dst      | Decrement by 1 |
| `jmp`       | dst      | Jump to address |
| `bne`       | dst      | Branch if not equal |
| `jsr`       | dst      | Jump to subroutine |
| `red`       | dst      | Read input |
| `prn`       | dst      | Print operand |
| `rts`       | —        | Return from subroutine |
| `stop`      | —        | Halt execution |

## Supported Directives

| Directive | Example | Description |
|-----------|---------|-------------|
| `.data`   | `.data 1, -2, 3` | Store integers in data segment |
| `.string` | `.string "hello"` | Store a null-terminated string |
| `.entry`  | `.entry LABEL` | Mark label as entry point |
| `.extern` | `.extern LABEL` | Declare an external label |

## Macros

Define reusable code blocks:

```asm
mcro PRINT_R1
    prn r1
endmcro

PRINT_R1        ; expands inline
```

---

## Addressing Modes

| Mode | Syntax | Code |
|------|--------|------|
| Immediate | `#5` | 1 |
| Direct (label) | `LABEL` | 3 |
| Register | `r0`–`r7` | 5 |

---

## Error Reporting

Errors are printed to `stderr` in the format:

```
filename:line error: description
filename:line warning: description
```

The assembler continues processing after non-fatal errors so that all errors in a file are reported at once.

---

## Module Overview

| Module | Responsibility |
|--------|---------------|
| `main.c` | Parses CLI arguments, calls `assembler()` |
| `assembler.c` | Orchestrates preprocessing and two-pass compilation; produces output files |
| `lexer.c` | Tokenizes each line into an AST node; validates instruction/directive syntax |
| `preprosesor.c` | Reads `.as`, expands macros, writes `.am` |
| `function.c` | Shared helpers: string utilities, error printing, tree initialization |
| `tree.c` | Trie for O(n) keyword and symbol lookup |
| `vector.c` | Generic dynamic array used for code/data/symbol sections |
