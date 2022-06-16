QT       += core gui
QT       += network
QT       += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
INCLUDEPATH += src/Dlg \
    src/utils \

SOURCES += \
    main.cpp \
    src/Dlg/paradigm.cpp \
    src/Dlg/setupdlg.cpp \
    src/Dlg/waitforfiledlg.cpp \
    src/utils/scontrol.cpp \
    src/utils/sdisplay.cpp \
    src/utils/sjson_qt.cpp \
    src/utils/sserial.cpp \
    src/utils/unzip.cpp \
    src/utils/utils.cpp

HEADERS += \
    src/Dlg/paradigm.h \
    src/Dlg/setupdlg.h \
    src/Dlg/waitforfiledlg.h \
    src/utils/data_info.h \
    src/utils/scontrol.h \
    src/utils/sdisplay.h \
    src/utils/sjson_qt.h \
    src/utils/sserial.h \
    src/utils/unzip.h \
    src/utils/utils.h


FORMS += \
    src/Dlg/setupdlg.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/7z.qrc
