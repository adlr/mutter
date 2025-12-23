#!/bin/bash

sudo tee /etc/systemd/sleep.conf.d/sus-hib-off.conf<<'EOF'
AllowSuspend=no
AllowHibernation=no
AllowSuspendThenHibernate=no
AllowHybridSleep=no
EOF
echo reboot now to ensure it takes effect
