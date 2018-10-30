TEMPLATE = app
QT -= core
CONFIG += console
CONFIG -= qt app_bundle

QMAKE += -pthread
LIBS += -pthread

HEADERS += \
	arguments.h \
	concurrency.h \
	dir.h \
	levdist.h \
	queue.h \
        levenshtein.h \

SOURCES += \
	arguments.c \
	concurrency.c \
	dir.c \
	levdist.c \
	main.c \
	queue.c \
        levenshtein.c \
