#pragma once


void err_quit(char* msg);
void err_display(char* msg);

int recvn(SOCKET s, char* buf, int len, int flags);
int recvf(SOCKET s, char* fname, char* buf, int len, int flags);

/// <summary>
/// �����͸� ������Ʈ�Ѵ�.
/// </summary>
void UpdateServer();


DWORD WINAPI ProcessClient(LPVOID arg);
