QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    connectconfigdialog.cpp \
    errorstatistic.cpp \
    generatefiledialog.cpp \
    main.cpp \
    manualinputdialog.cpp \
    snrwidget.cpp \
    transferrate.cpp \
    widget.cpp

HEADERS += \
    connectconfigdialog.h \
    errorstatistic.h \
    generatefiledialog.h \
    manualinputdialog.h \
    snrwidget.h \
    transferrate.h \
    widget.h

FORMS += \
    connectconfigdialog.ui \
    generatefiledialog.ui \
    manualinputdialog.ui \
    snrwidget.ui \
    transferrate.ui \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc
