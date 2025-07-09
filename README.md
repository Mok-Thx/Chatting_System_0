<img width="416" alt="image" src="https://github.com/user-attachments/assets/59ed1995-1d5e-4853-bfb2-18855fb3828a" /><img width="416" alt="image" src="https://github.com/user-attachments/assets/7bae4406-24ff-4032-a786-11a5497b28b7" /># Chatting_System_0，一个类似于QQ的聊天系统
## 系统实现的功能
### 基本功能：
  - 新用户注册
  - 用户登录和退出
  - 聊天服务
  - 在线用户查询
### 附加功能：
  - 文件传送服务
### 本项目实现的其他功能：
  - 用户注销功能
  - 用户密码安全传输功能
  - 用户错误登录次数限制功能
## 项目整体设计框架及说明
  本项目整体使用TCP套接字对网络实时聊天程序进行设计开发，在服务器端部署Mysql服务器用以存储用户信息，使用多线程构造并发型服务器，并且使用MD5信息摘要算法为用户的密码信息提供安全保障。
## SEVER端：
  - 服务器启动后，首先创建监听套接字，并绑定服务器地址和端口号，然后监听等待客户端的连接请求。当有客户端接入时，服务器为客户端分配数据传输套接字和线程序号，创建新线程为客户端提供服务，主程序则继续监听其他客户端的请求，实现服务器对客户端的并发处理。
  - 服务端首先为客户端提供注册、登录服务，并且可在本步骤内退出程序。用户通过身份认证后，可以按照操作提示，使用服务器端的查询在线用户、聊天、退出、注销、文件传输等功能。

   <img width="400" alt="image" src="https://github.com/user-attachments/assets/b0c5f586-0141-4f54-9e81-aaf8aae1148b" />
  
## CLIENT端：
  - 客户端启动后，首先创建套接字，然后请求与服务器建立连接，连接建立成功后，用户通过注册/登录进行身份认证，如果多次输入密码错误程序将自动退出，保证了用户不被弱密码攻击。身份认证成功后，客户端即可与服务器进行信息传递，并且客户端自己创建接收线程，为客户端持续接收来自服务器的消息。

   <img width="287" alt="image" src="https://github.com/user-attachments/assets/312324bc-757a-4cd2-b003-8dba99831fdf" />

## 项目分模块设计说明
### 1、用户注册模块：
  - 用户首先通过提示信息进入到注册模块。按照规定格式<username#password>输入用户名和密码。客户端对密码进行加密后以<username#encrypted_password>的格式传给服务器。
  - 服务器接收到该数据后，首先对格式进行判定，若格式错误，则返回给客户端“wrong”提示信息，并结束自身线程；然后用exist()函数查询数据库，判定该用户名是否已被注册，若已被注册，也返回给客户端“wrong”提示信息，结束自身线程。若都无误，则使用Register函数进行注册，修改数据库信息，并返回给客户端“success”提示信息，完成本次注册。

   <img width="440" alt="image" src="https://github.com/user-attachments/assets/f1f5773f-6c9d-4fbd-815f-3cd1f3017f8a" />

### 2、用户登录模块：
  - 用户首先通过提示信息进入到登录模块。按照规定格式<username#password>输入用户名和密码。客户端对密码进行加密后以<username#encrypted_password>的格式传给服务器。
  - 服务器接收到该数据后，首先对格式进行判定，若格式错误，则返回给客户端“w_input”提示信息；然后用exist()函数查询数据库，判断输入的用户名是否存在，若不存在，则返回给客户端“w_input”提示信息；若存在则使用compare()函数进行用户名密码是否正确匹配的判定，若判定结果为正确则返回给客户端“success”提示信息，并且服务器使用Log()函数更改数据库信息，完成本次登录；若判定结果为错误则返回给客户端“wrong”提示信息，并且客户端和服务器的计数器+1。计数器为3时，客户端退出，服务器结束本线程。

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/24e4002a-f9f8-4834-ada4-f54cf3631593" />

### 3、查询在线用户模块：
  - 客户端和服务器同时进入到查询在线用户模块，服务器通过online()函数获取到在线人数和用户名，并且以<num#online_user1#online_user2……>的形式发送给客户端，客户端对收到的字符串进行分割后输出。

   <img width="342" alt="image" src="https://github.com/user-attachments/assets/633484e7-1883-4870-904b-a77dfeefa38f" />

