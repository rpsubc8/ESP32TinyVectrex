#include "gbConfig.h"
#include "osd.h"
#ifndef use_lib_wifi
 #include "dataFlash/gbrom.h"
#endif 
#include "gbGlobals.h"
#include "fabgl.h"
#include "PS2Kbd.h"

#ifdef use_lib_wifi
 #include "gbWifiConfig.h"
 #include "gbWifi.h" 
#endif

#define BLACK   0
#define BLUE    4
#define RED     1
#define MAGENTA 5
#define GREEN   2
#define CYAN    6
#define YELLOW  3
#define WHITE   15



//extern int gb_screen_xIni;
//extern int gb_screen_yIni;
//extern unsigned char gb_cache_zxcolor[8];
//JJ static SDL_Surface * gb_osd_sdl_surface;
//JJ static SDL_Event * gb_osd_sdl_event;
//static SDL_Color * gb_osd_sdl_palette;
unsigned char gb_show_osd_main_menu=0;

//extern SDL_Surface * gb_screen;
//extern SDL_Event gb_event;

//JJ Uint32 gbSDLVideoDelay = 20;

//**********************************
//JJ SDL_Surface * SDLOSDGetSurface()
//JJ {
//JJ  return gb_osd_sdl_surface;
//JJ }


//JJ void SDLputpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
//JJ {
//JJ  if (x >= (surface->w) || y >= (surface->h))
//JJ   return;
//JJ  Uint8* p = (Uint8*)surface->pixels + (y * surface->pitch) + x;
//JJ  *p= pixel;
//JJ }

//*************************************************************************************
//JJ void SDLprintChar(SDL_Surface *surface,char car,int x,int y,unsigned char color,unsigned char backcolor,unsigned char isDouble)
//JJ { 
//JJ // unsigned char aux = gb_sdl_font_6x8[(car-64)];
//JJ  int auxId = car << 3; //*8
//JJ  unsigned char aux;
//JJ  unsigned char auxBit,auxColor;
//JJ  for (unsigned char j=0;j<8;j++)
//JJ  {
//JJ   aux = gb_sdl_font_6x8[auxId + j];
//JJ   for (int i=0;i<8;i++)
//JJ   {
//JJ    auxColor= ((aux>>i) & 0x01);
//JJ    SDLputpixel(surface,x+(6-i),y+j,(auxColor==1)?color:backcolor);
//JJ   }
//JJ  }
//JJ }

//*****************************************************************************************
//JJ void SDLprintText(SDL_Surface *surface,const char *cad,int x, int y, unsigned char color,unsigned char backcolor,unsigned char isDouble)
//JJ {
//JJ //SDL_Surface *surface,
//JJ // gb_sdl_font_6x8
//JJ  int auxLen= strlen(cad);
//JJ  if ((auxLen>50)&&(isDouble==1))
//JJ   auxLen=50;
//JJ  for (int i=0;i<auxLen;i++)
//JJ  {
//JJ   SDLprintChar(surface,cad[i],x,y,color,backcolor,isDouble);
//JJ   x+=7;
//JJ  }
//JJ }

//**********************************
//void SDLOSDClear()
//{
// SDLClear(gb_osd_sdl_surface);
//}

//JJ void SDLClear(SDL_Surface *auxSurface)
//JJ {
//JJ  for (int y=0; y<(auxSurface->w); y++)
//JJ   for (int x=0; x<(auxSurface->h); x++)
//JJ    SDLputpixel(auxSurface,x,y,0);
//JJ }

//Asignamos el Surface global
//void SDLAssignSDLSurfaceOSD(SDL_Surface *surface,SDL_Event * ptrEvent,SDL_Color *ptrPalette)
//JJ void SDLAssignSDLSurfaceOSD(SDL_Surface *surface,SDL_Event * ptrEvent)
//JJ {
//JJ  gb_osd_sdl_surface = surface;
//JJ  gb_osd_sdl_event = ptrEvent;
//JJ  //gb_osd_sdl_palette = ptrPalette;
//JJ }






#define max_gb_osd_screen 1
const char * gb_osd_screen[max_gb_osd_screen]={
 "Pixels Left"//,
 //"Pixels Top",
 //"Color 8",
 //"Mono Blue 8",
 //"Mono Green 8",
 //"Mono Red 8",
 //"Mono Grey 8"
};

