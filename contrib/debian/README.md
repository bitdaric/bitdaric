
Debian
====================
This directory contains files used to package bitdaricd/bitdaric-qt
for Debian-based Linux systems. If you compile bitdaricd/bitdaric-qt yourself, there are some useful files here.

## bitdaric: URI support ##


bitdaric-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install bitdaric-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your bitdaric-qt binary to `/usr/bin`
and the `../../share/pixmaps/bitcoin128.png` to `/usr/share/pixmaps`

bitdaric-qt.protocol (KDE)

