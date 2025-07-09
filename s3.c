#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>
#include <mysql.h>


#define MAXDATASIZE 1024
#define PORT 3000
#define BACKLOG 5

#define HOST "localhost"
#define USERNAME "debian-sys-maint"
#define PASSWORD "yhlBhfOnTOm7kBr6"
#define DATABASE "Q_user"

MYSQL mysql; //mysql连接
MYSQL_FIELD *fd;  //字段列数组
char field[32][32];  //存字段名二维数组
MYSQL_RES *res; //这个结构代表返回行的一个查询结果集
MYSQL_ROW column; //一个行数据的类型安全(type-safe)的表示，表示数据行的列
char query[150]; //查询语句



bool Register(char new_name[20],char new_pw[32])
{
	memset(query, 0, sizeof query);
   	//strcpy(query, "insert into user values (new_name, new_pw,False,0);");  //注册
    	strcpy(query, "insert into user values ('"); 
	strcat(query,new_name);
	strcat(query,"',");
	strcat(query,"'");
	strcat(query,new_pw);
	strcat(query,"'");
	strcat(query,",0,0)");

    if (mysql_query(&mysql, query))        //执行SQL语句  
    {
        printf("Query failed \n");
        return false;
    }
    else
    {
        printf("Register success\n");
        return true;
    }
}

bool compare(char id[20],char pw[32])
{
    mysql_query(&mysql, "set names gbk");
	memset(query, 0, sizeof query);
	strcpy(query, "select pw from user where name='");
	strcat(query, id);
	strcat(query, "';");
    //返回0 查询成功，返回1查询失败  
    if (mysql_query(&mysql, query))        //执行SQL语句  
    {
        printf("Query failed (%s)\n", mysql_error(&mysql));
        return false;
    }
    else
    {
        printf("query success\n");
    }
    res = mysql_store_result(&mysql);
    
    int j = mysql_num_fields(res);  // 获取列数  

    column = mysql_fetch_row(res);//column[0]就是对应用户id的密码！！！！

    if(!strcmp(column[0],pw)){return 1;}	
    else{return 0;}
		
}

void Log(bool state,int sock,char name[20])//修改状态和套接字
{
	
	char s1[10],s2[10];
	memset(s1, 0, sizeof s1);
	memset(s2, 0, sizeof s2);
	
	//sprintf(s, "%d", 123);  //把整数123打印成一个字符串保存在s中
	sprintf(s1,"%d",state);
	sprintf(s2,"%d",sock);
	
	memset(query, 0, sizeof query);
	strcat(query,"update user set state=");
	strcat(query,s1);
	strcat(query,",data_sock=");
	strcat(query,s2);
	strcat(query," where name='");
	strcat(query,name);
	strcat(query,"';");
	//update user set state=s1,data_sock=s2 where name='s3';
    //strcpy(query, "update user set pw='09876543' where name='xx';");
    if (mysql_query(&mysql, query))        //执行SQL语句  
    {
        printf("Query failed (%s)\n", mysql_error(&mysql));

    }
    else
    {
        printf("Query success\n");

    }
}

void Online(char online[80])//在线人数查询,传来一个空的字符串数组，最终返回一个数组，第一个值是在线人数x，后面是id。online="x#a#b#c"
{//数组传过来的时候已经置空了
    
    int i=0;
    mysql_query(&mysql, "set names gbk");
    //返回0 查询成功，返回1查询失败  
    if (mysql_query(&mysql, "select name from user where state=1"))        //执行SQL语句  
    {
        printf("Query failed (%s)\n", mysql_error(&mysql));
    }
    else
    {
        printf("query success\n");
    }
    res = mysql_store_result(&mysql);
    
    char s[10];
    memset(s, 0, sizeof s);

	//sprintf(s, "%d", 123);  //把整数123打印成一个字符串保存在s中
	sprintf(s,"%d",mysql_affected_rows(&mysql));//这里是个数
    strcpy(online,s);

    while (column = mysql_fetch_row(res))
    {
    	
		strcat(online,"#");
        strcat(online,column[0]);
    }
	
    
}

bool Del(char new_name[20],char new_pw[32]){
	memset(query, 0, sizeof query);
    //strcpy(query, "delete from user where name='new_name';");  //注册
    strcpy(query, "delete from user where name='"); 
	strcat(query,new_name);
	strcat(query,"';");

    if (mysql_query(&mysql, query))        //执行SQL语句  
    {
        printf("Query failed \n");
        return false;
    }
    else
    {
        printf("Delete success\n");
        return true;
    }
}

