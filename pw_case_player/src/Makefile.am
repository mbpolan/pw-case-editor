bin_PROGRAMS = pw_case_player
pw_case_player_SOURCES = application.cpp audio.cpp case.cpp character.cpp \
	font.cpp game.cpp iohandler.cpp pw_case_player.cpp renderer.cpp sdlcontext.cpp \
	sdlcontext.h sprite.cpp textparser.cpp texture.cpp

# set the include path found by configure
AM_CPPFLAGS =  $(LIBSDL_CFLAGS) $(all_includes)

# the library search path.
pw_case_player_LDFLAGS = $(all_libraries) $(LIBSDL_RPATH)
pw_case_player_LDADD = -lSDL_mixer $(LIBSDL_LIBS) -lSDL_gfx -lSDL_image -lm
noinst_HEADERS = application.h audio.h case.h character.h common.h font.h \
	game.h iohandler.h renderer.h sprite.h textparser.h texture.h
