################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
fatfs_lib/diskio.obj: ../fatfs_lib/diskio.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti-cgt-arm_18.1.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --include_path="/home/gabriel/Workspace/eracing/elec/DAQ/CAN_module/v2/Firmware/DAQ_Module_v2" --include_path="/home/gabriel/Workspace/eracing/elec/Source_Code/tiva-c-master" --include_path="/opt/ti-cgt-arm_18.1.2.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="fatfs_lib/diskio.d_raw" --obj_directory="fatfs_lib" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

fatfs_lib/ff.obj: ../fatfs_lib/ff.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti-cgt-arm_18.1.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --include_path="/home/gabriel/Workspace/eracing/elec/DAQ/CAN_module/v2/Firmware/DAQ_Module_v2" --include_path="/home/gabriel/Workspace/eracing/elec/Source_Code/tiva-c-master" --include_path="/opt/ti-cgt-arm_18.1.2.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="fatfs_lib/ff.d_raw" --obj_directory="fatfs_lib" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

fatfs_lib/ffsystem.obj: ../fatfs_lib/ffsystem.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti-cgt-arm_18.1.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --include_path="/home/gabriel/Workspace/eracing/elec/DAQ/CAN_module/v2/Firmware/DAQ_Module_v2" --include_path="/home/gabriel/Workspace/eracing/elec/Source_Code/tiva-c-master" --include_path="/opt/ti-cgt-arm_18.1.2.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="fatfs_lib/ffsystem.d_raw" --obj_directory="fatfs_lib" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

fatfs_lib/ffunicode.obj: ../fatfs_lib/ffunicode.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti-cgt-arm_18.1.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --include_path="/home/gabriel/Workspace/eracing/elec/DAQ/CAN_module/v2/Firmware/DAQ_Module_v2" --include_path="/home/gabriel/Workspace/eracing/elec/Source_Code/tiva-c-master" --include_path="/opt/ti-cgt-arm_18.1.2.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="fatfs_lib/ffunicode.d_raw" --obj_directory="fatfs_lib" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '

fatfs_lib/tm4c123gh6pm_startup_ccs.obj: ../fatfs_lib/tm4c123gh6pm_startup_ccs.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"/opt/ti-cgt-arm_18.1.2.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me -Ooff --include_path="/home/gabriel/Workspace/eracing/elec/DAQ/CAN_module/v2/Firmware/DAQ_Module_v2" --include_path="/home/gabriel/Workspace/eracing/elec/Source_Code/tiva-c-master" --include_path="/opt/ti-cgt-arm_18.1.2.LTS/include" --define=ccs="ccs" --define=PART_TM4C123GH6PM -g --c99 --gcc --diag_warning=225 --diag_wrap=off --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="fatfs_lib/tm4c123gh6pm_startup_ccs.d_raw" --obj_directory="fatfs_lib" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


