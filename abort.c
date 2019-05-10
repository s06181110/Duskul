/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
#include <stdio.h>
#include <stdlib.h>
#include "getitem.h"
#include "abort.h"

/* defined in errmessages.c */
const char *getErrorMessage(const char *key);  //errmessages.cに実態があり、エラーメッセージを返す

//abortMessage_string=メッセージを出力する関数
//msg=エラー内容、str=エラー箇所等
static void abortMessage_string(const char *msg, const char *str)
{
    //エラーメッセージを出力した後、エラー箇所などを出力したければ出力
    fprintf(stderr, "ERROR: %s", getErrorMessage(msg));
    if (str)　　
        fprintf(stderr, " %s", str);
    fputc('\n', stderr);
    if (stdtxin) {
        printTextBuffer(stderr, stdtxin);  //printTextBuffer=nextch.cにある
        freeTextBuffer(stdtxin);  //freeTextBuffer=nextch.cとmain.cにある
        stdtxin = NULL;  //stdtxinをNULLにする
    }
    exit(1); // === EXIT THE PROGRAM ===  //プログラムを閉じる。1が引数の時は強制終了、0なら正常終了
}

void abortMessage(const char *msg) {  //出力したいエラーメッセージをそのまま出力
    abortMessage_string(msg, NULL);
}

void abortMessageWithToken(const char *msg, const item *s)  //変数sはgetItem関数で定義されている
{
    char buffer[80];
    itemToString(buffer, s);  //itemToString=printitem.cに実態がある
    abortMessage_string(msg, buffer);  //strにbufferの内容を入れる
}

void abortMessageWithString(const char *msg, const char *str)
{
    char buffer[40];
    sprintf(buffer, "'%s'", shorten(str, 32));  //buffer配列にshorten(str, 32)を入れる
    abortMessage_string(msg, buffer);  //strにshorten(str, 32)の内容を入れる
}

#if defined(DEBUG)
void _assert(int cond, const char *fn, int line)
{
    if (!cond) {
        fprintf(stderr, "Assertion Error: %s (%d)\n", fn, line);
        exit(1);
    }
}
#endif
