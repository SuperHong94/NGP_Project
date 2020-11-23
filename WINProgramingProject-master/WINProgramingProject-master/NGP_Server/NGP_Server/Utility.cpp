#include "stdafx.h"
#include "Utility.h"

void err_quit(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

// 소켓 함수 오류 출력
void err_display(char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}


//사용자 정의 데이터 수신 함수
int recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;
	char* ptr = buf;
	int left = len;

	while (left > 0) {
		received = recv(s, ptr, left, flags);

		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;

	}
	return(len - left);
}


//파일 받기
int recvf(SOCKET s, char* fname, char* buf, int len, int flags)
{
	int index = g_clientCnt;
	int received;
	char* ptr = buf;
	int left = len;
	int remain = 0;
	int percent = len;
	while (left > 0) {
		received = recv(s, ptr, 10, flags);

		if (received == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (received == 0)
			break;
		left -= received;
		ptr += received;
		remain += received;
		int result = (float)remain * 100 / (float)percent;


		g_pers[index] = ' ';
		g_pers[index] = fname;
		g_pers[index] += "진행률: ";
		g_pers[index] += to_string(result);
		g_pers[index] += "% \t";
		string r;
		/*for (int i = 0; i < g_clientCnt + 1; i++)
			r += g_pers[i];*/
		r += g_pers[1];
		r += g_pers[2];
		EnterCriticalSection(&cs);
		cout << r << '\r';
		LeaveCriticalSection(&cs);

	}
	return(len - left);
}



