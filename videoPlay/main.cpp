#include "mainwindow.h"

#include <QApplication>
#include <iostream>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <thread>

extern "C"{
#include "libavformat/avformat.h"
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"  // 图像转化
#include "libswresample/swresample.h" // 音频转化
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

void XSleep(int ms) {
    chrono::milliseconds du(ms);
    this_thread::sleep_for(du);
}

//测试解封装
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
#if 0
    // 初始化封装库
    // av_register_all();
    string name = "11.mp4";
    string path = QDir::currentPath().toStdString();

    cout<<"name is "<< path<<endl;
    path+="/../../../"+name;
    //初始化网络库 (rtsp rtmp http 协议流媒体)
    avformat_network_init();
    // 注册解码器  注册解码器函数已经弃用
    // avcodec_register_all();



    //参数设置
    AVDictionary * opts =nullptr;
    //设置rtsp流用tcp打开
    av_dict_set(&opts,"rtsp_transport","tcp",0);
    av_dict_set(&opts,"max_delay","500",0);
    // 创建流 上下文信息
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


    // 找到音频解码器
    AVCodec* acodec= avcodec_find_decoder(ic->streams[audioStreamID]->codecpar->codec_id);
    if(!acodec) {
        qDebug()<<"can't find the codec id"<<ic->streams[audioStreamID]->codecpar->codec_id;
        return -1;

    }

    qDebug()<<"finde AVCodec" <<ic->streams[audioStreamID]->codecpar->codec_id;
    // 创建解码器上下文
    AVCodecContext *ac =  avcodec_alloc_context3(acodec);
    // 配置解码器上下文参数
    avcodec_parameters_to_context(ac,ic->streams[audioStreamID]->codecpar);
    // 八线程解码器 这里可以调用API判断CPU线程数
    ac->thread_count = 8;
    // 打开解码器
    re = avcodec_open2(ac,0,0);
    if(re != 0) {
        char buf[1024] = {0};
        av_strerror(re,buf,sizeof (buf));
        cout<<"avcodec_open2 faild " <<path<<"faild"<<buf<<endl;
        return  -1;
    }

    qDebug()<<"audio avcodec_open2 sucessful";





    // 找到视频解码器
    AVCodec* vcodec= avcodec_find_decoder(ic->streams[videoStreamID]->codecpar->codec_id);
    if(!vcodec) {
        qDebug()<<"can't find the codec id"<<ic->streams[videoStreamID]->codecpar->codec_id;
        return -1;

    }

    qDebug()<<"finde audio" <<ic->streams[videoStreamID]->codecpar->codec_id;
    // 创建解码器上下文
    AVCodecContext *vc =  avcodec_alloc_context3(vcodec);
    // 配置解码器上下文参数
    avcodec_parameters_to_context(vc,ic->streams[videoStreamID]->codecpar);
    // 八线程解码器 这里可以调用API判断CPU线程数
    vc->thread_count = 8;
    // 打开解码器
    re = avcodec_open2(vc,0,0);
    if(re != 0) {
        char buf[1024] = {0};
        av_strerror(re,buf,sizeof (buf));
        cout<<"avcodec_open2 faild " <<path<<"faild"<<buf<<endl;
        return  -1;
    }

    qDebug()<<"video avcodec_open2 sucessful";
    // malloc AVPaket 并初始化
    AVPacket* pkt = av_packet_alloc();

    AVFrame * frame = av_frame_alloc();

    // 像素格式和尺寸转化的上下文
    SwsContext* vctx = nullptr;
    // 图像格式转化输出的数据
    unsigned char * rgb = nullptr;

    // 音频重采样率
    SwrContext* actx = swr_alloc();
    actx = swr_alloc_set_opts(actx,
                              av_get_default_channel_layout(2), //输出格式
                              AV_SAMPLE_FMT_S16,// 输出样本格式
                              ac->sample_fmt, //采样率 一秒钟的样本数量
                              av_get_default_channel_layout(ac->channels), //输入的格式
                              ac->sample_fmt,
                              ac->sample_rate,
                              0,0
                              );
    re = swr_init(actx);
    if(re != 0) {
        char buf[1024] = {0};
        av_strerror(re,buf,sizeof (buf));
        cout<<"swr_init faild " <<path<<"faild"<<buf<<endl;
        return  -1;
    }
    unsigned char * pcm = nullptr;

    for(;;) {
        int re =av_read_frame(ic,pkt);
        if(re != 0) {
            //读取失败
            int ms = 3000; //3秒位置，根据时间基数(分数)转化
            long long pos = (double)ms / r2d(ic->streams[pkt->stream_index]->time_base);
            av_seek_frame(ic,videoStreamID,pos,AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
            continue;
        }

        qDebug()<<"pkg size is "<<pkt->size;
        qDebug()<<"pkg pts is "<<pkt->pts;
        // 转化为毫秒，方便做时间同步
        qDebug()<<"pkt->pts ms is" <<pkt->pts* (r2d(ic->streams[pkt->stream_index]->time_base) * 1000);

        qDebug()<<"pkg dts is "<<pkt->dts;
        AVCodecContext* cc = 0
;        if(pkt->stream_index == videoStreamID) {
            qDebug()<<"视频信息";
            cc = vc;

        }
        if(pkt->stream_index == audioStreamID) {
            qDebug()<<"音频信息";
            cc = ac;
        }

        //解码视频
        // 发送packet 到解码线程 不占用cpu,send 传null ,调用多次receive 取出所有缓冲帧
        re = avcodec_send_packet(cc,pkt);
        //引用计数减一
        av_packet_unref(pkt);
        if(re != 0) {
            char buf[1024] = {0};
            av_strerror(re,buf,sizeof (buf));
            cout<<"avcodec_send_packet faild " <<path<<"faild"<<buf<<endl;
            continue;
        }

        for(;;) {
        // 不占用cpu  从线程中获取数据 ,一次send 可能对应多次receive
            re = avcodec_receive_frame(cc,frame);
            if(re != 0 )
                break;
            qDebug()<<"recv frame " <<frame->format <<" "<<frame->linesize[0];

            if(cc = vc ) {
                // 图像转化
                vctx = sws_getCachedContext(
                            vctx,// 传null 会新创建
                            frame->width,frame->height,
                            (AVPixelFormat)frame->format,// 输入的宽高,比如格式 YUV420P
                            frame->width,frame->height, // 输出的
                            AV_PIX_FMT_RGBA,
                            SWS_BILINEAR,// 尺寸变化指定的算法
                            0,0,0
                            );
                if(vctx){

                    if(!rgb) rgb = new unsigned char[frame->width*frame->height*4];
                    uint8_t *data[2] ={0};
                    data[0] = rgb;
                    int lines[2] = {0};
                    lines[0] = frame->width*4 ; // 一行的字节数
                    qDebug()<<"像素格式转化上下文 创建或者获取成功";
                    re = sws_scale(
                           vctx,
                           frame->data,// 输入数据
                           frame->linesize, // 输入行大小
                            0, // 切片开始的位置
                            frame->height, // 输入高度
                            data ,// 输出的数据
                            lines
                     ); // 这一步的开销非常大
                    qDebug()<<"sws_scale " <<re;
                }
                else
                    qDebug()<<"像素格式转化上下文 创建或者获取失败";
            } else { // 音频重采样
                uint8_t *data[2] ={0};
                if(pcm) pcm = new uint8_t[frame->nb_samples*2*2];
                data[0] = pcm;
                re = swr_convert(actx,data,frame->nb_samples, // 输出
                            (const u_int8_t**)frame->data,frame->nb_samples //输入
                            );
            }

        }


        XSleep(500);
    }
    av_frame_free(&frame);

    if(ic) {
        // 释放封装上下文，并且将ic 置0
        avformat_close_input(&ic);
    }
#endif
    return a.exec();
}
