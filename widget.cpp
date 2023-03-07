#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{

    ui->setupUi(this);

    serialPort = new QSerialPort(this);
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data5);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setPortName("com5");
    serialPort->open(QSerialPort::ReadWrite);

    connect(serialPort,SIGNAL(readyRead()),this,SLOT(readyRead_slot()));
    QPen peny(Qt::darkRed , 3 , Qt::SolidLine , Qt::RoundCap , Qt::RoundJoin);
//    QPen(darkRed,)

    chart_coil = new QChart();
    chart_hall = new QChart();
    timer_socket = new QTimer(this);
    timer_alarm = new QTimer(this);

    series_coil = new QSplineSeries(this);
    series_hall = new QSplineSeries(this);
    axisX_coil = new QDateTimeAxis();
    axisY_coil = new QValueAxis();
    axisX_hall = new QDateTimeAxis();
    axisY_hall = new QValueAxis();

    file_date_name = new QFile();

//    datastream = new QDataStream();

    tcpserver = new QTcpServer(this);
    tcpsocket = new QTcpSocket(this);

    splineseries_coil = new QSplineSeries(this);
    splineseries_coil2 = new QSplineSeries(this);
    splineseries_hall = new QSplineSeries(this);
    splineseries_hall2 = new QSplineSeries(this);
    connect(tcpsocket,SIGNAL(connected()),this,SLOT(tcpsocket_connected_slot()));

    chart_coil->legend()->hide(); //隐藏图例
    chart_coil->addSeries(series_coil);//添加线
//  axisX_coil->setTickCount(8);//设置坐标轴格数
    axisY_coil->setTickCount(6);

//    axisX_coil->setFormat("hh:mm:ss");
    axisY_coil->setMin(0);//最小值
    axisY_coil->setMax(200);//最大值
//    axisX_coil->setTitleText("hour");//设置横坐标标题
    axisY_coil->setLinePenColor(QColor(Qt::darkBlue));//设置坐标轴颜色
    axisY_coil->setGridLineColor(QColor(Qt::darkBlue));
    axisY_coil->setGridLineVisible(true);//y轴栅格不可见
    axisY_coil->setLinePen(peny);
//    axisX_coil->setLinePen(peny);


    chart_hall->legend()->hide(); //隐藏图例
    chart_hall->addSeries(series_hall);//添加线
//    axisX_hall->setTickCount(8);//设置坐标轴格数
    axisY_hall->setTickCount(6);

//    axisX_hall->setFormat("hh:mm:ss");
    axisY_hall->setMin(0);//最小值
    axisY_hall->setMax(200);//最大值
//    axisX_hall->setTitleText("hour");//设置横坐标标题
    axisY_hall->setLinePenColor(QColor(Qt::darkBlue));//设置坐标轴颜色
    axisY_hall->setGridLineColor(QColor(Qt::darkBlue));
    axisY_hall->setGridLineVisible(true);//y轴栅格不可见
    axisY_hall->setLinePen(peny);
//    axisX_hall->setLinePen(peny);
    chart_coil->setTheme(QtCharts::QChart::ChartThemeDark);
//    chart_coil->addAxis(axisX_coil,Qt::AlignBottom);
    chart_coil->addAxis(axisY_coil,Qt::AlignLeft);

//    chart_hall->addAxis(axisX_hall,Qt::AlignBottom);
    chart_hall->setTheme(QtCharts::QChart::ChartThemeDark);
    chart_hall->addAxis(axisY_hall,Qt::AlignLeft);

//    series_coil->attachAxis(axisX_coil);
    series_coil->attachAxis(axisY_coil);

//    series_hall->attachAxis(axisX_hall);
    series_hall->attachAxis(axisY_hall);

