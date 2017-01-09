#!/bin/bash

export ANDROID_ABI=armeabi
export ANDROID_ARCH=armv7-a
export ANDROID_ARCH_FAMILY=arm
export ANDROID_PLATFORM=android-15
export ANDROID_TOOLCHAIN=arm-linux-androideabi-6
export ANDROID_CROSS=arm-linux-androideabi
export ANDROID_SYSTEM=linux-x86_64

export ANDROID_CFLAGS="-march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16"
export ANDROID_CPPFLAGS="-DANDROID -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16"
export ANDROID_LDFLAGS="-march=armv7-a -Wl,--fix-cortex-a8"

export EXTERNAL_ROOT=${PWD}/../../pisk-externals
export PISK_ROOT=${PWD}/../../pisk
export INSTALL_ROOT=${EXTERNAL_ROOT}
export COMMON_BUILD_ROOT=${EXTERNAL_ROOT}/sources
export BUILD_ROOT=${COMMON_BUILD_ROOT}/${ANDROID_PLATFORM}/${ANDROID_ARCH}

export NDK_ROOT=${COMMON_BUILD_ROOT}/crystax-ndk

export ANDROID_TOOLCHAIN_ROOT=${BUILD_ROOT}/${ANDROID_TOOLCHAIN}
export ANDROID_SYSROOT=${ANDROID_TOOLCHAIN_ROOT}/sysroot

export INSTALL_DIR=${INSTALL_ROOT}/prebuilt/${ANDROID_PLATFORM}/${ANDROID_ARCH}

mkdir -p ${BUILD_ROOT}
# mkdir -p ${INSTALL_ROOT}/prebuilt/ios/device
# mkdir -p ${INSTALL_ROOT}/prebuilt/ios/simulator
mkdir -p ${INSTALL_DIR}/bin/
mkdir -p ${INSTALL_DIR}/include/
mkdir -p ${INSTALL_DIR}/lib/

./get-android-sdk.sh || exit 1
./get-android-ndk.sh || exit 1
./build-toolchain.sh || exit 1

./build-igloo.sh || exit 1
./build-curl.sh || exit 1
./build-json.sh || exit 1
./build-luajit.sh || exit 1
./build-oggvorbis.sh || exit 1
./build-openal.sh || exit 1

if [ -f "${ANDROID_TOOLCHAIN_ROOT}/arm-linux-androideabi/lib/${ANDROID_ARCH}/libcrystax.so" ]
then
	cp -f ${ANDROID_TOOLCHAIN_ROOT}/arm-linux-androideabi/lib/${ANDROID_ARCH}/libcrystax.so ${INSTALL_DIR}/lib/ || exit 1
else
	cp -f ${ANDROID_TOOLCHAIN_ROOT}/arm-linux-androideabi/lib/libcrystax.so ${INSTALL_DIR}/lib/ || exit 1
fi

#cp -f ${NDK_ROOT}/platforms/${ANDROID_PLATFORM}/arch-${ANDROID_ARCH_FAMILY}/usr/lib/libdl.so ${INSTALL_DIR}/lib/
#cp -f ${NDK_ROOT}/platforms/${ANDROID_PLATFORM}/arch-${ANDROID_ARCH_FAMILY}/usr/lib/liblog.so ${INSTALL_DIR}/lib/
#cp -f ${NDK_ROOT}/platforms/${ANDROID_PLATFORM}/arch-${ANDROID_ARCH_FAMILY}/usr/lib/libandroid.so ${INSTALL_DIR}/lib/