### 4、聊天模块：
  - 用户按照提示信息输入要发送消息的对象或退出聊天模式的符号，并发送给服务器，若为退出符号，客户端和服务器都退出聊天模块；若为用户名，客户端按照提示信息输入要发的消息，并发送给服务器；服务器首先使用sel_socket()函数判定接收方用户是否在线，若不在线则给发送方客户端发送“Failed”提示信息，否则，接收发方客户端发来的消息并且转发给收方客户端并且通知收方消息的发送用户的用户名，并向发方客户端返回“success”提示信息。完成后继续使用聊天模块直至用户输入退出信息。

   <img width="385" alt="image" src="https://github.com/user-attachments/assets/954ff68f-ca1f-4fa5-bf24-19cbb34767cf" />

### 5、用户退出模块：
  - 客户端进入退出模式后，发送自己的用户名至服务器，服务器接收后使用Log()函数对数据库信息进行更改，更改成功后把“success”提示信息传回给客户端并接收本进程，客户端接收后退出程序。

   <img width="288" alt="image" src="https://github.com/user-attachments/assets/bd3d2d1f-3f3f-4910-9b9f-931de6a8b0c9" />

### 6、账号注销模块：
  - 客户端进入退出模式后，按照提示信息输入密码，服务器对密码进行加密后，以<username#encrypted_password>的格式发送用户名和加密后的密码至服务器，服务器接收后使用compare()函数通过查找数据库信息判定用户名和密码是否匹配，若匹配错误，则返回“failed”提示信息给客户端，客户端收到该信息后注销失败；若匹配成功，则使用Del()函数删除数据库中的相关信息，然后返回“success”提示信息给客户端并且结束本线程，客户端收到该提示信息后退出程序。

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/ec0abc28-9ed9-4eb5-a4ad-2156b45f8a43" />

### 7、文件传输模块：
  - 客户端进入到文件传输模式后，首先按照提示信息输入收方用户名，并且发给服务器端，服务器端收到后用sel_socket()函数对收方是否在线进行判定，若在线，则发送收文件通知给收方，若不在线则不进行任何其他操作，最终将在线结果传回发方客户端。发方客户端收到服务器的在线结果后，如果收方不在线则退出文件传输模块，若在线则还是正式的文件传输。
  - 先将文件信息发送给收方用户：发方用户首先按照提示信息输入文件路径，并且使用size()函数计算文件大小，然后打开文件准备读取。发方客户端把用basename()函数提取的文件名称发送给服务器，服务器接收后转发给收方服务器，收方服务器创建新的空文件等待文件内容的传入；发方客户端发送文件大小给服务器，服务器接收后转发给收方客户端，收方客户端接收后进行保存，以此作为判断文件是否传输结束的标志。
  - 再传输文件内容：发方客户端按序读出文件中的字节发给服务器，服务器接收后转发给收方客户端，收方客户端接收后将其写入先前创建好的用来接收的空文件中。反复以上步骤直至发方客户端已将全部文件内容传给收方客户端。
  - 此时文件传完成，收发客户端关闭文件，服务器完成本次文件传输的服务。

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/a2014790-aa80-4307-9cdd-871e07333e56" />

## 项目关键数据结构及说明
### 1、sockaddr
  - 用于存储参与（IP）Windows/linux套接字通信的计算机上的一个internet协议（IP）地址。为了统一地址结构的表示方法 ，统一接口函数，使得不同的地址结构可以被bind()、connect()、recvfrom()、sendto()等函数调用。但一般的编程中并不直接对此数据结构进行操作，而使用另一个与之等价的数据结构sockaddr_in。

   <img width="395" alt="image" src="https://github.com/user-attachments/assets/32d0b4bb-235a-46ca-9306-7652fdfb9aed" />

### 2、sockaddr_in
  - 由于struct sockaddr 是一个通用地址结构，为了统一地址结构的表示方法、接口函数，使不同的地址结构可以被bind() , connect() 等函数调用，然而，sa_data把目标地址和端口信息混在一起了。为了解决sockaddr的缺陷引入了sockaddr_in结构体。
  - struct sockaddr_in中的in 表示internet，就是网络地址，这只是我们比较常用的地址结构，属于AF_INET地址族。sockaddr_in结构体解决了sockaddr的缺陷，把port和addr 分开以网络字节序的方式储存在两个变量中。

   <img width="402" alt="image" src="https://github.com/user-attachments/assets/3a577ee3-0d37-4f1f-a5a2-2f326c927917" />

