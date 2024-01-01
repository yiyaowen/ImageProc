cmake_minimum_required(VERSION 3.5)

project(ImageProcessor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DD14LIB_PATH=L"${PROJECT_SOURCE_DIR}/d14uikit/lib/")

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /DELAYLOAD:D14UIKit.dll")

link_directories(d14uikit/lib)
include_directories(d14uikit/include)

aux_source_directory(src SRC_LIST)
add_executable(ImageProcessor ${SRC_LIST})
target_link_libraries(ImageProcessor D14UIKit)
