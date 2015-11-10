set(CMAKE_MFC_FLAG 1)
set(MSVC_RUNTIME "static")
file(GLOB OUTMON_CPP_FILES *.cpp)

# files are only copied if a target depends on them
add_executable(outmon ${OUTMON_CPP_FILES})
target_include_directories(outmon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/../outlib)
target_link_libraries(outmon outlib)
ChangeStaticRuntimeLib()