//查询name为name的用户的套接字（0为不在线）
int sel_socket(char name[20])
{
	//select data_sock from user where name=name;
    mysql_query(&mysql, "set names gbk");
   	memset(query, 0, sizeof query);
	strcpy(query, "select data_sock from user where name='");
	strcat(query, name);
	strcat(query, "';");
    if (mysql_query(&mysql, query))        //执行SQL语句  
    {
        printf("Query failed (%s)\n", mysql_error(&mysql));
        return false;
    }
    else
    {
        printf("query success\n");
    }
    res = mysql_store_result(&mysql);
    column = mysql_fetch_row(res);
    if(atoi(column[0])!=0)	return atoi(column[0]);
    else	return 0;
    /*if(!strcmp(column[0],"1")) return true;
    else return false;*/
}


//查询数据:sock->name
void find(int sock,char name[20])//name传进来的时候已经置空了！
{
    mysql_query(&mysql, "set names gbk");
	//select name from user wherer data_sock=sock
    int u_id;
    char s[10];
    memset(s, 0, sizeof s);
    memset(query, 0, sizeof query);
    strcpy(query, "select name from user where data_sock=");
    sprintf(s, "%d", sock);
    strcat(query,s);
 
    if (mysql_query(&mysql,query))        //执行SQL语句  
    {
        printf("Query failed (%s)\n", mysql_error(&mysql));
        exit(1);
    }
    else
    {
        printf("query success\n");
    }
    res = mysql_store_result(&mysql);
    
    while (column = mysql_fetch_row(res))
    {
    	    strcpy(name,(column[0]));
    }
    
}


bool exist(char name[20])
{
	int num=0;
	mysql_query(&mysql, "set names gbk");
    memset(query, 0, sizeof query);
	//select count(*) from user where name = 'name';
	strcpy(query, "select count(*) from user where name='");
	strcat(query,name);
	strcat(query,"';");
    if (mysql_query(&mysql,query))   /*执行失败*/
    {
        printf("Query failed (%s)\n", mysql_error(&mysql));
        exit(1);
    }
    else     /*现在就代表执行成功了*/
    {
        printf("query success1\n");
    }
	/*将查询的結果给res*/
	res = mysql_store_result(&mysql);
    /*如果结果不为空*/
    column = mysql_fetch_row(res);
    num=atoi(column[0]);
    printf("column[0]=%d\n",num);
    	if (num>0)	return true;
	else	return false;
}

/*int timed_out;

void sig_handler(){
	timed_out=1;
}*/
//xiancheng bukeyi yong zhege xinhao 



