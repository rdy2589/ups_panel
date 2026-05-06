#!/bin/sh
#set -x # debug mode on
clear

echo "----------------------Make gd32f_cmake----------------------------------"
rm -rf build
mkdir build

#export PATH=/usr/local/gcc-arm-none-eabi/bin:$PATH
export PATH=/home/rdy/00_workgit/stm32/gcc-arm-none-eabi/bin/:$PATH

make clean
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=cmake/gcc-arm-none-eabi.cmake

#cmake -DGOAL="mbs_hub" -S src/ -B "build_mbs" -DDEV_TYPE=0 -DCMAKE_VERBOSE_MAKEFILE=ON    #build to directory
#MAKE_PATH=$RISCV_TOOLCHAIN_PATH/make-3.8/usr/local/bin
STM32_OBJCOPY_PATH=D:\00_workgit\gd32_tools\GNU Tools ARM Embedded\xpack-arm-none-eabi-gcc\9.2.1-1.1\bin\arm-none-eabi-objcopy.exe
#OPENOCD_PATH=$TOOLCHAIN_DIR/OpenOCD/bin

#echo "MAKE_PATH =" $MAKE_PATH
#echo "OPENOCD_PATH =" $OPENOCD_PATH


CORES=$(nproc)
#make VERBOSE=1 -j$CORES all --directory="./build/" --file="Makefile" -Wstack-usage=2048
make -j$CORES all --directory="./build/" --file="Makefile" -Wstack-usage=2048
$STM32_OBJCOPY_PATH -O ihex ./build/gd32f_cmake.elf ./build/gd32f_cmake.hex 
set -e
#cp ./build_mam/mam_sdr.hex ./bin
#cp ./build_mam/mam_sdr.bin ./bin
#cp ./build_mbs/mbs_hub.hex ./bin
#cp ./build_mbs/mbs_hub.bin ./bin

# -n