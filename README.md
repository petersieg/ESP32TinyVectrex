# Tiny Vectrex port for ESP32 and Oscilloscope
This is a port of the Ackerman Tiny Vectrex (which used TTGO VGA32 and a VGA monitor)
(link: See https://github.com/rpsubc8/ESP32TinyVectrex)

This version uses a standard ESP32 and an old style CRT Oscilloscope for a real vector screen experience.

See video: https://youtu.be/J523SKyvs14

This github repo is just a cleanup/remove unnused code from: See https://github.com/OzmoOzmo/ESP32TinyVectrex

It is a work in progress - currently no audio and due to limitations of ESP - speed is not 100% of the original Vectrex.
But it is great to see real vector graphics in action.

<hr>

Original code owners - see chapter above and see LICENSE file. My pictures and changes are CC-BY-NC-SA.

Here are pictures of MyEsp32Vectrex:

Overview:

<img src='https://github.com/petersieg/ESP32_TinyVectrex/blob/main/MyEsp32Vectrex-1.jpeg'>

Esp32 and joystick buttons case (old network switch):

<img src='https://github.com/petersieg/ESP32_TinyVectrex/blob/main/MyEsp32Vectrex-2.jpeg'>

<pre>
Code var - GPIO - Remark
b1 - 15 - (not wired/used)
b2 - 13 - Hyperspace
b3 - 12 - Thrust
b4 - 14 - Fire
CuL - 16 - Move left - Rx2 Pin
CuR - 17 - Move right - Tx2 Pin
CuU - 18 - Move Up - (not wired/used)
CuD - 19 - Move Down - (not wired/used)
</pre>

See Esp32 30-pin GPIO Pinout:
<img src='https://github.com/petersieg/ESP32_TinyVectrex/blob/main/ESP32-GPIO-Pins.png'>

HM412 display:

<img src='https://github.com/petersieg/ESP32_TinyVectrex/blob/main/MyEsp32Vectrex-3.jpeg'>

<hr>

<center><img src='https://github.com/petersieg/ESP32_TinyVectrex/blob/main/previewVectrexLogo.gif'></center>
Modifications from TTGO VGA32 version:
<ul>
 <li>Outputs vector lines via ESP32 Dac outputs</li>
 <li>Removed all VGA libraries to improve performance</li>
 <li>Removed all PS2 keyboard support - now uses esp32 pins for input - see FastDac.h</li>
 <li>Removed all OSD stuff - P.Sieg March 2023</li>
 <li>Removed all WIFI stuff - P.Sieg March 2023</li>
 <li>Removed all DEMOS after seeing, that they don't run fine - P.Sieg March 2023</li>
 <li>Removed all fabgl dependencies - no longer need fabgl lib - P.Sieg April 2023</li>
 <li>Unfortunitely no vga means loosing Ackermans menus</li>
</ul>


<h1>Requirements</h1>
Required:
 <ul>
  <li>Any ESP32 board which gives access to both DAC Pin 26 + 25 and other defined Pins in FastDac.h for keypad</li>
  <li>Arduino IDE 1.8.11 (or better) with ESP32 addon for Arduino IDE from Espressif System Version 1.0.6</li>
 </ul>


<h1>Setup</h1>
<ul>
<li>Just connect the two dac pins on the ESP32 (pin 25 and 26) to the scope channels </li>
<li>connect one GND on ESP32 to one of the scopes GND.</li>
<li>Set the scope to XY mode</li>
<li>And importantly, set both to channels DC (not AC) or display will not be stable. </li>
</ul>
 
<h1>Notes:</h1>
<ul>
<li>Simply swap X and Y leads around if the image appears rotated</li>
<li>I have some of the key inputs assigned to input pins - see code FastDac.h and osint.cpp for details.</li>
<li>Must be a ESP32 - ESP8266 doesnt have DACs.</li>
<li>its not 100% full speed - todo: moving some code to the second unused core of the ESP32 might help.</li>
</ul>


<h1>Arduino IDE</h1>
The whole project is compatible with the Arduino 1.8.11 framework.
We only have to open the <b>TinyVectrex.ino</b> in the <b>TinyVectrex</b> directory.
<center><img src='https://github.com/petersieg/ESP32_TinyVectrex/blob/main/previewArduinoIDEpreferences.gif'></center>
We must install the espressif extensions in the additional card urls manager <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
<br>
We must deactivate the PSRAM option, and in case of exceeding 1 MB of binary, select 4 MB of partition when uploading. Although the code does not use PSRAM, if the option is active and our ESP32 does not have it, an exception will be generated and it will restart in loop mode.

 
<h1>Test Applications</h1>
The MineStorm game is included:
<center><img src='https://github.com/petersieg/ESP32_TinyVectrex/blob/main/previewMineStorm.gif'></center>
