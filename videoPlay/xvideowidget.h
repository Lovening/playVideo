#ifndef XVIDEOWIDGET_H
#define XVIDEOWIDGET_H

#include <QObject>
#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QGLShaderProgram>

class XVideoWidget : public QOpenGLWidget,protected QOpenGLFunctions
{
    Q_OBJECT
public:
    XVideoWidget(QWidget* parent = nullptr);

    ~XVideoWidget();

protected:
    // 设置OpenGL资源和状态。在第一次调用resizeGL或paintGL之前被调用一次
    void initializeGL() override;
    // 渲染OpenGL场景，每当需要更新小部件时使用
    void paintGL() override;
    // 设置OpenGL视口、投影等，每当尺寸大小改变时调用
    void resizeGL(int width, int height) override;

private:
    // shader程序
    QGLShaderProgram program;
};

#endif // XVIDEOWIDGET_H
