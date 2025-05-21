# 420D – Firmware Add-on for Canon EOS 400D

**420D** is an unofficial fork of the [400plus](https://github.com/400plus/400plus) firmware add-on, originally developed for the Canon EOS 400D (also known as the Digital Rebel XTi).
This version celebrates the **20th anniversary** of the 400D by building upon the original project with updates, improvements, and a refreshed focus.

---

## 🎯 Project Goals

- Modernize and clean up the original codebase
- Improve documentation and accessibility
- Modify existing features to personal taste
- Add new features suited to today’s use cases
- Extend the useful life of the Canon EOS 400D

---

## 📦 Based on 400plus

This project is based on the original [400plus/400plus](https://github.com/400plus/400plus) repository.
Full credit and thanks go to the original developers for their outstanding work.

The original source code is licensed under the [GNU GPL v2](https://www.gnu.org/licenses/old-licenses/gpl-2.0.html), which this project continues to honor.

---

## Original 400plus features

Some of the most popular features of `400plus` are:
  * Custom modes:
    * Complete snapshots of all settings and parameters.
    * Can be assigned to any scene mode for a quick access.
    * Store up to 16 custom modes on each CF card.
  * Scripting support:
    * Extended AEB,
    * Flash, Aperture, Exposure, and ISO based AEB,
    * Ultra-long exposures, featuring a long-exposure calculator,
    * Intervalometer, featuring a time-lapse calculator,
    * Bulb-ramping,
    * Self-timer,
    * Touch-less trigger,
    * Depth of field and hyperfocal calculator.
  * Extended range for some parameters, line Av or Flash compensation.
  * Custom white balance, up to 16 color temperatures can be named and stored.
  * View and change the ISO while looking through the viewfinder.
  * Our own configurable AutoISO.
  * Safety Shift for creative modes.
  * Extended AF patterns.
  * Intermediate ISO values.
  * Spot metering mode.
  * Multi-spot metering.
  * Fixed exposure for M mode.
  * Quick exposure button for M mode.
  * Configurable buttons.
  * RAW file format output for auto modes. 
  * Release counter.

`400plus` has been translated into several languages, and can even be translated to languages not supported by the camera.

## 🧩 New Features / Planned Work

- Revamped user interface
- Menu reorganization
- Few modifications/corrections according to 400plus issue tracker

---

## 🐳 Building with Docker/Podman

To build the project in a clean and reproducible environment, you need to install podman, then:

- make

will build the docker image if necessary, then build the software in the container.

- make install

will copy the generated software and files to the INSTALL_PATH, defaulting to ./output

- make clean

will delete the intermediate build files (but not the docker image).

# The firmware will be built using the toolchain inside the container

---

## Original 400plus instructions

_Please, read "[a word of warning](https://github.com/400plus/400plus/wiki/A-word-of-warning)" at [our wiki](https://github.com/400plus/400plus/wiki) before doing anything else._

There is a [user guide](https://github.com/400plus/400plus/wiki/User-guide), where you can get a better idea of what to expect from this project. Then, if you want to give it a try, there is also an [installation guide](https://github.com/400plus/400plus/wiki/Firmware-Hack-Installation).

## 🔧 Installation & Usage

> *Installation instructions will be added once testing is complete.*

---

## 📝 License

This project is released under the **GNU GPL v2** license, in accordance with the original 400plus project.
See [LICENSE](./LICENSE) for full terms.

---

## 📸 About

- Project started in 2025 by Frederic Cordonier
- A tribute to the Canon EOS 400D – 2005–2025 ❤️
