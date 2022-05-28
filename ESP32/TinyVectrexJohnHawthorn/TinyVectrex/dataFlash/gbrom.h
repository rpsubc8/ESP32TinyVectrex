#ifndef _GBROM_H
 #define _GBROM_H
 
 #include "../gbConfig.h"
   
 #ifdef use_lib_cartdridge_flash_ram  
 
  #include "rom/gbCartEmpty.h"
  #include "rom/gbCartRainy.h"
  #include "rom/gbCartOnslaughtElect.h"
  #include "rom/gbCartDemo2019party.h"
  #include "rom/gbCartDeadline2019.h"
  #include "rom/gbCartRaidingparty.h"
  #include "rom/gbCartTrex.h"

  #define max_list_rom 7
  static const char * gb_list_rom_title[max_list_rom]={
   "Empty",
   "rainy",
   "OnslaughtElect",
   "demo2019party",
   "DEADLINE2019",
   "raidingparty",
   "trex"         
  };  
  
   //Tamanio en bytes
  static const unsigned int gb_list_cart_size[max_list_rom]={
   413
   ,413
   ,31730
   ,65536
   ,507
   ,31927
   ,11027
  };
  
 //Datos
  
  static const unsigned char * gb_list_rom_data[max_list_rom]={
   gb_cart_empty
   ,gb_cart_rainy
   ,gb_cart_OnslaughtElect      
   ,gb_cart_demo2019party
   ,gb_cart_deadline2019
   ,gb_cart_raidingparty
   ,gb_cart_trex
  };
 
 #else
 
  #define max_list_rom 6
 
  static const char * gb_list_rom_title[max_list_rom]={
   "rainy.bin",
   "OnslaughtElect.bin",
   "demo2019party.bin",
   "DEADLINE2019.bin",
   "raidingparty.bin",
   "trex.bin"
   //"OnslaughtElectronBeamPlayboys.bin"
  };
  
  static const unsigned char * gb_list_rom_data[max_list_rom]={
   NULL
   ,NULL
   ,NULL
   ,NULL
   ,NULL
   ,NULL        
  }; 
 
 #endif 
 
#endif
 
