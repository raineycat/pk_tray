cmake_minimum_required(VERSION 3.20)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_MSVC_RUNTIME_LIBRARY MultiThreaded$<$<CONFIG:Debug>:Debug>)

add_custom_target(ALL COMMAND "echo MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(pktray
	DESCRIPTION "Interact with thr PluralKit API from the windows system tray"
	VERSION "0.1.0.0"
	LANGUAGES CXX)

add_executable(pk_tray WIN32
	"pk_tray/main.cpp"
	"pk_tray/requests.cpp" "pk_tray/requests.hpp"
	"pk_tray/api.cpp" "pk_tray/api.hpp"
	"pk_tray/utils.hpp"
	"pk_tray/models.hpp" 
	"pk_tray/tray.cpp" "pk_tray/tray.hpp"
	"pk_tray/resources/pk_tray.rc"
	"pk_tray/resources/resource.h"
	"pk_tray/resources/pk_tray.manifest")

target_include_directories(pk_tray PRIVATE
	"third_party")

target_link_libraries(pk_tray PRIVATE
	winhttp.lib)