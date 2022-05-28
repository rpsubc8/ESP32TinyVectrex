#ifndef _GB_CONFIG_H
 #define _GB_CONFIG_H
 
 //Medir rendimiento sin usar fabgl video
 #define use_lib_gfx

 //Usar solo uno de lo 3 modos de video
 //#define use_lib_vga640x480
 //#define use_lib_vga800x600
 #define use_lib_vga1024x768

 //Corrige teclados que no se inicializan(solucion dcrespo3D)
 //#define FIX_PERIBOARD_NOT_INITING

 //Vector unsigned char en lugar de long
 #define use_lib_vectortiny
 
 #define use_lib_regtiny
 
 //Cartucho en Memoria Flash RAM
 #define use_lib_cartdridge_flash_ram
 
 //No usa 32768 bytes RAM cartucho
 //#ifdef use_lib_cartdridge_flash_ram
 // #define use_lib_cartdridge_no_use_ram
 //#endif
 
 //No usa 8192 bytes de BIOS rom
 //#define use_lib_rom_no_use_ram
 


 #define gb_ms_vga 50

 #define use_lib_optimice_vecx_reset
 //Elimina e6809_read8 write8, usa read8 write8 directo
 #define use_lib_optimice_readwrite8
 #define use_lib_optimice_readwrite16
 #define use_lib_optimice_pc_read8
 #define use_lib_optimice_pc_read16

 #define use_lib_optimice_e6809_sstep
 #define use_lib_optimice_alg_sstep
 #define use_lib_optimice_int_update

 //30000 veces que se suele llamar
 #define use_lib_optimice_call_via_sstep1
 //30000 veces que se suele llamar
 #define use_lib_optimice_call_via_sstep0
 //30000 veces
 #define use_lib_optimice_call_alg_sstep

#endif
