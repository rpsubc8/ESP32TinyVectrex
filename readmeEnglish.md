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


<br><br>
<h1>Arduino IDE</h1>
The whole project is compatible with the Arduino 1.8.11 framework.
We only have to open the <b>TinyVectrex.ino</b> in the <b>TinyVectrex</b> directory.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewArduinoIDEpreferences.gif'></center>
We must install the spressif extensions in the additional card urls manager <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
For video, <b>fabgl 1.0.8</b> must be installed.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewFabglVersion.gif'></center>
We must deactivate the PSRAM option, and in case of exceeding 1 MB of binary, select 4 MB of partition when uploading. Although the code does not use PSRAM, if the option is active and our ESP32 does not have it, an exception will be generated and it will restart in loop mode.


<br><br>
<h1>Usability</h1>
Loading is allowed:
 <ul>
  <li>32K cartridges</li>  
  <li>Restart</li>
 </ul>
A basic low-resource OSD is available, i.e. very simple, which is displayed by pressing the <b>F1</b> key.
 <center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewOSD.gif'></center>
 The files must be converted to .h in hexadecimal. The online tool can be used:<br>
 <a href='http://tomeko.net/online_tools/file_to_hex.php?lang=en'>http://tomeko.net/online_tools/file_to_hex.php?lang=en</a><br>
 Allowed to use:
 <ul>
 <li>Cursors left, right, up, down</li>
 <li>Keys a, s, d, f</li>
 </ul>
 
 
<br><br>
<h1>Options</h1>
The <b>gbConfig.h</b> file selects the options:
<ul>
 <li><b>use_lib_vga640x480:</b> 640x480 video mode.</li>
 <li><b>use_lib_vga800x600:</b> 800x600 video mode.</li>
 <li><b>use_lib_vga1024x768:</b> 1024x768 video mode.</li>
 <li><b>use_lib_gfx:</b> Disables video output. Displays nothing on the screen. Use only to measure emulation times.</li>
 <li><b>use_lib_wifi:</b> Enable WIFI to be able to load cartridges from a web server. As it requires a lot of RAM, it is advisable to use an http server, instead of https. The network name and password must be entered in the gbWifiConfig.h file.</li>
</ul>



<br><br>
<h1>WIFI support</h1>
A basic WIFI support has been added for TEST, to be able to load the cartridges (bin) from a basic HTML server, without the need of CORS, so the deployment is very fast. Apache Server, NGINX, etc... can be used.<br>
By default, it has been left pointing to the local server, since the github pages of the project requires https (RAM consumption), and will give memory failure:
<pre>
https://rpsubc8.github.io/ESP32TinyVectrex/www/vectrex/output
</pre>

To activate this mode, uncomment the line <b>use_lib_wifi</b> in <b>gbConfig.h</b><br>
 We must configure in the file <b>gbWIFIConfig.h</b> the data:
 <pre>
  #define gb_wifi_ssd "nombreDeNuestraRedWIFIdelRooter"
  #define gb_wifi_pass "passwordDeNuestraRedWIFIdelRooter"

  //#define gb_wifi_url_base_path "http://192.168.0.36/vectrex/output"
  #define gb_wifi_url_base_path "https://rpsubc8.github.io/ESP32TinyVectrex/www/vectrex/output"

  //millisecons delay stream read
  #define gb_wifi_delay_available 0

  #define use_lib_wifi_debug
 </pre>
 
 For now, the configuration is fixed in our <b>gbWIFIConfig.h</b> that we will have to recompile, so that it will only connect to our rooter network. Therefore we must change <b>gb_wifi_ssd</b> and <b>gb_wifi_pass</b>.<br>
 The <b>gb_wifi_url_base_path</b> is the path where our <b>outlist</b> and <b>outdat</b> directories are located, which contain the list of files, as well as the files themselves, so this path will be different if we use a local server.<br><br>
 The concept is simple, you have:
 <pre>
  outlist --> File with the list of bin names (length 8). Limit of 128 files
  outdat  --> The bin files.
 </pre>
 For now, to optimize RAM consumption, we have left an 8:3 name length structure, i.e. 8 name characters and 3 extension characters. I leave some intermediate tools to prepare and make the conversion:<br>
 <pre>
  build.bat --> Launches all bats, processing input to output
  
  data83.bat --> Converts all input files to 8:3 format
  
  list.bat --> Generates the outlist (list of files).
  dsk.exe --> Generates a txt file containing a list of files with name length 8.
  lowercart.bat --> Convert BIN to .bin extensions
 </pre>

 An example <b>outlist</b>, for example from cart.txt, containing:
 <pre>
 ArmorAttBedlam  Berzerk BlitzActCleanBosCleanSweCosmicChDarkToweDEADLINEdemo2019
 </pre>
 
 Whenever a file is added, we must regenerate the list with the <b>list.bat</b> or by calling the whole <b>build.bat</b> process.<br>
 
 Inside is the list of files with a maximum length of 8 characters, which is what will be displayed in the cartridge selection menu in the ESP32. These files, for now, are intended for a maximum of 128 entries, which is equivalent to 1024 bytes (128 x 8).<br>
 Each time a request is made to a type, the .TXT file with the list (1024 bytes, 128 names) is loaded. And when it is selected, the request is made to the file in the outdat.<br>
 When a file is selected, it will be loaded into <b>outdat</b> with its path. The files must have the extension in lower case.<br>
 
 If you are using an external WEB server, it is possible that policies may prevent you from making consecutive requests, so it is advisable not to make requests too close together.<br>
 
 To debug the WIFI, uncomment <b>use_lib_wifi_debug</b> in the <b>gbWifiConfig.h</b> file.
 
<br><br>
<h1>Test Applications</h1>
The MineStorm game is included:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewMineStorm.gif'></center>
<ul>
 <li>rainy (demoscene)</li>
 <li>OnslaughtElect (demoscene)</li>
 <li>demo 2019 party (demoscene)</li>
 <li>deadline 2019 (demoscene)</li>
 <li>raiding party (demoscene)</li>
 <li>trex</li> 
</ul>
