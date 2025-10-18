#!/bin/bash

meson setup --buildtype=plain --prefix=/usr --libdir=/usr/lib64 --libexecdir=/usr/libexec --bindir=/usr/bin --sbindir=/usr/bin --includedir=/usr/include --datadir=/usr/share --mandir=/usr/share/man --infodir=/usr/share/info --localedir=/usr/share/locale --sysconfdir=/etc --localstatedir=/var --sharedstatedir=/var/lib --auto-features=enabled . ${BUILDDIR:-builddir} -Degl_device=true

#meson setup ${BUILDDIR:-builddir}
#meson configure ${BUILDDIR:-builddir} --prefix=/usr
