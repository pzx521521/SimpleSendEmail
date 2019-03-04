//C语言实现简单的SMTP发送邮件
#include "stdafx.h"
#include <windows.h>
#include <stdio.h>
#include <WinSock.h>
#include <iostream>
#include<string>
using namespace std;


/*
#pragma是一个预处理指令，会把后面的值传给编译器。
这个预处理指令是用于向编译器提供窗外信息的标准方法。
对于这个指令C标准中没有明确规定，所以不同编译器实现是不同的。
同一个#pragma命令，换一个编译器可能完全没有意义或者是不同意义。
格式一般为: #pragma Para。其中Para 为参数。
*/
#pragma  comment(lib, "ws2_32.lib")
//解释:http://blog.csdn.net/CL18652469346/article/details/53199167


struct Base64Date6
{
	unsigned int d4 : 6;
	unsigned int d3 : 6;
	unsigned int d2 : 6;
	unsigned int d1 : 6;
};

// 协议中加密部分使用的是base64方法
char ConvertToBase64(char   c6);
void EncodeBase64(char   *dbuf, char *buf128, int len);
void SendMail(char *account, char *password, char *email, char *body);
int  OpenSocket(struct sockaddr *addr);

int main(int argc, _TCHAR* argv[])
{
	//-a Account 发送邮件的账号
	//-p Password 发送邮件的密码
	//-s 给谁
	//-t thema 邮件主题
	//-c content 邮件内容
	char *paramArr[5] = { "-a", "-p", "-s", "-t", "-c" };
	char account[1500] = { 0 };
	char password[1500] = { 0 };
	char towho[1500] = { 0 };
	char thema[1500] = { 0 };
	char content[1500] = { 0 };
	for (size_t i = 1; i < argc; i++)
	{
		if (i + 1 > argc){
			break;
		}
		if (strcmp(argv[i], paramArr[0]) == 0){
			sprintf_s(account, 1500, (argv[i + 1]));
		}
		else if (strcmp(argv[i], paramArr[1]) == 0){
			sprintf_s(password, 1500, (argv[i + 1]));
		}
		else if (strcmp(argv[i], paramArr[2]) == 0){
			sprintf_s(towho, 1500, (argv[i + 1]));
		}
		else if (strcmp(argv[i], paramArr[3]) == 0){
			sprintf_s(thema, 1500, (argv[i + 1]));
		}
		else if (strcmp(argv[i], paramArr[4]) == 0){
			sprintf_s(content, 1500, (argv[i + 1]));
		}
		i++;
	};
	char EmailContents[1500] = { 0 };
	sprintf_s(EmailContents, 1500, "%s%s%s%s%s%s%s%s", "From: \"para\"<", account, 
		">\r\nTo: \"dasiy\"<", towho, ">\r\nSubject: ", thema, "\r\n\r\n", content);
	SendMail(account, password, towho, EmailContents);
	system("pause");
	return 0;
}

char ConvertToBase64(char uc)
{
	if (uc < 26)
	{
		return 'A' + uc;
	}
	if (uc < 52)
	{
		return 'a' + (uc - 26);
	}
	if (uc < 62)
	{
		return '0' + (uc - 52);
	}
	if (uc == 62)
	{
		return '+';
	}
	return '/';
}

// base64的实现
void EncodeBase64(char *dbuf, char *buf128, int len)
{
	struct Base64Date6 *ddd = NULL;
	int           i = 0;
	char          buf[256] = { 0 };
	char          *tmp = NULL;
	char          cc = '\0';

	memset(buf, 0, 256);
	strcpy_s(buf, 256, buf128);
	for (i = 1; i <= len / 3; i++)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
		dbuf[(i - 1) * 4 + 3] = ConvertToBase64((unsigned int)ddd->d4);
	}
	if (len % 3 == 1)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = '=';
		dbuf[(i - 1) * 4 + 3] = '=';
	}
	if (len % 3 == 2)
	{
		tmp = buf + (i - 1) * 3;
		cc = tmp[2];
		tmp[2] = tmp[0];
		tmp[0] = cc;
		ddd = (struct Base64Date6 *)tmp;
		dbuf[(i - 1) * 4 + 0] = ConvertToBase64((unsigned int)ddd->d1);
		dbuf[(i - 1) * 4 + 1] = ConvertToBase64((unsigned int)ddd->d2);
		dbuf[(i - 1) * 4 + 2] = ConvertToBase64((unsigned int)ddd->d3);
		dbuf[(i - 1) * 4 + 3] = '=';
	}
	return;
}

