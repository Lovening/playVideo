#include "xvideowidget.h"
#include <QtDebug>

//自动加双引号
#define GET_STR(x) #x
// 顶点shader
const char *vString = GET_STR(
    attribute vec4 vertexIn; //顶点坐标
    attribute vec2 textureIn;
    varying vec2 textureOut;
    void main(void){
        gl_Position = vertexIn;
        textureOut = textureIn;
    }
);

//片元shader
const char*tString = GET_STR(
    varying vec2 textureOut;
    uniform sampler2D tex_y;
    uniform sampler2D tex_u;
    uniform sampler2D tex_v;
    void main(void){
        vec3 yuv;
        vec3 rgb;
        yuv.x = texture2D(tex_y, textureOut).r;
        yuv.y = texture2D(tex_u, textureOut).r - 0.5;
        yuv.z = texture2D(tex_v, textureOut).r - 0.5;
        rgb = mat3(1.0,1.0,1.0,
                   0.0,-0.39465,2.03211,
                   1.13983,-0.58060,0.0
                   )*yuv;
        gl_FragColor = vec4(rgb,1.0);
    }
);


// 准备yuv数据 ffmpeg
// ffmpeg -i v1080.mp4 -t 10 -s 240*128 -pix_fmt yuv420p out240x128.yuv
XVideoWidget::XVideoWidget(QWidget* parent)
    :QOpenGLWidget(parent)
{

}

XVideoWidget::~XVideoWidget()
{

}

void XVideoWidget::initializeGL()
{
    qDebug()<<"initializeGL";
    // 初始化 opengl (QOpenGLFunction) 函数
    initializeOpenGLFunctions();

    //program 加载 shader(定点和片元)脚本
    // 片元(像素)
    qDebug()<<program.addShaderFromSourceCode(QGLShader::Fragment,tString);
    qDebug()<<program.addShaderFromSourceCode(QGLShader::Vertex,vString);
}

void XVideoWidget::paintGL()
{
    qDebug()<<"paintGL";
}

void XVideoWidget::resizeGL(int width, int height)
{
    qDebug()<<"resizeGL"<<width<<height;
}
