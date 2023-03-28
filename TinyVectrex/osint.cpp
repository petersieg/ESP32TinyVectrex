
#include <math.h>
#include "gbConfig.h"
#include "fabgl.h"
#include "gbGlobals.h"

#include "osint.h"
#include "vecx.h"

#ifndef use_lib_wifi
#ifdef use_lib_cartdridge_flash_ram
#include "dataFlash/gbrom.h"
#endif
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

#ifdef use_lib_wifi
#include "gbWifi.h"
#include "gbWifiConfig.h"
#endif

#define EMU_TIMER 20 // the emulators heart beats at 20 milliseconds

// char *gb_cartfilename=NULL;
unsigned char gb_load_new_rom = 1;
unsigned char gb_id_cur_rom = 0; // rom actual; Default 0 = Minestorm - max_list_rom 6; see file: dataFlash/gbrom.h
//  0 = "rom/gbCartEmpty.h"
//  1 = "rom/gbCartRainy.h"
//  2 = "rom/gbCartOnslaughtElect.h"
//  3 = "rom/gbCartDemo2019party.h"
//  4 = "rom/gbCartDeadline2019.h"
//  5 = "rom/gbCartRaidingparty.h"
//  6 = "rom/gbCartTrex.h"

unsigned char gb_salir = 0;
// unsigned char gb_reset=0;

#ifdef use_lib_vectortiny
static int screenx;
static int screeny;
static int scl_factor;
static int offx;
static int offy;
#else
static long screenx;
static long screeny;
static long scl_factor;
//static float scl_factor_float;
static long offx;
static long offy;
#endif


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

#ifdef use_lib_remove_fabgl_queue
#ifdef use_lib_wifi
static void Clear_bresenham()
#else
static void IRAM_ATTR Clear_bresenham()
#endif
{
    // uint8_t * ptrVideo;
    int tope = VGAController.getViewPortHeight();
    unsigned char topeByte = (VGAController.getViewPortWidth() >> 3); // DIV 8
    for (unsigned int i = 0; i < tope; i++) {
        // uint8_t* gb_buffer_vga= VGAController.getScanline(i);
        // memset(gb_buffer_vga[i],0,128); //1024 DIV 8
        memset(gb_buffer_vga[i], 0, topeByte); // 1024 DIV 8
        // ptrVideo = (uint8_t *)VGAController.sgetScanline(i);
        // memset(ptrVideo,0,128); //1024 DIV 8
    }
}

#ifdef use_lib_wifi
static void draw_pixel_bresenham(short int x, short int y)
#else
static void IRAM_ATTR draw_pixel_bresenham(short int x, short int y)
#endif
{
  //unsigned short int aRow;
    uint8_t *ptrVideo;
    unsigned char aColByte, aColShift;
    unsigned char aBit;
    unsigned int auxOffs;

  //if (x>=0 && x<screenx && y>=0 && y<screeny)  
  //if (x>=0 && x<1024 && y>=0 && y<700)
    {
        // aRow= y;
        aColByte = x >> 3;          // x div 8
        aColShift = 7 - (x & 0x07); // x mod 8

        // gb_buffer_vga[y][x]= pixel;

        aBit = 1 << aColShift;
        // gb_buffer_vga[y][aColByte]= gb_buffer_vga[y][aColByte] | aBit;

        // uint8_t* gb_buffer_vga= VGAController.getScanline(y);
        auxOffs = aColByte;
        gb_buffer_vga[y][auxOffs] = gb_buffer_vga[y][auxOffs] | aBit;
        // ptrVideo = (uint8_t *)VGAController.sgetScanline(y);
        // ptrVideo[auxOffs]= ptrVideo[auxOffs] | aBit;
    }
}

