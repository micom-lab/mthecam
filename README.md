[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)

[This readme in english](#this-readme-in-english)

# MTheCam – Mini-Thermo-Cam (MTC)

## Einfache Thermocam zur Lokalisierung von Hot- und Coldspots
MTC erzeugt Wärmebilder im Format 8 * 8 Werte als laufenden Film mit bis zu 10 Aufnahmen/Sekunde. Als Sensor wird AMG8833 ‚Grid-Eye‘ von Panasonic eingesetzt, zur Steuerung ein ESP32 von Espressif. Optimal erschien uns dafür das M5StickC-Pico-Kit von M5Stack – ESP32 mit Mini-Display und Akku im kleinen Gehäuse - grundsätzlich werden aber andere MCU auf Basis ESP32 ebenfalls funktionieren, wenn die Display-Ansteuerung angepasst wird. Das Grid-Eye-Sensorboard kommt von MICOM in zwei Varianten.

Das Display zeigt einen Film von Thermobildern in Falschfarben sowie einen Messwert in °C je Pixel an. Eine Tabelle liefert Maximum-, Minimum- und Durchschnittswerte eines jeden Bildes. 

Ein eingebauter Webserver zeigt auf dem Client-Webbrowser diesen Film und die Messwerte an. Eine weitere Ansicht mit interpolierten Werten (32*32 Punkte) ist ebenfalls abrufbar (Button ‚image‘). Zur weiteren Verarbeitung der Messwerte wird eine JSON-Datensatz über /frame ausgegeben sowie Temperatur- und Thermofarbwert je Pixel mit den über jedes Bild errechneten Werten.

![MTheCam](images/mthecam.jpg "MTheCam – Mini-Thermo-Cam (MTC)")

Mehr zu den Grundlagen: <https://www.micom.de/lab/mthecam>

1. [Voraussetzungen](#voraussetzungen)
2. [Installation](#installation)
3. [Bau der Firmware](#bau-der-firmware)
4. [Ausgabe](#ausgabe)

### Voraussetzungen
- ESP32 basierte MCU – z. B. M5STickC
- AMG8833 Breakout-Board – z. B. MTC-BoB
- Arduino Entwicklungsumgebung
- Microsoft Visual-Code IDE
- Lust auf Experimente

### Installation
Das Breakout-Board wird auf die Stiftleiste des M5StickC-Moduls aufgesteckt. Es werden die Powerleitungen 5V und GND sowie für die I2C-Schnittstelle SDA und SCL verwendet. Der Anschluss des M5StickC an einen Computer geht über USB-C.

Die Software ist für die Arduino-Umgebung entwickelt worden. Daher muss diese installiert werden, auch, wenn eine andere IDE wie Visual-Studio verwendet werden soll.

#### Download und Installation von:
- Arduino-Entwicklungsumgebung
- MS Visual-Code als IDE (empfohlen)
- Extension ‚Arduino‘ vom VC-Marketplace
- ESP32 über ‚F1‘-Board Manager
- ‚M5StickC‘ über ‚F1‘-Board Config
- ‚M5StickC‘ über ‚F1‘-Lib Manager
- ‚ArduinoJSON‘ über ‚F1‘-Lib Manager (min. v6, nicht Arduino_JSON oder andere)
- MTheCam_LTSoftware in einem Verzeichnis mit gleichem Namen

Sollte ‚ESP32‘ nicht im Board-Manager erscheinen, bitte die Zeile `https://dl.espressif.com/dl/package_esp32_index.json` in die (**globale!**) `settings.json` unter `arduino.additionalUrls` eintragen. Mehr unter <https://docs.m5stack.com/#/en/arduino/arduino_development>

### Bau der Firmware
1. `SSID` und `PASSWORD` in der `MTheCam_LT.ino` mit den Werten des eigenen Wifi-Netzes belegen.
   ```
   // MTheCam_LT.ino
   const char* _ssid         = "YourSSIDHere";
   const char* _password       = "YourPasswordHere";
   ```
2. ‚F1‘-Upload über korrekten COMx Anschluss

### Ausgabe
`<IP-Adresse>` s. Router, z.B. 192.168.1.12.

- `<IP-Adresse>/` zeigt Wärmebild-Film mit Messwerten.
- `<IP-Adresse>/frame` liefert ständig aktualisierte JSON-Datensätze mit Messwerten und Farbwerten für eine Visualisierung.

---

Copyright (c) 2020 Olaf Mertens / MICOM GmbH Germany

---

##### This readme in english

# MTheCam - Mini Thermo Cam (MTC)

## Simple thermocam for localization of hot- and coldspots 
MTC generates thermal images in 8 * 8 values format as a movie with up to 10 frames/second. The sensor used is AMG8833 'Grid-Eye' from Panasonic, for control an ESP32 from Espressif. The M5StickC-Pico-Kit from M5Stack - ESP32 with mini-display and battery in a small housing - seemed to be optimal for this purpose. In principle, other MCUs based on ESP32 will also work, if the display control is adapted. The Grid-Eye-Sensorboard comes from MICOM in two different variants. 

The display shows a movie of thermal images in rainbow colors and a measured value in °C per pixel. A table provides maximum, minimum and average values of each single image. 

A built-in web server displays this movie and the measured values on the client web browser, too. Another view with interpolated values (32*32 points) is also available (button 'image'). For further processing of the measured values a JSON record is output via /frame as well as temperature and thermal color value per pixel with the values calculated over each image. 

![MTheCam](images/mthecam.jpg "MTheCam – Mini-Thermo-Cam (MTC)")

More about the basics: <https://www.micom.de/lab/mthecam>

1. [Requirements](#requirements)
2. [Installation](#installation)
3. [Building the firmware](#building-the-firmware)
4. [Output](#output)

### Requirements
- ESP32 based MCU - e.g. M5STickC 
- AMG8833 breakout board - e.g. MTC-BoB  
- Arduino development platform 
- Microsoft Visual Code DIE 
- Lot of fun to experiment 

### Installation
The breakout board is plugged into the header of the M5StickC module. The power lines - 5V and GND - and for the I2C interface - SDA and SCL - are used. The M5StickC gets connected to a computer via USB-C. 

The software has been developed for the Arduino environment. Therefore it must be installed first, even if another IDE like Visual-Studio is to be used.

#### Download and install
- Arduino development environment
- MS Visual-Code as IDE (recommended)
- Extension 'Arduino' from VC-Marketplace
- ESP32 via 'F1'-board manager
- 'M5StickC' via 'F1'-board config
- 'M5StickC' via 'F1'-lib manager
- 'ArduinoJSON' via 'F1'-lib manager (min. v6, not Arduino_JSON or others)
- MTheCam_LT software in a directory with the same name

If ‚ESP32‘ does not appear in theboard manager, please add the line `https://dl.espressif.com/dl/package_esp32_index.json` to the (**global!**) `settings.json` under ‚arduino.additionalUrls‘. More at <https://docs.m5stack.com/#/en/arduino/arduino_development>

### Building the firmware
1. Replace `SSID` and `PASSWORD` in the file `MTheCam_LT.ino` with the details of your Wifi network.
   ```
   // MTheCam_LT.ino
   const char* _ssid         = "YourSSIDHere";
   const char* _password       = "YourPasswordHere";
   ```
2. ‚F1‘-upload via COMx port

### Output
`<IP-Adresse>` see router, e.g. 192.168.1.12.

- `<IP-Adresse>/` shows thermal image movie with rainbow colors and measured values.
- `<IP-Adresse>/frame` provides continuously updated JSON dataset with measured temperature values and assigned color values for a visualization.

---

Copyright (c) 2020 Olaf Mertens / MICOM GmbH Germany