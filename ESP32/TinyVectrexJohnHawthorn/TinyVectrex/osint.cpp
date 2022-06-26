
//JJ #include "SDL/SDL.h"
#include <math.h>
#include "gbConfig.h"
#include "fabgl.h"
#include "gbGlobals.h"
#include "osd.h"
//JJ #include "SDL_gfxPrimitives.h"

#include "osint.h"
#include "vecx.h"

#ifndef use_lib_wifi
 #ifdef use_lib_cartdridge_flash_ram
  #include "dataFlash/gbrom.h"
 #endif
#endif 

//#ifdef use_lib_rom_no_use_ram
 #include "dataFlash/gbbios.h"
//#endif

#include <Arduino.h>
#include "PS2Kbd.h"
#include "PS2KeyCode.h"
#include "PS2Kbd.h"

#ifdef use_lib_wifi
 #include "gbWifiConfig.h"
 #include "gbWifi.h" 
#endif


#define EMU_TIMER 20 // the emulators heart beats at 20 milliseconds


//char *gb_cartfilename=NULL;
unsigned char gb_load_new_rom=1;
unsigned char gb_id_cur_rom=0; //rom actual 
//Portar SDL_Event gb_event;
unsigned char gb_salir=0;
//unsigned char gb_reset=0;

//Portar SDL_Color gb_colors[256]; //paleta sdl 256 colores


//Portar static SDL_Surface *screen = NULL;

#ifdef use_lib_vectortiny
 static int screenx;
 static int screeny;
 static int scl_factor;  
 static int offx;
 static int offy; 
#else
 static long screenx;
 static long screeny;
 static long scl_factor; 
 //static float scl_factor_float;
 static long offx;
 static long offy; 
#endif 



//Portar void PreparaPaleta()
//Portar {
//Portar  //Paleta real TTGO VGA32 64 colores
//Portar  for (int j=0;j<4;j++)
//Portar  {
//Portar   for(int i=0;i<64;i++)
//Portar   {
//Portar    gb_colors[i+(j*64)].b=((i>>4)&0x03)*85;
//Portar    gb_colors[i+(j*64)].g=((i>>2)&0x03)*85;
//Portar    gb_colors[i+(j*64)].r=(i & 0x03)*85;
//Portar   }
//Portar  }
//Portar }

//Portar void draw_pixel(SDL_Surface *screen, int x, int y, unsigned char pixel)
//Portar { 
//Portar  Uint8 *bufp = (Uint8 *)screen->pixels + y * screen->w + x;
//Portar  if (x>=0 && x<screenx && y>=0 && y<screeny)
//Portar  {
//Portar   *bufp = pixel;
//Portar  }
//Portar  //else
//Portar  //{
//Portar  // printf("x:%d y:%d\n",x,y);     
//Portar  // fflush(stdout);
//Portar  //}
//Portar }


void swap(int *i, int *j)
{
 int t = *i;
 *i = *j;
 *j = t;
}

void swap_short(short int *i, short int *j)
{
 short int t = *i;
 *i = *j;
 *j = t;
}


#ifdef use_lib_vectortiny
 //****************************************************************************
//Portar  void jj_aalineRGBA_tiny(SDL_Surface *surface, int x0, int y0, int x1, int y1)
//Portar  {
//Portar   int i;
//Portar   float x = x1 - x0; 
//Portar   float y = y1 - y0;
//Portar   float length = sqrt( x*x + y*y );
//Portar   float addx = x / length;
//Portar   float addy = y / length;
//Portar   x = x0;
//Portar   y = y0;
//Portar   
//Portar   for (int i = 0; i < length; i += 1)
//Portar   {
//Portar    draw_pixel(surface, (int)x, (int)y, 255);
//Portar    x += addx;
//Portar    y += addy;	         
//Portar   }     
//Portar  }
#endif 

//****************************************************************************
//Portar void jj_aalineRGBA(SDL_Surface *surface, int x0, int y0, int x1, int y1, Uint8 r, Uint8 g, Uint8 b, Uint8 a) 
//Portar {
//Portar  unsigned char pixel= (r>0)?255:0;
//Portar  int i;
//Portar  float x = x1 - x0;
//Portar  float y = y1 - y0;
//Portar  float length = sqrt( x*x + y*y );
//Portar  float addx = x / length;
//Portar  float addy = y / length;
//Portar  x = x0;
//Portar  y = y0;
//Portar   
//Portar  for ( i = 0; i < length; i += 1)
//Portar  {
//Portar   draw_pixel(surface, (int)x, (int)y, pixel );
//Portar   x += addx;
//Portar   y += addy;	         
//Portar  }
//Portar }

