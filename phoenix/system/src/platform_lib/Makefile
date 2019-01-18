ifndef TOP
	TOP = ..
endif

DIRS=$(shell ls -d */ | grep -vE 'VFD' )

.PHONY: all clean $(DIRS)

all: $(DIRS)

$(DIRS):
	@if test -d $@; then cd $@; $(MAKE) all; fi

clean:
	@for dir in $(DIRS); do \
		if test -d $$dir; then \
			$(ECHO) "$$dir: $(MAKE) $@"; \
			if (cd $$dir; $(MAKE) $@); then true; else exit 1; fi; \
		fi \
	done;
