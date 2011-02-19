#!/bin/sh

arm-elf-gcc -Wl,-elf2flt -o rr rr.c
cp rr /tftpboot

arm-elf-gcc -Wl,-elf2flt -o wr wr.c
cp wr /tftpboot

arm-elf-gcc -Wl,-elf2flt -o pcmcia pcmcia.c
cp pcmcia /tftpboot

arm-elf-gcc -Wl,-elf2flt -o pcmcia-setup pcmcia-setup.c
cp pcmcia-setup /tftpboot
