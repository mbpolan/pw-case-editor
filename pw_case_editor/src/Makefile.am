bin_PROGRAMS = pw_case_editor



AM_CXXFLAGS = @CXXFLAGS@ @GTKMM_CFLAGS@

pw_case_editor_LDADD = -larchive -lMagick -lMagick++ -lWand @GTKMM_LIBS@ @LIBS@
pw_case_editor_SOURCES = case.cpp casecombobox.cpp character.cpp clistview.cpp \
	dialogs.cpp hotspotwidget.cpp iconmanager.cpp iohandler.cpp locationwidget.cpp \
	main.cpp mainwindow.cpp scriptwidget.cpp sprite.cpp spriteeditor.cpp \
	testimonyeditor.cpp textboxdialog.cpp textboxeditor.cpp triggerdialogs.cpp utilities.cpp
noinst_HEADERS = case.h casecombobox.h character.h clistview.h dialogs.h \
	exceptions.h hotspotwidget.h iconmanager.h iohandler.h locationwidget.h mainwindow.h \
	scriptwidget.h sprite.h spriteeditor.h testimonyeditor.h textboxdialog.h \
	textboxeditor.h triggerdialogs.h triplet.h utilities.h