/*
void jj_aalineRGBA (SDL_Surface *surface, int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a)
{  
 int steep,inc;
 int color= (r+g+b)/3;
 //x1= x1-(495/2);
 //x2= x2-(495/2);
 //y1= y1-(615/2);
 //y2= y2-(615/2); 

        // bresenham line

            steep = fabs(y2 - y1) > fabs(x2 - x1),
            inc = -1;

        if (steep) {
                swap(&x1, &y1);
                swap(&x2, &y2);
        }

        if (x1 > x2) {
                swap(&x1,&x2);
                swap(&y1,&y2);
        }

        if (y1 < y2) {
                inc = 1;
        }

        int dx = (int)fabs(x2 - x1);
        int dy = (int)fabs(y2 - y1);
        int y = y1, x = x1;
        int e = 0;

        for (x; x <= x2; x++) {
                if (steep) {
                        draw_pixel(surface, y, x, color);
                } else {
                        draw_pixel(surface, x, y, color);
                }

                if ((e + dy) << 1 < dx) {
                        e = e + dy;
                } else {
                        y += inc;
                        e = e + dy - dx;
                }
        }

}
*/



#ifdef use_lib_remove_fabgl_queue
 #ifdef use_lib_wifi
  static void Clear_bresenham()
 #else
 static void IRAM_ATTR Clear_bresenham()
 #endif
 {
  //uint8_t * ptrVideo;
  int tope = VGAController.getViewPortHeight();
  unsigned char topeByte= (VGAController.getViewPortWidth()>>3); //DIV 8
  for (unsigned int i=0;i<tope;i++)
  {
   //uint8_t* gb_buffer_vga= VGAController.getScanline(i);  
   //memset(gb_buffer_vga[i],0,128); //1024 DIV 8 
   memset(gb_buffer_vga[i],0,topeByte); //1024 DIV 8 
   //ptrVideo = (uint8_t *)VGAController.sgetScanline(i);
   //memset(ptrVideo,0,128); //1024 DIV 8 
  }  
  //memset(gb_buffer_vga[702],0xFF,10);
  //ptrVideo = (uint8_t *)VGAController.sgetScanline(10);
//  for (int i=0;i<tope;i++)
//  {
//   //ptrVideo = (uint8_t *)VGAController.sgetScanline(i);
//   //memset(ptrVideo,0xFF,10);
//   memset(gb_buffer_vga[i],0xFF,10);
//  }  
 }
 
 #ifdef use_lib_wifi
  static void draw_pixel_bresenham(short int x, short int y)
 #else
  static void IRAM_ATTR draw_pixel_bresenham(short int x, short int y)
 #endif
 {
  //unsigned short int aRow;
  uint8_t * ptrVideo;
  unsigned char aColByte, aColShift;
  unsigned char aBit;
  unsigned int auxOffs;
  
  //if (x>=0 && x<screenx && y>=0 && y<screeny)  
  //if (x>=0 && x<1024 && y>=0 && y<700)
  {
   //aRow= y;
   aColByte= x>>3; //x div 8
   aColShift= 7 - (x & 0x07); //x mod 8

   
   //gb_buffer_vga[y][x]= pixel;
   
   aBit= 1 << aColShift;
   //gb_buffer_vga[y][aColByte]= gb_buffer_vga[y][aColByte] | aBit;

   //uint8_t* gb_buffer_vga= VGAController.getScanline(y);
   auxOffs = aColByte;
   gb_buffer_vga[y][auxOffs]= gb_buffer_vga[y][auxOffs] | aBit;
   //ptrVideo = (uint8_t *)VGAController.sgetScanline(y);
   //ptrVideo[auxOffs]= ptrVideo[auxOffs] | aBit;
  }
 }

 #ifdef use_lib_wifi
  static void draw_line_bresenham(short int x1,short int y1,short int x2, short int y2)
 #else
  static void IRAM_ATTR draw_line_bresenham(short int x1,short int y1,short int x2, short int y2)
 #endif
 {
        // bresenham line
        //int x1 = cord1.x, y1 = cord1.y,
        //    x2 = cord2.x, y2 = cord2.y,
        //    steep = fabs(y2 - y1) > fabs(x2 - x1),
        //    inc = -1;
        //int steep = fabs(y2 - y1) > fabs(x2 - x1);

int topeY = VGAController.getViewPortHeight();
int topeX = VGAController.getViewPortWidth();

        int steep = abs(y2 - y1) > abs(x2 - x1);
        int inc = -1;        

        if (steep) {
                swap_short(&x1, &y1);
                swap_short(&x2, &y2);
        }

        if (x1 > x2) {
                swap_short(&x1,&x2);
                swap_short(&y1,&y2);
        }

        if (y1 < y2) {
                inc = 1;
        }

//        int dx = fabs(x2 - x1),
//            dy = fabs(y2 - y1),
//            y = y1, x = x1,
//            e = 0;
// int dx = fabs(x2 - x1);
// int dy = fabs(y2 - y1);
int dx = abs(x2 - x1);
int dy = abs(y2 - y1);
int y = y1;
int x = x1;
int e = 0;


        for (x; x <= x2; x++) {
                if (steep) 
                {
                 //draw_pixel( y, x, 255);
                 if (y>=0 && y<topeX && x>=0 && x<topeY)
                 {
                  draw_pixel_bresenham(y,x);
                 }
                 else
                 {
                  return;
                 }
                } 
                else 
                {
                 //draw_pixel( x, y, 255);
                 if (x>=0 && x<topeX && y>=0 && y<topeY)
                 {
                  draw_pixel_bresenham(x,y);
                 }
                 else
                 {
                  return;
                 }
                }

                if ((e + dy) << 1 < dx) {
                        e = e + dy;
                } else {
                        y += inc;
                        e = e + dy - dx;
                }
        }
 }
