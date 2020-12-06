# Project pisk
# Copyright (C) 2016-2017 Dmitry Shatilov
#
# Original sources:
#   https://github.com/shatilov-diman/pisk/
#   https://bitbucket.org/charivariltd/pisk/
#
# Author contacts:
#   Dmitry Shatilov (e-mail: shatilov.diman@gmail.com; site: https://www.linkedin.com/in/shatilov)
#
#  OPENGLES_FOUND		- system has OpenGLES
#  OPENGLES_INCLUDE_DIR  - the GL include directory
#  OPENGLES_LIBRARIES	- Link these to use OpenGLES

find_path(
	OPENGLES_INCLUDE_DIR
	NAMES
		GLES2/gl2.h
		ES2/gl.h
	PATH_SUFFIXES
		OpenGLES
	HINTS
		"${GLES2_ROOT_DIR}/include"
		"$ENV{GLES2_ROOT_DIR}/include"
		"${ANDROID_STANDALONE_TOOLCHAIN}/usr/include"
)

find_library(
	OPENGLES_LIBRARIES
	NAMES GLESv2 OpenGLES libGLESv2 libOpenGLES
	PATH_SUFFIXES
		x86 x86-64
	HINTS
		"${GLES2_ROOT_DIR}"
		"$ENV{GLES2_ROOT_DIR}"
		"${ANDROID_STANDALONE_TOOLCHAIN}/usr/lib"
)

if(IOS)
    find_library(OPENGLES_FRAMEWORKS OpenGLES)

    if(OPENGLES_FRAMEWORKS)
        set( OPENGLES_LIBRARIES "-framework OpenGLES" )
    endif()

endif()

set(OPENGLES_FOUND "NO")
if(OPENGLES_LIBRARIES)
	set(OPENGLES_FOUND "YES")
endif(OPENGLES_LIBRARIES)

mark_as_advanced(
  OPENGLES_INCLUDE_DIR
  OPENGLES_LIBRARIES
)

