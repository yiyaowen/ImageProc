cmake_minimum_required(VERSION 3.5)

project(ImageProcessor LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DD14LIB_PATH=L"${PROJECT_SOURCE_DIR}/d14uikit/lib/")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:D14UIKit.dll")

link_directories(d14uikit/lib)
include_directories(d14uikit/include)

file(GLOB_RECURSE SRC_LIST "src/*.c" "src/*.cpp")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_executable(ImageProcessor ${SRC_LIST})
target_link_libraries(ImageProcessor D14UIKit Comdlg32.lib)
