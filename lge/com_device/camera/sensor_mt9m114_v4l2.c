/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#undef VT_CAMERA_DBG
#ifdef CDBG
#undef CDBG
#endif

#ifdef VT_CAMERA_DBG
#undef CDBG_VT
#define CDBG_VT(fmt, args...)  do { } while (0)
#define CDBG(fmt, args...)  do { } while (0)
#else
#undef CDBG_VT
#define CDBG_VT(fmt, args...) do { } while (0)
#define CDBG(fmt, args...) do { } while (0)
#endif

#include "msm_sensor.h"
#define SENSOR_NAME "mt9m114"
#define PLATFORM_DRIVER_NAME "msm_camera_mt9m114"
#define mt9m114_obj mt9m114_##obj

/* Sysctl registers */
#define MT9M114_COMMAND_REGISTER                0x0080
#define MT9M114_COMMAND_REGISTER_APPLY_PATCH    (1 << 0)
#define MT9M114_COMMAND_REGISTER_SET_STATE      (1 << 1)
#define MT9M114_COMMAND_REGISTER_REFRESH        (1 << 2)
#define MT9M114_COMMAND_REGISTER_WAIT_FOR_EVENT (1 << 3)
#define MT9M114_COMMAND_REGISTER_OK             (1 << 15)

DEFINE_MUTEX(mt9m114_mut);
static struct msm_sensor_ctrl_t mt9m114_s_ctrl;

enum mt9m114_iso_value {
	CAMERA_ISO_AUTO = 0,
	CAMERA_ISO_100 = 2,
	CAMERA_ISO_200,
	CAMERA_ISO_400,
	CAMERA_ISO_MAX
};

static int prev_effect_mode;
static int prev_balance_mode;
static int prev_iso_mode;
static int prev_antibanding_mode;
static int prev_brightness_mode;
static int prev_fps_mode;
static int prev_awb_mode;
static int prev_aec_mode;
static int prev_ae_metering_mode;

#if defined(CONFIG_MACH_LGE_IJB_BOARD_SKT )|| defined(CONFIG_MACH_LGE_IJB_BOARD_LGU)
static struct msm_camera_i2c_reg_conf init_tbl_sub[] = {
//##### Reset
//Step1-Post-Reset Settings
//POLL_FIELD= COMMAND_REGISTER, HOST_COMMAND_1, !=0, DELAY=10, TIMEOUT=100
//===> 0x0080 번지 1bit자리가 0이 아니면 10ms씩 delay주면서 max100ms안에서 계속 체크.
//DELAY 100

// ###
//{0x301A, 0x0230},     // RESET_REGISTER
//POLL_FIELD= COMMAND_REGISTER, HOST_COMMAND_1, !=0, DELAY=10, TIMEOUT=100
//===>0x0080 번지 1bit자리가 0이 아니면 10ms씩 delay주면서 max100ms안에서 계속 체크.
//DELAY 100
	{0xFFFE, 0x0002},	// john.park, 2011-05-09, To replace Polling check
//{0x301A, 0x0230},     // RESET_REGISTER
	{0x301A, 0x0000},	// john.park, 2011-05-09, To replace Reset reg check

//[Step2-PLL_Timing]
	{0x098E, 0x1000},	// LOGICAL_ADDRESS_ACCESS
	{0x098E, 0xC97E},	// LOGICAL_ADDRESS_ACCESS
	{0x0990, 0x0100},	// CAM_SYSCTL_PLL_ENABLE                
	{0xC980, 0x0660},	//cam_sysctl_pll_divider_m_n = 1632
	{0xC982, 0x0700},	//cam_sysctl_pll_divider_p = 1792
//MIPI Timing                    
	{0xC984, 0x8041},	//cam_port_output_control = 32833
	{0xC988, 0x0F00},	//cam_port_mipi_timing_t_hs_zero = 3840
	{0xC98A, 0x0B07},	//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xC98C, 0x0D01},	//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xC98E, 0x071D},	//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xC990, 0x0006},	//cam_port_mipi_timing_t_lpx = 6
	{0xC992, 0x0A0C},	//cam_port_mipi_timing_init_timing = 2572
//Timing_settings                
	{0xC800, 0x0004},	//cam_sensor_cfg_y_addr_start = 4
	{0xC802, 0x0004},	//cam_sensor_cfg_x_addr_start = 4
	{0xC804, 0x03CB},	//cam_sensor_cfg_y_addr_end = 971
	{0xC806, 0x050B},	//cam_sensor_cfg_x_addr_end = 1291
	{0x098E, 0x4808},	// LOGICAL_ADDRESS_ACCESS               ===== 32
	{0x0990, 0x02DC},	// cam_sensor_cfg_pixclk
	{0x0992, 0x6C00},	// cam_sensor_cfg_pixclk
// 0xC808, 0x2DC1A9E    //cam_sensor_cfg_pixclk = 47979166
	{0xC80C, 0x0001},	//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x00DB},	//cam_sensor_cfg_fine_integ_time_min = 219
	{0xC810, 0x05B3},	//cam_sensor_cfg_fine_integ_time_max = 1469
	{0xC812, 0x03EE},	//cam_sensor_cfg_frame_length_lines = 1500
	{0xC814, 0x0636},	//cam_sensor_cfg_line_length_pck = 1600
	{0xC816, 0x0060},	//cam_sensor_cfg_fine_correction = 96
	{0xC818, 0x03C3},	//cam_sensor_cfg_cpipe_last_row = 963
	{0xC826, 0x0020},	//cam_sensor_cfg_reg_0_data = 32
#if defined(CONFIG_MACH_LGE_325_BOARD) || defined(CONFIG_MACH_LGE_IJB_BOARD_SKT )|| defined(CONFIG_MACH_LGE_IJB_BOARD_LGU)
	{0xC834, 0x0000},	//cam_sensor_control_read_mode = 0                                                                                     // Others
#else //
	{0xC834, 0x0003},	//cam_sensor_control_read_mode = 3 (h&v flip)         // DCM
#endif
	{0xC854, 0x0000},	//cam_crop_window_xoffset = 0
	{0xC856, 0x0000},	//cam_crop_window_yoffset = 0
	{0xC858, 0x0500},	//cam_crop_window_width = 1280                          
	{0xC85A, 0x03C0},	//cam_crop_window_height = 960                          
	{0x098E, 0xC85C},	//LOGICAL_ADDRESS_ACCESS                        ===== 8
	{0x0990, 0x0300},	//cam_crop_cropmode = 3                                         
	{0xC868, 0x0500},	//cam_output_width = 1280                                       
	{0xC86A, 0x03C0},	//cam_output_height = 960                                       
	{0x098E, 0xC878},	//LOGICAL_ADDRESS_ACCESS                        ===== 8
	{0x0990, 0x0000},	//cam_aet_aemode = 0    
	{0xC88C, 0x1E02},	//cam_aet_max_frame_rate = 5120
	{0xC88E, 0x0A00},	//cam_aet_min_frame_rate = 2560
	{0xC914, 0x0000},	//cam_stat_awb_clip_window_xstart = 0
	{0xC916, 0x0000},	//cam_stat_awb_clip_window_ystart = 0
	{0xC918, 0x04FF},	//cam_stat_awb_clip_window_xend = 1279
	{0xC91A, 0x03BF},	//cam_stat_awb_clip_window_yend = 959
	{0xC91C, 0x0000},	//cam_stat_ae_initial_window_xstart = 0
	{0xC91E, 0x0000},	//cam_stat_ae_initial_window_ystart = 0
	{0xC920, 0x00FF},	//cam_stat_ae_initial_window_xend = 255
	{0xC922, 0x00BF},	//cam_stat_ae_initial_window_yend = 191
	{0x098E, 0xE801},	//LOGICAL_ADDRESS_ACCESS                        ===== 8
	{0x0990, 0x0000},	//cam_aet_aemode = 0    
	{0x098E, 0xCC03},	// LOGICAL_ADDRESS_ACCESS [UVC_POWER_LINE_FREQUENCY_CONTROL]                    ===== 8
	{0x0990, 0x0200},	// UVC_POWER_LINE_FREQUENCY_CONTROL  ==> 60Hz : 0x0200 , 50Hz ==>0x0100          
// 0x098E, 0xC88B,      // LOGICAL_ADDRESS_ACCESS [CAM_AET_FLICKER_FREQ_HZ]             ===== 8
// 0x0990, 0x3400,      // CAM_AET_FLICKER_FREQ_HZ ==> 60Hz : 0x3400 , 50Hz ==>0x2C00                   
//[Step3-Recommended] //Optimization
	{0x316A, 0x8270},
	{0x316C, 0x8270},
	{0x3ED0, 0x2305},
	{0x3ED2, 0x77CF},
	{0x316E, 0x8202},
	{0x3180, 0x87FF},
	{0x30D4, 0x6080},
	{0xA802, 0x0008},	// AE_TRACK_MODE
	{0x3E14, 0xFF39},
//{0x301A, 0x0234},
	{0x31E0, 0x0001},	//0x33F4, 0x0009  //저조도 지글지글 
	{0x301A, 0x0000},	// john.park, 2011-05-09, To replace reset reg check

//[Load Patch 1204]
	{0x0982, 0x0001},	// ACCESS_CTL_STAT
	{0x098A, 0x60BC},	// PHYSICAL_ADDRESS_ACCESS
	{0xE0BC, 0xC0F1},
	{0xE0BE, 0x082A},
	{0xE0C0, 0x05A0},
	{0xE0C2, 0xD800},
	{0xE0C4, 0x71CF},
	{0xE0C6, 0xFFFF},
	{0xE0C8, 0xC344},
	{0xE0CA, 0x77CF},
	{0xE0CC, 0xFFFF},
	{0xE0CE, 0xC7C0},
	{0xE0D0, 0xB104},
	{0xE0D2, 0x8F1F},
	{0xE0D4, 0x75CF},
	{0xE0D6, 0xFFFF},
	{0xE0D8, 0xC84C},
	{0xE0DA, 0x0811},
	{0xE0DC, 0x005E},
	{0xE0DE, 0x70CF},
	{0xE0E0, 0x0000},
	{0xE0E2, 0x500E},
	{0xE0E4, 0x7840},
	{0xE0E6, 0xF019},
	{0xE0E8, 0x0CC6},
	{0xE0EA, 0x0340},
	{0xE0EC, 0x0E26},
	{0xE0EE, 0x0340},
	{0xE0F0, 0x95C2},
	{0xE0F2, 0x0E21},
	{0xE0F4, 0x101E},
	{0xE0F6, 0x0E0D},
	{0xE0F8, 0x119E},
	{0xE0FA, 0x0D56},
	{0xE0FC, 0x0340},
	{0xE0FE, 0xF008},
	{0xE100, 0x2650},
	{0xE102, 0x1040},
	{0xE104, 0x0AA2},
	{0xE106, 0x0360},
	{0xE108, 0xB502},
	{0xE10A, 0xB5C2},
	{0xE10C, 0x0B22},
	{0xE10E, 0x0400},
	{0xE110, 0x0CCE},
	{0xE112, 0x0320},
	{0xE114, 0xD800},
	{0xE116, 0x70CF},
	{0xE118, 0xFFFF},
	{0xE11A, 0xC5D4},
	{0xE11C, 0x902C},
	{0xE11E, 0x72CF},
	{0xE120, 0xFFFF},
	{0xE122, 0xE218},
	{0xE124, 0x9009},
	{0xE126, 0xE105},
	{0xE128, 0x73CF},
	{0xE12A, 0xFF00},
	{0xE12C, 0x2FD0},
	{0xE12E, 0x7822},
	{0xE130, 0x7910},
	{0xE132, 0xB202},
	{0xE134, 0x1382},
	{0xE136, 0x0700},
	{0xE138, 0x0815},
	{0xE13A, 0x03DE},
	{0xE13C, 0x1387},
	{0xE13E, 0x0700},
	{0xE140, 0x2102},
	{0xE142, 0x000A},
	{0xE144, 0x212F},
	{0xE146, 0x0288},
	{0xE148, 0x1A04},
	{0xE14A, 0x0284},
	{0xE14C, 0x13B9},
	{0xE14E, 0x0700},
	{0xE150, 0xB8C1},
	{0xE152, 0x0815},
	{0xE154, 0x0052},
	{0xE156, 0xDB00},
	{0xE158, 0x230F},
	{0xE15A, 0x0003},
	{0xE15C, 0x2102},
	{0xE15E, 0x00C0},
	{0xE160, 0x7910},
	{0xE162, 0xB202},
	{0xE164, 0x9507},
	{0xE166, 0x7822},
	{0xE168, 0xE080},
	{0xE16A, 0xD900},
	{0xE16C, 0x20CA},
	{0xE16E, 0x004B},
	{0xE170, 0xB805},
	{0xE172, 0x9533},
	{0xE174, 0x7815},
	{0xE176, 0x6038},
	{0xE178, 0x0FB2},
	{0xE17A, 0x0560},
	{0xE17C, 0xB861},
	{0xE17E, 0xB711},
	{0xE180, 0x0775},
	{0xE182, 0x0540},
	{0xE184, 0xD900},
	{0xE186, 0xF00A},
	{0xE188, 0x70CF},
	{0xE18A, 0xFFFF},
	{0xE18C, 0xE210},
	{0xE18E, 0x7835},
	{0xE190, 0x8041},
	{0xE192, 0x8000},
	{0xE194, 0xE102},
	{0xE196, 0xA040},
	{0xE198, 0x09F1},
	{0xE19A, 0x8094},
	{0xE19C, 0x7FE0},
	{0xE19E, 0xD800},
	{0xE1A0, 0xC0F1},
	{0xE1A2, 0xC5E1},
	{0xE1A4, 0x71CF},
	{0xE1A6, 0x0000},
	{0xE1A8, 0x45E6},
	{0xE1AA, 0x7960},
	{0xE1AC, 0x7508},
	{0xE1AE, 0x70CF},
	{0xE1B0, 0xFFFF},
	{0xE1B2, 0xC84C},
	{0xE1B4, 0x9002},
	{0xE1B6, 0x083D},
	{0xE1B8, 0x021E},
	{0xE1BA, 0x0D39},
	{0xE1BC, 0x10D1},
	{0xE1BE, 0x70CF},
	{0xE1C0, 0xFF00},
	{0xE1C2, 0x3354},
	{0xE1C4, 0x9055},
	{0xE1C6, 0x71CF},
	{0xE1C8, 0xFFFF},
	{0xE1CA, 0xC5D4},
	{0xE1CC, 0x116C},
	{0xE1CE, 0x0103},
	{0xE1D0, 0x1170},
	{0xE1D2, 0x00C1},
	{0xE1D4, 0xE381},
	{0xE1D6, 0x22C6},
	{0xE1D8, 0x0F81},
	{0xE1DA, 0x0000},
	{0xE1DC, 0x00FF},
	{0xE1DE, 0x22C4},
	{0xE1E0, 0x0F82},
	{0xE1E2, 0xFFFF},
	{0xE1E4, 0x00FF},
	{0xE1E6, 0x29C0},
	{0xE1E8, 0x0222},
	{0xE1EA, 0x7945},
	{0xE1EC, 0x7930},
	{0xE1EE, 0xB035},
	{0xE1F0, 0x0715},
	{0xE1F2, 0x0540},
	{0xE1F4, 0xD900},
	{0xE1F6, 0xF00A},
	{0xE1F8, 0x70CF},
	{0xE1FA, 0xFFFF},
	{0xE1FC, 0xE224},
	{0xE1FE, 0x7835},
	{0xE200, 0x8041},
	{0xE202, 0x8000},
	{0xE204, 0xE102},
	{0xE206, 0xA040},
	{0xE208, 0x09F1},
	{0xE20A, 0x8094},
	{0xE20C, 0x7FE0},
	{0xE20E, 0xD800},
	{0xE210, 0xFFFF},
	{0xE212, 0xCB40},
	{0xE214, 0xFFFF},
	{0xE216, 0xE0BC},
	{0xE218, 0x0000},
	{0xE21A, 0x0000},
	{0xE21C, 0x0000},
	{0xE21E, 0x0000},
	{0xE220, 0x0000},
	{0x098E, 0x0000},	// LOGICAL_ADDRESS_ACCESS
//[Apply Patch 1204]
	{0xE000, 0x1184},	// PATCHLDR_LOADER_ADDRESS
	{0xE002, 0x1204},	// PATCHLDR_PATCH_ID
	{0x098E, 0x6004},	// PATCHLDR_FIRMWARE_ID 
	{0x0990, 0x4103},	// PATCHLDR_FIRMWARE_ID 
	{0x0992, 0x0202},	// PATCHLDR_FIRMWARE_ID 
	{0x0080, 0x8000},	// john.park, 2011-05-09, To replace Patch check
//{0x0080, 0xFFF0}, // COMMAND_REGISTER
//POLL_FIELD= COMMAND_REGISTER, HOST_COMMAND_0, !=0, DELAY=10, TIMEOUT=100
//===> 0x0080 번지 0bit자리가 0이 아니면 10ms씩 delay주면서 max100ms안에서 계속 체크. 
//DELAY 100
	{0xFFFF, 0x0032},
	{0x0080, 0x0001},	// john.park, 2011-05-09, To replace Patch check
//{0x0080, 0xFFF1},     // COMMAND_REGISTER
//POLL_FIELD= COMMAND_REGISTER, HOST_COMMAND_0, !=0, DELAY=10, TIMEOUT=100
//===> 0x0080 번지 0bit자리가 0이 아니면 10ms씩 delay주면서 max100ms안에서 계속 체크. 
//DELAY 100   
	{0xFFFF, 0x0032},
	{0xA804, 0x01BF},
//ERROR_IF= COMMAND_REGISTER, HOST_COMMAND_OK, !=1, "Couldn't apply patch",
//===> 0x0080 번지 15bit자리가 1이 아니면 메세지 뿌림.  "Couldn't apply patch" 
//ERROR_IF= PATCHLDR_APPLY_STATUS, !=0, "Apply status non-zero",
//===> 0xE008 번지가 0이 아니면 메세지 뿌림.  "Apply status non-zero"
//DELAY 1
//[Step4-APGA]
//[APGA Settings 90% 2011/10/10 06:01:42]
	{0x3640, 0x00D0},	//      P_G1_P0Q0
	{0x3642, 0xD7AA},	//      P_G1_P0Q1
	{0x3644, 0x0791},	//      P_G1_P0Q2
	{0x3646, 0xD2CD},	//      P_G1_P0Q3
	{0x3648, 0xF8B0},	//      P_G1_P0Q4
	{0x364A, 0x0190},	//      P_R_P0Q0
	{0x364C, 0x90EA},	//      P_R_P0Q1
	{0x364E, 0x0651},	//      P_R_P0Q2
	{0x3650, 0xA54D},	//      P_R_P0Q3
	{0x3652, 0xD190},	//      P_R_P0Q4
	{0x3654, 0x0170},	//      P_B_P0Q0
	{0x3656, 0x51AB},	//      P_B_P0Q1
	{0x3658, 0x4570},	//      P_B_P0Q2
	{0x365A, 0x86AD},	//      P_B_P0Q3
	{0x365C, 0x9590},	//      P_B_P0Q4
	{0x365E, 0x00D0},	//      P_G2_P0Q0
	{0x3660, 0xDB2A},	//      P_G2_P0Q1
	{0x3662, 0x0751},	//      P_G2_P0Q2
	{0x3664, 0xD0CD},	//      P_G2_P0Q3
	{0x3666, 0xF6D0},	//      P_G2_P0Q4
	{0x3680, 0xBB4C},	//      P_G1_P1Q0
	{0x3682, 0x44EB},	//      P_G1_P1Q1
	{0x3684, 0x0BCE},	//      P_G1_P1Q2
	{0x3686, 0xD1AA},	//      P_G1_P1Q3
	{0x3688, 0x368C},	//      P_G1_P1Q4
	{0x368A, 0xACAD},	//      P_R_P1Q0
	{0x368C, 0x1DEC},	//      P_R_P1Q1
	{0x368E, 0x43CE},	//      P_R_P1Q2
	{0x3690, 0x9B2B},	//      P_R_P1Q3
	{0x3692, 0x7A0E},	//      P_R_P1Q4
	{0x3694, 0xAB2B},	//      P_B_P1Q0
	{0x3696, 0x4369},	//      P_B_P1Q1
	{0x3698, 0xB2CC},	//      P_B_P1Q2
	{0x369A, 0x814D},	//      P_B_P1Q3
	{0x369C, 0x216F},	//      P_B_P1Q4
	{0x369E, 0xBD8C},	//      P_G2_P1Q0
	{0x36A0, 0x2EAB},	//      P_G2_P1Q1
	{0x36A2, 0x126E},	//      P_G2_P1Q2
	{0x36A4, 0xA069},	//      P_G2_P1Q3
	{0x36A6, 0x1D2C},	//      P_G2_P1Q4
	{0x36C0, 0x0671},	//      P_G1_P2Q0
	{0x36C2, 0x8B2F},	//      P_G1_P2Q1
	{0x36C4, 0xFBF0},	//      P_G1_P2Q2
	{0x36C6, 0x336E},	//      P_G1_P2Q3
	{0x36C8, 0xD892},	//      P_G1_P2Q4
	{0x36CA, 0x1011},	//      P_R_P2Q0
	{0x36CC, 0xECCD},	//      P_R_P2Q1
	{0x36CE, 0x9D51},	//      P_R_P2Q2
	{0x36D0, 0xCB4E},	//      P_R_P2Q3
	{0x36D2, 0xC272},	//      P_R_P2Q4
	{0x36D4, 0x3D10},	//      P_B_P2Q0
	{0x36D6, 0x888D},	//      P_B_P2Q1
	{0x36D8, 0xDCD0},	//      P_B_P2Q2
	{0x36DA, 0x8E2F},	//      P_B_P2Q3
	{0x36DC, 0x9AD2},	//      P_B_P2Q4
	{0x36DE, 0x0651},	//      P_G2_P2Q0
	{0x36E0, 0x884F},	//      P_G2_P2Q1
	{0x36E2, 0xF750},	//      P_G2_P2Q2
	{0x36E4, 0x2CCE},	//      P_G2_P2Q3
	{0x36E6, 0xDEF2},	//      P_G2_P2Q4
	{0x3700, 0x292F},	//      P_G1_P3Q0
	{0x3702, 0x4AED},	//      P_G1_P3Q1
	{0x3704, 0x6470},	//      P_G1_P3Q2
	{0x3706, 0x9250},	//      P_G1_P3Q3
	{0x3708, 0x9FB3},	//      P_G1_P3Q4
	{0x370A, 0x378F},	//      P_R_P3Q0
	{0x370C, 0xFFAA},	//      P_R_P3Q1
	{0x370E, 0x17D1},	//      P_R_P3Q2
	{0x3710, 0xE66E},	//      P_R_P3Q3
	{0x3712, 0xC9F3},	//      P_R_P3Q4
	{0x3714, 0x224F},	//      P_B_P3Q0
	{0x3716, 0x0AAD},	//      P_B_P3Q1
	{0x3718, 0x01D1},	//      P_B_P3Q2
	{0x371A, 0x2E2F},	//      P_B_P3Q3
	{0x371C, 0x8393},	//      P_B_P3Q4
	{0x371E, 0x2B0F},	//      P_G2_P3Q0
	{0x3720, 0x04EE},	//      P_G2_P3Q1
	{0x3722, 0x5E70},	//      P_G2_P3Q2
	{0x3724, 0xA530},	//      P_G2_P3Q3
	{0x3726, 0x9FD3},	//      P_G2_P3Q4
	{0x3740, 0xFBF0},	//      P_G1_P4Q0
	{0x3742, 0xAFCF},	//      P_G1_P4Q1
	{0x3744, 0xCF53},	//      P_G1_P4Q2
	{0x3746, 0x1032},	//      P_G1_P4Q3
	{0x3748, 0x62D5},	//      P_G1_P4Q4
	{0x374A, 0xCDB0},	//      P_R_P4Q0
	{0x374C, 0xD950},	//      P_R_P4Q1
	{0x374E, 0xBBF3},	//      P_R_P4Q2
	{0x3750, 0x55F2},	//      P_R_P4Q3
	{0x3752, 0x4E95},	//      P_R_P4Q4
	{0x3754, 0xCEAE},	//      P_B_P4Q0
	{0x3756, 0xBA90},	//      P_B_P4Q1
	{0x3758, 0xA353},	//      P_B_P4Q2
	{0x375A, 0x0B12},	//      P_B_P4Q3
	{0x375C, 0x2C75},	//      P_B_P4Q4
	{0x375E, 0xFBF0},	//      P_G2_P4Q0
	{0x3760, 0xC4AF},	//      P_G2_P4Q1
	{0x3762, 0xCFF3},	//      P_G2_P4Q2
	{0x3764, 0x1152},	//      P_G2_P4Q3
	{0x3766, 0x6595},	//      P_G2_P4Q4
	{0x3784, 0x0294},	//      CENTER_COLUMN
	{0x3782, 0x01B0},	//      CENTER_ROW
	{0x37C0, 0x0FE9},	//      P_GR_Q5
	{0x37C2, 0x7768},	//      P_RD_Q5
	{0x37C4, 0xF829},	//      P_BL_Q5
	{0x37C6, 0x1909},	//      P_GB_Q5
	{0x098E, 0x0000},	//      LOGICAL addressing
	{0xC960, 0x0Af0},	//      CAM_PGA_L_CONFIG_COLOUR_TEMP
	{0xC962, 0x7630},	//      CAM_PGA_L_CONFIG_GREEN_RED_Q14
	{0xC964, 0x5CB0},	//      CAM_PGA_L_CONFIG_RED_Q14
	{0xC966, 0x75CE},	//      CAM_PGA_L_CONFIG_GREEN_BLUE_Q14
	{0xC968, 0x6BB8},	//      CAM_PGA_L_CONFIG_BLUE_Q14
	{0xC96A, 0x16A8},	//      CAM_PGA_M_CONFIG_COLOUR_TEMP
	{0xC96C, 0x8090},	//      CAM_PGA_M_CONFIG_GREEN_RED_Q14
	{0xC96E, 0x807C},	//      CAM_PGA_M_CONFIG_RED_Q14
	{0xC970, 0x8099},	//      CAM_PGA_M_CONFIG_GREEN_BLUE_Q14
	{0xC972, 0x7F08},	//      CAM_PGA_M_CONFIG_BLUE_Q14
	{0xC974, 0x1964},	//      CAM_PGA_R_CONFIG_COLOUR_TEMP
	{0xC976, 0x795F},	//      CAM_PGA_R_CONFIG_GREEN_RED_Q14
	{0xC978, 0x6A04},	//      CAM_PGA_R_CONFIG_RED_Q14
	{0xC97A, 0x7918},	//      CAM_PGA_R_CONFIG_GREEN_BLUE_Q14
	{0xC97C, 0x7C22},	//      CAM_PGA_R_CONFIG_BLUE_Q14
	{0xC95E, 0x0003},	//      CAM_PGA_PGA_CONTROL
//[AE]
	{0x098E, 0xC87A},	//                      ===== 8
	{0x0990, 0x3E00},	// CAM_AET_TARGET_AVERAGE_LUMA

//[AE weight update 1]
	{0x098E, 0x2404},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_ALGO]
	{0xA404, 0x0001},	// AE_RULE_ALGO
	{0x098E, 0xA407},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_0_0]
	{0x0990, 0x1900},
	{0x098E, 0xA408},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_0_1]
	{0x0990, 0x1900},
	{0x098E, 0xA409},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_0_2]
	{0x0990, 0x1900},
	{0x098E, 0xA40A},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_0_3]
	{0x0990, 0x1900},
	{0x098E, 0xA40B},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_0_4]
	{0x0990, 0x1900},
	{0x098E, 0xA40C},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_1_0]
	{0x0990, 0x1900},
	{0x098E, 0xA40D},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_1_1]
	{0x0990, 0x4B00},
	{0x098E, 0xA40E},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_1_2]
	{0x0990, 0x4B00},
	{0x098E, 0xA40F},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_1_3]
	{0x0990, 0x4B00},
	{0x098E, 0xA410},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_1_4]
	{0x0990, 0x1900},
	{0x098E, 0xA411},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_2_0]
	{0x0990, 0x1900},
	{0x098E, 0xA412},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_2_1]
	{0x0990, 0x4B00},
	{0x098E, 0xA413},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_2_2]
	{0x0990, 0x6400},
	{0x098E, 0xA414},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_2_3]
	{0x0990, 0x4B00},
	{0x098E, 0xA415},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_2_4]
	{0x0990, 0x1900},
	{0x098E, 0xA416},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_3_0]
	{0x0990, 0x1900},
	{0x098E, 0xA417},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_3_1]
	{0x0990, 0x4B00},
	{0x098E, 0xA418},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_3_2]
	{0x0990, 0x4B00},
	{0x098E, 0xA419},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_3_3]
	{0x0990, 0x4B00},
	{0x098E, 0xA41A},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_3_4]
	{0x0990, 0x1900},
	{0x098E, 0xA41B},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_4_0]
	{0x0990, 0x1900},
	{0x098E, 0xA41C},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_4_1]
	{0x0990, 0x1900},
	{0x098E, 0xA41D},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_4_2]
	{0x0990, 0x1900},
	{0x098E, 0xA41E},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_4_3]
	{0x0990, 0x1900},
	{0x098E, 0xA41F},	// LOGICAL_ADDRESS_ACCESS [AE_RULE_AE_WEIGHT_TABLE_4_4]
	{0x0990, 0x1900},

