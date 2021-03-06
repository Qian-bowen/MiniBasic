QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    error.cpp \
    handle.cpp \
    main.cpp \
    memory.cpp \
    msgWindow.cpp \
    parser.cpp \
    program.cpp \
    statement.cpp \
    variable.cpp \
    widget.cpp

HEADERS += \
    error.h \
    expressionNode.h \
    handle.h \
    line.h \
    memory.h \
    msgWindow.h \
    parser.h \
    program.h \
    statement.h \
    type.h \
    variable.h \
    widget.h

FORMS += \
    msgWindow.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
