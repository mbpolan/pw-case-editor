######################################################################
# Automatically generated by qmake (2.01a) czw. czerwca 26 22:51:57 2008
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += . src
INCLUDEPATH += . src
MOC_DIR = moc
UI_DIR = src/ui
DEFINES += __MAC__

TRANSLATIONS = intl/pw_case_editor-qt_pl.ts

RESOURCES = src/resources.qrc

FORMS = src/ui/newcasedialog.ui \
	  src/ui/charactereditor.ui \
	  src/ui/charactermanager.ui \
	  src/ui/locationeditor.ui \
	  src/ui/evidenceeditor.ui \
	  src/ui/bgmanager.ui

# Input
HEADERS =   src/clistview.h \
	    src/listmanager.h \
	    src/mainwindow.h \
	    src/scriptwidget.h \
	    src/case.h \
	    src/character.h \
	    src/iohandler.h \
	    src/sprite.h \
	    src/utilities.h \
	    src/highlighter.h \
	    src/dialogs.h \
	    src/common.h

SOURCES =  src/clistview.cpp \
           src/listmanager.cpp \
           src/mainwindow.cpp \
           src/pw_case_editor.cpp \
           src/scriptwidget.cpp \
	   src/case.cpp \
	   src/character.cpp \
	   src/iohandler.cpp \
	   src/sprite.cpp \
	   src/utilities.cpp \
	   src/highlighter.cpp \
	   src/dialogs.cpp