//[Step5-AWB_CCM]
	{0x098E, 0xC873},
	{0x0990, 0x0100},	// hue angle
	{0xC892, 0x029B},	// CAM_AWB_CCM_L_0
	{0xC894, 0xFF03},	// CAM_AWB_CCM_L_1
	{0xC896, 0xFF8D},	// CAM_AWB_CCM_L_2
	{0xC898, 0xFF4C},	// CAM_AWB_CCM_L_3
	{0xC89A, 0x0329},	// CAM_AWB_CCM_L_4
	{0xC89C, 0xFFED},	// CAM_AWB_CCM_L_5
	{0xC89E, 0xFF94},	// CAM_AWB_CCM_L_6
	{0xC8A0, 0xFEAF},	// CAM_AWB_CCM_L_7
	{0xC8A2, 0x0426},	// CAM_AWB_CCM_L_8
	{0xC8A4, 0x01B6},	// CAM_AWB_CCM_M_0
	{0xC8A6, 0xFFA3},	// CAM_AWB_CCM_M_1
	{0xC8A8, 0xFFD2},	// CAM_AWB_CCM_M_2
	{0xC8AA, 0xFFA6},	// CAM_AWB_CCM_M_3
	{0xC8AC, 0x0227},	// CAM_AWB_CCM_M_4
	{0xC8AE, 0xFFC8},	// CAM_AWB_CCM_M_5
	{0xC8B0, 0xFFDD},	// CAM_AWB_CCM_M_6
	{0xC8B2, 0xFEB3},	// CAM_AWB_CCM_M_7
	{0xC8B4, 0x02EC},	// CAM_AWB_CCM_M_8
	{0xC8B6, 0x0328},	// CAM_AWB_CCM_R_0
	{0xC8B8, 0xFED6},	// CAM_AWB_CCM_R_1
	{0xC8BA, 0xFFE1},	// CAM_AWB_CCM_R_2
	{0xC8BC, 0xFF5D},	// CAM_AWB_CCM_R_3
	{0xC8BE, 0x02B2},	// CAM_AWB_CCM_R_4
	{0xC8C0, 0xFFDE},	// CAM_AWB_CCM_R_5
	{0xC8C2, 0xFFEE},	// CAM_AWB_CCM_R_6
	{0xC8C4, 0xFF4B},	// CAM_AWB_CCM_R_7
	{0xC8C6, 0x01E7},	// CAM_AWB_CCM_R_8
	{0xC8C8, 0x0075},	// CAM_AWB_CCM_L_RG_GAIN
	{0xC8CA, 0x011C},	// CAM_AWB_CCM_L_BG_GAIN
	{0xC8CC, 0x009A},	// CAM_AWB_CCM_M_RG_GAIN
	{0xC8CE, 0x0105},	// CAM_AWB_CCM_M_BG_GAIN
	{0xC8D0, 0x00A4},	// CAM_AWB_CCM_R_RG_GAIN
	{0xC8D2, 0x00AC},	// CAM_AWB_CCM_R_BG_GAIN
	{0xC8D4, 0x0A8C},	// CAM_AWB_CCM_L_CTEMP
	{0xC8D6, 0x0F0A},	// CAM_AWB_CCM_M_CTEMP
	{0xC8D8, 0x1964},	// CAM_AWB_CCM_R_CTEMP
	{0xC914, 0x0000},	// CAM_STAT_AWB_CLIP_WINDOW_XSTART
	{0xC916, 0x0000},	// CAM_STAT_AWB_CLIP_WINDOW_YSTART
	{0xC918, 0x04FF},	// CAM_STAT_AWB_CLIP_WINDOW_XEND
	{0xC91A, 0x03BF},	// CAM_STAT_AWB_CLIP_WINDOW_YEND
	{0xC904, 0x0033},	// CAM_AWB_AWB_XSHIFT_PRE_ADJ
	{0xC906, 0x0040},	// CAM_AWB_AWB_YSHIFT_PRE_ADJ
	{0x098E, 0xC8F2},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0400},	// CAM_AWB_AWB_XSCALE   
	{0x098E, 0xC8F3},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0200},	// CAM_AWB_AWB_XSCALE   
	{0xC8F4, 0x0000},	// CAM_AWB_AWB_WEIGHTS_0
	{0xC8F6, 0x0000},	// CAM_AWB_AWB_WEIGHTS_1
	{0xC8F8, 0x0000},	// CAM_AWB_AWB_WEIGHTS_2
	{0xC8FA, 0xE724},	// CAM_AWB_AWB_WEIGHTS_3
	{0xC8FC, 0x1583},	// CAM_AWB_AWB_WEIGHTS_4
	{0xC8FE, 0x2045},	// CAM_AWB_AWB_WEIGHTS_5
	{0xC900, 0x061C},	// CAM_AWB_AWB_WEIGHTS_6 //0x05DC
	{0xC902, 0x007C},	// CAM_AWB_AWB_WEIGHTS_7
	{0x098E, 0x490A},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_TINTS_CTEMP_THRESHOLD]
	{0xC90A, 0x1838},	// CAM_AWB_TINTS_CTEMP_THRESHOLD
	{0x098E, 0xC90C},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x8000},	// CAM_AWB_K_R_L
	{0x098E, 0xC90D},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x8000},	// CAM_AWB_K_G_L
	{0x098E, 0xC90E},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x8000},	// CAM_AWB_K_B_L
	{0x098E, 0xC90F},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x7400},	// CAM_AWB_K_R_R
	{0x098E, 0xC910},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x8000},	// CAM_AWB_K_G_R
	{0x098E, 0xC911},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x8000},	// CAM_AWB_K_B_R
	{0x098E, 0xC912},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0000},	// CAM_STAT_LUMA_THRESH_LOW
	{0x098E, 0xAC0C},	// LOGICAL_ADDRESS_ACCESS [AWB_B_SCENE_RATIO_LOWER]
	{0x0990, 0x2F00},	// AWB_B_SCENE_RATIO_LOWER //0x3500

	{0x098E, 0xAC16},	// AWB_PRE_AWB_RATIOS_TRACKING_SPEED            // 2011.07.04
	{0x0990, 0x2000},
	{0x098E, 0xAC17},	// AWB_STATISTICS_TRACKING_SPEED                        // 2011.07.04
	{0x0990, 0x2000},

