// OnlineJudge.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"



#include <winsock.h> //注意顺序，要放在mysql.h前
#include "include\mysql.h"//控制台项目中要在mysql.h之前include<winsock.h> 
#pragma comment(lib, "libmysql.lib")  //注意lib添加到“库目录”中，而不是“引用目录”中
#include<stdlib.h>
#include<io.h>
#include<string.h>
#include<windows.h>
#include<WinNT.h>
#include<time.h>
#include<strsafe.h>
#include<Psapi.h>
LPTSTR host = new char[50];
LPTSTR database = new char[50];
LPTSTR username = new char[50];
LPTSTR password = new char[50];
int port = 0;



void split(char* splitstr, char* str)
{
	if (str != NULL)
	{
		int i = 0;
		int k = 0;
		while (str[i] != '/'&&str[i] != '\0')
		{
			splitstr[i] = str[i];
			i++;
		}
		splitstr[i] = '\0';
		i++;
		while (str[i] != '\0')
		{
			str[k] = str[i];
			i++;
			k++;
		}
		str[k] = '\0';
	}
}
void trim(char trimstr[], char str[])
{
	int i = 0;
	int k = 0;
	while (str[i] != '\0')
	{
		if (str[i] != ' ')
		{
			trimstr[k] = str[i];
			k++;
		}
		i++;
	}
	trimstr[k] = '\0';
}
char *getfileall(char *fname)
{
	FILE *fp;
	char *str;
	char txt[1000];
	int filesize;
	if ((fp = fopen(fname, "r")) == NULL) {
		printf("打开文件%s错误\n", fname);
		return NULL;
	}

	fseek(fp, 0, SEEK_END);

	filesize = ftell(fp);
	str = (char *)malloc(filesize);
	str[0] = 0;

	rewind(fp);
	while ((fgets(txt, 1000, fp)) != NULL) {
		strcat(str, txt);
	}
	fclose(fp);
	return str;
}
//将代码写入文件
void writeCodeToFile(const char* fileName, char* text)
{
	FILE* fp;
	if (_access("user/code.c", 0) == 0)
		remove("user/code.c");                           //如果存在就删除
	if (_access("user/code.exe", 0) == 0)
		remove("user/code.exe");                         //如果存在就删除
	if (_access("user/in.txt", 0) == 0)
		remove("user/in.txt");                             //如果存在就删除
	if (_access("user/out.txt", 0) == 0)
		remove("user/out.txt");                             //如果存在就删除
	fp = fopen(fileName, "w");                         //创建
	fprintf(fp, "%s", text);                               //写入
	fclose(fp);                                          //关闭文件
														 //free(fp);
}
void writeInTempleToFile(const char* fileName, char* text)
{
	FILE* fp;
	if (_access("user/in.txt", 0) == 0)
		remove("user/in.txt");                             //如果存在就删除
	if (_access("user/out.txt", 0) == 0)
		remove("user/out.txt");                             //如果存在就删除
	fp = fopen(fileName, "w");                         //创建
	fprintf(fp, "%s", text);                               //写入
	fclose(fp);                                          //关闭文件
														 //free(fp);
}
//调用编译器
DWORD invokeGccCompiler()
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));
	TCHAR cmdline[] = TEXT("gcc -o C:\\OnlineJudge\\OnlineJudge\\user\\code C:\\OnlineJudge\\OnlineJudge\\user\\code.c");
	bool bol = CreateProcess(TEXT("C:\\gcc\\bin\\gcc.exe"), cmdline, NULL, NULL, false, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
	if (bol == FALSE)
	{
		printf("调用gcc编译器错误，请在C盘根目录下安装gcc编译器");
		system("pause");
		exit(0);
	}
	DWORD gccexitcode;
	WaitForSingleObject(pi.hProcess, INFINITE);        // 等待子进程退出 
	GetExitCodeProcess(pi.hProcess, &gccexitcode);
	CloseHandle(pi.hProcess);                          // 关闭句柄 
	CloseHandle(pi.hThread);
	return gccexitcode;
}
//创建沙箱
HANDLE createSandBox(int timeLimit, int memoryLimit)
{
	//创建一个作业
	HANDLE hJob = CreateJobObject(NULL, NULL); //
											   //基本限制结构体
	JOBOBJECT_BASIC_LIMIT_INFORMATION jobli = { 0 };
	//作业的优先级
	jobli.PriorityClass = IDLE_PRIORITY_CLASS;
	//作业的CPU时间限制
	jobli.PerJobUserTimeLimit.QuadPart = timeLimit * 10000; //timeLimit的单位是ms， 这里的单位是100ns，1ms=1000000ns
															//指明限制条件
	jobli.LimitFlags = JOB_OBJECT_LIMIT_PRIORITY_CLASS | JOB_OBJECT_LIMIT_JOB_TIME;

	//设定作业限制
	SetInformationJobObject(hJob, JobObjectBasicLimitInformation, &jobli, sizeof(jobli));
	//添加内存限制
	//JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobe = { 0 };
	//jobe.ProcessMemoryLimit = 1;
	//SetInformationJobObject(hJob, JOBOBJECT_EXTENDED_LIMIT_INFORMATION, &jobe, sizeof(jobe));

	//为作业添加一些基本UI限制
	//基本UI限制结构体
	JOBOBJECT_BASIC_UI_RESTRICTIONS jobuir;
	//初始无限制
	jobuir.UIRestrictionsClass = JOB_OBJECT_UILIMIT_NONE; //
														  //增加限制：作业（进程）不能注销操作系统
	jobuir.UIRestrictionsClass |= JOB_OBJECT_UILIMIT_EXITWINDOWS;
	//增加限制：作业（进程）不能访问 系统的用户对象（如其他窗口）
	jobuir.UIRestrictionsClass |= JOB_OBJECT_UILIMIT_HANDLES;
	//设定作业限制
	jobuir.UIRestrictionsClass |= JOB_OBJECT_UILIMIT_DISPLAYSETTINGS;
	jobuir.UIRestrictionsClass |= JOB_OBJECT_UILIMIT_SYSTEMPARAMETERS;
	jobuir.UIRestrictionsClass |= JOB_OBJECT_UILIMIT_HANDLES;
	//防止切换桌面
	jobuir.UIRestrictionsClass |= JOB_OBJECT_UILIMIT_DESKTOP;
	SetInformationJobObject(hJob, JobObjectBasicUIRestrictions, &jobuir, sizeof(jobuir));
	return hJob;
}
//测试数据库连接
void testDatabaseConnect()
{
	MYSQL *pConn;
	pConn = mysql_init(NULL);//第2、3、4、5参数的意思分别是：服务器地址、用户名、密码、数据库名，第6个为mysql端口号（0为默认值3306）

	if (!mysql_real_connect(pConn, host, username, password, database, port, NULL, 0))//连接失败
	{
		printf("无法连接数据库:%s", mysql_error(pConn));
		system("pause");
		exit(0);
	}
	mysql_query(pConn, "set names gbk");//防止乱码。设置和数据库的编码一致就不会乱码 数据库编码为utf-8；这里不知为何要设成gbk才能不乱码，源代码中不会出出现中文，所以乱码问题不会影响
	printf("数据库连接成功，开始判题...\n");
	mysql_close(pConn); //关闭连接
}
//执行更新语句
void updateDatebase(MYSQL *pConn, char* sql)
{
	if (mysql_query(pConn, sql))
	{
		printf("数据库更新失败:%s", mysql_error(pConn));
		mysql_rollback(pConn);//事务回滚
		mysql_close(pConn); //关闭连接
		system("pause");
		exit(-1);
	}
}
//执行查询语句
MYSQL_RES* selectDatabase(MYSQL* pConn, char* sql)
{
	if (mysql_query(pConn, sql))  //查询出一条记录
	{
		printf("查询数据库记录失败，请检测数据库连接:%s", mysql_error(pConn));
		mysql_rollback(pConn);//事务回滚
		mysql_close(pConn); //关闭连接
		system("pause");
		exit(-1);
	}
	return mysql_store_result(pConn);        //读取结果集
}
//更新日志文件
void updateLog(char* solutionId, char* string)
{
	time_t rawtime;
	struct tm * timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	FILE* fp;
	fp = fopen("log/log.txt", "ab");
	fprintf(fp, "题号：%s       %s      %s\r\n", solutionId, string, asctime(timeinfo));
	fclose(fp);
//	free(timeinfo);
	return;
}
//初始化数据库连接信息
void init()
{

	GetPrivateProfileString("database", "host", "", host, 50, ".\\database.ini");
	GetPrivateProfileString("database", "database", "", database, 50, ".\\database.ini");
	GetPrivateProfileString("database", "username", "", username, 50, ".\\database.ini");
	GetPrivateProfileString("database", "password", "", password, 50, ".\\database.ini");
	port = GetPrivateProfileInt("database", "port", 0, ".\\database.ini");

}




