# Tiny ESP32 Vectrex
Port x86 del emulador Vectrex de John Hawthorn al ESP32.<br>
He realizado varias modificaciones:
<ul>
 <li>Se ha portado al ESP32</li>
 <li>No se usa PSRAM, funcionando en ESP32 de 520 KB de RAM (TTGO VGA32 v1.x)</li> 
 <li>Uso de un OSD de bajos recursos</li>
 <li>Creado proyecto compatible con Arduino IDE y Platform IO</li>
 <li>Modo blanco y negro 1 bpp (2 colores) VGA2Controller</li>
 <li>Soporte de 640x480, 800x600 y 1024x768</li>
 <li>Para video se usa fabgl 1.0.8</li>
 <li>Emulación a la mitad de velocidad, 23 fps en 640x480 y 19 fps en 1024x768</li>
 <li>No hay sonido por el momento</li>
</ul>


<br><br>
<h1>Requerimientos</h1>
Se requiere:
 <ul>
  <li>TTGO VGA32 v1.x (1.0, 1.1, 1.2, 1.4)</li>
  <li>Visual Studio 1.66.1 PLATFORMIO 2.4.3 Espressif32 v3.5.0 (python 3.6)</li>
  <li>Arduino IDE 1.8.11 Espressif System 1.0.6</li>
  <li>Librería reducida Arduino fabgl 1.0.8 (incluida en proyecto PLATFORMIO)</li>
 </ul>
 <center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/ttgovga32v12.jpg'></center>
<br><br>


<h1>PlatformIO</h1>
Se debe instalar el PLATFORMIO 2.4.3 desde las extensiones del Visual Studio. Se requiere también Espressif32 v3.5.0 (python 3.6).
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewPlatformIOinstall.gif'></center>
Luego se seleccionará el directorio de trabajo <b>TinyVectrex</b>.
Debemos modificar el fichero <b>platformio.ini</b> la opción <b>upload_port</b> para seleccionar el puerto COM donde tenemos nuestra placa TTGO VGA32.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewPlatformIO.gif'></center>
Luego procederemos a compilar y subir a la placa. No se usa particiones, así que debemos subir todo el binario compilado.
Está todo preparado para no tener que instalar las librerias de fabgl.


<br><br>
<h1>Arduino IDE</h1>
Todo el proyecto es compatible con la estructura de Arduino 1.8.11.
Tan sólo tenemos que abrir el <b>TinyVectrex.ino</b> del directorio <b>TinyVectrex</b>.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewArduinoIDEpreferences.gif'></center>
Debemos instalar las extensiones de spressif en el gestor de urls adicionales de tarjetas <b>https://dl.espressif.com/dl/package_esp32_index.json</b>
Para video se requiere instalar <b>fabgl 1.0.8</b>.
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewFabglVersion.gif'></center>
Debemos desactivar la opción de PSRAM, y en caso de superar 1 MB de binario, seleccionar 4 MB de partición a la hora de subir. Aunque el código no use PSRAM, si la opción está activa y nuestro ESP32 no dispone de ella, se generará una excepción y reinicio del mismo en modo bucle.


<br><br>
<h1>Usabilidad</h1>
Se permite cargar:
 <ul>
  <li>Cartuchos de 32K</li>  
  <li>Reiniciar</li>
 </ul>
Se dispone de un OSD básico de bajos recursos, es decir, muy simple, que se visualiza pulsando la tecla <b>F1</b>.
 <center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewOSD.gif'></center>
 Los ficheros deben ser convertidos a .h en hexadecimal. Puede usarse la herramienta online:<br>
 <a href='http://tomeko.net/online_tools/file_to_hex.php?lang=en'>http://tomeko.net/online_tools/file_to_hex.php?lang=en</a>


<br><br>
<h1>Opciones</h1>
El archivo <b>gbConfig.h</b> se seleccionan las opciones:
<ul>
 <li><b>use_lib_vga640x480:</b> Modo de video 640x480.</li>
 <li><b>use_lib_vga800x600 </b> Modo de video 800x600.</li>
 <li><b>use_lib_vga1024x768 </b> Modo de video 1024x768.</li>
</ul>
 
