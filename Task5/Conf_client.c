#include "mynet.h"
#include "Conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>

#define MSGBUF_SIZE 512
#define S_BUFSIZE 512 /* 送信用バッファサイズ */
#define R_BUFSIZE 512 /* 受信用バッファサイズ */
struct idobata
{
  char header[4]; /* パケットのヘッダ部(4バイト) */
  char sep;       /* セパレータ(空白、またはゼロ) */
  char data[];    /* データ部分(メッセージ本体) */
};
static char Buf[MSGBUF_SIZE];
static char *chop_nl(char *s);

void Conf_client(int port_number,char * ServerIPAddress,char * username)
{
  // printf("Client\n");
  // printf("ServerIPAddress : %s\n", ServerIPAddress);
  // printf("Username : %s\n",username);
  int sock;
  char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE], buf[MSGBUF_SIZE];
  int strsize;
  fd_set mask, readfds;
  /* サーバに接続する */
  sock = init_tcpclient(ServerIPAddress, port_number);
  printf("Connected.\n");
  snprintf(Buf,MSGBUF_SIZE,"JOIN %s",username);
  // printf("JOINtext : %s\n",Buf);

  strsize = Send(sock,Buf,MSGBUF_SIZE,0);
  // printf("strsize :%d\n",strsize);
  // printf("Sended\n");
  FD_ZERO(&mask);
  FD_SET(0,&mask);
  FD_SET(sock,&mask);
  struct idobata *packet;
  for(;;)
  {

    readfds = mask;
    select(sock + 1,&readfds,NULL,NULL,NULL);
    memset(r_buf,0,sizeof(r_buf));
    if(FD_ISSET(0,&readfds)){
      /* キーボードから文字列を入力する */
      fgets(s_buf, S_BUFSIZE, stdin);
      chop_nl(s_buf);
      if(strcmp(s_buf,"QUIT") == 0)//QUITが入力されたかどうか
      {
        strsize = strlen(s_buf);
        Send(sock, s_buf, strsize, 0);
        close(sock);
        exit(EXIT_SUCCESS);
      }
      else{
        snprintf(buf,MSGBUF_SIZE,"POST %s",s_buf);
        strsize = strlen(buf);
        Send(sock, buf, strsize, 0);
      }
    }

    if(FD_ISSET(sock,&readfds))
    {
      
      strsize = Recv(sock, r_buf, R_BUFSIZE - 1, 0);
      packet = (struct idobata *)r_buf;
      printf("パケット受信:%s\n",r_buf);
      // printf("packet->header:%s\n",packet->header);
      // printf("packet->data:%s\n", packet->data);
      if(strncmp(packet->header,"MESG",4) == 0)
      {
        printf("%s\n",packet->data);
      }
    }
    //printf("%d\n",i);
  }
  //TCP接続要求

  //サーバーに「JOIN username」メッセージを送信

  //メッセージがきたら表示．
  //if(自分の入力テキストが「QUIT」以外)
    //「POST 発言メッセージ」の形式でサーバに送信
  //else if(「QUIT」なら)
    //「QUIT」メッセージをサーバに 送信し、サーバとの接続を閉じて、プログラムを終了
}
static char *chop_nl(char *s)
{
  int len;
  len = strlen(s);
  if (s[len - 1] == '\n')
  {
    s[len - 1] = '\0';
  }
  return (s);
}