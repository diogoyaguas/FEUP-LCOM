COMPILER_TYPE = gnu

CC = gcc

PROG = proj
SRCS = bitmap.c jumpingPeanut.c keyboard.c main.c mouse.c timer.c vbe.c video_gr.c game.c stateMachine.c cloud.c rtc.c

CFLAGS= -Wall

DPADD += ${LIBDRIVER} ${LIBSYS}
LDADD += -llm -ldriver -lsys

LDFLAGS += -L .

BINDIR? = /usr/sbin

MAN=

.include <bsd.prog.mk>
.include <bsd.gcc.mk>
