MYLIBDIR=../mynet
MYLIB=-lmynet
CFLAGS=-I${MYLIBDIR} -L${MYLIBDIR}
OBJS=Conf.o Conf_server.o Conf_client.o Conf_common.o
all: Conf
Conf: ${OBJS}
	${CC} ${CFLAGS} -o $@ $^ ${MYLIB}
clean:
	${RM} *.o Conf *~
