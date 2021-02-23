//chat_util.c
#include "mynet.h"
#include "chat.h"
#include <stdlib.h>
#include <sys/select.h>

#define NAMELENGTH 20 /* ログイン名の長さ制限 */
#define BUFLEN 500    /* 通信バッファサイズ */

/* 各クライアントのユーザ情報を格納する構造体の定義 */
typedef struct{
  int  sock;
  char name[NAMELENGTH];
} client_info;

/* プライベート変数 */
static int N_client;         /* クライアントの数 */
static client_info *Client;  /* クライアントの情報 */
static int Max_sd;               /* ディスクリプタ最大値 */
static char Buf[BUFLEN];     /* 通信用バッファ */
static char GetBuf[BUFLEN];  // 文字列を制御するための変数
static int rnbool;           //受け取ったテキストが改行のみだった時のフラグ変数

/* プライベート関数 */
static int client_login(int sock_listen);
static int get_chat();
static void send_chat();
static char *chop_nl(char *s);

void init_client(int sock_listen, int n_client)
{
  N_client = n_client;

  /* クライアント情報の保存用構造体の初期化 */
  if( (Client=(client_info *)malloc(N_client*sizeof(client_info)))==NULL ){
    exit_errmesg("malloc()");
  }

  /* クライアントのログイン処理 */
  Max_sd = client_login(sock_listen);

}

//チャットの全体のループ
void chat_loop()
{
  char *text;
  rnbool = 0;
  int client_id;
  for(;;){
    //テキストの受け取り
    client_id = get_chat();

    //テキストの送信
    send_chat(client_id);
    rnbool = 0;
  }
}
//クライアントのログイン関数
static int client_login(int sock_listen)
{
  int client_id,sock_accepted;
  static char prompt[]=" Input your name: ";
  char loginname[NAMELENGTH];
  int strsize;

  for( client_id=0; client_id<N_client; client_id++){
    /* クライアントの接続を受け付ける */
    sock_accepted = Accept(sock_listen, NULL, NULL);
    printf("Client[%d] connected.\n",client_id);

    /* ログインプロンプトを送信 */
    Send(sock_accepted, prompt, strlen(prompt), 0);

    /* ログイン名を受信 */
    strsize = Recv(sock_accepted, loginname, NAMELENGTH-1, 0);
    loginname[strsize] = '\0';
    chop_nl(loginname);

    /* ユーザ情報を保存 */
    Client[client_id].sock = sock_accepted;
    strncpy(Client[client_id].name, loginname, NAMELENGTH);
  }

  return(sock_accepted);

}
//クライアントから送られてきたテキストを受信する関数
static int get_chat()
{
  fd_set mask, readfds;
  int client_id;
  int answered;
  int strsize;
  int chatclientid;
  /* ビットマスクの準備 */
  FD_ZERO(&mask);
  for(client_id=0; client_id<N_client; client_id++){
    FD_SET(Client[client_id].sock, &mask);
  }
  readfds = mask;
  select( Max_sd+1, &readfds, NULL, NULL, NULL );
  for( client_id=0; client_id<N_client; client_id++ ){
    if( FD_ISSET(Client[client_id].sock, &readfds) ){
        printf("__________________________\n");
        memset(GetBuf,0,sizeof(GetBuf));
	      Recv(Client[client_id].sock, GetBuf, BUFLEN-1,0);
        printf("%d\n",strcmp(GetBuf,"\n")  );
        if(strcmp(GetBuf,"\n") == 0){
          printf("改行コード\n" );
          rnbool = 1;
          break;
        }
        printf("%s\n", Client[client_id].name);
        printf("%s\n", GetBuf);
        snprintf(Buf,BUFLEN,"[%s]%s", Client[client_id].name,GetBuf);
        printf("%s\n", Buf);
        strsize = strlen(Buf);
        printf("%d\n", strsize);


	      Buf[strsize]='\0';
        chatclientid = client_id;
        printf("chatclientid : %d\n", chatclientid);
    }
  }
  return chatclientid;
}

//クライアントに対してテキストを送信する．テキストを送ってきたクライアントに対してはテキストの先頭に's'を付け足し，それ以外のクライアントには'o'を付け足す．
static void send_chat(int speakid){
  int len;
  len = strlen(Buf);

  char speakBuf[BUFLEN];
  char otherBuf[BUFLEN];
  if(rnbool == 0){
    for(int client_id=0; client_id<N_client; client_id++){
      if(client_id == speakid)
      {
        printf("client_id SPEAK: %d \n", client_id);
        snprintf(speakBuf,BUFLEN,"%s%s","s",Buf);
        printf("speakBuf: %s\n", speakBuf);
        Send(Client[client_id].sock, speakBuf, len, 0);
        memset(speakBuf,0,sizeof(speakBuf));
      }
      else
      {
        printf("client_id OTHER: %d \n", client_id);
        snprintf(otherBuf,BUFLEN,"%s%s","o",Buf);
        printf("otherBuf: %s\n", otherBuf);
        Send(Client[client_id].sock, otherBuf, len, 0);
        memset(otherBuf,0,sizeof(otherBuf));
      }
    }
  }
}

static char *chop_nl(char *s)
{
  int len;
  len = strlen(s);
  if( s[len-1] == '\n' ){
    s[len-1] = '\0';
  }
  return(s);
}
