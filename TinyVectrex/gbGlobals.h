#ifndef _GB_GLOBAL_H
 #define _GB_GLOBAL_H
  
 #include "gbConfig.h"
 #include "fabgl.h"

 #ifdef use_lib_remove_fabgl_queue
  extern uint8_t * gb_buffer_vga[768];
 #endif 

 extern unsigned int gb_time_vga_before;
 extern unsigned int gb_time_now;

 extern unsigned int gb_stats_time_cur_unified;
 extern unsigned int gb_stats_time_min_unified;
 extern unsigned int gb_stats_time_max_unified;
 extern unsigned int gb_stats_video_cur_unified;
 extern unsigned int gb_stats_video_min_unified;
 extern unsigned int gb_stats_video_max_unified;
 extern unsigned long gb_currentTime;
 extern unsigned long gb_fps_time_ini_unified;
 extern unsigned long gb_fps_unified;
 extern unsigned long gb_fps_ini_unified;
 
 #ifdef use_lib_gfx  
  //class VGA2DirectControllerPublic : public fabgl::VGAController
  class VGA2DirectControllerPublic : public fabgl::VGA2Controller
  {
   public:
    unsigned char GetISRHandle()
    {
     return 0;
    }
  };

  extern VGA2DirectControllerPublic VGAController;
 #endif
   
 //extern char *gb_cartfilename;
 extern unsigned char gb_load_new_rom;
 extern unsigned char gb_id_cur_rom; //rom actual 
 //extern unsigned char gb_reset;
 
  extern unsigned char gb_show_osd_main_menu;
 extern unsigned char gb_salir;
 
 
 #ifdef use_lib_rom_no_use_ram
  extern const unsigned char *rom; //Usa puntero a Flash
 #else
  extern unsigned char rom[8192];
 #endif

 #ifdef use_lib_cartdridge_no_use_ram
  extern const unsigned char *cart; //puntero a Flash RAM
 #else
   extern unsigned char cart[32768];
 #endif
 
#endif
