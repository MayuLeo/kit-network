#include "mynet.h"
#include <stdlib.h>
#include <sys/select.h>
#include <sys/time.h>
#include <arpa/inet.h>

#define SERVER_LEN 256     /// サーバ名格納用バッファサイズ 
#define DEFAULT_PORT 50001 // クライアントデフォルトポート番号 

#define S_BUFSIZE 512   /* 送信用バッファサイズ */
#define R_BUFSIZE 512   /* 受信用バッファサイズ */
#define TIMEOUT_SEC 1

extern char *optarg;
extern int optind, opterr, optopt;

int main(int argc, char *argv[])
{
  struct sockaddr_in broadcast_adrs;
  struct sockaddr_in from_adrs;
  socklen_t from_len;

  int sock;
  int broadcast_sw=1;
  fd_set mask, readfds;
  struct timeval timeout;

  char s_buf[S_BUFSIZE], r_buf[R_BUFSIZE];
  int strsize;
  char *username;

  char *HELO = "HELO";

  int port_number=DEFAULT_PORT;
  int c;
  int is_Reply = 0;
  //char servername[SERVER_LEN] = "localhost";
  
  // オプション文字列の取得 
  opterr = 0;
  while( 1 ){
	  c = getopt(argc, argv, "u:p:");
	  if( c == -1 ) break;

	  switch( c ){
	  case 'u' :  /* usernameの入力 *///Todo
	    username = optarg;
	    break;
	  case 'p':  /* ポート番号の指定 */
	    port_number = atoi(optarg);
	    break;

	  }
  }
  if(username == NULL)
  {
    printf("ユーザー名を-uオプションを使って指定してください\n");
    exit_errmesg("username");
  }


  printf("portNumber : %d\n",port_number);
  //HELOパケットをUDPでブロードキャストしてみる
  /* ブロードキャストアドレスの情報をsockaddr_in構造体に格納する */
  set_sockaddr_in(&broadcast_adrs, "255.255.255.255", (in_port_t)port_number);
  /* ソケットをDGRAMモードで作成する */
  sock = init_udpclient();

  /* ソケットをブロードキャスト可能にする */
  if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&broadcast_sw, sizeof(broadcast_sw)) == -1){
    exit_errmesg("setsockopt()");
  }

  /* ビットマスクの準備 */
  FD_ZERO(&mask);
  FD_SET(sock, &mask);

  for(int i = 0;i <= 3;i++){
    /* 文字列をサーバに送信する */
    Sendto(sock, HELO, strlen(HELO), 0, (struct sockaddr *)&broadcast_adrs, sizeof(broadcast_adrs) );

    /* サーバから文字列を受信して表示 */
    for(;;){

      /* 受信データの有無をチェック */
      readfds = mask;
      timeout.tv_sec = TIMEOUT_SEC;
      timeout.tv_usec = 0;
      
      if( select( sock+1, &readfds, NULL, NULL, &timeout )==0 ){
        printf("Time out.\n");
        break;
      }
      from_len = sizeof(from_adrs);
      strsize = Recvfrom(sock, r_buf, R_BUFSIZE-1, 0,(struct sockaddr *)&from_adrs, &from_len);
      r_buf[strsize] = '\0';
      printf("[%s] %s\n",inet_ntoa(from_adrs.sin_addr), r_buf);
    
    }
    if(strcmp(r_buf,"HELE") == 0){
      is_Reply = 1;
      break;
    }
  }
  //できなかったら3回再送

  //if(返事が来なければ)
  if(is_Reply == 0)
  {
    //if(port_number == DEFAULT_PORT)
    //  port_number = 50001;
    Conf_server(port_number,username);
  }
    //自身がサーバー
    //ポート番号が特に指定されていなければデフォルトポート番号を"50001"に設定し直す
    //Conf_server(port_number);  // サーバ部分ができたらコメントを外す 
  else
  {
    Conf_client(port_number,inet_ntoa(from_adrs.sin_addr),username);
  }
  //else(返事が帰ってきたら)
    //自身がクライアント
    //サーバーからのIPアドレスを保存する
    //Conf_client(port_number,serverIPaddress); //クライアント部分を作る．
  

  
  

  exit(EXIT_SUCCESS);
}