//[Step7-CPIPE_Preference]
	{0x098E, 0x3C02},	// LOGICAL_ADDRESS_ACCESS [LL_MODE]
	{0xBC02, 0x000D},	// LL_MODE
	{0xC926, 0x0020},	// CAM_LL_START_BRIGHTNESS
	{0xC928, 0x009A},	// CAM_LL_STOP_BRIGHTNESS
	{0xC946, 0x0070},	// CAM_LL_START_GAIN_METRIC
	{0xC948, 0x00F3},	// CAM_LL_STOP_GAIN_METRIC
	{0xC952, 0x0020},	// CAM_LL_START_TARGET_LUMA_BM
	{0xC954, 0x009A},	// CAM_LL_STOP_TARGET_LUMA_BM
	{0x098E, 0xC92A},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x7000},	// CAM_LL_START_SATURATION      // 2011.07.04
	{0x098E, 0xC92B},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x6000},	// CAM_LL_END_SATURATION                // 2011.07.04
	{0x098E, 0xC92C},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0000},	// CAM_LL_START_DESATURATION
	{0x098E, 0xC92D},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x9F00},	// CAM_LL_END_DESATURATION
	{0x098E, 0xC92E},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x7000},	// CAM_LL_START_DEMOSAIC                // 2011.07.04
	{0x098E, 0xC95B},	// LOGICAL_ADDRESS_ACCESS                       ===== 8                         
	{0x0990, 0x0000},	// CAM_SEQ_DARK_COLOR_KILL
	{0x098E, 0xC95C},	// LOGICAL_ADDRESS_ACCESS                       ===== 8  
	{0x0990, 0x6200},	// CAM_SEQ_BRIGHT_COLOR_KILL       
	{0x098E, 0x4948},	// LOGICAL_ADDRESS_ACCESS                                 
	{0xC948, 0x01A0},	// CAM_LL_STOP_GAIN_METRIC                                                                                                
	{0x098E, 0xC92F},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0500},	// CAM_LL_START_AP_GAIN
	{0x098E, 0xC930},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0200},	// CAM_LL_START_AP_THRESH
	{0x098E, 0xC931},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x6400},	// CAM_LL_STOP_DEMOSAIC
	{0x098E, 0xC932},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0100},	// CAM_LL_STOP_AP_GAIN          // 2011.07.04
	{0x098E, 0xC933},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0C00},	// CAM_LL_STOP_AP_THRESH
	{0x098E, 0xC934},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x3600},	// CAM_LL_START_NR_RED
	{0x098E, 0xC935},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x1800},	// CAM_LL_START_NR_GREEN
	{0x098E, 0xC936},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x3600},	// CAM_LL_START_NR_BLUE
	{0x098E, 0xC937},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x1800},	// CAM_LL_START_NR_THRESH
	{0x098E, 0xC938},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x5000},	// CAM_LL_STOP_NR_RED
	{0x098E, 0xC939},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x5000},	// CAM_LL_STOP_NR_GREEN
	{0x098E, 0xC93A},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x5000},	// CAM_LL_STOP_NR_BLUE
	{0x098E, 0xC93B},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x5000},	// CAM_LL_STOP_NR_THRESH
	{0xC93C, 0x0020},	// CAM_LL_START_CONTRAST_BM
	{0xC93E, 0x009A},	// CAM_LL_STOP_CONTRAST_BM
	{0xC940, 0x00DC},	// CAM_LL_GAMMA
	{0x098E, 0xC942},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x4000},	// CAM_LL_START_CONTRAST_GRADIENT
// 0xC942, 0x38         // CAM_LL_START_CONTRAST_GRADIENT
	{0x098E, 0xC943},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x3D00},	// CAM_LL_STOP_CONTRAST_GRADIENT
// 0xC943, 0x50         // CAM_LL_STOP_CONTRAST_GRADIENT
	{0x098E, 0xC944},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x6400},	// CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
// 0xC944, 0x50         // CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
	{0x098E, 0xC945},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x6400},	// CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
// 0xC945, 0x19         // CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
	{0xC94A, 0x0230},	// CAM_LL_START_FADE_TO_BLACK_LUMA
	{0xC94C, 0x0010},	// CAM_LL_STOP_FADE_TO_BLACK_LUMA
	{0xC94E, 0x000E},	// CAM_LL_CLUSTER_DC_TH_BM      //2011.08.10 // 2011.07.04(0x01CD)
	{0x098E, 0xC950},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0500},	// CAM_LL_CLUSTER_DC_GATE_PERCENTAGE
	{0x098E, 0xC951},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x4000},	// CAM_LL_SUMMING_SENSITIVITY_FACTOR
	{0x098E, 0xC87B},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x1B00},	// CAM_AET_TARGET_AVERAGE_LUMA_DARK
	{0x098E, 0xC878},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0800},	// CAM_AET_AEMODE = 0
	{0xC890, 0x0080},	// CAM_AET_TARGET_GAIN
	{0xC886, 0x0100},	// CAM_AET_AE_MAX_VIRT_AGAIN
	{0xC87C, 0x000A},	// CAM_AET_BLACK_CLIPPING_TARGET
	{0x098E, 0xB00C},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x2500},	// BLACKLEVEL_MAX_BLACK_LEVEL     
	{0x098E, 0xB00D},	// LOGICAL_ADDRESS_ACCESS [BLACKLEVEL_BLACK_LEVEL_DAMPENING]                    ===== 8
	{0x0990, 0x1000},	// BLACKLEVEL_BLACK_LEVEL_DAMPENING  
	{0x098E, 0xB42A},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0500},	// CCM_DELTA_GAIN
	{0x098E, 0xA80A},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x2000},	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED
//0x098E, 0x488E,       // LOGICAL_ADDRESS_ACCESS [CAM_AET_MIN_FRAME_RATE]
//[Step8-Features]
//{0x098E, 0x0000},     // LOGICAL_ADDRESS_ACCESS
	{0xC984, 0x8041},	// CAM_PORT_OUTPUT_CONTROL
//0x3C5A, 0x0009,               /MIPI_DELAY_TRIM, this line is only needed for Aptina demo camera
	{0x001E, 0x0777},	// PAD_SLEW
	{0x098E, 0xB42C},	// LOGICAL_ADDRESS_ACCESS [CCM_GREY_SATURATION]
	{0x0990, 0x6000},	// CCM_GREY_SATURATION                  ===== 8          // 2011.07.04
//[Change-Config]
//0x098E, 0xDC00,       // LOGICAL_ADDRESS_ACCESS                       ===== 8
//0x0990, 0x2800,               / AE_TRACK_AE_TRACKING_DAMPENING_SPEED
//0x0080, 0x8002,       // COMMAND_REGISTER
//ERROR_IF= SYSMGR_CURRENT_STATE, !=0x31, "System state is not STREAMING",
//===> 0xDC01번지가 0x31이 아니면 메세지 뿌림. "System state is not STREAMING",
//DELAY 1
//DELAY 50
//0xFFFF, 0x0032,
};
#else // for Batman & Cayman
static struct msm_camera_i2c_reg_conf init_tbl_sub[] = {
	{0x301A, 0x0230},	// RESET_REGISTER

	{0xFFFF, 0x0064},	// RESET_REGISTER
	{0x301A, 0x0230},	// RESET_REGISTER

//[Step2-PLL_Timing]
	{0x098E, 0x1000},	// LOGICAL_ADDRESS_ACCESS
	{0x098E, 0xC97E},	// LOGICAL_ADDRESS_ACCESS
	{0x0990, 0x0100},	// CAM_SYSCTL_PLL_ENABLE
	{0xC980, 0x0660},	//cam_sysctl_pll_divider_m_n = 1632
	{0xC982, 0x0700},	//cam_sysctl_pll_divider_p = 1792
//MIPI Timing
	{0xC984, 0x8041},	//cam_port_output_control = 32833
	{0xC988, 0x0F00},	//cam_port_mipi_timing_t_hs_zero = 3840
	{0xC98A, 0x0B07},	//cam_port_mipi_timing_t_hs_exit_hs_trail = 2823
	{0xC98C, 0x0D01},	//cam_port_mipi_timing_t_clk_post_clk_pre = 3329
	{0xC98E, 0x071D},	//cam_port_mipi_timing_t_clk_trail_clk_zero = 1821
	{0xC990, 0x0006},	//cam_port_mipi_timing_t_lpx = 6
	{0xC992, 0x0A0C},	//cam_port_mipi_timing_init_timing = 2572
//Timing_settings
	{0xC800, 0x0004},	//cam_sensor_cfg_y_addr_start = 4
	{0xC802, 0x0004},	//cam_sensor_cfg_x_addr_start = 4
	{0xC804, 0x03CB},	//cam_sensor_cfg_y_addr_end = 971
	{0xC806, 0x050B},	//cam_sensor_cfg_x_addr_end = 1291
	{0x098E, 0x4808},	// LOGICAL_ADDRESS_ACCESS               ===== 32
	{0x0990, 0x02DC},	// cam_sensor_cfg_pixclk
	{0x0992, 0x6C00},	// cam_sensor_cfg_pixclk
// 0xC808, 0x2DC1A9E    //cam_sensor_cfg_pixclk = 47979166
	{0xC80C, 0x0001},	//cam_sensor_cfg_row_speed = 1
	{0xC80E, 0x00DB},	//cam_sensor_cfg_fine_integ_time_min = 219
	{0xC810, 0x05B3},	//cam_sensor_cfg_fine_integ_time_max = 1469
	{0xC812, 0x03EE},	//cam_sensor_cfg_frame_length_lines = 1500
	{0xC814, 0x0636},	//cam_sensor_cfg_line_length_pck = 1600
	{0xC816, 0x0060},	//cam_sensor_cfg_fine_correction = 96
	{0xC818, 0x03C3},	//cam_sensor_cfg_cpipe_last_row = 963
	{0xC826, 0x0020},	//cam_sensor_cfg_reg_0_data = 32
#if defined(CONFIG_MACH_LGE_325_BOARD) || defined(CONFIG_MACH_LGE_IJB_BOARD_SKT )|| defined(CONFIG_MACH_LGE_IJB_BOARD_LGU)
	{0xC834, 0x0000},	//cam_sensor_control_read_mode = 0                              // Others
#else //
	{0xC834, 0x0003},	//cam_sensor_control_read_mode = 3 (h&v flip)   // DCM
#endif
	{0xC854, 0x0000},	//cam_crop_window_xoffset = 0
	{0xC856, 0x0000},	//cam_crop_window_yoffset = 0
	{0xC858, 0x0500},	//cam_crop_window_width = 1280
	{0xC85A, 0x03C0},	//cam_crop_window_height = 960
	{0x098E, 0xC85C},	//LOGICAL_ADDRESS_ACCESS                        ===== 8
	{0x0990, 0x0300},	//cam_crop_cropmode = 3
	{0xC868, 0x0500},	//cam_output_width = 1280
	{0xC86A, 0x03C0},	//cam_output_height = 960
	{0x098E, 0xC878},	//LOGICAL_ADDRESS_ACCESS                        ===== 8
	{0x0990, 0x0000},	//cam_aet_aemode = 0
	{0xC88C, 0x1E02},	//cam_aet_max_frame_rate = 5120
	{0xC88E, 0x0A00},	//cam_aet_min_frame_rate = 2560
	{0xC914, 0x0000},	//cam_stat_awb_clip_window_xstart = 0
	{0xC916, 0x0000},	//cam_stat_awb_clip_window_ystart = 0
	{0xC918, 0x04FF},	//cam_stat_awb_clip_window_xend = 1279
	{0xC91A, 0x03BF},	//cam_stat_awb_clip_window_yend = 959
	{0xC91C, 0x0000},	//cam_stat_ae_initial_window_xstart = 0
	{0xC91E, 0x0000},	//cam_stat_ae_initial_window_ystart = 0
	{0xC920, 0x00FF},	//cam_stat_ae_initial_window_xend = 255
	{0xC922, 0x00BF},	//cam_stat_ae_initial_window_yend = 191
	{0x098E, 0xE801},	//LOGICAL_ADDRESS_ACCESS                        ===== 8
	{0x0990, 0x0000},	//cam_aet_aemode = 0
	{0x098E, 0xCC03},	// LOGICAL_ADDRESS_ACCESS [UVC_POWER_LINE_FREQUENCY_CONTROL]                    ===== 8
	{0x0990, 0x0200},	// UVC_POWER_LINE_FREQUENCY_CONTROL  ==> 60Hz : 0x0200 , 50Hz ==>0x0100
// 0x098E, 0xC88B,      // LOGICAL_ADDRESS_ACCESS [CAM_AET_FLICKER_FREQ_HZ]                     ===== 8
// 0x0990, 0x3400,      // CAM_AET_FLICKER_FREQ_HZ ==> 60Hz : 0x3400 , 50Hz ==>0x2C00
//[Step3-Recommended] //Optimization
	{0x316A, 0x8270},
	{0x316C, 0x8270},
	{0x3ED0, 0x2305},
	{0x3ED2, 0x77CF},
	{0x316E, 0x8202},
	{0x3180, 0x87FF},
	{0x30D4, 0x6080},
	{0xA802, 0x0008},	// AE_TRACK_MODE
	{0x3E14, 0xFF39},
	{0x301A, 0x0234},

//Patch 0202
	{0x0982, 0x0001},	// ACCESS_CTL_STAT
	{0x098A, 0x5000},	// PHYSICAL_ADDRESS_ACCESS
	{0xD000, 0x70CF},
	{0xD002, 0xFFFF},
	{0xD004, 0xC5D4},
	{0xD006, 0x903A},
	{0xD008, 0x2144},
	{0xD00A, 0x0C00},
	{0xD00C, 0x2186},
	{0xD00E, 0x0FF3},
	{0xD010, 0xB844},
	{0xD012, 0xB948},
	{0xD014, 0xE082},
	{0xD016, 0x20CC},
	{0xD018, 0x80E2},
	{0xD01A, 0x21CC},
	{0xD01C, 0x80A2},
	{0xD01E, 0x21CC},
	{0xD020, 0x80E2},
	{0xD022, 0xF404},
	{0xD024, 0xD801},
	{0xD026, 0xF003},
	{0xD028, 0xD800},
	{0xD02A, 0x7EE0},
	{0xD02C, 0xC0F1},
	{0xD02E, 0x08BA},
	{0xD030, 0x0600},
	{0xD032, 0xC1A1},
	{0xD034, 0x76CF},
	{0xD036, 0xFFFF},
	{0xD038, 0xC130},
	{0xD03A, 0x6E04},
	{0xD03C, 0xC040},
	{0xD03E, 0x71CF},
	{0xD040, 0xFFFF},
	{0xD042, 0xC790},
	{0xD044, 0x8103},
	{0xD046, 0x77CF},
	{0xD048, 0xFFFF},
	{0xD04A, 0xC7C0},
	{0xD04C, 0xE001},
	{0xD04E, 0xA103},
	{0xD050, 0xD800},
	{0xD052, 0x0C6A},
	{0xD054, 0x04E0},
	{0xD056, 0xB89E},
	{0xD058, 0x7508},
	{0xD05A, 0x8E1C},
	{0xD05C, 0x0809},
	{0xD05E, 0x0191},
	{0xD060, 0xD801},
	{0xD062, 0xAE1D},
	{0xD064, 0xE580},
	{0xD066, 0x20CA},
	{0xD068, 0x0022},
	{0xD06A, 0x20CF},
	{0xD06C, 0x0522},
	{0xD06E, 0x0C5C},
	{0xD070, 0x04E2},
	{0xD072, 0x21CA},
	{0xD074, 0x0062},
	{0xD076, 0xE580},
	{0xD078, 0xD901},
	{0xD07A, 0x79C0},
	{0xD07C, 0xD800},
	{0xD07E, 0x0BE6},
	{0xD080, 0x04E0},
	{0xD082, 0xB89E},
	{0xD084, 0x70CF},
	{0xD086, 0xFFFF},
	{0xD088, 0xC8D4},
	{0xD08A, 0x9002},
	{0xD08C, 0x0857},
	{0xD08E, 0x025E},
	{0xD090, 0xFFDC},
	{0xD092, 0xE080},
	{0xD094, 0x25CC},
	{0xD096, 0x9022},
	{0xD098, 0xF225},
	{0xD09A, 0x1700},
	{0xD09C, 0x108A},
	{0xD09E, 0x73CF},
	{0xD0A0, 0xFF00},
	{0xD0A2, 0x3174},
	{0xD0A4, 0x9307},
	{0xD0A6, 0x2A04},
	{0xD0A8, 0x103E},
	{0xD0AA, 0x9328},
	{0xD0AC, 0x2942},
	{0xD0AE, 0x7140},
	{0xD0B0, 0x2A04},
	{0xD0B2, 0x107E},
	{0xD0B4, 0x9349},
	{0xD0B6, 0x2942},
	{0xD0B8, 0x7141},
	{0xD0BA, 0x2A04},
	{0xD0BC, 0x10BE},
	{0xD0BE, 0x934A},
	{0xD0C0, 0x2942},
	{0xD0C2, 0x714B},
	{0xD0C4, 0x2A04},
	{0xD0C6, 0x10BE},
	{0xD0C8, 0x130C},
	{0xD0CA, 0x010A},
	{0xD0CC, 0x2942},
	{0xD0CE, 0x7142},
	{0xD0D0, 0x2250},
	{0xD0D2, 0x13CA},
	{0xD0D4, 0x1B0C},
	{0xD0D6, 0x0284},
	{0xD0D8, 0xB307},
	{0xD0DA, 0xB328},
	{0xD0DC, 0x1B12},
	{0xD0DE, 0x02C4},
	{0xD0E0, 0xB34A},
	{0xD0E2, 0xED88},
	{0xD0E4, 0x71CF},
	{0xD0E6, 0xFF00},
	{0xD0E8, 0x3174},
	{0xD0EA, 0x9106},
	{0xD0EC, 0xB88F},
	{0xD0EE, 0xB106},
	{0xD0F0, 0x210A},
	{0xD0F2, 0x8340},
	{0xD0F4, 0xC000},
	{0xD0F6, 0x21CA},
	{0xD0F8, 0x0062},
	{0xD0FA, 0x20F0},
	{0xD0FC, 0x0040},
	{0xD0FE, 0x0B02},
	{0xD100, 0x0320},
	{0xD102, 0xD901},
	{0xD104, 0x07F1},
	{0xD106, 0x05E0},
	{0xD108, 0xC0A1},
	{0xD10A, 0x78E0},
	{0xD10C, 0xC0F1},
	{0xD10E, 0x71CF},
	{0xD110, 0xFFFF},
	{0xD112, 0xC7C0},
	{0xD114, 0xD840},
	{0xD116, 0xA900},
	{0xD118, 0x71CF},
	{0xD11A, 0xFFFF},
	{0xD11C, 0xD02C},
	{0xD11E, 0xD81E},
	{0xD120, 0x0A5A},
	{0xD122, 0x04E0},
	{0xD124, 0xDA00},
	{0xD126, 0xD800},
	{0xD128, 0xC0D1},
	{0xD12A, 0x7EE0},
	{0x098E, 0x0000},	// LOGICAL_ADDRESS_ACCESS
	{0xE000, 0x010C},	// PATCHLDR_LOADER_ADDRESS
	{0xE002, 0x0202},	// PATCHLDR_PATCH_ID
	{0x098E, 0x6004},	// LOGICA_ADDRESS_ACCESS                ===== 32
	{0x0990, 0x4103},	// PATCHLDR_FIRMWARE_ID
	{0x0992, 0x0202},	// PATCHLDR_FIRMWARE_ID
	{0x0080, 0xFFF0},	// COMMAND_REGISTER
	{0xFFFF, 0x0064},

