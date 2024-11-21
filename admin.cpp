#include "admin.h"
#include "ui_admin.h"
#include "widget.h"

admin::admin(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::admin)


{

    ui->setupUi(this);
    QTimer *timer = new QTimer(this);
    timer->setInterval(1200);

    connect(timer, &QTimer::timeout, this, &admin::on_pushButton_clicked);
    connect(timer,&QTimer::timeout,this,&admin::on_pushButton_2_clicked);
    timer->start();
    m_timer = new QTimer(this);
    m_timer->setInterval(1000); // 设置定时间隔为1000毫秒
    connect(m_timer, &QTimer::timeout, this, &admin::ijiajia);
    connectserver();
    int plannedCount = ui->ui_productnumber->text().toInt(); // 计划的数量

    // 假设开始时完成的数量为0
    ui->progressBar->setMaximum(ui->ui_plannumber->text().toInt());
    ui->progressBar->setMinimum(0);//设置进度调的区间


    // 更新进度条的显示
    //updateProgressBar(completedCount, plannedCount);


    // 创建图标
    chart->setTitle("温度曲线");
    chartView->setChart(chart);
    //setCentralWidget(chartView);
    // 创建曲线序列
    series0->setName("温度");
    chart->addSeries(series0);
    chartView->setGeometry(700,0,300, 200);
    //创建坐标轴
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(0, 10);
    chart->setAxisX(axisX, series0);
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(-10, 50);
    chart->setAxisY(axisY, series0);
}

admin::~admin()
{
    delete ui;
}

void admin::showmead()
{
    this->show();
}
void admin::ijiajia()
{
    leftTime2--;
}

// void admin::updateProgressBar(int completed, int total) {
//     if (completed < 0 || total <= 0) {
//         // 防止除以0或负数的情况
//         return;
//     }
//     ui->progressBar->setValue((completed * 100) / total); // 设置进度条的值
// }





void admin::connectserver(){
    QOpcUaProvider provider; // 创建实例
    if (provider.availableBackends().isEmpty())
        return;
    if (!client) {
        client = provider.createClient(provider.availableBackends()[0]);
        if (!client)
            return;
    }//返回第一个后端标识符

    QObject::connect(client, &QOpcUaClient::stateChanged, [this](QOpcUaClient::ClientState state) {
        qDebug() << "Client state changed:" << state;
        if (state == QOpcUaClient::ClientState::Connected) {
            QOpcUaNode *node = client->node("ns=4;i=4");
            // ui->ui_statebox->setText("Connect Success");
            if (node)
                qDebug() << "A node object has been created";
            // ui->singlelight->setStyleSheet("background-color: green;");
        }
        // 修正else if语句
        else if (state == QOpcUaClient::ClientState::Closing || state == QOpcUaClient::ClientState::Disconnected)
        {
            // ui->singlelight->setStyleSheet("background-color:red");
            // ui->ui_statebox->setText("Connect Failed");
        }
    }); // 检查连接状态并且获取特定的节点返回状态

    QObject::connect(client, &QOpcUaClient::endpointsRequestFinished,
                     [this](QList<QOpcUaEndpointDescription> endpoints) {
                         qDebug() << "Endpoints returned:" << endpoints.count();
                         if (!endpoints.isEmpty())
                             client->connectToEndpoint(endpoints.first()); // 收到请求，打印端点数并链接第一个端点
                     });

    // QString url = ui->ui_urlad->text();
    client->requestEndpoints(QUrl("opc.tcp://192.168.0.10:4840")); // 发起请求
    //readNodeVlue();

}


// void admin::readNodeVlue() {
//     QList<QOpcUaReadItem> request;
//         QString nodeid = QString("ns=4;i=24"); // 构建节点 ID
//         request.push_back(QOpcUaReadItem(nodeid, QOpcUa::NodeAttribute::Value));
//     // 发起读取请求
//     bool success = client->readNodeAttributes(request);
//     if (!success) {
//         qDebug() << "Failed to initiate read operation";
//         //ui->ui_statebox->setText("Failed to initiate read operation");
//         return;
//     }

//     // 连接信号到槽函数以处理读取结果
//     connect(client, &QOpcUaClient::readNodeAttributesFinished, this, &admin::handleReadNodeAttributesFinished1);
// }



// void admin::handleReadNodeAttributesFinished1(const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult) {
//     if (serviceResult != QOpcUa::UaStatusCode::Good) {
//         qDebug() << "Read operation failed with serviceResult:" << serviceResult;
//         // ui->ui_statebox->setText("Read operation failed with serviceResult:" + QString::number(serviceResult));
//         return;
//     }