#endif



void osint_render()
{
 //Portar SDL_FillRect(screen, NULL, 0);  
 int v;
 //unsigned int x0,x1,y0,y1;
 int x0,x1,y0,y1;
 unsigned int vga_begin,vga_end;
 
 vga_begin = micros();

 if (vector_draw_cnt<=0)
 {
  vga_end = micros();
  unsigned int time_prev = vga_end - vga_begin;
  gb_stats_video_cur_unified = time_prev;
  if (time_prev< gb_stats_video_min_unified){
   gb_stats_video_min_unified= time_prev;
  }
  if (time_prev> gb_stats_video_max_unified){
   gb_stats_video_max_unified= time_prev;
  }         
  return;
 }

 #ifdef use_lib_gfx

#ifdef use_lib_remove_fabgl_queue
 Clear_bresenham();
#else
 Canvas cv(&VGAController);
 fabgl::Primitive p;
 Point auxPoint;

 //cv.setPenColor(Color::Black); //Borro pantalla SDL_FillRect
 //BEGIN setPenColor
 p.cmd = fabgl::PrimitiveCmd::SetPenColor;
 p.color = Color::Black;
 VGAController.addPrimitive(p);
 //END setPenColor

 //cv.setBrushColor(Color::Black); //Para el clear
 //BEGIN SetBrushColor
 p.cmd = fabgl::PrimitiveCmd::SetBrushColor;
 p.color = Color::Black;
 VGAController.addPrimitive(p);
 //END SetBrushColor

 //cv.clear();
 //Clear BEGIN  
 p.cmd = fabgl::PrimitiveCmd::Clear;
 p.ivalue = 0;
 VGAController.addPrimitive(p);
 //Clear END

 //if (vector_draw_cnt > GetVECTOR_CNT())
 //{//Si supera los 1000 vectores lo muestra
 // Serial.printf("vcnt:%d tope:%d\n",vector_draw_cnt,GetVECTOR_CNT());
 // //fflush(stdout);
 //}
#endif
    
 #ifdef use_lib_vectortiny
  //cv.setPenColor(Color::White);    
  //BEGIN setPenColor
  #ifdef use_lib_remove_fabgl_queue
  #else
   p.cmd = fabgl::PrimitiveCmd::SetPenColor;
   p.color = Color::White;
   VGAController.addPrimitive(p);
   //END setPenColor
  #endif 

  for(v = 0; v < vector_draw_cnt; v++)
  {   
   x0= (int)(vectors_draw[v].x0);
   y0= (int)(vectors_draw[v].y0);
   x1= (int)(vectors_draw[v].x1);
   y1= (int)(vectors_draw[v].y1);
   
   //printf("x,y:%d,%d x,y:%d,%d\n",x0,y0,x1,y1);
   //printf("x,y:%d,%d x,y:%d,%d\n",(x0/scl_factor),(y0/scl_factor),(x1/scl_factor),(y1/scl_factor));
   
   //x0= x0*100; //MULT 128
   //y0= y0*100;
   //x1= x1*100;
   //y1= y1*100;

   //scl_factor 85 640x480
   //scl_factor 68 800x600
   //scl_factor 53 1024x768
   if ((x0==x1) && (y0==y1))
   {//Punto      
    x0= offx + x0 / scl_factor;
    y0= offy + y0 / scl_factor;
    //x1= offx + x1 / scl_factor;
    //y1= offy + y1 / scl_factor;  
    //cv.setPixel(x0,y0,Color::White);
    //BEGIN setPixel   
    #ifdef use_lib_remove_fabgl_queue
     //draw_line_bresenham(x0,y0,x0,x0);
     draw_pixel_bresenham(x0,y0);
    #else
     p.cmd = fabgl::PrimitiveCmd::SetPixelAt;
     //auxPoint.X= x0;
     //auxPoint.Y= y0;
     auxPoint = {x0,y0};
     p.pixelDesc = { auxPoint, Color::White };
     VGAController.addPrimitive(p);
     //END setPixel
    #endif 
   
//Portar    jj_aalineRGBA_tiny(screen,x0,y0,x1,y1);
   //cv.drawLine(x0, y0, x1, y1);   
   //drawLine es moveTo y lineTo   
//   cv.moveTo(x0, y0);
//   cv.lineTo(x1, y1);
    
   }
   else
   {//Linea
    x0= offx + x0 / scl_factor;
    y0= offy + y0 / scl_factor;
    x1= offx + x1 / scl_factor;
    y1= offy + y1 / scl_factor;     
    #ifdef use_lib_remove_fabgl_queue
     draw_line_bresenham(x0,y0,x1,y1);
    #else
     //BEGIN moveTo   
     p.cmd = fabgl::PrimitiveCmd::MoveTo;
     p.position = Point(x0, y0);         
     VGAController.addPrimitive(p);
     //End moveTo    

     //Begin lineTo
     //Primitive p;
     p.cmd = fabgl::PrimitiveCmd::LineTo;
     p.position = Point(x1,y1);
     VGAController.addPrimitive(p);
     //End lineTo
    #endif 
   }
  }
  
  #ifdef use_lib_remove_fabgl_queue
  #else
   VGAController.processPrimitives();
  #endif 

  vga_end = micros();
  unsigned int time_prev = vga_end - vga_begin;
  gb_stats_video_cur_unified = time_prev;
  if (time_prev< gb_stats_video_min_unified){
   gb_stats_video_min_unified= time_prev;
  }
  if (time_prev> gb_stats_video_max_unified){
   gb_stats_video_max_unified= time_prev;
  }
  //Serial.printf("vga %d\n",(vga_end-vga_begin));
  //fflush(stdout);
 #else
	//printf("vcnt:%d\n",vector_draw_cnt);
	//fflush(stdout);
	for(v = 0; v < vector_draw_cnt; v++)
    {
		Uint8 c = vectors_draw[v].color * 256 / VECTREX_COLORS;
//JJ		aalineRGBA(screen,
//JJ				offx + vectors_draw[v].x0 / scl_factor,
//JJ        offy + vectors_draw[v].y0 / scl_factor,
//JJ				offx + vectors_draw[v].x1 / scl_factor,
//JJ        offy + vectors_draw[v].y1 / scl_factor,
//JJ				c, c, c, 0xff);

    //int x0=(vectors_draw[v].x0*7)/1024, x1=(vectors_draw[v].x1*7)/1024, y0=((vectors_draw[v].y0/128-32)*900)/1024, y1=((vectors_draw[v].y1/128-32)*900)/1024;
    //sline(  x0,y0,x1,y1, c);
    
    x0= vectors_draw[v].x0;
    y0= vectors_draw[v].y0;
    x1= vectors_draw[v].x1;
    y1= vectors_draw[v].y1;
    //if (c!=254)
    //{
    // printf("Color:%d\n",c);
    // fflush(stdout);
    //}
    
	jj_aalineRGBA(screen,
				  (offx + x0 / scl_factor),
                  (offy + y0 / scl_factor),
		  		  (offx + x1 / scl_factor),
                  (offy + y1 / scl_factor),
				c, c, c, 0xff);

//	jj_aalineRGBA(screen,
//				  (int)((float)offx + (float)x0 / (float)scl_factor_float),
//                  (int)((float)offy + (float)y0 / (float)scl_factor_float),
//		  		  (int)((float)offx + (float)x1 / (float)scl_factor_float),
//                  (int)((float)offy + (float)y1 / (float)scl_factor_float),
//				c, c, c, 0xff);				
				
     //printf("x0:%d y0:%d x1:%d y1:%d\n", x0,y0,x1,y1);
	} 
 #endif
    
#endif    

 //Portar SDL_Flip(screen);
}