// 发送邮件account, password, towho, EmailContents
void SendMail(char *account, char *password, char *email, char *body)
{
	int     sockfd = { 0 };
	char    buf[1500] = { 0 };
	char    rbuf[1500] = { 0 };
	char    login[128] = { 0 };
	char    pass[128] = { 0 };
	string host(account);
	host = host.substr(host.find("@")+ 1, host.size() - host.find("@"));
	host = "smtp." + host;
	const char *hostname = host.c_str();
	WSADATA WSAData;
	struct sockaddr_in their_addr = { 0 };
	WSAStartup(MAKEWORD(2, 2), &WSAData);
	memset(&their_addr, 0, sizeof(their_addr));

	their_addr.sin_family = AF_INET;
	their_addr.sin_port = htons(25);
	hostent* hptr = gethostbyname(hostname);
	memcpy(&their_addr.sin_addr.S_un.S_addr, hptr->h_addr_list[0], hptr->h_length);
	printf("IP of smpt.qq.com is : %d:%d:%d:%d\n",
		their_addr.sin_addr.S_un.S_un_b.s_b1,
		their_addr.sin_addr.S_un.S_un_b.s_b2,
		their_addr.sin_addr.S_un.S_un_b.s_b3,
		their_addr.sin_addr.S_un.S_un_b.s_b4);

	// 连接邮件服务器，如果连接后没有响应，则2 秒后重新连接
	sockfd = OpenSocket((struct sockaddr *)&their_addr);
	memset(rbuf, 0, 1500);
	while (recv(sockfd, rbuf, 1500, 0) == 0)
	{
		cout << "reconnect..." << endl;
		Sleep(2);
		sockfd = OpenSocket((struct sockaddr *)&their_addr);
		memset(rbuf, 0, 1500);
	}

	cout << rbuf << endl;

	// EHLO
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "EHLO HYL-PC\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "EHLO REceive: " << rbuf << endl;

	// AUTH LOGIN
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "AUTH LOGIN\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Auth Login Receive: " << rbuf << endl;

	// USER
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, account);//你的邮箱账号
	memset(login, 0, 128);
	EncodeBase64(login, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", login);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 UserName: " << buf << endl;
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "User Login Receive: " << rbuf << endl;

	// PASSWORD
	sprintf_s(buf, 1500, password);//你的邮箱密码
	memset(pass, 0, 128);
	EncodeBase64(pass, buf, strlen(buf));
	sprintf_s(buf, 1500, "%s\r\n", pass);
	send(sockfd, buf, strlen(buf), 0);
	cout << "Base64 Password: " << buf << endl;

	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Password Receive: " << rbuf << endl;

	// MAIL FROM
	memset(buf, 0, 1500);
	sprintf_s(buf, 1500, "%s%s%s","MAIL FROM: <", account, ">\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "set Mail From Receive: " << rbuf << endl;

	// RCPT TO 第一个收件人
	sprintf_s(buf, 1500, "RCPT TO:<%s>\r\n", email);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Tell Sendto Receive: " << rbuf << endl;

	// DATA 准备开始发送邮件内容
	sprintf_s(buf, 1500, "DATA\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Prepare Receive: " << rbuf << endl;

	// 发送邮件内容，\r\n.\r\n内容结束标记
	sprintf_s(buf, 1500, "%s\r\n.\r\n", body);
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Send Mail Receive: " << rbuf << endl;

	// QUIT
	sprintf_s(buf, 1500, "QUIT\r\n");
	send(sockfd, buf, strlen(buf), 0);
	memset(rbuf, 0, 1500);
	recv(sockfd, rbuf, 1500, 0);
	cout << "Quit Receive: " << rbuf << endl;

	//清理工作
	closesocket(sockfd);
	WSACleanup();
	return;
}
// 打开TCP Socket连接
int OpenSocket(struct sockaddr *addr)
{
	int sockfd = 0;
	sockfd = socket(PF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
	{
		cout << "Open sockfd(TCP) error!" << endl;
		exit(-1);
	}
	if (connect(sockfd, addr, sizeof(struct sockaddr)) < 0)
	{
		cout << "Connect sockfd(TCP) error!" << endl;
		exit(-1);
	}
	return sockfd;
}