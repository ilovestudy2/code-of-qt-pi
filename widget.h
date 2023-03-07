#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtCharts>
#include <QTimer>
#include <QTime>
#include <QtSerialPort/QSerialPort>
#include <QString>
#include <QtCharts/QChartView>
#include <QtCharts/QSplineSeries>
#include <QDateTimeAxis>
#include <QValueAxis>
#include <QPen>
#include <QColor>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QFile>
#include <QDataStream>
#include <QSplineSeries>
#include <QVector>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    QSerialPort *serialPort;
    QTimer *timer_socket;
    QTimer *timer_alarm;
    QChart *chart_coil;
    QChart *chart_hall;
    QSplineSeries *series_coil;
    QSplineSeries *series_hall;
    QDateTimeAxis *axisX_coil;
    QValueAxis *axisY_coil;
    QDateTimeAxis *axisX_hall;
    QValueAxis *axisY_hall;
    QPen *pen;
    QFile *file_date_name;
    QDataStream datastream;
    QTcpServer *tcpserver;
    QTcpSocket *tcpsocket;
    QSplineSeries *splineseries_coil;
    QSplineSeries *splineseries_coil2;
    QSplineSeries *splineseries_hall;
    QSplineSeries *splineseries_hall2;
    char flag_tcpsocket_error = 0;
    char DAQ_socket_cmd = 'L';
    double distance_buff = 0;
    qint32 int_distance_calc = 8388608;
    QVector<QPointF> QVector_QPointF_data_buff; // 存储业务数据的 list
    QVector<QPointF> QVector_qpointf_coil_graph_data1;
    QVector<QPointF> QVector_qpointf_coil_graph_data2;
    QVector<QPointF> QVector_qpointf_hall_graph_data1;
    QVector<QPointF> QVector_qpointf_hall_graph_data2;
    long long detec_count = 0;
    int int_alarm_up_limit = 4000;
    int int_defect_nubmer = 0;
    int int_alarm_down_limit = -2000;
    int graph_x_axis_Count = 1600;//x-axis长度为4000
private slots:
    void on_startBt_clicked();
    void readyRead_slot();
    void on_stopBt_clicked();
    void timeout_socket_slot();
    void on_quitBt_clicked();
    void timeout_alarm_slot();
    void tcpsocket_connected_slot();
    void tcpsocket_readyRead_slot();
    void tcp_data_trans_together_func(QByteArray data_in_array);
    void thread_getdata_graph_plot(int data_in[]);//长度160

private:
    Ui::Widget *ui;

};

#endif // WIDGET_H
