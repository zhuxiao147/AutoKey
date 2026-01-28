QT += core gui widgets

CONFIG += c++14

TARGET = AutoKeyTool
TEMPLATE = app

SOURCES += main.cpp \
    mainwindow.cpp \
    hiddevice.cpp \
    keysequenceparser.cpp

HEADERS += mainwindow.h \
    hiddevice.h \
    keysequenceparser.h

#LIBS += -lhid -lsetupapi

FORMS += mainwindow.ui

# 测试程序配置
test_sequence.target = test_sequence
test_sequence.CONFIG += console
test_sequence.CONFIG -= app_bundle
test_sequence.SOURCES = test_sequence.cpp \
    keysequenceparser.cpp
test_sequence.HEADERS = keysequenceparser.h


QMAKE_EXTRA_TARGETS += test_sequence
