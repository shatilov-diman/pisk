cmake_minimum_required(VERSION 2.8)

set(BASE_NAME base)

find_package(JsonCpp REQUIRED)
find_package(igloo REQUIRED)

include_directories(${JSONCPP_INCLUDE_DIR} ${IGLOO_INCLUDE_DIR} ${PISK_INCLUDE_DIRS})

set(AUTOSRC_DIRS "sources/core" "sources/infrastructure" "sources/tools" "sources/utils")
FILES(MY_HEADERS "*.h" AUTOSRC_DIRS)
FILES(MY_SOURCES "*.cpp" AUTOSRC_DIRS)

set(AUTOSRC_DIRS "tests" "tests/core" "tests/infrastructure" "tests/tools" "tests/utils")
FILES(MY_TESTS "*.cpp" AUTOSRC_DIRS)

file(GLOB_RECURSE MY_INCLUDES LIST_DIRECTORIES true "include/*.h")

if (UNIX)
	list(APPEND OS_SPECIFIC_LIBRARIES "dl")
	list(APPEND OS_SPECIFIC_LIBRARIES ${CMAKE_THREAD_LIBS_INIT})
endif()


set(MY_TEST_NAME test_${BASE_NAME})
project(${MY_TEST_NAME})

add_executable(${MY_TEST_NAME} ${MY_SOURCES} ${MY_HEADERS} ${MY_INCLUDES} ${MY_TESTS})
target_link_libraries(${MY_TEST_NAME} ${OS_SPECIFIC_LIBRARIES} ${JSONCPP_LIBRARIES})


set(MY_PROJ_NAME ${BASE_NAME})
project(${MY_PROJ_NAME})

add_library(${MY_PROJ_NAME} SHARED ${MY_SOURCES} ${MY_HEADERS} ${MY_INCLUDES})
target_link_libraries(${MY_PROJ_NAME} ${OS_SPECIFIC_LIBRARIES} ${JSONCPP_LIBRARIES})


if (DONT_RUN_TESTS)
	add_dependencies(${MY_PROJ_NAME} ${MY_TEST_NAME})
else()
	add_custom_target(${MY_TEST_NAME}_run COMMAND ${MY_TEST_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
		COMMENT "Run ${MY_TEST_NAME}")
	add_dependencies(${MY_TEST_NAME}_run ${MY_TEST_NAME})
	add_dependencies(${MY_PROJ_NAME} ${MY_TEST_NAME}_run)
endif()

