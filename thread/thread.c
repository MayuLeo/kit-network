/*
  echo_server3th.c (Thread版)
*/
#include "mynet.h"
#include <pthread.h>

#define BUFSIZE 50   /* バッファサイズ */

void * echo_thread(void *arg);


extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char *argv[])
{
  int port_number = 30200;
  int sock_listen, sock_accepted;
  int *tharg;
  int ThreadNUM = 10;
  pthread_t tid;

  int c;
  opterr = 0;
  while( 1 ){
	  c = getopt(argc, argv, "n:p:h");
	  if( c == -1 ) break;

	  switch( c ){
	  case 'n' :  /* スレッド数 */
	    ThreadNUM = atoi(optarg);
	    break;

	  case 'p':  /* ポート番号の指定 */
	    port_number = atoi(optarg);
	    break;

	  case '?' :
	    fprintf(stderr,"Unknown option '%c'\n", optopt );
	  case 'h' :
	    fprintf(stderr,"Usage: %s -n Threadnum -p port_number\n",
		    argv[0]);
	    exit(EXIT_FAILURE);
	    break;
	  }
  }
  /* サーバの初期化 */
  sock_listen = init_tcpserver(port_number, 5);

  for(int i = 0;i < ThreadNUM;i++){
    ///* クライアントの接続を受け付ける */
    //sock_accepted = accept(sock_listen, NULL, NULL);

    /* スレッド関数の引数を用意する */
    if( (tharg = (int *)malloc(sizeof(int)))==NULL ){
      exit_errmesg("malloc()");
    }

    *tharg = sock_listen;

    /* スレッドを生成する */
    if( pthread_create(&tid, NULL, echo_thread, (void *)tharg) != 0 ){
      exit_errmesg("pthread_create()");
    }
  }
  while (1) {

  }
  /* never reached */
}

/* スレッドの本体 */
void * echo_thread(void *arg)
{
  int sock;
  char buf[BUFSIZE];
  int strsize;
  int sock_accepted;


  sock = *((int *)arg);
  free(arg);            /* 引数用のメモリを開放 */

  while (1) {
    /* code */

  sock_accepted = accept(sock, NULL, NULL);


  pthread_detach(pthread_self()); /* スレッドの分離(終了を待たない) */

  do{
    /* 文字列をクライアントから受信する */
    if((strsize=recv(sock_accepted, buf, BUFSIZE, 0)) == -1){
      exit_errmesg("recv()");
    }

    /* 文字列をクライアントに送信する */
    if(send(sock_accepted, buf, strsize, 0) == -1 ){
      exit_errmesg("send()");
    }
  }while( buf[strsize-1] != '\n' ); /* 改行コードを受信するまで繰り返す */
  }

  close(sock_accepted);   /* ソケットを閉じる */
  return(NULL);
}
