
#include <math.h>
#include "gbConfig.h"
#include "gbGlobals.h"

#include "osint.h"
#include "vecx.h"

#ifdef use_lib_cartdridge_flash_ram
#include "dataFlash/gbrom.h"
#endif

// ac{}
#include "FastDac.h"
#include "esp32-hal-cpu.h"
#include <Arduino.h>
#include <driver/dac.h>
#include <soc/rtc.h>
#include <soc/sens_reg.h>

#include "dataFlash/gbbios.h"

#include <Arduino.h>

#define EMU_TIMER 20 // the emulators heart beats at 20 milliseconds

unsigned char gb_load_new_rom = 1;
unsigned char gb_id_cur_rom = 0; // rom actual; Default 0 = Minestorm = only available option 
unsigned char gb_salir = 0;

static int screenx;
static int screeny;
static int scl_factor;
static int offx;
static int offy;

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

inline void drawdot(int x, int y) {
    DAC1Write(map(x, 0, _ALG_MAX_X, 0, 255));
    DAC2Write(map(_ALG_MAX_Y - y, 0, _ALG_MAX_Y, 0, 255));
}

#define drawskip 1
void drawline(int x1, int y1, int x2, int y2) {
    int x, y, xe, ye;
    int dx = x2 - x1;
    int dy = y2 - y1;
    int dx1 = labs(dx); // long abs
    int dy1 = labs(dy);
    int px = 2 * dy1 - dx1;
    int py = 2 * dx1 - dy1;
    if (dy1 <= dx1) {
        if (dx >= 0) {
            x = x1;
            y = y1;
            xe = x2;
        } else {
            x = x2;
            y = y2;
            xe = x1;
        }
        drawdot(x, y);
        // return;

        for (int i = 0; x < xe; i += drawskip) {
            x = x + 1;
            if (px < 0) {
                px = px + 2 * dy1;
            } else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    y = y + 1;
                } else {
                    y = y - 1;
                }
                px = px + 2 * (dy1 - dx1);
            }
            drawdot(x, y);
        }
    } else {
        if (dy >= 0) {
            x = x1;
            y = y1;
            ye = y2;
        } else {
            x = x2;
            y = y2;
            ye = y1;
        }
        drawdot(x, y);

        for (int i = 0; y < ye; i += drawskip) {
            y = y + 1;
            if (py <= 0) {
                py = py + 2 * dx1;
            } else {
                if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) {
                    x = x + 1;
                } else {
                    x = x - 1;
                }
                py = py + 2 * (dx1 - dy1);
            }
            drawdot(x, y);
        }
    }
}

void osint_render() {

    int x0, x1, y0, y1;
    DACPrepare(true);
    for (int v = 0; v < vector_draw_cnt; v++) {
        x0 = (int)(vectors_draw[v].x0);
        y0 = (int)(vectors_draw[v].y0);
        x1 = (int)(vectors_draw[v].x1);
        y1 = (int)(vectors_draw[v].y1);

        drawline(x0, y0, x1, y1);
    }

    DACUnprepare(true);
    // Serial.print(vector_draw_cnt / (millis() - t));
    // Serial.println(" Sa/s");
    drawdot(0, 0);
}


static void initLoadROM() {
#ifdef use_lib_rom_no_use_ram
#ifdef use_lib_log_serial
    Serial.printf("Load rom FLASH\n");
#endif
    // fflush(stdout);
    rom = gb_rom_bios;
#else
    memcpy(rom, gb_rom_bios, 8192);
#endif

#ifdef use_lib_cartdridge_no_use_ram
#else
    memset(cart, 0, sizeof(cart));
#endif

#ifdef use_lib_log_serial
    Serial.printf("Load cartdridge FLASH id:%d size:%d\n", gb_id_cur_rom, gb_list_cart_size[gb_id_cur_rom]);
#endif

    int topeCartBytes;
    if (gb_list_cart_size[gb_id_cur_rom] < 32768) {
        topeCartBytes = gb_list_cart_size[gb_id_cur_rom];
    } else {
        topeCartBytes = 32768;
#ifdef use_lib_log_serial
        Serial.printf("Excede tamanio Cartucho 32768 bytes\n");
#endif
    }

#ifdef use_lib_cartdridge_no_use_ram
    cart = gb_list_rom_data[gb_id_cur_rom];
#else
    memcpy(cart, gb_list_rom_data[gb_id_cur_rom], topeCartBytes);
#endif
}

