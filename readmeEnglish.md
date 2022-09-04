# Tiny ESP32 Vectrex
x86 port of John Hawthorn's Vectrex emulator to the ESP32.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewVectrexLogo.gif'></center>
I have made several modifications:
<ul>
 <li>Ported to ESP32</li>
 <li>No PSRAM used, running on ESP32 520 KB RAM (TTGO VGA32 v1.x)</li> 
 <li>Use of a low-resource OSD</li>
 <li>Created project compatible with Arduino IDE and Platform IO</li>
 <li>Black and white mode 1 bpp (2 colors) VGA2Controller</li>
 <li>640x480, 800x600 and 1024x768 support</li>
 <li>For video, fabgl 1.0.8 is used.</li>
 <li>Half speed emulation, 23 fps at 640x480 and 19 fps at 1024x768</li>
 <li>No sound at the moment</li>
 <li>Support for reading cartridges via WIFI</li>
 <li>Precompiled version (flash download 3.9.2) 1024x768</li>  
</ul>


<br><br>
<h1>Precompiled version</h1>
In the precompile folder there is a version already compiled to be saved with the flash download tool 3.9.2. It is a version for the 1024x768 video mode and the FLASH demo games.<br><br>
<a href='https://github.com/rpsubc8/ESP32TinyVectrex/tree/main/ESP32/precompile'>https://github.com/rpsubc8/ESP32TinyVectrex/tree/main/ESP32/precompile</a>
<br><br>
We must choose the ESP32 type:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/flash00.gif'></center>
Subsequently, select the files as shown in the attached screenshot, with the same offset values:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/flash01.gif'></center>
And we will press start. If everything has been correct, we will only have to restart the ESP32.


<br><br>
<h1>Requirements</h1>
Required:
 <ul>
  <li>TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4)</li>
  <li>Visual Studio 1.66.1 PLATFORMIO 2.4.3 Espressif32 v3.5.0 (python 3.6)</li>
  <li>Arduino IDE 1.8.11 Espressif System 1.0.6</li>
  <li>Arduino fabgl 1.0.8 reduced library (included in PLATFORMIO project)</li>
 </ul>
 <center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/ttgovga32v12.jpg'></center>
<br><br>


<h1>PlatformIO</h1>
PLATFORMIO 2.4.3 must be installed from the Visual Studio extensions. Espressif32 v3.5.0 (python 3.6) is also required.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewPlatformIOinstall.gif'></center>
The <b>TinyVectrex</b> working directory is then selected.
We must modify the <b>platformio.ini</b> file the <b>upload_port</b> option to select the COM port where we have our TTGO VGA32 board.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewPlatformIO.gif'></center>
Then we will proceed to compile and upload to the board. No partitions are used, so we must upload the entire compiled binary.
It is all set up so you don't have to install the fabgl libraries.
