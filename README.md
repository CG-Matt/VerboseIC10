# Verbose IC10

Verbose IC10 is an experimental compiler for the IC10 microcontroller used in Stationeers.
It accepts `.vic10` source files and emits IC10 code, with a small higher-level syntax for
registers, devices, constants, subroutines, conditionals, and common device access patterns.

The language and compiler are still incomplete, but the included example program shows the
currently supported syntax.

## Quick Start

Build the compiler with `make`:

```sh
git clone https://github.com/CG-Matt/VerboseIC10.git
cd VerboseIC10
make
```

Compile the example program:

```sh
./build/victc programs/example.vic10
```

Generated `.ic10` output is written to the output folder configured in `parser.config`.
By default, both input and output paths point at `./programs`.

To remove build output:

```sh
make clean
```

## Configuration

The compiler reads `parser.config` from the working directory. The default file controls
logging, input/output folders, and the available IC10 target limits:

```txt
LOG_OUTPUT FALSE
LOG_REF_TABLE FALSE
IN_FOLDER_PATH ./programs
OUT_FOLDER_PATH ./programs
MAX_DEVICE_COUNT 6
MAX_REGISTER_COUNT 16
```

## Writing `.vic10` Files

Each `.vic10` file should declare the syntax version it expects:

```vic10
#version 4
```

The optional carry module reserves the carry register for commands that need temporary
storage:

```vic10
#using carry
```

Lines beginning with `$` are comments.

## Registers And Devices

Registers can be declared one at a time:

```vic10
reg counter
reg setting
```

or as a list:

```vic10
reg [counter, setting, prefab_hash]
```

Devices can also be declared individually:

```vic10
dev d0 computer
dev d1 vending_machine
```

or assigned automatically from the available device targets:

```vic10
dev * = [computer, vending_machine]
```

## Constants

Constants are compile-time references and do not reserve registers:

```vic10
const target_temperature = 293
```

Constants can be read like other values, but cannot be assigned to.

## Assignments And Math

Registers can be assigned directly:

```vic10
counter = 0
```

Math operations can be written implicitly:

```vic10
counter += 5
counter = counter + 1
```

Math functions use `<-` to pass their input value:

```vic10
setting = sqrt <- 125
```

The older explicit `set` and `math` commands are still supported.

## Device Access

`xref` loads values from device variables or writes values back to them.

Load a device variable into a register:

```vic10
xref computer.On -> setting
```

Write a register or value to a device variable:

```vic10
xref vending_machine.Setting <- setting
xref computer.Setting <- 10
```

Use `@device_name` as shorthand for `device_name.PrefabHash`:

```vic10
xref @computer -> prefab_hash
```

Use `*` when writing to every connected device with a matching prefab hash:

```vic10
xref *prefab_hash.Setting <- 10
```

The older `import` and `export` commands are still supported, but `xref` is preferred.

## Control Flow

Labels define jump targets:

```vic10
label loop_start
jump => loop_start
```

Branches jump only when their condition is true:

```vic10
branch => loop_start if counter < 20
```

`loop` emits a compiler-managed loop label, and `end loop` jumps back to it:

```vic10
loop
    counter += 1

    if counter >= 3
        jump => done
    end if
end loop

label done
```

Subroutines are declared with `sub` and closed with `end sub`:

```vic10
sub increment
    counter += 1
end sub

call increment
call increment if counter < 20
```

Conditionals use `if`, optional `else`, and `end if`:

```vic10
if setting != 0 and counter > 20
    xref computer.Setting <- setting
else
    counter += 10
end if
```

## Example Program

See `programs/example.vic10` for a longer syntax outline covering the currently implemented
commands.