static void readevents() {

    if (digitalRead(b1)==0){
        //Serial.print("B1 Down");
        snd_regs[14] &= ~0x01;
    }
    else{
        //Serial.print("B1 Up");
        snd_regs[14] |= 0x01;
    }

    if (digitalRead(b2)==0){
        //Serial.print("B2 Down");
        snd_regs[14] &= ~0x02;
    }
    else
        snd_regs[14] |= 0x02;

    if (digitalRead(b3)==0)
        snd_regs[14] &= ~0x04;
    else
        snd_regs[14] |= 0x04;

    if (digitalRead(b4)==0)
        snd_regs[14] &= ~0x08;
    else
        snd_regs[14] |= 0x08;



    if (digitalRead(CuL)==0) {
        //Serial.print("Left");
        alg_jch0 = 0x00;
    } else {
        alg_jch0 = 0x80;
    }

    if (digitalRead(CuR)==0) {
        //Serial.print("Right");
        alg_jch0 = 0xff;
    } else {
        //alg_jch0 = 0x80;
    }

    if (digitalRead(CuU)==0) {
        alg_jch1 = 0xff;
    } else {
        alg_jch1 = 0x80;
    }

    if (digitalRead(CuD)==0) {
        alg_jch1 = 0x00;
    } else {
        // alg_jch1 = 0x80;
    }
}

void osint_emuloop() {
    unsigned long cpu_begin, cpu_end;
    // Portar 	Uint32 next_time = SDL_GetTicks() + EMU_TIMER;
    unsigned int next_time = millis() + EMU_TIMER;

    vecx_reset();

    for (;;) {
        if (gb_load_new_rom == 1) {
            break;
        }

        cpu_begin = micros();
        vecx_emu((VECTREX_MHZ / 1000) * EMU_TIMER, 0);
        cpu_end = micros();
        gb_fps_unified++;
        gb_stats_time_cur_unified = (cpu_end - cpu_begin);
        if (gb_stats_time_cur_unified < gb_stats_time_min_unified)
            gb_stats_time_min_unified = gb_stats_time_cur_unified;
        if (gb_stats_time_cur_unified > gb_stats_time_max_unified)
            gb_stats_time_max_unified = gb_stats_time_cur_unified;

        readevents();

        gb_currentTime = millis();
        if ((gb_currentTime - gb_fps_time_ini_unified) > 1000) {
            gb_fps_time_ini_unified = gb_currentTime;
            unsigned int aux_fps = gb_fps_unified - gb_fps_ini_unified;
            gb_fps_ini_unified = gb_fps_unified;
#ifdef use_lib_log_serial
            Serial.printf("fps:%d\n", aux_fps);
#endif
            gb_stats_time_min_unified = 500000;
            gb_stats_time_max_unified = 0;
            gb_stats_video_min_unified = 500000;
            gb_stats_video_max_unified = 0;
        }

        //		{
        // Portar 			Uint32 now = SDL_GetTicks();
        unsigned int now = millis();
        if (now < next_time) {
            // Portar 				SDL_Delay(next_time - now);
        } else {
            next_time = now;
        }
        next_time += EMU_TIMER;
        //		}
    }
}

//********************************
void ImprimeMemoria() {
#ifdef use_lib_log_serial
    Serial.printf("VECTREX_MHZ:%d\n", VECTREX_MHZ);
    Serial.printf("VECTREX_PDECAY:%d\n", GetVECTREX_PDECAY());
    Serial.printf("VECTOR_CNT:%d\n", GetVECTOR_CNT());
    Serial.printf("vectors_set:%d bytes\n", GetSizeBytes_vectors_set());
    Serial.printf("VECTOR_HASH:%d\n", GetVECTOR_HASH());
    Serial.printf("vector_hash:%d bytes\n", GetSizeBytes_vector_hash());
#endif
    // fflush(stdout);
}

//********************************
int mainEmulator() {
    ImprimeMemoria();

    DACInit();

#ifdef use_lib_log_serial
    Serial.printf("Sizeof long:%d\n", sizeof(long));
    Serial.printf("screenx:%d screeny:%d\n", screenx, screeny);
#endif

    while (gb_salir == 0) {
        if (gb_load_new_rom == 1) {
            gb_load_new_rom = 0;
            initLoadROM(); // Cambio de init que lo usa fabgl
            osint_emuloop();
        }
    }

    return 0;
}
