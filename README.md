# LED-HAP-ESP8266

HomeKit Accessory Protocol integration for ESP8266 with WS2812B individually addressable LEDs

## Installation Guide

### Requirements

- Install git
- Install python
- Install Visual Studio Code
- Create a GitHub Account

### 1. GitHub Access

First make sure you have access to the following repositories:

- [ESPLEDControl](https://github.com/MaxMac99/ESPLEDControl)
- [ESPHomeKit](https://github.com/MaxMac99/ESPHomeKit)
- [ESPWiFiSetup](https://github.com/MaxMac99/ESPWiFiSetup)

### 2. Setup SSH Keys

Type in the windows search bar `cmd` and hit enter and type in the following commands

1. `ssh-keygen -t ed25519 -C "your_email@example.com"`
2. Use the default location
3. Do not use a password (just hit enter 2 times)
4. Go to your user folder and open the .ssh folder
5. Open the `id_ed25519.pub` file with the editor and copy all
6. Go to GitHub and select settings > SSH and GPG Keys > New SSH Key.
Past the content of the file to the key field, add a title you want and hit Add SSH Key.
7. Go back to the cmd and enter the following command `ssh -T git@github.com`

### 3. Clone Repository

Open Visual Studio Code, click on Clone a repository and paste the following:

`git@github.com:MaxMac99/ESPWiFiSetup.git`

Select a folder where you want to store the code.
Afterwards open the cloned folder with VSCode.
### 4. 
