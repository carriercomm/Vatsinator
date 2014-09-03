# 
#   Copyright (C) 2012-2014  Michał Garapich michal@garapich.pl
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

project (vatsinator)

cmake_minimum_required (VERSION 2.8.11 FATAL_ERROR)
cmake_policy (SET CMP0020 OLD)

if (POLICY CMP0028)
  cmake_policy (SET CMP0028 NEW)
endif (POLICY CMP0028)

if (UNIX AND NOT APPLE AND NOT MINGW AND NOT ANDROID)
    set (LINUX 1)
endif (UNIX AND NOT APPLE AND NOT MINGW AND NOT ANDROID)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules)

find_package (Qt5Core REQUIRED)
find_package (Qt5Xml REQUIRED)
find_package (Qt5Network REQUIRED)

include_directories(${Qt5Core_INCLUDE_DIRS})
include_directories(${Qt5Network_INCLUDE_DIRS})
include_directories(${Qt5Xml_INCLUDE_DIRS})

if (ANDROID)
  find_package(Qt5Quick REQUIRED)
  include_directories(${Qt5Quick_INCLUDE_DIRS})
else ()
  find_package (Qt5Widgets REQUIRED)  
  find_package (Qt5OpenGL REQUIRED) # TODO Qt 5.4: remove (QtOpenGLWidget)
  find_package (OpenGL REQUIRED)
  include_directories(${Qt5Widgets_INCLUDE_DIRS})  
  include_directories(${Qt5OpenGL_INCLUDE_DIRS})
endif ()

find_package (QuaZip REQUIRED)

get_target_property (QT_QMAKE_EXECUTABLE Qt5::qmake IMPORTED_LOCATION)
    
if (Qt5_POSITION_INDEPENDENT_CODE)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

exec_program (${QT_QMAKE_EXECUTABLE} ARGS "-query QT_VERSION"
      OUTPUT_VARIABLE QT_VERSION)

# set CMAKE_BUILD_TYPE
if (CMAKE_BUILD_TYPE MATCHES Debug)
    set (CMAKE_BUILD_TYPE Debug)
else (CMAKE_BUILD_TYPE MATCHES Debug)
    set (CMAKE_BUILD_TYPE Release)
endif (CMAKE_BUILD_TYPE MATCHES Debug)

# set CXX_FLAGS
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set (CMAKE_CXX_FLAGS_RELEASE "-DQT_NO_DEBUG -DQT_NO_DEBUG_OUTPUT")
set (CMAKE_CXX_FLAGS_DEBUG "-g -Wall")

# find version
if (NOT vatsinator_VERSION)
    file (READ ${CMAKE_CURRENT_SOURCE_DIR}/VERSION vatsinator_VERSION)
    string (REPLACE "\n" "" vatsinator_VERSION ${vatsinator_VERSION})
    
    include(GetGitRevisionDescription)
    get_git_head_revision(GIT_REFSPEC GIT_SHA1)
    
    if (NOT "${GIT_SHA1}" STREQUAL "GITDIR-NOTFOUND")
        # shorten the sha
        string (SUBSTRING "${GIT_SHA1}" 0 10 GIT_HASH)
        set (vatsinator_VERSION ${vatsinator_VERSION}-${GIT_HASH}-git)
    endif (NOT "${GIT_SHA1}" STREQUAL "GITDIR-NOTFOUND")
endif (NOT vatsinator_VERSION)

# specify user interface implementation
if (NOT vatsinator_UI_IMPLEMENTATION)
  if (WIN32 OR LINUX OR APPLE) # use QtWidgets for desktop
    set (vatsinator_UI_IMPLEMENTATION "widgets")
  elseif (ANDROID) # use QtQuick for mobile
    set (vatsinator_UI_IMPLEMENTATION "quick")
  endif (WIN32 OR LINUX OR APPLE)
endif (NOT vatsinator_UI_IMPLEMENTATION)

if (ANDROID)
    set (vatsinator_ANDROID_APK ${CMAKE_BINARY_DIR}/vatsinator.apk)
    set (CMAKE_INSTALL_PREFIX ${vatsinator_ANDROID_APK})
endif (ANDROID)

# exec file
set (vatsinator_TARGET
    vatsinator
)

if (APPLE) # bundle looks better
  set (vatsinator_TARGET
      Vatsinator
  )
endif (APPLE)

include (VatsinatorInstall)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty)

if (NOT APPLE)
    set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
endif (NOT APPLE)
add_subdirectory (source)

add_subdirectory (resources)
add_subdirectory (dist)

# Platform-specific actions
if (LINUX)
    
    # This actually creates .deb
    include (Packaging-linux)
    
    # This sets up the "debian" directory
    option (WITH_DEBIAN "Setup \"debian\" subdirectory" OFF)
    if (WITH_DEBIAN AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/debian")
        add_subdirectory (debian)
    endif (WITH_DEBIAN AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/debian")

    # Spec file
    option (WITH_RPM "Create RPM package" OFF)

elseif (APPLE)
    
    # For bundle creation
    include (Packaging-darwin)

# builds for win32
elseif (WIN32)
    
    # TODO merge misc/msvc-2012
    
    # Awesome NSIS packaging
    include (Packaging-win32)

elseif (ANDROID)

    get_target_property(QtCore_location Qt5::Core LOCATION)
    get_target_property(QtGui_location Qt5::Gui LOCATION)
    get_target_property(QtNetwork_location Qt5::Network LOCATION)
    get_target_property(QtXml_location Qt5::Xml LOCATION)
    get_target_property(QtQuick_location Qt5::Quick LOCATION)
    get_target_property(QtQml_location Qt5::Qml LOCATION)
    get_target_property(qjson_location qjson LOCATION)

    install (FILES
            ${QtCore_location}
            ${QtGui_location}
            ${QtNetwork_location}
            ${QtXml_location}
            ${QtQuick_location}
            ${QtQml_location}
            ${QUAZIP_LIBRARIES}
            ${qjson_location}
            DESTINATION libs/${ANDROID_ABI}
        )
        
    
endif (LINUX)

set (vatsinator_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/source)
option (WITH_PLUGINS "Build plugins" ON)
if (WITH_PLUGINS)
    add_subdirectory(plugins)
endif (WITH_PLUGINS)

# Sum up our configuration
message (STATUS "Vatsinator-${vatsinator_VERSION} will be built with the following options:")
message (STATUS " * install prefix: ${CMAKE_INSTALL_PREFIX}")
message (STATUS " * build type: ${CMAKE_BUILD_TYPE}")
if (LINUX)
    message (STATUS " * platform: Linux")
elseif (APPLE)
    message (STATUS " * platform: Apple")
elseif (WIN32)
    message (STATUS " * platform: Win32")
elseif (ANDROID)
    message (STATUS " * platform: Android (${ANDROID_ABI})")
else (LINUX)
    message (STATUS " * platform: Undefined")
endif (LINUX)
message (STATUS " * Qt version: ${QT_VERSION}")

# uninstall target
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY
)

add_custom_target (
    uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)
