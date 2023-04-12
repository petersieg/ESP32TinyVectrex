//Port x86 John Hawthorn to ESP32 TTGO VGA v1.x by ackerman
//Port to DAC output to oscilloskop in X-Y mode by OzmoOzmo
//Arduino IDE 1.8.19 Espressif System 1.0.6
//March 2023 P.Sieg - removed: PS2*, OSD, WIFI + cleanups uncommented/unnused sections
//April 2023 P.Sieg - removed remaining fabgl leftovers

#include "gbConfig.h"
#include "gbGlobals.h"
#include "osint.h"

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
}


void loop()
{
  mainEmulator();
}
