# Tiny Vectrex port for ESP32 and Oscilloscope
This is a port of the Ackerman Tiny Vectrex (which used TTGO VGA32 and a VGA monitor)
(link: See https://github.com/rpsubc8/ESP32TinyVectrex)

This version uses a standard ESP32 and an old style CRT Oscilloscope for a real vector screen experience.

See video: https://youtu.be/J523SKyvs14

This github repo is just a cleanup/remove unnused code from: See https://github.com/OzmoOzmo/ESP32TinyVectrex

It is a work in progress - currently no audio and due to limitations of ESP - speed is not 100% of the original Vectrex.
But it is great to see real vector graphics in action.

<center><img src='https://github.com/petersieg/ESP32TinyVectrex/blob/main/previewVectrexLogo.gif'></center>
Modifications from TTGO VGA32 version:
<ul>
 <li>Outputs vector lines via ESP32 Dac outputs</li>
 <li>Removed all VGA libraries to improve performance</li>
 <li>Removed all PS2 keyboard support - now uses esp32 pins for input - see FastDac.h</li>
 <li>Removed all OSD stuff - P.Sieg March 2023</li>
 <li>Removed all WIFI stuff - P.Sieg March 2023</li>
 <li>Unfortunitely no vga means loosing Ackermans menus</li>
</ul>


<h1>Requirements</h1>
Required:
 <ul>
  <li>Any ESP32 board which gives access to both DAC Pin 26 + 25 and other defined Pins in FastDac.h for keypad</li>
  <li>or Arduino IDE 1.8.11 (or better) with ESP32 addon for Arduino IDE from Espressif System Version 1.0.6</li>
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
<li>there are several ROMs included in the code - again see code to select which rom loads at start.</li>
<li>Must be a ESP32 - ESP8266 doesnt have DACs.</li>
<li>its not 100% full speed - todo: moving some code to the second unused core of the ESP32 might help.</li>
</ul>


<h1>Arduino IDE</h1>
The whole project is compatible with the Arduino 1.8.11 framework.
We only have to open the <b>TinyVectrex.ino</b> in the <b>TinyVectrex</b> directory.
<center><img src='https://github.com/petersieg/ESP32TinyVectrex/blob/main/previewArduinoIDEpreferences.gif'></center>
We must install the espressif extensions in the additional card urls manager <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
<br>For video, <b>fabgl 1.0.8 or 1.0.9</b> must be installed.
<center><img src='https://github.com/petersieg/ESP32TinyVectrex/blob/main/previewFabglVersion.gif'></center>
We must deactivate the PSRAM option, and in case of exceeding 1 MB of binary, select 4 MB of partition when uploading. Although the code does not use PSRAM, if the option is active and our ESP32 does not have it, an exception will be generated and it will restart in loop mode.

 
<h1>Test Applications</h1>
The MineStorm game is included:
<center><img src='https://github.com/petersieg/ESP32TinyVectrex/blob/main/previewMineStorm.gif'></center>

ROM/Cartridge choice is at compile time in file osint.cpp:
<pre>
// char *gb_cartfilename=NULL;
unsigned char gb_load_new_rom = 1;
unsigned char gb_id_cur_rom = 3; // rom actual; Default 0 = Minestorm - max_list_rom 6; see file: dataFlash/gbrom.h
//  0 = "rom/gbCartEmpty.h"
//  1 = "rom/gbCartRainy.h"
//  2 = "rom/gbCartOnslaughtElect.h"
//  3 = "rom/gbCartDemo2019party.h"
//  4 = "rom/gbCartDeadline2019.h"
//  5 = "rom/gbCartRaidingparty.h"
//  6 = "rom/gbCartTrex.h"
</pre>
