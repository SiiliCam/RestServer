# Add source to this project's executable.
add_executable (RestExample RestExample.cpp)

target_link_libraries(RestExample PUBLIC RestServer Boost::json Logger)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RestExample PROPERTY CXX_STANDARD 20)
endif()

add_dependencies(RestExample RestServer Logger)