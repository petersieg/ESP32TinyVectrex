#ifndef _GB_CONFIG_H
 #define _GB_CONFIG_H
 
 //Medir rendimiento sin usar fabgl video
 #define use_lib_gfx

 #define use_lib_vectortiny
 
 #define use_lib_regtiny
 
 #define use_lib_cartdridge_flash_ram
 
 //Salida log puerto serie usb
 #define use_lib_log_serial 

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


 //registro flags cc de 8 bits
 #define use_lib_snd_regs_8bits
 #define use_lib_snd_select_8bits
 #define use_lib_via_ora_8bits
 #define use_lib_via_orb_8bits
 #define use_lib_via_ddra_8bits
 #define use_lib_via_ddrb_8bits
 #define use_lib_via_t1on_8bits
 #define use_lib_via_t1int_8bits
 #define use_lib_via_t1pb7_8bits
 #define use_lib_via_t2on_8bits
 #define use_lib_via_t2int_8bits
 #define use_lib_via_srb_8bits
 #define use_lib_via_src_8bits
 #define use_lib_via_srclk_8bits
 #define use_lib_via_acr_8bits
 #define use_lib_via_pcr_8bits
 #define use_lib_via_ifr_8bits
 #define use_lib_via_ier_8bits
 #define use_lib_via_ca2_8bits
 #define use_lib_via_cb2h_8bits
 #define use_lib_alg_jch_8bits

 #define use_lib_reg_cc_8bits
 //register x,y,u,s 16 bits
 #define use_lib_reg_16bits
 #define use_lib_reg_dp_8bits
 #define use_lib_optimice_inst_com
 #define use_lib_optimice_inst_lsr
 #define use_lib_optimice_inst_ror
 #define use_lib_optimice_inst_asr
 #define use_lib_optimice_inst_asl
 #define use_lib_optimice_inst_rol
 #define use_lib_optimice_inst_dec
 #define use_lib_optimice_inst_inc
 #define use_lib_optimice_inst_tst8
 #define use_lib_optimice_inst_tst16

 #define use_lib_optimice_test_z16
 #define use_lib_optimice_test_v
 #define use_lib_optimice_sign_extend
 #define use_lib_optimice_ea_direct
 #define use_lib_optimice_ea_extended
 #define use_lib_optimice_inst_and
 #define use_lib_optimice_ea_indexed
 #define use_lib_optimice_inst_neg 

#endif