//     for (const QOpcUaReadResult &result : results) {
//         if (result.statusCode() == QOpcUa::UaStatusCode::Good) {
//             QString nodeid = result.nodeId();
//             strValue = result.value().toString();
//              ui->temp->setText(strValue);
//             qDebug()<<"******************************************************";
//         }
//     }
// }




//读值处理函数
void admin::handleReadNodeAttributesFinished(const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult) {
    if (serviceResult == QOpcUa::UaStatusCode::Good) {
        const QOpcUaReadResult &result = results[0];
        if (result.statusCode() == QOpcUa::UaStatusCode::Good) {
            qDebug() << "Value read for :"  << "is" << result.value();
            QString strvalue = result.value().toString();//读值转成字符串显示
            //ui->nodevalue->setText(strvalue);
            ui->temp->setText(strvalue);//获取温度丢到图表上
            double temper = strvalue.toDouble();

            series0->append(i, temper);
            i++;
            chart->update();
            if (series0->count() > 5) {
                chart->scroll(14,0); // 滚动一个数据点
            }
            if (temper>=40)
            {
                ui->ui_tempnormal->setStyleSheet("background-color:red;");
            }
            else if(temper<=40)
            {
                ui->ui_tempnormal->setStyleSheet("background-color:green;");
            }


            //readNodeVlue();
        } else {
            //qDebug() << "Failed to read value for '" << selectedText << "', error:" << result.statusCode();
        }
    } else {
        qDebug() << "Read operation failed, serviceResult:" << serviceResult;
    }
}

// 读值函数，一次性读取多个节点
// void admin::readNodeVlue() {

// }



//主页按钮
void admin::on_ui_back_clicked()
{
    this->hide();
    emit showmain();
}
//重绘图函数
void admin::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
//获取温度按钮
void admin::on_pushButton_clicked()
{
    // 声明请求列表并添加读取项
    QList<QOpcUaReadItem> request;
    request.push_back(QOpcUaReadItem("ns=4;i=24", QOpcUa::NodeAttribute::Value));

    // 发起读取请求
    bool success = client->readNodeAttributes(request);
    if (!success) {
        // 处理请求未成功发出的情况
        qDebug() << "Failed to initiate read operation";
        return;
    }

    // 断开可能已经存在的连接，避免重复连接
    disconnect(client, &QOpcUaClient::readNodeAttributesFinished, this, &admin::handleReadNodeAttributesFinished);

    // 连接信号到槽函数以处理读取结果
    QObject::connect(client, &QOpcUaClient::readNodeAttributesFinished,this, &admin::handleReadNodeAttributesFinished);
}

//获取所有节点函数
void admin::on_pushButton_2_clicked()
{
    QList<QOpcUaReadItem> request;
    for (int i = 0; i < 100; ++i) {
        QString nodeid = QString("ns=4;i=%1").arg(i); // 构建节点 ID
        request.push_back(QOpcUaReadItem(nodeid, QOpcUa::NodeAttribute::Value));
    }

    // 发起读取请求
    bool success = client->readNodeAttributes(request);
   // qDebug() << "Read operation initiated:" << success; // 调试语句
    if (!success) {
        //qDebug() << "Failed to initiate read operation"; // 调试语句
        return;
    }

    // 断开可能已经存在的连接，避免重复连接
    QObject::disconnect(client, &QOpcUaClient::readNodeAttributesFinished, this, &admin::handleReadNodeAttributesFinished1);
    //qDebug() << "Disconnected previous read finished connections"; // 调试语句

    // 连接信号到槽函数以处理读取结果
    connect(client, &QOpcUaClient::readNodeAttributesFinished, this, &admin::handleReadNodeAttributesFinished1);
    //qDebug() << "Connected read finished signal to slot"; // 调试语句
}

