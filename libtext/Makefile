#
# Text routine library
#
# $Header$
#

# Make Rules:
# ===========
#
.SUFFIXES: .3

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<


# Compiler flags:
# ===============
#
CFLAGS	= -Wall
CPPFLAGS=

# Linker flags:
# =============
#
LDFLAGS	=
LIBS	=


#
# Labels:
# =======
#
SRCS	= easy_pattern_match.c find_next_line.c find_string.c easy_sprintf.c \
	  transform_text.c wordwrap.c
OBJS	= $(SRCS:.c=.o)
MANFILES= text_easy_pattern_match.3 text_find_string.3 text_transform_text.3 \
	  text_find_next_line.3 text_easy_sprintf.3 text_wordwrap.3


#
# Targets
#
.PHONY: all man clean depend

all:	libtext.a

man:	$(MANFILES)

clean:
	rm -f libtext.a test $(OBJS)
	rm -f *.o *.3 *.core *.bak

depend:
	makedepend -Y /usr/include $(SRCS)
	@rm -f Makefile.bak


#
# Actions:
#=========
#
libtext.a:	$(OBJS)
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@

text_easy_pattern_match.3:	easy_pattern_match.c text.h
	c2man -itext.h easy_pattern_match.c

text_find_string.3:		find_string.c text.h
	c2man -itext.h find_string.c

text_transform_text.3:		transform_text.c text.h
	c2man -itext.h transform_text.c

text_find_next_line.3:		find_next_line.c text.h
	c2man -itext.h find_next_line.c

text_easy_sprintf.3:		easy_sprintf.c text.h
	c2man -itext.h easy_sprintf.c

text_wordwrap.3:		wordwrap.c text.h
	c2man -itext.h wordwrap.c


#
# Dependencies
#
