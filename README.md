# Hackathon 2022 - Smart Watering System

This project is a Smart Watering System for the IoT Octopus.
A soil moisture sensor measures soil moisture and temperature and relays it to an Octopus.
The Octopus evaluates the sensor data and sends it to a second octopus via LoRaWAN.
The second Octopus transmits the sensor data to Thingspeak via WLAN.
If the soil moisture is too dry, it activates a solenoid valve to water the plants.

## Hardware components
- 2× [IoT Octopus](https://www.tindie.com/products/fablab/iot-octopus-badge-for-iot-evaluation/) with an ESP8266
- 2× TTN FeatherWing or [LoRa FeatherWing](https://www.adafruit.com/product/3229) with the RMF69 chip (868MHz)
- 1× [STM50 Soil Moisture Sensor](https://www.truebner.de/de/smt50.php) by Truebner
- 1× [Grove I2C ADC](https://www.seeedstudio.com/Grove-I2C-ADC.html) by Seeed Studio
- 2× [Grove Screw Terminal](https://www.seeedstudio.com/Grove-Screw-Terminal.html) by Seeed Studio
- 1× [Protective Outdoor Case](https://www.amazon.de/dp/B017ILZF6C) by Dostmann
- 1× [Relay 5V Module](https://www.amazon.com/dp/B07BVXT1ZK) Breakout Board
- 1× [Grove 4 Pin Female Jumper](https://www.amazon.com/dp/B07BVXT1ZK) for the Relay
- 1× [Solenoid Valve DC 12V G3/4](https://www.amazon.com/dp/B07F1DYFNK) for the water hose
- 1× [3D printed Case](https://www.thingiverse.com/thing:5378593) for the Octopus

## Software
The software of the Smart Watering System uses the ["IoT Workshop"](https://www.umwelt-campus.de/forschung/projekte/iot-werkstatt) with can be downloaded [here](https://seafile.rlp.net/f/bb6bdbfe28034cd4861a/).
To use the software, you also need the [LoRa library](https://github.com/sandeepmistry/arduino-LoRa) by Sandeep Mistry, which can be downloaded from the Arduino library manager via "Sketch->Include Library->Manage Libraries".
Ardublock can then be opened via "Tools->Ardublock".
You can use either the Ardublock files or the Arduino C code of this project to upload the software.

## Assembly
1. Enter your WiFi credentials and your Thingspeak API key into the "Lawn_Sprinkler" file
2. Upload the program "Moisture_Detector" to the first Octopus and the "Lawn_Sprinkler" to the second Octopus
3. Screw the wires of the soil moisture sensor to the two screw terminals
4. Connect one screw terminal to the Octopus ADC connector and the second screw terminal to the Grove I2C ADC
5. Connect the Grove I2C ADC to the Octopus I2C connector
6. Plug the LoRaWAN module onto the pin headers of the Octopus
7. Put the Octopus together with the I2C ADC and the screw terminals in the protective outdoor case
8. Power the Octopus with a 3V battery or a micro USB cable
9. Connect the Relais with the Grove Jumper to the I2C connector of the second Octopus
10. Place the solenoid valve on the watering tap
11. Screw the relay between the wire from the 12V power supply to the solenoid valve
12. Put the second Octopus together with the relay into the [3D printed Case](https://www.thingiverse.com/thing:5378593)
13. Plug the LoRaWAN module onto the pin headers of the second Octopus
14. Power the second Octopus with a micro USB cable
