# Tiny ESP32 Vectrex
Port x86 del emulador Vectrex de John Hawthorn al ESP32.<br>
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewVectrexLogo.gif'></center>
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
 <li>Soporte para leer cartuchos via WIFI</li>
 <li>Versión precompilada (flash download 3.9.2) 1024x768</li>
</ul>

<br><br>
<h1>Versión precompilada</h1>
En la carpeta precompile se encuentra una versión ya compilada para poder ser grabada con el flash download tool 3.9.2. Se trata de una versión para el modo de video 1024x768 y los juegos de demostración en FLASH.<br><br>
<a href='https://github.com/rpsubc8/ESP32TinyVectrex/tree/main/ESP32/precompile'>https://github.com/rpsubc8/ESP32TinyVectrex/tree/main/ESP32/precompile</a>
<br><br>
Debemos de elegir el tipo ESP32:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/flash00.gif'></center>
Posteriormente, seleccionaremos los archivos tal y como la captura adjunta, con los mismos valores de offset:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/flash01.gif'></center>
Y le daremos a start. Si todo ha sido correcto, sólo tendremos que reiniciar el ESP32.


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
 <a href='http://tomeko.net/online_tools/file_to_hex.php?lang=en'>http://tomeko.net/online_tools/file_to_hex.php?lang=en</a><br>
 Se permite usar:
 <ul>
 <li>Cursores izquierda, derecha, arriba, abajo</li>
 <li>Teclas a, s, d, f</li>
 </ul>


<br><br>
<h1>Opciones</h1>
El archivo <b>gbConfig.h</b> se seleccionan las opciones:
<ul>
 <li><b>use_lib_vga640x480:</b> Modo de video 640x480.</li>
 <li><b>use_lib_vga800x600:</b> Modo de video 800x600.</li>
 <li><b>use_lib_vga1024x768:</b> Modo de video 1024x768.</li>
 <li><b>use_lib_gfx:</b> Deshabilita la salida de video. No muestra nada por pantalla. Usar solo para medir tiempos de emulación.</li>
 <li><b>use_lib_wifi:</b> Activa la WIFI para poder cargar cartuchos desde un servidor web. Al requerir mucha RAM, se aconseja usar un servidor http, en lugar de https. Se debe introducir el nombre de red y clave en el fichero gbWifiConfig.h</li>
</ul>


<br><br>
<h1>Soporte WIFI</h1>
Se ha añadido para TEST un soporte básico de WIFI, para poder cargar los cartuchos (bin) desde un servidor básico HTML, sin necesidad de CORS, por lo que el despliegue es muy rápido. Se puede usar un Servidor Apache, NGINX, etc...<br>
Por defecto, se ha dejado apuntando al servidor local, dado que el pages de github del proyecto requiere https (consumo de RAM), y dará fallo de memoria:
<pre>
https://rpsubc8.github.io/ESP32TinyVectrex/www/vectrex/output
</pre>

 Para activar este modo, se debe descomentar la línea <b>use_lib_wifi</b> en el <b>gbConfig.h</b><br>
 Debemos configurar en el archivo <b>gbWIFIConfig.h</b> los datos:
 <pre>
  #define gb_wifi_ssd "nombreDeNuestraRedWIFIdelRooter"
  #define gb_wifi_pass "passwordDeNuestraRedWIFIdelRooter"

  //#define gb_wifi_url_base_path "http://192.168.0.36/vectrex/output"
  #define gb_wifi_url_base_path "https://rpsubc8.github.io/ESP32TinyVectrex/www/vectrex/output"

  //millisecons delay stream read
  #define gb_wifi_delay_available 0

  #define use_lib_wifi_debug
 </pre>
 
 Por ahora, la configuración está fijada en nuestro <b>gbWIFIConfig.h</b> que tendremos que recompilar, de manera, que sólo se conectará a nuestra red del rooter. Por tanto debemos de cambiar <b>gb_wifi_ssd</b> y <b>gb_wifi_pass</b>.<br>
 El <b>gb_wifi_url_base_path</b> es la ruta en donde se encuentran nuestros directorios <b>outlist</b> y <b>outdat</b>, que contienen el listado de archivos, así como los mismos, por lo que dicho path será diferente si usamos un servidor local.<br><br>
 El concepto es simple, se dispone de:
 <pre>
  outlist --> Fichero con la lista de nombres (longitud 8) de bin. Límite de 128 ficheros
  outdat  --> Los fichero bin.
 </pre>
 Por ahora, para optimizar el consumo de RAM, se ha dejado una estructura de longitud de nombres 8:3, es decir, 8 caracteres de nombre y 3 de extensión. Dejo unas tools intermedias para preparar y hacer la conversión:<br>
 <pre>
  build.bat --> Lanza todos los bats, procesando input en output
  
  data83.bat --> Convierte todos los archivos input a formato 8:3
  
  list.bat --> Genera los outlist (lista de archivos).
  dsk.exe --> Genera un txt que dentro contiene la lista de archivos con longitud de nombre 8.
  lowercart.bat --> Convierte las extensiones BIN a .bin  
 </pre>

 Un ejemplo de <b>outlist</b>, por ejemplo de cart.txt, que contiene:
 <pre>
 ArmorAttBedlam  Berzerk BlitzActCleanBosCleanSweCosmicChDarkToweDEADLINEdemo2019
 </pre>
 
 Siempre que se añada un fichero, debemos de regenerar la lista con el <b>list.bat</b> o bien llamando a todo el proceso <b>build.bat</b>.<br>
 
 Dentro esta la lista de archivos con longitud máxima de 8 caracteres, que es la que se mostrará en el menu de selección de cartucho en el ESP32. Estos archivos, por ahora están pensados para un máximo de 128 entradas, que equivale a 1024 bytes (128 x 8).<br>
 Cada vez que se hace una petición a un tipo, se carga el fichero .TXT con la lista (1024 bytes, 128 nombres). Y cuando se selecciona, se hace la petición al fichero que se encuentra en el outdat.<br>
 Cuando se seleccione un archivo, se procederá a cargarlo en <b>outdat</b> con su ruta. Los archivos tienen que tener la extensión en minúsculas.<br>
 
 Si se usa un servidor externo WEB, es posible, que por políticas impida realizar peticiones seguidas, así que es recomendable no hacer peticiones muy seguidas.<br>
 
 Para depurar la WIFI, se debe descomentar <b>use_lib_wifi_debug</b> en el fichero <b>gbWifiConfig.h</b>




<br><br>
<h1>Aplicaciones Test</h1>
Se incluye el juego MineStorm:
<center><img src='https://raw.githubusercontent.com/rpsubc8/ESP32TinyVectrex/main/preview/previewMineStorm.gif'></center>
<ul>
 <li>rainy (demoscene)</li>
 <li>OnslaughtElect (demoscene)</li>
 <li>demo 2019 party (demoscene)</li>
 <li>deadline 2019 (demoscene)</li>
 <li>raiding party (demoscene)</li>
 <li>trex</li> 
</ul>
