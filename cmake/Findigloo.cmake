cmake_minimum_required(VERSION 2.8)

include (FindPackageHandleStandardArgs)
include (CheckLibraryExists)

find_path (IGLOO_ROOT_DIR
	NAMES igloo/igloo.h
	PATHS ENV IGLOO_ROOT_DIR
	DOC "igloo root dir")

find_path (IGLOO_INCLUDE_DIR
	NAMES igloo/igloo.h
	HINTS ${IGLOO_ROOT_DIR}
	PATH_SUFFIXES include
	DOC "igloo include directory")

mark_as_advanced (IGLOO_INCLUDE_DIR)

FIND_PACKAGE_HANDLE_STANDARD_ARGS (igloo REQUIRED_VARS IGLOO_INCLUDE_DIR)

