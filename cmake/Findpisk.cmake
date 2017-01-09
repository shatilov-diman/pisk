cmake_minimum_required(VERSION 2.8)

include (FindPackageHandleStandardArgs)
include (CheckLibraryExists)

find_path (PISK_ROOT_DIR
	NAMES include/pisk/defines.h
		base/include/pisk/defines.h
	HINTS / /pisk ENV PISK_ROOT_DIR
	DOC "pisk root dir")

if (NOT DEFINED TARGET_PLATFORM)
	set(PISK_PLATFORM_DIR ${PISK_ROOT_DIR}/.output/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR})
else ()
	set(PISK_PLATFORM_DIR ${PISK_ROOT_DIR}/.output/${TARGET_PLATFORM}/${CMAKE_SYSTEM_PROCESSOR})
endif()

find_path (PISK_INCLUDE_DIRS
	NAMES pisk/defines.h include/pisk/defines.h
	HINTS ${PISK_ROOT_DIR} ${PISK_PLATFORM_DIR}
	DOC "pisk include directory")

set (PISK_LIBRARIES "")
set (MAIN_MODULES_LIST "base" "os" "system")

foreach(MODULE ${MAIN_MODULES_LIST})
	unset (MODULE_LIBRARY_DIR CACHE)
	find_library (MODULE_LIBRARY_DIR ${MODULE}
		HINTS ${PISK_ROOT_DIR} ${PISK_PLATFORM_DIR}
		PATH_SUFFIXES libs libs/${CMAKE_BUILD_TYPE}
		DOC "pisk libraries directory")
	list (APPEND PISK_LIBRARIES ${MODULE_LIBRARY_DIR})
endforeach()

list(GET PISK_LIBRARIES 0 PISK_LIBRARIES_0)
get_filename_component(PISK_LIBRARIES_DIR ${PISK_LIBRARIES_0} DIRECTORY)

FIND_PACKAGE_HANDLE_STANDARD_ARGS (pisk REQUIRED_VARS PISK_INCLUDE_DIRS PISK_LIBRARIES_DIR)

mark_as_advanced (PISK_ROOT_DIR PISK_PLATFORM_DIR PISK_INCLUDE_DIRS PISK_LIBRARIES_DIR PISK_LIBRARIES)