void start()
{
	char* tokStrings = (char*)malloc(500);
	MYSQL *pConn = NULL;
	char* solutionid = NULL;       //答题id
	char* code = NULL;             //答题代码
	char* questionid = NULL;       //所属题的id
	char* timelimit = NULL;        //时间限制
	char* memorylimit = NULL;      //内存限制
	char* blongExam = NULL;       //所属考试的id
	char* author = NULL;           //提交人
	char* in = NULL;               //标准输入
	char* out = NULL;              //标准输出
	char sql[250];
	pConn = mysql_init(NULL);
	MYSQL_RES* solutionResult = NULL;
	MYSQL_RES* questionResult = NULL;
	MYSQL_ROW solutionRow;
	MYSQL_ROW questionRow;
	mysql_options(pConn, MYSQL_INIT_COMMAND, "SET autocommit=0");//开启事务
	if (!mysql_real_connect(pConn, host, username, password, database, port, NULL, 0))
	{
		printf("无法连接数据库:%s", mysql_error(pConn));
		system("pause");
		mysql_close(pConn); //关闭连接
		exit(-1);
	}
	mysql_query(pConn, "set names gbk");

	strcpy(sql, "select * from solution where statu='waiting' limit 0,1");
	
	solutionResult = selectDatabase(pConn, sql);        //查询数据库                      
	if (solutionRow = mysql_fetch_row(solutionResult))            //存在未判题记录
	{
		
		solutionid = solutionRow[0];                                   //答题id
		code = solutionRow[1];                                         //答题代码
		author = solutionRow[3];                                       //提交者
		questionid = solutionRow[5];                                   //所属题的id
		blongExam = solutionRow[6];                                    //所属考试

//如果想支持多终端则放开这段注释																	   
//		strcpy(sql, "update solution set  statu='judging' where id=");
//		strcat(sql, solutionid);
//		updateDatebase(pConn,sql);
		mysql_commit(pConn);   //提交事务
		
		writeCodeToFile("user/code.c", code);                     //将代码写入code.c
		DWORD gccexitcode = invokeGccCompiler();   //调用gcc编译器		
		if (gccexitcode != 0)                       //编译失败
		{
			printf("编译错误\n");
			strcpy(sql, "update solution set statu='BuildErro' where id= ");
			strcat(sql, solutionid);
			updateDatebase(pConn, sql);//更新数据库
			mysql_commit(pConn);   //提交事务
			/*更新日志*/
			char string[10] = "编译失败";
			updateLog(solutionid, string);

		}
		else           //编译成功
		{
			strcpy(sql, "select * from question where id=");         //整合查询语句
			strcat(sql, questionid);
			questionResult = selectDatabase(pConn, sql);  //查询考题信息
			mysql_commit(pConn);   //提交
			if (questionRow = mysql_fetch_row(questionResult))                          //如果考题存在，开始将in写入文件
			{
				char* in = questionRow[3];                                                   //样例字符串
				char* out = questionRow[4];                                                  //样例输出字符串
				timelimit = questionRow[5];
				memorylimit = questionRow[6];
				char* stopString;
				long memorylimits = strtol(memorylimit, &stopString, 10) * 1024 * 8;
				//free(stopString);
				char* tokString;
				int flag = 0;                               //完全匹配与模糊匹配标识，0表示完全匹配，1表示模糊匹配
				int outOfMemoryFlag = 0;
				while ((tokString = strtok(in, "/")) != NULL)              //开始循环，一次测试一组数据
				{
					//printf("%d",&tokString);
					in = NULL;
					writeInTempleToFile("user\\in.txt", tokString);//写入输入文件in.txt
																   /*重定向输入输出流*/
					STARTUPINFO si;
					PROCESS_INFORMATION pi;
					ZeroMemory(&si, sizeof(si));
					si.cb = sizeof(si);
					ZeroMemory(&pi, sizeof(pi));
					SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES),NULL,TRUE };
					HANDLE cmdOutput = CreateFile("C:/OnlineJudge/OnlineJudge/user/out.txt",
						GENERIC_WRITE,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						&sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (cmdOutput == INVALID_HANDLE_VALUE)
					{
						printf("创建输出流出错，");
						system("pause");
						mysql_close(pConn); //关闭连接
						exit(-1);
					}

					HANDLE cmdInput = CreateFile("C:/OnlineJudge/OnlineJudge/user/in.txt",
						GENERIC_READ,
						FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
						&sa, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
					if (cmdInput == INVALID_HANDLE_VALUE)
					{
						printf("创建输入流出错，");
						system("pause");
						mysql_close(pConn); //关闭连接
						exit(-1);
					}
					si.hStdInput = cmdInput;
					si.hStdOutput = cmdOutput;
					si.dwFlags = STARTF_USESTDHANDLES;

					/********************************************创建沙箱*************************************************/

					//创建沙箱
					HANDLE hJob = createSandBox(atoi(timelimit), atoi(memorylimit));
					//创建进程
					bool bol = CreateProcess(TEXT("C:/OnlineJudge/OnlineJudge/user/code.exe"), NULL, NULL, NULL, true, CREATE_NO_WINDOW | CREATE_SUSPENDED, NULL, NULL, &si, &pi);

					if (bol == FALSE)
					{
						printf("创建子进程失败");
						system("pause");
						mysql_close(pConn); //关闭连接
						exit(-1);
					}
					//将进程添加到作业  
					AssignProcessToJobObject(hJob, pi.hProcess);
					//唤醒进程（的主线程）
					ResumeThread(pi.hThread);
					//等待进程结束或作业CPU时间耗完					
					HANDLE h[2];
					h[0] = pi.hProcess;
					h[1] = hJob;
					DWORD dwCode = WaitForMultipleObjects(2, h, FALSE, INFINITE);//等待子进程退出,单位为毫秒
					CloseHandle(cmdOutput);                               //关闭输入输出流
					CloseHandle(cmdInput);
					DWORD dwExitCode;
					GetExitCodeProcess(pi.hProcess, &dwExitCode);        //得到退出代码
					switch (dwCode - WAIT_OBJECT_0) {
					case 0:                                         //自动退出
					{	if (dwExitCode == 0)                        //正确结束               

					{

						PROCESS_MEMORY_COUNTERS memInfo;
						GetProcessMemoryInfo(pi.hProcess, &memInfo, sizeof(memInfo));
						long mem = memInfo.PeakWorkingSetSize;//运行时内存
						char filename[50] = "user\\out.txt";
						char* fname = (char *)malloc(50);;
						strcpy(fname, filename);
						char* outs = getfileall(fname);       //获取程序输出
						if(outs[strlen(outs)-1]==' ')
						{
							outs[strlen(outs) - 1] = '\0'; //如果最后是空格，切除最后以为空格
						}
						char* stdo = (char *)malloc(500);   //获取标准输出
						char* trimouts = (char *)malloc(500);       //获取去空格后程序输出
						char* trimstdo = (char *)malloc(500);   //获取去空格后标准输出
						split(stdo, out);                 //截取标准输出   
					//	printf("标准输出：%s",stdo);
					//	printf("程序剩余部分输出：%s", out);
					//	printf("程序输出：%s", outs);
						trim(trimouts, outs);   //去除空格
						trim(trimstdo, stdo);
					//	printf("标准输出:\n%s\n",stdo);
					//	printf("程序输出:\n%s\n", outs);
					//	printf("剪切后标准输出:\n%s", trimstdo);
					//	printf("剪切后程序输出:\n%s", trimouts);
						if (strcmp(outs, stdo) != 0 && strcmp(trimouts, trimstdo) == 0)                       //判断两个输出是否模糊相等
						{

							flag = 1;                           //模糊匹配标识  
							if (mem > memorylimits)  //超出运行时内存
							{

								outOfMemoryFlag = 1;
								free(trimouts);
								free(trimstdo);
								free(fname);
								free(outs);
								free(stdo);
								break;
								

							}
							else
							{                        //未超出内存
								free(trimouts);
								free(trimstdo);
								free(fname);
								free(outs);
								free(stdo);
								break;
							}
						}
						if (strcmp(out, stdo) != 0 && strcmp(trimouts, trimstdo) != 0)                  //程序不匹配
						{
							printf("算法错误\n");
							strcpy(sql, "update solution set statu = 'Erro' where id =");
							strcat(sql, solutionid);
							updateDatebase(pConn, sql);
							mysql_commit(pConn);   //提交
							char string[15] = "算法错误";
							updateLog(solutionid, string);
							CloseHandle(pi.hThread);
							CloseHandle(pi.hProcess);
							CloseHandle(hJob);
							mysql_free_result(solutionResult);    //释放结果集
							mysql_free_result(questionResult);    //释放结果集
							mysql_close(pConn);           //释放连接
							free(trimouts);
							free(trimstdo);
							free(fname);
							free(outs);
							free(stdo);

							return;

						}
						if (strcmp(outs, stdo) == 0)  //程序完全匹配
						{
							if (mem > memorylimits)  //超出运行时内存
							{

								outOfMemoryFlag = 1;
								free(trimouts);
								free(trimstdo);
								free(fname);
								free(outs);
								free(stdo);
								break;

							}
							else          //未超出内存
							{
								free(trimouts);
								free(trimstdo);
								free(fname);
								free(outs);
								free(stdo);
								break;
							}
						}
					}
					else                //程序被迫终止，发生运行时异常:除0错误
					{
						printf("运行时异常\n");
						strcpy(sql, "update solution set statu = 'RuntimeException' where id =");
						strcat(sql, solutionid);
						updateDatebase(pConn, sql);
						mysql_commit(pConn);   //提交
						char string[12] = "运行时异常";
						updateLog(solutionid, string);
						CloseHandle(pi.hThread);
						CloseHandle(pi.hProcess);
						CloseHandle(hJob);
						mysql_free_result(solutionResult);    //释放结果集
						mysql_free_result(questionResult);    //释放结果集
						mysql_close(pConn);           //释放连接

						return;
					}
					}//case:0 end
					case 1:  //时间片运行完 timeout异常，作业杀死进程
					{
						printf("运行超时\n");
						strcpy(sql, "update solution set statu = 'TimeoutException' where id =");
						strcat(sql, solutionid);
						updateDatebase(pConn, sql);
						mysql_commit(pConn);   //提交
						char string[12] = "运行超时";
						updateLog(solutionid, string);
						CloseHandle(pi.hThread);
						CloseHandle(pi.hProcess);
						CloseHandle(hJob);
						mysql_free_result(solutionResult);    //释放结果集
						mysql_free_result(questionResult);    //释放结果集
						mysql_close(pConn);           //释放连接

						return;



					}//case:1 end
					}
					CloseHandle(pi.hThread);
					CloseHandle(pi.hProcess);
					CloseHandle(hJob);


				}//程序执行完所有的输入案例while end


				if (flag == 0)
				{
					if (outOfMemoryFlag == 1)
					{
						printf("内存超出\n");
						strcpy(sql, "update solution set statu = 'OutOfMemory' where id =");
						strcat(sql, solutionid);
						updateDatebase(pConn, sql);
						mysql_commit(pConn);   //提交
						char string[15] = "超出限定内存";
						updateLog(solutionid, string);
					}
					else {
						printf("完全匹配\n");
						
						strcpy(sql, "update solution set statu='Accept' where id= ");
						strcat(sql, solutionid);
						updateDatebase(pConn, sql);//更新数据库
						char string[10] = "完全匹配";
						updateLog(solutionid, string);                        //更新日志
						strcpy(sql, "SELECT COUNT(*) FROM solution WHERE question_id="); //获取提交次数
						strcat(sql, questionid);
						strcat(sql, " AND blong_exam=");
						strcat(sql, blongExam);
						strcat(sql, " AND statu='Accept'");
						strcat(sql, " AND author=");
						strcat(sql, author);
						MYSQL_RES* AcceptCountResult = selectDatabase(pConn, sql);				        
						MYSQL_ROW AcceptCountRow;
						char* count = NULL;
						while (AcceptCountRow = mysql_fetch_row(AcceptCountResult))            //获取提交次数
						{
							count = AcceptCountRow[0];
						}
						mysql_free_result(AcceptCountResult);
						printf("此题该用户Accept的次数：%d\n", atoi(count));
						if (atoi(count) == 1)                         //第一次提交
						{
							strcpy(sql, "UPDATE question SET Accept=Accept+1 WHERE id=");
							strcat(sql, questionid);
							updateDatebase(pConn, sql);  //提交数+1
							strcpy(sql, "UPDATE sort SET accept=accept+1 WHERE exam_id=");
							strcat(sql, blongExam);
							strcat(sql, " AND student_id=");
							strcat(sql, author);
							updateDatebase(pConn, sql);//排名表+1

						}
						mysql_commit(pConn);
						
					}
				}
				else
				{
					printf("模糊匹配\n");
					strcpy(sql, "update solution set statu='LikeAccept' where id= ");
					strcat(sql, solutionid);
					updateDatebase(pConn, sql);//更新数据库
					mysql_commit(pConn);   //提交
					char string[10] = "模糊匹配";
					updateLog(solutionid, string);                        //更新日志
				}
			}
			}//编译成功end

		}/*一条答题记录end*/

		mysql_free_result(solutionResult);    //释放结果集
		mysql_free_result(questionResult);    //释放结果集
		mysql_close(pConn);           //释放连接..
	}

int main(int argc, char* argv[])
{
	init();//根据配置文件初始化数据库连接信息
	testDatabaseConnect();//测试数据库连接
	while(true)
	{
		start();
    }
	
	return 0;


}



