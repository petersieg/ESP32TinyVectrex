//Port x86 John Hawthorn to ESP32 TTGO VGA v1.x by ackerman
//Port to DAC output to oscilloskop in X-Y mode by OzmoOsmo
//Support 1024x768
//VGA2Controller 1 bpp
//Arduino IDE 1.8.19 Espressif System 1.0.6
//Video fabgl 1.0.9 (required Arduino IDE)
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

void setup()
{
  #ifdef use_lib_log_serial
   Serial.begin(115200);
  #endif 
  
  #ifdef use_lib_gfx
   VGAController.begin();  
   VGAController.setResolution(SVGA_1024x768_60Hz); //resize (1024,768);
   VGAController.enableBackgroundPrimitiveExecution(false);
   VGAController.enableBackgroundPrimitiveTimeout(false);
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
