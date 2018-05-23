# Modified by andres@uma.es 7/2014
# common variables to use in makefiles for Odroid and Snapdragon boards

#hay que instalar las multilib:
#   sudo apt-get install gcc-4.8-multilib
#   sudo apt-get install g++-4.8-multilib

INCLUDE_TBB_ARM= /root/tbb44/include
LIB_TBB_ARM = /root/tbb44/build/linux_armv7_gcc_cc4.6_libc2.15_kernel3.6.0_release

INCLUDE_TBB_APPLE= /Users/asenjo/Documents/Invest/tbb44/include
LIB_TBB_APPLE= /Users/asenjo/Documents/Invest/tbb44/build/macos_intel64_clang_cc7.3.0_os10.11.5_release

# Para compilar tbb sin modificar ningún fichero. Directamente desde los fuentes usando make.

# make target=linux arch=arm CXXFLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=neon" ??

# .../tbb42_********oss/build/***
# yo he hecho un softlink en tbb/lib

