################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Applications/ti/ccs1040/ccs/tools/compiler/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-gcc-9.2.1" -c @"/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc/Debug/syscfg/device_config.opt"  -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DWSN_USE_DISPLAY -I"/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc" -I"/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc/Debug" -I"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/source" -I"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/source/ti/posix/gcc" -I"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano" -I"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include" -I"/Applications/ti/ccs1040/ccs/tools/compiler/gcc-arm-none-eabi-9-2019-q4-major/arm-none-eabi/include" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"$(basename $(<F)).d_raw" -MT"$(@)" -I"/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc/Debug/syscfg" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

build-818785841: ../rfWsnNode.syscfg
	@echo 'Building file: "$<"'
	@echo 'Invoking: SysConfig'
	"/Applications/ti/ccs1040/ccs/utils/sysconfig_1.9.0/sysconfig_cli.sh" -s "/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/.metadata/product.json" --script "/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc/rfWsnNode.syscfg" -o "syscfg" --compiler gcc
	@echo 'Finished building: "$<"'
	@echo ' '

syscfg/ti_easylink_config.h: build-818785841 ../rfWsnNode.syscfg
syscfg/ti_easylink_config.c: build-818785841
syscfg/device_config.opt: build-818785841
syscfg/ti_devices_config.c: build-818785841
syscfg/ti_radio_config.c: build-818785841
syscfg/ti_radio_config.h: build-818785841
syscfg/ti_drivers_config.c: build-818785841
syscfg/ti_drivers_config.h: build-818785841
syscfg/ti_utils_build_linker.cmd.genlibs: build-818785841
syscfg/syscfg_c.rov.xs: build-818785841
syscfg/ti_utils_runtime_model.gv: build-818785841
syscfg/ti_utils_runtime_Makefile: build-818785841
syscfg/: build-818785841

syscfg/%.o: ./syscfg/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: GNU Compiler'
	"/Applications/ti/ccs1040/ccs/tools/compiler/gcc-arm-none-eabi-9-2019-q4-major/bin/arm-none-eabi-gcc-9.2.1" -c @"/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc/Debug/syscfg/device_config.opt"  -mcpu=cortex-m4 -march=armv7e-m -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -DWSN_USE_DISPLAY -I"/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc" -I"/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc/Debug" -I"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/source" -I"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/source/ti/posix/gcc" -I"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include/newlib-nano" -I"/Users/jangsangjin/ti/simplelink_cc13x2_26x2_sdk_5_20_00_52/kernel/tirtos/packages/gnu/targets/arm/libs/install-native/arm-none-eabi/include" -I"/Applications/ti/ccs1040/ccs/tools/compiler/gcc-arm-none-eabi-9-2019-q4-major/arm-none-eabi/include" -O0 -ffunction-sections -fdata-sections -g -gdwarf-3 -gstrict-dwarf -Wall -MMD -MP -MF"syscfg/$(basename $(<F)).d_raw" -MT"$(@)" -I"/Users/jangsangjin/workspace_v10/CC1312R1_Solar_Power_ Asset_Tracker_example_gcc/Debug/syscfg" -std=c99 $(GEN_OPTS__FLAG) -o"$@" "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


