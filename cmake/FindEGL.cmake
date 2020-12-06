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
#  EGL_FOUND		- system has EGL
#  EGL_INCLUDE_DIR  - the EGL include directory
#  EGL_LIBRARIES	- Link these to use EGL

find_path(
	EGL_INCLUDE_DIR
	NAMES
		EGL/egl.h
	PATH_SUFFIXES
		OpenGLES
	HINTS
		"${EGL_ROOT_DIR}/include"
		"$ENV{EGL_ROOT_DIR}/include"
		"${ANDROID_STANDALONE_TOOLCHAIN}/usr/include"
)

find_library(
	EGL_LIBRARIES
	NAMES EGL libEGL
	PATH_SUFFIXES
		x86 x86-64
	HINTS
		"${EGL_ROOT_DIR}"
		"$ENV{EGL_ROOT_DIR}"
)

if(IOS)
	find_library( EGL_FRAMEWORKS EGL )

	if(EGL_FRAMEWORKS)
		set( EGL_LIBRARIES "-framework EGL" )
	endif()

endif()

set(EGL_FOUND "NO")
if(EGL_LIBRARIES)
	set(EGL_FOUND "YES")
endif(EGL_LIBRARIES)

mark_as_advanced(
  EGL_INCLUDE_DIR
  EGL_LIBRARIES
)