	{0x0080, 0xFFF1},

	{0xFFFF, 0x0064},

	{0x0982, 0x0001},	// ACCESS_CTL_STAT
	{0x098A, 0x512C},	// PHYSICAL_ADDRESS_ACCESS
	{0xD12C, 0x70CF},
	{0xD12E, 0xFFFF},
	{0xD130, 0xC5D4},
	{0xD132, 0x903A},
	{0xD134, 0x2144},
	{0xD136, 0x0C00},
	{0xD138, 0x2186},
	{0xD13A, 0x0FF3},
	{0xD13C, 0xB844},
	{0xD13E, 0x262F},
	{0xD140, 0xF008},
	{0xD142, 0xB948},
	{0xD144, 0x21CC},
	{0xD146, 0x8021},
	{0xD148, 0xD801},
	{0xD14A, 0xF203},
	{0xD14C, 0xD800},
	{0xD14E, 0x7EE0},
	{0xD150, 0xC0F1},
	{0xD152, 0x71CF},
	{0xD154, 0xFFFF},
	{0xD156, 0xC610},
	{0xD158, 0x910E},
	{0xD15A, 0x208C},
	{0xD15C, 0x8014},
	{0xD15E, 0xF418},
	{0xD160, 0x910F},
	{0xD162, 0x208C},
	{0xD164, 0x800F},
	{0xD166, 0xF414},
	{0xD168, 0x9116},
	{0xD16A, 0x208C},
	{0xD16C, 0x800A},
	{0xD16E, 0xF410},
	{0xD170, 0x9117},
	{0xD172, 0x208C},
	{0xD174, 0x8807},
	{0xD176, 0xF40C},
	{0xD178, 0x9118},
	{0xD17A, 0x2086},
	{0xD17C, 0x0FF3},
	{0xD17E, 0xB848},
	{0xD180, 0x080D},
	{0xD182, 0x0090},
	{0xD184, 0xFFEA},
	{0xD186, 0xE081},
	{0xD188, 0xD801},
	{0xD18A, 0xF203},
	{0xD18C, 0xD800},
	{0xD18E, 0xC0D1},
	{0xD190, 0x7EE0},
	{0xD192, 0x78E0},
	{0xD194, 0xC0F1},
	{0xD196, 0x71CF},
	{0xD198, 0xFFFF},
	{0xD19A, 0xC610},
	{0xD19C, 0x910E},
	{0xD19E, 0x208C},
	{0xD1A0, 0x800A},
	{0xD1A2, 0xF418},
	{0xD1A4, 0x910F},
	{0xD1A6, 0x208C},
	{0xD1A8, 0x8807},
	{0xD1AA, 0xF414},
	{0xD1AC, 0x9116},
	{0xD1AE, 0x208C},
	{0xD1B0, 0x800A},
	{0xD1B2, 0xF410},
	{0xD1B4, 0x9117},
	{0xD1B6, 0x208C},
	{0xD1B8, 0x8807},
	{0xD1BA, 0xF40C},
	{0xD1BC, 0x9118},
	{0xD1BE, 0x2086},
	{0xD1C0, 0x0FF3},
	{0xD1C2, 0xB848},
	{0xD1C4, 0x080D},
	{0xD1C6, 0x0090},
	{0xD1C8, 0xFFD9},
	{0xD1CA, 0xE080},
	{0xD1CC, 0xD801},
	{0xD1CE, 0xF203},
	{0xD1D0, 0xD800},
	{0xD1D2, 0xF1DF},
	{0xD1D4, 0x9040},
	{0xD1D6, 0x71CF},
	{0xD1D8, 0xFFFF},
	{0xD1DA, 0xC5D4},
	{0xD1DC, 0xB15A},
	{0xD1DE, 0x9041},
	{0xD1E0, 0x73CF},
	{0xD1E2, 0xFFFF},
	{0xD1E4, 0xC7D0},
	{0xD1E6, 0xB140},
	{0xD1E8, 0x9042},
	{0xD1EA, 0xB141},
	{0xD1EC, 0x9043},
	{0xD1EE, 0xB142},
	{0xD1F0, 0x9044},
	{0xD1F2, 0xB143},
	{0xD1F4, 0x9045},
	{0xD1F6, 0xB147},
	{0xD1F8, 0x9046},
	{0xD1FA, 0xB148},
	{0xD1FC, 0x9047},
	{0xD1FE, 0xB14B},
	{0xD200, 0x9048},
	{0xD202, 0xB14C},
	{0xD204, 0x9049},
	{0xD206, 0x1958},
	{0xD208, 0x0084},
	{0xD20A, 0x904A},
	{0xD20C, 0x195A},
	{0xD20E, 0x0084},
	{0xD210, 0x8856},
	{0xD212, 0x1B36},
	{0xD214, 0x8082},
	{0xD216, 0x8857},
	{0xD218, 0x1B37},
	{0xD21A, 0x8082},
	{0xD21C, 0x904C},
	{0xD21E, 0x19A7},
	{0xD220, 0x009C},
	{0xD222, 0x881A},
	{0xD224, 0x7FE0},
	{0xD226, 0x1B54},
	{0xD228, 0x8002},
	{0xD22A, 0x78E0},
	{0xD22C, 0x71CF},
	{0xD22E, 0xFFFF},
	{0xD230, 0xC350},
	{0xD232, 0xD828},
	{0xD234, 0xA90B},
	{0xD236, 0x8100},
	{0xD238, 0x01C5},
	{0xD23A, 0x0320},
	{0xD23C, 0xD900},
	{0xD23E, 0x78E0},
	{0xD240, 0x220A},
	{0xD242, 0x1F80},
	{0xD244, 0xFFFF},
	{0xD246, 0xD4E0},
	{0xD248, 0xC0F1},
	{0xD24A, 0x0811},
	{0xD24C, 0x0051},
	{0xD24E, 0x2240},
	{0xD250, 0x1200},
	{0xD252, 0xFFE1},
	{0xD254, 0xD801},
	{0xD256, 0xF006},
	{0xD258, 0x2240},
	{0xD25A, 0x1900},
	{0xD25C, 0xFFDE},
	{0xD25E, 0xD802},
	{0xD260, 0x1A05},
	{0xD262, 0x1002},
	{0xD264, 0xFFF2},
	{0xD266, 0xF195},
	{0xD268, 0xC0F1},
	{0xD26A, 0x0E7E},
	{0xD26C, 0x05C0},
	{0xD26E, 0x75CF},
	{0xD270, 0xFFFF},
	{0xD272, 0xC84C},
	{0xD274, 0x9502},
	{0xD276, 0x77CF},
	{0xD278, 0xFFFF},
	{0xD27A, 0xC344},
	{0xD27C, 0x2044},
	{0xD27E, 0x008E},
	{0xD280, 0xB8A1},
	{0xD282, 0x0926},
	{0xD284, 0x03E0},
	{0xD286, 0xB502},
	{0xD288, 0x9502},
	{0xD28A, 0x952E},
	{0xD28C, 0x7E05},
	{0xD28E, 0xB5C2},
	{0xD290, 0x70CF},
	{0xD292, 0xFFFF},
	{0xD294, 0xC610},
	{0xD296, 0x099A},
	{0xD298, 0x04A0},
	{0xD29A, 0xB026},
	{0xD29C, 0x0E02},
	{0xD29E, 0x0560},
	{0xD2A0, 0xDE00},
	{0xD2A2, 0x0A12},
	{0xD2A4, 0x0320},
	{0xD2A6, 0xB7C4},
	{0xD2A8, 0x0B36},
	{0xD2AA, 0x03A0},
	{0xD2AC, 0x70C9},
	{0xD2AE, 0x9502},
	{0xD2B0, 0x7608},
	{0xD2B2, 0xB8A8},
	{0xD2B4, 0xB502},
	{0xD2B6, 0x70CF},
	{0xD2B8, 0x0000},
	{0xD2BA, 0x5536},
	{0xD2BC, 0x7860},
	{0xD2BE, 0x2686},
	{0xD2C0, 0x1FFB},
	{0xD2C2, 0x9502},
	{0xD2C4, 0x78C5},
	{0xD2C6, 0x0631},
	{0xD2C8, 0x05E0},
	{0xD2CA, 0xB502},
	{0xD2CC, 0x72CF},
	{0xD2CE, 0xFFFF},
	{0xD2D0, 0xC5D4},
	{0xD2D2, 0x923A},
	{0xD2D4, 0x73CF},
	{0xD2D6, 0xFFFF},
	{0xD2D8, 0xC7D0},
	{0xD2DA, 0xB020},
	{0xD2DC, 0x9220},
	{0xD2DE, 0xB021},
	{0xD2E0, 0x9221},
	{0xD2E2, 0xB022},
	{0xD2E4, 0x9222},
	{0xD2E6, 0xB023},
	{0xD2E8, 0x9223},
	{0xD2EA, 0xB024},
	{0xD2EC, 0x9227},
	{0xD2EE, 0xB025},
	{0xD2F0, 0x9228},
	{0xD2F2, 0xB026},
	{0xD2F4, 0x922B},
	{0xD2F6, 0xB027},
	{0xD2F8, 0x922C},
	{0xD2FA, 0xB028},
	{0xD2FC, 0x1258},
	{0xD2FE, 0x0101},
	{0xD300, 0xB029},
	{0xD302, 0x125A},
	{0xD304, 0x0101},
	{0xD306, 0xB02A},
	{0xD308, 0x1336},
	{0xD30A, 0x8081},
	{0xD30C, 0xA836},
	{0xD30E, 0x1337},
	{0xD310, 0x8081},
	{0xD312, 0xA837},
	{0xD314, 0x12A7},
	{0xD316, 0x0701},
	{0xD318, 0xB02C},
	{0xD31A, 0x1354},
	{0xD31C, 0x8081},
	{0xD31E, 0x7FE0},
	{0xD320, 0xA83A},
	{0xD322, 0x78E0},
	{0xD324, 0xC0F1},
	{0xD326, 0x0DC2},
	{0xD328, 0x05C0},
	{0xD32A, 0x7608},
	{0xD32C, 0x09BB},
	{0xD32E, 0x0010},
	{0xD330, 0x75CF},
	{0xD332, 0xFFFF},
	{0xD334, 0xD4E0},
	{0xD336, 0x8D21},
	{0xD338, 0x8D00},
	{0xD33A, 0x2153},
	{0xD33C, 0x0003},
	{0xD33E, 0xB8C0},
	{0xD340, 0x8D45},
	{0xD342, 0x0B23},
	{0xD344, 0x0000},
	{0xD346, 0xEA8F},
	{0xD348, 0x0915},
	{0xD34A, 0x001E},
	{0xD34C, 0xFF81},
	{0xD34E, 0xE808},
	{0xD350, 0x2540},
	{0xD352, 0x1900},
	{0xD354, 0xFFDE},
	{0xD356, 0x8D00},
	{0xD358, 0xB880},
	{0xD35A, 0xF004},
	{0xD35C, 0x8D00},
	{0xD35E, 0xB8A0},
	{0xD360, 0xAD00},
	{0xD362, 0x8D05},
	{0xD364, 0xE081},
	{0xD366, 0x20CC},
	{0xD368, 0x80A2},
	{0xD36A, 0xDF00},
	{0xD36C, 0xF40A},
	{0xD36E, 0x71CF},
	{0xD370, 0xFFFF},
	{0xD372, 0xC84C},
	{0xD374, 0x9102},
	{0xD376, 0x7708},
	{0xD378, 0xB8A6},
	{0xD37A, 0x2786},
	{0xD37C, 0x1FFE},
	{0xD37E, 0xB102},
	{0xD380, 0x0B42},
	{0xD382, 0x0180},
	{0xD384, 0x0E3E},
	{0xD386, 0x0180},
	{0xD388, 0x0F4A},
	{0xD38A, 0x0160},
	{0xD38C, 0x70C9},
	{0xD38E, 0x8D05},
	{0xD390, 0xE081},
	{0xD392, 0x20CC},
	{0xD394, 0x80A2},
	{0xD396, 0xF429},
	{0xD398, 0x76CF},
	{0xD39A, 0xFFFF},
	{0xD39C, 0xC84C},
	{0xD39E, 0x082D},
	{0xD3A0, 0x0051},
	{0xD3A2, 0x70CF},
	{0xD3A4, 0xFFFF},
	{0xD3A6, 0xC90C},
	{0xD3A8, 0x8805},
	{0xD3AA, 0x09B6},
	{0xD3AC, 0x0360},
	{0xD3AE, 0xD908},
	{0xD3B0, 0x2099},
	{0xD3B2, 0x0802},
	{0xD3B4, 0x9634},
	{0xD3B6, 0xB503},
	{0xD3B8, 0x7902},
	{0xD3BA, 0x1523},
	{0xD3BC, 0x1080},
	{0xD3BE, 0xB634},
	{0xD3C0, 0xE001},
	{0xD3C2, 0x1D23},
	{0xD3C4, 0x1002},
	{0xD3C6, 0xF00B},
	{0xD3C8, 0x9634},
	{0xD3CA, 0x9503},
	{0xD3CC, 0x6038},
	{0xD3CE, 0xB614},
	{0xD3D0, 0x153F},
	{0xD3D2, 0x1080},
	{0xD3D4, 0xE001},
	{0xD3D6, 0x1D3F},
	{0xD3D8, 0x1002},
	{0xD3DA, 0xFFA4},
	{0xD3DC, 0x9602},
	{0xD3DE, 0x7F05},
	{0xD3E0, 0xD800},
	{0xD3E2, 0xB6E2},
	{0xD3E4, 0xAD05},
	{0xD3E6, 0x0511},
	{0xD3E8, 0x05E0},
	{0xD3EA, 0xD800},
	{0xD3EC, 0xC0F1},
	{0xD3EE, 0x0CFE},
	{0xD3F0, 0x05C0},
	{0xD3F2, 0x0A96},
	{0xD3F4, 0x05A0},
	{0xD3F6, 0x7608},
	{0xD3F8, 0x0C22},
	{0xD3FA, 0x0240},
	{0xD3FC, 0xE080},
	{0xD3FE, 0x20CA},
	{0xD400, 0x0F82},
	{0xD402, 0x0000},
	{0xD404, 0x190B},
	{0xD406, 0x0C60},
	{0xD408, 0x05A2},
	{0xD40A, 0x21CA},
	{0xD40C, 0x0022},
	{0xD40E, 0x0C56},
	{0xD410, 0x0240},
	{0xD412, 0xE806},
	{0xD414, 0x0E0E},
	{0xD416, 0x0220},
	{0xD418, 0x70C9},
	{0xD41A, 0xF048},
	{0xD41C, 0x0896},
	{0xD41E, 0x0440},
	{0xD420, 0x0E96},
	{0xD422, 0x0400},
	{0xD424, 0x0966},
	{0xD426, 0x0380},
	{0xD428, 0x75CF},
	{0xD42A, 0xFFFF},
	{0xD42C, 0xD4E0},
	{0xD42E, 0x8D00},
	{0xD430, 0x084D},
	{0xD432, 0x001E},
	{0xD434, 0xFF47},
	{0xD436, 0x080D},
	{0xD438, 0x0050},
	{0xD43A, 0xFF57},
	{0xD43C, 0x0841},
	{0xD43E, 0x0051},
	{0xD440, 0x8D04},
	{0xD442, 0x9521},
	{0xD444, 0xE064},
	{0xD446, 0x790C},
	{0xD448, 0x702F},
	{0xD44A, 0x0CE2},
	{0xD44C, 0x05E0},
	{0xD44E, 0xD964},
	{0xD450, 0x72CF},
	{0xD452, 0xFFFF},
	{0xD454, 0xC700},
	{0xD456, 0x9235},
	{0xD458, 0x0811},
	{0xD45A, 0x0043},
	{0xD45C, 0xFF3D},
	{0xD45E, 0x080D},
	{0xD460, 0x0051},
	{0xD462, 0xD801},
	{0xD464, 0xFF77},
	{0xD466, 0xF025},
	{0xD468, 0x9501},
	{0xD46A, 0x9235},
	{0xD46C, 0x0911},
	{0xD46E, 0x0003},
	{0xD470, 0xFF49},
	{0xD472, 0x080D},
	{0xD474, 0x0051},
	{0xD476, 0xD800},
	{0xD478, 0xFF72},
	{0xD47A, 0xF01B},
	{0xD47C, 0x0886},
	{0xD47E, 0x03E0},
	{0xD480, 0xD801},
	{0xD482, 0x0EF6},
	{0xD484, 0x03C0},
	{0xD486, 0x0F52},
	{0xD488, 0x0340},
	{0xD48A, 0x0DBA},
	{0xD48C, 0x0200},
	{0xD48E, 0x0AF6},
	{0xD490, 0x0440},
	{0xD492, 0x0C22},
	{0xD494, 0x0400},
	{0xD496, 0x0D72},
	{0xD498, 0x0440},
	{0xD49A, 0x0DC2},
	{0xD49C, 0x0200},
	{0xD49E, 0x0972},
	{0xD4A0, 0x0440},
	{0xD4A2, 0x0D3A},
	{0xD4A4, 0x0220},
	{0xD4A6, 0xD820},
	{0xD4A8, 0x0BFA},
	{0xD4AA, 0x0260},
	{0xD4AC, 0x70C9},
	{0xD4AE, 0x0451},
	{0xD4B0, 0x05C0},
	{0xD4B2, 0x78E0},
	{0xD4B4, 0xD900},
	{0xD4B6, 0xF00A},
	{0xD4B8, 0x70CF},
	{0xD4BA, 0xFFFF},
	{0xD4BC, 0xD520},
	{0xD4BE, 0x7835},
	{0xD4C0, 0x8041},
	{0xD4C2, 0x8000},
	{0xD4C4, 0xE102},
	{0xD4C6, 0xA040},
	{0xD4C8, 0x09F1},
	{0xD4CA, 0x8114},
	{0xD4CC, 0x71CF},
	{0xD4CE, 0xFFFF},
	{0xD4D0, 0xD4E0},
	{0xD4D2, 0x70CF},
	{0xD4D4, 0xFFFF},
	{0xD4D6, 0xC594},
	{0xD4D8, 0xB03A},
	{0xD4DA, 0x7FE0},
	{0xD4DC, 0xD800},
	{0xD4DE, 0x0000},
	{0xD4E0, 0x0000},
	{0xD4E2, 0x0500},
	{0xD4E4, 0x0500},
	{0xD4E6, 0x0200},
	{0xD4E8, 0x0330},
	{0xD4EA, 0x0000},
	{0xD4EC, 0x0000},
	{0xD4EE, 0x03CD},
	{0xD4F0, 0x050D},
	{0xD4F2, 0x01C5},
	{0xD4F4, 0x03B3},
	{0xD4F6, 0x00E0},
	{0xD4F8, 0x01E3},
	{0xD4FA, 0x0280},
	{0xD4FC, 0x01E0},
	{0xD4FE, 0x0109},
	{0xD500, 0x0080},
	{0xD502, 0x0500},
	{0xD504, 0x0000},
	{0xD506, 0x0000},
	{0xD508, 0x0000},
	{0xD50A, 0x0000},
	{0xD50C, 0x0000},
	{0xD50E, 0x0000},
	{0xD510, 0x0000},
	{0xD512, 0x0000},
	{0xD514, 0x0000},
	{0xD516, 0x0000},
	{0xD518, 0x0000},
	{0xD51A, 0x0000},
	{0xD51C, 0x0000},
	{0xD51E, 0x0000},
	{0xD520, 0xFFFF},
	{0xD522, 0xC9B4},
	{0xD524, 0xFFFF},
	{0xD526, 0xD324},
	{0xD528, 0xFFFF},
	{0xD52A, 0xCA34},
	{0xD52C, 0xFFFF},
	{0xD52E, 0xD3EC},
	{0x098E, 0x0000},	// LOGICAL_ADDRESS_ACCESS
	{0xE000, 0x04B4},	// PATCHLDR_LOADER_ADDRESS
	{0xE002, 0x0302},	// PATCHLDR_PATCH_ID
	{0x098E, 0x6004},	// LOGICAL_ADDRESS_ACCESS               ===== 32
	{0x0990, 0x4103},	// PATCHLDR_FIRMWARE_ID
	{0x0992, 0x0202},	// PATCHLDR_FIRMWARE_ID
	{0x0080, 0xFFF0},	// COMMAND_REGISTER

