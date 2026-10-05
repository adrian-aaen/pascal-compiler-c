# MiniPas to C Compiler

This is a university project originally written in 01.2024, but uploaded to GitHub at a later date.

PoC compiler that compiles MiniPas to a minimal style of C. The style of the C output is meant to resemble assembly or an IR language, without having to build the stack frames ourselves.

Running `make` creates a `compiler` binary in the `./build` folder. Run it with `./build/compiler my-code.pas my-output.c` or see the `compile` make command.