#define max_gb_osd_screen_values 5
const char * gb_osd_screen_values[max_gb_osd_screen_values]={
 "0",
 "2",
 "4", 
 "8", 
 "16"
};


//#define max_gb_main_menu 8
#define max_gb_main_menu 3
#ifdef use_lib_wifi
 const char * gb_main_menu[max_gb_main_menu]={
  "Load CARTDRIDGE WIFI",
  "Reset",
  //"Speed",
  //"Screen Adjust",
  //"Reset",
  //"Test 8",
  //"Test 64",
  //"Dump DSK",
  "Return"
 };
#else
 const char * gb_main_menu[max_gb_main_menu]={
  "Load CARTDRIDGE",
  "Reset",
  "Return"
 };
#endif 


#define max_gb_speed_sound_menu 7
const char * gb_speed_sound_menu[max_gb_speed_sound_menu]={
 "0",
 "1",
 "2",
 "4",
 "8",
 "16",
 "32"
};

#define max_gb_value_binary_menu 2
const char * gb_value_binary_menu[max_gb_value_binary_menu]={
 "0",
 "1"
};


#define max_gb_speed_videoaudio_options_menu 4
const char * gb_speed_videoaudio_options_menu[max_gb_speed_videoaudio_options_menu]={
 "Audio poll",
 "Video delay",
 "Skip Frame",
 "Keyboard poll"
};


#define max_gb_reset_menu 2
const char * gb_reset_menu[max_gb_reset_menu]={
 "Soft",
 "Hard"
};

#define max_gb_speed_menu 5
const char * gb_speed_menu[max_gb_speed_menu]={
 "Normal",
 "2x",
 "4x",
 "8x",
 "16x"
};




//****************************
void SDLEsperaTeclado()
{
 //JJ SDL_WaitEvent(gb_osd_sdl_event);
}

//#define gb_pos_x_menu 50
//#define gb_pos_y_menu 20
#define gb_pos_x_menu 120
#define gb_pos_y_menu 80
#define gb_osd_max_rows 10

void OSDMenuRowsDisplayScroll(const char **ptrValue,unsigned char currentId,unsigned char aMax)
{//Dibuja varias lineas
//Portar for (int i=0;i<gb_osd_max_rows;i++)
//Portar  SDLprintText(gb_osd_sdl_surface,"                    ",gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),BLACK,BLACK,1);
#ifdef use_lib_gfx
 Canvas cv(&VGAController); 
 cv.setGlyphOptions(GlyphOptions().FillBackground(true));
 
 cv.setPenColor(Color::Black);
 cv.setBrushColor(Color::Black);    
 for (int i=0;i<gb_osd_max_rows;i++){
  cv.drawText(gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),"                    ");
 }

 for (int i=0;i<gb_osd_max_rows;i++)
 {
  if (currentId >= aMax)
   break;
//Portar  SDLprintText(gb_osd_sdl_surface,ptrValue[currentId],gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?CYAN:WHITE),((i==0)?BLUE:BLACK),1);  
  if (i==0){
   cv.setPenColor(Color::Black);
   cv.setBrushColor(Color::White);
  }
  else{
   cv.setPenColor(Color::White);
   cv.setBrushColor(Color::Black);    
  }

  cv.drawText(gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),ptrValue[currentId]);
  currentId++;
 }     
#endif
}

