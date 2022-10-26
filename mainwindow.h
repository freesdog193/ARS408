#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "ICANCmd.h"
#include "canuser.h"
#include "canthread.h"
#include <QTableWidgetItem>
#include <QTimer>
#include <QtCharts>
#include <QtCharts/QScatterSeries>
#include <QChartView>
#include <QPointF>
#include "filter.h"
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_openButton_clicked();

    void on_recieveButton_clicked();

    void on_stopButton_clicked();

    void on_sendButton_clicked();

    void on_readInfoButton_clicked();

    void on_showInfoButton_clicked();

    void on_stopShowInfoButton_clicked();

    void on_filterButton_clicked();

    void on_closeButton_clicked();

private:
    Ui::MainWindow *ui;
    CanUser *user;
    CanThread *reciever;
    CanThread *sender;
    QTimer *timer;
    QChart *m_chart;
    QScatterSeries *m_scatterSeries;
//    QListWidgetItem * item;

    QVector<QString> deviceInfo;

    void initScatter();
    void showPoint();

};
#endif // MAINWINDOW_H
