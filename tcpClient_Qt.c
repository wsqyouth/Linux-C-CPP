tcp客户端是client,用Linux C写
tcp服务器端是server,用Qt写

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

typedef char                s8;
typedef unsigned char       u8;
typedef short               s16;
typedef unsigned short      u16;
typedef int                 s32;
typedef unsigned    int     u32;
typedef float               f32;
typedef signed long long    s64;
typedef unsigned long long  u64;

int16_t tcpClientfd;
int16_t err_log;

int Tcp_Client_Init(uint8_t* ip,uint16_t port)
{
	uint16_t server_port = port;   // 服务器的端口号6001
	uint8_t  *server_ip =  ip; // 服务器ip地址"192.168.33.33"
	
	//printf("connect %s ...\n",server_ip);
	tcpClientfd = socket(AF_INET, SOCK_STREAM, 0);      // 创建通信端点：套接字
	if (tcpClientfd < 0) {
		perror("socket failed");
		//exit(-1);
		return -1;
	}
	
    //初始化服务器地址结构体
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));    // 初始化服务器地址
	server_addr.sin_family = AF_INET;   		 // IPv4
	server_addr.sin_port = htons(server_port); // 端口 6001
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);    // ip
	// 主动连接服务器
	err_log = connect(tcpClientfd, (struct sockaddr*) &server_addr,sizeof(server_addr));
	if (err_log != 0) {
		printf("connect %s failed!\n",server_ip);
		//close(tcpClientfd);
		//exit(-1);
		return -1;
	}else
		printf("connected %s \n",server_ip);

	return 0;
}

ssize_t writen(int fd, const void *buf, size_t count)
{
    size_t nleft = count;
    ssize_t nwritten;
    char *bufp = (char *)buf;

    while (nleft > 0)
    {

        if ((nwritten = write(fd, bufp, nleft)) < 0)
        {

            if (errno == EINTR)
                continue;
            return -1;
        }

        else if (nwritten == 0)
            continue;

        bufp += nwritten;
        nleft -= nwritten;
    }

    return count;

}

int TCPMyRecv(int iSock,char * pchBuf,size_t tCount){

	 size_t tBytesRead=0;
        int iThisRead;
        while(tBytesRead < tCount){
              do{
                     iThisRead = read(iSock, pchBuf, tCount-tBytesRead);
              } while((iThisRead<0) && (errno==EINTR));
              if(iThisRead < 0){
                      return(iThisRead);
              }else if (iThisRead == 0)
                      return(tBytesRead);
              tBytesRead += iThisRead;
              pchBuf += iThisRead;
       }
}


void printArr(char buf[],int n)
{
    for(int j=0;j<n;j++)
    {
        printf("%x\n",buf[j]);
    }
}
int main()
{
	int16_t ret;
    /**TCP 客户端初始化**/
	ret = Tcp_Client_Init("192.168.2.253",8765);
	if (ret != 0) {
		printf("192.168.2.253,tcp client init failed!\n");
	}
    printf("start send\n");
	int8_t send_buf[] = {"hello qt"};
	writen(tcpClientfd, send_buf, sizeof(send_buf));

	int num;
    int8_t recvbuf[1024];
	//int32_t content_buf[2048*5];
	int count = 0;

	u8 rxGainModeNew; //增益模式
    s8 rxGainValueNew; //增益值
    u32 rxFreqBandwidthNew; //带宽
    u32 rxLoFreqencyNew; //Lo频率
    u32 rxFreqSampleNew;  //采样频率

    u8 head_buf[6]; //包头+数据长度
	u8* data;
	
	
	while(1)
	{
	   ret=recv(tcpClientfd, head_buf, sizeof(head_buf), 0);// 接收数据
	   if(ret>0)
	   {

		   printf("recv buf len:%d \r\n",ret);
		   //for( i=0;i<ret;i++)
		   //{
		   //   printf("--%x\r\n",*(u8*)(&head_buf[i]));
		   //}
		   //printf("analyze data receive from 192.168.33.35\r\n");
           if(*(u16*)(&head_buf[0])==0x7E7E)
		   {
                u32 packet_len = *(u32*)(&head_buf[2]); //content len + tail len
                printf("packet_len = %x \r\n",packet_len);

			    data = malloc(packet_len);  //开辟实际长度的data
			
                ret=TCPMyRecv(tcpClientfd, data,packet_len);// 接收数
				//ret = recv(tcpClientfd, data, packet_len,0);
			    if(ret>0)
			    {	
                    printf("recv buf data len:%d\r\n",ret);
                    #if 0
                    for(int i=0;i<ret;i++)
                    {
                        printf("%x\r\n",*(u8*)(&data[i]));
                    }
                    printf("-**************\r\n");
                    #endif
					//解析有效数据(去除IP)
					int index = 4;
					rxGainModeNew = *(u8*)(&data[index]);
					index += 1;

					rxGainValueNew  = *(s8*)(&data[index]);
					index += 1;
					rxFreqBandwidthNew  = *(u32*)(&data[index]);
					index += 4;
					rxLoFreqencyNew  = *(u32*)(&data[index]);
					index += 4;
					rxFreqSampleNew= *(u32*)(&data[index]) ;
					index += 4;


					printf("%d\n",rxGainModeNew);
					printf("%d\n",rxGainValueNew);
					printf("%Ld\n",rxFreqBandwidthNew);
					printf("%Ld\n",rxLoFreqencyNew);
					printf("%Ld\n",rxFreqSampleNew);
                }
           }
		   
		}else
		{
			printf("this is 33.35 recv disconnect\n");
			sleep(1);
		}

	}
	//释放资源,避免野指针
	free(data);
	data = NULL;/*请加上这句*/
	return 0;
}


