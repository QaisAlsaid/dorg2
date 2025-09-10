cmake_minimum_required(VERSION 3.10)
project(dorg2 LANGUAGES CXX)

option(EMSCRIPTEN "build web" OFF)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

#if(not ${TARGET_RELEASE})
  set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)
  add_compile_options(-o4 -fPIC)
#endif()


add_subdirectory(vendor/raylib)

file(GLOB_RECURSE DORG2_SRC CONFIGURE_DEPENDS src/*.cpp)
file(GLOB_RECURSE DORG2_HDR CONFIGURE_DEPENDS src/*.hpp)

add_executable(${PROJECT_NAME} ${DORG2_SRC} ${DORG2_HDR})
target_link_libraries(${PROJECT_NAME} raylib)
target_include_directories(${PROJECT_NAME} PRIVATE vendor/raylib/src src/)
if(${EMSCRIPTEN})
  set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html"
    LINK_FLAGS "-s ASSERTIONS=1 -sWASM=0 -s GL_ENABLE_GET_PROC_ADDRESS=1 -s INITIAL_MEMORY=268435456 --embed-file ../assets --shell-file ../template.html -s ASYNCIFY")
    #s ALLOW_MEMORY_GROWTH=1 
    #-sUSE_GLFW=3 
    #--shell-file ../template.html
    # --bind 
    #-sFULL_ES3
    #-s ASYNCIFY
endif()
