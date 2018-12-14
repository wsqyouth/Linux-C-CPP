功能：做个二维数组存取的demo,方便后续使用。
接口：Linux C
---
#include <stdio.h>
#include <stdlib.h>

void writefileArr2D(const char* filename,double arr[][2],int rank)
{
    FILE *fp;
    if((fp = fopen(filename,"w")) == NULL)
    {
        printf("file open error");
    }
    for(int i=0; i<rank; ++i)
    {
        fprintf(fp,"%lf,%lf\n",arr[i][0],arr[i][1]);
    }
    fclose(fp);


}
void readfileArr2D(const char* filename,double arr[][2],int rank)
{
    FILE *fp;
    if((fp = fopen(filename,"r")) == NULL)
    {
        printf("file not exist\n");
    }
    for(int i=0; i<rank; ++i)
    {
        fscanf(fp,"%lf,%lf\n",&arr[i][0],&arr[i][1]);
    }
    fclose(fp);


}

int main() 
{
    const int N = 5;

    double arr[N][2];
    for(int i=0;i<5;++i)
    {
        arr[i][0] = i*1.0;
        arr[i][1] = i*3.3;
    }
    writefileArr2D("data.txt",arr,N);
    printf("data has wirte\n");
    readfileArr2D("data.txt",arr,N);
    for(int i=0;i<5;++i)
    {
        printf("%lf %f\n",arr[i][0],arr[i][1]);
    }
    printf("data has read\n");

    return 0;
}


----
附
Qt读取文件到数组的函数：
/****************************************
* Qt中使用文件选择对话框步骤如下：
* 1. 定义一个QFileDialog对象
* 2. 设置路径、过滤器等属性
*****************************************/
void MainWindow::selectFile()
{
    //定义文件对话框类
    QFileDialog *fileDialog = new QFileDialog(this);
    //定义文件对话框标题
    fileDialog->setWindowTitle(tr("打开文件"));
    //设置默认文件路径
    fileDialog->setDirectory(".");
    //设置文件过滤器
    //fileDialog->setNameFilter(tr("Files(*.txt *.ini)"));
    fileDialog->setNameFilter( "Allfile(*.*);;数据文件 (*.txt *.dat);;inifile(*.ini);;cfile(*.c)"); //设置文件过滤器
    fileDialog->setViewMode(QFileDialog::List);  //设置浏览模式，有 列表（list） 模式和 详细信息（detail）两种方式
    //设置可以选择多个文件,默认为只能选择一个文件
    //fileDialog->setFileMode("QFileDialog::ExistingFiles");
    //设置视图模式
    fileDialog->setViewMode(QFileDialog::Detail);
    //打印当前选中文件的路径
    QStringList fileNames;
    QString   myfileName;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
        myfileName =  fileNames[0];     //取第一个文件名
    }
    else
    {
        fileDialog->close();
    }

    qDebug()<<myfileName<<endl;
    readData(myfileName);
}

//根据文件名读取数据
void MainWindow::readData(QString filename)
{
      QFile file(filename);
      if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
      {
          qDebug()<<"Can't open the file!"<<endl;
          return;
      }
      QTextStream stream(&file);
      QStringList data_file;
      QString lineString;//返回文件的一行，不包含换行符。

      int lineCount = 0;
      QString dataA;
      QString dataB;
      while(!stream.atEnd())
      {
          lineString = stream.readLine();
          if(!lineString.isEmpty())
          {
              data_file = lineString.split(",",QString::SkipEmptyParts);
              dataA=data_file.at(0);
              dataB=data_file.at(1);
              qreal areal=dataA.toDouble();
              qreal breal=dataB.toDouble();
              QPointF point(areal,breal);
              dataVec.append(point);

              lineCount++;
          }

      }
      qDebug()<<"tolal lines:"<<lineCount<<endl;
      for(int i=0;i<dataVec.size();++i)
      {
            qDebug()<<dataVec[i].x()<<" "<<dataVec[i].y()<<endl;
      }
}
