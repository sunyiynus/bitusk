Include(FetchContent)

FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.0.1 # or a later release
)

FetchContent_MakeAvailable(Catch2)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
include_directories(${PROJECT_SOURCE_DIR}/include/)
include_directories(${PROJECT_SOURCE_DIR}/src/common)
include_directories(${PROJECT_SOURCE_DIR}/extern/catch)
#set(CATCH_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/extern/Catch2)
aux_source_directory(./test_mm/ DIR_LIB_SRCS)
#add_executable(tests datacache_test.cpp)
add_executable(tests ${DIR_LIB_SRCS})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_link_libraries(tests PRIVATE btr)
