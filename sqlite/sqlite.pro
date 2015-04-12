TEMPLATE = lib
CONFIG += staticlib
HEADERS = sqlite3.h
SOURCES = sqlite3.c
DEFINES += SQLITE_ENABLE_FTS4
DESTDIR = build
