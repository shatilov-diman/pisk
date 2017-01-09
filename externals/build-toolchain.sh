#!/bin/bash

pushd ${BUILD_ROOT}

# 0. Make "standalone toolchain" for android
if [ ! -d ${ANDROID_TOOLCHAIN_ROOT} ]
then
	${NDK_ROOT}/build/tools/make-standalone-toolchain.sh --platform=${ANDROID_PLATFORM} --install-dir=${ANDROID_TOOLCHAIN_ROOT} --toolchain=${ANDROID_TOOLCHAIN} --system=${ANDROID_SYSTEM}
fi
if [ ! -d ${ANDROID_TOOLCHAIN_ROOT} ]
then
	echo "Unable to build android standalone toolchain"
	exit
fi

popd


TARGET_DIR=${INSTALL_DIR}/android_helps
mkdir -p ${TARGET_DIR}

function copy_helper_module() {
	echo $1
	if [ ! -d ${TARGET_DIR}/$1 ]
	then
		cp -rf $2/$1 ${TARGET_DIR} || exit 1
	fi

	if [ ! -d ${TARGET_DIR}/$1 ]
	then
		echo "Unable to copy $1"
		exit 1
	fi
}

copy_helper_module native_app_glue ${NDK_ROOT}/sources/android/ || exit 1

