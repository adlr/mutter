rm /var/run/user/1000/mutter-bt.log
CLUTTER_DEBUG="frame-clock,frame-timings" MUTTER_DEBUG="backend,kms" dbus-run-session mutter --wayland vte-2.91 &> mutter.log
