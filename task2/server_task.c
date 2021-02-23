/*
ポートを閉じたあと，同じポートをすぐ開くとbind()エラーが発生するため，init_tcpserverのsocket()関数の直後に
const int one = 1;
setsockopt(sock_listen, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int));
を追加しています．

passは"kit"と設定しています．
*/
#include "mynet.h"
#include <dirent.h>
#define BUFSIZE 100000   /* バッファサイズ */

int main(int argc,char *argv[])
{
  int PORT = atoi(argv[1]);
  if(argv[1] == NULL)//コマンドライン引数でポートを指定しない場合終了する
    exit_errmesg("error");
  int sock_listen, sock_accepted;

  char buf[BUFSIZE];
  char getcomm[BUFSIZE];
  char getpass[BUFSIZE];
  int strsize;

  char *dirpath = strcat(getenv("HOME"),"/work/");//~/work/のディレクトのパスの文字列の生成．UNIX環境のみ
  char *list = "list\r\n";
  char *type = "type\r\n";
  char *exitcomm = "exit\r\n";
  char *txt = ">";
  char *pass = "pass :";

  char *password  = "kit\r\n";//パスワード

  do{//初回起動時のログイン

    close(sock_listen);
    close(sock_accepted);
    memset(getpass,0,sizeof(getpass));
    sock_listen = init_tcpserver(PORT,5);

    /* クライアントの接続を受け付ける */
    sock_accepted = accept(sock_listen, NULL, NULL);

    //パスワード入力受付
    if(send(sock_accepted, pass, strlen(pass), 0) == -1 ){
      exit_errmesg("send()");
    }
    if((strsize=recv(sock_accepted, getpass, BUFSIZE, 0)) == -1){
      exit_errmesg("recv()");
    }
  }while(strcmp(getpass,password) != 0);//入力されたパスワードと設定したパスワードが一致するまでループ

  while(1){
    if(send(sock_accepted, txt, 1, 0) == -1 )//>の送信
      exit_errmesg("send()");
    DIR *dir; //ディレクトリ
    FILE *fp; //ファイル
    char txt_con[BUFSIZE];//ファイルの中身
    if((dir = opendir(dirpath)) == NULL)//ディレクトリオープン
      exit_errmesg("opendir");
    struct dirent *dp;

    // 文字列をクライアントから受信する
    if((strsize=recv(sock_accepted, getcomm, BUFSIZE, 0)) == -1){
      exit_errmesg("recv()");
    }

    //#########################listコマンドを受け取った時の処理
    if(strcmp(getcomm,list) == 0)
    {
      for(dp = readdir(dir);dp != NULL;dp = readdir(dir)){
        sprintf(buf,"%s%s\n",buf,dp->d_name);
      }
    }

    //#########################typeコマンドを受け取った時の処理
    else if((strstr(getcomm,"type") != NULL) && (strcmp(getcomm,"type\r\n") != 0))
    {
      char *fname;
      fname = strtok(getcomm," ");//空白文字で文字列を区切る
      fname = strtok(NULL," ");//ファイル名取得
      int fname_Length = (int)strlen(fname) - 2;//末尾2文字を削除するため-2
      char *str;
      str = (char*)malloc(sizeof(char) * fname_Length);
      if(str == NULL)exit(0);
      for (int i = 0;i < fname_Length;i++)
      {
        str[i] = *(fname + i);
      }//fnameで取得した文字列には末尾2文字に制御文字が入り正しく動作しない．そのためstr配列にその制御文字が入らない範囲でコピーする．
      int fileflag = 1;
      if(strstr(str,"/") != NULL){//"/"を含む文字列は許可しない
        fileflag = 0;
        sprintf(buf,"%s%s\n",buf,"You Can't see that file");
      }

      if(fileflag  == 1){
        char filepath[BUFSIZE];
        sprintf(filepath,"%s%s",dirpath,str);//ディレクトリのパスとファイル名を結合しファイルの絶対パスを取得している．
        if((fp = fopen(filepath,"r")) != NULL){
          while (fgets(txt_con,256,fp) != NULL) {//1行ずつ抜き出している
            sprintf(buf,"%s%s\n",buf,txt_con);
          }
        }
        else
        {
          sprintf(buf,"%s%s\n",buf,"this file is not found");
        }
      }
    }
    //#########################exitコマンドを受け取った時の処理
    else if(strcmp(getcomm,exitcomm) == 0)
    {
      do{
        close(sock_listen);//クローズする
        close(sock_accepted);
        memset(getpass,0,sizeof(getpass));

        //再び接続をまつ
        sock_listen = init_tcpserver(PORT,5);

        /* クライアントの接続を受け付ける */
        sock_accepted = accept(sock_listen, NULL, NULL);

        //パスワード入力受付
        if(send(sock_accepted, pass, strlen(pass), 0) == -1 ){//>の送信
          exit_errmesg("send()");
        }
        if((strsize=recv(sock_accepted, getpass, BUFSIZE, 0)) == -1){
          exit_errmesg("recv()");
        }
      }while(strcmp(getpass,password) != 0);
    }
    //#########################各コマンド以外の文字が入力されたら
    else
    {
      sprintf(buf,"%s%s\n",buf,"This command not found");
    }
      if(send(sock_accepted, buf, strlen(buf), 0) == -1 ){
        exit_errmesg("send()");
      }
      memset(getcomm,0,sizeof(getcomm));
      memset(buf,0,sizeof(buf));
      memset(txt_con,0,sizeof(txt_con));
      closedir(dir);
  }
  close(sock_listen);
  close(sock_accepted);
  exit(EXIT_SUCCESS);
}
