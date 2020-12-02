#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <QDir>
#include <QFile>
extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
}
//int main(int argc, char *argv[])
//{
//    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
//    return a.exec();
//}

using namespace std;
//测试解封装
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWindow w;
//    w.show();
    // 初始化封装库
    // av_register_all();
    string name = "11.mp4";
    string path = QDir::currentPath().toStdString();

    cout<<"name is "<<name <<endl;
    path+="../"+name;
    //初始化网络库 (rtsp rtmp http 协议流媒体)
    avformat_network_init();
    cout<<"path is " <<path <<endl;
    //参数设置
    AVDictionary * opts =nullptr;
    //设置rtsp流用tcp打开
    av_dict_set(&opts,"rtsp_transport","tcp",0);
    av_dict_set(&opts,"max_delay","500",0);
    //上下文信息
    AVFormatContext *ic = NULL;
    int re = avformat_open_input(&ic,
                        path.c_str(),
                        0, //自动选择解封装器
                        &opts // 参数设置，比如rtsp的延迟时间
                        );

    if(re != 0) {
        char buf[1024] = {0};
        av_strerror(re,buf,sizeof (buf));
        cout<<"open " <<path<<"faild"<<buf<<endl;
        return  0;
    }

    cout<<"open "<<path<<"sucess"<<endl;
    return a.exec();
}
