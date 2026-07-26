#!/usr/bin/env bash

sudo apt install libsdl2-dev

set -euo pipefail

MODULE="thinkpad_acpi"
CONF="/etc/modprobe.d/thinkpad_acpi.conf"

if [[ $EUID -ne 0 ]]; then
    echo "run as root niguh"
    echo "  sudo $0"
    exit 1
fi

mkdir -p /etc/modprobe.d

cat > "$CONF" <<EOF
options thinkpad_acpi fan_control=1
EOF


if lsmod | grep -q "^${MODULE}\b"; then
    if modprobe -r "$MODULE"; then
        echo "done unloading"
    else
        echo "module in use probably cant unload, reboot bich"
        exit 0
    fi
fi

if modprobe "$MODULE" fan_control=1; then
    echo "sahi ho"
else
    echo "bhayena mu.."
    exit 1
fi

echo

if [[ -f /sys/module/thinkpad_acpi/parameters/fan_control ]]; then
    echo -n "fan_control: "
    cat /sys/module/thinkpad_acpi/parameters/fan_control
fi

if [[ -f /proc/acpi/ibm/fan ]]; then
    echo
    echo "fan status:"
    cat /proc/acpi/ibm/fan

else
    echo "/proc/acpi/ibm/fan not found."
    echo "aape laptop"
fi