static char *romfilename = "rom.dat";
//static char *romfilename = "OnslaughtElectronBeamPlayboys.bin";
//static char *romfilename = "rainy.bin";
static char *cartfilename = NULL;
//static char *cartfilename = "rainy.bin";

#ifdef use_lib_wifi
 //void load_cart_WIFI(char * cadUrl)
 void load_cart_WIFI()
 {
  int auxFileSize=0;

  //if (strcmp(cadUrl,"")==0)  
  if (strcmp(gb_cadUrl,"")==0)
  {
   #ifdef use_lib_wifi_debug
    Serial.printf("cart name empty\n");
   #endif          
   return;
  }

  #ifdef use_lib_wifi_debug
   Serial.printf("load_cart_WIFI\n");
  #endif
  #ifdef use_lib_wifi_debug
   Serial.printf("Check WIFI\n");
  #endif 
  if (Check_WIFI() == false)
  {
   return;
  }
  int leidos=0;
  #ifdef use_lib_wifi_debug
   //Serial.printf("URL:%s\n",cadUrl);
   Serial.printf("URL:%s\n",gb_cadUrl);   
  #endif   
  //Asignar_URL_stream_WIFI(cadUrl);
  Asignar_URL_stream_WIFI(gb_cadUrl);
  auxFileSize= gb_size_file_wifi;
  #ifdef use_lib_wifi_debug
   Serial.printf("Size cart:%d\n",gb_size_file_wifi);
  #endif
  Leer_url_stream_WIFI(&leidos);
  #ifdef use_lib_wifi_debug
   Serial.printf("Leidos:%d\n",leidos); //Leemos 1024 bytes
  #endif

  //He leido 1024 bytes. Lee resto
  int contBuffer=0;
  int cont1024= 0;
  while (contBuffer< auxFileSize)
  {
   if (contBuffer>= sizeof(cart))
   {
    #ifdef use_lib_wifi_debug
     Serial.printf("Limit exced cart 32768\n");
    #endif            
    break;
   }
   cart[contBuffer]= gb_buffer_wifi[cont1024];
   contBuffer++;

   cont1024++;
   if (cont1024 >= 1024)
   {
    Leer_url_stream_WIFI(&leidos);
    #ifdef use_lib_wifi_debug
     Serial.printf("Leidos:%d\n",leidos);
    #endif 
    cont1024= 0;
   }
  }

 }
