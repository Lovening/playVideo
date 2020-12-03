#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <QDir>
#include <QFile>
#include <QDebug>
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
static double r2d(AVRational r) {

    return r.den == 0 ?0:double(r.num)/(double)r.den;
}

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

    cout<<"name is "<< path<<endl;
    path+="/../../../"+name;
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
    //获取流信息
    re = avformat_find_stream_info(ic,0);

    //总时长  毫秒
    int totalMs = ic->duration/(AV_TIME_BASE/1000);
    // 打印视频流详细信息
    av_dump_format(ic,0,path.c_str(),0);

    //音视频索引
    int videoStreamID = 1;
    int audioStreamID = 0;
    // 获取音视频流信息
    for(int i = 0; i< ic->nb_streams ;++i) {
        AVStream* &as = ic->streams[i];
        // 音频
        if(as->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) {
            qDebug()<<"id "<<i<<"音频信息"<<endl;
            audioStreamID = i;
            qDebug()<<"sample_rate "<<as->codecpar->sample_rate<<endl;
            qDebug()<<"format "<<as->codecpar->format<<endl;
            qDebug()<<"channel "<<as->codecpar->channels<<endl;
            qDebug()<<"channel id "<<as->codecpar->codec_id<<endl;
            qDebug()<<"audio fps=  "<<r2d(as->avg_frame_rate);
            //一帧数据单通道样本数量
            qDebug()<<"frame_size "<<as->codecpar->frame_size;
            //1024*2*2=4096  fps = sample_rate/frame_size
        }
        //视频
        if(as->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            qDebug()<<"id "<<i<<"视频信息"<<endl;
            videoStreamID = i;
            qDebug()<<"width "<<as->codecpar->width<<endl;
            qDebug()<<"height "<<as->codecpar->height<<endl;
            //帧率 fps 分数转化
            qDebug()<<"video fps=  "<<r2d(as->avg_frame_rate);
        }

    }
    //获取视频流
    videoStreamID = av_find_best_stream(ic,AVMEDIA_TYPE_VIDEO,-1,-1,nullptr,0);
    //ic->streams[videoStreamID];

    if(ic) {
        // 释放封装上下文，并且将ic 置0
        avformat_close_input(&ic);
    }

    cout<<"open "<<path<<"sucess"<<endl;
    return a.exec();
}
