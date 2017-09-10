cmake_minimum_required(VERSION 3.0)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/modules/")

project(dxplorer)
aux_source_directory(src/common SRC_common)
aux_source_directory(src/ui SRC_ui)
include_directories(src)

add_library(dxplorer_common ${SRC_common})
add_library(dxplorer_ui ${SRC_ui})

if((${CMAKE_SYSTEM_NAME} MATCHES "Windows"))
	set(SRC_rc res/wsprlite.rc)
	set(CMAKE_RC_COMPILER_INIT windres)
	ENABLE_LANGUAGE(RC)
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff <DEFINES> <FLAGS> <SOURCE> <OBJECT>")
else()
	set(SRC_rc "")
endif()

add_executable(WSPRliteConfig src/wsprlite-config.cpp ${SRC_rc})
target_link_libraries(dxplorer_ui dxplorer_common)
target_link_libraries(WSPRliteConfig dxplorer_common dxplorer_ui)


if((${CMAKE_SYSTEM_NAME} MATCHES "Windows"))
	include_directories(${CMAKE_SYSROOT}/include)
endif()

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if((${CMAKE_SYSTEM_NAME} MATCHES "Windows"))
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -static-libstdc++ -static-libgcc")
	endif()
endif()


find_package(wxWidgets COMPONENTS core adv base REQUIRED)
include(${wxWidgets_USE_FILE})
target_link_libraries(dxplorer_ui ${wxWidgets_LIBRARIES})

if((${CMAKE_SYSTEM_NAME} MATCHES "Windows"))
    string(REPLACE ";" " -I " wxWidgets_RC_INC "${wxWidgets_INCLUDE_DIRS}")
    set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} -I ${wxWidgets_RC_INC}")
endif()

find_package(libserialport REQUIRED)
target_link_libraries(dxplorer_common ${libserialport_LIBRARY})
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	target_link_libraries(dxplorer_common cfgmgr32 setupapi)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	if(CMAKE_COMPILER_IS_GNUCXX)
	  set(CMAKE_CXX_LINK_EXECUTABLE "${CMAKE_CXX_LINK_EXECUTABLE} -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic")
	endif()
endif()


target_include_directories(dxplorer_common PUBLIC lib/cppcodec/cppcodec)


