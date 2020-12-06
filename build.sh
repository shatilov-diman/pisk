#!/bin/bash

THIRD_PARTY=~/Develop/3rd_party/
THIS_DIR=$(pwd)
EXTERNALS_ROOT_DIR=`realpath -m $(pwd)/../pisk-externals`
EXTERNAL_TAG=-crystax-10.3.2

BUILD_TYPE="RelWithDebInfo"

if [ "$1" = "android" ]; then
	PATH=$THIRD_PARTY/android-sdk/tools:$PATH
	PATH=$THIRD_PARTY/android-sdk/build-tools/25.0.1/:$PATH

	ARCH="armv7-a"
	TARGET_PLATFORM="android-15"
	ANDROID_TOOLCHAIN=arm-linux-androideabi-5

	SUBSYSTEM="activity"
	DONT_RUN_TESTS="TRUE"

	BUILD_DIR=".build/${TARGET_PLATFORM}/${ARCH}"
	EXTERNALS_DIR=${EXTERNALS_ROOT_DIR}/prebuilt${EXTERNAL_TAG}/${TARGET_PLATFORM}/${ARCH}/

	CMAKE_TOOLCHAIN_FILE="${THIS_DIR}/cmake/toolchain.android.${ARCH}.cmake"
	TOOLCHAIN_ROOT=${EXTERNALS_ROOT_DIR}/sources${EXTERNAL_TAG}/${TARGET_PLATFORM}/${ARCH}/${ANDROID_TOOLCHAIN}/

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

elif [ "$1" = "unix" ]; then
	ARCH=$(uname -m)
	TARGET_PLATFORM=$(uname -s)
	if [ "$2" == "cui" ]; then
		echo "Console subsystem building"
	elif [ "$2" == "gui" ]; then
		echo "X subsystem building"
	else
		echo "please, provide subsystem (cui or gui)"
		exit 1
	fi
	SUBSYSTEM="$2"

	BUILD_DIR=".build/Linux/${ARCH}"
	PROJECT_OUTPUT_DIR="${THIS_DIR}/.output/${TARGET_PLATFORM}/${ARCH}"

	export CC=/usr/bin/gcc
	export CXX=/usr/bin/g++

	if [ "${TARGET_PLATFORM}" == "Darwin" ]; then
		CMAKE_CMD="cmake
		-DCMAKE_BUILD_TYPE=${BUILD_TYPE}
		-DSUBSYSTEM=${SUBSYSTEM}
		-DCURL_INCLUDE_DIR:PATH=/usr/local/opt/openssl/include
		-DCURL_LIBRARY:PATH=/usr/local/opt/curl/lib/libcurl.dylib
		-DOPENSSL_ROOT_DIR:PATH=/usr/local/opt/openssl/
		-DIGLOO_ROOT_DIR:PATH=~/Develop/3rd_party/igloo/
		-DGTEST_ROOT:PATH=~/Develop/3rd_party/googletest/googletest/
		-DGMOCK_ROOT:PATH=~/Develop/3rd_party/googletest/googlemock/
		../../.."
	else
		CMAKE_CMD="cmake
		-DCMAKE_BUILD_TYPE=${BUILD_TYPE}
		-DSUBSYSTEM=${SUBSYSTEM}
		../../.."
	fi
else
	echo "please, provide os (unix or android)"
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

