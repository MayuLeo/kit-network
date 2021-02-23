#ifndef CONF_H_
#define CONF_H_

#include "mynet.h"

/* サーバメインルーチン */
void Conf_server(int port_number, char *username);

/* クライアントメインルーチン */
void Conf_client(int port_number, char *SeverIPAddress,char *username);

/* Accept関数(エラー処理つき) */
int Accept(int s, struct sockaddr *addr, socklen_t *addrlen);

/* 送信関数(エラー処理つき) */
int Send(int s, void *buf, size_t len, int flags);

/* 受信関数(エラー処理つき) */
int Recv(int s, void *buf, size_t len, int flags);

#endif 