#endif

static void initLoadROM()
{
   #ifdef use_lib_rom_no_use_ram
    #ifdef use_lib_log_serial
     Serial.printf("Load rom FLASH\n");
    #endif 
    //fflush(stdout);
    rom= gb_rom_bios;
   #else
    memcpy(rom,gb_rom_bios,8192);
//	FILE *f;
//	if(!(f = fopen(romfilename, "rb"))){
//		perror(romfilename);
//		printf("ERROR load rom\n");
//		//fflush(stdout);
//		exit(EXIT_FAILURE);
//	}
	//printf("Load rom %s\n",romfilename);
    //fflush(stdout);
//	if(fread(rom, 1, sizeof (rom), f) != sizeof (rom)){
//		printf("Invalid rom length\n");
//		//fflush(stdout);
//		exit(EXIT_FAILURE);
//	}
//	fclose(f);
   #endif	

    #ifdef use_lib_cartdridge_no_use_ram
    #else
     memset(cart, 0, sizeof (cart));
    #endif
             
    #ifdef use_lib_cartdridge_flash_ram
     #ifdef use_lib_log_serial
      #ifdef use_lib_wifi
       Serial.printf("Load cartdridge FLASH WIFI id:%d name:%s\n",gb_id_cur_rom,gb_cadUrl);
      #else
       Serial.printf("Load cartdridge FLASH id:%d size:%d\n",gb_id_cur_rom,gb_list_cart_size[gb_id_cur_rom]);
      #endif 
     #endif

     #ifdef use_lib_wifi      
     #else
      int topeCartBytes;
      if (gb_list_cart_size[gb_id_cur_rom] < 32768){
       topeCartBytes= gb_list_cart_size[gb_id_cur_rom];
      }
      else{
       topeCartBytes= 32768;
       #ifdef use_lib_log_serial
        Serial.printf("Excede tamanio Cartucho 32768 bytes\n");
       #endif 
      }
     #endif

     #ifdef use_lib_cartdridge_no_use_ram
      cart = gb_list_rom_data[gb_id_cur_rom];
     #else
      #ifdef use_lib_wifi
       ShowStatusWIFI(1);
       load_cart_WIFI(); //uso gb_cadUrl
       ShowStatusWIFI(0);
      #else
       memcpy(cart, gb_list_rom_data[gb_id_cur_rom], topeCartBytes);
      #endif 
     #endif
     
    #else	
	 if(cartfilename)
     {
		FILE *f;
		if(!(f = fopen(cartfilename, "rb"))){
			perror(cartfilename);
   		    printf("Invalid cart\n");
		    //fflush(stdout);			
			exit(EXIT_FAILURE);
		}
        printf("Load cartdridge %s\n",cartfilename);
		//fflush(stdout);        
		fread(cart, 1, sizeof (cart), f);
		fclose(f);
	 }
    #endif 
}

