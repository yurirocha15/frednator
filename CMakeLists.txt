cmake_minimum_required(VERSION 2.8)
project(frednator)

#########################################################
# FINDING QIBUILD
#########################################################
find_package(qibuild)

#########################################################
# INCLUDE THE QIBUILD'S QT MODULES
#########################################################
include(qibuild/modules/qt-tools)
include(qibuild/modules/qt_qtcore-config)
include(qibuild/modules/qt_qtgui-config)

#########################################################
# SOURCES
#########################################################
set(_srcs
  mainwindow.cpp
  mainwindow.h
  videoThread.cpp
  videoThread.hpp
  controller.cpp
  controller.hpp
  main.cpp
)

#########################################################
# SETUP QT
#########################################################

# Qt headers that need be processed by 'uic' to generate 'moc' files
set(q_HEADERS mainwindow.h controller.hpp videoThread.hpp)

# Qt user interface files to be processed
set(q_UI ${CMAKE_CURRENT_SOURCE_DIR}/mainwindow.ui)

# Qt resources files
set(q_RCCS resources.qrc)

# Include files generated to be included in the compilation
#INCLUDE(${QT_USE_FILE})
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

# Wrap the files to generate all neccesary 'moc' and 'ui' files
QT4_ADD_RESOURCES(q_HEADERS_RCCS ${q_RCCS})
QT4_WRAP_CPP(q_HEADERS_MOC ${q_HEADERS})
QT4_WRAP_UI(q_HEADERS_UI ${q_UI})

#########################################################
# RUN BASH SCRIPT
#########################################################

#Make the script possible to be executed
FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/copyIcon.sh
"mv ${PROJECT_NAME}.desktop ~/.local/share/applications/
chmod +x ~/.local/share/applications/${PROJECT_NAME}.desktop")

execute_process(COMMAND chmod +x copyIcon.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

FILE(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_NAME}.desktop
"[Desktop Entry]
Name=${PROJECT_NAME}
GenericName=${PROJECT_NAME}QT
Comment=QT Simulator for NAO robot.
Type=Application
Categories=Qt;NaoRobot;Simulation;VideoRecording;
Terminal=false
Icon=${CMAKE_CURRENT_SOURCE_DIR}/unbeatablesicon.png
Exec=${CMAKE_CURRENT_BINARY_DIR}/sdk/bin/${PROJECT_NAME}")

execute_process(COMMAND ./copyIcon.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

execute_process(COMMAND rm copyIcon.sh WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})




#########################################################
# OTHER THINGS
#########################################################
#Disable BOOST Useless warning
add_definitions(-DBOOST_SIGNALS_NO_DEPRECATION_WARNING)
#add_definitions(-DCMAKE_TOOLCHAIN_FILE=/home/yuri/nao/devtools/naoqi-sdk-2.1.3.3-linux64/toolchain-pc.cmake)


#########################################################
# CREATE BINARY FILE AND ADD LIBRARIES
#########################################################
# Create a executable named video_recorder
qi_create_bin(${PROJECT_NAME} ${_srcs} ${q_HEADERS_MOC} ${q_HEADERS_UI} ${q_HEADERS_RCCS})
qi_use_lib(${PROJECT_NAME} ALCOMMON ALPROXIES BOOST ALVISION OPENCV2_CORE OPENCV2_HIGHGUI OPENCV2_IMGPROC QT_QTCORE QT_QTGUI)

