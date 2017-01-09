#!/bin/bash

THIRD_PARTY=~/Develop/3rd_party/
THIS_DIR=$(pwd)
EXTERNALS_ROOT_DIR=`realpath -m $(pwd)/../pisk-externals`

BUILD_TYPE="RelWithDebInfo"

if [ "$1" = "android" ]
then
	PATH=$THIRD_PARTY/android-sdk/tools:$PATH
	PATH=$THIRD_PARTY/android-sdk/build-tools/25.0.1/:$PATH

	ARCH="armv7-a"
	TARGET_PLATFORM="android-15"
	ANDROID_TOOLCHAIN=arm-linux-androideabi-6

	SUBSYSTEM="activity"
	DONT_RUN_TESTS="TRUE"

	BUILD_DIR="build/${TARGET_PLATFORM}/${ARCH}"
	EXTERNALS_DIR=${EXTERNALS_ROOT_DIR}/prebuilt/${TARGET_PLATFORM}/${ARCH}/

	CMAKE_TOOLCHAIN_FILE="${THIS_DIR}/cmake/toolchain.android.${ARCH}.cmake"
	TOOLCHAIN_ROOT=${EXTERNALS_ROOT_DIR}/sources/${TARGET_PLATFORM}/${ARCH}/${ANDROID_TOOLCHAIN}/

	PROJECT_OUTPUT_DIR="${THIS_DIR}/.output/${TARGET_PLATFORM}/${ARCH}"

	CMAKE_CMD="cmake
	-DCMAKE_BUILD_TYPE=${BUILD_TYPE}
	-DDONT_RUN_TESTS=${DONT_RUN_TESTS}
	-DEXTERNALS_DIR=${EXTERNALS_DIR}
	-DSUBSYSTEM=${SUBSYSTEM}

	-DTOOLCHAIN_ROOT=${TOOLCHAIN_ROOT}

	-DCMAKE_TOOLCHAIN_FILE=${CMAKE_TOOLCHAIN_FILE}
	-DCMAKE_SYSROOT=${TOOLCHAIN_ROOT}/sysroot
	-DCMAKE_FIND_ROOT_PATH=${EXTERNALS_DIR}
	-DCMAKE_INCLUDE_PATH=${EXTERNALS_DIR}

	-DPROJECT_OUTPUT_DIR=${PROJECT_OUTPUT_DIR}
	../../.."

elif [ "$1" = "" ]
then
	ARCH=$(uname -m)
	TARGET_PLATFORM=$(uname -s)
	SUBSYSTEM="cui"

	BUILD_DIR="build/Linux/${ARCH}"
	PROJECT_OUTPUT_DIR="${THIS_DIR}/.output/${TARGET_PLATFORM}/${ARCH}"

	CMAKE_CMD="cmake
	-DCMAKE_BUILD_TYPE=${BUILD_TYPE}
	-DSUBSYSTEM=${SUBSYSTEM}
	../../.."
else
	echo unexpected build target
	exit 1
fi

cmake -E make_directory $BUILD_DIR
cmake -E chdir $BUILD_DIR $CMAKE_CMD
cmake --build $BUILD_DIR --config ${BUILD_TYPE} -- -sj2

#copy so with links

if [ ! -z ${EXTERNALS_DIR} ]
then
	cp ${EXTERNALS_DIR}/lib/*.so ${PROJECT_OUTPUT_DIR}/libs/
fi

