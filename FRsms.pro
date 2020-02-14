#-------------------------------------------------
#
# Project created by QtCreator 2020-02-05T12:57:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FRsms
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        main.cpp \
    mainwindow.cpp \
    database.cpp \
    dialog.cpp \
    error_dialog.cpp \
    sms/coolsms.cpp \
    sms/CSCP.cpp \
    sms/cslog.cpp \
    sms/keygen.cpp \
    sms/serverloadbalance.cpp \
    sms/TBSP.cpp \
    sms/tcpconn.cpp \
    sms/token.cpp \
    sms/csmd5.c \
    imageutil.cpp


HEADERS += \
    mainwindow.h \
    database.h \
    dialog.h \
    error_dialog.h \
    sms/coolsms.hpp \
    sms/CSCP.hpp \
    sms/cslog.hpp \
    sms/csmd5.h \
    sms/keygen.hpp \
    sms/serverloadbalance.hpp \
    sms/TBSP.hpp \
    sms/tcpconn.hpp \
    sms/token.hpp \
    imageutil.h


FORMS += \
    mainwindow.ui \
    dialog.ui \
    error_dialog.ui \
    dialog.ui


INCLUDEPATH += /opt/intel/openvino/opencv/include
unix: LIBS += -L"/opt/intel/openvino/opencv/lib" -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs
DEPENDPATH += /opt/intel/openvino/opencv/bin

INCLUDEPATH += /home/intel01/intel_vas/vas/include
unix: LIBS += -L"/home/intel01/intel_vas/vas/lib/intel64" -lvasfd -lvasfr -lvasfac -lvasot -lvaspvd -lvascore
DEPENDPATH += /home/intel01/intel_vas/vas/lib/intel64

INCLUDEPATH += /usr/include/mysql
unix: LIBS += -L"/usr/lib/mysql/plugin" -lmysqlclient
DEPENDPATH += /usr/lib/mysql/plugin



# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
