//mpMain.c
#include "motoPlus.h"

int  SetApplicationInfo();
void mpTask1();
void mpTask2();

//GLOBAL DATA DEFINITIONS
int nTaskID1;
int nTaskID2;



INT32 ReadPosition(int sock_id, INT32 n)
{
	INT32 msg = mpHtonl((INT32)n);//转换网络数据到可收发数据类型
	mpSend(sock_id, (char *)(&msg), sizeof(msg), 0);
	
	INT32 msg_recv;
	mpRecv(sock_id, (char *)(&msg_recv), sizeof(msg_recv), 0);
	msg_recv = mpNtohl(msg_recv);
	return msg_recv;
}

void mpUsrRoot(int arg1, int arg2, int arg3, int arg4, int arg5, int arg6, int arg7, int arg8, int arg9, int arg10)
{
	int rc;

	//TODO: Add additional intialization routines.

	//Creates and starts a new task in a seperate thread of execution.
	//All arguments will be passed to the new task if the function
	//prototype will accept them.
	nTaskID1 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)mpTask1,
						arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);
	nTaskID2 = mpCreateTask(MP_PRI_TIME_NORMAL, MP_STACK_SIZE, (FUNCPTR)mpTask2,
						arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10);

	//Set application information.
	rc = SetApplicationInfo();

	//Ends the initialization task.
	mpExitUsrRoot;
}

//Set application information.
int SetApplicationInfo(void)
{
	MP_APPINFO_SEND_DATA    sData;
	MP_STD_RSP_DATA         rData;
	int                     rc;

	memset(&sData, 0x00, sizeof(sData));
	memset(&rData, 0x00, sizeof(rData));

	strncpy(sData.AppName,  "Default Application",  MP_MAX_APP_NAME);
	strncpy(sData.Version,  "0.00",                 MP_MAX_APP_VERSION);
	strncpy(sData.Comment,  "MotoPlus Application", MP_MAX_APP_COMMENT);

	rc = mpApplicationInfoNotify(&sData, &rData);
	return rc;
}

void mpTask1(void)
{
	//Create socket
	LONG socket_id = mpSocket(AF_INET, SOCK_STREAM, 0);  // IPv4, TCP
	if(socket_id<0)
	{
		printf("Create socket error!");
		return;
	}
	// Connect to server
	struct sockaddr_in servAddr;
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = mpHtons(8081);
	servAddr.sin_addr.s_addr = mpInetAddr("192.168.10.14"); // Server IP
	
	int conn_st;
	while(1)
	{
		conn_st = mpConnect(socket_id, (struct sockaddr*)&servAddr, sizeof(servAddr));
		if( conn_st == -1)
		{
			printf("Connect error.\n");
			continue;
		}
		else
		{
			printf("Connect successful.\n");
			break;
		}
	}
	
	while(1)
	{
		printf("Start main function....\n");
		
		printf("Read Position....\n");
		INT32 pos = ReadPosition(socket_id, 1);//挂起
		printf("Read Position %d\n" ,pos);
		
		printf("Set Position....\n");
		MP_USR_VAR_INFO varInfo;
		varInfo.var_type = MP_VAR_B;
		varInfo.var_no = pos;
		varInfo.val.b = 1;
		mpPutUserVars(&varInfo);
		
		printf("Start Action....\n");
		MP_USR_VAR_INFO start;
		start.var_type = MP_VAR_B;
		start.var_no = 0;
		start.val.b = 1;
		mpPutUserVars(&start);
	}
	
}

void mpTask2(int arg1, int arg2)
{
	//TODO: Add the code for this task
}