	{0xFFFF, 0x0064},
	{0x0080, 0xFFF1},	// COMMAND_REGISTER

	{0xFFFF, 0x0064},

//[Step4-APGA]
//[APGA Settings 100% 2011/09/22 11:59:56]
	{0x3640, 0x0130},	//  P_G1_P0Q0
	{0x3642, 0x5CCC},	//  P_G1_P0Q1
	{0x3644, 0x7450},	//  P_G1_P0Q2
	{0x3646, 0xF06D},	//  P_G1_P0Q3
	{0x3648, 0xFC4F},	//  P_G1_P0Q4
	{0x364A, 0x0270},	//  P_R_P0Q0
	{0x364C, 0x1FCC},	//  P_R_P0Q1
	{0x364E, 0x7590},	//  P_R_P0Q2
	{0x3650, 0x956D},	//  P_R_P0Q3
	{0x3652, 0xBA4F},	//  P_R_P0Q4
	{0x3654, 0x0330},	//  P_B_P0Q0
	{0x3656, 0x2CCD},	//  P_B_P0Q1
	{0x3658, 0x3630},	//  P_B_P0Q2
	{0x365A, 0xC3CD},	//  P_B_P0Q3
	{0x365C, 0xB92F},	//  P_B_P0Q4
	{0x365E, 0x0130},	//  P_G2_P0Q0
	{0x3660, 0x5B8C},	//  P_G2_P0Q1
	{0x3662, 0x7410},	//  P_G2_P0Q2
	{0x3664, 0xEDED},	//  P_G2_P0Q3
	{0x3666, 0xF96F},	//  P_G2_P0Q4
	{0x3680, 0x102D},	//  P_G1_P1Q0
	{0x3682, 0x4ACC},	//  P_G1_P1Q1
	{0x3684, 0xEACA},	//  P_G1_P1Q2
	{0x3686, 0xD3EE},	//  P_G1_P1Q3
	{0x3688, 0xD16F},	//  P_G1_P1Q4
	{0x368A, 0x6B4C},	//  P_R_P1Q0
	{0x368C, 0xD1AB},	//  P_R_P1Q1
	{0x368E, 0x652D},	//  P_R_P1Q2
	{0x3690, 0x5DCB},	//  P_R_P1Q3
	{0x3692, 0xC80F},	//  P_R_P1Q4
	{0x3694, 0x124D},	//  P_B_P1Q0
	{0x3696, 0xC02D},	//  P_B_P1Q1
	{0x3698, 0xD08C},	//  P_B_P1Q2
	{0x369A, 0x584D},	//  P_B_P1Q3
	{0x369C, 0xC40E},	//  P_B_P1Q4
	{0x369E, 0x0F6D},	//  P_G2_P1Q0
	{0x36A0, 0x4A8C},	//  P_G2_P1Q1
	{0x36A2, 0x8D88},	//  P_G2_P1Q2
	{0x36A4, 0xD5AE},	//  P_G2_P1Q3
	{0x36A6, 0xE16F},	//  P_G2_P1Q4
	{0x36C0, 0x1091},	//  P_G1_P2Q0
	{0x36C2, 0x614B},	//  P_G1_P2Q1
	{0x36C4, 0x5EF1},	//  P_G1_P2Q2
	{0x36C6, 0xBBB0},	//  P_G1_P2Q3
	{0x36C8, 0x8074},	//  P_G1_P2Q4
	{0x36CA, 0x0DB1},	//  P_R_P2Q0
	{0x36CC, 0x1ECD},	//  P_R_P2Q1
	{0x36CE, 0x0312},	//  P_R_P2Q2
	{0x36D0, 0xC1D0},	//  P_R_P2Q3
	{0x36D2, 0x8FF4},	//  P_R_P2Q4
	{0x36D4, 0x5E90},	//  P_B_P2Q0
	{0x36D6, 0x226E},	//  P_B_P2Q1
	{0x36D8, 0x74D1},	//  P_B_P2Q2
	{0x36DA, 0x9DD1},	//  P_B_P2Q3
	{0x36DC, 0x85F4},	//  P_B_P2Q4
	{0x36DE, 0x10B1},	//  P_G2_P2Q0
	{0x36E0, 0x356C},	//  P_G2_P2Q1
	{0x36E2, 0x6611},	//  P_G2_P2Q2
	{0x36E4, 0xC890},	//  P_G2_P2Q3
	{0x36E6, 0x84B4},	//  P_G2_P2Q4
	{0x3700, 0x15CC},	//  P_G1_P3Q0
	{0x3702, 0xB40D},	//  P_G1_P3Q1
	{0x3704, 0xB4D1},	//  P_G1_P3Q2
	{0x3706, 0x5FD0},	//  P_G1_P3Q3
	{0x3708, 0x21B3},	//  P_G1_P3Q4
	{0x370A, 0xD60D},	//  P_R_P3Q0
	{0x370C, 0xCD0E},	//  P_R_P3Q1
	{0x370E, 0xDF10},	//  P_R_P3Q2
	{0x3710, 0x53F0},	//  P_R_P3Q3
	{0x3712, 0x3172},	//  P_R_P3Q4
	{0x3714, 0xE34A},	//  P_B_P3Q0
	{0x3716, 0x8BCC},	//  P_B_P3Q1
	{0x3718, 0xBA31},	//  P_B_P3Q2
	{0x371A, 0x09B0},	//  P_B_P3Q3
	{0x371C, 0x0473},	//  P_B_P3Q4
	{0x371E, 0x18AC},	//  P_G2_P3Q0
	{0x3720, 0xB90D},	//  P_G2_P3Q1
	{0x3722, 0xBE11},	//  P_G2_P3Q2
	{0x3724, 0x6610},	//  P_G2_P3Q3
	{0x3726, 0x27D3},	//  P_G2_P3Q4
	{0x3740, 0x8229},	//  P_G1_P4Q0
	{0x3742, 0xA450},	//  P_G1_P4Q1
	{0x3744, 0x88B5},	//  P_G1_P4Q2
	{0x3746, 0x4FF2},	//  P_G1_P4Q3
	{0x3748, 0x2DF6},	//  P_G1_P4Q4
	{0x374A, 0x50B0},	//  P_R_P4Q0
	{0x374C, 0x9131},	//  P_R_P4Q1
	{0x374E, 0x8C55},	//  P_R_P4Q2
	{0x3750, 0x1C33},	//  P_R_P4Q3
	{0x3752, 0x39F6},	//  P_R_P4Q4
	{0x3754, 0x1030},	//  P_B_P4Q0
	{0x3756, 0xB091},	//  P_B_P4Q1
	{0x3758, 0x8795},	//  P_B_P4Q2
	{0x375A, 0x7833},	//  P_B_P4Q3
	{0x375C, 0x47D6},	//  P_B_P4Q4
	{0x375E, 0xDBAA},	//  P_G2_P4Q0
	{0x3760, 0xBA10},	//  P_G2_P4Q1
	{0x3762, 0x8B95},	//  P_G2_P4Q2
	{0x3764, 0x6192},	//  P_G2_P4Q3
	{0x3766, 0x3456},	//  P_G2_P4Q4
	{0x3784, 0x02A8},	//  CENTER_COLUMN
	{0x3782, 0x01F4},	//  CENTER_ROW
	{0x37C0, 0xF96A},	//  P_GR_Q5
	{0x37C2, 0x8C2B},	//  P_RD_Q5
	{0x37C4, 0xDE4B},	//  P_BL_Q5
	{0x37C6, 0x80AB},	//  P_GB_Q5
	{0x098E, 0x0000},	//  LOGICAL addressing
	{0xC960, 0x0C80},	//  CAM_PGA_L_CONFIG_COLOUR_TEMP
	{0xC962, 0x79B6},	//  CAM_PGA_L_CONFIG_GREEN_RED_Q14
	{0xC964, 0x5AA8},	//  CAM_PGA_L_CONFIG_RED_Q14
	{0xC966, 0x79BC},	//  CAM_PGA_L_CONFIG_GREEN_BLUE_Q14
	{0xC968, 0x75A6},	//  CAM_PGA_L_CONFIG_BLUE_Q14
	{0xC96A, 0x1004},	//  CAM_PGA_M_CONFIG_COLOUR_TEMP
	{0xC96C, 0x7E0D},	//  CAM_PGA_M_CONFIG_GREEN_RED_Q14
	{0xC96E, 0x7DD0},	//  CAM_PGA_M_CONFIG_RED_Q14
	{0xC970, 0x7DFE},	//  CAM_PGA_M_CONFIG_GREEN_BLUE_Q14
	{0xC972, 0x7C87},	//  CAM_PGA_M_CONFIG_BLUE_Q14
	{0xC974, 0x1964},	//  CAM_PGA_R_CONFIG_COLOUR_TEMP
	{0xC976, 0x7BC0},	//  CAM_PGA_R_CONFIG_GREEN_RED_Q14
	{0xC978, 0x62D4},	//  CAM_PGA_R_CONFIG_RED_Q14
	{0xC97A, 0x7BB5},	//  CAM_PGA_R_CONFIG_GREEN_BLUE_Q14
	{0xC97C, 0x7404},	//  CAM_PGA_R_CONFIG_BLUE_Q14
	{0xC95E, 0x0003},	//  CAM_PGA_PGA_CONTROL
//[AE]
	{0x098E, 0xC87A},	//                      ===== 8
	{0x0990, 0x4e00},	// CAM_AET_TARGET_AVERAGE_LUMA
	{0x098E, 0xBC07},	//                      ===== 8
	{0x0990, 0x0000},	// LL_GAMMA_SELECT
//[Gamma]
	{0x098E, 0xBC0A},	//                      ===== 8
	{0x0990, 0x0000},	// LL_GAMMA_CONTRAST_CURVE_0
	{0x098E, 0xBC0B},	//                      ===== 8
	{0x0990, 0x0A00},	// LL_GAMMA_CONTRAST_CURVE_1
	{0x098E, 0xBC0C},	//                      ===== 8
	{0x0990, 0x1D00},	// LL_GAMMA_CONTRAST_CURVE_2
	{0x098E, 0xBC0D},	//                      ===== 8
	{0x0990, 0x3700},	// LL_GAMMA_CONTRAST_CURVE_3
	{0x098E, 0xBC0E},	//                      ===== 8
	{0x0990, 0x5800},	// LL_GAMMA_CONTRAST_CURVE_4
	{0x098E, 0xBC0F},	//                      ===== 8
	{0x0990, 0x7100},	// LL_GAMMA_CONTRAST_CURVE_5
	{0x098E, 0xBC10},	//                      ===== 8
	{0x0990, 0x8600},	// LL_GAMMA_CONTRAST_CURVE_6
	{0x098E, 0xBC11},	//                      ===== 8
	{0x0990, 0x9800},	// LL_GAMMA_CONTRAST_CURVE_7
	{0x098E, 0xBC12},	//                      ===== 8
	{0x0990, 0xA700},	// LL_GAMMA_CONTRAST_CURVE_8
	{0x098E, 0xBC13},	//                      ===== 8
	{0x0990, 0xAA00},	// LL_GAMMA_CONTRAST_CURVE_9
	{0x098E, 0xBC14},	//                      ===== 8
	{0x0990, 0xB400},	// LL_GAMMA_CONTRAST_CURVE_10
	{0x098E, 0xBC15},	//                      ===== 8
	{0x0990, 0xB900},	// LL_GAMMA_CONTRAST_CURVE_11
	{0x098E, 0xBC16},	//                      ===== 8
	{0x0990, 0xBE00},	// LL_GAMMA_CONTRAST_CURVE_12
	{0x098E, 0xBC17},	//                      ===== 8
	{0x0990, 0xC300},	// LL_GAMMA_CONTRAST_CURVE_13
	{0x098E, 0xBC18},	//                      ===== 8
	{0x0990, 0xC800},	// LL_GAMMA_CONTRAST_CURVE_14
	{0x098E, 0xBC19},	//                      ===== 8
	{0x0990, 0xCD00},	// LL_GAMMA_CONTRAST_CURVE_15
	{0x098E, 0xBC1A},	//                      ===== 8
	{0x0990, 0xD200},	// LL_GAMMA_CONTRAST_CURVE_16
	{0x098E, 0xBC1B},	//                      ===== 8
	{0x0990, 0xD700},	// LL_GAMMA_CONTRAST_CURVE_17
	{0x098E, 0xBC1C},	//                      ===== 8
	{0x0990, 0xDC00},	// LL_GAMMA_CONTRAST_CURVE_18
	{0x098E, 0xBC1F},	//                      ===== 8
	{0x0990, 0x1D00},	// LL_GAMMA_NRCURVE_2
	{0x098E, 0xBC20},	//                      ===== 8
	{0x0990, 0x3700},	// LL_GAMMA_NRCURVE_3
	{0x098E, 0xBC21},	//                      ===== 8
	{0x0990, 0x5800},	// LL_GAMMA_NRCURVE_4
	{0x098E, 0xBC22},	//                      ===== 8
	{0x0990, 0x7100},	// LL_GAMMA_NRCURVE_5

//[Step5-AWB_CCM]
	{0xC892, 0x020F},	// CAM_AWB_CCM_L_0
	{0xC894, 0xFF84},	// CAM_AWB_CCM_L_1
	{0xC896, 0xFFB7},	// CAM_AWB_CCM_L_2
	{0xC898, 0xFF3A},	// CAM_AWB_CCM_L_3
	{0xC89A, 0x0209},	// CAM_AWB_CCM_L_4
	{0xC89C, 0x0004},	// CAM_AWB_CCM_L_5
	{0xC89E, 0xFF8C},	// CAM_AWB_CCM_L_6
	{0xC8A0, 0xFF16},	// CAM_AWB_CCM_L_7
	{0xC8A2, 0x028A},	// CAM_AWB_CCM_L_8
	{0xC8A4, 0x01C5},	// CAM_AWB_CCM_M_0
	{0xC8A6, 0xFFA8},	// CAM_AWB_CCM_M_1
	{0xC8A8, 0xFFCE},	// CAM_AWB_CCM_M_2
	{0xC8AA, 0xFFAA},	// CAM_AWB_CCM_M_3
	{0xC8AC, 0x01B3},	// CAM_AWB_CCM_M_4
	{0xC8AE, 0xFFE6},	// CAM_AWB_CCM_M_5
	{0xC8B0, 0xFFD7},	// CAM_AWB_CCM_M_6
	{0xC8B2, 0xFEF5},	// CAM_AWB_CCM_M_7
	{0xC8B4, 0x0260},	// CAM_AWB_CCM_M_8
	{0xC8B6, 0x0211},	// CAM_AWB_CCM_R_0
	{0xC8B8, 0xFF59},	// CAM_AWB_CCM_R_1
	{0xC8BA, 0xFFC2},	// CAM_AWB_CCM_R_2
	{0xC8BC, 0xFF84},	// CAM_AWB_CCM_R_3
	{0xC8BE, 0x01CD},	// CAM_AWB_CCM_R_4
	{0xC8C0, 0xFFE8},	// CAM_AWB_CCM_R_5
	{0xC8C2, 0xFFD1},	// CAM_AWB_CCM_R_6
	{0xC8C4, 0xFF82},	// CAM_AWB_CCM_R_7
	{0xC8C6, 0x01DD},	// CAM_AWB_CCM_R_8
	{0xC8C8, 0x0075},	// CAM_AWB_CCM_L_RG_GAIN
	{0xC8CA, 0x011C},	// CAM_AWB_CCM_L_BG_GAIN
	{0xC8CC, 0x009A},	// CAM_AWB_CCM_M_RG_GAIN
	{0xC8CE, 0x0105},	// CAM_AWB_CCM_M_BG_GAIN
	{0xC8D0, 0x00A4},	// CAM_AWB_CCM_R_RG_GAIN
	{0xC8D2, 0x00AC},	// CAM_AWB_CCM_R_BG_GAIN
	{0xC8D4, 0x0A8C},	// CAM_AWB_CCM_L_CTEMP
	{0xC8D6, 0x0F0A},	// CAM_AWB_CCM_M_CTEMP
	{0xC8D8, 0x1964},	// CAM_AWB_CCM_R_CTEMP
	{0xC914, 0x0000},	// CAM_STAT_AWB_CLIP_WINDOW_XSTART
	{0xC916, 0x0000},	// CAM_STAT_AWB_CLIP_WINDOW_YSTART
	{0xC918, 0x04FF},	// CAM_STAT_AWB_CLIP_WINDOW_XEND
	{0xC91A, 0x03BF},	// CAM_STAT_AWB_CLIP_WINDOW_YEND
	{0xC904, 0x0033},	// CAM_AWB_AWB_XSHIFT_PRE_ADJ
	{0xC906, 0x0040},	// CAM_AWB_AWB_YSHIFT_PRE_ADJ
	{0x098E, 0xC8F2},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0400},	// CAM_AWB_AWB_XSCALE
	{0x098E, 0xC8F3},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0200},	// CAM_AWB_AWB_XSCALE
	{0xC8F4, 0x0000},	// CAM_AWB_AWB_WEIGHTS_0
	{0xC8F6, 0x0000},	// CAM_AWB_AWB_WEIGHTS_1
	{0xC8F8, 0x0000},	// CAM_AWB_AWB_WEIGHTS_2
	{0xC8FA, 0xE724},	// CAM_AWB_AWB_WEIGHTS_3
	{0xC8FC, 0x1583},	// CAM_AWB_AWB_WEIGHTS_4
	{0xC8FE, 0x2045},	// CAM_AWB_AWB_WEIGHTS_5
	{0xC900, 0x061C},	// 0x05DC,              // CAM_AWB_AWB_WEIGHTS_6
	{0xC902, 0x007C},	// CAM_AWB_AWB_WEIGHTS_7
	{0x098E, 0xC90C},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x8000},	// CAM_AWB_K_R_L
	{0x098E, 0xC90D},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x9000},	// CAM_AWB_K_G_L
	{0x098E, 0xC90E},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x8000},	// CAM_AWB_K_B_L
	{0x098E, 0xC90F},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x7300},	// CAM_AWB_K_R_R
	{0x098E, 0xC910},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x7F00},	// CAM_AWB_K_G_R                // 2012-05-31 Greenish or Not?
	{0x098E, 0xC911},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x7000},	// CAM_AWB_K_B_R
	{0x098E, 0xC912},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0000},	// CAM_STAT_LUMA_THRESH_LOW
	{0x098E, 0xAC0C},	// LOGICAL_ADDRESS_ACCESS [AWB_B_SCENE_RATIO_LOWER]
	{0x0990, 0x2F00},	// AWB_B_SCENE_RATIO_LOWER
	{0x098E, 0xAC16},	// AWB_PRE_AWB_RATIOS_TRACKING_SPEED   // 2011.07.04
	{0x0990, 0x2000},
	{0x098E, 0xAC17},	// AWB_STATISTICS_TRACKING_SPEED  // 2011.07.04
	{0x0990, 0x2000},