//    axisY_coil->setTitleText("Y1");
//    axisY_hall->setTitleText("Y2");

    ui->coilwidget->setChart(chart_coil);
    ui->coilwidget->setRenderHint(QPainter::Antialiasing);

    ui->hallwidget->setChart(chart_hall);
    ui->hallwidget->setRenderHint(QPainter::Antialiasing);


    connect(timer_socket,SIGNAL(timeout()),this,SLOT(timeout_socket_slot()));
    connect(timer_alarm,SIGNAL(timeout()),this,SLOT(timeout_alarm_slot()));

    timer_alarm->start(500);

    if (QVector_qpointf_coil_graph_data1.isEmpty())//开辟空间
    {
        QVector_qpointf_coil_graph_data1.reserve(graph_x_axis_Count);
        QVector_qpointf_coil_graph_data2.reserve(graph_x_axis_Count);
        QVector_qpointf_hall_graph_data1.reserve(graph_x_axis_Count);
        QVector_qpointf_hall_graph_data2.reserve(graph_x_axis_Count);
        for (int i = 0; i < graph_x_axis_Count; ++i)
        {
            QVector_qpointf_coil_graph_data1.append(QPointF(i, 0));
            QVector_qpointf_coil_graph_data2.append(QPointF(i, 0));
            QVector_qpointf_hall_graph_data1.append(QPointF(i, 0));
            QVector_qpointf_hall_graph_data2.append(QPointF(i, 0));
        }
    }

    //qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

    //增加新的点到曲线末尾
   // series->append(currentTime.toMSecsSinceEpoch() , number);


}
void Widget::on_startBt_clicked()
{
    qDebug("good");
    QDateTime current_date_time=QDateTime::currentDateTime();//系统时间
    QString current_date=current_date_time.toString("yyyyMMddhhmmsszzzddd");
    QString file_name="./date_dir"+current_date+".txt";
    qDebug()<<file_name;
    file_date_name->setFileName(file_name);//创建文件
    bool file_open_sucess=file_date_name->open(QIODevice::WriteOnly);
    if(file_open_sucess == true)
    {
        datastream.setDevice(file_date_name);
    }
    if(timer_socket->isActive() == false)
    {
        timer_socket->start(1000);
    }
    tcpsocket->abort();
    tcpsocket->connectToHost("192.168.1.10",1000,QAbstractSocket::ReadWrite, QAbstractSocket::IPv4Protocol);//连接服务器
    if(this->flag_tcpsocket_error == 1){
        this->flag_tcpsocket_error = 0;
        file_date_name->close();
        return;
    }
}

void Widget::tcpsocket_connected_slot()
{
    static bool flag_connect = false;
    qDebug()<<"sucess connecting";
    if(timer_socket->isActive() == true)
    {
        timer_socket->stop();
    }
    if(flag_connect == false)//只连接一次槽函数，否则报错
    {
        connect(tcpsocket, SIGNAL(readyRead()),this,SLOT(tcpsocket_readyRead_slot()));
        flag_connect = true;
    }
    char char_buff = 'C';
    tcpsocket->write(&char_buff,1);
    QTime dieTime = QTime::currentTime().addMSecs(1000);
    while( QTime::currentTime() < dieTime )
      QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
    char_buff = 'L';
    tcpsocket->write(&char_buff,1);

}
void Widget::tcpsocket_readyRead_slot()
{
    static int flag_number = 0;
    switch (flag_number)
    {
    case 0:
        DAQ_socket_cmd = 'L';
        flag_number++;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        DAQ_socket_cmd = 'A';
        flag_number++;
        break;
    case 10:
        DAQ_socket_cmd = 'H';
        flag_number++;
        break;
    case 11:
    case 12:
    case 13:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
        DAQ_socket_cmd = 'A';
        flag_number++;
        break;
    case 19:
        DAQ_socket_cmd = 'A';
        flag_number = 0;
        break;
    }
    QByteArray array = tcpsocket->readAll();
    //QThread::msleep(12);
    tcpsocket->write(&DAQ_socket_cmd,1);
    tcp_data_trans_together_func(array);

//    if(flag_cloud_realTime_upLoad == 1){
//        QTcpSocket_cloud.write(array);//将数据上传到服务器
//        qDebug()<<array.size();
//    }

}
void Widget::tcp_data_trans_together_func(QByteArray data_in_array)//高八位低八位数据整合
{
    int data_return[160];
    int data_buff[640];
    int i = 0;
    for (i=0;i<640;i++)
    {
        data_buff[i] = data_in_array[2*i] + data_in_array[2*i+1]*256;
        //QDataStream_detec_data<<(qint32)data_buff[i];
    }
    //QDataStream_detec_data<<distance_buff;
    for (i=0;i<40;i++)
    {
        data_return[4*i] = data_buff[i*16+1]+data_buff[i*16+2]+data_buff[i*16+3]+data_buff[i*16+4]+data_buff[i*16+5]+data_buff[i*16+6]-10000;
        data_return[4*i+1] = data_buff[i*16+0]+data_buff[i*16+7]+data_buff[i*16+8]+data_buff[i*16+13]+data_buff[i*16+14]+data_buff[i*16+15]-10000;
        data_return[4*i+2] = data_buff[i*16+10];
        data_return[4*i+3] = data_buff[i*16+11];


        datastream<<(qint32)data_return[4*i];
        datastream<<(qint32)data_return[4*i+1];
        datastream<<(qint32)data_return[4*i+2];
        datastream<<(qint32)data_return[4*i+3];
        datastream<<distance_buff;
    }
    int_distance_calc = (uint8_t)data_in_array[18] + (uint8_t)data_in_array[19]*256+(uint8_t)data_in_array[24]*65536 ;//将位移信息写入变量
    thread_getdata_graph_plot(data_return);
}

