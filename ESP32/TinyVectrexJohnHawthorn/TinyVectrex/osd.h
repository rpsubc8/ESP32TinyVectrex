#ifndef OSDFILE_H
 #define OSDFILE_H

  //JJ #include "gb_sdl_font6x8.h"
 
  //JJ #include <SDL/SDL.h>


//JJ extern Uint32 gbSDLVideoDelay;

//Portar #define byte unsigned char

//Portar #define boolean unsigned char
void changeSna2Flash(unsigned char id);
void do_tinyOSD(void);
//JJ void SDLAssignSDLSurfaceOSD(SDL_Surface *surface,SDL_Event * ptrEvent);
void SDLActivarOSDMainMenu();
void SDLOSDClear(void);
//JJ SDL_Surface * SDLOSDGetSurface();
void SDLEsperaTeclado(void);
//void load_prg(unsigned char id);
void ShowStatusWIFI(unsigned char aState);
#endif
