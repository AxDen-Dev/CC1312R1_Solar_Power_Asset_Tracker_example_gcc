################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
SYSCFG_SRCS += \
../rfWsnNode.syscfg 

LDS_SRCS += \
../cc13x2_cc26x2_tirtos.lds 

C_SRCS += \
../RadioTask.c \
../SensorTask.c \
../bg96.c \
../main.c \
../quectel_gps.c \
./syscfg/ti_easylink_config.c \
./syscfg/ti_devices_config.c \
./syscfg/ti_radio_config.c \
./syscfg/ti_drivers_config.c \
../sara_u2.c \
../si7051.c \
../ublox_gps.c 

GEN_FILES += \
./syscfg/ti_easylink_config.c \
./syscfg/device_config.opt \
./syscfg/ti_devices_config.c \
./syscfg/ti_radio_config.c \
./syscfg/ti_drivers_config.c 

GEN_MISC_DIRS += \
./syscfg/ 

C_DEPS += \
./RadioTask.d \
./SensorTask.d \
./bg96.d \
./main.d \
./quectel_gps.d \
./syscfg/ti_easylink_config.d \
./syscfg/ti_devices_config.d \
./syscfg/ti_radio_config.d \
./syscfg/ti_drivers_config.d \
./sara_u2.d \
./si7051.d \
./ublox_gps.d 

GEN_OPTS += \
./syscfg/device_config.opt 

OBJS += \
./RadioTask.o \
./SensorTask.o \
./bg96.o \
./main.o \
./quectel_gps.o \
./syscfg/ti_easylink_config.o \
./syscfg/ti_devices_config.o \
./syscfg/ti_radio_config.o \
./syscfg/ti_drivers_config.o \
./sara_u2.o \
./si7051.o \
./ublox_gps.o 

GEN_MISC_FILES += \
./syscfg/ti_easylink_config.h \
./syscfg/ti_radio_config.h \
./syscfg/ti_drivers_config.h \
./syscfg/ti_utils_build_linker.cmd.genlibs \
./syscfg/syscfg_c.rov.xs \
./syscfg/ti_utils_runtime_model.gv \
./syscfg/ti_utils_runtime_Makefile 

GEN_MISC_DIRS__QUOTED += \
"syscfg/" 

OBJS__QUOTED += \
"RadioTask.o" \
"SensorTask.o" \
"bg96.o" \
"main.o" \
"quectel_gps.o" \
"syscfg/ti_easylink_config.o" \
"syscfg/ti_devices_config.o" \
"syscfg/ti_radio_config.o" \
"syscfg/ti_drivers_config.o" \
"sara_u2.o" \
"si7051.o" \
"ublox_gps.o" 

GEN_MISC_FILES__QUOTED += \
"syscfg/ti_easylink_config.h" \
"syscfg/ti_radio_config.h" \
"syscfg/ti_drivers_config.h" \
"syscfg/ti_utils_build_linker.cmd.genlibs" \
"syscfg/syscfg_c.rov.xs" \
"syscfg/ti_utils_runtime_model.gv" \
"syscfg/ti_utils_runtime_Makefile" 

C_DEPS__QUOTED += \
"RadioTask.d" \
"SensorTask.d" \
"bg96.d" \
"main.d" \
"quectel_gps.d" \
"syscfg/ti_easylink_config.d" \
"syscfg/ti_devices_config.d" \
"syscfg/ti_radio_config.d" \
"syscfg/ti_drivers_config.d" \
"sara_u2.d" \
"si7051.d" \
"ublox_gps.d" 

GEN_FILES__QUOTED += \
"syscfg/ti_easylink_config.c" \
"syscfg/device_config.opt" \
"syscfg/ti_devices_config.c" \
"syscfg/ti_radio_config.c" \
"syscfg/ti_drivers_config.c" 

C_SRCS__QUOTED += \
"../RadioTask.c" \
"../SensorTask.c" \
"../bg96.c" \
"../main.c" \
"../quectel_gps.c" \
"./syscfg/ti_easylink_config.c" \
"./syscfg/ti_devices_config.c" \
"./syscfg/ti_radio_config.c" \
"./syscfg/ti_drivers_config.c" \
"../sara_u2.c" \
"../si7051.c" \
"../ublox_gps.c" 

SYSCFG_SRCS__QUOTED += \
"../rfWsnNode.syscfg" 