### 3、in_addr
  - 用来表示一个32位的IPv4地址。

   <img width="224" alt="image" src="https://github.com/user-attachments/assets/43d960a9-83d2-45cd-bb25-fc0a21be71b5" />

### 4、Mysql数据库中的数据结构
  - 数据库中存储四列信息：分别是varchar(20) name; char(32) pw; bool state; int data_sock; 其中name不为空且为主键。name为用户名，pw为经过MD5加密后的密码，state为用户是否在线的判定，data_sock为用户在线之后的套接字。

   <img width="271" alt="image" src="https://github.com/user-attachments/assets/e12368e4-ec14-483a-baa2-b9fa0073f0b1" /> <img width="301" alt="image" src="https://github.com/user-attachments/assets/50e6b0df-39a2-49cb-b373-8bddacc88f8d" />

## 项目关键函数说明
### 1.int size(char file_path[256])
  - 通过文件路径获取文件大小

   <img width="141" alt="image" src="https://github.com/user-attachments/assets/cbd96c8d-44a2-4220-9c4f-76f5ab06fe6c" />

### 2.void* rcv(int sockfd)
  - 该函数内为客户端接收线程需执行的代码。
	- FLAG作为全局变量，用户注册/登录成功后，FLAG为0，用户退出/注销后，FLAG置为1，结束接收消息线程。
	- flag0作为全局变量，在一些特定的功能模块中置为1时，将会把接收线程中从服务器收到的消息通过全局变量temp1[MAXDATASIZE]以共享内存使得主函数中获得服务器发来的消息。
	- 如果收到了“\n##FFI”消息则为文件传输通知，以收方的身份进入到文件传输功能。
   
   <img width="293" alt="image" src="https://github.com/user-attachments/assets/ea13e95e-e13f-4990-bf6e-6bf5cd414667" />

### 3.void* ser(int sockfd)
  - 该函数为服务器端线程执行的代码。
	- 首先使用mysql_real_connect()函数连接数据库，然后进入到登录/注册/直接退出的功能选择，用户成功登录/注册之后，即可进入到功能选择页面，即查询在线人数/聊天/注销/退出/文件传输功能。具体功能信息参考上文的项目分模块设计说明。
   
   <img width="285" alt="image" src="https://github.com/user-attachments/assets/7effb3ff-4376-41f7-8a2a-c003c1e59f0c" />

### 4.与Mysql操作相关的函数
  - Register()、compare()、Log()、Online()、Del()、sel_socket()、find()、exist()函数流程基本相同。首先将要进行操作的值或者对象传入函数中，通过字符串相关函数拼凑成完整的Mysql命令语句，使用mysql_query()函数执行命令语句，将查询/修改结果作为返回值返回。

   <img width="161" alt="image" src="https://github.com/user-attachments/assets/8066fb74-baf6-494c-95e7-b00a0510845b" />

### 5. void MMD(char encrypt[128],char end[16])
  - 使用时MD5杂凑算法，首先进行数据填充，然后添加消息长度，处理数据，最后进行MD5运算，得到的运算结果即为加密后的密码值，返回即可。

	 <img width="120" alt="image" src="https://github.com/user-attachments/assets/f179fbaf-f4d7-486c-93b1-5f907aac6f5d" />


## 文件功能说明及项目编译步骤
### 文件功能说明
- s3.c：服务器源代码
- c3.c：客户端源代码
- s3：由s3.c编译链接生成的可执行文件
- c3：由c3.c编译链接生成的可执行文件
- md5.h：散列函数MD5算法的头文件
### 项目编译步骤
- 服务器编译口令：gcc -g -Wall -o s3 s3.c -I /usr/include/mysql -L/usr/lib/mysql -lmysqlclient -lpthread
		（具体要根据mysql相关文件的位置变化）
- 服务器执行口令：./s3
- 客户端编译口令：gcc -o c3 c3.c -lmysqlclient -lpthread
- 客户端执行口令：./c3
## 项目演示步骤
### 1、建立服务器端数据库，调整s3代码中数据库用户信息的部分
- 数据库格式：

	 <img width="278" alt="image" src="https://github.com/user-attachments/assets/47641120-8087-46a0-93ca-8b8a213adcce" />

- 需要调整的代码部分：

	 <img width="183" alt="image" src="https://github.com/user-attachments/assets/59e04be2-2fb7-4178-869a-8bc81c2fd88b" />

### 2、对源码进行编译
- 使用上文提到的编译口令将 s3.c 和 c3.c 编译连接成可执行文件s3和 c3。

