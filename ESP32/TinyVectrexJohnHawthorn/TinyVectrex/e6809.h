#ifndef __E6809_H
#define __E6809_H

#include "gbConfig.h"

//user defined read and write functions

#ifdef use_lib_optimice_readwrite8
#else
 extern unsigned char (*e6809_read8) (unsigned address);
 extern void (*e6809_write8) (unsigned address, unsigned char data);
#endif 

void e6809_reset (void);
#ifdef use_lib_optimice_e6809_sstep
 ////JJ unsigned int e6809_sstep (unsigned char irq_i, unsigned char irq_f);
 unsigned int e6809_sstep (unsigned char irq_i); //No se usa parametro 2
#else
 unsigned e6809_sstep (unsigned irq_i, unsigned irq_f);
#endif 


#endif
