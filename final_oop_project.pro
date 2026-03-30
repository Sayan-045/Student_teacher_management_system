QT += widgets core gui sql

CONFIG += c++17

SOURCES += \
    Database.cpp \
    loginwindow.cpp \
    main.cpp \
    sqlite3.c \
    studentdashboard.cpp \
    teacherdashboard.cpp

HEADERS += \
    Database.h \
    loginwindow.h \
    sqlite3.h \
    studentdashboard.h \
    teacherdashboard.h

FORMS += \
    loginwindow.ui \
    studentdashboard.ui \
    teacherdashboard.ui