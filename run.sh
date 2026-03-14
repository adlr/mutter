#!/bin/bash


rotation=
tiling="notile"
while getopts "r:t" opt; do
    case $opt in
        r) rotation="$OPTARG" ;;
        t) tiling="tile" ;;
    esac
done

MUTTER_LOGFILE="mutter-${rotation}-${tiling}.log"

if [ -n "$rotation" ]; then
    export XDG_CONFIG_HOME=/home/adlr/Code/rpms/mutter-src/config-debug/"$rotation"
fi

export TILE_EN=0
if [ "$tiling" = "tile" ]; then
    export TILE_EN=1
fi

rm -f /var/run/user/1000/mutter-bt.log

CLUTTER_DEBUG="frame-clock,frame-timings" \
MUTTER_DEBUG="backend,kms" \
dbus-run-session mutter --wayland vte-2.91 &> "${MUTTER_LOGFILE:-mutter-notile.log}"
# Valgrind variant:
#CLUTTER_DEBUG="frame-clock,frame-timings" MUTTER_DEBUG="backend,kms" dbus-run-session /usr/bin/valgrind --log-file=mutter-valgrind.txt --enable-debuginfod=no --leak-check=full --show-leak-kinds=definite mutter --wayland vte-2.91 &> "${MUTTER_LOGFILE:-mutter-notile.log}"
