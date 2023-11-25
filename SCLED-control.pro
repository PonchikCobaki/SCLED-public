QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets network

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    deviceDataFiles.cpp \
    main.cpp \
    mainwindow.cpp \
    serviceParemeters.cpp

HEADERS += \
    deviceDataFiles.h \
    gradientPaletts.cpp \
    mainwindow.h \
    serviceParemeters.h

FORMS += \
    mainwindow.ui

win32:RC_FILE = file.rc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/ -lqmdnsengine
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/ -lqmdnsengined

INCLUDEPATH += $$PWD/lib/include/qmdnsengine
DEPENDPATH += $$PWD/lib/include/qmdnsengine

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/libqmdnsengine.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/libqmdnsengined.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/lib/qmdnsengine.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/lib/qmdnsengined.lib

RESOURCES += \
  icons.qrc
