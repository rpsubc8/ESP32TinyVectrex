//Port x86 John Hawthorn to ESP32 TTGO VGA v1.x by ackerman
//No PSRAM (minimal SRAM)
//TTGO VGA32 v1.0, 1.1, 1.2 y 1.4
//19 fps 1024x768 and 23 fps 640x480
//Support 640x480, 800x600 and 1024x768
//VGA2Controller 1 bpp
//Visual Studio 1.66.1 PLATFORMIO 2.4.3 Espressif32 v3.5.0 (python 3.6)
//Arduino IDE 1.8.11 Espressif System 1.0.6
//Video fabgl 1.0.8 (required Arduino IDE)
//WIFI support load cart (http recommended)

#include "gbConfig.h"

#include "fabgl.h"
#include "WiFiGeneric.h" //Para que compile

#include "gbGlobals.h"
#include "osint.h"

#include "PS2Kbd.h"

#ifdef use_lib_wifi
 #include "gbWifi.h"
 #include <WiFi.h>
 //#include <WiFiMulti.h>
 #include <HTTPClient.h>
 #include "gbWifiConfig.h"
 //#include <esp_wifi.h>
 //WiFiMulti wifiMulti;
 HTTPClient http;
 WiFiClient * stream;
 
 unsigned char gb_buffer_wifi[1024]; //128 ficheros * 8
 int gb_size_file_wifi=0; 
 char gb_cadUrl[128]="";
#endif



#ifdef use_lib_gfx
 VGA2DirectControllerPublic VGAController;
#endif

//fabgl::VGA2Controller VGAController;
//fabgl::PS2Controller PS2Controller;


unsigned int gb_stats_time_cur_unified = 0;
unsigned int gb_stats_time_min_unified = 500000;
unsigned int gb_stats_time_max_unified = 0;
unsigned int gb_stats_video_cur_unified = 0;
unsigned int gb_stats_video_min_unified = 500000;
unsigned int gb_stats_video_max_unified = 0;
unsigned long gb_currentTime=0;
unsigned long gb_fps_time_ini_unified= 0;
unsigned long gb_fps_unified= 0;
unsigned long gb_fps_ini_unified= 0;

unsigned int gb_time_vga_before=0;
unsigned int gb_time_now=0;

#ifdef use_lib_remove_fabgl_queue
 uint8_t * gb_buffer_vga[768];
 void PreparaBitluniVGA(void);

 //***************************************
 void PreparaBitluniVGA()
 {
  //1024x768 da 1024x700
  int tope = VGAController.getViewPortHeight();
  for (unsigned int i=0; i<tope; i++)
  {
   gb_buffer_vga[i] = (uint8_t *)VGAController.sgetScanline(i);
  }  
 }
#endif 


void setup()
{
  #ifdef use_lib_log_serial
   Serial.begin(115200);
  #endif 
  //delay(500); //avoid garbage into the UART

  #ifdef use_lib_wifi
   cart= (unsigned char *)malloc(32768); //Ahorro memoria WIFI   
  #endif
  
  #ifdef use_lib_gfx
   VGAController.begin();  
   #ifdef use_lib_vga640x480
    VGAController.setResolution(VGA_640x480_60Hz); //resize (640,480);
   #else 
    #ifdef use_lib_vga800x600
     VGAController.setResolution(SVGA_800x600_60Hz); //resize (800,600);
    #else
     #ifdef use_lib_vga1024x768
      VGAController.setResolution(SVGA_1024x768_60Hz); //resize (1024,768);
     #endif 
    #endif
   #endif  
   //VGAController.setResolution(VGA_320x200_70Hz); //resize (640,480);
   VGAController.enableBackgroundPrimitiveExecution(false);
   VGAController.enableBackgroundPrimitiveTimeout(false);
  #endif
  //VGAController.setResolution(SVGA_800x600_60Hz); //resize (800,600);
  //VGAController.setResolution(SVGA_1024x768_60Hz); //resize (1024,768);

  #ifdef use_lib_remove_fabgl_queue
   PreparaBitluniVGA();
  #endif

  //PS2Controller.begin(PS2Preset::KeyboardPort0, KbdMode::NoVirtualKeys);
  //PS2Controller.begin(PS2Preset::KeyboardPort0_MousePort1, KbdMode::GenerateVirtualKeys);  
//  PS2Controller.begin(PS2Preset::KeyboardPort0, (KbdMode::CreateVirtualKeysQueue));
//  PS2Controller.keyboard()->setLayout(&fabgl::UKLayout);

  kb_begin();

  #ifdef use_lib_wifi
   #ifdef use_lib_log_serial
    Serial.printf("RAM before WIFI %d\n",ESP.getFreeHeap());
   #endif 
   //for(uint8_t t = 4; t > 0; t--) 
   //{
   //   Serial.printf("[SETUP] WAIT %d...\n", t);
   //   Serial.flush();
   //   delay(1000);
   //}
   //wifiMulti.addAP(gb_wifi_ssd, gb_wifi_pass);     

   WiFi.mode(WIFI_STA);
   //esp_wifi_set_ps(WIFI_PS_NONE); //Ahorro energia desactivado
   WiFi.begin(gb_wifi_ssd, gb_wifi_pass);

   for(unsigned char t = 4; t > 0; t--)
   {
    #ifdef use_lib_wifi_debug
     Serial.printf("WIFI WAIT %d...\n", t);
     Serial.flush();
    #endif
    delay(1000);
   }
   //AsignarOSD_WIFI(&wifiMulti, &http, stream);
   //AsignarOSD_WIFI(&http, stream);
   Asignar_WIFI(&http, stream);
   Check_WIFI();
   #ifdef use_lib_wifi_debug
    Serial.print("IP address:");
    Serial.println(WiFi.localIP());
   #endif 
   //AsignarDISK_WIFI(&http, stream);
  #endif  

  #ifdef use_lib_log_serial
   Serial.printf("Width:%d Hi:%d\n",VGAController.getScreenWidth(),VGAController.getScreenHeight());
   Serial.printf("View Width:%d Hi:%d\n",VGAController.getViewPortWidth(),VGAController.getViewPortHeight());
   Serial.printf("END SETUP %d\n", ESP.getFreeHeap());
  #endif 
}


void loop()
{
  //fabgl::VGATimings t;
  mainEmulator();
}