//[Step7-CPIPE_Preference]
	{0x098E, 0x3C02},	// LOGICAL_ADDRESS_ACCESS [LL_MODE]
	{0xBC02, 0x000D},	// LL_MODE
	{0xC926, 0x0020},	// CAM_LL_START_BRIGHTNESS
	{0xC928, 0x009A},	// CAM_LL_STOP_BRIGHTNESS
	{0xC946, 0x0070},	// CAM_LL_START_GAIN_METRIC
	{0xC948, 0x00F3},	// CAM_LL_STOP_GAIN_METRIC
	{0xC952, 0x0020},	// CAM_LL_START_TARGET_LUMA_BM
	{0xC954, 0x009A},	// CAM_LL_STOP_TARGET_LUMA_BM
	{0x098E, 0xC92A},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x7000},	// CAM_LL_START_SATURATION      // 2011.07.04
	{0x098E, 0xC92B},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x6000},	// CAM_LL_END_SATURATION                // 2011.07.04
	{0x098E, 0xC92C},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0000},	// CAM_LL_START_DESATURATION
	{0x098E, 0xC92D},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x9F00},	// CAM_LL_END_DESATURATION
	{0x098E, 0xC92E},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x6400},	//{0x0990, 0x7000},     // CAM_LL_START_DEMOSAIC                // 2011.07.04   // SKT IOT 2nd (2012-01-07) : Change Sharpness
	{0x098E, 0xC95B},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0000},	// CAM_SEQ_DARK_COLOR_KILL
	{0x098E, 0xC95C},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x6200},	// CAM_SEQ_BRIGHT_COLOR_KILL
	{0x098E, 0x4948},	// LOGICAL_ADDRESS_ACCESS
	{0xC948, 0x01A0},	// CAM_LL_STOP_GAIN_METRIC
	{0x098E, 0xC92F},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0700},	//{0x0990, 0x0500},     // CAM_LL_START_AP_GAIN                                                 // SKT IOT 2nd (2012-01-07) : Change Sharpness
	{0x098E, 0xC930},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0100},	// CAM_LL_START_AP_THRESH
	{0x098E, 0xC931},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x6400},	// CAM_LL_STOP_DEMOSAIC
	{0x098E, 0xC932},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0100},	// CAM_LL_STOP_AP_GAIN          // 2011.07.04
	{0x098E, 0xC933},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0C00},	// CAM_LL_STOP_AP_THRESH
	{0x098E, 0xC934},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x3200},	//{0x0990, 0x3600},     // CAM_LL_START_NR_RED                                                  // SKT IOT 2nd (2012-01-07) : Change Sharpness
	{0x098E, 0xC935},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x1400},	//{0x0990, 0x1800},     // CAM_LL_START_NR_GREEN                                                        // SKT IOT 2nd (2012-01-07) : Change Sharpness
	{0x098E, 0xC936},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x3200},	//{0x0990, 0x3600},     // CAM_LL_START_NR_BLUE                                                 // SKT IOT 2nd (2012-01-07) : Change Sharpness
	{0x098E, 0xC937},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x1400},	//{0x0990, 0x1800},     // CAM_LL_START_NR_THRESH                                               // SKT IOT 2nd (2012-01-07) : Change Sharpness
	{0x098E, 0xC938},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x5000},	// CAM_LL_STOP_NR_RED
	{0x098E, 0xC939},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x5000},	// CAM_LL_STOP_NR_GREEN
	{0x098E, 0xC93A},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x5000},	// CAM_LL_STOP_NR_BLUE
	{0x098E, 0xC93B},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x5000},	// CAM_LL_STOP_NR_THRESH
	{0xC93C, 0x0020},	// CAM_LL_START_CONTRAST_BM
	{0xC93E, 0x009A},	// CAM_LL_STOP_CONTRAST_BM
	{0xC940, 0x00Df},	// CAM_LL_GAMMA
	{0x098E, 0xC942},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x4000},	// CAM_LL_START_CONTRAST_GRADIENT
	{0x098E, 0xC943},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x3D00},	// CAM_LL_STOP_CONTRAST_GRADIENT
	{0x098E, 0xC944},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x6400},	// CAM_LL_START_CONTRAST_LUMA_PERCENTAGE
	{0x098E, 0xC945},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x2800},	// CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE
	{0xC94A, 0x0230},	// CAM_LL_START_FADE_TO_BLACK_LUMA
	{0xC94C, 0x0010},	// CAM_LL_STOP_FADE_TO_BLACK_LUMA
	{0xC94E, 0x000E},	//{0xC94E, 0x01CD},     // CAM_LL_CLUSTER_DC_TH_BM                                      // SKT IOT 2nd (2012-01-07) : Change Sharpness
	{0x098E, 0xC950},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0500},	// CAM_LL_CLUSTER_DC_GATE_PERCENTAGE
	{0x098E, 0xC951},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x4000},	// CAM_LL_SUMMING_SENSITIVITY_FACTOR
	{0x098E, 0xC87B},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x1B00},	// CAM_AET_TARGET_AVERAGE_LUMA_DARK
	{0x098E, 0xC878},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0800},	// CAM_AET_AEMODE = 0
	{0xC890, 0x0080},	// CAM_AET_TARGET_GAIN
	{0xC882, 0x00E0},	//0x00AF//Dgain
	{0xC886, 0x01F8},	// CAM_AET_AE_MAX_VIRT_AGAIN
	{0xC87C, 0x0005},	// CAM_AET_BLACK_CLIPPING_TARGET
	{0x098E, 0xB00C},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x2500},	// BLACKLEVEL_MAX_BLACK_LEVEL
	{0x098E, 0xB00D},	// LOGICAL_ADDRESS_ACCESS [BLACKLEVEL_BLACK_LEVEL_DAMPENING]                    ===== 8
	{0x0990, 0x1000},	// BLACKLEVEL_BLACK_LEVEL_DAMPENING
	{0x098E, 0xB42A},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0500},	// CCM_DELTA_GAIN
	{0x098E, 0xA80A},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x2000},	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED

	{0xC984, 0x8041},	// CAM_PORT_OUTPUT_CONTROL

	{0x001E, 0x0777},	// PAD_SLEW

	{0xFFFF, 0x0032},
};
#endif

/* Preview  register settings	*/
static struct msm_camera_i2c_reg_conf mode_preview_tbl_sub[] = {
//DELAY 50
	//[Change-Config]
	{0x098E, 0xDC00},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x2800},	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED
	{0x0080, 0x8002},	// COMMAND_REGISTER
	{0xFFFE, 0x0002},	// john.park, 2011-05-09, To replace Polling check
};

#if 0
/* Snapshot register settings */
static struct msm_camera_i2c_reg_conf mode_snapshot_tbl_sub[] = {
//DELAY 50
	//[Change-Config]
	{0x098E, 0xDC00, 0, 0},	// LOGICAL_ADDRESS_ACCESS===== 8
	{0x0990, 0x2800, 0, 0},	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED
	{0x0080, 0x8002, 0, 0},	// COMMAND_REGISTER
	{0x0080, 0x0002, MSM_CAMERA_I2C_UNSET_WORD_MASK, MSM_CAMERA_I2C_CMD_POLL},	// john.park 2011-05-09, To replace Polling check
};
#endif

/* Change-config register settings */
static struct msm_camera_i2c_reg_conf change_config_tbl_sub[] = {
//DELAY 50
	//[Change-Config]
	{0x098E, 0xDC00},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x2800},	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED
	{0x0080, 0x8002},	// COMMAND_REGISTER
	{0xFFFE, 0x0002},	// john.park, 2011-05-09, To replace Polling check
};

/* Effect register settings */
static struct msm_camera_i2c_reg_conf effect_default_tbl_sub[] = {
	{0x098E, 0xC874},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0000},	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED
};

static struct msm_camera_i2c_reg_conf effect_mono_tbl_sub[] = {
	{0x098E, 0xC874},	// LOGICAL_ADDRESS_ACCESS                       ===== 8
	{0x0990, 0x0100},	// AE_TRACK_AE_TRACKING_DAMPENING_SPEED
};

static struct msm_camera_i2c_reg_conf effect_sepia_tbl_sub[] = {
	{0x098E, 0xC874},	// LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
	{0x0990, 0x0200},	// CAM_SFX_CONTROL                              ========== 8
	{0x098E, 0xC876},	// LOGICAL_ADDRESS_ACCESS [CAM_SFX_SEPIA_CR]
	{0x0990, 0x1E00},	// CAM_SFX_SEPIA_CR                             ========== 8
	{0x098E, 0xC877},	// LOGICAL_ADDRESS_ACCESS [CAM_SFX_SEPIA_CB]
	{0x0990, 0xE800},	// CAM_SFX_SEPIA_CR                             ========== 8
};

static struct msm_camera_i2c_reg_conf effect_aqua_tbl_sub[] = {
	{0x098E, 0xC874},	// LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
	{0x0990, 0x0200},	// CAM_SFX_CONTROL                              ========== 8
	{0x098E, 0xC876},	// LOGICAL_ADDRESS_ACCESS [CAM_SFX_SEPIA_CR]
	{0x0990, 0x0600},	// CAM_SFX_SEPIA_CR                             ========== 8
	{0x098E, 0xC877},	// LOGICAL_ADDRESS_ACCESS [CAM_SFX_SEPIA_CB]
	{0x0990, 0xE000},	// CAM_SFX_SEPIA_CR                             ========== 8
};

static struct msm_camera_i2c_reg_conf effect_negative_tbl_sub[] = {
	{0x098E, 0xC874},	// LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
	{0x0990, 0x0300},	// CAM_SFX_CONTROL                              ========== 8
};

static struct msm_camera_i2c_reg_conf effect_solarization_tbl_sub[] = {
	{0x098E, 0xC874},	// LOGICAL_ADDRESS_ACCESS [CAM_SFX_CONTROL]
	{0x0990, 0x0400},	// CAM_SFX_CONTROL                              ========== 8
};

/* White balance register settings */
static struct msm_camera_i2c_reg_conf wb_default_tbl_sub[] = {
	{0x098E, 0xC909},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_AWBMODE]
	{0x0990, 0x0300},	// CAM_AWB_AWBMODE                                      ========== 8
	{0x098E, 0xC90F},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_R_R]
	{0X0990, 0X8000},
	{0x098E, 0xC910},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_G_R]
	{0X0990, 0X8000},
	{0x098E, 0xC911},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_B_R]
	{0X0990, 0X8000},
};

static struct msm_camera_i2c_reg_conf wb_sunny_tbl_sub[] = {
	{0x098E, 0xC909},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_AWBMODE]
	{0x0990, 0x0100},	// CAM_AWB_AWBMODE                                      ========== 8
	{0x098E, 0xC90F},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_R_R]
	{0X0990, 0X8000},
	{0x098E, 0xC910},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_G_R]
	{0X0990, 0X8000},
	{0x098E, 0xC911},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_B_R]
	{0X0990, 0X7800},
	{0xC8F0, 0x1964},	// CAM_AWB_COLOR_TEMPERATURE
};

static struct msm_camera_i2c_reg_conf wb_cloudy_tbl_sub[] = {
	{0x098E, 0xC909},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_AWBMODE]
	{0x0990, 0x0100},	// CAM_AWB_AWBMODE                                      ========== 8
	{0x098E, 0xC90F},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_R_R]
	{0X0990, 0x9D00},
	{0x098E, 0xC910},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_G_R]
	{0X0990, 0x8000},
	{0x098E, 0xC911},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_B_R]
	{0X0990, 0x5C00},
	{0xC8F0, 0x1964},	// CAM_AWB_COLOR_TEMPERATURE
};

static struct msm_camera_i2c_reg_conf wb_fluorescent_tbl_sub[] = {
	{0x098E, 0xC909},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_AWBMODE]
	{0x0990, 0x0100},	// CAM_AWB_AWBMODE                                      ========== 8
	{0x098E, 0xC90F},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_R_R]
	{0X0990, 0X9000},
	{0x098E, 0xC910},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_G_R]
	{0X0990, 0X8000},
	{0x098E, 0xC911},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_B_R]
	{0X0990, 0X9F00},
	{0xC8F0, 0x11E6},	// CAM_AWB_COLOR_TEMPERATURE
};

static struct msm_camera_i2c_reg_conf wb_incandescent_tbl_sub[] = {
	{0x098E, 0xC909},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_AWBMODE]
	{0x0990, 0x0100},	// CAM_AWB_AWBMODE                                      ========== 8
	{0x098E, 0xC90F},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_R_R]
	{0X0990, 0X1800},
	{0x098E, 0xC910},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_G_R]
	{0X0990, 0X8000},
	{0x098E, 0xC911},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_K_B_R]
	{0X0990, 0XA300},
	{0xC8F0, 0x0C80},	// CAM_AWB_COLOR_TEMPERATURE
};

/* ISO register settings */
static struct msm_camera_i2c_reg_conf iso_default_tbl_sub[] = {
	{0x098E, 0x488C},	// LOGICAL_ADDRESS_ACCESS [CAM_AET_MAX_FRAME_RATE]
	{0xC88C, 0x1292},	//cam_aet_max_frame_rate = 7680
	{0xC886, 0x0100},	// CAM_AET_AE_MAX_VIRT_AGAIN
	{0xC890, 0x0080},	// CAM_AET_TARGET_GAIN
};

static struct msm_camera_i2c_reg_conf iso_100_tbl_sub[] = {
	{0x098E, 0x488C},	// LOGICAL_ADDRESS_ACCESS [CAM_AET_MAX_FRAME_RATE]
	{0xC88C, 0x0F00},	// CAM_AET_MAX_FRAME_RATE
	{0xC886, 0x0080},	// CAM_AET_AE_MAX_VIRT_AGAIN
	{0xC890, 0x0080},	// CAM_AET_TARGET_GAIN
};

static struct msm_camera_i2c_reg_conf iso_200_tbl_sub[] = {
	{0x098E, 0x488C},	// LOGICAL_ADDRESS_ACCESS [CAM_AET_MAX_FRAME_RATE]
	{0xC88C, 0x1200},	// CAM_AET_MAX_FRAME_RATE
	{0xC886, 0x00C0},	// CAM_AET_AE_MAX_VIRT_AGAIN
	{0xC890, 0x0080},	// CAM_AET_TARGET_GAIN
};

static struct msm_camera_i2c_reg_conf iso_400_tbl_sub[] = {
	{0x098E, 0x488C},	// LOGICAL_ADDRESS_ACCESS [CAM_AET_MAX_FRAME_RATE]
	{0xC88C, 0x1E00},	// CAM_AET_MAX_FRAME_RATE
	{0xC886, 0x0100},	// CAM_AET_AE_MAX_VIRT_AGAIN
	{0xC890, 0x0080},	// CAM_AET_TARGET_GAIN
};

static struct v4l2_subdev_info mt9m114_subdev_info[] = {
	{
	 .code = V4L2_MBUS_FMT_YUYV8_2X8,
	 .colorspace = V4L2_COLORSPACE_JPEG,
	 .fmt = 1,
	 .order = 0,
	 },
	/* more can be supported, to be added later */
};

static void mt9m114_start_stream(struct msm_sensor_ctrl_t *s_ctrl)
{
	msm_sensor_start_stream_114(s_ctrl);
};

static void mt9m114_stop_stream(struct msm_sensor_ctrl_t *s_ctrl)
{

	prev_effect_mode = -1;

};

