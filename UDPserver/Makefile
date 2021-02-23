MYLIBDIR=../mynet
MYLIB=-lmynet
CFLAGS=-I${MYLIBDIR} -L${MYLIBDIR}
OBJS=echo_server.o
all: echo_server
echo_server: ${OBJS}
	${CC} ${CFLAGS} -o $@ $^ ${MYLIB}
clean:
	${RM} *.o echo_server *~
