#pragma once


void err_quit(char* msg);
void err_display(char* msg);

int recvn(SOCKET s, char* buf, int len, int flags);


/// <summary>
/// �����͸� ������Ʈ�Ѵ�.
/// </summary>
void UpdateServer();



extern int g_clientCnt;