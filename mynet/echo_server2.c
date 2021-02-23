#include "mynet.h"
#define PORT 30200         /* ポート番号 ←適当に書き換える */
#define BUFSIZE 50   /* バッファサイズ */

int main()
{
  //struct sockaddr_in my_adrs;

  int sock_listen, sock_accepted;

  char buf[BUFSIZE];
  int strsize;

  sock_listen = init_tcpserver(PORT,5);

  /* クライアントの接続を受け付ける */
  sock_accepted = accept(sock_listen, NULL, NULL);


  do{
    /* 文字列をクライアントから受信する */
    if((strsize=recv(sock_accepted, buf, BUFSIZE, 0)) == -1){
      exit_errmesg("recv()");
      //exit(EXIT_FAILURE);
    }

    /* 文字列をクライアントに送信する */
    if(send(sock_accepted, buf, strsize, 0) == -1 ){
      exit_errmesg("send()");
      //exit(EXIT_FAILURE);
    }
  }while( buf[strsize-1] != '\n' ); /* 改行コードを受信するまで繰り返す */
  close(sock_listen);
  close(sock_accepted);

  exit(EXIT_SUCCESS);
}