void resize(int width, int height)
{
 #ifdef use_lib_vectortiny     
  int sclx, scly;
 #else
  long sclx, scly;
 #endif 
 float float_sclx, float_scly;

	screenx = width;
	screeny = height;
	//JJ screen = SDL_SetVideoMode(screenx, screeny, 0, SDL_SWSURFACE | SDL_RESIZABLE);
//Portar 	screen = SDL_SetVideoMode(screenx, screeny, 8, SDL_SWSURFACE | SDL_RESIZABLE);
//Portar 	SDL_WM_SetCaption ("SDL Port ESP32 Vectrex", NULL);
//Portar 	PreparaPaleta();
//Portar 	SDL_SetPalette(screen,SDL_LOGPAL|SDL_PHYSPAL, gb_colors, 0, 256);
//Portar 	SDLAssignSDLSurfaceOSD(screen,&gb_event);//Asignamos surface

//Portar 	sclx = ALG_MAX_X / screen->w;
//Portar 	scly = ALG_MAX_Y / screen->h;


// 	sclx = ALG_MAX_X / 640;
// 	scly = ALG_MAX_Y / 480;

 	sclx = ALG_MAX_X / width;
 	scly = ALG_MAX_Y / height;         


	
    //float_sclx = (float)ALG_MAX_X / (float)screen->w;
	//float_scly = (float)ALG_MAX_Y / (float)screen->h;

	scl_factor = sclx > scly ? sclx : scly;
	//scl_factor_float= float_sclx > float_scly ? float_sclx : float_scly;

  offx = (screenx - ALG_MAX_X / scl_factor) / 2;
  offy = (screeny - ALG_MAX_Y / scl_factor) / 2;
  
  //offx = (int)(screenx - ALG_MAX_X / scl_factor_float) / 2;
  //offy = (int)(screeny - ALG_MAX_Y / scl_factor_float) / 2;
}



