TARGET = platyvg
ios|macx: TEMPLATE = lib
CONFIG += lib
QT -= core
QT -= gui

unix{
    target.path = /usr/lib
    INSTALLS += target
}

CONFIG += c++11
CONFIG += mobility
CONFIG += warn_off
MOBILITY += systeminfo

DEFINES += PX_BUILD_API=PlatyVG
DEFINES += PX_BUILD_API_NAME=\\\"PlatyVG\\\"
INCLUDEPATH += ../../PlatyVG/source/api

###########################################################
# platyvg
###########################################################
SOURCES += ../../PlatyVG/source/api/adapter.cpp
SOURCES += ../../PlatyVG/source/api/command.cpp
SOURCES += ../../PlatyVG/source/api/export.cpp
SOURCES += ../../PlatyVG/source/api/input.cpp
#SOURCES += ../../PlatyVG/source/api/listener.cpp
#SOURCES += ../../PlatyVG/source/api/platyvg.cpp
SOURCES += ../../PlatyVG/source/core/device.cpp
SOURCES += ../../PlatyVG/source/core/document.cpp
SOURCES += ../../PlatyVG/source/core/layer.cpp
SOURCES += ../../PlatyVG/source/core/record.cpp
SOURCES += ../../PlatyVG/source/core/shape.cpp
SOURCES += ../../PlatyVG/source/core/view.cpp
SOURCES += ../../PlatyVG/source/element/action.cpp
SOURCES += ../../PlatyVG/source/element/color.cpp
SOURCES += ../../PlatyVG/source/element/context.cpp
SOURCES += ../../PlatyVG/source/element/coord.cpp
SOURCES += ../../PlatyVG/source/element/edge.cpp
SOURCES += ../../PlatyVG/source/element/matrix.cpp
SOURCES += ../../PlatyVG/source/element/mesh.cpp
SOURCES += ../../PlatyVG/source/element/point.cpp
SOURCES += ../../PlatyVG/source/element/rect.cpp
SOURCES += ../../PlatyVG/source/element/selector.cpp
SOURCES += ../../PlatyVG/source/element/size.cpp
SOURCES += ../../PlatyVG/source/element/table.cpp
SOURCES += ../../PlatyVG/source/element/zoom.cpp
SOURCES += ../../PlatyVG/source/px/array.cpp
SOURCES += ../../PlatyVG/source/px/buffer.cpp
SOURCES += ../../PlatyVG/source/px/file.cpp
SOURCES += ../../PlatyVG/source/px/math.cpp
SOURCES += ../../PlatyVG/source/px/memory.cpp
SOURCES += ../../PlatyVG/source/px/parser.cpp
SOURCES += ../../PlatyVG/source/px/px.cpp
SOURCES += ../../PlatyVG/source/px/share.cpp
SOURCES += ../../PlatyVG/source/px/string.cpp
HEADERS += ../../PlatyVG/source/api/adapter.hpp
HEADERS += ../../PlatyVG/source/api/command.hpp
HEADERS += ../../PlatyVG/source/api/export.hpp
HEADERS += ../../PlatyVG/source/api/input.hpp
HEADERS += ../../PlatyVG/source/api/listener.hpp
HEADERS += ../../PlatyVG/source/api/platyvg.hpp
HEADERS += ../../PlatyVG/source/core/device.hpp
HEADERS += ../../PlatyVG/source/core/document.hpp
HEADERS += ../../PlatyVG/source/core/layer.hpp
HEADERS += ../../PlatyVG/source/core/record.hpp
HEADERS += ../../PlatyVG/source/core/shape.hpp
HEADERS += ../../PlatyVG/source/core/view.hpp
HEADERS += ../../PlatyVG/source/element/action.hpp
HEADERS += ../../PlatyVG/source/element/color.hpp
HEADERS += ../../PlatyVG/source/element/context.hpp
HEADERS += ../../PlatyVG/source/element/coord.hpp
HEADERS += ../../PlatyVG/source/element/edge.hpp
HEADERS += ../../PlatyVG/source/element/matrix.hpp
HEADERS += ../../PlatyVG/source/element/mesh.hpp
HEADERS += ../../PlatyVG/source/element/point.hpp
HEADERS += ../../PlatyVG/source/element/rect.hpp
HEADERS += ../../PlatyVG/source/element/selector.hpp
HEADERS += ../../PlatyVG/source/element/size.hpp
HEADERS += ../../PlatyVG/source/element/table.hpp
HEADERS += ../../PlatyVG/source/element/zoom.hpp
HEADERS += ../../PlatyVG/source/px/array.hpp
HEADERS += ../../PlatyVG/source/px/buffer.hpp
HEADERS += ../../PlatyVG/source/px/file.hpp
HEADERS += ../../PlatyVG/source/px/math.hpp
HEADERS += ../../PlatyVG/source/px/memory.hpp
HEADERS += ../../PlatyVG/source/px/parser.hpp
HEADERS += ../../PlatyVG/source/px/px.hpp
HEADERS += ../../PlatyVG/source/px/share.hpp
HEADERS += ../../PlatyVG/source/px/string.hpp