void Widget::thread_getdata_graph_plot(int data_in[])//长度160
{
    static int i = 0;
    static int x_axis = 0;
    int flag_defect_detec = 1;//损伤判断标志位
    if(x_axis>=1600)
    {
        x_axis = 0;
        splineseries_coil->replace(QVector_qpointf_coil_graph_data1);
        splineseries_coil2->replace(QVector_qpointf_coil_graph_data2);
        splineseries_hall->replace(QVector_qpointf_hall_graph_data1);
        splineseries_hall2->replace(QVector_qpointf_hall_graph_data2);
        detec_count+=800;
//        ui->label_axisX1->setText(QString("%1ms").arg(detec_count));
//        ui->label_axisX2->setText(QString("%1ms").arg(detec_count+1600));
    }
    else if(x_axis%1600==800)
    {
        splineseries_coil->replace(QVector_qpointf_coil_graph_data1);
        splineseries_coil2->replace(QVector_qpointf_coil_graph_data2);
        splineseries_hall->replace(QVector_qpointf_hall_graph_data1);
        splineseries_hall2->replace(QVector_qpointf_hall_graph_data2);
        detec_count+=800;
//        ui->label_axisX1->setText(QString("%1ms").arg(detec_count));
//        ui->label_axisX2->setText(QString("%1ms").arg(detec_count+1600));
    }
    /*
    for(i=0;i<40;i++)
    {
        int_filter_data_buff[i] = int_filter_data_buff[i+40];
        int_filter_data_buff[i+40] = data_in[4*i];
    }
    */
    //data_filter_func(int_filter_data_buff);
    for(i=0;i<40;i++)
    {
        //qDebug()<<int_after_filter_data[40+i];
        //QVector_QPointF_coil_graph_data1[x_axis].setY(int_after_filter_data[40+i]);
        QVector_qpointf_coil_graph_data1[x_axis].setY(data_in[4*i+0]);
        QVector_qpointf_coil_graph_data2[x_axis].setY(data_in[4*i+1]);
        QVector_qpointf_hall_graph_data1[x_axis].setY(data_in[4*i+2]);
        QVector_qpointf_hall_graph_data2[x_axis].setY(data_in[4*i+3]);
        x_axis++;
        if(flag_defect_detec ==1)//损伤判断
        {
            if(data_in[4*i+0]>int_alarm_up_limit)
            {
                flag_defect_detec = 0;
                int_defect_nubmer++;
            }
            else if(data_in[4*i+0]<int_alarm_down_limit)
            {
                flag_defect_detec = 0;
                int_defect_nubmer++;
            }
            else if(data_in[4*i+1]>int_alarm_up_limit)
            {
                flag_defect_detec = 0;
                int_defect_nubmer++;
            }
            else if(data_in[4*i+1]<int_alarm_down_limit)
            {
                flag_defect_detec = 0;
                int_defect_nubmer++;
            }
        }
    }
}

Widget::~Widget()
{
    delete ui;
}


void Widget::readyRead_slot()
{
   QString buf;
   buf = QString(serialPort->readAll());

}

void Widget::timeout_socket_slot()
{



}

void Widget::timeout_alarm_slot()
{



}


void Widget::on_stopBt_clicked()
{

}

void Widget::on_quitBt_clicked()
{
    qDebug()<<"quit";
    timer_alarm->stop();
    this->close();
}
