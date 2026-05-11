cmake_minimum_required(VERSION 3.22)
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


#set(TOOLCHAIN_DIR "D:/00_workgit/gd32_tools/GNU Tools ARM Embedded/xpack-arm-none-eabi-gcc/9.2.1-1.1/bin")
set(TOOLCHAIN_DIR "D:/00_workgit/xpack-arm-none-eabi-gcc-14.2.1-1.1/bin")
set(TOOLCHAIN_PREFIX "arm-none-eabi-")


#set(CMAKE_C_COMPILER "${TOOLCHAIN_DIR}\\${TOOLCHAIN_PREFIX}gcc.exe")
#set(CMAKE_C_COMPILER "D:/00_workgit/gd32_tools/GNU Tools ARM Embedded/xpack-arm-none-eabi-gcc/9.2.1-1.1/bin/arm-none-eabi-gcc.exe")
set(CMAKE_C_COMPILER "D:/00_workgit/xpack-arm-none-eabi-gcc-14.2.1-1.1/bin/arm-none-eabi-gcc.exe")
set(CMAKE_C_COMPILER_WORKS 1)
# Alternative approach using find_program (recommended)
# find_program(ARM_GCC_COMPILER NAMES arm-none-eabi-gcc PATHS ${TOOLCHAIN_DIR} REQUIRED)
# set(CMAKE_C_COMPILER ${ARM_GCC_COMPILER})
if(EXISTS "${TOOLCHAIN_DIR}/arm-none-eabi-gcc.exe")
    message(STATUS "Compiler found: ${TOOLCHAIN_DIR}/arm-none-eabi-gcc.exe")
else()
    message(FATAL_ERROR "Compiler NOT found: ${TOOLCHAIN_DIR}/arm-none-eabi-gcc.exe")
endif()

#set project name and languages
project(ups_panel VERSION 1.0 LANGUAGES C ASM)   
# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
#set(CMAKE_PROJECT_NAME gd32f_cmake)
set(TARGET ${PROJECT_NAME}.elf)

message(" ")
message("Target name: " ${TARGET})
message("Build type: " ${CMAKE_BUILD_TYPE})
message("-> TOOLPATH:" ${TOOLCHAIN_DIR})
message("-> CMAKE_C_COMPILER:" ${CMAKE_C_COMPILER})
message("-> CMAKE_CXX_COMPILER:" ${CMAKE_CXX_COMPILER})
message("-> CMAKE_ASM_COMPILER:" ${CMAKE_ASM_COMPILER})
message(" ")

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Create an executable object type
add_executable(${TARGET} "")
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}.elf")

# Link directories setup
#target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
#    # Add user defined library search paths
#
#)

