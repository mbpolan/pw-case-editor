bin_PROGRAMS = pw_case_player
pw_case_player_SOURCES = application.cpp audio.cpp case.cpp character.cpp \
	font.cpp fpstimer.cpp game.cpp iohandler.cpp pw_case_player.cpp renderer.cpp \
	sdlcontext.cpp sdlcontext.h sprite.cpp stock.cfg textparser.cpp texture.cpp theme.cpp \
	theme.xml uimanager.cpp utilities.cpp

# set the include path found by configure
AM_CPPFLAGS =  $(LIBSDL_CFLAGS) $(all_includes)

# the library search path.
pw_case_player_LDFLAGS = $(all_libraries) $(LIBSDL_RPATH)
pw_case_player_LDADD = -L/usr/X11R6/lib -lGL -lGLU $(LIBSDL_LIBS) -lSDL_gfx \
	-lSDL_image -lSDL_mixer -lSDL_ttf -larchive -lglib-2.0 -lglibmm-2.4 -lgobject-2.0 \
	-lm -lsigc-2.0 -lxml2
noinst_HEADERS = application.h audio.h callback.h case.h character.h common.h \
	font.h fpstimer.h game.h iohandler.h renderer.h sprite.h textparser.h \
	texture.h theme.h uimanager.h utilities.h
INCLUDES = -I/usr/include/glibmm-2.4 -I/usr/lib/glibmm-2.4/include \
	-I/usr/include/sigc++-2.0 -I/usr/lib/sigc++-2.0/include -I/usr/include/glib-2.0 \
	-I/usr/lib/glib-2.0/include -I/usr/include/libxml2
