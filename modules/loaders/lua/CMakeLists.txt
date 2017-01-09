cmake_minimum_required(VERSION 2.8)

set(BASE_NAME lua)
set(BASE_DIRNAME lua)

find_package(igloo REQUIRED)
find_package(LuaJIT REQUIRED)

include_directories(${IGLOO_INCLUDE_DIR} ${LUA_INCLUDE_DIR} ${LUA_BRIDGE_INCLUDE_DIRS} ${PISK_INCLUDE_DIRS})

set(AUTOSRC_DIRS "sources/${BASE_DIRNAME}")
FILES(MY_HEADERS "*.h" AUTOSRC_DIRS)
FILES(MY_SOURCES "*.cpp" AUTOSRC_DIRS)

set(AUTOSRC_DIRS "tests" "tests/${BASE_DIRNAME}")
FILES(MY_TESTS "*.cpp" AUTOSRC_DIRS)

file(GLOB_RECURSE MY_INCLUDES LIST_DIRECTORIES true "include/*.h")


set(MY_TEST_NAME test_${BASE_NAME})
project(${MY_TEST_NAME})

add_executable(${MY_TEST_NAME} ${MY_SOURCES} ${MY_HEADERS} ${MY_INCLUDES} ${MY_TESTS})
target_link_libraries(${MY_TEST_NAME} ${LUA_LIBRARIES} ${OS_SPECIFIC_LIBRARIES} ${PISK_LIBRARIES})
add_dependencies(${MY_TEST_NAME} ${PISK_LIBRARIES})

set(MY_PROJ_NAME ${BASE_NAME})
project(${MY_PROJ_NAME})

add_library(${MY_PROJ_NAME} SHARED ${MY_SOURCES} ${MY_HEADERS} ${MY_INCLUDES})
target_link_libraries(${MY_PROJ_NAME} ${LUA_LIBRARIES} ${OS_SPECIFIC_LIBRARIES} ${PISK_LIBRARIES})
add_dependencies(${MY_PROJ_NAME} ${PISK_LIBRARIES})

if (DONT_RUN_TESTS)
	add_dependencies(${MY_PROJ_NAME} ${MY_TEST_NAME})
else()
	add_custom_target(${MY_TEST_NAME}_run COMMAND ${MY_TEST_NAME} WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
		COMMENT "Run ${MY_TEST_NAME}")
	add_dependencies(${MY_TEST_NAME}_run ${MY_TEST_NAME})
	add_dependencies(${MY_PROJ_NAME} ${MY_TEST_NAME}_run)
endif()

