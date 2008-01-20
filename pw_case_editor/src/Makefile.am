bin_PROGRAMS = pw_case_editor



AM_CXXFLAGS = @CXXFLAGS@ @GTKMM_CFLAGS@

pw_case_editor_LDADD = -lgthread-2.0 -lMagick -lMagick++ -lWand -larchive \
	@GTKMM_LIBS@ @LIBS@
pw_case_editor_SOURCES = alphaimage.cpp case.cpp casecombobox.cpp character.cpp \
	clistview.cpp customizedialog customizedialog.cpp dialogs.cpp editdialogs.cpp \
	hotspotwidget.cpp iconmanager.cpp iohandler.cpp locationwidget.cpp main.cpp \
	mainwindow.cpp scriptwidget.cpp splashscreen.cpp sprite.cpp spriteeditor.cpp \
	testimonyeditor.cpp textboxdialog.cpp textboxeditor.cpp tooltips.cpp triggerdialogs.cpp \
	utilities.cpp
noinst_HEADERS = alphaimage.h case.h casecombobox.h character.h clistview.h \
	customizedialog.h dialogs.h editdialogs.h exceptions.h hotspotwidget.h iconmanager.h \
	iohandler.h locationwidget.h mainwindow.h scriptwidget.h splashscreen.h sprite.h \
	spriteeditor.h testimonyeditor.h textboxdialog.h textboxeditor.h tooltips.h \
	triggerdialogs.h triplet.h utilities.h
