TEMPLATE = app
QT -= core
CONFIG += console
CONFIG -= qt app_bundle

HEADERS += \
	arguments.h \
	concurrency.h \
	dir.h \
    queue.h \
    levdist.h

SOURCES += \
	arguments.c \
	concurrency.c \
	dir.c \
	main.c \
    queue.c \
    levdist.c
