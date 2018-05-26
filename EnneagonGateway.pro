QT += core sql network
QT -= gui

TARGET = EnneagonGateway
CONFIG += console
CONFIG -= app_bundle

CONFIG += release

TEMPLATE = app

LIBS += -lcryptopp -lqamqp

SOURCES += main.cpp \
    device/canworkerthread.cpp \
    device/canwrapper.cpp \
    utility/cdatahelper.cpp \
    utility/json.cpp \
    network/creghttp.cpp \
    manage/cmanage.cpp \
    network/ccardverifyhttphandler.cpp \
    network/cflownohttp.cpp \
    pile/cenneagonconnector.cpp \
    manage/capp.cpp \
    pile/cconnectormanager.cpp \
    pile/cverify.cpp \
    pile/cconnectorstatus.cpp \
    pile/cbasewarning.cpp \
    pile/cconnectororder.cpp \
    pile/cmodule.cpp \
    network/cstatehttp.cpp \
    pile/crecordorder.cpp \
    device/ccandatapacker.cpp \
    network/crabbitmq.cpp \
    network/cconsumehttp.cpp \
    network/crecordhttp.cpp \
    utility/cdatauploader.cpp \
    network/cremotesocket.cpp \
    network/cbookorderhttp.cpp \
    network/cbookorderupdatehttp.cpp \
    network/cwechatrefund.cpp \
    network/calirefund.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    device/canworkerthread.h \
    device/canwrapper.h \
    utility/cdatahelper.h \
    utility/json.h \
    network/creghttp.h \
    pile/cgundefine.h \
    manage/cmanage.h \
    network/ccardverifyhttphandler.h \
    device/ccandatapacker.h \
    network/cflownohttp.h \
    pile/cenneagonconnector.h \
    manage/capp.h \
    pile/cconnectormanager.h \
    pile/cverify.h \
    pile/cconnectorstatus.h \
    pile/cbasewarning.h \
    pile/cconnectororder.h \
    pile/cmodule.h \
    network/cstatehttp.h \
    pile/crecordorder.h \
    utility/aeshandler.h \
    network/crabbitmq.h \
    network/cconsumehttp.h \
    network/crecordhttp.h \
    utility/cdatauploader.h \
    network/cremotesocket.h \
    network/cbookorderhttp.h \
    network/cbookorderupdatehttp.h \
    network/cwechatrefund.h \
    network/calirefund.h
