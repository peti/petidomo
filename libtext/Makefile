#
# Build the libtext library.
#
# $Header$
#

CC 		= gcc
AR 		= ar
RANLIB		= ranlib

CFLAGS		= -Wall -pedantic -O3
CPPFLAGS	=
LDFLAGS		=

OBJECTS		= easy_pattern_match.o find_next_line.o find_string.o \
		  easy_sprintf.o transform_text.o wordwrap.o

.SUFFIXES:
.SUFFIXES:	.c .o

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

libtext.a:	$(OBJECTS)
	$(AR) cr libtext.a $(OBJECTS)
	$(RANLIB) libtext.a

realclean distclean clean::
	rm -f libtext.a $(OBJECTS)

# Dependencies

easy_pattern_match.o: text.h
easy_sprintf.o: text.h
find_next_line.o: text.h
find_string.o: text.h
transform_text.o: text.h
wordwrap.o: text.h
