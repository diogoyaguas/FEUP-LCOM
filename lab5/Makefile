# Makefile for lab5

COMPILER_TYPE= gnu

CC= gcc

PROG= lab5
SRCS= lab5.c test5.c vbe.c video_gr.c timer.c kbd.c video_test.c read_xpm.c sprite.c

CFLAGS= -Wall
CPPFLAGS += -D LAB5

DPADD+= ${LIBDRIVER} ${LIBSYS}
LDADD+= -llm -ldriver -lsys

LDFLAGS+= -L .

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
