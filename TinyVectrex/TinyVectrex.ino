//Port x86 John Hawthorn to ESP32 TTGO VGA v1.x by ackerman
//No PSRAM (minimal SRAM)
//TTGO VGA32 v1.0, 1.1, 1.2 y 1.4
//19 fps 1024x768 and 23 fps 640x480
//Support 640x480, 800x600 and 1024x768
//VGA2Controller 1 bpp
//Visual Studio 1.66.1 PLATFORMIO 2.4.3 Espressif32 v3.5.0 (python 3.6)
//Arduino IDE 1.8.19 Espressif System 1.0.6
//Video fabgl 1.0.9 (required Arduino IDE)
//WIFI support load cart (http recommended)
//March 2023 P.Sieg - removed: PS2*, OSD, WIFI + cleanups uncommented/unnused sections

#include "gbConfig.h"
#include "fabgl.h"
#include "WiFiGeneric.h" //Para que compile
#include "gbGlobals.h"
#include "osint.h"

#ifdef use_lib_gfx
 VGA2DirectControllerPublic VGAController;
#endif

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
   VGAController.enableBackgroundPrimitiveExecution(false);
   VGAController.enableBackgroundPrimitiveTimeout(false);
  #endif

  #ifdef use_lib_remove_fabgl_queue
   PreparaBitluniVGA();
  #endif

  #ifdef use_lib_log_serial
   Serial.printf("Width:%d Hi:%d\n",VGAController.getScreenWidth(),VGAController.getScreenHeight());
   Serial.printf("View Width:%d Hi:%d\n",VGAController.getViewPortWidth(),VGAController.getViewPortHeight());
   Serial.printf("END SETUP %d\n", ESP.getFreeHeap());
  #endif 
}


void loop()
{
  mainEmulator();
}