void admin::handleReadNodeAttributesFinished1(const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult) {
    //qDebug() << "Read operation finished with serviceResult:" << serviceResult; // 调试语句
    if (serviceResult != QOpcUa::UaStatusCode::Good) {
        //qDebug() << "Read operation failed"; // 调试语句
        return;
    }

    for (const QOpcUaReadResult &result : results) {
        if (result.statusCode() == QOpcUa::UaStatusCode::Good) {
            QString nodeid = result.nodeId();
            QString strValue = result.value().toString();
            //qDebug() << "Read value from node" << nodeid << "with value" << strValue; // 调试语句
            if(nodeid == qlltrobstop&&strValue=="false")
            {
                ui->ui_qllt->setText("正常");
            }
            else if (nodeid == qlltrobstop&&strValue == "true")
            {
                ui->ui_qllt->setText("手动停机");
            }
            else if (nodeid == qllsrobstop&&strValue == "false")
            {
                ui->ui_qlls->setText("正常");
            }
            else if(nodeid == qllsrobstop&&strValue == "true")
            {
                ui->ui_qlls->setText("手动停机");
            }
            else if (nodeid == qllmrobstop&&strValue == "false")
            {
                ui->ui_qllm->setText("正常");
            }
            else if(nodeid == qllmrobstop&&strValue == "true")
            {
                ui->ui_qllm->setText("手动停机");
            }
            else if (nodeid == hlltrobstop&&strValue == "false")
            {
                ui->ui_hllt->setText("正常");
            }
            else if(nodeid == hlltrobstop&&strValue == "true")
            {
                ui->ui_hllt->setText("手动停机");
            }
            else if (nodeid == hllsrobstop&&strValue == "false")
            {
                ui->ui_hlls->setText("正常");
            }
            else if(nodeid == hllsrobstop&&strValue == "true")
            {
                ui->ui_hlls->setText("手动停机");
            }
            else if (nodeid == hllmrobstop&&strValue == "false")
            {
                ui->ui_hllm->setText("正常");
            }
            else if(nodeid == hllmrobstop&&strValue == "true")
            {
                ui->ui_hllm->setText("手动停机");
            }
            else if (nodeid == zyazrobstop&&strValue == "false")
            {
                ui->ui_zyaz->setText("正常");
            }
            else if(nodeid == zyazrobstop&&strValue == "true")
            {
                ui->ui_zyaz->setText("手动停机");
            }
            else if (nodeid == zylsrobstop&&strValue == "false")
            {
                ui->ui_zyls->setText("正常");
            }
            else if(nodeid == zylsrobstop&&strValue == "true")
            {
                ui->ui_zyls->setText("手动停机");
            }
            else if(nodeid == allstop&&strValue=="true")
            {
                 ui->ui_stopall->setStyleSheet("background-color:red;");
            }
            else if(nodeid == allstop&&strValue=="false")
            {
                ui->ui_stopall->setStyleSheet("background-color:grey;");
            }
            else if(nodeid == allstart&&strValue=="false")
            {
                ui->ui_startall->setStyleSheet("background-color:grey;");
                m_timer->start();
            }
            else if(nodeid == allstart&&strValue=="true")
            {
                ui->ui_startall->setStyleSheet("background-color:green;");
                ui->ui_nomencar->setStyleSheet("background-color:green;");
            }
            else if(nodeid == allhome&&strValue=="false")
            {
                ui->ui_allhome->setStyleSheet("background-color:grey;");
            }
            else if(nodeid == allhome&&strValue=="true")
            {
                ui->ui_allhome->setStyleSheet("background-color:green;");
            }
            else if(nodeid == allemstop&&strValue=="true")
            {
                ui->ui_allemstop->setStyleSheet("background-color:red;");
            }
            else if(nodeid == allemstop&&strValue=="false")
            {
                ui->ui_allemstop->setStyleSheet("background-color:grey;");
            }
            else if(nodeid =="ns=4;i=73")
            {
                ui->ui_completenumber->setText(strValue);//获取完成的数量
                ui->progressBar->setValue(ui->ui_completenumber->text().toInt());//实时更新进度条，显示完成状况
                qDebug()<<strValue;
            }
            else if(nodeid =="ns=4;i=69")
            {
                shuchu = strValue.toInt();
            }
            else if(nodeid =="ns=4;i=76")
            {
                wancheng1 = strValue.toInt();
            }


            else if(nodeid == "ns=4;i=25")
            {
                shengchan = strValue.toInt();
                qDebug()<<"输出"<<shuchu<<"生产："<<shengchan<<"完成"<<wancheng;
                if(shuchu==shengchan)
                {
                    QOpcUaWriteItem writeItem("ns=4;i=71", QOpcUa::NodeAttribute::Value, true);
                    QList<QOpcUaWriteItem> request;
                    request.append(writeItem);
                    client->writeNodeAttributes(request);
                }
                else if(shuchu!=shengchan)
                {
                    QOpcUaWriteItem writeItem("ns=4;i=71", QOpcUa::NodeAttribute::Value, false);
                    QList<QOpcUaWriteItem> request;
                    request.append(writeItem);
                    client->writeNodeAttributes(request);
                }


                if(wancheng1==shengchan)
                {
                    QOpcUaWriteItem writeItem("ns=4;i=70", QOpcUa::NodeAttribute::Value, true);
                    QList<QOpcUaWriteItem> request;
                    request.append(writeItem);
                    client->writeNodeAttributes(request);
                    writeNodeValueWithDelay(allstart,false,500);
                    // QOpcUaWriteItem writeItem1 (allstart, QOpcUa::NodeAttribute::Value, false);
                    // QList<QOpcUaWriteItem> request1;
                    // request.append(writeItem1);
                    // client->writeNodeAttributes(request1);

                }
                else if(wancheng1!=shengchan)
                {
                    QOpcUaWriteItem writeItem("ns=4;i=70", QOpcUa::NodeAttribute::Value, false);
                    QList<QOpcUaWriteItem> request;
                    request.append(writeItem);
                    client->writeNodeAttributes(request);
                }

                ui->ui_lefttime->setText(QString::number(leftTime2)+"s");
                qDebug()<<leftTime2;



            }
        }
    }
}
//PLC回0函数
void admin::writeNodeValueWithDelay(const QString &nodeId, const QVariant &value, int delayMillis)
{
    // 创建一个QTimer对象
    QTimer *timer = new QTimer(this);
    // 设置计时器为单次触发
    timer->setSingleShot(true);

    // 存储参数，以便在计时器超时时使用
    QString nodeIdCopy = nodeId;
    QVariant valueCopy = value;

    // 连接计时器超时的信号到槽函数
    connect(timer, &QTimer::timeout, this, [this, nodeIdCopy, valueCopy]() {
        writeNodeValue(nodeIdCopy, valueCopy);
    });

    // 设置延迟时间并启动计时器
    timer->start(delayMillis);
}


