################################################################################
# Automatically-generated file. Do not edit!
################################################################################

CG_TOOL_ROOT := /Applications/ti/ccs1040/ccs/tools/compiler/gcc-arm-none-eabi-9-2019-q4-major

GEN_OPTS__FLAG := @"syscfg/device_config.opt" @"/Users/jangsangjin/workspace_v10/tirtos_builds_cc13x2_cc26x2_release_gcc_5_20_00_52/Debug/configPkg/compiler.opt" 
GEN_CMDS__FLAG := -Wl,-T"/Users/jangsangjin/workspace_v10/tirtos_builds_cc13x2_cc26x2_release_gcc_5_20_00_52/Debug/configPkg/linker.cmd" 

ORDERED_OBJS += \
"./RadioTask.o" \
"./SensorTask.o" \
"./bg96.o" \
"./main.o" \
"./quectel_gps.o" \
"./syscfg/ti_easylink_config.o" \
"./syscfg/ti_devices_config.o" \
"./syscfg/ti_radio_config.o" \
"./syscfg/ti_drivers_config.o" \
"./sara_u2.o" \
"./si7051.o" \
"./ublox_gps.o" \
"./easylink/EasyLink.o" \
-Wl,-T"../cc13x2_cc26x2_tirtos.lds" \
-l:ti_utils_build_linker.cmd.genlibs \
$(GEN_CMDS__FLAG) \
-l:"ti/devices/cc13x2_cc26x2/driverlib/bin/gcc/driverlib.lib" \
-lgcc \
-lc \
-lm \
-lnosys \

-include ../makefile.init

RM := rm -rf
RMDIR := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include easylink/subdir_vars.mk
-include subdir_rules.mk
-include easylink/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(INO_DEPS)),)
-include $(INO_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(SV7A_DEPS)),)
-include $(SV7A_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(PDE_DEPS)),)
-include $(PDE_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
CC1312R1_Solar_Power_\ Asset_Tracker_example_gcc.out \

EXE_OUTPUTS__QUOTED += \
"CC1312R1_Solar_Power_ Asset_Tracker_example_gcc.out" \

BIN_OUTPUTS += \
CC1312R1_Solar_Power_\ Asset_Tracker_example_gcc.hex \

BIN_OUTPUTS__QUOTED += \
"CC1312R1_Solar_Power_ Asset_Tracker_example_gcc.hex" \


# All Target
all: $(OBJS) $(LDS_SRCS) $(GEN_CMDS)
	@$(MAKE) --no-print-directory -Onone "CC1312R1_Solar_Power_ Asset_Tracker_example_gcc.out"

dependents:
	-cd /Users/jangsangjin/workspace_v10/tirtos_builds_cc13x2_cc26x2_release_gcc_5_20_00_52/Debug && $(MAKE) clean all

# Tool invocations
CC1312R1_Solar_Power_\ Asset_Tracker_example_gcc.out: $(OBJS) $(LDS_SRCS) $(GEN_CMDS) /Users/jangsangjin/workspace_v10/tirtos_builds_cc13x2_cc26x2_release_gcc_5_20_00_52/Debug/libtirtos_builds_cc13x2_cc26x2_release_gcc_5_20_00_52.a
	@echo 'Building target: "$@"'
	@echo 'Invoking: GNU Linker'
	"/Applications/ti/ccs1040/ccs/tools/compiler/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-gcc-9.2.1" @"/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc/Debug/syscfg/device_config.opt"  -mfpu=fpv4-sp-d16 -DWSN_USE_DISPLAY -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -mcpu=cortex-m4 -Wl,-Map,"CC1312R1_Solar_Power_ Asset_Tracker_example_gcc.map" -nostartfiles -static -Wl,--gc-sections -L"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/source" -L"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/kernel/tirtos/packages" -L"/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc/Debug/syscfg" -L"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/lib/thumb/v7e-m/hard" -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 --specs=nano.specs -o"CC1312R1_Solar_Power_ Asset_Tracker_example_gcc.out" $(ORDERED_OBJS)
	@echo 'Finished building target: "$@"'
	@echo ' '

CC1312R1_Solar_Power_\ Asset_Tracker_example_gcc.hex: $(EXE_OUTPUTS)
	@echo 'Building secondary target: "$@"'
	@echo 'Invoking: GNU Objcopy Utility'
	"/Applications/ti/ccs1040/ccs/tools/compiler/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-objcopy"  $(EXE_OUTPUTS__QUOTED) "CC1312R1_Solar_Power_ Asset_Tracker_example_gcc.hex"
	@echo 'Finished building secondary target: "$@"'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)
	-$(RM) "RadioTask.o" "SensorTask.o" "bg96.o" "main.o" "quectel_gps.o" "syscfg/ti_easylink_config.o" "syscfg/ti_devices_config.o" "syscfg/ti_radio_config.o" "syscfg/ti_drivers_config.o" "sara_u2.o" "si7051.o" "ublox_gps.o" "easylink/EasyLink.o" 
	-$(RM) "RadioTask.d" "SensorTask.d" "bg96.d" "main.d" "quectel_gps.d" "syscfg/ti_easylink_config.d" "syscfg/ti_devices_config.d" "syscfg/ti_radio_config.d" "syscfg/ti_drivers_config.d" "sara_u2.d" "si7051.d" "ublox_gps.d" "easylink/EasyLink.d" 
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/Users/jangsangjin/workspace_v10/tirtos_builds_cc13x2_cc26x2_release_gcc_5_20_00_52/Debug/libtirtos_builds_cc13x2_cc26x2_release_gcc_5_20_00_52.a:

-include ../makefile.targets