static void readevents()
{

  if (checkAndCleanKey(KEY_F1))  
  {
   gb_show_osd_main_menu=1;
   //Serial.printf("Tecla F1 Menu OSD\n");                     
   do_tinyOSD();
  }

  if (keymap[PS2_KC_A] == 0){ 
   snd_regs[14] &= ~0x01; 
  }
  else{ 
   snd_regs[14]|= 0x01; 
  }

  if (keymap[PS2_KC_S] == 0){ 
   snd_regs[14] &= ~0x02; 
  }
  else{ 
   snd_regs[14]|= 0x02; 
  }  

  if (keymap[PS2_KC_D] == 0){ 
   snd_regs[14] &= ~0x04; 
  }
  else{ 
   snd_regs[14]|= 0x04; 
  }  

  if (keymap[PS2_KC_F] == 0){ 
   snd_regs[14] &= ~0x08; 
  }
  else{ 
   snd_regs[14]|= 0x08; 
  }  

  if (keymap[KEY_CURSOR_LEFT] == 0){ 
   alg_jch0 = 0x00;
  }
  else{ 
   alg_jch0 = 0x80;
  }

  if (keymap[KEY_CURSOR_RIGHT] == 0){
   alg_jch0 = 0xff;
  }
  else{ 
   //alg_jch0 = 0x80;
  }    

  if (keymap[KEY_CURSOR_UP] == 0){ 
   alg_jch1 = 0xff;
  }
  else{ 
   alg_jch1 = 0x80;
  }      

  if (keymap[KEY_CURSOR_DOWN] == 0){ 
   alg_jch1 = 0x00;
  }
  else{ 
   //alg_jch1 = 0x80;
  }      
 





/*        
 auto keyboard = PS2Controller.keyboard();
 if (keyboard->isKeyboardAvailable())
 {
  if (keyboard->isVKDown(fabgl::VK_F1)){
   gb_show_osd_main_menu=1;
   //Serial.printf("Tecla F1 Menu OSD\n");                     
   do_tinyOSD();
  }

  if (keyboard->isVKDown(fabgl::VK_a)){ 
   snd_regs[14] &= ~0x01; 
  }
  else{ 
   snd_regs[14]|= 0x01; 
  }

  if (keyboard->isVKDown(fabgl::VK_s)){ 
   snd_regs[14] &= ~0x02; 
  }
  else{ 
   snd_regs[14]|= 0x02; 
  }  

  if (keyboard->isVKDown(fabgl::VK_d)){ 
   snd_regs[14] &= ~0x04; 
  }
  else{ 
   snd_regs[14]|= 0x04; 
  }  

  if (keyboard->isVKDown(fabgl::VK_f)){ 
   snd_regs[14] &= ~0x08; 
  }
  else{ 
   snd_regs[14]|= 0x08; 
  }  

  if (keyboard->isVKDown(fabgl::VK_LEFT)){ 
   alg_jch0 = 0x00;
  }
  else{ 
   alg_jch0 = 0x80;
  }

  if (keyboard->isVKDown(fabgl::VK_RIGHT)){ 
   alg_jch0 = 0xff;
  }
  else{ 
   alg_jch0 = 0x80;
  }    

  if (keyboard->isVKDown(fabgl::VK_UP)){ 
   alg_jch1 = 0xff;
  }
  else{ 
   alg_jch1 = 0x80;
  }      

  if (keyboard->isVKDown(fabgl::VK_DOWN)){ 
   alg_jch1 = 0x00;
  }
  else{ 
   alg_jch1 = 0x80;
  }      
 }
*/

 
/*Portar         

	//JJ SDL_Event e;
	while(SDL_PollEvent(&gb_event)){
		switch(gb_event.type){
			case SDL_QUIT:
				exit(0);
				break;
      case SDL_VIDEORESIZE:
        resize(gb_event.resize.w, gb_event.resize.h);
        break;
			case SDL_KEYDOWN:
				switch(gb_event.key.keysym.sym){
                    case SDLK_F1:
                     gb_show_osd_main_menu=1;
                     printf("Tecla F1 Menu OSD\n");
                     //fflush(stdout);
                     do_tinyOSD();
                     break;                                         
					case SDLK_ESCAPE:
                        gb_salir=1;
						exit(0);
					case SDLK_a:
						snd_regs[14] &= ~0x01;
						break;
					case SDLK_s:
						snd_regs[14] &= ~0x02;
						break;
					case SDLK_d:
						snd_regs[14] &= ~0x04;
						break;
					case SDLK_f:
						snd_regs[14] &= ~0x08;
						break;
					case SDLK_LEFT:
						alg_jch0 = 0x00;
						break;
					case SDLK_RIGHT:
						alg_jch0 = 0xff;
						break;
					case SDLK_UP:
						alg_jch1 = 0xff;
						break;
					case SDLK_DOWN:
						alg_jch1 = 0x00;
						break;
          default:
            break;
				}
				break;
			case SDL_KEYUP:
				switch(gb_event.key.keysym.sym){
					case SDLK_a:
						snd_regs[14] |= 0x01;
						break;
					case SDLK_s:
						snd_regs[14] |= 0x02;
						break;
					case SDLK_d:
						snd_regs[14] |= 0x04;
						break;
					case SDLK_f:
						snd_regs[14] |= 0x08;
						break;
					case SDLK_LEFT:
						alg_jch0 = 0x80;
						break;
					case SDLK_RIGHT:
						alg_jch0 = 0x80;
						break;
					case SDLK_UP:
						alg_jch1 = 0x80;
						break;
					case SDLK_DOWN:
						alg_jch1 = 0x80;
						break;
          default:
            break;
				}
				break;
      default:
        break;
		}
	}
*/        
}