static struct msm_camera_i2c_conf_array mt9m114_init_conf[] = {
	{&init_tbl_sub[0],
	 ARRAY_SIZE(init_tbl_sub), 0, MSM_CAMERA_I2C_WORD_DATA},
};

static struct msm_camera_i2c_conf_array mt9m114_confs[] = {
	{&mode_preview_tbl_sub[0],
	 ARRAY_SIZE(mode_preview_tbl_sub), 0, MSM_CAMERA_I2C_WORD_DATA},
};

static struct msm_camera_i2c_reg_conf mt9m114_saturation[][1] = {
	{{0xCC12, 0x00},},
	{{0xCC12, 0x1A},},
	{{0xCC12, 0x34},},
	{{0xCC12, 0x4E},},
	{{0xCC12, 0x68},},
	{{0xCC12, 0x80},},
	{{0xCC12, 0x9A},},
	{{0xCC12, 0xB4},},
	{{0xCC12, 0xCE},},
	{{0xCC12, 0xE8},},
	{{0xCC12, 0xFF},},
};

static struct msm_camera_i2c_reg_conf mt9m114_refresh[] = {
	{MT9M114_COMMAND_REGISTER, MT9M114_COMMAND_REGISTER_REFRESH,
	 MSM_CAMERA_I2C_UNSET_WORD_MASK, MSM_CAMERA_I2C_CMD_POLL},
	{MT9M114_COMMAND_REGISTER, (MT9M114_COMMAND_REGISTER_OK |
				    MT9M114_COMMAND_REGISTER_REFRESH),
	 MSM_CAMERA_I2C_WORD_DATA,
	 MSM_CAMERA_I2C_CMD_WRITE},
	{MT9M114_COMMAND_REGISTER, MT9M114_COMMAND_REGISTER_REFRESH,
	 MSM_CAMERA_I2C_UNSET_WORD_MASK, MSM_CAMERA_I2C_CMD_POLL},
	{MT9M114_COMMAND_REGISTER, MT9M114_COMMAND_REGISTER_OK,
	 MSM_CAMERA_I2C_SET_WORD_MASK, MSM_CAMERA_I2C_CMD_POLL},
};

static struct msm_camera_i2c_conf_array mt9m114_saturation_confs[][2] = {
	{{mt9m114_saturation[0],
	  ARRAY_SIZE(mt9m114_saturation[0]), 0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9m114_saturation[1],
	  ARRAY_SIZE(mt9m114_saturation[1]), 0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9m114_saturation[2],
	  ARRAY_SIZE(mt9m114_saturation[2]), 0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9m114_saturation[3],
	  ARRAY_SIZE(mt9m114_saturation[3]), 0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9m114_saturation[4],
	  ARRAY_SIZE(mt9m114_saturation[4]), 0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9m114_saturation[5],
	  ARRAY_SIZE(mt9m114_saturation[5]), 0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9m114_saturation[6],
	  ARRAY_SIZE(mt9m114_saturation[6]), 0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9m114_saturation[7],
	  ARRAY_SIZE(mt9m114_saturation[7]), 0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9m114_saturation[8],
	  ARRAY_SIZE(mt9m114_saturation[8]), 0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9m114_saturation[9],
	  ARRAY_SIZE(mt9m114_saturation[9]), 0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
	{{mt9m114_saturation[10],
	  ARRAY_SIZE(mt9m114_saturation[10]),
	  0, MSM_CAMERA_I2C_WORD_DATA},
	 {mt9m114_refresh,
	  ARRAY_SIZE(mt9m114_refresh), 0, MSM_CAMERA_I2C_WORD_DATA},},
};

static int mt9m114_saturation_enum_map[] = {
	MSM_V4L2_SATURATION_L0,
	MSM_V4L2_SATURATION_L1,
	MSM_V4L2_SATURATION_L2,
	MSM_V4L2_SATURATION_L3,
	MSM_V4L2_SATURATION_L4,
	MSM_V4L2_SATURATION_L5,
	MSM_V4L2_SATURATION_L6,
	MSM_V4L2_SATURATION_L7,
	MSM_V4L2_SATURATION_L8,
	MSM_V4L2_SATURATION_L9,
	MSM_V4L2_SATURATION_L10,
};

static struct msm_camera_i2c_enum_conf_array mt9m114_saturation_enum_confs = {
	.conf = &mt9m114_saturation_confs[0][0],
	.conf_enum = mt9m114_saturation_enum_map,
	.num_enum = ARRAY_SIZE(mt9m114_saturation_enum_map),
	.num_index = ARRAY_SIZE(mt9m114_saturation_confs),
	.num_conf = ARRAY_SIZE(mt9m114_saturation_confs[0]),
	.data_type = MSM_CAMERA_I2C_WORD_DATA,
};

struct msm_sensor_v4l2_ctrl_info_t mt9m114_v4l2_ctrl_info[] = {
	{
	 .ctrl_id = V4L2_CID_SATURATION,
	 .min = MSM_V4L2_SATURATION_L0,
	 .max = MSM_V4L2_SATURATION_L10,
	 .step = 1,
	 .enum_cfg_settings = &mt9m114_saturation_enum_confs,
	 .s_v4l2_ctrl = msm_sensor_s_ctrl_by_enum,
	 },
};

static struct msm_sensor_output_info_t mt9m114_dimensions[] = {
	{			//MSM_SENSOR_RES_FULL
	 .x_output = 0x500,	// 1280
	 .y_output = 0x3C0,	// 960
#if 0
	 .line_length_pclk = 0xA00,	//1280*2
	 .frame_length_lines = 0x3C0,	//960
	 .vt_pixel_clk = 96000000,
#else
//fps10to30
	 .line_length_pclk = 0x0636,
	 .frame_length_lines = 0x03EE,
	 .vt_pixel_clk = 48000000,
#endif
	 .op_pixel_clk = 128000000,
	 .binning_factor = 1,
	 },
};

#ifdef CONFIG_LGE_CAMERA
static struct msm_camera_csi_params mt9m114_csi_params = {
	.data_format = CSI_8BIT,
	.lane_cnt = 1,
	.lane_assign = 0xe4,
	.dpcm_scheme = 0,
	.settle_cnt = 0x14,
};

static struct msm_camera_csi_params *mt9m114_csi_params_array[] = {
	&mt9m114_csi_params,
};
#else
static struct msm_camera_csid_vc_cfg mt9m114_cid_cfg[] = {
	{0, CSI_YUV422_8, CSI_DECODE_8BIT},
	{1, CSI_EMBED_DATA, CSI_DECODE_8BIT},
};

static struct msm_camera_csi2_params mt9m114_csi_params = {
	.csid_params = {
			.lane_cnt = 1,
			.lut_params = {
				       .num_cid = 2,
				       .vc_cfg = mt9m114_cid_cfg,
				       },
			},
	.csiphy_params = {
			  .lane_cnt = 1,
			  .settle_cnt = 0x14,
			  },
};

static struct msm_camera_csi2_params *mt9m114_csi_params_array[] = {
	&mt9m114_csi_params,
	&mt9m114_csi_params,
};
#endif

static struct msm_sensor_output_reg_addr_t mt9m114_reg_addr = {
	.x_output = 0xC868,
	.y_output = 0xC86A,
	.line_length_pclk = 0xC814,
	.frame_length_lines = 0xC812,
};

static struct msm_sensor_id_info_t mt9m114_id_info = {
	.sensor_id_reg_addr = 0x0,
	.sensor_id = 0x2481,
};

static const struct i2c_device_id mt9m114_i2c_id[] = {
	{SENSOR_NAME, (kernel_ulong_t) & mt9m114_s_ctrl},
	{}
};

static struct i2c_driver mt9m114_i2c_driver = {
	.id_table = mt9m114_i2c_id,
	.probe = msm_sensor_i2c_probe,
	.driver = {
		   .name = SENSOR_NAME,
		   },
};

static struct msm_camera_i2c_client mt9m114_sensor_i2c_client = {
	.addr_type = MSM_CAMERA_I2C_WORD_ADDR,
};

//static const int brightness_table[] = {0x0020, 0x0024, 0x0028, 0x002E, 0x0032, 0x0035, 0x003E, 0x0048, 0x0050, 0x0054, 0x0058, 0x005C, 0x005F}; // 13 step
																						// value < 0x80 --> value should be increased , max value => 0x7F00
//                                                                                         
static const int brightness_table[] = { 0x0008, 0x0011, 0x001A, 0x0023, 0x002C, 0x0035, 0x003E, 0x0047, 0x0050, 0x0059, 0x0062, 0x006B, 0x0074 };	// 13 step

								// value > 0x80 --> value should be decreased, max value => 0x8100
static const int gamma_table_sub[] = { 0x00C4, 0x00C8, 0x00CC, 0x00D0, 0x00D4, 0x00D8, 0x00DC, 0x00DF, 0x00E3, 0x00E8, 0x00ED, 0x00F2, 0x00F7 };	// 13 step

