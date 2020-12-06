#!/bin/bash

pushd ${BUILD_ROOT}

# 1. Get sources
OGG_PAGE=http://downloads.xiph.org/releases/ogg/
VORBIS_PAGE=http://downloads.xiph.org/releases/vorbis/
OGG_SRC=libogg-1.3.2
VORBIS_SRC=libvorbis-1.3.5
OGG_ROOT=ogg
VORBIS_ROOT=vorbis

BUILD_TARGET=
INCLUDE_NAME_OGG=ogg
INCLUDE_NAME_VORBIS=vorbis

function download_and_unpack() {
    TARBALL=$1.tar.gz
    echo $TARBALL
    if [ ! -f $TARBALL ]
    then
	curl -O $2/$TARBALL || exit 1
	rm -rf $1
        tar -xzf $TARBALL
        rm $3
        ln -s $1 $3
    fi
}
download_and_unpack $OGG_SRC $OGG_PAGE $OGG_ROOT
download_and_unpack $VORBIS_SRC $VORBIS_PAGE $VORBIS_ROOT


# 2. Build sources
CPU_COUNT=2 #`sysctl -n hw.logicalcpu_max`
OGG_STATIC=src/.libs/libogg.a
OGG_SHARED=src/.libs/libogg.so

VORBIS_STATIC=lib/.libs/lib*.a
VORBIS_SHARED=lib/.libs/lib*.so

# 2.3.1 Build LIBRARY for Android
pushd $OGG_ROOT

SONAME=libogg.so

export PATH=${ANDROID_TOOLCHAIN_ROOT}/bin:$PATH
export CROSS_PREFIX=${ANDROID_CROSS}-
export CC=${CROSS_PREFIX}gcc
export CXX=${CROSS_PREFIX}g++
export AR=${CROSS_PREFIX}ar
export RANLIB=${CROSS_PREFIX}ranlib
export CROSS_SYSROOT=${ANDROID_SYSROOT}
export CFLAGS=${ANDROID_CFLAGS}
export CPPFLAGS=${ANDROID_CPPFLAGS}
export LDFLAGS=${ANDROID_LDFLAGS}

./configure --prefix="${INSTALL_DIR}" --with-sysroot="${CROSS_SYSROOT}" --host=${ANDROID_CROSS} || exit 1
make clean && make -j ${CPU_COUNT} LDFLAGS="-avoid-version ${LDFLAGS}" || exit 1

mkdir -p ${INSTALL_DIR}/include/${INCLUDE_NAME_OGG}
cp -f -r include/${INCLUDE_NAME_OGG}/*.h ${INSTALL_DIR}/include/${INCLUDE_NAME_OGG}

cp -f ${OGG_STATIC} ${INSTALL_DIR}/lib/
cp -f ${OGG_SHARED} ${INSTALL_DIR}/lib/
popd

# 2.3.2 Build LIBRARY for Android
pushd $VORBIS_ROOT
OGG=${INSTALL_DIR}

SONAME=libvorbis.so

export PATH=${ANDROID_TOOLCHAIN_ROOT}/bin:$PATH
export CROSS_PREFIX=${ANDROID_CROSS}-
export CC=${CROSS_PREFIX}gcc
export CXX=${CROSS_PREFIX}g++
export AR=${CROSS_PREFIX}ar
export RANLIB=${CROSS_PREFIX}ranlib
export CROSS_SYSROOT=${ANDROID_SYSROOT}
export CFLAGS="${ANDROID_CFLAGS} -I${INSTALL_DIR}/include/"
export CPPFLAGS="${ANDROID_CPPFLAGS} -I${INSTALL_DIR}/include/"
export LDFLAGS="${ANDROID_LDFLAGS} -L${INSTALL_DIR}/lib/"

# disable rint: crystax's issue #1716
export CFLAGS="${CFLAGS} -DDJGPP"

# --with-ogg does not work
./configure --prefix="${INSTALL_DIR}" --with-ogg="${OGG}" --with-sysroot="${CROSS_SYSROOT}" --host=${ANDROID_CROSS} || exit 1
make clean && make -j ${CPU_COUNT} LDFLAGS="-avoid-version ${LDFLAGS}" || exit 1

mkdir -p ${INSTALL_DIR}/include/${INCLUDE_NAME_VORBIS}
cp -f -r include/${INCLUDE_NAME_VORBIS}/*.h ${INSTALL_DIR}/include/${INCLUDE_NAME_VORBIS}
cp -f ${VORBIS_STATIC} ${INSTALL_DIR}/lib/
cp -f ${VORBIS_SHARED} ${INSTALL_DIR}/lib/
popd

popd

