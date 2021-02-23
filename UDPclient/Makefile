MYLIBDIR=../mynet
MYLIB=-lmynet
CFLAGS=-I${MYLIBDIR} -L${MYLIBDIR}
OBJS=echo_client.o
all: echo_client
echo_client: ${OBJS}
	${CC} ${CFLAGS} -o $@ $^ ${MYLIB}
clean:
	${RM} *.o echo_client *~
