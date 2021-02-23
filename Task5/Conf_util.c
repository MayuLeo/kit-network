#include "mynet.h"
#include <stdlib.h>
#include <sys/select.h>

#define L_USERNAME 15
/* 各クライアントのユーザ情報を格納する構造体の定義 */
typedef struct _imember {
  char username[L_USERNAME];     /* ユーザ名 */
  int  sock;                     /* ソケット番号 */
  struct _imember *next;        /* 次のユーザ */
} imember;

imember Client;