#ifdef use_lib_wifi
static void draw_line_bresenham(short int x1, short int y1, short int x2, short int y2)
#else
static void IRAM_ATTR draw_line_bresenham(short int x1, short int y1, short int x2, short int y2)
#endif
{
    int topeY = VGAController.getViewPortHeight();
    int topeX = VGAController.getViewPortWidth();

    int steep = abs(y2 - y1) > abs(x2 - x1);
    int inc = -1;

    if (steep) {
        swap_short(&x1, &y1);
        swap_short(&x2, &y2);
    }

    if (x1 > x2) {
        swap_short(&x1, &x2);
        swap_short(&y1, &y2);
    }

    if (y1 < y2) {
        inc = 1;
    }

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int y = y1;
    int x = x1;
    int e = 0;

    for (x; x <= x2; x++) {
        if (steep) {
            // draw_pixel( y, x, 255);
            if (y >= 0 && y < topeX && x >= 0 && x < topeY) {
                draw_pixel_bresenham(y, x);
            } else {
                return;
            }
        } else {
            // draw_pixel( x, y, 255);
            if (x >= 0 && x < topeX && y >= 0 && y < topeY) {
                draw_pixel_bresenham(x, y);
            } else {
                return;
            }
        }

        if ((e + dy) << 1 < dx) {
            e = e + dy;
        } else {
            y += inc;
            e = e + dy - dx;
        }
    }
}
#endif

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
    unsigned int vga_begin = micros();

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

    unsigned int vga_end = micros();
    unsigned int time_prev = vga_end - vga_begin;
    gb_stats_video_cur_unified = time_prev;
    if (time_prev < gb_stats_video_min_unified) {
        gb_stats_video_min_unified = time_prev;
    }
    if (time_prev > gb_stats_video_max_unified) {
        gb_stats_video_max_unified = time_prev;
    }
}

void VGA_osint_render() {
    // Portar SDL_FillRect(screen, NULL, 0);
    int v;
    // unsigned int x0,x1,y0,y1;
    int x0, x1, y0, y1;
    unsigned int vga_begin, vga_end;

    vga_begin = micros();

    if (vector_draw_cnt <= 0) {
        vga_end = micros();
        unsigned int time_prev = vga_end - vga_begin;
        gb_stats_video_cur_unified = time_prev;
        if (time_prev < gb_stats_video_min_unified) {
            gb_stats_video_min_unified = time_prev;
        }
        if (time_prev > gb_stats_video_max_unified) {
            gb_stats_video_max_unified = time_prev;
        }
        return;
    }

#ifdef use_lib_gfx

#ifdef use_lib_remove_fabgl_queue
    Clear_bresenham();
#else
    Canvas cv(&VGAController);
    fabgl::Primitive p;
    Point auxPoint;

    // cv.setPenColor(Color::Black); //Borro pantalla SDL_FillRect
    // BEGIN setPenColor
    p.cmd = fabgl::PrimitiveCmd::SetPenColor;
    p.color = Color::Black;
    VGAController.addPrimitive(p);
    // END setPenColor

    // cv.setBrushColor(Color::Black); //Para el clear
    // BEGIN SetBrushColor
    p.cmd = fabgl::PrimitiveCmd::SetBrushColor;
    p.color = Color::Black;
    VGAController.addPrimitive(p);
    // END SetBrushColor

    // cv.clear();
    // Clear BEGIN
    p.cmd = fabgl::PrimitiveCmd::Clear;
    p.ivalue = 0;
    VGAController.addPrimitive(p);
#endif

#ifdef use_lib_vectortiny
// cv.setPenColor(Color::White);
// BEGIN setPenColor
#ifdef use_lib_remove_fabgl_queue
#else
    p.cmd = fabgl::PrimitiveCmd::SetPenColor;
    p.color = Color::White;
    VGAController.addPrimitive(p);
    // END setPenColor
#endif

    for (v = 0; v < vector_draw_cnt; v++) {
        x0 = (int)(vectors_draw[v].x0);
        y0 = (int)(vectors_draw[v].y0);
        x1 = (int)(vectors_draw[v].x1);
        y1 = (int)(vectors_draw[v].y1);

        if ((x0 == x1) && (y0 == y1)) { // Punto
            x0 = offx + x0 / scl_factor;
            y0 = offy + y0 / scl_factor;

#ifdef use_lib_remove_fabgl_queue
            // draw_line_bresenham(x0,y0,x0,x0);
            draw_pixel_bresenham(x0, y0);
#else
            p.cmd = fabgl::PrimitiveCmd::SetPixelAt;
            // auxPoint.X= x0;
            // auxPoint.Y= y0;
            auxPoint = {x0, y0};
            p.pixelDesc = {auxPoint, Color::White};
            VGAController.addPrimitive(p);
            // END setPixel
#endif

        } else { // Linea
            x0 = offx + x0 / scl_factor;
            y0 = offy + y0 / scl_factor;
            x1 = offx + x1 / scl_factor;
            y1 = offy + y1 / scl_factor;
#ifdef use_lib_remove_fabgl_queue
            draw_line_bresenham(x0, y0, x1, y1);
#else
            // BEGIN moveTo
            p.cmd = fabgl::PrimitiveCmd::MoveTo;
            p.position = Point(x0, y0);
            VGAController.addPrimitive(p);
            // End moveTo

            // Begin lineTo
            // Primitive p;
            p.cmd = fabgl::PrimitiveCmd::LineTo;
            p.position = Point(x1, y1);
            VGAController.addPrimitive(p);
            // End lineTo
#endif
        }
    }

#ifdef use_lib_remove_fabgl_queue
#else
    VGAController.processPrimitives();
#endif

    vga_end = micros();
    unsigned int time_prev = vga_end - vga_begin;
    gb_stats_video_cur_unified = time_prev;
    if (time_prev < gb_stats_video_min_unified) {
        gb_stats_video_min_unified = time_prev;
    }
    if (time_prev > gb_stats_video_max_unified) {
        gb_stats_video_max_unified = time_prev;
    }
    // Serial.printf("vga %d\n",(vga_end-vga_begin));
    // fflush(stdout);
#else
    // printf("vcnt:%d\n",vector_draw_cnt);
    // fflush(stdout);
    for (v = 0; v < vector_draw_cnt; v++) {
        Uint8 c = vectors_draw[v].color * 256 / VECTREX_COLORS;
        x0 = vectors_draw[v].x0;
        y0 = vectors_draw[v].y0;
        x1 = vectors_draw[v].x1;
        y1 = vectors_draw[v].y1;

        jj_aalineRGBA(screen, (offx + x0 / scl_factor), (offy + y0 / scl_factor), (offx + x1 / scl_factor),
                      (offy + y1 / scl_factor), c, c, c, 0xff);

    }
#endif

#endif
}