//Maximo 256 elementos
unsigned char ShowTinyMenu(const char *cadTitle,const char **ptrValue,unsigned char aMax)
{ 
#ifdef use_lib_gfx  
 Canvas cv(&VGAController);
// auto keyboard = PS2Controller.keyboard();
 unsigned char aReturn=0;
 unsigned char salir=0;
//Portar SDLClear(gb_osd_sdl_surface);  
 cv.setGlyphOptions(GlyphOptions().FillBackground(true));
 cv.setPenColor(Color::Black);
 cv.setBrushColor(Color::Black); 
 cv.clear();
 cv.selectFont(&fabgl::FONT_8x8);
 cv.setPenColor(Color::White);
 cv.setBrushColor(Color::Black);
 cv.drawText(gb_pos_x_menu-(4<<3), gb_pos_y_menu-16,"ESP32 VECTREX by Ackerman");
 cv.setPenColor(Color::White);
 cv.setBrushColor(Color::Black);
 for (int i=0;i<20;i++){
  cv.drawText(gb_pos_x_menu+(i<<3),gb_pos_y_menu," ");
 }
 cv.setPenColor(Color::Black);
 cv.setBrushColor(Color::White); 
 cv.drawText(gb_pos_x_menu,gb_pos_y_menu,cadTitle);
 cv.setPenColor(Color::White);
 cv.setBrushColor(Color::Black); 

//Portar SDLprintText(gb_osd_sdl_surface,cadTitle,gb_pos_x_menu,gb_pos_y_menu,BLACK,WHITE,1); 
 
//Portar SDLprintText(gb_osd_sdl_surface,"Port VECTREX by Ackerman",gb_pos_x_menu-(4<<3),gb_pos_y_menu-16,WHITE,BLACK,1);
//Portar for (int i=0;i<20;i++)
//Portar  SDLprintChar(gb_osd_sdl_surface,' ',gb_pos_x_menu+(i<<3),gb_pos_y_menu,BLACK,WHITE,1);
//Portar SDLprintText(gb_osd_sdl_surface,cadTitle,gb_pos_x_menu,gb_pos_y_menu,BLACK,WHITE,1);
 
 OSDMenuRowsDisplayScroll(ptrValue,0,aMax);
// for (int i=0;i<aMax;i++)
// {
//  SDLprintText(gb_osd_sdl_surface,ptrValue[i],gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?CYAN:WHITE),((i==0)?BLUE:BLACK),1);  
// }
 
 
 while (salir == 0)
 {            
  //case SDLK_UP:
  if (checkAndCleanKey(KEY_CURSOR_UP))
  {
   if (aReturn>0) aReturn--;
   OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
  }
  if (checkAndCleanKey(KEY_CURSOR_DOWN))
  {
   if (aReturn < (aMax-1)) aReturn++;
   OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
  }
  if (checkAndCleanKey(KEY_ENTER))
  {
   salir= 1;
  }     
  if (checkAndCleanKey(KEY_ESC))
  {
   salir=1; aReturn= 255;    
  }                
 } 
 //gb_show_osd_main_menu= 0;
 //return aReturn;

 gb_show_osd_main_menu= 0;
 return aReturn;
 #else
  return 0;
 #endif
}


