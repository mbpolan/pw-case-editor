bin_PROGRAMS = pw_case_editor



AM_CXXFLAGS = @CXXFLAGS@ @GTKMM_CFLAGS@ @ImageMagick_CFLAGS@ @MagickWand_CFLAGS@

pw_case_editor_LDADD = -lgthread-2.0 -larchive @GTKMM_LIBS@ @ImageMagick_LIBS@ @MagickWand_LIBS@ @LIBS@
pw_case_editor_SOURCES = alphaimage.cpp auxdialogs.cpp case.cpp \
	casecombobox.cpp character.cpp clistview.cpp colorwidget.cpp config.cpp \
	coreblockdialog.cpp customizedialog customizedialog.cpp dialogs.cpp editdialogs.cpp \
	hotspotwidget.cpp iconmanager.cpp intl.cpp iohandler.cpp locationwidget.cpp \
	mainwindow.cpp pw_case_editor.cpp scriptwidget.cpp splashscreen.cpp sprite.cpp \
	spriteeditor.cpp testimonyeditor.cpp textboxdialog.cpp textboxeditor.cpp tooltips.cpp \
	triggerdialogs.cpp utilities.cpp
noinst_HEADERS = alphaimage.h auxdialogs.h case.h casecombobox.h character.h \
	clistview.h colorwidget.h config.h coreblockdialog.h customizedialog.h dialogs.h \
	editdialogs.h exceptions.h hotspotwidget.h iconmanager.h intl.h iohandler.h \
	locationwidget.h mainwindow.h scriptwidget.h splashscreen.h sprite.h spriteeditor.h \
	testimonyeditor.h textboxdialog.h textboxeditor.h tooltips.h triggerdialogs.h triplet.h \
	utilities.h