static char *romfilename = "rom.dat";
// static char *romfilename = "OnslaughtElectronBeamPlayboys.bin";
// static char *romfilename = "rainy.bin";
static char *cartfilename = NULL;
// static char *cartfilename = "rainy.bin";

#ifdef use_lib_wifi
// void load_cart_WIFI(char * cadUrl)
void load_cart_WIFI() {
    int auxFileSize = 0;

    // if (strcmp(cadUrl,"")==0)
    if (strcmp(gb_cadUrl, "") == 0) {
#ifdef use_lib_wifi_debug
        Serial.printf("cart name empty\n");
#endif
        return;
    }

#ifdef use_lib_wifi_debug
    Serial.printf("load_cart_WIFI\n");
#endif
#ifdef use_lib_wifi_debug
    Serial.printf("Check WIFI\n");
#endif
    if (Check_WIFI() == false) {
        return;
    }
    int leidos = 0;
#ifdef use_lib_wifi_debug
    // Serial.printf("URL:%s\n",cadUrl);
    Serial.printf("URL:%s\n", gb_cadUrl);
#endif
    // Asignar_URL_stream_WIFI(cadUrl);
    Asignar_URL_stream_WIFI(gb_cadUrl);
    auxFileSize = gb_size_file_wifi;
#ifdef use_lib_wifi_debug
    Serial.printf("Size cart:%d\n", gb_size_file_wifi);
#endif
    Leer_url_stream_WIFI(&leidos);
#ifdef use_lib_wifi_debug
    Serial.printf("Leidos:%d\n", leidos); // Leemos 1024 bytes
#endif

    // He leido 1024 bytes. Lee resto
    int contBuffer = 0;
    int cont1024 = 0;
    while (contBuffer < auxFileSize) {
        if (contBuffer >= sizeof(cart)) {
#ifdef use_lib_wifi_debug
            Serial.printf("Limit exced cart 32768\n");
#endif
            break;
        }
        cart[contBuffer] = gb_buffer_wifi[cont1024];
        contBuffer++;

        cont1024++;
        if (cont1024 >= 1024) {
            Leer_url_stream_WIFI(&leidos);
#ifdef use_lib_wifi_debug
            Serial.printf("Leidos:%d\n", leidos);
#endif
            cont1024 = 0;
        }
    }
}
#endif

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

