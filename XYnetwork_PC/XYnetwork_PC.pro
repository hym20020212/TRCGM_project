QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    baseset.cpp \
    connectdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    mxmodedialog.cpp \
    nodeitem.cpp \
    replacechaindialog.cpp \
    senddata.cpp \
    slotitem.cpp \
    topologyview.cpp

HEADERS += \
    baseset.h \
    connectdialog.h \
    mainwindow.h \
    mxmodedialog.h \
    nodeitem.h \
    replacechaindialog.h \
    senddata.h \
    slotitem.h \
    topologyview.h

FORMS += \
    baseset.ui \
    connectdialog.ui \
    mainwindow.ui \
    mxmodedialog.ui \
    replacechaindialog.ui \
    senddata.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
