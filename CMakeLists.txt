cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT DEFINED PROJECT_OUTPUT_DIR)
	set(PROJECT_OUTPUT_DIR "${PROJECT_SOURCE_DIR}/.output/${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_OUTPUT_DIR}/libs")
set(LIBRARY_OUTPUT_PATH "${PROJECT_OUTPUT_DIR}/libs")

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/")

find_package(Threads REQUIRED)


if (NOT SUBSYSTEM)
	message(FATAL_ERROR "Please, specify variable SUBSYSTEM: set gui or cui, etc.")
endif()


set(OS_SPECIFIC_INCLUDE_DIRS "")
set(OS_SPECIFIC_LIBRARIES "")

if(ANDROID)
	list(APPEND OS_SPECIFIC_INCLUDE_DIRS "${EXTERNALS_DIR}/android_helps")
	list(APPEND OS_SPECIFIC_LIBRARIES "log")
	list(APPEND OS_SPECIFIC_LIBRARIES "android")
endif()


if(ANDROID)
	set(OSDIR "android")
elseif(WIN32)
	set(OSDIR "windows")
elseif(UNIX)
	set(OSDIR "unix")
else()
	message(FATAL_ERROR "Unsuported OS!")
endif()


if(MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
elseif(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")
	if(ANDROID)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable")
	endif()
endif()


macro(FILES VARNAME FILTER DIRS)
	foreach(DIR ${${DIRS}})
		file(GLOB RESULT LIST_DIRECTORIES true "${DIR}/${FILTER}")
		list(APPEND ${VARNAME} ${RESULT})
	endforeach()
endmacro()

macro(SUBPROJECTS VARNAME IN_DIR)
	file(GLOB SUBDIRECTORIES LIST_DIRECTORIES true RELATIVE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_CURRENT_LIST_DIR}/${IN_DIR}/*)
	foreach(DIR ${SUBDIRECTORIES})
		if (NOT IS_SYMLINK "${CMAKE_CURRENT_LIST_DIR}/${DIR}/CMakeLists.txt")
		if (NOT IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${DIR}/CMakeLists.txt")
		if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/${DIR}/CMakeLists.txt")
			list(APPEND ${VARNAME} ${DIR})
		endif ()
		endif ()
		endif ()
	endforeach()
endmacro()



set(PISK_INCLUDE_DIRS "")
set(PISK_LIBRARY_DIRS "")
set(PISK_LIBRARIES "base")


set(DIRS "base" "system" "os" "model")
SUBPROJECTS(DIRS "modules/services")
SUBPROJECTS(DIRS "modules/engines")
SUBPROJECTS(DIRS "modules/loaders")
list(APPEND DIRS "pisk")

foreach(DIR ${DIRS})
	get_filename_component(_INCLUDE "./${DIR}/include" ABSOLUTE)
	list(APPEND PISK_INCLUDE_DIRS ${_INCLUDE})
endforeach()

foreach(DIR ${DIRS})
	add_subdirectory(${DIR})
endforeach()

file(REMOVE_RECURSE "${PROJECT_OUTPUT_DIR}/include")
foreach(DIR ${DIRS})
	if (IS_DIRECTORY "${PROJECT_SOURCE_DIR}/${DIR}/include")
		message(STATUS "copy headers from '${DIR}' to the output directory")
		file(COPY "${DIR}/include" DESTINATION "${PROJECT_OUTPUT_DIR}/")
	endif()

	if (IS_DIRECTORY "${PROJECT_SOURCE_DIR}/${DIR}/${OSDIR}")
		message(STATUS "copy os-specific files from '${DIR}/${OSDIR}' to the output directory")
		file(REMOVE_RECURSE "${PROJECT_OUTPUT_DIR}/${OSDIR}")
		file(COPY "${DIR}/${OSDIR}" DESTINATION "${PROJECT_OUTPUT_DIR}/")
	endif()
endforeach()