#ifdef use_lib_wifi
 //***************************
 void ShowStatusWIFI(unsigned char aState)
 {
  #ifdef use_lib_gfx
   Canvas cv(&VGAController);
   cv.setGlyphOptions(GlyphOptions().FillBackground(true));
   if (aState == 1)
   {
    //SDLprintText("WIFI LOAD",8,8,ID_COLOR_BLACK,ID_COLOR_WHITE);
    cv.setPenColor(Color::Black);
    cv.setBrushColor(Color::White);
    cv.drawText(8,8,"WIFI LOAD");
   }
   else
   {  
    //SDLprintText("         ",8,8,ID_COLOR_BLACK,ID_COLOR_BLACK);   
    cv.setPenColor(Color::Black);
    cv.setBrushColor(Color::Black);   
    cv.drawText(8,8,"         ");
   }
  #endif 
 }

 //**********************************************************************************
 void OSDMenuRowsDisplayScrollFromWIFI(unsigned char *ptrBuffer,unsigned char currentId,unsigned char aMax) 
 {//Dibuja varias lineas 
  #ifdef use_lib_gfx
   Canvas cv(&VGAController); 
   cv.setGlyphOptions(GlyphOptions().FillBackground(true));
 
   cv.setPenColor(Color::Black);
   cv.setBrushColor(Color::Black);    

   char cadName8[10];
   cadName8[8]='\0';
   for (int i=0;i<gb_osd_max_rows;i++)
   {
    //SDLprintText("                    ",gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),ID_COLOR_BLACK,ID_COLOR_BLACK);
    cv.drawText(gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),"                    ");
   }
 
   for (int i=0;i<gb_osd_max_rows;i++)
   {
    if (currentId >= aMax){
     break;
    }
    memcpy(cadName8, &ptrBuffer[(currentId*8)], 8);
    cadName8[8]='\0';
    //SDLprintText(ptrValue[currentId],gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?ID_COLOR_WHITE:ID_COLOR_WHITE),((i==0)?ID_COLOR_MAGENTA:ID_COLOR_BLACK));
    //SDLprintText((const char*)cadName8,gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),((i==0)?ID_COLOR_WHITE:ID_COLOR_WHITE),((i==0)?ID_COLOR_MAGENTA:ID_COLOR_BLACK));
    if (i==0){
     cv.setPenColor(Color::Black);
     cv.setBrushColor(Color::White);
    }
    else{
     cv.setPenColor(Color::White);
     cv.setBrushColor(Color::Black);    
    }
    cv.drawText(gb_pos_x_menu,gb_pos_y_menu+8+(i<<3),(const char*)cadName8);
    currentId++;
   }     
  #endif 
 }


 //**********************************************************************************
 unsigned char ShowTinyMenuFromWIFI(const char *cadTitle,unsigned char *ptrBuffer,unsigned char aMax)
 {
  unsigned char aReturn=0;
  unsigned char salir=0;   
  #ifdef use_lib_gfx
   Canvas cv(&VGAController);

   cv.setGlyphOptions(GlyphOptions().FillBackground(true));
   cv.setPenColor(Color::Black);
   cv.setBrushColor(Color::Black); 
   cv.clear();
   cv.selectFont(&fabgl::FONT_8x8);
   cv.setPenColor(Color::White);
   cv.setBrushColor(Color::Black);
   cv.drawText(gb_pos_x_menu-(4<<3), gb_pos_y_menu-16,"ESP32 VECTREX by Ackerman");
   cv.setPenColor(Color::White);
   cv.setBrushColor(Color::Black);
   for (int i=0;i<20;i++){
    cv.drawText(gb_pos_x_menu+(i<<3),gb_pos_y_menu," ");
   }
   cv.setPenColor(Color::Black);
   cv.setBrushColor(Color::White); 
   cv.drawText(gb_pos_x_menu,gb_pos_y_menu,cadTitle);
   cv.setPenColor(Color::White);
   cv.setBrushColor(Color::Black);   

   
   OSDMenuRowsDisplayScrollFromWIFI(ptrBuffer,0,aMax);


  while (salir == 0)
  {  
   if (checkAndCleanKey(KEY_CURSOR_LEFT))
   {
    if (aReturn>10) aReturn-=10;
    OSDMenuRowsDisplayScrollFromWIFI(ptrBuffer,aReturn,aMax);
   }
   if (checkAndCleanKey(KEY_CURSOR_RIGHT))
   {
    if (aReturn<(aMax-10)) aReturn+=10;
    OSDMenuRowsDisplayScrollFromWIFI(ptrBuffer,aReturn,aMax);
   }
   if (checkAndCleanKey(KEY_CURSOR_UP))
   {
    if (aReturn>0) aReturn--;
    OSDMenuRowsDisplayScrollFromWIFI(ptrBuffer,aReturn,aMax);
   }
   if (checkAndCleanKey(KEY_CURSOR_DOWN))
   {
    if (aReturn < (aMax-1)) aReturn++;
    OSDMenuRowsDisplayScrollFromWIFI(ptrBuffer,aReturn,aMax);
   } 
   if (checkAndCleanKey(KEY_ENTER))
   {
    salir= 1;
   }
   if (checkAndCleanKey(KEY_ESC))
   {
    salir=1; aReturn= 255;
   }
  }
  gb_show_osd_main_menu= 0;

  #endif
  return aReturn;
 } 
#endif




//Sobra
     /* SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((contY+1)<<3),WHITE,BLACK,1);
      if (aReturn>0)
      {
       aReturn--;
       contY--;
      }
      if ((aReturn % gb_osd_max_rows == 0))
      {
       OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
       contY=gb_osd_max_rows;
      } 
      else
       SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((contY+1)<<3),CYAN,BLUE,1);
       */


/*      SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((contY+1)<<3),WHITE,BLACK,1);
      if (aReturn < (aMax-1))
      {
       aReturn++;
       contY++;
      }
      if ((aReturn!=0)&&(aReturn % gb_osd_max_rows == 0))
      {
       OSDMenuRowsDisplayScroll(ptrValue,aReturn,aMax);
       contY=0;
      }
      else 
       SDLprintText(gb_osd_sdl_surface,ptrValue[aReturn],gb_pos_x_menu,gb_pos_y_menu+((contY+1)<<3),CYAN,BLUE,1);
       */


//Menu SNA
void ShowTinyD64Menu()
{
/* unsigned char aSelNum;     
 aSelNum = ShowTinyMenu("Machine DSK",gb_machine_menu,max_gb_machine_menu);
 if (aSelNum == 0)
 {
  aSelNum = ShowTinyMenu("464 DSK",gb_list_dsk_title,max_list_dsk);  
  loaddsk2Flash(aSelNum);
 }
 else
 {
  aSelNum = ShowTinyMenu("6128 DSK",gb_list_dsk_title,max_list_dsk);                
  loaddsk2Flash(aSelNum);  
 }   */      
}


