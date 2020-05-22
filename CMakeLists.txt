cmake_minimum_required(VERSION 3.5)
project(face_view)

set(CMAKE_CXX_STANDARD 11)

#SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

include_directories(/usr/include/libxml2)
#link_directories(${PROJECT_BINARY_DIR}/lib)

include_directories(/usr/include/SDL2)

add_subdirectory(gsoap)
add_subdirectory(res)

set(EXTRA_LIBS)
set(EXTRA_LIBS ${EXTRA_LIBS} crypto ssl z pthread xml2 gsoap)
set(EXTRA_LIBS ${EXTRA_LIBS} SDL2 SDL2_image SDL2_ttf SDL2_mixer)

#find_package(SDL2 REQUIRED)
#if (SDL2_FOUND)
#    message(STATUS ${SDL2_LIBRARYS})
#    include_directories(${SDL2_INCLUDE_DIRS})
#    set(EXTRA_LIBS ${EXTRA_LIBS} ${SDL2_LIBRARYS})
#endif (SDL2_FOUND)

#configure_file(configure.xml configure.xml)
file(COPY configure.xml DESTINATION .)

aux_source_directory(. DIR_SRCS)

add_executable(face_view ${DIR_SRCS})

target_link_libraries(face_view ${EXTRA_LIBS})



