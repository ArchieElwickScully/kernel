#!/bin/bash

cd src
export PATH="$HOME/opt/cross/bin:$PATH"
i686-elf-as boot.s -o bin/boot.o
i686-elf-gcc -c kernel.c -o bin/kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
i686-elf-gcc -T linker.ld -o bin/kernel.bin -ffreestanding -O2 -nostdlib bin/boot.o bin/kernel.o -lgcc