------
服务器端代码（Qt)


#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->lineEdit_Port->setText("8765");
    ui->pushButton_Send->setEnabled(false);

    server = new QTcpServer();

    //连接信号槽
    connect(server,&QTcpServer::newConnection,this,&MainWindow::server_New_Connect);
}

MainWindow::~MainWindow()
{
    server->close();
    server->deleteLater();
    delete ui;
}

void MainWindow::on_pushButton_Listen_clicked()
{
    if(ui->pushButton_Listen->text() == tr("侦听"))
    {
        //从输入框获取端口号
        int port = ui->lineEdit_Port->text().toInt();

        //监听指定的端口
        if(!server->listen(QHostAddress::Any, port))
        {
            //若出错，则输出错误信息
            qDebug()<<server->errorString();
            return;
        }
        //修改按键文字
        ui->pushButton_Listen->setText("取消侦听");
        qDebug()<< "Listen succeessfully!";
    }
    else
    {
        //如果正在连接（点击侦听后立即取消侦听，若socket没有指定对象会有异常，应修正——2017.9.30）
        if(socket->state() == QAbstractSocket::ConnectedState)
        {
            //关闭连接
            socket->disconnectFromHost();
        }
        //取消侦听
        server->close();
        //修改按键文字
        ui->pushButton_Listen->setText("侦听");
        //发送按键失能
        ui->pushButton_Send->setEnabled(false);
    }

}
void printArr(char buf[],int n)
{
    for(int j=0;j<n;j++)
    {
        printf("%x\n",buf[j]);
    }
}

void MainWindow::on_pushButton_Send_clicked()
{
    //qDebug() << "Send: " << ui->textEdit_Send->toPlainText();
    //获取文本框内容并以ASCII码形式发送
    //socket->write(ui->textEdit_Send->toPlainText().toLatin1());
    //socket->flush();
       s8 buf[256] = {0};
       int index = 0;

       u8 rxGainMode = 1; //增益模式
       s8 rxGainValue = -3; //增益值
       u32 rxFreqBandwidth = 5000000; //带宽
       u32 rxLoFreqency = 2450000000; //Lo频率
       u32 rxFreqSample = 10000000;  //采样频率

       *(u8*)(&buf[index]) = rxGainMode;
       index += 1;
       *(s8*)(&buf[index]) = rxGainValue;
       index += 1;
       *(u32*)(&buf[index]) = rxFreqBandwidth;
       index += 4;
       *(u32*)(&buf[index]) = rxLoFreqency;
       index += 4;
       *(u32*)(&buf[index]) = rxFreqSample;
       index += 4;

       int newLen = DealFrameData(buf, index);
       if (newLen>0) {
           qDebug() << "DealFrameData Success";
           socket->write(buf,newLen);
           socket->flush();
           printArr(buf,newLen);
       }else{
           qDebug() << "DealFrameData Failed";
       }
}

void MainWindow::server_New_Connect()
{
    //获取客户端连接
    socket = server->nextPendingConnection();
    //连接QTcpSocket的信号槽，以读取新数据
    QObject::connect(socket, &QTcpSocket::readyRead, this, &MainWindow::socket_Read_Data);
    QObject::connect(socket, &QTcpSocket::disconnected, this, &MainWindow::socket_Disconnected);
    //发送按键使能
    ui->pushButton_Send->setEnabled(true);

    qDebug() << "A Client connect!";
}

void MainWindow::socket_Read_Data()
{
    QByteArray buffer;
    //读取缓冲区数据
    buffer = socket->readAll();
    if(!buffer.isEmpty())
    {
        QString str = ui->textEdit_Recv->toPlainText();
        str+=tr(buffer);
        //刷新显示
        ui->textEdit_Recv->setText(str);
    }
}

void MainWindow::socket_Disconnected()
{
    //发送按键失能
    ui->pushButton_Send->setEnabled(false);
    qDebug() << "Disconnected!";
}


unsigned char DealFrameData(char *buf, int size) {

    char *allData=NULL;
    frameData   smallData;

    smallData.cmdHead = FRAME_HEAD;
    smallData.dataLen = size+6; //ip+content+ tail
    smallData.dstIp =  DST_IP;//0x00 01
    smallData.selfIp = SELF_IP;//0x00 02
    smallData.pContent = buf;
    smallData.cmdTail = FRAME_TAIL;

    allData = (char *)malloc(6+smallData.dataLen); //head+len+ip+content+ tail
    if (allData == 0) {
        return -1;

    }
    int index = 0;
    //数据帧头
    *(u16*)(&allData[index]) = smallData.cmdHead;
    index+=2;
    //2 3 4 5
    *(u32*)(&allData[index]) = smallData.dataLen;
    index+=4;
    //6 7
    *(u16*)(&allData[index]) = smallData.dstIp;
    index+=2;
    //8 9
    *(u16*)(&allData[index]) = smallData.selfIp;
    index+=2;
    //10~10+size
    memcpy(&allData[index], smallData.pContent, size);  //拷贝数据
    index+=size;
    //数据帧尾
     *(u16*)(&allData[index]) = smallData.cmdTail;
    index+=2;
    //将新的数据放回buf
    memcpy(buf, allData, index); //所有元素(head+len+ip+content+ tail)

    free(allData);

    return index;
}