void ShowTinyROMMenu()
{
 unsigned char aSelNum;     
 #ifdef use_lib_wifi
  unsigned char aSelType;
  //unsigned char aSelNum;
  unsigned char tope;
  int leidos=0;
  //char cadUrl[256];
  if (Check_WIFI() == true)
  {
   //PreparaURL(cadUrl, "/outlist/cart", "", (char*)"cart", "txt");
   PreparaURL(gb_cadUrl, "/outlist/cart", "", (char*)"cart", "txt");   
   #ifdef use_lib_wifi_debug
    //Serial.printf("URL:%s\n",cadUrl);    
    Serial.printf("URL:%s\n",gb_cadUrl);    
   #endif
   ShowStatusWIFI(1);
   //Asignar_URL_stream_WIFI(cadUrl);
   Asignar_URL_stream_WIFI(gb_cadUrl);
   Leer_url_stream_WIFI(&leidos);
   ShowStatusWIFI(0);
   #ifdef use_lib_wifi_debug
    Serial.printf("Leidos:%d\n",leidos);
   #endif
   tope = gb_size_file_wifi>>3; //DIV 8
   #ifdef use_lib_wifi_debug
    Serial.printf("Tope:%d\n",tope);
   #endif
   if (tope<1){
    return;
   }
   if (tope>127)
   {
    tope= 127;
   }
   aSelNum = ShowTinyMenuFromWIFI("CARTDRIDGE WIFI",gb_buffer_wifi,tope);
   if (aSelNum == 255){
    return;
   }
   char cadFile[10];
   for (int i=0;i<8;i++)
   {
    cadFile[i]= gb_buffer_wifi[(aSelNum*8)+i];
    if (cadFile[i] ==' '){
     cadFile[i]='\0';
    }
   }
   cadFile[8]='\0';
   #ifdef use_lib_wifi_debug
    Serial.printf("Select:%d\n",aSelNum);
   #endif
   //PreparaURL(cadUrl, "/outdat/cart/", "", cadFile,"bin");
   //PreparaURL(gb_cadUrl, "/outdat/cart/", "", cadFile,"bin");
   PreparaURL(gb_cadUrl, "/outdat/cart", "", cadFile,"bin");
   #ifdef use_lib_wifi_debug
    //Serial.printf("URL:%s\n",cadUrl);    
    Serial.printf("URL:%s\n",gb_cadUrl);    
   #endif 
   ShowStatusWIFI(1);   
   //WIFI changeSna2FlashFromWIFI(cadUrl,1);
   //strcpy(gb_cadUrl,cadUrl);
   ShowStatusWIFI(0);
  }  
 #else
  aSelNum = ShowTinyMenu("CARTDRIDGE",gb_list_rom_title,max_list_rom);
 #endif 

 //gb_cartfilename= (char *)gb_list_rom_title[aSelNum];
 gb_load_new_rom = 1;
 gb_id_cur_rom = aSelNum;
 //running= 0;
}

//Menu ROM
void ShowTinyMachineMenu()
{
/* //unsigned char aSelNum;
 model = ShowTinyMenu("Machine",gb_machine_menu,max_gb_machine_menu);
 loadroms2FlashModel();
 //vTaskDelay(2);
 resetz80();
 resetcrtc();
 SDL_Delay(2);*/
}


//Menu resetear
void ShowTinyResetMenu()
{
 unsigned char aSelNum;
 aSelNum= ShowTinyMenu("Reset",gb_reset_menu,max_gb_reset_menu);
 if (aSelNum == 1)
 {
  ESP.restart();
 }
 else
 {
   //gb_reset= 1;
   gb_load_new_rom = 1;
   gb_id_cur_rom = 0;
 } 
}


//Menu TAPE
/*void ShowTinyTAPEMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("48K TAPE",gb_list_tapes_48k_title,max_list_tape_48);
 load_tape2Flash(aSelNum);
}

//Play sound tape
void ShowTinySelectTAPEMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("48K TAPE",gb_list_tapes_48k_title,max_list_tape_48);
 Z80EmuSelectTape(aSelNum);
// gb_play_tape_current = aSelNum;
// gb_play_tape_on_now = 1;
}

//Menu SCREEN
void ShowTinySCRMenu()
{
 unsigned char aSelNum;
 aSelNum = ShowTinyMenu("48K SCREEN",gb_list_scr_48k_title,max_list_scr_48);
 load_scr2Flash(aSelNum);     
}
*/

