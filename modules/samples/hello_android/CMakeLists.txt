cmake_minimum_required(VERSION 2.8)

if (NOT DEFINED ANDROID_ABI) #deprecated
	message(FATAL_ERROR "Please, provide ANDROID_ABI (armeabi-v7a, mips, etc)")
endif()

list(APPEND CMAKE_MODULE_PATH ${PISK_FIND_CMAKE_DIR})


set(MY_PROJ_NAME hello_android)
project(${MY_PROJ_NAME})

find_package(pisk REQUIRED)

add_custom_target(${MY_PROJ_NAME} ALL)


include ("${PISK_ROOT_DIR}/cmake/toolchain.android.apk.cmake" REQUIRED)
file(COPY "${PISK_PLATFORM_DIR}/android" DESTINATION "${PROJECT_SOURCE_DIR}")
android_create_apk(${MY_PROJ_NAME} "os" "com.sample.hello_android" "${PROJECT_SOURCE_DIR}/apk/${ANDROID_ABI}" "${PISK_LIBRARIES_DIR}/" "${PROJECT_SOURCE_DIR}/android" "${PROJECT_SOURCE_DIR}/data")

