#!/bin/bash

meson setup ${BUILDDIR:-builddir}
meson configure ${BUILDDIR:-builddir} --prefix=/usr