//Menu velocidad emulador
void ShowTinySpeedMenu()
{
 unsigned char aSelNum,aSelNumSpeed;
 aSelNum = ShowTinyMenu("SPEED VIDEO AUDIO",gb_speed_videoaudio_options_menu,max_gb_speed_videoaudio_options_menu);
 if (aSelNum == 255)
  return;
 switch (aSelNum)
 {
  case 0: aSelNumSpeed= ShowTinyMenu("AUDIO Poll ms",gb_speed_sound_menu,max_gb_speed_sound_menu);
   if (aSelNumSpeed == 255)
    return;
//   gb_current_ms_poll_sound= (aSelNumSpeed<<1); //Multiplico x2
   break;
  case 1: aSelNumSpeed= ShowTinyMenu("Video DELAY ms",gb_speed_sound_menu,max_gb_speed_sound_menu);
   if (aSelNumSpeed == 255)
    return;
//   gb_current_delay_emulate_ms = (aSelNumSpeed<<1);
   break;
//  case 2: gb_current_frame_crt_skip = ShowTinyMenu("Skip Frame",gb_value_binary_menu,max_gb_value_binary_menu);
//   break;
  case 3: aSelNumSpeed= ShowTinyMenu("Keyboard Poll ms",gb_speed_sound_menu,max_gb_speed_sound_menu);
   if (aSelNumSpeed == 255)
    return;
//   gb_current_ms_poll_keyboard= (aSelNumSpeed<<1);
   break;   
  default: break;
 }
}


//***********************************************
void OSD_PreparaPaleta64para8Colores()
{//8 tonalidades de color en Modo 64 colores
/* for(int i=0;i<64;i++)
 {
  gb_osd_sdl_palette[i].r=((i>>4)&0x03)*85;
  gb_osd_sdl_palette[i].g=((i>>2)&0x03)*85;
  gb_osd_sdl_palette[i].b=(i & 0x03)*85;
 } */ 
/* SDL_Color * auxPal= SDLCRTCGetPalette();
 for (int j=0;j<4;j++)
 {
  for (int i=0;i<64;i++)
  {
   auxPal[i+(j*64)].b = ((i>>4)&0x03)*85;
   auxPal[i+(j*64)].g = ((i>>2)&0x03)*85;
   auxPal[i+(j*64)].r = (i & 0x03)*85;
  }
 }*/
}

//Test de 64 colores
void OSD_Test64Colores()
{
/* SDL_Color * auxPal = SDLCRTCGetPalette();
 // OSD_PreparaPaleta64para8Colores();
 //SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, auxPal, 0, 256);
 for (int i=0;i<64;i++)
 {
  SDLprintText(gb_osd_sdl_surface," ",(i*2),30,i,i,1);  
 }
 SDL_Flip(gb_osd_sdl_surface);
 SDL_Delay(4000);
 //SDL_PollEvent(gb_osd_sdl_event); 
 //SDLEsperaTeclado();*/
}


//***********************************************
void OSD_PreparaPaleta8Colores()
{
/*  SDL_Color * auxPal= SDLCRTCGetPalette();
  for (int i=0;i<256;i+=8)
  {
    auxPal[0+i].b = 0; auxPal[0+i].g = 0; auxPal[0+i].r= 0;
	auxPal[1+i].b = 0; auxPal[1+i].g = 0; auxPal[1+i].r= 255;
	auxPal[2+i].b = 0; auxPal[2+i].g = 255; auxPal[2+i].r= 0;
    auxPal[3+i].b = 0; auxPal[3+i].g = 255; auxPal[3+i].r= 255;
	auxPal[4+i].b = 255; auxPal[4+i].g = 0; auxPal[4+i].r= 0;
    auxPal[5+i].b = 255; auxPal[5+i].g = 0; auxPal[5+i].r= 255;
    auxPal[6+i].b = 255; auxPal[6+i].g = 255; auxPal[6+i].r= 0;
    auxPal[7+i].b = 255; auxPal[7+i].g = 255; auxPal[7+i].r= 255;
  }*/
}