void* ser(int sockfd){
	char buf[MAXDATASIZE];
	int nbytes;
	int count=0;
	int on=0;
	char temp[MAXDATASIZE];
	const char s[2] = "#";
	char name[20]={0};
	char pw[32]={0};
	char who_on[80]={0};
	char temp1[MAXDATASIZE]={0};
	char temp2[MAXDATASIZE]={0};
	char temp3[MAXDATASIZE]={0};
	char temp_msg[MAXDATASIZE]={0};
	char temp_name[MAXDATASIZE]={0};
	char*token;
	char*loc;
	
	int i;
	int ssize=0;
	
	
	/*struct sigaction act;
	act.sa_handler=sig_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags=0;
	sigaction(SIGALRM,&act,NULL);*/

	int res;
	MYSQL_RES *res_ptr;/*指向查询结果的指针*/
	mysql_init(&mysql); 
	
	if(!&mysql)  
	{  
		fprintf(stderr,"mysql_init failed\n");  
		return EXIT_FAILURE;  
	}  
  
	mysql_real_connect(&mysql,HOST,USERNAME,PASSWORD,DATABASE,0,NULL,0);  
  
	if(&mysql)  
		printf("Connection success\n");  
	else  
		printf("Connection failed\n");  
	
	//一个客户端连接就会获得一个线程，这里是数据传输套接字直接进行操作即可
	
	//test
	bzero(buf,sizeof(buf));
	sprintf(buf,"welcome!");
	write(sockfd,buf,strlen(buf));
	printf("send response to client:%s\n", buf);
	sleep(2);


	printf("now_socket=%d\n",sockfd);
	
	//从用户端获取操作指令：0为注册；1为登录
	nbytes=read(sockfd,buf,MAXDATASIZE);
	buf[nbytes]='\0';
	printf("recv message from client:%s\n",buf);
	strcpy(temp,buf);
	//注册
	//名字重复的情况【***:√】
	if(!strcmp(temp,"0")){
		//提示输入用户名和8位密码，用#分隔
		//e.g. name00#12345678
		
		nbytes=read(sockfd,buf,MAXDATASIZE);
		buf[nbytes]='\0';
		printf("recv message from client:%s\n",buf);
		
		strcpy(temp1,buf);
		
		strcpy(temp3,temp1);
		//char* strchr(char* str,char ch); 
		//找出str指向的字符串中第一次出现字符ch的位置
		loc=strchr(temp3,'#');
		//if(!loc)	{printf("input error!");	exit(1);}
		if(loc){//有#：格式正确
				token = strtok(temp1, s);strcpy(name,token);
			}
		else{////格式错误：线程结束
			bzero(buf,sizeof(buf));
			sprintf(buf,"Wrong:The format is wrong!");
			write(sockfd,buf,strlen(buf));
			printf("send response to client:%s\n", buf);
			sleep(1);
			pthread_exit (NULL);
		}
		if(exist(name)){//名字重复
			bzero(buf,sizeof(buf));
			sprintf(buf,"Wrong:The user name is already registered");
			write(sockfd,buf,strlen(buf));
			printf("send response to client:%s\n", buf);
			sleep(1);
			pthread_exit (NULL);
		}
		//如果名字不重复：正常
	
		token = strtok(NULL, s);strcpy(pw,token);
		
		if(Register(name,pw)){
			bzero(buf,sizeof(buf));
			sprintf(buf,"You have registered successfully!");
			write(sockfd,buf,strlen(buf));
			printf("send response to client:%s\n", buf);
			Log(1,sockfd,name);
			
				
				
		}
		else{
			printf("register error!");
		}
	}
	//登录【正常情况下ok；如果用户名输错了就崩了（记得改）；√】
	//或许这里需要一个检查用户是否在数据库中，在：判断密码正误；不在：返回用户名错误，count不++
	//这里补一个退出选项
	else if(!strcmp(temp,"1")){
		int flag=0;
		while(1){
			count++;
			nbytes=read(sockfd,buf,MAXDATASIZE);//读入登录的账号密码
			buf[nbytes]='\0';
			printf("recv message from client:%s\n",buf);

			strcpy(temp1,buf);
			strcpy(temp3,temp1);
			//char* strchr(char* str,char ch); 
			//找出str指向的字符串中第一次出现字符ch的位置
			loc=strchr(temp3,'#');
			//if(!loc)	{printf("input error!");	exit(1);}
			if(loc){
				token = strtok(temp1, s);strcpy(name,token);
			}
			
			
			if(loc&&(exist(name))){
			
				token = strtok(NULL, s);strcpy(pw,token);
				printf("token is ok!\n");
				
				//把结果传回给客户端，成功为right，失败为wrong
				if(compare(name,pw)){
					bzero(buf,sizeof(buf));
					sprintf(buf,"right");
					write(sockfd,buf,strlen(buf));//传回判定结果
					printf("send response to client:%s\n", buf);
					flag=1;
				}
				else{
					bzero(buf,sizeof(buf));
					sprintf(buf,"wrong");
					write(sockfd,buf,strlen(buf));
					printf("send response to client:%s\n", buf);//一直循环下去 ?（错误，mysql那边会崩）	
				}
				//printf("??\n");
			}
			else{
				count--;
				bzero(buf,sizeof(buf));
				sprintf(buf,"w_input");
				write(sockfd,buf,strlen(buf));
				printf("send response to client:%s\n", buf);
			}

			if(flag==1){
				//把sockfd加到数据库里
				Log(1,sockfd,name);
				
				sleep(1);//很重要，避免同时发送两条消息
				bzero(buf,sizeof(buf));
				sprintf(buf,"You have login successfully!");
				write(sockfd,buf,strlen(buf));
				printf("send response to client:%s\n", buf);
				break;
			}
			
			if(count==3) {pthread_exit (NULL);	break;}
		}
		
	}
	
	else if(!strcmp(temp,"2")){
		bzero(buf,sizeof(buf));
		sprintf(buf,"You have exit successfully!");
		write(sockfd,buf,strlen(buf));
		printf("send response to client:%s\n", buf);
		pthread_exit (NULL);
	}
	
	else{//输入异常（服务器结束线程，客户端退出<用exit>）
		bzero(buf,sizeof(buf));
		sprintf(buf,"Input error");
		write(sockfd,buf,strlen(buf));
		printf("send response to client:%s\n", buf);
		pthread_exit (NULL);
	}
	/////////////////////////////进入到功能界面
	while(1){
		//从用户端获取操作指令：2为查询在线人数；3为聊天；4为注销账号
		nbytes=read(sockfd,buf,MAXDATASIZE);
		buf[nbytes]='\0';
		printf("recv message from client:%s\n",buf);
		strcpy(temp,buf);
		
		if(!strcmp(temp,"2")){
			//查询成功
			Online(who_on);//who_on数组中：在线个数#name1#name2    （至少有一个他自己）
			bzero(buf,sizeof(buf));
			sprintf(buf,who_on);
			write(sockfd,buf,strlen(buf));
			printf("send response to client:%s\n", buf);

			sleep(1);
			/*bzero(buf,sizeof(buf));
			sprintf(buf,"You have queryed the online list successfully!");
			write(sockfd,buf,strlen(buf));
			printf("send response to client:%s\n", buf);*/
				
		}
		
		
		else if(!strcmp(temp,"3")){
			/*bzero(buf,sizeof(buf));
			sprintf(buf,"You have chat successfully!");
			write(sockfd,buf,strlen(buf));
			printf("send response to client:%s\n", buf);*/
			while(1){
				nbytes=read(sockfd,buf,MAXDATASIZE);
				buf[nbytes]='\0';
				printf("recv message from client:%s\n",buf);
				strcpy(temp2,buf);
				if(!strcmp(temp2,"###"))	break;
				//到这temp2里面是被聊天的对象B
				//判断B在不在线,on是B当前的socket
				on=sel_socket(temp2);
				//读客户端发来的msg
				nbytes=read(sockfd,buf,MAXDATASIZE);
				buf[nbytes]='\0';
				printf("recv message from client:%s\n",buf);
				if(on){
					//给收方B的
					strcpy(temp_msg,"recv message from ");
					//根据套接字查找发方用户名
					find(sockfd,temp_name);
					strcat(temp_msg,temp_name);//发方用户名
					strcat(temp_msg," :");
					strcat(temp_msg,buf);
					
					bzero(buf,sizeof(buf));
					sprintf(buf,temp_msg);
					write(on,buf,strlen(buf));
					printf("send response to client:%s\n", buf);
				
					//给发方A的！！！！！！
					bzero(buf,sizeof(buf));
					sprintf(buf,"The msg has already been sent");
					write(sockfd,buf,strlen(buf));
					printf("send response to client:%s\n", buf);
				}
				else{//B不在线
					bzero(buf,sizeof(buf));
					sprintf(buf,"The receiver is not online");
					write(sockfd,buf,strlen(buf));
					printf("send response to client:%s\n", buf);//
				}
			
			}
		}
		
		
		else if(!strcmp(temp,"4")){
			//删完关闭线程
			nbytes=read(sockfd,buf,MAXDATASIZE);//读入登录的账号密码
			buf[nbytes]='\0';
			printf("recv message from client:%s\n",buf);
			
			strcpy(temp1,buf);
			token = strtok(temp1, s);strcpy(name,token);
			token = strtok(NULL, s);strcpy(pw,token);
			//先要核对用户名和密码，然后再删除
			//【核对一下是不是当前用户！用套接字核对，最后写：√】
			if(compare(name,pw)&&Del(name,pw)){//可以删除，并且成功
				//或许pthread_exit (void * retval);
				bzero(buf,sizeof(buf));
				sprintf(buf,"You have deleted successfully!");
				write(sockfd,buf,strlen(buf));
				printf("send response to client:%s\n", buf);
				pthread_exit (NULL);
			}
			else{//删除失败
				bzero(buf,sizeof(buf));
				sprintf(buf,"Delete failed!");
				write(sockfd,buf,strlen(buf));
				printf("send response to client:%s\n", buf);
			}
			
			
		}
		
		else if(!strcmp(temp,"5")){
			nbytes=read(sockfd,buf,MAXDATASIZE);
			buf[nbytes]='\0';
			printf("recv message from client:%s\n",buf);
			
			strcpy(name,buf);
			Log(0,0,name);
			
			bzero(buf,sizeof(buf));
			sprintf(buf,"You have logout successfully!");
			write(sockfd,buf,strlen(buf));
			printf("send response to client:%s\n", buf);
			pthread_exit (NULL);
		}
		
		else if(!strcmp(temp,"6")){
			//文件传输
			//收到username判断是否在线
			nbytes=read(sockfd,buf,MAXDATASIZE);
			buf[nbytes]='\0';
			printf("recv message from client:%s\n",buf);
			strcpy(temp2,buf);
			//到这temp2里面是接收文件的对象B，查询套接字判断是否在线
			on=sel_socket(temp2);//on是B的套接字
			
			if(on){
				bzero(buf,sizeof(buf));
				sprintf(buf,"ON");
				write(sockfd,buf,strlen(buf));
				printf("send response to client:%s\n", buf);
				
				bzero(buf,sizeof(buf));
				find(sockfd,temp_name);
				strcpy(temp3,"\n##FFI");
				sprintf(buf,temp3);
				write(on,buf,strlen(buf));//服务器 gaosu B收来自A的文件
				printf("send response to client:%s\n", buf);
				
				sleep(2);
				bzero(buf,sizeof(buf));
				strcpy(temp3,temp_name);
				sprintf(buf,temp3);
				write(on,buf,strlen(buf));//fa song fa fang name
				printf("send response to client:%s\n", buf);
				
				//开始正式的文件传输
				//先收到文件名字
				nbytes=read(sockfd,buf,MAXDATASIZE);
				buf[nbytes]='\0';
				printf("recv filename from sender:%s\n",buf);
				
				//把文件名字发给收方
				char file_name[256]={0};
				strcpy(file_name,buf);
				bzero(buf,sizeof(buf));
				sprintf(buf,file_name);
				write(on,buf,strlen(buf));
				
				//收到文件size
				nbytes=read(sockfd,buf,MAXDATASIZE);
				buf[nbytes]='\0';
				printf("recv filesize from sender:%s\n",buf);
				//把文件名size发给收方
				char file_size[256]={0};
				strcpy(file_size,buf);
				bzero(buf,sizeof(buf));
				sprintf(buf,file_size);
				write(on,buf,strlen(buf));
				printf("..................\n");
				
				puts(file_size);
				//(str)file_size->int
				//int ssize=strnum(file_size);
				
				
				for(i=0;i<strlen(file_size);i++){
					ssize=ssize*10;
					ssize=ssize+file_size[i]-48;
					//printf("%d\n",ssize);
				}

				//printf("%d\n",ssize);

				
				int ret;
				sleep(1);
				bzero(buf,sizeof(buf));
				
				int write_len = 0;
				
				while(1){
					printf("%d\n",ssize+1);
					ret = read(sockfd, buf, sizeof(buf));
					write(on, buf, ret);
					bzero(buf,sizeof(buf));
					write_len += ret;//记录写入的字节数
					printf("len=%d",write_len);
					
					if( ret <= 0 || write_len==ssize)
					{
						printf("\n [recv-%s] receive file done!!!\n", file_name);
						break;
					}
					
				}
				ssize=0;
						
			}
			else{
				bzero(buf,sizeof(buf));
				sprintf(buf,"The receiver is not online");
				write(sockfd,buf,strlen(buf));
				printf("send response to client:%s\n", buf);
			}

		}
	
	}
	
}