void admin::writeNodeValue(const QString &nodeId, const QVariant &value) {
    // 创建写入请求项
    QOpcUaWriteItem writeItem(nodeId, QOpcUa::NodeAttribute::Value, value);
    QList<QOpcUaWriteItem> request;
    request.append(writeItem);
    // 执行写入操作
    client->writeNodeAttributes(request);
}



//产线开始按钮1
void admin::on_ui_startall_clicked()
{



    ui->ui_plannumber->setText(ui->ui_productnumber->text());

    startb = !startb;

    QOpcUaWriteItem writeItem(allstart, QOpcUa::NodeAttribute::Value, startb);
    QList<QOpcUaWriteItem> request;
    request.append(writeItem);
    client->writeNodeAttributes(request);


    // 假设 ui->ui_plannumber 包含一个整数字符串
    int planNumber = ui->ui_plannumber->text().toInt();
    int baseTime = 75; // 基础时间
    int additionalTime = 35; // 每次增加的时间

    // 计算剩余时间
    leftTime = baseTime + planNumber * additionalTime;
    leftTime2 = leftTime-20;
    m_timer->start();
    // 使用 QString::arg() 来格式化字符串


}

//产线停止按钮1
void admin::on_ui_stopall_clicked()
{
    allstopb = !allstopb;
    QOpcUaWriteItem writeItem(allstop, QOpcUa::NodeAttribute::Value, allstopb);
    QList<QOpcUaWriteItem> request;
    request.append(writeItem);
    client->writeNodeAttributes(request);
    if(wancheng==shengchan)
    {
        QMessageBox::information(this, "完成", "生产完成");
    }
}

//复位按钮1
void admin::on_ui_allhome_clicked()
{
    QOpcUaWriteItem writeItem(allhome, QOpcUa::NodeAttribute::Value, true);
    QList<QOpcUaWriteItem> request;
    request.append(writeItem);
    client->writeNodeAttributes(request);
    writeNodeValueWithDelay(allhome,false,1000);
}

//急停按钮
void admin::on_ui_allemstop_clicked()
{
    QOpcUaWriteItem writeItem(allemstop, QOpcUa::NodeAttribute::Value, true);
    QList<QOpcUaWriteItem> request;
    request.append(writeItem);
    client->writeNodeAttributes(request);
    writeNodeValueWithDelay(allemstop,false,1000);
}


void admin::on_ui_comfrimproduct_clicked()
{
    int productnumber = ui->ui_productnumber->text().toInt();
    // QOpcUaWriteItem writeItem("ns=4;i=25", QOpcUa::NodeAttribute::Value, 10);
    QList<QOpcUaWriteItem> request;
    request.append(QOpcUaWriteItem("ns=4;i=25", QOpcUa::NodeAttribute::Value, productnumber, QOpcUa::Types::Int16));
    client->writeNodeAttributes(request);
    QObject::connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &admin::handleWriteNodeAttributesFinished);
}

    void admin::handleWriteNodeAttributesFinished(const QList<QOpcUaWriteResult> &results, QOpcUa::UaStatusCode serviceResult)
    {
        if (serviceResult == QOpcUa::UaStatusCode::Good) {
            // 写入成功
            qDebug() << "Write operation succeeded";
            // ui->ui_statebox->setText( "Write operation succeeded");
        } else {
            // 写入失败，处理错误
            qDebug() << "Write operation failed, serviceResult:" << serviceResult;
            // ui->ui_statebox->setText( "Write operation failed, serviceResult:"+serviceResult);
        }
}
