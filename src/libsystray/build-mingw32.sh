#!/bin/sh

CC=i486-mingw32-

export PKG_CONFIG_PATH=~/static-gtk2-mingw32-0.1-1/lib/pkgconfig
rm -f libzimbrasystray.dll
"${CC}gcc" --shared -static -O2 -Wall -Wextra -Wunused -Wshadow -Wcast-qual -fvisibility=hidden -o libzimbrasystray.dll zimbrasystray.c $(pkg-config --static --cflags --libs gtk+-2.0)
"${CC}strip" libzimbrasystray.dll