# Add sources to executable
target_sources(${TARGET} PRIVATE
    # Add user sources here
    ${CMAKE_SOURCE_DIR}/src/main.c
    ${CMAKE_SOURCE_DIR}/src/gd32f30x_it.c
    ${CMAKE_SOURCE_DIR}/src/systick.c
    ${CMAKE_SOURCE_DIR}/src/sin_timer.c
    ${CMAKE_SOURCE_DIR}/src/sin_table.c
    ${CMAKE_SOURCE_DIR}/src/Debug/debug.c
    ${CMAKE_SOURCE_DIR}/src/driver_pwm.c
    ${CMAKE_SOURCE_DIR}/src/relay.c
    ${CMAKE_SOURCE_DIR}/src/adc.c
    ${CMAKE_SOURCE_DIR}/src/global.c
    ${CMAKE_SOURCE_DIR}/src/timer.c
    ${CMAKE_SOURCE_DIR}/src/app_time.c
    ${CMAKE_SOURCE_DIR}/src/timer_impl.c
    ${CMAKE_SOURCE_DIR}/src/state_machine.c
    ${CMAKE_SOURCE_DIR}/src/lcd.c
    ${CMAKE_SOURCE_DIR}/src/encoder.c

    ${CMAKE_SOURCE_DIR}/CMSIS/GD/GD32F30x/Source/ARM/startup_gd32f30x_hd.S
    ${CMAKE_SOURCE_DIR}/CMSIS/GD/GD32F30x/Source/system_gd32f30x.c
    ${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Source/gd32f30x_rcu.c
    ${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Source/gd32f30x_gpio.c
    ${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Source/gd32f30x_misc.c
    ${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Source/gd32f30x_exti.c
    ${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Source/gd32f30x_usart.c
    ${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Source/gd32f30x_timer.c
    ${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Source/gd32f30x_dma.c
    ${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Source/gd32f30x_adc.c
    ${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Source/gd32f30x_i2c.c
    #${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Source/gd32f30x_gpio.c
    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/Source/system_gd32f30x.c
    ${CMAKE_SOURCE_DIR}/src/gd32f30x_eval.c
    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/Firmware/Peripherals/src/gd32f30x_rcu.c
    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/Firmware/Peripherals/src/gd32f30x_gpio.c
    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/Firmware/Peripherals/src/gd32f30x_misc.c
    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/Firmware/Peripherals/src/gd32f30x_exti.c
    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/Firmware/Peripherals/src/gd32f30x_usart.c
    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/Firmware/Peripherals/src/gd32f30x_timer.c

)

# Add include paths
target_include_directories(${TARGET} PRIVATE
    # Add user defined include paths
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/Debug
    ${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Include
    ${CMAKE_SOURCE_DIR}/CMSIS
    ${CMAKE_SOURCE_DIR}/CMSIS/GD/GD32F30x/Include

    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/Include
    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/Utilities/
    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/GigaDevice/GD32F30x/Include
    #${CMAKE_SOURCE_DIR}/pack/GigaDevice/GD32F30x_DFP.2.0.0/Device/Firmware/Peripherals/inc
    #${CMAKE_SOURCE_DIR}/GD32F30x_standard_peripheral/Include
    
)


# Add linked libraries
#target_link_libraries(${CMAKE_PROJECT_NAME}
#    stm32cubemx

    # Add user defined libraries
#)

#set(COMMON_FLAGS "-mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard -g -ffunction-sections -fdata-sections")
set(COMMON_FLAGS
    -mcpu=cortex-m4
    -mthumb
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
    -g
    -ffunction-sections
    -fdata-sections
    -fno-exceptions

)
set(FPU_OPTS -mcpu=cortex-m4 -mthumb -mfpu=fpv4-sp-d16 -mfloat-abi=hard)

#convert list to string
string(REPLACE ";" " " COMMON_FLAGS_STR "${COMMON_FLAGS}")
string(REPLACE ";" " " FPU_OPTS_STR "${FPU_OPTS}")

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS_STR}")
set(CMAKE_ASM_FLAGS "${CMAKE_ASM_FLAGS} ${COMMON_FLAGS_STR} -x assembler-with-cpp")



set(CUSTOM_OPTIONS
    -Wall
    -Og
    -g3
    -nostdlib
    -nostartfiles
    -nodefaultlibs
    -ffreestanding
    -fno-builtin
    -Wextra
    -fstack-usage
    -fsigned-char
    #-Wconversion
    -gdwarf-5
    -Wl,--print-memory-usage
    #-specs=rdimon.specs          # Вместо nosys.specs — лучше поддерживает printf_float
    -specs=nano.specs             #(если нужна компактная версия)
    

    
)



set(STARTUP_FILE ${CMAKE_SOURCE_DIR}/CMSIS/GD/GD32F30x/Source/ARM/startup_gd32f30x_hd.S)
set_source_files_properties(${STARTUP_FILE} PROPERTIES
    COMPILE_FLAGS ${CMAKE_ASM_FLAGS}
    LANGUAGE ASM
) 

target_compile_options(${TARGET} PRIVATE
    $<$<COMPILE_LANGUAGE:C>:${COMMON_FLAGS} ${CUSTOM_OPTIONS}>
#    $<$<COMPILE_LANGUAGE:ASM>:${COMMON_FLAGS} -x assembler-with-cpp>
)

 

add_link_options(
                    -nostdlib
                    -nostartfiles
                    -nodefaultlibs
                    -ffreestanding
                    -fno-builtin
                    -specs=nosys.specs
                    -Wl,--gc-sections
                    -Wl,--print-memory-usage
                    -Wl,-Map,${PROJECT_NAME}.map,--cref
                    #-Wl,--no-warn-rwx-segments
                    -u _printf_float      # Разрешает printf с float
                    #-specs=rdimon.specs          # Вместо nosys.specs — лучше поддерживает printf_float
                    -specs=nano.specs (если нужна компактная версия)
                    
                    #--enable-newlib-io-long-long
                #    -o ${PROJECT_NAME}.elf
                    #-L "/home/vit/riscv_gcc/xpack-riscv-none-embed-gcc-8.2.0-3.1-linux-x64/xPacks/riscv-none-embed-gcc/8.2.0-3.1/riscv-none-embed/lib"
                    -lm
                    -Wl,-noexecstack
                    #-verbose

                    #--def flashOrigin=0x08000000
                    #--def flashLength=0x80000 
                    #-mfpu=fpv4-sp-d16
                    #-mfloat-abi=hard
                    #-x assembler-with-cpp
                    #-mfloat-abi=hard
                )



message(STATUS "Compiler flags: ${CMAKE_C_FLAGS}")
message(STATUS "Linker flags: ${LINKER_FLAGS}")
execute_process(
    COMMAND ${CMAKE_C_COMPILER} -dumpspecs
    OUTPUT_VARIABLE GCC_SPECS
)
if(GCC_SPECS MATCHES "fpv4-sp-d16")
    message(STATUS "FPU support detected in compiler specs")
else()
    message(WARNING "FPU not detected in compiler specs!")
endif()
message(STATUS "Using compiler: ${CMAKE_C_COMPILER}")
execute_process(
    COMMAND ${CMAKE_C_COMPILER} --version
    OUTPUT_VARIABLE COMPILER_VERSION
)
message(STATUS "Compiler version: ${COMPILER_VERSION}")




set(LINKER_SCRIPT  ${CMAKE_SOURCE_DIR}/CMSIS/GD/GD32F30x/gd32f30x_flash.ld)
list(APPEND LINKER_FLAGS "-T${LINKER_SCRIPT}")
target_link_options(${TARGET} PRIVATE
    ${LINKER_FLAGS}
    -Wl,--print-memory-usage    #need pass to linker directly
)

set(FLASH_SIZE 262144)  # 256 KB
set(RAM_SIZE   65536)   # 64 KB


set(BINNAME ${PROJECT_NAME})
set(HEX_FILE ${PROJECT_BINARY_DIR}/${BINNAME}.hex)
set(BIN_FILE ${PROJECT_BINARY_DIR}/${BINNAME}.bin)
set(LST_FILE ${PROJECT_BINARY_DIR}/${BINNAME}.lst)
MESSAGE(STATUS "${PROJECT_BINARY_DIR}, ${BINNAME}")

#add_custom_command(TARGET ${TARGET} PRE_LINK
#    COMMAND @${CMAKE_COMMAND} -E echo "------------------------------------LINK----------------------------------------"
#)

#add_custom_command(TARGET ${TARGET} PRE_BUILD
#    COMMAND @${CMAKE_COMMAND} -E echo "------------------------------------1--------------------------------------------"
#)

add_custom_command(TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${TARGET}> ${HEX_FILE}
            COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${TARGET}> ${BIN_FILE}
            COMMAND ${CMAKE_OBJDUMP} --source --all-headers --demangle --debugging --line-numbers --syms --wide $<TARGET_FILE:${TARGET}> > ${LST_FILE}
#            COMMAND @${CMAKE_COMMAND} -E echo "--------------------------------------------------------------------------------"
#            #COMMAND ${CMAKE_SIZE} --format=sysv --totals $<TARGET_FILE:${TARGET}>
#            COMMAND ${CMAKE_SIZE} --format=berkeley  $<TARGET_FILE:${TARGET}>
#            COMMAND @${CMAKE_COMMAND} -E echo "--------------------------------------------------------------------------------"
)
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK)            