#include "Conf.h"
#include "mynet.h"
#include <stdlib.h>
#include <pthread.h>
#include <sys/select.h>
#define HELLO   1
#define HERE    2
#define JOIN    3
#define POST    4
#define MESSAGE 5
#define QUIT    6

#define BUFSIZE 512   /* バッファサイズ */
struct idobata {
  char header[4];   /* パケットのヘッダ部(4バイト) */
  char sep;         /* セパレータ(空白、またはゼロ) */
  char data[];      /* データ部分(メッセージ本体) */
};
#define L_USERNAME 15
/* 各クライアントのユーザ情報を格納する構造体の定義 */
typedef struct _imember
{
  char username[L_USERNAME]; /* ユーザ名 */
  int sock;                  /* ソケット番号 */
  struct _imember *next;     /* 次のユーザ */
} imember;

imember Client;
static imember * Client_login(int sock_listen);
static void Client_logout(int sock);
static void * Conf_thread();
u_int32_t analyze_header(char *header);
static char *chop_nl(char *s);
static void UserList();

void Conf_server(int port_number, char *username) //サーバー処理
{
  printf("Server\n");
  int *tharg;
  pthread_t tid;
  printf("Serverport:%d\n",port_number);
  /* スレッド関数の引数を用意する */
  if( (tharg = (int *)malloc(sizeof(int)))==NULL ){
    exit_errmesg("malloc()");
  }
  *tharg = port_number;
  /* スレッドを生成する */
  if( pthread_create(&tid, NULL, Conf_thread, (void *)tharg) != 0 ){
    exit_errmesg("pthread_create()");
  }

  char buf[BUFSIZE],s_buf[BUFSIZE];
  int MaxFDS;
  fd_set mask, readfds;

  //TCP処理
  int sock_listen,sock_accepted;
  //サーバーの初期化
  sock_listen = init_tcpserver(port_number,5);
  MaxFDS = sock_listen;
  /* ビットマスクの準備 */
  FD_ZERO(&mask);
  FD_SET(0,&mask);
  FD_SET(sock_listen, &mask);
  
  while(1)
  {
    readfds = mask;
    select(MaxFDS + 1,&readfds,NULL,NULL,NULL);
    //imember *p;
    imember *next;
    imember *prev;
    if(FD_ISSET(0,&readfds))//標準入力
    {
      memset(buf, 0, sizeof(buf));
      memset(s_buf, 0, sizeof(s_buf));
      fgets(buf, BUFSIZE, stdin);
      chop_nl(buf);
      if(strcmp(buf,"USER_LIST") == 0)
      {
        UserList();
      }
      else{
        imember *client = Client.next;
        for (; client != NULL; client = client->next)
        {
          snprintf(s_buf, BUFSIZE, "MESG [%s]%s", username, buf);
          Send(client->sock, s_buf, strlen(s_buf), 0);
        }
      }
    }
    if(FD_ISSET(sock_listen,&readfds))//初回ログイン
    {
      printf("初回ログイン\n");
      imember *login = Client_login(sock_listen);
      



      FD_SET(login->sock,&mask);
      printf("login->sock:%d\n",login->sock);
      if(MaxFDS < login->sock)
      {
        MaxFDS = login->sock;
        readfds = mask;
        select(MaxFDS + 1, &readfds, NULL, NULL, NULL);
      }
      //##################################################
    }

    //パケットの受信
    
    int count = 0;
    printf("AA\n");
    imember *p = Client.next;
    printf("AA\n");
    for (; p != NULL; p = p->next)
    {
      printf("クライアントごとの処理\n");
      struct idobata *packet;
      memset(buf, 0, sizeof(buf));
      memset(s_buf, 0, sizeof(s_buf));
      count++;
      printf("sockNumber :%d\n",p->sock);
      printf("sockUsername :%s\n",p->username);
      
      if(FD_ISSET(p->sock,&readfds))
      {
        int strsize = Recv(p->sock,buf,BUFSIZE-1,0);

        if(strcmp(buf,"") == 0)
        {
          continue;
        }
        packet = (struct idobata *)buf; /* packetがバッファの先頭を指すようにする */
        printf("Recv :%s\n",buf);
        imember *pos = Client.next;
        switch (analyze_header(packet->header))
        { /* ヘッダに応じて分岐 */

        case HELLO:
          /* HELOパケットを受けとった時の処理 */
          break;
        case JOIN:
          chop_nl(packet->data); /* Usernameに改行があれば除く */
          //先頭から辿っていって，sockと今受け取ったp->sockが同じところのusernameに代入
          for (next = Client.next; next != NULL; next = next->next)
          {
            if(next->sock == p->sock)
            {
              //next->username = packet->data;
              snprintf(next->username,15,"%s",packet->data);
              break;
            }
          }
          break;
        case POST:
           printf("[%s]\n",buf);
          
          for (; pos != NULL; pos = pos->next)
          {
            if(pos->sock != p->sock)
            {
              snprintf(s_buf,BUFSIZE,"MESG [%s]%s",p->username,packet->data);
              Send(pos->sock, s_buf, strlen(s_buf), 0);
              printf("CASEPOST::::Send :%s\n",s_buf);
            }
          }
          printf("[%s]%s\n",p->username,packet->data);
          break;
        case QUIT:
          printf("QUIT 処理\n");
          // printf("[%s]\n",buf);
          //int sockacc = p->sock;
          FD_CLR(p->sock,&mask);
          printf("A\n");
          close(p->sock);
          printf("B\n");
          Client_logout(p->sock);
          printf("C\n");

          break;
          
          /* 以下、省略 */
        }
      }
    }
    // printf("Client num :%d\n",count);
    // printf("#####################\n");
    // printf("Username:sock\n");
    //現在ログイン中のユーザーの一覧
    // for (imember *p = Client.next; p != NULL; p = p->next)
    // {
    //   printf("%s:%d\n",p->username,p->sock);
    // }
    // printf("#####################\n");
    //for(リスト先頭から末尾まで)
    //FD_ISSET(先頭から末尾まで)



    //JOIN

  //ログイン完了しているクライアントから、「POST 発言メッセージ」を受信したら他のクライアントに「MESG 発言メッセージ」 のメッセージを送信する。サーバー自身の画面には「発言メッセージ」を表示する。なお、発言メッセージの先頭部分には、サーバ側で、発言者のusernameを [username]の形で付加するものとする。発言メッセージの長さは488バイト以内とする。

  //if(QUITを受け取ったら)
    //接続を閉じて接続情報を消す．
  
  //サーバーのキーボードから入力したら他のクライアントに送信する．
  }
  
  

}
static imember * Client_login(int sock_listen)
{
  imember *p;
  imember *next;
  imember *prev;
  /* クライアントの接続を受け付ける *///ここをSelectできるといい？
  int sock_accepted = accept(sock_listen, NULL, NULL);
  //printf("accepted\n");
  //###############################################
  p = (imember *)malloc(sizeof(imember));
  //char namee[64] = "Aaaa";
  //strncpy(p->username, namee, 64);
  p->sock = sock_accepted;
  p->next = NULL;
  prev = &Client;
  for (next = Client.next; next != NULL; next = next->next)
  {
    prev = next;
  }
  prev->next = p;//JOINが送信されたらsock_acceptと同じimemberを探して代入
  //FD_SET(sock_accepted, &mask);
  //printf("sock_accepted :%d\n",sock_accepted);
  //printf("p->sock :%d\n",p->sock);
  return p;
}
static void Client_logout(int sock)
{
  // printf("logoutFlow\n");
  imember *prev;
  prev = &Client;
  imember *p = Client.next;
  for (; p != NULL; p = p->next)
  {
    if (p->sock == sock)
    {
      if (p->next != NULL)
      {
        prev->next = p->next;
        free(p);
        return;
      }
      prev->next = NULL;
      free(p);
      return;
    }
    prev = p;
  }
}
static void UserList()
{
  printf("#####ログイン中のユーザー一覧#####\n");
  imember *p;
  for (*p = *Client.next; p != NULL; p = p->next)
  {
    printf("%s:%d\n",p->username,p->sock);
  }
  printf("##############################\n");
}
u_int32_t analyze_header(char *header)
{
  if (strncmp(header, "HELO", 4) == 0)
    return (HELLO);
  if (strncmp(header, "HERE", 4) == 0)
    return (HERE);
  if (strncmp(header, "JOIN", 4) == 0)
    return (JOIN);
  if (strncmp(header, "POST", 4) == 0)
    return (POST);
  if (strncmp(header, "MESG", 4) == 0)
    return (MESSAGE);
  if (strncmp(header, "QUIT", 4) == 0)
    return (QUIT);
  return 0;
}
static void * Conf_thread(void *arg)
{
  // printf("Thread!!\n");
  pthread_detach(pthread_self()); // スレッドの分離(終了を待たない) 

  struct sockaddr_in broadcast_adrs;
  struct sockaddr_in from_adrs;
  in_port_t port_number;
  int sock;
  int broadcast_sw=1;
  int strsize;
  socklen_t from_len;
  char buf[BUFSIZE];
  port_number = *((in_port_t *)arg);
  //set_sockaddr_in(&broadcast_adrs, "255.255.255.255", (in_port_t)port_number);
  
  // printf("ThreadPort:%d\n",port_number);

  /* ブロードキャストアドレスの情報をsockaddr_in構造体に格納する */
  memset(&broadcast_adrs, 0, sizeof(broadcast_adrs));
  broadcast_adrs.sin_family = AF_INET;
  broadcast_adrs.sin_port = htons(port_number);
  broadcast_adrs.sin_addr.s_addr = htonl(INADDR_ANY);

  /* ソケットをDGRAMモードで作成する */
  if((sock = socket(PF_INET, SOCK_DGRAM, 0)) == -1){
    exit_errmesg("socket()");
  }

  /* ソケットをブロードキャスト可能にする 
  if(setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&broadcast_sw, sizeof(broadcast_sw)) == -1){
    exit_errmesg("setsockopt()");
  }*/
  /* ソケットに自分自身のアドレス情報を結びつける */
  if(bind(sock, (struct sockaddr *)&broadcast_adrs, sizeof(broadcast_adrs)) == -1 ){
    exit_errmesg("bind()");
  }

  while(1){//Selectした方がいいかも？

    /* 文字列をクライアントから受信する */
    from_len = sizeof(from_adrs);
    if((strsize=recvfrom(sock, buf, BUFSIZE, 0,
			 (struct sockaddr *)&from_adrs, &from_len)) == -1){
      exit_errmesg("recvfrom()");
    }

    if(strcmp(buf,"HELO") == 0)//受け取ったbufがHELOなら
    {
      // HELEをクライアントに送信する 
      if(sendto(sock, "HELE", strsize, 0,
          (struct sockaddr *)&from_adrs, sizeof(from_adrs)) == -1 ){
        exit_errmesg("sendto()");
      }
    }
  }
  
  return (NULL);
  //UDPポートを監視し「HELO」パケットが送られてきたら送り返す．IPアドレスとユーザー名を保持する
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
