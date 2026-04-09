#!/usr/bin/env bash

set -e

echo "=============================="
echo " ESP-IDF Debian Setup Script"
echo "=============================="

ESP_DIR="$HOME/esp"
IDF_DIR="$ESP_DIR/esp-idf"

echo ""
echo ">>> Installing required Debian packages..."
sudo apt update
sudo apt install -y \
    git wget curl flex bison gperf \
    python3 python3-pip python3-venv python3-setuptools \
    cmake ninja-build ccache libffi-dev libssl-dev \
    dfu-util libusb-1.0-0

echo ""
echo ">>> Creating ESP directory..."
mkdir -p "$ESP_DIR"
cd "$ESP_DIR"

if [ -d "$IDF_DIR" ]; then
    echo "ESP-IDF already exists. Pulling latest changes..."
    cd "$IDF_DIR"
    git pull
    git submodule update --init --recursive
else
    echo "Cloning ESP-IDF..."
    git clone --recursive https://github.com/espressif/esp-idf.git
    cd "$IDF_DIR"
fi

#echo 'source ~/esp/esp-idf/export.sh' >> ~/.bashrc

echo ""
echo ">>> Running ESP-IDF installer..."
./install.sh

echo ""
echo ">>> Adding export.sh to .bashrc (if not already present)..."

if ! grep -q "esp-idf/export.sh" "$HOME/.bashrc"; then
    echo "source $IDF_DIR/export.sh" >> "$HOME/.bashrc"
    echo "Added export.sh to .bashrc"
else
    echo "export.sh already in .bashrc"
fi

echo ""
echo ">>> Activating environment for this session..."
source "$IDF_DIR/export.sh"

echo ""
echo ">>> Checking idf.py..."
idf.py --version

echo ""
echo ">>> Adding user to dialout group (for USB flashing)..."
sudo usermod -aG dialout $USER

echo ""
echo "========================================"
echo " Setup complete!"
echo " IMPORTANT: Log out and log back in"
echo " to apply dialout group permissions."
echo "========================================"


#source ~/esp/esp-idf/export.sh //connect to clang
#which clang