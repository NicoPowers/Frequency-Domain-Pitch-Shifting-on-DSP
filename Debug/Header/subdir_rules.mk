################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
Header/%.obj: ../Header/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: C2000 Compiler'
	"D:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS/bin/cl2000" -v28 -ml -mt --cla_support=cla1 --float_support=fpu32 --tmu_support=tmu0 --vcu_support=vcu2 -Ooff --opt_for_speed=2 --fp_mode=relaxed --include_path="D:/ti/c2000/C2000Ware_3_03_00_00/libraries/dsp/FPU/c28/source/fpu32/fft" --include_path="D:/ti/c2000/C2000Ware_3_03_00_00/libraries/dsp/FPU/c28/include" --include_path="D:/ti/c2000/C2000Ware_3_03_00_00/libraries/dsp/FPU/c28/include/fpu32" --include_path="D:/Google Drive/College stuff/UF Stuff/Fall 2020/EEE4511C/Final Project/DSP Code/Final Project CCS" --include_path="D:/ti/ccs1011/ccs/tools/compiler/ti-cgt-c2000_20.2.2.LTS/include" --include_path="D:/Google Drive/College stuff/UF Stuff/Fall 2020/EEE4511C/Final Project/DSP Code/Final Project CCS/Header" --include_path="D:/Google Drive/College stuff/UF Stuff/Fall 2020/EEE4511C/Final Project/DSP Code/Final Project CCS/Common" --include_path="D:/ti/c2000/C2000Ware_3_03_00_00/device_support/f2837xd/headers/include" --include_path="D:/ti/c2000/C2000Ware_3_03_00_00/device_support/f2837xd/common/include" --include_path="D:/ti/c2000/C2000Ware_3_03_00_00/driverlib/f2837xd/driverlib" --advice:performance=all --define=CPU1 --define=_LAUNCHXL_F28379D -g --c99 --diag_warning=225 --diag_wrap=off --display_error_number --abi=coffabi --preproc_with_compile --preproc_dependency="Header/$(basename $(<F)).d_raw" --obj_directory="Header" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