								// 2.2 gamma                                                                                                                    // 0.5 gamma

static int mt9m114_set_effect(struct msm_sensor_ctrl_t *s_ctrl, int8_t effect)
{
	int rc = 0;

	if (prev_effect_mode == effect) {
		CDBG_VT("%s: skip this function, effect_mode -> %d\n", __func__,
			effect);
		return rc;
	}

	CDBG_VT("###  ; [CHECK]%s: effect -> %d\n", __func__, effect);

	switch (effect) {
	case CAMERA_EFFECT_OFF:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &effect_default_tbl_sub[0],
						  ARRAY_SIZE
						  (effect_default_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_EFFECT_MONO:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &effect_mono_tbl_sub[0],
						  ARRAY_SIZE
						  (effect_mono_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_EFFECT_NEGATIVE:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &effect_negative_tbl_sub[0],
						  ARRAY_SIZE
						  (effect_negative_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_EFFECT_SOLARIZE:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &effect_solarization_tbl_sub
						  [0],
						  ARRAY_SIZE
						  (effect_solarization_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_EFFECT_SEPIA:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &effect_sepia_tbl_sub[0],
						  ARRAY_SIZE
						  (effect_sepia_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_EFFECT_AQUA:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &effect_aqua_tbl_sub[0],
						  ARRAY_SIZE
						  (effect_aqua_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_EFFECT_POSTERIZE:	// effect off code
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &effect_default_tbl_sub[0],
						  ARRAY_SIZE
						  (effect_default_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	default:
		return -EINVAL;
	}

	rc = mt9m114_i2c_write_w_sensor(s_ctrl->sensor_i2c_client, 0x0080,
					0x8004);

	if (rc < 0)
		return rc;
	{
		unsigned short test_data = 0, i;
		for (i = 0; i < 50; i++) {	// max delay ==> 500 ms
			rc = mt9m114_i2c_read(s_ctrl->sensor_i2c_client, 0x0080,
					      &test_data,
					      MSM_CAMERA_I2C_WORD_ADDR);
			if (rc < 0)
				return rc;

			if ((test_data & 0x0004) == 0)
				break;
			else
				mdelay(1);	// 10 > 1

			CDBG_VT
			    ("### %s :  Refresh Polling set, 0x0080 Reg : 0x%x\n",
			     __func__, test_data);
		}
	}
	prev_effect_mode = effect;
	return rc;
}

static int mt9m114_set_wb(struct msm_sensor_ctrl_t *s_ctrl, uint8_t mode)
{
	int32_t rc = 0;

	if (prev_balance_mode == mode) {
		CDBG_VT("###  [CHECK]%s: skip this function, wb_mode -> %d\n",
			__func__, mode);
		return rc;
	}
	CDBG_VT("###  [CHECK]%s: mode -> %d\n", __func__, mode);

	switch (mode) {
	case CAMERA_WB_AUTO:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &wb_default_tbl_sub[0],
						  ARRAY_SIZE
						  (wb_default_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_WB_DAYLIGHT:	// sunny
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &wb_sunny_tbl_sub[0],
						  ARRAY_SIZE(wb_sunny_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_WB_CLOUDY_DAYLIGHT:	// cloudy
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &wb_cloudy_tbl_sub[0],
						  ARRAY_SIZE(wb_cloudy_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_WB_FLUORESCENT:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &wb_fluorescent_tbl_sub[0],
						  ARRAY_SIZE
						  (wb_fluorescent_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_WB_INCANDESCENT:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &wb_incandescent_tbl_sub[0],
						  ARRAY_SIZE
						  (wb_incandescent_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	default:
		return -EINVAL;
	}

	prev_balance_mode = mode;
	return rc;
}

static int mt9m114_set_iso(struct msm_sensor_ctrl_t *s_ctrl, uint8_t mode)
{
	int32_t rc = 0;

	if (prev_iso_mode == mode) {
		CDBG_VT("###  [CHECK]%s: skip this function, iso_mode -> %d\n",
			__func__, mode);
		return rc;
	}
	CDBG_VT("###  [CHECK]%s: mode -> %d\n", __func__, mode);

	switch (mode) {
	case CAMERA_ISO_AUTO:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &iso_default_tbl_sub[0],
						  ARRAY_SIZE
						  (iso_default_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_ISO_100:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &iso_100_tbl_sub[0],
						  ARRAY_SIZE(iso_100_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_ISO_200:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &iso_200_tbl_sub[0],
						  ARRAY_SIZE(iso_200_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_ISO_400:
		rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
						  &iso_400_tbl_sub[0],
						  ARRAY_SIZE(iso_400_tbl_sub),
						  MSM_CAMERA_I2C_WORD_DATA);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	default:
		return 0;	//return -EINVAL;
	}

	rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
					  &change_config_tbl_sub[0],
					  ARRAY_SIZE(change_config_tbl_sub),
					  MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) {
		CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__, __LINE__, rc);
		return rc;
	}

	prev_iso_mode = mode;
	return rc;
}

static int mt9m114_set_antibanding(struct msm_sensor_ctrl_t *s_ctrl,
				   uint16_t mode)
{
	int32_t rc = 0;

	if (prev_antibanding_mode == mode) {
		CDBG_VT
		    ("###  [CHECK]%s: skip this function, antibanding_mode -> %d\n",
		     __func__, mode);
		return rc;
	}
	CDBG_VT("###  [CHECK]%s: mode -> %d\n", __func__, mode);

	prev_antibanding_mode = mode;

	switch (mode) {
	case CAMERA_ANTIBANDING_60HZ:
		rc = mt9m114_i2c_write_w_sensor(s_ctrl->sensor_i2c_client,
						0x098E, 0xCC03);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		rc = mt9m114_i2c_write_w_sensor(s_ctrl->sensor_i2c_client,
						0x0990, 0x0200);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	case CAMERA_ANTIBANDING_50HZ:
		rc = mt9m114_i2c_write_w_sensor(s_ctrl->sensor_i2c_client,
						0x098E, 0xCC03);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		rc = mt9m114_i2c_write_w_sensor(s_ctrl->sensor_i2c_client,
						0x0990, 0x0100);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
		break;
	default:
		return 0;	// return -EINVAL;
	}
	return rc;
}

static int32_t mt9m114_set_brightness(struct msm_sensor_ctrl_t *s_ctrl,
				      int mode)
{
	int32_t rc = 0;

	if (prev_brightness_mode == mode) {
		CDBG_VT
		    ("###  [CHECK]%s: skip this function, brightness_mode -> %d\n",
		     __func__, mode);
		return rc;
	}
	mode = mode % 13;
	CDBG_VT("###  [CHECK]%s: mode -> %d\n", __func__, mode);

	if (mode < 0 || mode > 12) {
		CDBG("###[ERROR]%s: Invalid Mode value\n", __func__);
		return -EINVAL;
	}
	// LOGICAL_ADDRESS_ACCESS [CAM_LL_GAMMA]
	rc = mt9m114_i2c_write_w_sensor(s_ctrl->sensor_i2c_client, 0x098E,
					0x4940);
	if (rc < 0) {
		CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__, __LINE__, rc);
		return rc;
	}
	// CAM_LL_GAMMA
	rc = mt9m114_i2c_write_w_sensor(s_ctrl->sensor_i2c_client, 0xC940,
					gamma_table_sub[mode]);
	if (rc < 0) {
		CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__, __LINE__, rc);
		return rc;
	}
	// UVC_BRIGHTNESS_CONTROL
	rc = mt9m114_i2c_write_w_sensor(s_ctrl->sensor_i2c_client, 0xCC0A,
					brightness_table[mode]);
	if (rc < 0) {
		CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__, __LINE__, rc);
		return rc;
	}

	prev_brightness_mode = mode;

	return rc;
}

//                                                              
static int32_t mt9m114_set_Fps(struct msm_sensor_ctrl_t *s_ctrl, int mode)
{
	int32_t rc = 0;

	CDBG_VT("mt9m114_set_Fps mode = %d \n ", mode);

	if (prev_fps_mode == mode) {
		CDBG_VT
		    ("###  [CHECK]%s: skip this function, prev_fps_mode -> %d\n",
		     __func__, mode);
		return rc;
	}

	switch (mode) {
	case SENSOR_FIXED_FPS_15:	//CAMERA_FPS_15:
		// 15 fps fixed
		{
			struct msm_camera_i2c_reg_conf fps15_tbl_sub[] = {
				{0x098E, 0x4812},
				{0xC810, 0x05B3},
				{0xC812, 0x07E0},
				{0xC814, 0x0636},
				{0xC88C, 0x1E02},
				{0xC88E, 0x1E02},
			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &fps15_tbl_sub[0],
							  ARRAY_SIZE
							  (fps15_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	case SENSOR_FIXED_FPS_30:	//CAMERA_FPS_30:      // sunny
		// 30 fps fixed
		{
			struct msm_camera_i2c_reg_conf fps30_tbl_sub[] = {
				{0x098E, 0x4812},
				{0xC810, 0x05B3},
				{0xC812, 0x03EE},
				{0xC814, 0x0636},
				{0xC88C, 0x1E02},
				{0xC88E, 0x1E02},

			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &fps30_tbl_sub[0],
							  ARRAY_SIZE
							  (fps30_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	case SENSOR_AUTO_FPS_1030:	//CAMERA_FPS_10to30:
		// 10 fps ~ 30 fps variable
		{
			struct msm_camera_i2c_reg_conf fps10to30_tbl_sub[] = {
				{0x098E, 0x4812},
				{0xC810, 0x05B3},
				{0xC812, 0x03EE},
				{0xC814, 0x0636},
				{0xC88C, 0x1E02},
				{0xC88E, 0x0A00},

			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &fps10to30_tbl_sub[0],
							  ARRAY_SIZE
							  (fps10to30_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	case SENSOR_AUTO_FPS_0730:	//CAMERA_FPS_7to30:  // cloudy
		// 7.5 fps ~ 30 fps variable
		{
			struct msm_camera_i2c_reg_conf fps7dot5to30_tbl_sub[] = {
				{0x098E, 0x4812},
				{0xC810, 0x05B3},
				{0xC812, 0x03EE},
				{0xC814, 0x0636},
				{0xC88C, 0x1E02},
				{0xC88E, 0x0780},

			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &fps7dot5to30_tbl_sub
							  [0],
							  ARRAY_SIZE
							  (fps7dot5to30_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

		//                                                  
	case SENSOR_FIXED_FPS_10:
		// 10fps fixed
		{
			struct msm_camera_i2c_reg_conf fps10_tbl_sub[] = {
				{0x098E, 0x4812},
				{0xC810, 0x05BD},
				{0xC812, 0x0BB8},
				{0xC814, 0x0640},
				{0xC88C, 0x0A00},
				{0xC88E, 0x0A00},

			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &fps10_tbl_sub[0],
							  ARRAY_SIZE
							  (fps10_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	case SENSOR_FIXED_FPS_08:
		// 8fps fixed
		{
			struct msm_camera_i2c_reg_conf fps8_tbl_sub[] = {
				{0x098E, 0x4812},
				{0xC810, 0x05BD},
				{0xC812, 0x0EA6},
				{0xC814, 0x0640},
				{0xC88C, 0x0800},
				{0xC88E, 0x0800},

			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &fps8_tbl_sub[0],
							  ARRAY_SIZE
							  (fps8_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	case SENSOR_FIXED_FPS_07:
		// 7fps fixed
		{
			struct msm_camera_i2c_reg_conf fps7_tbl_sub[] = {
				{0x098E, 0x4812},
				{0xC810, 0x15EC},
				{0xC812, 0x04AA},
				{0xC814, 0x166F},
				{0xC88C, 0x0700},
				{0xC88E, 0x0700},

			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &fps7_tbl_sub[0],
							  ARRAY_SIZE
							  (fps7_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;
		//                                                 

	default:
		CDBG_VT("mt9m114_set_Fps wrong value : %d \n ", mode);
		rc = 0;
		return rc;
	}

	rc = msm_camera_i2c_write_tbl_114(s_ctrl->sensor_i2c_client,
					  &change_config_tbl_sub[0],
					  ARRAY_SIZE(change_config_tbl_sub),
					  MSM_CAMERA_I2C_WORD_DATA);
	if (rc < 0) {
		CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__, __LINE__, rc);
		return rc;
	}

	prev_fps_mode = mode;
	CDBG_VT("mt9m114_set_Fps Change Frame rate \n ");

	if (prev_brightness_mode != 6)	// Set again when mode is not center
	{
		CDBG_VT("### Set again when mode is not center, value is %d\n ",
			prev_brightness_mode);
		rc = mt9m114_i2c_write_w_sensor(s_ctrl->sensor_i2c_client,
						0x337E,
						brightness_table
						[prev_brightness_mode]);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}

		rc = mt9m114_i2c_write_w_sensor(s_ctrl->sensor_i2c_client,
						0xC940,
						gamma_table_sub
						[prev_brightness_mode]);
		if (rc < 0) {
			CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
			     __LINE__, rc);
			return rc;
		}
	}

	return rc;
}

//                                                              

static int mt9m114_set_AE_metering(struct msm_sensor_ctrl_t *s_ctrl, int mode)
{
	int32_t rc = 0;

	if (prev_ae_metering_mode == mode) {
		CDBG_VT
		    ("###  [CHECK]%s: skip this function, prev_ae_metering_mode -> %d\n",
		     __func__, mode);
		return rc;
	}

	CDBG_VT("###  [CHECK]%s: mode -> %d\n", __func__, mode);

	switch (mode) {
	case AE_METERING_DEFAULT:	// AE DEFAULT metering:
		//[AE DEFAULT metering]
		{
			struct msm_camera_i2c_reg_conf ae_default_tbl_sub[] = {
				{0x098E, 0xA407},
				{0x0990, 0x1900},
				{0x098E, 0xA408},
				{0x0990, 0x1900},
				{0x098E, 0xA409},
				{0x0990, 0x1900},
				{0x098E, 0xA40A},
				{0x0990, 0x1900},
				{0x098E, 0xA40B},
				{0x0990, 0x1900},
				{0x098E, 0xA40C},
				{0x0990, 0x1900},
				{0x098E, 0xA40D},
				{0x0990, 0x4B00},
				{0x098E, 0xA40E},
				{0x0990, 0x4B00},
				{0x098E, 0xA40F},
				{0x0990, 0x4B00},
				{0x098E, 0xA410},
				{0x0990, 0x1900},
				{0x098E, 0xA411},
				{0x0990, 0x1900},
				{0x098E, 0xA412},
				{0x0990, 0x4B00},
				{0x098E, 0xA413},
				{0x0990, 0x6400},
				{0x098E, 0xA414},
				{0x0990, 0x4B00},
				{0x098E, 0xA415},
				{0x0990, 0x1900},
				{0x098E, 0xA416},
				{0x0990, 0x1900},
				{0x098E, 0xA417},
				{0x0990, 0x4B00},
				{0x098E, 0xA418},
				{0x0990, 0x4B00},
				{0x098E, 0xA419},
				{0x0990, 0x4B00},
				{0x098E, 0xA41A},
				{0x0990, 0x1900},
				{0x098E, 0xA41B},
				{0x0990, 0x1900},
				{0x098E, 0xA41C},
				{0x0990, 0x1900},
				{0x098E, 0xA41D},
				{0x0990, 0x1900},
				{0x098E, 0xA41E},
				{0x0990, 0x1900},
				{0x098E, 0xA41F},
				{0x0990, 0x1900},
			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &ae_default_tbl_sub
							  [0],
							  ARRAY_SIZE
							  (ae_default_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	case AE_METERING_CENTER:	//AE CENTER metering:
		//[AE CENTER metering]
		{
			struct msm_camera_i2c_reg_conf ae_center_tbl_sub[] = {
				{0x098E, 0xA407},
				{0x0990, 0x0000},
				{0x098E, 0xA408},
				{0x0990, 0x0000},
				{0x098E, 0xA409},
				{0x0990, 0x0000},
				{0x098E, 0xA40A},
				{0x0990, 0x0000},
				{0x098E, 0xA40B},
				{0x0990, 0x0000},
				{0x098E, 0xA40C},
				{0x0990, 0x0000},
				{0x098E, 0xA40D},
				{0x0990, 0x0000},
				{0x098E, 0xA40E},
				{0x0990, 0x0000},
				{0x098E, 0xA40F},
				{0x0990, 0x0000},
				{0x098E, 0xA410},
				{0x0990, 0x0000},
				{0x098E, 0xA411},
				{0x0990, 0x0000},
				{0x098E, 0xA412},
				{0x0990, 0x0000},
				{0x098E, 0xA413},
				{0x0990, 0x6400},
				{0x098E, 0xA414},
				{0x0990, 0x0000},
				{0x098E, 0xA415},
				{0x0990, 0x0000},
				{0x098E, 0xA416},
				{0x0990, 0x0000},
				{0x098E, 0xA417},
				{0x0990, 0x0000},
				{0x098E, 0xA418},
				{0x0990, 0x0000},
				{0x098E, 0xA419},
				{0x0990, 0x0000},
				{0x098E, 0xA41A},
				{0x0990, 0x0000},
				{0x098E, 0xA41B},
				{0x0990, 0x0000},
				{0x098E, 0xA41C},
				{0x0990, 0x0000},
				{0x098E, 0xA41D},
				{0x0990, 0x0000},
				{0x098E, 0xA41E},
				{0x0990, 0x0000},
				{0x098E, 0xA41F},
				{0x0990, 0x0000},
			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &ae_center_tbl_sub[0],
							  ARRAY_SIZE
							  (ae_center_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	default:
		return -EINVAL;
	}

	prev_ae_metering_mode = mode;

	return rc;
}

//                                                                              
/*                                                                           
                                       
  
                                                                                               
                                                                            */
static int32_t mt9m114_set_awb_lock(struct msm_sensor_ctrl_t *s_ctrl, int mode)
{
	int rc = 0;

	CDBG_VT("[CAMERA/VT] %s: mode=%d\n", __func__, mode);

	if (prev_awb_mode == mode) {
		CDBG_VT
		    ("[CAMERA/VT] %s: skip this function, prev_awb_mode -> %d\n",
		     __func__, mode);
		return rc;
	}
	switch (mode) {
	case SENSOR_VT_AWB_UNLOCK:	// default
		// [AWB ON]
		{
			struct msm_camera_i2c_reg_conf awb_on_tbl_sub[] = {
				{0x098E, 0x2C04},	// LOGICAL_ADDRESS_ACCESS [AWB_ALGO]
				{0x0990, 0x0288},	// AWB_ALGO
				{0x098E, 0xC909},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_AWBMODE]
				{0x0990, 0x0300},	// CAM_AWB_AWBMODE
			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &awb_on_tbl_sub[0],
							  ARRAY_SIZE
							  (awb_on_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	case SENSOR_VT_AWB_LOCK:
		// [AWB OFF]
		{
			struct msm_camera_i2c_reg_conf awb_off_tbl_sub[] = {
				{0x098E, 0x2C04},	// LOGICAL_ADDRESS_ACCESS [AWB_ALGO]
				{0xAC04, 0x0000},	// AWB_ALGO
				{0x098E, 0xC909},	// LOGICAL_ADDRESS_ACCESS [CAM_AWB_AWBMODE]
				{0x0990, 0x0000},	// CAM_AWB_AWBMODE
			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &awb_off_tbl_sub[0],
							  ARRAY_SIZE
							  (awb_off_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	default:
		printk(KERN_ERR "[CAMERA/VT] %s wrong value : %d \n ", __func__,
		       mode);
		rc = -EIO;
		return rc;
	}

	prev_awb_mode = mode;
	CDBG_VT("[CAMERA/VT] %s Change AWB Lock mode\n ", __func__);

	return rc;
}

/*                                                                           
                                       
  
                                                                                               
                                                                            */
static int32_t mt9m114_set_aec_lock(struct msm_sensor_ctrl_t *s_ctrl, int mode)
{
	int rc = 0;

	CDBG_VT("[CAMERA/VT] %s: mode=%d\n", __func__, mode);

	if (prev_aec_mode == mode) {
		CDBG_VT
		    ("[CAMERA/VT] %s: skip this function, prev_aec_mode -> %d\n",
		     __func__, mode);
		return rc;
	}
	switch (mode) {
	case SENSOR_VT_AEC_UNLOCK:	// default
		// [AE ON]
		{
			struct msm_camera_i2c_reg_conf ae_on_tbl_sub[] = {
				{0x098E, 0x2804},	//  LOGICAL_ADDRESS_ACCESS [AE_TRACK_ALGO]
				{0xA804, 0x00FF},	// AE_TRACK_ALGO
			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &ae_on_tbl_sub[0],
							  ARRAY_SIZE
							  (ae_on_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	case SENSOR_VT_AEC_LOCK:
		// [AE OFF]
		{
			struct msm_camera_i2c_reg_conf ae_off_tbl_sub[] = {
				{0x098E, 0x2804},	//  LOGICAL_ADDRESS_ACCESS [AE_TRACK_ALGO]
				{0x0990, 0x00FE},	// AE_TRACK_ALGO
			};
			rc = msm_camera_i2c_write_tbl_114(s_ctrl->
							  sensor_i2c_client,
							  &ae_off_tbl_sub[0],
							  ARRAY_SIZE
							  (ae_off_tbl_sub),
							  MSM_CAMERA_I2C_WORD_DATA);

			if (rc < 0) {
				CDBG("###[ERROR]%s:%d failed rc=%d\n", __func__,
				     __LINE__, rc);
				return rc;
			}
		}
		break;

	default:
		printk(KERN_ERR "[CAMERA/VT] %s wrong value : %d \n ", __func__,
		       mode);
		rc = -EIO;
		return rc;
	}

	prev_aec_mode = mode;
	CDBG_VT("[CAMERA/VT] %s Change AEC Lock mode\n ", __func__);

	return rc;
}

//                                                                              

static int32_t mt9m114_config(struct msm_sensor_ctrl_t *s_ctrl,
			      void __user * argp)
{
	struct sensor_cfg_data cdata;
	long rc = 0;
	if (copy_from_user(&cdata,
			   (void *)argp, sizeof(struct sensor_cfg_data)))
		return -EFAULT;
	mutex_lock(s_ctrl->msm_sensor_mutex);
	CDBG("%s: cfgtype = %d\n", __func__, cdata.cfgtype);
	switch (cdata.cfgtype) {
	case CFG_SET_EFFECT:
		rc = mt9m114_set_effect(s_ctrl, cdata.cfg.effect);
		break;

	case CFG_SET_WB:
		rc = mt9m114_set_wb(s_ctrl, cdata.cfg.wb_val);
		break;

	case CFG_SET_ISO:
		rc = mt9m114_set_iso(s_ctrl, cdata.cfg.iso_type);
		break;

	case CFG_SET_ANTIBANDING:
		rc = mt9m114_set_antibanding(s_ctrl, cdata.cfg.antibanding);
		break;

	case CFG_SET_BRIGHTNESS:
		rc = mt9m114_set_brightness(s_ctrl, cdata.cfg.brightness);
		break;

	case CFG_FIXED_FPS:	//                                                            
		rc = mt9m114_set_Fps(s_ctrl, cdata.mode);
		break;

	case CFG_VT_AWB_LOCK:	//                                                                              
		rc = mt9m114_set_awb_lock(s_ctrl, cdata.mode);
		break;

	case CFG_VT_AEC_LOCK:	//                                                                              
		rc = mt9m114_set_aec_lock(s_ctrl, cdata.mode);
		break;

	case SENSOR_AE_METERING:	//                                             
		rc = mt9m114_set_AE_metering(s_ctrl, cdata.mode);
		break;

	default:
		rc = -EINVAL;
		break;
	}

	mutex_unlock(s_ctrl->msm_sensor_mutex);

	if (rc == -EINVAL) {
		rc = msm_sensor_config(s_ctrl, argp);
	}

	return rc;
}

static int __init msm_sensor_init_module(void)
{
	prev_effect_mode = -1;
	prev_balance_mode = -1;
	prev_iso_mode = -1;
	prev_antibanding_mode = -1;
	prev_brightness_mode = -1;
	prev_fps_mode = -1;
	prev_awb_mode = -1;	//                                                                            
	prev_aec_mode = -1;	//                                                                            
	return i2c_add_driver(&mt9m114_i2c_driver);
}

static struct v4l2_subdev_core_ops mt9m114_subdev_core_ops = {
	.s_ctrl = msm_sensor_v4l2_s_ctrl,
	.queryctrl = msm_sensor_v4l2_query_ctrl,
	.ioctl = msm_sensor_subdev_ioctl,
	.s_power = msm_sensor_power,
};

static struct v4l2_subdev_video_ops mt9m114_subdev_video_ops = {
	.enum_mbus_fmt = msm_sensor_v4l2_enum_fmt,
};

static struct v4l2_subdev_ops mt9m114_subdev_ops = {
	.core = &mt9m114_subdev_core_ops,
	.video = &mt9m114_subdev_video_ops,
};

static struct msm_sensor_fn_t mt9m114_func_tbl = {
	.sensor_start_stream = mt9m114_start_stream,	//msm_sensor_start_stream_114,//msm_sensor_start_stream,
	.sensor_stop_stream = mt9m114_stop_stream,
#ifdef CONFIG_LGE_CAMERA
	.sensor_csi_setting = msm_sensor_setting_114,	//msm_sensor_setting_114,
#else
	.sensor_setting = msm_sensor_setting,
#endif
	.sensor_set_sensor_mode = msm_sensor_set_sensor_mode,
	.sensor_mode_init = msm_sensor_mode_init,
	.sensor_get_output_info = msm_sensor_get_output_info,
	.sensor_config = mt9m114_config,
	.sensor_power_up = msm_sensor_power_up,
	.sensor_power_down = msm_sensor_power_down,
	.sensor_get_csi_params = msm_sensor_get_csi_params,
};

static struct msm_sensor_reg_t mt9m114_regs = {
	.default_data_type = MSM_CAMERA_I2C_WORD_DATA,
	.start_stream_conf = change_config_tbl_sub,
	.start_stream_conf_size = ARRAY_SIZE(change_config_tbl_sub),
	.init_settings = &mt9m114_init_conf[0],
	.init_size = ARRAY_SIZE(mt9m114_init_conf),
	.mode_settings = &mt9m114_confs[0],
	.output_settings = &mt9m114_dimensions[0],
	.num_conf = ARRAY_SIZE(mt9m114_confs),
};

static struct msm_sensor_ctrl_t mt9m114_s_ctrl = {
	.msm_sensor_reg = &mt9m114_regs,
	//.msm_sensor_v4l2_ctrl_info = mt9m114_v4l2_ctrl_info,
	//.num_v4l2_ctrl = ARRAY_SIZE(mt9m114_v4l2_ctrl_info),
	.sensor_i2c_client = &mt9m114_sensor_i2c_client,
	.sensor_i2c_addr = 0x90,	// real addr : 0x90 >> 1( 0x48 임.)
	.sensor_output_reg_addr = &mt9m114_reg_addr,
	.sensor_id_info = &mt9m114_id_info,
	.cam_mode = MSM_SENSOR_MODE_INVALID,
#ifdef CONFIG_LGE_CAMERA
	.csic_params = &mt9m114_csi_params_array[0],
#else
	.csi_params = &mt9m114_csi_params_array[0],
#endif
	.msm_sensor_mutex = &mt9m114_mut,
	.sensor_i2c_driver = &mt9m114_i2c_driver,
	.sensor_v4l2_subdev_info = mt9m114_subdev_info,
	.sensor_v4l2_subdev_info_size = ARRAY_SIZE(mt9m114_subdev_info),
	.sensor_v4l2_subdev_ops = &mt9m114_subdev_ops,
	.func_tbl = &mt9m114_func_tbl,
};

//module_init(msm_sensor_init_module);
late_initcall(msm_sensor_init_module);
MODULE_DESCRIPTION("Aptina 1.26MP YUV sensor driver");
MODULE_LICENSE("GPL v2");
