#platform.team@lge.com	2011.01
#
# Makefile for the sensor drivers.
#

# Each configuration option enables a list of files.

EXTRA_CFLAGS += -Ilge/include

obj-$(CONFIG_LGE_SENSOR_ACCELEROMETER)	+= k3dh.o
obj-$(CONFIG_LGE_SENSOR_GYROSCOPE)	+= k3g.o
obj-$(CONFIG_LGE_SENSOR_DCOMPASS)	+= ami306.o
obj-$(CONFIG_LGE_SENSOR_PROXIMITY)	+= apds9900.o
#blocked by mo2chunghan.lee@lge.com, 11/13/2012 --------------------------------
#obj-$(CONFIG_LGE_TOUCHSCREEN_SYNAPTICS_RMI4_I2C) += touch_synaptics_rmi4_i2c.o
#obj-$(CONFIG_LGE_TOUCH_SYNAPTICS_FW_UPGRADE) += touch_synaptics_fw_upgrade.o
#-------------------------------------------------------------------------------
obj-$(CONFIG_LGE_HEADSET_DETECTION_FSA8008) += fsa8008.o

#obj-$(CONFIG_LGE_TOUCHSCREEN_CORE) += lge_touch_core.o


#IJB board ======================================================================
obj-$(CONFIG_LGE_TOUCHSCREEN_CORE_IJB) += lge_touch_core_ijb.o	#<- lge_touch_core.c
obj-$(CONFIG_LGE_TOUCH_SYNAPTICS_IJB) += touch_synaptics_ijb.o	#<- touch_synaptics.c
obj-$(CONFIG_LGE_TOUCH_SYNAPTICS_IJB_FW_UPGRADE) += touch_synaptics_ijb_fw_upgrade.o	#<- touch_synaptics_fw_upgrade.c
