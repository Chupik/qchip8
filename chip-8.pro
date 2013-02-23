HEADERS += \
    mainwindow.h \
    cpu8.h \
    chip8_display.h

SOURCES += \
    mainwindow.cpp \
    cpu8.cpp \
    chip8_display.cpp
greaterThan(QT_MAJOR_VERSION, 4) {
  QT += widgets
  DEFINES += HAVE_QT5
}
