#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
//    printf("Hello FFMPEG, av_version_info is %s\n",
//               av_version_info());

//    printf("avutil_configuration is \n%s\n",
//              avutil_configuration());
}

MainWindow::~MainWindow()
{
    delete ui;
}

