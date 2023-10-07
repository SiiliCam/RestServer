# CMakeList.txt : CMake project for RestServer, include source and define
# project specific logic here.
#

# Add source to this project's executable.
# Specify the required source files
set(SOURCES "src/RestServer.cpp" "src/Session.cpp" "src/RestSender.cpp")

# Create the RestServer library
add_library(RestServer ${SOURCES})

# Link Boost libraries to RestServer
target_link_libraries(RestServer PUBLIC Boost::system Boost::json ${CMAKE_THREAD_LIBS_INIT} Logger)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(RestServer PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/ 
     DESTINATION ${CMAKE_BINARY_DIR}/include/restserver)


add_dependencies(RestServer Logger)


if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RestServer PROPERTY CXX_STANDARD 20)
endif()


# TODO: Add tests and install targets if needed.
