#ifndef __VECX_H
#define __VECX_H

#include "gbConfig.h"
enum {
	VECTREX_MHZ		= 1500000, /* speed of the vectrex being emulated */
	VECTREX_COLORS  = 128,     /* number of possible colors ... grayscale */

	ALG_MAX_X		= 33000,	
	ALG_MAX_Y		= 41000
};

#ifdef use_lib_vectortiny
 typedef struct vector_type
 {
  //Para unsigned char dividimos y multiplicamos por 4
  //unsigned char x0, y0; //start coordinate  
  //unsigned char x1, y1; //end coordinate         
  //Funciona con short int
  unsigned short int x0, y0; //start coordinate
  unsigned short int x1, y1; //end coordinate
  //unsigned char color; //No necesito color
 } vector_t;
#else
 typedef struct vector_type
 {
  long x0, y0; //start coordinate
  long x1, y1; //end coordinate
  // color [0, VECTREX_COLORS - 1], if color = VECTREX_COLORS, then this is
  // an invalid entry and must be ignored.
  unsigned char color;
 } vector_t;
#endif 

//#ifdef use_lib_rom_no_use_ram
// extern const unsigned char *rom; //Usa puntero a Flash
//#else
// extern unsigned char rom[8192];
//#endif

//#ifdef use_lib_cartdridge_no_use_ram
// extern const unsigned char *cart; //puntero a Flash RAM
//#else
// extern unsigned char cart[32768];
//#endif 

#ifdef use_lib_snd_regs_8bits
 extern unsigned char snd_regs[16];
#else
 extern unsigned int snd_regs[16];
#endif
#ifdef use_lib_alg_jch_8bits
 extern unsigned char alg_jch0;
 extern unsigned char alg_jch1;
 extern unsigned char alg_jch2;
 extern unsigned char alg_jch3;
#else
 extern unsigned int alg_jch0;
 extern unsigned int alg_jch1;
 extern unsigned int alg_jch2;
 extern unsigned int alg_jch3;
#endif 

#ifdef use_lib_vectortiny
 extern int vector_draw_cnt;
 extern int vector_erse_cnt;
#else
 extern long vector_draw_cnt;
 extern long vector_erse_cnt;
#endif 

extern vector_t *vectors_draw;
extern vector_t *vectors_erse;

void vecx_reset (void);
#ifdef use_lib_vectortiny
 void vecx_emu (int cycles, int ahead);
#else
 void vecx_emu (long cycles, int ahead);
#endif 


int GetSizeBytes_vectors_set(void);
int GetSizeBytes_vector_hash(void);
int GetVECTOR_HASH(void);
int GetVECTOR_CNT(void);
int GetVECTREX_PDECAY(void);


#ifdef use_lib_optimice_readwrite8
 unsigned char read8 (unsigned short int address);
 void write8 (unsigned short int address, unsigned char data);
#else
 unsigned char read8 (unsigned address);
 void write8 (unsigned address, unsigned char data);
#endif 


#ifndef use_lib_optimice_call_via_sstep0
 void via_sstep0(void);
#endif
#ifndef use_lib_optimice_call_alg_sstep
 void alg_sstep(void);
#endif 
//void via_sstep1(void);

void snd_update(void);
void alg_update (void);

#endif
