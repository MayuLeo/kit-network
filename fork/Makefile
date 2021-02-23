#
# Makefile for tcp_echo
#
MYLIBDIR=../lib
MYLIB=-lmynet
CFLAGS=-I${MYLIBDIR} -L${MYLIBDIR}

all: fork_server

fork_server: fork_server.o
	${CC} ${CFLAGS} -o $@ $^ ${MYLIB}

clean:
	${RM} *.o fork_server *~