#ifdef use_lib_cartdridge_flash_ram
#ifdef use_lib_log_serial
#ifdef use_lib_wifi
    Serial.printf("Load cartdridge FLASH WIFI id:%d name:%s\n", gb_id_cur_rom, gb_cadUrl);
#else
    Serial.printf("Load cartdridge FLASH id:%d size:%d\n", gb_id_cur_rom, gb_list_cart_size[gb_id_cur_rom]);
#endif
#endif

#ifdef use_lib_wifi
#else
    int topeCartBytes;
    if (gb_list_cart_size[gb_id_cur_rom] < 32768) {
        topeCartBytes = gb_list_cart_size[gb_id_cur_rom];
    } else {
        topeCartBytes = 32768;
#ifdef use_lib_log_serial
        Serial.printf("Excede tamanio Cartucho 32768 bytes\n");
#endif
    }
#endif

#ifdef use_lib_cartdridge_no_use_ram
    cart = gb_list_rom_data[gb_id_cur_rom];
#else
#ifdef use_lib_wifi
    ShowStatusWIFI(1);
    load_cart_WIFI(); // uso gb_cadUrl
    ShowStatusWIFI(0);
#else
    memcpy(cart, gb_list_rom_data[gb_id_cur_rom], topeCartBytes);
#endif
#endif

#else
    if (cartfilename) {
        FILE *f;
        if (!(f = fopen(cartfilename, "rb"))) {
            perror(cartfilename);
            printf("Invalid cart\n");
            // fflush(stdout);
            exit(EXIT_FAILURE);
        }
        printf("Load cartdridge %s\n", cartfilename);
        // fflush(stdout);
        fread(cart, 1, sizeof(cart), f);
        fclose(f);
    }
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

        // Serial.printf("%d %d %d\n",cpu_end,cpu_begin,(cpu_end-cpu_begin));
        // Serial.printf("%d\n",(cpu_end-cpu_begin));
        gb_currentTime = millis();
        if ((gb_currentTime - gb_fps_time_ini_unified) > 1000) {
            gb_fps_time_ini_unified = gb_currentTime;
            unsigned int aux_fps = gb_fps_unified - gb_fps_ini_unified;
            gb_fps_ini_unified = gb_fps_unified;
#ifdef use_lib_log_serial
            Serial.printf("fps:%d\n", aux_fps);
            //Serial.printf("fps:%d %d m:%d mx:%d v %d m:%d mx:%d\n", aux_fps, gb_stats_time_cur_unified,
            //              gb_stats_time_min_unified, gb_stats_time_max_unified, gb_stats_video_cur_unified,
            //              gb_stats_video_min_unified, gb_stats_video_max_unified);
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
// int main(int argc, char *argv[])
int mainEmulator() {
    ImprimeMemoria();

    DACInit();

#ifdef use_lib_log_serial
    Serial.printf("Sizeof long:%d\n", sizeof(long));
    Serial.printf("screenx:%d screeny:%d\n", screenx, screeny);
#endif

    while (gb_salir == 0) {
        if (gb_load_new_rom == 1) {
            // cartfilename= gb_cartfilename;
            gb_load_new_rom = 0;
            initLoadROM(); // Cambio de init que lo usa fabgl
            osint_emuloop();
        }
    }

    return 0;
}
