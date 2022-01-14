find_package(Boost 1.40.0 REQUIRED COMPONENTS system thread)
include_directories(${Boost_INCLUDE_DIR})
message("boost include : ${Boost_INCLUDE_DIR}")
message("Link libraries : ${Boost_LIBRARIES}")


include_directories( ${PROJECT_SOURCE_DIR}/include/ )
aux_source_directory(. DIR_LIB_SRCS)
add_library( btr STATIC ${DIR_LIB_SRCS} )
target_link_libraries(btr PUBLIC ${Boost_LIBRARIES})