int main(){
	int listen_sock;//监听套接字
	int num=0;
	int client_sock;
	int sin_size;
	int rc;
	struct sockaddr_in servaddr,clientaddr;
	//struct id_sock new_client;
	pthread_t tids[20];
	
	//Create socket
	listen_sock=socket(AF_INET,SOCK_STREAM,0);
	if(listen_sock==-1){
		printf("can't create socket\n");
		exit(1);
	}
	int opt=1;
 	unsigned int len=sizeof(opt);
 	setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt,len);
    	setsockopt(listen_sock,SOL_SOCKET,SO_KEEPALIVE,&opt,len);

	
	//Assign socket address
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(PORT);
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	//Bind ip and port
	if(bind(listen_sock,(struct sockaddr *)&servaddr,sizeof(struct sockaddr))==-1){
		printf("bind error\n");
		close(listen_sock);
		exit(1);
	}
	
	//listen on socket
	if(listen(listen_sock,BACKLOG)==-1){
		printf("listen error\n");
		close(listen_sock);
		exit(1);
	}
	
	
	
	while(1){
		sin_size=sizeof(struct sockaddr_in);
		client_sock=accept(listen_sock,(struct sockaddr *)&clientaddr,&sin_size);//该客户端的数据传输套接字创建
		//printf("now socket=%d\n",client_sock);//dui zhe ne 
		if(client_sock==-1){
				printf("accept error\n");
				continue;
		}
		rc=pthread_create(&tids[num],NULL,ser,(void*)client_sock);//ser是线程要执行的代码
		if (rc) {
      
            		printf("ERORR; return code from pthread_create() is %d\n", rc);
            		exit(EXIT_FAILURE);
        	}
		num++;
	}
	
	close(listen_sock);
	mysql_close(&mysql); 

	return 0;
}



	