//Test de 8 colores
void OSD_Test8Colores()
{
/* SDL_Color * auxPal= SDLCRTCGetPalette();     
 //OSD_PreparaPaleta8Colores();
 //SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, auxPal, 0, 256);
 for (int i=0;i<8;i++)
 {
  SDLprintText(gb_osd_sdl_surface," ",(i*2),30,i,i,1);  
 }
 SDL_Flip(gb_osd_sdl_surface);
 SDL_Delay(4000);
 //SDL_PollEvent(gb_osd_sdl_event); 
 //SDLEsperaTeclado();*/
}

//Ajustar pantalla
void ShowTinyScreenAdjustMenu()
{
 unsigned char aSelNum, auxCol; 
 aSelNum= ShowTinyMenu("Screen Adjust",gb_osd_screen,max_gb_osd_screen);
 switch (aSelNum)
 {
  case 2:
   //OSD_PreparaPaleta8Colores();
   OSD_PreparaPaleta64para8Colores();
   //memcpy(gb_cache_zxcolor,gb_const_colornormal8,8);
   #ifdef use_lib_use_bright
    //memcpy(gb_cache_zxcolor_bright,gb_const_colornormal8_bright,8);
   #endif
   //SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
   break;
  case 3:
   OSD_PreparaPaleta64para8Colores();
   //memcpy(gb_cache_zxcolor,gb_const_monoBlue8,8);
   #ifdef use_lib_use_bright
    //memcpy(gb_cache_zxcolor_bright,gb_const_monoBlue8_bright,8);
   #endif 
   //SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   //for (int k=0;k<256;k++)
   // printf("%d %d %d\n",gb_osd_sdl_palette[k].r,gb_osd_sdl_palette[k].g,gb_osd_sdl_palette[k].b);
   //for (int k=0;k<8;k++)
   // printf("%d ",gb_cache_zxcolor[k]);
   //printf("\n");    
   return;
   break;
  case 4:
   OSD_PreparaPaleta64para8Colores();   
   //memcpy(gb_cache_zxcolor,gb_const_monoGreen8,8);
   #ifdef use_lib_use_bright
    //memcpy(gb_cache_zxcolor_bright,gb_const_monoGreen8_bright,8);
   #endif 
   //SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
   break;
  case 5:
   OSD_PreparaPaleta64para8Colores();   
   //memcpy(gb_cache_zxcolor,gb_const_monoRed8,8);
   #ifdef use_lib_use_bright
    //memcpy(gb_cache_zxcolor_bright,gb_const_monoRed8_bright,8);
   #endif 
   //SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
  case 6:
   OSD_PreparaPaleta64para8Colores();   
   //memcpy(gb_cache_zxcolor,gb_const_grey8,8);
   #ifdef use_lib_use_bright
    //memcpy(gb_cache_zxcolor_bright,gb_const_grey8_bright,8);
   #endif 
   //SDL_SetPalette(gb_osd_sdl_surface, SDL_LOGPAL|SDL_PHYSPAL, gb_osd_sdl_palette, 0, 256);
   return;
   break;          
 }
 auxCol= ShowTinyMenu("Pixels",gb_osd_screen_values,max_gb_osd_screen_values);
 auxCol = auxCol<<1; //x2
// gb_screen_xOffset = auxCol;
// switch (aSelNum)
// {
//  case 0: //gb_screen_xIni = -(auxCol<<3); 
//   break;
//  case 1: //gb_screen_yIni = (auxCol<=3)? (-auxCol):(-3); 
//   break;  
//  default: break;
// }
}


//*******************************************
void SDLActivarOSDMainMenu()
{     
 gb_show_osd_main_menu= 1;   
}

//Very small tiny osd
void do_tinyOSD() 
{
#ifdef use_lib_gfx  
 unsigned char aSelNum;
 if (gb_show_osd_main_menu == 1)
 {
  aSelNum = ShowTinyMenu("MAIN MENU",gb_main_menu,max_gb_main_menu);
  switch (aSelNum)
  {   
   case 0: ShowTinyROMMenu(); break;
   case 1: ShowTinyResetMenu(); break;
   //case 1: ShowTinySpeedMenu(); break;
   //case 2: ShowTinyScreenAdjustMenu(); break;  
   //case 4: OSD_Test8Colores(); break;
   //case 5: OSD_Test64Colores(); break;   
//   case 8: FDCdumpdisc(); break;
   default: break;
  }
 }
#endif 
}

