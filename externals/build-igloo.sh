#!/bin/bash

pushd ${BUILD_ROOT}

# 1. Get sources
GIT_PAGE_IGLOO=https://github.com/joakimkarlsson/igloo.git
LIB_ROOT=igloo

GIT_PAGE_SNOWHOUSE=https://github.com/joakimkarlsson/snowhouse.git
SNOWHOUSE_ROOT=${LIB_ROOT}/igloo/external/snowhouse

if [ ! -d ${LIB_ROOT} ]
then
	git clone $GIT_PAGE_IGLOO $LIB_ROOT || exit 1
	git clone $GIT_PAGE_SNOWHOUSE $SNOWHOUSE_ROOT || exit 1
fi


INCLUDE_NAME_IGLOO=igloo
cp -f -r ./igloo/${INCLUDE_NAME_IGLOO} ${INSTALL_DIR}/include/

popd

