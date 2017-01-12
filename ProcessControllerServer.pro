QT += core network sql websockets
QT -= gui

CONFIG(debug, release|debug):DEFINES += _DEBUG TEST
CONFIG += c++11

TARGET = ProcessControllerServer
TEMPLATE = app

RESOURCES +=

DISTFILES += \
    src/QtSolutions/qtservice.pri

HEADERS += \
    src/QtSolutions/QtService \
    src/QtSolutions/qtservice.h \
    src/QtSolutions/qtservice_p.h \
    src/QtSolutions/QtServiceBase \
    src/QtSolutions/QtServiceController \
    src/QtSolutions/qtunixserversocket.h \
    src/QtSolutions/qtunixsocket.h \
    src/AppConfigurer.hpp \
    src/AppController.hpp \
    src/AppDeployer.hpp \
    src/AppMonitor.hpp \
    src/AppTakeDown.hpp \
    src/AppUpgrader.hpp \
    src/Database.hpp \
    src/MainController.hpp \
    src/ServerProcess.hpp \
    src/Service.hpp \
    src/TcpCommunicator.hpp

SOURCES += \
    src/QtSolutions/qtservice.cpp \
    src/QtSolutions/qtservice_unix.cpp \
    src/QtSolutions/qtunixserversocket.cpp \
    src/QtSolutions/qtunixsocket.cpp \
    src/AppConfigurer.cpp \
    src/AppController.cpp \
    src/AppDeployer.cpp \
    src/AppMonitor.cpp \
    src/AppTakeDown.cpp \
    src/AppUpgrader.cpp \
    src/Database.cpp \
    src/main.cpp \
    src/MainController.cpp \
    src/ServerProcess.cpp \
    src/Service.cpp \
    src/TcpCommunicator.cpp \
    src/Test.cpp
