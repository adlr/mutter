#!/bin/bash

rm -f /var/run/user/1000/mutter-bt.log
CLUTTER_DEBUG="frame-clock,frame-timings" MUTTER_DEBUG="backend,kms" dbus-run-session mutter --wayland vte-2.91 &> "${MUTTER_LOGFILE:-mutter-notile.log}"
# Valgrind variant:
#CLUTTER_DEBUG="frame-clock,frame-timings" MUTTER_DEBUG="backend,kms" dbus-run-session /usr/bin/valgrind --log-file=mutter-valgrind.txt --enable-debuginfod=no --leak-check=full --show-leak-kinds=definite mutter --wayland vte-2.91 &> "${MUTTER_LOGFILE:-mutter-notile.log}"
