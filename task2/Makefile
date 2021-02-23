#
# Makefile for tcp_echo
#
MYLIBDIR=../lib
MYLIB=-lmynet
CFLAGS=-I${MYLIBDIR} -L${MYLIBDIR}

all: server_task

server_task: server_task.o
	${CC} ${CFLAGS} -o $@ $^ ${MYLIB}

clean:
	${RM} *.o server_task *~
