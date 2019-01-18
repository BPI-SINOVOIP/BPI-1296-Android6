SHELL=/bin/bash

SDL_LIB=SDL2-2.0.1

all:
	@if [ ! -r $(SDL_LIB)/Makefile ]; then \
		$(SHELL) ./119x_SDL_config.sh $(SDL_LIB); \
	fi
	make -C $(SDL_LIB)

clean:
	@if test -f $(SDL_LIB)/Makefile; then \
		make -C $(SDL_LIB) $@; \
	fi
