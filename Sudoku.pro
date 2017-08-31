#-------------------------------------------------
#
# Project created by QtCreator 2017-08-28T15:38:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Sudoku
TEMPLATE = app


SOURCES += main.cpp\
        MainWindow.cpp \
    sudoku.cpp \
    force_solver.cpp \
    CrossButton.cpp \
    KeyboardFilter.cpp

HEADERS  += MainWindow.h \
    sudoku.h \
    force_solver.h \
    CrossButton.h \
    KeyboardFilter.h

FORMS    += MainWindow.ui
