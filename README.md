# picotype

![](logo/logo.png | width=100)

A Raspberry Pi Pico W based keyboard with a 3D printed case.

Type from your phone using bluetooth.

The case is designed in tinkercad and the firmware is written in C using the pico sdk, the pwa is created using
SvelteKit + TypeScript + Flowbite Svelte & tailwindcss.

@todo short gif of using picotype + bitwarden on phone

For nice looking documentation, see the [docs here](https://picotype.stack.rip/docs.html)

## 📜 Table of Contents

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

## 🔧 Installation

### 💾 Firmware

#### Requirements

- a Raspberry Pi Pico W (no headers required)
- a way to connect the pico to your computer (i.e. a micro usb cable)

1. Download the latest firmware.uf2 release from the [releases page](https://github.com/0xC9C3/picotype/releases)
2. Connect the pico to your computer while holding the BOOTSEL button
3. Copy the firmware.uf2 file to the RPI-RP2 drive that appears
4. The pico will reboot and the firmware will be loaded
5. Connect to the pico using bluetooth
6. Type away!

### 🖨 Case

#### Requirements

@todo add instructions

- 3D printer
- Filament
- Glue ?

1. Download the latest release from the [releases page](https://github.com/0xC9C3/picotype/releases)

### 📱 Web App

**While not necessary, I strongly recommend hosting your own instance of the web app
to avoid sending your keystrokes to a third party, in case the hosted version is compromised.**

**If you do not want to host your own instance, you can use the hosted version at https://picotype.stack.rip at your own
risk.**

**The following instructions are for hosting your own instance using cloudflare pages.**

**Alternatively a docker image is also available [here](https://hub.docker.com/r/0xc9c3/picotype).**

**If you just want to drop the pwa somewhere in a web server you can also download the built version
from the [releases page](https://github.com/0xC9C3/picotype/releases) and untar it or build it yourself.**

#### Requirements

- A cloudflare account

#### Setup

1. Fork this repo
2. Create a new cloudflare pages project
3. Connect the project to your forked repo
4. Deploy the project
5. For more information on how to use cloudflare pages, see
   the [cloudflare pages documentation](https://developers.cloudflare.com/pages/get-started/guide/#connect-your-git-provider-to-pages)

## 📝 Usage

1. Plug the pico into your computer
2. Connect to the pico using the web app, preferably using a phone, alternatively using a computer
   Tip: If you are using a phone, you can add the web app to your home screen for a more native experience
3. When asked, pair with the pico and when the LED is solid green, press the button on the pico once to accept the
   pairing request
4. Type away!
5. To reset the bonded device, press and hold the button on the pico for 5 seconds until the LED starts flashing
   green

Once the web app is bonded to the pico, it will automatically connect to it when it is plugged in.

**Note: The pico will not be discoverable by other devices while it is connected to the web app.**

## 🔒 Security Considerations

While I am not a security expert, I have tried to make this project as secure as possible. If you find any security
issues, please open an issue or contact me directly. I will try to fix any issues as soon as possible.

Here are some of the security considerations I have taken into account:

### Bluetooth

The firmware requires a pairing request to be accepted by pressing the button on the pico, this is to prevent
unauthorized devices from connecting to the pico. The pico is not discoverable while it is connected to the web app.

The pico only accepts to be bonded to one device at a time.

The bonding information can be reset by pressing and holding the button on the pico for 5 seconds until the LED starts
flashing green.

For the authentication, the pico uses the [Just Works](https://www.bluetooth.com/blog/bluetooth-pairing-part-4/)
method, which is not secure, but is the only method supported by the web bluetooth api.

### WebApp / PWA

The web app is served over https and uses
the [Content Security Policy](https://developer.mozilla.org/en-US/docs/Web/HTTP/CSP)
header to prevent loading of external resources. The web app does not use any external resources.

The web app does not store any data on the client side.

The best way to use the web app is to host your own instance, see [Web App](#web-app) for details.

Also, you can use the web app in offline mode, by adding it to your home screen.

### Firmware

The firmware does not store any data on the pico. The only data stored on the pico is the bonding information, which can
be reset by pressing and holding the button on the pico for 5 seconds until the LED starts flashing green.

## 💻 Development

### Requirements

- Raspberry Pi Debug Probe
- Pico Zero W with headers
- breadboard
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

## ⚖️ License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

Alternatively, see [WTFPL](http://www.wtfpl.net/) if you don't care about licensing.

Please also check the licenses of the dependencies used in this project. (This list is not complete!)

- [pico-sdk](https://github.com/raspberrypi/pico-sdk)
- [tinyusb](https://github.com/hathach/tinyusb)
- [btstack](https://github.com/bluekitchen/btstack)
- [Flowbite Svelte](https://flowbite-svelte.com/)

## saying thanks

If you want to say thanks or want to support the project the best way is by contributing.

If you still want to say thanks, I would be happy if you donated to any charity of your liking, because this
is [Careware](https://en.wikipedia.org/wiki/Careware) and not meant as a product or an income stream for me.