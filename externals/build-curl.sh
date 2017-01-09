#!/bin/bash
pushd ${BUILD_ROOT}

# 1. Get sources
LIBCURL_SRC=curl-7.51.0
OPENSSL_SRC=openssl-1.1.0c
LIBCURL_PAGE=https://curl.haxx.se/download
OPENSSL_PAGE=https://www.openssl.org/source
LIBCURL_ROOT=curl
OPENSSL_ROOT=openssl

INCLUDE_NAME_OPENSSL=openssl
INCLUDE_NAME_CURL=curl

# param 1: lib name
# param 2: download page
# param 3: symlink to source dir
# exit with status 1 if downloading failed
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

download_and_unpack $LIBCURL_SRC $LIBCURL_PAGE $LIBCURL_ROOT
download_and_unpack $OPENSSL_SRC $OPENSSL_PAGE $OPENSSL_ROOT

# 2. Build sources
CURL_EXTRA="--enable-ipv6 --disable-ftp --disable-file --disable-ldap --disable-ldaps --disable-rtsp --disable-proxy --disable-dict --disable-telnet --disable-tftp --disable-pop3 --disable-imap --disable-smtp --disable-gopher --disable-sspi --disable-manual"
LIBCURL_STATIC=lib/.libs/libcurl.a
LIBCURL_SHARED=lib/.libs/libcurl.so
LIBCRYPTO_STATIC=libcrypto.a
LIBCRYPTO_SHARED=libcrypto.so
LIBOPENSSL_STATIC=libssl.a
LIBOPENSSL_SHARED=libssl.so
CPU_COUNT=2 #`sysctl -n hw.logicalcpu_max`
export IPHONEOS_DEPLOYMENT_TARGET="5.0"

# # 2.1 Build cURL for iOS device (armv7, armv7s)
# pushd $LIBCURL_ROOT
# export CC="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/usr/bin/llvm-gcc-4.2"
# export CFLAGS="-arch armv7 -arch armv7s -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
# export LDFLAGS="-arch armv7 -arch armv7s -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS6.1.sdk"
# ./configure --disable-shared --enable-static --disable-dependency-tracking --host=armv7-apple-darwin --with-darwinssl $CURL_EXTRA || exit 1
# make clean && make -j $CPU_COUNT || exit 1
# cp -f $LIBCURL_STATIC ${INSTALL_ROOT}/prebuilt/ios/device
# cp -f $LIBCURL_SHARED ${INSTALL_ROOT}/prebuilt/ios/device
# popd
#
# # 2.2 Build cURL for iOS simulator (i386)
# pushd $LIBCURL_ROOT
# export CC="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/usr/bin/llvm-gcc-4.2"
# export CFLAGS="-arch i386 -pipe -Os -gdwarf-2 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator6.1.sdk"
# export CPPFLAGS="-D__IPHONE_OS_VERSION_MIN_REQUIRED=${IPHONEOS_DEPLOYMENT_TARGET%%.*}0000"
# export LDFLAGS="-arch i386 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer/SDKs/iPhoneSimulator6.1.sdk"
# ./configure --disable-shared --enable-static --disable-dependency-tracking --host=i386-apple-darwin --with-darwinssl $CURL_EXTRA || exit 1
# make clean && make -j $CPU_COUNT || exit 1
# cp -f $LIBCURL_STATIC ${INSTALL_ROOT}/prebuilt/ios/simulator
# cp -f $LIBCURL_SHARED ${INSTALL_ROOT}/prebuilt/ios/simulator
# popd

# 2.3.1 Build OpenSSL for Android
pushd $OPENSSL_ROOT
# See 0 above

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
./Configure android-${ANDROID_ABI} shared || exit 1
make clean && make build_libs CALC_VERSIONS="SHLIB_COMPAT=; SHLIB_SOVER=" -j ${CPU_COUNT} || exit 1

mkdir -p ${INSTALL_DIR}/include/${INCLUDE_NAME_OPENSSL}
cp -f -r ./include/${INCLUDE_NAME_OPENSSL}/*.h ${INSTALL_DIR}/include/${INCLUDE_NAME_OPENSSL}/

cp -f ${LIBCRYPTO_STATIC} ${INSTALL_DIR}/lib/
cp -f ${LIBCRYPTO_SHARED} ${INSTALL_DIR}/lib/
cp -f ${LIBOPENSSL_STATIC} ${INSTALL_DIR}/lib/
cp -f ${LIBOPENSSL_SHARED} ${INSTALL_DIR}/lib/
popd

# 2.3.2 Build CURL for Android
pushd $LIBCURL_ROOT
OPENSSL=`pwd`/../openssl

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
./configure --with-sysroot=${CROSS_SYSROOT} --target=${ANDROID_CROSS} --host=${ANDROID_CROSS} --with-random=/dev/urandom --with-ssl=$OPENSSL --without-ca-bundle --without-ca-path --with-zlib $CURL_EXTRA || exit 1
make clean && make -j ${CPU_COUNT} || exit 1

mkdir -p ${INSTALL_DIR}/include/${INCLUDE_NAME_CURL}
cp -f -r ./include/${INCLUDE_NAME_CURL}/*.h ${INSTALL_DIR}/include/${INCLUDE_NAME_CURL}/

cp -f ${LIBCURL_STATIC} ${INSTALL_DIR}/lib/
cp -f ${LIBCURL_SHARED} ${INSTALL_DIR}/lib/
popd

popd

