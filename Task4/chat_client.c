//chat_client.c
#include "chat.h"
#include "mynet.h"
#include <stdlib.h>
#include <sys/select.h>

#define S_BUFSIZE 100   /* 送信用バッファサイズ */
#define R_BUFSIZE 100   /* 受信用バッファサイズ */

void chat_client(char* servername, int port_number)
{
  int sock;
  char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
  int strsize;
  fd_set mask, readfds;
  char *textcolor = "\x1b[39m";

  /* サーバに接続する */
  sock = init_tcpclient(servername, port_number);
  printf("Connected.\n");

  /* ビットマスクの準備 */
  FD_ZERO(&mask);
  FD_SET(0, &mask);
  FD_SET(sock, &mask);

  for(;;){
    /* 受信データの有無をチェック */

    readfds = mask;
    select( sock+1, &readfds, NULL, NULL, NULL );

    if( FD_ISSET(0, &readfds) ){
      /* キーボードから文字列を入力する */
      //printf("\x1b[37m");
      fgets(s_buf, S_BUFSIZE, stdin);
      strsize = strlen(s_buf);
      Send(sock, s_buf, strsize, 0);
    }

    //サーバーから文字列を受信する．自分が送った文章は緑で表示し，他人の文章はターミナルのデフォルトの色で表示する．
    if( FD_ISSET(sock, &readfds) ){
      /* サーバから文字列を受信する */
      strsize = Recv(sock, r_buf, R_BUFSIZE-1, 0);
      r_buf[strsize] = '\0';
      if(r_buf[0] == 's')
        printf("\x1b[32m");
      else
        printf("\x1b[39m");
      printf("\r%s\n",(r_buf+1));
      fflush(stdout);
      //printf("\x1b[37m");
    }


  }

}
