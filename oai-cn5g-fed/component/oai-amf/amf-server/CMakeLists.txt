cmake_minimum_required(VERSION 3.9)
project(amf-server)# Set our project to use C++ 17
set(CMAKE_CXX_STANDARD 17)# Set our source files as just the main.cpp

set(SOURCE_FILES amf_server.cc)
find_package(nlohmann_json REQUIRED)
find_package(PkgConfig)

pkg_check_modules(Pistache REQUIRED IMPORTED_TARGET libpistache)# Create our executable file from our source files
add_executable(amf_server ${SOURCE_FILES})
target_link_libraries(amf_server PkgConfig::Pistache nlohmann_json::nlohmann_json)