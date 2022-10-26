#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

#define DEF_DEV_INDEX    0
#define DEF_USE_CAN_NUM  0
#define DEF_SEND_TYPE    2
#define DEF_SEND_FRAMES  4
#define DEF_SEND_TIMES   10000
#define DEF_SEND_DELAY   5

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    resize(1600, 1200);
    user = new CanUser(DEF_DEV_INDEX, DEF_USE_CAN_NUM, DEF_SEND_TYPE, DEF_SEND_FRAMES,
                       DEF_SEND_TIMES, DEF_SEND_DELAY);
    reciever = new CanThread();
    sender = new CanThread();
    ui->stopButton->setEnabled(false);
    ui->recieveButton->setEnabled(true);
    ui->sendButton->setEnabled(true);

    ui->table_data->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->clusterTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    initScatter();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [=](){
        int Row = 0;
        for(auto iter = reciever->user_thread->data_vector.begin(); iter != reciever->user_thread->data_vector.end(); ++iter)
        {
            Row = iter -  reciever->user_thread->data_vector.begin();
            if(Row >= 20)
                break;
            for(int i = 0; i < (*iter).length(); i++)
                ui->table_data->setItem(Row, i, new QTableWidgetItem(QString("%1").arg((*iter)[i], 4, 16)));
        }
        for(int k = Row; k < 20; k++)
        {
            for(int n = 0; n < 9; n++)
                ui->table_data->setItem(k, n, new QTableWidgetItem("0"));
        }
        ui->numberShow->setText(QString("%1")
                                .arg(reciever->user_thread->cluster_status->far_num + reciever->user_thread->cluster_status->near_num));
        for(auto iter = reciever->user_thread->cluster_vector.begin(); iter != reciever->user_thread->cluster_vector.end(); ++iter)
        {
            Row = iter - reciever->user_thread->cluster_vector.begin();
            if(Row >= 20)
                break;
            ui->clusterTable->setItem(Row, 0, new QTableWidgetItem(QString("%1").arg((*iter).ID)));
            ui->clusterTable->setItem(Row, 1, new QTableWidgetItem(QString("%1").arg((*iter).X)));
            ui->clusterTable->setItem(Row, 2, new QTableWidgetItem(QString("%1").arg((*iter).Y)));
            ui->clusterTable->setItem(Row, 3, new QTableWidgetItem(QString("%1").arg((*iter).Vx)));
            ui->clusterTable->setItem(Row, 4, new QTableWidgetItem(QString("%1").arg((*iter).Vy)));
            ui->clusterTable->setItem(Row, 5, new QTableWidgetItem(QString("%1").arg((*iter).RCS)));
        }
        for(int k = Row; k < 20; k++)
        {
            for(int n = 0; n < 9; n++)
                ui->clusterTable->setItem(k, n, new QTableWidgetItem("0"));
        }
        showPoint();
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*************按键信号槽函数***************/
void MainWindow::on_openButton_clicked()
{
    QListWidgetItem *item = new QListWidgetItem();
    if(user->OpenDevice())
    {
        item->setText("设备已打开");
        ui->listWidget->addItem(item);
    }
//        qDebug() << "设备已打开。";
    else
    {
        item->setText("设备打开失败");
        ui->listWidget->addItem(item);
    }
//        qDebug() << "设备打开失败！";
    user->CanChannelConfig();
    user->ChannelStart();
}

void MainWindow::on_recieveButton_clicked()
{
    reciever->setStatus(RECIEVE);
    reciever->start();
    ui->recieveButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
}

void MainWindow::on_sendButton_clicked()
{
    sender->setStatus(SEND);
    sender->start();
    ui->sendButton->setEnabled(false);
    ui->stopButton->setEnabled(true);
}

void MainWindow::on_stopButton_clicked()
{
    if(reciever->isRunning())
    {
        reciever->stop();
        ui->recieveButton->setEnabled(true);
    }

    if(sender->isRunning())
    {
        sender->stop();
        ui->sendButton->setEnabled(true);
    }

    ui->stopButton->setEnabled(false);
}

void MainWindow::on_readInfoButton_clicked()
{
    deviceInfo = user->GetDeviceInfo();
    QStringList lst;
    for(auto iter = deviceInfo.begin(); iter != deviceInfo.end(); ++iter)
        lst << *iter;
    ui->listWidget->addItems(lst);
}

void MainWindow::on_showInfoButton_clicked()
{
    timer->start(100);
}

void MainWindow::on_stopShowInfoButton_clicked()
{
    timer->stop();
}

void MainWindow::initScatter()
{
    m_chart = new QChart();
    m_scatterSeries = new QScatterSeries();

    m_scatterSeries->setUseOpenGL(true);
    m_scatterSeries->setMarkerShape(QScatterSeries::MarkerShapeRectangle);
    m_scatterSeries->setMarkerSize(8);
    m_scatterSeries->setColor(QColor(255, 0, 0));

    QValueAxis* axis_h = new QValueAxis();
    QValueAxis* axis_v = new QValueAxis();
    axis_v->setRange(0.0, 30.0);
    axis_v->setTickCount(10);
    axis_v->setLabelFormat("%.1f");
    axis_h->setRange(-50.0, 50.0);
    axis_h->setTickCount(10);
    axis_h->setLabelFormat("%.1f");
    m_chart->addAxis(axis_h, Qt::AlignBottom);
    m_chart->addAxis(axis_v, Qt::AlignLeft);
    m_chart->addSeries(m_scatterSeries);
    m_scatterSeries->attachAxis(axis_h);
    m_scatterSeries->attachAxis(axis_v);

    m_chart->legend()->hide();

    ui->ClusterPointView->setChart(m_chart);
    ui->ClusterPointView->setRubberBand(QChartView::RectangleRubberBand);
    ui->ClusterPointView->chart()->setAnimationOptions(QChart::AllAnimations);
}

void MainWindow::showPoint()
{
    QList<QPointF> listPoint;
    m_scatterSeries->clear();
    for(auto iter = reciever->user_thread->cluster_vector.begin(); iter != reciever->user_thread->cluster_vector.end(); ++iter)
    {
        QPointF pointf;
        pointf.setX((*iter).Y * (-1));
        pointf.setY((*iter).X);
        listPoint << pointf;
    }
    m_scatterSeries->replace(listPoint);
}

void MainWindow::on_filterButton_clicked()
{
    Filter * filter = new Filter;
    filter->resize(800, 600);
    filter->setAttribute(Qt::WA_DeleteOnClose);
    filter->setWindowTitle("Filter");
    filter->show();

}

void MainWindow::on_closeButton_clicked()
{
    QListWidgetItem *item2 = new QListWidgetItem();
    if(user->CloseDevice())
    {
        item2->setText("设备已关闭");
        ui->listWidget->addItem(item2);
    }
}
