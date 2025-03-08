#!/bin/sh

make clean
make TARGET=funkey

mkdir -p opk
cp  main.elf opk/main.elf
cp -r ./res opk/res
cp KeyCraft.funkey-s.desktop opk/KeyCraft.funkey-s.desktop

mksquashfs ./opk KeyCraft.opk -all-root -noappend -no-exports -no-xattrs

rm -r opk