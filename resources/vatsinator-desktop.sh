#!/bin/sh
cat <<EOF
[Desktop Entry]
Version=$1
Type=Application
Name=Vatsinator
GenericName=Vatsim monitor
Comment=A simple Vatsim monitor
TryExec=$2/bin/vatsinator
Exec=$2/bin/vatsinator &
Categories=Application;Network;Qt;
Icon=vatsinator
EOF
