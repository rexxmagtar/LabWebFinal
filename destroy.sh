#!/bin/sh

SCR_SERVER_PATH="/var/www/json-docs"
if [[ ! -z "${SERVER_PATH}" ]]; then
    SCR_SERVER_PATH="${SERVER_PATH}"
fi

echo "Deleting the server root directory (${SCR_SERVER_PATH})..."
sudo rm -rf "${SCR_SERVER_PATH}"

echo "Deleting the user www and its group..."
sudo userdel www
sudo groupdel www

echo "All done!"
