#pragma once


void err_quit(char* msg);
void err_display(char* msg);

int recvn(SOCKET s, char* buf, int len, int flags);


/// <summary>
/// 데이터를 업데이트한다.
/// </summary>
void UpdateServer();



extern int g_clientCnt;