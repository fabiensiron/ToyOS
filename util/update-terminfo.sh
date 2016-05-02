#!/bin/bash

tic -e toyos toyos.terminfo -o toyos.tic
tic -e toyos-vga toyos.terminfo -o toyos-vga.tic
cp toyos.tic ../hdd/usr/share/terminfo/t/toyos
cp toyos-vga.tic ../hdd/usr/share/terminfo/t/toyos-vga
