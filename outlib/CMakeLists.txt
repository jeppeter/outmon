set(CMAKE_BUILD_TYPE Release) 
set(CMAKE_CONFIGURATION_TYPES "Debug;Release")
file(GLOB OUTLIB_CPP_FILES "*.cpp")
file(GLOB OUTLIB_C_FILES "*.c")
add_library(outlib STATIC ${OUTLIB_C_FILES} ${OUTLIB_CPP_FILES})
target_include_directories(outlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
ChangeStaticRuntimeLib()
