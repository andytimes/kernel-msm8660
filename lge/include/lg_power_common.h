/* 
                       
  
                                                           
*/

#ifdef CONFIG_LGE_PM_CURRENT_CABLE_TYPE
typedef enum {
	NO_INIT_CABLE,
	MHL_CABLE_500MA,
	TA_CABLE_600MA,
	TA_CABLE_800MA,
	TA_CABLE_DTC_800MA,	/* desk-top cradle */
	TA_CABLE_FORGED_500MA,
	LT_CABLE_56K,
	LT_CABLE_130K,
	USB_CABLE_400MA,	
	USB_CABLE_DTC_500MA,/* desk-top cradle */
	ABNORMAL_USB_CABLE_400MA,	
//                                                                
	LT_CABLE_910K,
//                                                 
#ifdef CONFIG_MACH_LGE_I_BOARD_SKT
	TA_CABLE_NOT_AUTH_700MA,
#endif
    MAX_CABLE,
}acc_cable_type;
#endif

#ifdef CONFIG_LGE_CHARGER_TEMP_SCENARIO
enum{
	THERM_M10,
	THERM_M5,
	THERM_42,
	THERM_45,
	THERM_55,
	THERM_57,
	THERM_60,	
	THERM_65,
	THERM_LAST,
};

enum{
	DISCHG_BATT_TEMP_OVER_60,
	DISCHG_BATT_TEMP_57_60,
	DISCHG_BATT_TEMP_UNDER_57,
	CHG_BATT_TEMP_OVER_55,
	CHG_BATT_TEMP_46_55,
	CHG_BATT_TEMP_42_45,
	CHG_BATT_TEMP_M4_41,
	CHG_BATT_TEMP_M10_M5,
	CHG_BATT_TEMP_UNDER_M10,
};

enum{
	DISCHG_BATT_NORMAL_STATE,
	DISCHG_BATT_WARNING_STATE,
	DISCHG_BATT_POWEROFF_STATE,
	CHG_BATT_NORMAL_STATE,
	CHG_BATT_DC_CURRENT_STATE,
	CHG_BATT_WARNING_STATE,
	CHG_BATT_STOP_CHARGING_STATE,
};
#endif

#ifdef CONFIG_LGE_PM
/*Classified the ADC value for cable detection */
typedef enum {
	NO_INIT_CABLE = 0,
	CABLE_MHL_1K,
	CABLE_U_28P7K,
	CABLE_28P7K,
	CABLE_56K,
	CABLE_100K,
	CABLE_130K,
	CABLE_180K,
	CABLE_200K,
	CABLE_220K,
	CABLE_270K,
	CABLE_330K,
	CABLE_620K,
	CABLE_910K,
	CABLE_NONE
} acc_cable_type;

struct chg_cable_info {
	acc_cable_type cable_type;
	unsigned ta_ma;
	unsigned usb_ma;
};

int lge_pm_get_cable_info(struct chg_cable_info *);
void lge_pm_read_cable_info(void);
acc_cable_type lge_pm_get_cable_type(void);
unsigned lge_pm_get_ta_current(void);
unsigned lge_pm_get_usb_current(void);
#endif

#ifdef CONFIG_LGE_PM_BATTERY_ID_CHECKER
bool is_lge_battery(void);
enum {
	BATT_ID_UNKNOWN,
	BATT_ID_DS2704_N = 17,
	BATT_ID_DS2704_L = 32,
	BATT_ID_ISL6296_N = 73,
	BATT_ID_ISL6296_L = 94,
	BATT_ID_DS2704_C = 48,
	BATT_ID_ISL6296_C =105,
};
#else
static inline bool is_lge_battery(void)
{
	return true;
}
#endif

struct kcal_platform_data {
	int (*set_values) (int r, int g, int b);
	int (*get_values) (int *r, int *g, int *b);
	int (*refresh_display) (void);
};

#ifdef CONFIG_LGE_PM
struct pseudo_batt_info_type {
	int mode;
	int id;
	int therm;
	int temp;
	int volt;
	int capacity;
	int charging;
};
#endif



