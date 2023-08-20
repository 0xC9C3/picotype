# picotype

A Raspberry Pi Pico W based keyboard with a 3D printed case.

Type from your phone using bluetooth.

The case is designed in tinkercad and the firmware is written in C using the pico sdk.

@todo short gif of using picotype + bitwarden on phone

For nice looking documentation, see the [docs here](picotype.stack.rip/docs.html)

## Table of Contents

- [Installation](#installation)
    - [Firmware](#firmware)
        - [Requirements](#requirements)
        - [Setup](#setup)
    - [Case](#case)
        - [Requirements](#requirements-1)
        - [Setup](#setup-1)
    - [Web App](#web-app)
        - [Requirements](#requirements-2)
        - [Setup](#setup-2)
- [Usage](#usage)
- [Development](#development)
    - [Requirements](#requirements-3)
    - [Setup](#setup-3)
- [License](#license)

## Installation

### Firmware

#### Requirements

- a Raspberry Pi Pico W (no headers required)

1. Download the latest release from the [releases page](https://github.com/0xC9C3/picotype/releases)
2. Connect the pico to your computer while holding the BOOTSEL button
3. Copy the firmware.uf2 file to the RPI-RP2 drive that appears
4. The pico will reboot and the firmware will be loaded
5. Connect to the pico using bluetooth
6. Type away!

### Case

#### Requirements

@todo add instructions

- 3D printer
- Filament
- Glue ?

1. Download the latest release from the [releases page](https://github.com/0xC9C3/picotype/releases)

### Web App

**While not necessary, I strongly recommend hosting your own instance of the web app
to avoid sending your keystrokes to a third party, in case the hosted version is compromised**

**If you do not want to host your own instance, you can use the hosted version at https://picotype.stack.rip at your own
risk**

**The following instructions are for hosting your own instance using cloudflare pages.**

#### Requirements

- A cloudflare account

#### Setup

1. Fork this repo
2. Create a new cloudflare pages project
3. Connect the project to your forked repo
4. Deploy the project
5. For more information on how to use cloudflare pages, see
   the [cloudflare pages documentation](https://developers.cloudflare.com/pages/get-started/guide/#connect-your-git-provider-to-pages)

## Usage

1. Plug the pico into your computer
2. Connect to the pico using the web app, preferably using a phone, alternatively using a computer
   Tip: If you are using a phone, you can add the web app to your home screen for a more native experience
3. Type away!

Once the web app is bonded to the pico, it will automatically connect to it when it is plugged in.

**Note: The pico will not be discoverable by other devices while it is connected to the web app.**

## Development

### Requirements

- Raspberry Pi Debug Probe
- Pico Zero W with headers
- openocd
- minicom or another serial terminal
- (optional) configure i.e. chrome to allow bluetooth connections
    - options can be found in chrome://flags for details see [this image](pwa/static/img/chrome_ble_settings.png)

### Setup

1. clone this repo including submodules
    ```bash
    git clone git@github.com:0xC9C3/picotype.git --recurse-submodules
    ```
2. install the pico sdk
    ```bash
    cd device/sdk &&
    git submodule update --init
    ```
3. build the project
    ```bash
    mkdir build
    cd build
    cmake ..
    make
    ```
4. connect the pico to the debug probe
5. load the firmware
    ```bash
    openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program cmake-build-debug/firmware.elf verify reset exit"
    ```
6. connect to the serial port
    ```bash
     minicom -b 115200 -o -D /dev/ttyACM0
    ```

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Alternatively, see [WTFPL](http://www.wtfpl.net/) if you don't care about licensing.

Please also check the licenses of the dependencies used in this project.

- [pico-sdk](https://github.com/raspberrypi/pico-sdk)
- [tinyusb](https://github.com/hathach/tinyusb)
- [btstack](https://github.com/bluekitchen/btstack)

