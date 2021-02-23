#include "mynet.h"
#include <sys/wait.h>


#define BUFSIZE 50   /* バッファサイズ */

extern char *optarg;
extern int optind, opterr, optopt;
int main(int argc, char *argv[])
{
  int port_number = 30202;
  int sock_listen, sock_accepted;
  int n_process = 0;
  pid_t child;
  char buf[BUFSIZE];
  int strsize;
  int PRCS_LIMIT = 10;

  int c;
  opterr = 0;
  while( 1 ){
    c = getopt(argc, argv, "n:p:h");
    if( c == -1 ) break;

    switch( c ){
    case 'n' :  /* プロセス数 */
      PRCS_LIMIT = atoi(optarg);
      break;

    case 'p':  /* ポート番号の指定 */
      port_number = atoi(optarg);
      break;

    case '?' :
      fprintf(stderr,"Unknown option '%c'\n", optopt );
    case 'h' :
      fprintf(stderr,"Usage: %s -n PRCS_LIMIT -p port_number\n",
        argv[0]);
      exit(EXIT_FAILURE);
      break;
    }
  }

  /* サーバの初期化 */
  sock_listen = init_tcpserver(port_number, 5);

  //あらかじめ子プロセスの生成
  //child = fork();



  child = 1;
  printf("First : %d\n",child );
  for(int i = 0;i < PRCS_LIMIT;i++){
    if(child > 0){
      child = fork();
      printf("i = %d : %d\n",i,child );
    }else if(child == 0)
    {
      break;
    }
    else{
      exit_errmesg("fork()");
    }

  }

  printf("Me %d\n",child );
  for(;;){
    printf("child = %d\n", child);
    /* クライアントの接続を受け付ける */
    sock_accepted = accept(sock_listen, NULL, NULL);

    if( child == 0 ){
      /* Child process */
      //close(sock_listen);
      do{
          printf("child\n");
	       /* 文字列をクライアントから受信する */
	        if((strsize=recv(sock_accepted, buf, BUFSIZE, 0)) == -1){
	           exit_errmesg("recv()");
	        }
          printf("Get : %s\n", buf);
	        /* 文字列をクライアントに送信する */
	        if(send(sock_accepted, buf, strsize, 0) == -1 ){
	           exit_errmesg("send()");
	        }
      }while( buf[strsize-1] != '\n' ); /* 改行コードを受信するまで繰り返す */

      close(sock_accepted);
      //exit(EXIT_SUCCESS);
    }
    else if( child >0 ){
      printf("parent\n");
      // parent's process
      //n_process++;
      printf("Client is accepted.[%d]\n", child);
      close(sock_accepted);
    }
    else{
      /* fork()に失敗 */
      printf("child = %d\n", child);
      close(sock_listen);
      exit_errmesg("fork()");
    }


  }

  /* never reached */
}