### 3、运行可执行文件

   <img width="320" alt="image" src="https://github.com/user-attachments/assets/83a52014-3c78-4d4c-89d8-6ed9423e9302" />
	
### 4、进行注册/登录/直接退出
#### 4.1 注册
- 根据提示输入0，然后输入<username#password>，例如：user1#123，正常注册后如下：

	 <img width="416" alt="image" src="https://github.com/user-attachments/assets/f34e296a-a0c1-4750-aa31-c6b01e8a131c" />

- 此时的数据库信息如下：

	 <img width="306" alt="image" src="https://github.com/user-attachments/assets/4583ea31-dc45-471f-9271-598bb98cfadc" />

- 输入重复的信息，例如：user1#123，结果如下：

	 <img width="416" alt="image" src="https://github.com/user-attachments/assets/c747cf24-c97b-4c1b-9c1f-df1fa18f0614" />

#### 4.2 登录
- 根据提示输入0，然后输入<username#password>，例如：user1#123

	 <img width="416" alt="image" src="https://github.com/user-attachments/assets/e1ea4377-3a33-44f0-90bb-deb7a81a4d95" />

- 输入错误的密码三次（用户名存在），例如<user1#000>

	 <img width="416" alt="image" src="https://github.com/user-attachments/assets/8c2d6750-f8ef-4668-9775-8b59e7292120" />

- 输入的用户信息不存在时，可以无限次输入，服务器返回给客户端的错误提示不同。例如：uu#00

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/d07241c9-b7bd-4b72-9577-100c3a2d5a0b" />

#### 4.3 直接退出
- 根据提示信息输入2即可

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/199b9ba3-72ec-407f-8b29-d202389a0ec7" />

### 5、查询在线用户
- 此时登录三个用户，然后从一个客户端进行查看，登录了3个用户后的数据库如下

   <img width="299" alt="image" src="https://github.com/user-attachments/assets/561879e0-a6db-46ed-a8e9-76f59f930acf" />

- 在user2端进行在线用户的查询如下

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/ea3d31e0-1653-4985-aa0e-f8c311c45b94" />

### 6、聊天功能
#### 6.1 收方在线
- 在此使用user1给user2发送消息

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/6d49ebd6-b311-45bc-821a-861cb3ead1b7" />

#### 6.2 收方不在线
- 在此让user2退出，然后user1给user2发送消息，退出后的数据库信息如下

	 <img width="311" alt="image" src="https://github.com/user-attachments/assets/f3be24cb-fe2e-4025-82c6-ca11f1a53bda" />

- 收方不在线时的聊天

   <img width="289" alt="image" src="https://github.com/user-attachments/assets/922acd6b-cbbc-403f-8df4-a7440aaaf063" />

### 7、注销功能
注销user1
- 密码输入正确，注销成功

   <img width="293" alt="image" src="https://github.com/user-attachments/assets/c23f46c2-ca09-44c5-b77b-cd11485be730" />

- 密码输入错误，注销失败

   <img width="302" alt="image" src="https://github.com/user-attachments/assets/045cc6f1-e99d-45ea-afaa-c2e242bacb47" />

### 8、退出功能
- 退出admian

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/e4d80a19-5f7f-429c-b14d-88bf22f47915" />

### 9、文件传输功能
此处user3给user2发送文件
#### 9.1 收方不在线
- 输入收方用户名，服务器返回不在线的通知，结束文件传输功能

   <img width="284" alt="image" src="https://github.com/user-attachments/assets/bd18473e-bdd0-4602-a4db-fefd48bf53ad" />

#### 9.2 收方在线
- 输入收方用户名，服务器返回在线的通知，发方输入完整的文件路径名称即可

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/070ce799-8213-4e40-9c87-044053478f60" />

### 10、跨主机连通性测试
- 在ip为192.168.43.8的主机上运行服务器代码，然后在ip为192.168.43.118的主机上运行客户端代码，可以看到“Connection success”，表示连接成功，本项目可以进行跨主机运行。

   <img width="406" alt="image" src="https://github.com/user-attachments/assets/d15a1d2f-8766-4882-8db6-7072285b9d6c" />

- 在主机1中运行服务器代码

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/10299ec2-5b37-49a5-a2f0-4b4d5349f560" />

- 在主机2中运行客户端代码

   <img width="416" alt="image" src="https://github.com/user-attachments/assets/d81f1d4c-875b-4739-a111-f7478c24a745" />
