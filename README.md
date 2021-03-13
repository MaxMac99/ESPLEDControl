# LED-HAP-ESP8266

HomeKit Accessory Protocol integration for ESP8266 with WS2812B individually addressable LEDs

## Installation Guide

### Requirements

- Install git
- Install python
- Install Visual Studio Code (VSCode)
- In VSCode go to the extensions tab on the left and install the Platformio IDE
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

### 4. Python setup

To prepare your python environment run `pip3 install -r requirements.txt`

### 5. Initial device setup

For the first setup of a new device, open [platformio.ini](platformio.ini) and paste the following template:

```ini
[env:<ENV_DEVICE_NAME>]
upload_port = <DEVICE_PORT>    ; For use with usb
upload_speed = 460800
monitor_port = <DEVICE_PORT>
monitor_filters = time, colorize, esp8266_exception_decoder
build_flags =
    ${env.build_flags}
    -D HKLOGLEVEL=4
    -D NUM_LEDS=<NUMBER_OF_LEDS>
    -D HK_NAME=\"<HK_DEVICE_NAME>\"
    -D HK_MODE_PREFIX=\"<DEVICE_PREFIX_FOR_HOMEKIT>\"
    -D HK_UNIQUE_NAME
    -D ALEXA_SUPPORT
    -D RGBW_COLORS  
    -D MODE_STATIC
    -D MODE_FADE
    -D MODE_RAINBOW_CYCLE
    -D MODE_RANDOM
    -D HKPASSWORD=\"<GEN_PASSWORD>\"        ; Add your generate Password from genCode.py
    -D HKSETUPID=\"<GEN_SETUP_ID>\"               ; Add your generate Setup ID from genCode.py
```

Change the following:
- <ENV_DEVICE_NAME>: The name to identify in the 
- <DEVICE_PORT>: Go to the platformio tab on the left, go to Default and click on devices. You will see a list of all your devices in the terminal at the bottom. Copy the full name to your device to the <DEVICE_PORT>
- <NUMBER_OF_LEDS>: Number of LEDs connected to the Chip
- <HK_DEVICE_NAME>: Name that will appear in the WiFi-Settings. Has to be escaped (" " to "\ ", """ to "\", ...)
- <HK_MODE_PREFIX>: Prefix that will appear in front of every HomeKit device ("Static" will be renamed to "<HK_MODE_PREFIX> Static")
- <GEN_PASSWORD>: Generated password (see below)
- <GEN_SETUP_ID>: Generated setup-id (see below)

You can comment out some of the modes by adding a ; in front of the -D.
For all available Options look at the top of [platformio.ini](platformio.ini).

After that go to the platformio tab, select you device and click Upload.
Once it says successful you can disconnect the device.

### Generate a password

You have to generate a new pair of password and setup-id. To do so, go back to the terminal and enter

`python3 genCode.py`

This command will show you a new password and setupid that you can replace with the one above. This is highly recommended due to security issues. Further, if you have multiple devices running the same password and setupid, the devices will no longer work.


On macOS and Linux you can generate a QR-Code by running:

`python3 genCode.py --qrcode --filename=<name_of_file_to_generate>`.

If you want to generate a QR-Code for a existing password and setup-id run the following: 

`python3 genCode.py --qrcode --filename=<name_of_file_to_generate> --password=<password> --setupid=<setupid>`


### 6. OTA Update

For Over the Air Updates (OTA) you have to change the [platformio.ini](platformio.ini) file again.
Go to the device you want to update and replace
```ini
upload_port = <DEVICE_PORT>    ; For use with usb
upload_speed = 460800
```
with
```ini
upload_protocol = espota                ; For use with OTA
upload_port = <DEVICE_IP>            ; Enter IP-Address or hostname.local for your device to flash
upload_flags =
    --port=8266
    --auth=<GEN_PASSWORD>                  ; Enter the HKPASSWORD as password
```

- <DEVICE_IP>: Go to your Router and find out the IP-address of your device.
- <GEN_PASSWORD>: Enter the password of HKPASSWORD here again, in the format of xxx-xx-xxx without any " or \ .

Go back to the platformio tab, select you device and click Upload.