void osint_emuloop()
{
 unsigned long cpu_begin,cpu_end;
 //Portar 	Uint32 next_time = SDL_GetTicks() + EMU_TIMER;
 unsigned int next_time = millis() + EMU_TIMER;

 vecx_reset();

    for(;;)
    {
     if (gb_load_new_rom==1)
     {
      break;
     }
     
     cpu_begin= micros();
      vecx_emu((VECTREX_MHZ / 1000) * EMU_TIMER, 0);
     cpu_end= micros();
     gb_fps_unified++;
     gb_stats_time_cur_unified= (cpu_end-cpu_begin);
     if (gb_stats_time_cur_unified < gb_stats_time_min_unified)
      gb_stats_time_min_unified= gb_stats_time_cur_unified;
     if (gb_stats_time_cur_unified > gb_stats_time_max_unified)
      gb_stats_time_max_unified= gb_stats_time_cur_unified;
     
     readevents();     

     //Serial.printf("%d %d %d\n",cpu_end,cpu_begin,(cpu_end-cpu_begin));
     //Serial.printf("%d\n",(cpu_end-cpu_begin));
     gb_currentTime= millis();
     if ((gb_currentTime - gb_fps_time_ini_unified) > 1000)
     {
      gb_fps_time_ini_unified= gb_currentTime;
      unsigned int aux_fps= gb_fps_unified - gb_fps_ini_unified;
      gb_fps_ini_unified = gb_fps_unified;
      #ifdef use_lib_log_serial
       Serial.printf ("fps:%d %d m:%d mx:%d v %d m:%d mx:%d\n",aux_fps,gb_stats_time_cur_unified,gb_stats_time_min_unified,gb_stats_time_max_unified,gb_stats_video_cur_unified,gb_stats_video_min_unified,gb_stats_video_max_unified);
      #endif 
      gb_stats_time_min_unified = 500000;
      gb_stats_time_max_unified = 0;
      gb_stats_video_min_unified = 500000;
      gb_stats_video_max_unified = 0;             
     }

//		{
//Portar 			Uint32 now = SDL_GetTicks();
                        unsigned int now = millis();
			if(now < next_time){                        
//Portar 				SDL_Delay(next_time - now);
                        }
			else{
				next_time = now;
                        }
			next_time += EMU_TIMER;
//		}
	}
}

//********************************
void ImprimeMemoria()
{
 #ifdef use_lib_log_serial        
  Serial.printf("VECTREX_MHZ:%d\n",VECTREX_MHZ);
  Serial.printf("VECTREX_PDECAY:%d\n",GetVECTREX_PDECAY()); 
  Serial.printf("VECTOR_CNT:%d\n",GetVECTOR_CNT());
  Serial.printf("vectors_set:%d bytes\n",GetSizeBytes_vectors_set());
  Serial.printf("VECTOR_HASH:%d\n",GetVECTOR_HASH());
  Serial.printf("vector_hash:%d bytes\n",GetSizeBytes_vector_hash());
 #endif 
 //fflush(stdout);
}




//********************************
//int main(int argc, char *argv[])
int mainEmulator()
{
  ImprimeMemoria();
    
//Portar   SDL_Init(SDL_INIT_VIDEO);

  //JJ resize(330*3/2, 410*3/2);
  //resize(320,200);
  #ifdef use_lib_vga640x480  
   resize(640,480); //se ve bien
  #else
   #ifdef use_lib_vga800x600
    resize (800,600);
   #else
    #ifdef use_lib_vga1024x768
     resize(1024,768); //Se ve perfecto
    #endif 
   #endif
  #endif 
  
  
  //resize(360,480);  
  //resize(360,240);
  #ifdef use_lib_log_serial
   Serial.printf("Sizeof long:%d\n",sizeof(long));
   Serial.printf("screenx:%d screeny:%d\n",screenx,screeny);
  #endif  
  
//Portar   if(argc > 1)
//Portar   {
//Portar    cartfilename = argv[1];
//Portar   }

  while (gb_salir == 0)
  { 
   if (gb_load_new_rom == 1)
   {
    //cartfilename= gb_cartfilename;
    gb_load_new_rom=0;                       
    initLoadROM(); //Cambio de init que lo usa fabgl
    osint_emuloop();
   }
  }

  return 0;
}

