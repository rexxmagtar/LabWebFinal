#!/bin/sh

SCR_SERVER_PATH=/var/www/json-docs
if [[ ! -z "${SERVER_PATH}" ]]; then
    SCR_SERVER_PATH="$SERVER_PATH"
fi
echo "Creating the server root directory ($SCR_SERVER_PATH)..."
sudo mkdir -p "$SCR_SERVER_PATH"
sudo mkdir -p "$SCR_SERVER_PATH/jsons"
sudo mkdir -p "$SCR_SERVER_PATH/fcgi"
echo "Copying the script.fcgi to $SCR_SERVER_PATH/fcgi..."
sudo cp build/script.fcgi "$SCR_SERVER_PATH/fcgi"

echo "Creating the user www..."
sudo useradd www
sudo usermod -a -G www $(id -un)

echo "chown'ing and chmod'ing the server root..."
sudo chown -R www:www "$SCR_SERVER_PATH"
sudo chmod -R 0770 "$SCR_SERVER_PATH"

echo "All done!"
