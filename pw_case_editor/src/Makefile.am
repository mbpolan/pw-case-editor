bin_PROGRAMS = pw_case_editor



AM_CXXFLAGS = @CXXFLAGS@ @GTKMM_CFLAGS@ 

pw_case_editor_LDADD = -lMagick++ -lWand -lMagick @GTKMM_LIBS@ @LIBS@
pw_case_editor_SOURCES = case.cpp character.cpp clistview.cpp dialogs.cpp \
	hotspotwidget.cpp iohandler.cpp main.cpp mainwindow.cpp scriptwidget.cpp spriteeditor.cpp \
	testimonyeditor.cpp textboxdialog.cpp textboxeditor.cpp utilities.cpp
noinst_HEADERS = case.h character.h clistview.h dialogs.h hotspotwidget.h \
	iohandler.h mainwindow.h scriptwidget.h sprite.h spriteeditor.h testimonyeditor.h \
	textboxdialog.h textboxeditor.h utilities.h
