#!/bin/bash

rm -f /var/run/user/1000/mutter-bt.log
CLUTTER_DEBUG="frame-clock,frame-timings" MUTTER_DEBUG="backend,kms" dbus-run-session gnome-shell --wayland -- vte-2.91 &> "${SHELL_LOGFILE:-shell-notile.log}"
