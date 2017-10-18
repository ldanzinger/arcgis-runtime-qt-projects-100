#-------------------------------------------------
#  Copyright 2016 ESRI
#
#  All rights reserved under the copyright laws of the United States
#  and applicable international laws, treaties, and conventions.
#
#  You may freely redistribute and use this sample code, with or
#  without modification, provided you include the original copyright
#  notice and use restrictions.
#
#  See the Sample code usage restrictions document for further information.
#-------------------------------------------------

TARGET = OverrideMapSignals 
TEMPLATE = app

QT += positioning core gui opengl network widgets sensors

CONFIG += c++11

ARCGIS_RUNTIME_VERSION = 100.1
include($$PWD/arcgisruntime.pri)

win32:CONFIG += \
    embed_manifest_exe

SOURCES += \
    main.cpp \
    OverrideMapSignals.cpp 

HEADERS += \
    OverrideMapSignals.h

#-------------------------------------------------------------------------------

macx {
    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.11
}
