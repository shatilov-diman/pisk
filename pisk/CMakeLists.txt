cmake_minimum_required(VERSION 2.8)

set(BASE_NAME pisk)

find_package(igloo REQUIRED)

include_directories(${IGLOO_INCLUDE_DIR} ${PISK_INCLUDE_DIRS})

set(AUTOSRC_DIRS "tests" "tests/acceptance")
FILES(MY_TESTS "*.cpp" AUTOSRC_DIRS)

file(GLOB_RECURSE MY_INCLUDES LIST_DIRECTORIES true "include/*.h")

if(APPLE)
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
		set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -pagezero_size 0x10000 -image_base 0x6000000")
	endif()
endif()


set(MY_TEST_NAME test_${BASE_NAME})
project(${MY_TEST_NAME})

add_executable(${MY_TEST_NAME} ${MY_SOURCES} ${MY_HEADERS} ${MY_INCLUDES} ${MY_TESTS})
target_link_libraries(${MY_TEST_NAME} ${OS_SPECIFIC_LIBRARIES} ${PISK_LIBRARIES} "os")
add_dependencies(${MY_TEST_NAME} ${PISK_LIBRARIES} "system" "os" "http" "lua" "script" "geolocation")


set(MY_PROJ_NAME ${BASE_NAME})
project(${MY_PROJ_NAME})

if (NOT ANDROID)
	add_executable(${MY_PROJ_NAME} "sources/main.cpp" ${MY_SOURCES} ${MY_HEADERS} ${MY_INCLUDES})
	target_link_libraries(${MY_PROJ_NAME} ${OS_SPECIFIC_LIBRARIES} ${PISK_LIBRARIES} "os")

	add_dependencies(${MY_PROJ_NAME} ${PISK_LIBRARIES} "os")

	if (DONT_RUN_TESTS)
		add_dependencies(${MY_PROJ_NAME} ${MY_TEST_NAME})
	else()
		add_custom_target(${MY_TEST_NAME}_run COMMAND ${MY_TEST_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
			COMMENT "Run ${MY_TEST_NAME}")
		add_dependencies(${MY_TEST_NAME}_run ${MY_TEST_NAME})
		add_dependencies(${MY_PROJ_NAME} ${MY_TEST_NAME}_run)
	endif()
endif()

