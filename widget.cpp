#include "widget.h"
#include "ui_widget.h"
#include <QOpcUaProvider>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    //MQTT
    ui->ui_spinBoxport->setRange(0, 65535); // 允许输入 0 到 65535
    QTimer *later = new QTimer;
    m_client = new QMqttClient(this);
    m_client->setHostname(ui->ui_lineEditHost->text());
    m_client->setPort(ui->ui_spinBoxport->value());
    QObject::connect(m_client,&QMqttClient::stateChanged,this,&Widget::updateLogStateChange);
    QObject::connect(m_client,&QMqttClient::disconnected,this,&Widget::brokerDisconnected);
    QObject::connect(m_client, &QMqttClient::messageReceived, this, [this](const QByteArray &message, const QMqttTopicName &topic) {
        const QString content = QDateTime::currentDateTime().toString()
        + QLatin1String(" Received Topic: ")
            + topic.name()
            + QLatin1String(" Message: ")
            + message
            + QLatin1Char('\n');
        ui->ui_editlog->insertPlainText(content);
        recivemessage = message;
        recivetopic = topic.name();

        if(recivemessage == "R/NODESTATE")
        {
            sendnodestateontime();
        }
        //拆分
    else if (recivemessage.length() >= 2&&recivemessage.substr(0, 2)=="W/" )
    {
        QList<QOpcUaWriteItem> request;
        std::string firstTwoChars = recivemessage.substr(0, 2);
        std::string nodeid = recivemessage.substr(2,9);
        std::string value = recivemessage.substr(12,recivemessage.length());
        QString qnodeid = QString::fromStdString(nodeid);
        QString qvalue = QString::fromStdString(value);

        qDebug() << "Message received:" << message;

        //判断写入节点命令
            if(firstTwoChars == "W/")
            {
                if (qvalue=="true")
                {

                    request.append(QOpcUaWriteItem(qnodeid, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                }
                else if (ui->nodevalue->text()=="false")
                {

                    request.append(QOpcUaWriteItem(qnodeid, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
                }
                else
                {

                    request.append(QOpcUaWriteItem(qnodeid, QOpcUa::NodeAttribute::Value, qvalue, QOpcUa::Types::Int16));
                    qDebug()<<"**************************";
                }
                qDebug()<<"**************************";

                //发起写入请求
                bool success = client->writeNodeAttributes(request);
                if (!success) {
                    qDebug() << "Failed to initiate write operation";
                    ui->ui_statebox->setText("Failed to initiate write operation");
                    return;
                }

                // 连接信号到槽函数以处理写入结果
                // disconnect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished);
                connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished4);
            }

            }

        else if (recivemessage.length() >= 2&&recivemessage.substr(0, 4)=="ROB/" )
        {
            QList<QOpcUaWriteItem> request;
            std::string firstTwoChars = recivemessage.substr(0, 4);
            std::string nodeid = recivemessage.substr(4,4);
            std::string value = recivemessage.substr(9,recivemessage.length());
            QString qnodeid = QString::fromStdString(nodeid);
            QString qvalue = QString::fromStdString(value);

            qDebug() << "Message received:" << message;

            //判断写入节点命令
            if(firstTwoChars == "ROB/")
            {
                if (qvalue=="START"&&qnodeid=="QLLT")
                {

                    request.append(QOpcUaWriteItem(qlltrobstart, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(qlltrobstart,false,1000);
                }
                else if (qvalue=="STOP/ON"&&qnodeid=="QLLT")
                {
                    request.append(QOpcUaWriteItem(qlltrobstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="STOP/OFF"&&qnodeid=="QLLT")
                {
                    request.append(QOpcUaWriteItem(qlltrobstop, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="EMSTOP"&&qnodeid=="QLLT")
                {
                    request.append(QOpcUaWriteItem(qlltrobemstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(qlltrobemstop,false,3000);
                }
                else if (qvalue=="HOME"&&qnodeid=="QLLT")
                {
                    request.append(QOpcUaWriteItem(qlltrobhome, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(qlltrobemstop,false,3000);
                }

                else if (qvalue=="START"&&qnodeid=="QLLS")
                {
                    request.append(QOpcUaWriteItem(qllsrobstart, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                   writeNodeValueWithDelay(qllsrobstart,false,1000);
                }
                else if (qvalue=="STOP/ON"&&qnodeid=="QLLS")
                {
                    request.append(QOpcUaWriteItem(qllsrobstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="STOP/OFF"&&qnodeid=="QLLS")
                {
                    request.append(QOpcUaWriteItem(qllsrobstop, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="EMSTOP"&&qnodeid=="QLLS")
                {
                    request.append(QOpcUaWriteItem(qllsrobemstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(qllsrobemstop,false,3000);
                }
                else if (qvalue=="HOME"&&qnodeid=="QLLS")
                {
                    request.append(QOpcUaWriteItem(qllsrobhome, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(qllsrobhome,false,3000);
                }

                else if (qvalue=="START"&&qnodeid=="QLLM")
                {
                    request.append(QOpcUaWriteItem(qllmrobstart, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(qllmrobstart,false,1000);
                }
                else if (qvalue=="STOP/ON"&&qnodeid=="QLLM")
                {
                    request.append(QOpcUaWriteItem(qllmrobstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="STOP/OFF"&&qnodeid=="QLLM")
                {
                    request.append(QOpcUaWriteItem(qllmrobstop, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="EMSTOP"&&qnodeid=="QLLM")
                {
                    request.append(QOpcUaWriteItem(qllmrobemstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(qllmrobemstop,false,3000);
                }
                else if (qvalue=="HOME"&&qnodeid=="QLLM")
                {
                    request.append(QOpcUaWriteItem(qllmrobhome, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(qllmrobhome,false,1000);
                }


                else if (qvalue=="START"&&qnodeid=="HLLT")
                {
                    request.append(QOpcUaWriteItem(hlltrobstart, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                   writeNodeValueWithDelay(hlltrobstart,false,1000);
                }
                else if (qvalue=="STOP/ON"&&qnodeid=="HLLT")
                {
                    request.append(QOpcUaWriteItem(hlltrobstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="STOP/OFF"&&qnodeid=="HLLT")
                {
                    request.append(QOpcUaWriteItem(hlltrobstop, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="EMSTOP"&&qnodeid=="HLLT")
                {
                    request.append(QOpcUaWriteItem(hlltrobemstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(hlltrobemstop,false,3000);
                }
                else if (qvalue=="HOME"&&qnodeid=="HLLT")
                {
                    request.append(QOpcUaWriteItem(hlltrobhome, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(hlltrobhome,false,1000);
                }

                else if (qvalue=="START"&&qnodeid=="HLLS")
                {
                    request.append(QOpcUaWriteItem(hllsrobstart, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(hllsrobstart,false,1000);
                }
                else if (qvalue=="STOP/ON"&&qnodeid=="HLLS")
                {
                    request.append(QOpcUaWriteItem(hllsrobstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="STOP/OFF"&&qnodeid=="HLLS")
                {
                    request.append(QOpcUaWriteItem(hllsrobstop, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="EMSTOP"&&qnodeid=="HLLS")
                {
                    request.append(QOpcUaWriteItem(hllsrobemstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(hllsrobemstop,false,1000);
                }
                else if (qvalue=="HOME"&&qnodeid=="HLLS")
                {
                    request.append(QOpcUaWriteItem(hllsrobhome, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(hllsrobhome,false,1000);
                }

                else if (qvalue=="START"&&qnodeid=="HLLM")
                {
                    request.append(QOpcUaWriteItem(hllmrobstart, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(hllmrobstart,false,1000);
                }
                else if (qvalue=="STOP/ON"&&qnodeid=="HLLM")
                {
                    request.append(QOpcUaWriteItem(hllmrobstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="STOP/OFF"&&qnodeid=="HLLM")
                {
                    request.append(QOpcUaWriteItem(hllmrobstop, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="EMSTOP"&&qnodeid=="HLLM")
                {
                    request.append(QOpcUaWriteItem(hllmrobemstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(hllmrobemstop,false,1000);
                }
                else if (qvalue=="HOME"&&qnodeid=="HLLM")
                {
                    request.append(QOpcUaWriteItem(hllmrobhome, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(hllmrobhome,false,1000);
                }

                else if (qvalue=="START"&&qnodeid=="ZYAZ")
                {
                    request.append(QOpcUaWriteItem(zyazrobstart, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(zyazrobstart,false,1000);
                }
                else if (qvalue=="STOP/ON"&&qnodeid=="ZYAZ")
                {
                    request.append(QOpcUaWriteItem(zyazrobstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="STOP/OFF"&&qnodeid=="ZYAZ")
                {
                    request.append(QOpcUaWriteItem(zyazrobstop, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="EMSTOP"&&qnodeid=="ZYAZ")
                {
                    request.append(QOpcUaWriteItem(zyazrobemstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(zyazrobemstop,false,1000);
                }
                else if (qvalue=="HOME"&&qnodeid=="ZYAZ")
                {
                    request.append(QOpcUaWriteItem(zyazrobhome, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(zyazrobhome,false,1000);
                }


                else if (qvalue=="START"&&qnodeid=="ZYLS")
                {
                    request.append(QOpcUaWriteItem(zylsrobstart, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(zylsrobstart,false,1000);
                }
                else if (qvalue=="STOP/ON"&&qnodeid=="ZYLS")
                {
                    request.append(QOpcUaWriteItem(zylsrobstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="STOP/OFF"&&qnodeid=="ZYLS")
                {
                    request.append(QOpcUaWriteItem(zylsrobstop, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
                }
                else if (qvalue=="EMSTOP"&&qnodeid=="ZYLS")
                {
                    request.append(QOpcUaWriteItem(zylsrobemstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(zylsrobemstop,false,1000);
                }
                else if (qvalue=="HOME"&&qnodeid=="ZYLS")
                {
                    request.append(QOpcUaWriteItem(zylsrobhome, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(zylsrobhome,false,1000);
                }

                else if (qvalue=="START"&&qnodeid=="ALLL")
                {
                    request.append(QOpcUaWriteItem(allstart, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));

                }
                else if (qvalue=="STOP/ON"&&qnodeid=="ALLL")
                {
                    request.append(QOpcUaWriteItem(allstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(allstop,false,1000);
                }
                else if (qvalue=="HOME"&&qnodeid=="ALLL")
                {
                    request.append(QOpcUaWriteItem(allhome, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
                    writeNodeValueWithDelay(allhome,false,1000);
                }

                //发起写入请求
                bool success = client->writeNodeAttributes(request);
                if (!success) {
                    qDebug() << "Failed to initiate write operation";
                    ui->ui_statebox->setText("Failed to initiate write operation");
                    return;
                }

                // 连接信号到槽函数以处理写入结果
                // disconnect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished);
                connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished4);
            }

        }


        // if (recivemessage.length() >= 2&&recivemessage.substr(0, 4)=="ROB/")
        // {
        //     QList<QOpcUaWriteItem> request;
        //     std::string firstthreeChars = recivemessage.substr(0, 4); // "ROB/"
        //     std::string Rob = recivemessage.substr(4, 4); // "QLLT"
        //     std::string Order = recivemessage.substr(9,recivemessage.length()); // "START"
        //     QString rob = QString::fromStdString(Rob);
        //     QString order = QString::fromStdString(Order);
        //     QString robvalue = QString::fromStdString(Order);
        //     qDebug()<<"收到消息！！！";
        //     if(firstthreeChars=="ROB/")
        //     {
        //         qDebug()<<Rob<<Order;

        //         if (rob=="QLLT")
        //         {
        //             if(order=="START")
        //             {
        //                 request.append(QOpcUaWriteItem(qlltrobstart, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
        //             }
        //             // else if(order=="STOP/ON")
        //             // {

        //             // }
        //         }

        //         disconnect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished4);
        //         connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished4);
        //     }

        // }

    });
    QObject::connect(m_client, &QMqttClient::pingResponseReceived, this, [this]() {
        const QString content = QDateTime::currentDateTime().toString()
        + QLatin1String(" PingResponse")
            + QLatin1Char('\n');
        ui->ui_editlog->insertPlainText(content);
    });
    connect(ui->ui_lineEditHost, &QLineEdit::textChanged, m_client, &QMqttClient::setHostname);
    connect(ui->ui_spinBoxport, QOverload<int>::of(&QSpinBox::valueChanged), this, &Widget::setClientPort);
    updateLogStateChange();


    //OPCUA
    ui->nodelistbox->setCurrentIndex(-1);
    selectedText = ui->nodelistbox->currentText();
    ui->singlelight->setStyleSheet("background-color:red;");//服务器初始状态
    QObject::connect(ui->nodelistbox,QOverload<int>::of(&QComboBox::activated),this,&Widget::on_nodelistbox_activated);//节点信号的list
    QObject::connect(ui->ui_getnodestate,&QPushButton::clicked,[this](){timer->start(1000);});
    QObject::connect(timer,&QTimer::timeout,this,&Widget::readNodeVlue);
    std::vector<std::pair<std::string,int>> nodeandvalue;

}
Widget::~Widget()
{
    delete ui;
    delete client;
    delete node;
    if (timer->isActive())
    {
        timer->stop();
    }
    delete timer;
}




//发送节点状态到MQTT broker
void Widget::sendnodestateontime()
{
    QTextDocument *document = ui->ui_nodestatevalue->document();

    // 使用 QTextDocument 的 toPlainText() 来获取所有文本
    QString state = document->toPlainText();
    if (m_client->state() != QMqttClient::Connected) {
        qDebug() << "MQTT client is not connected.";
        return;
    }
    QString topic = recivetopic;
    // 将 QString 转换为 QByteArray，因为 publish 需要 QByteArray 类型的数据
    QByteArray payload = state.toUtf8();

    // 发送消息到指定的主题
    m_client->publish(topic, payload);

    // 打印调试信息，确认消息已发送
    qDebug() << "Message published to topic:" << topic;
}


void Widget::sendrobstate(QString message)
{
    if (m_client->state() != QMqttClient::Connected) {
        qDebug() << "MQTT client is not connected.";
        return;
    }
    QString topic = recivetopic;
    // 将 QString 转换为 QByteArray，因为 publish 需要 QByteArray 类型的数据
    QByteArray payload = message.toUtf8();

    // 发送消息到指定的主题
    m_client->publish(topic, payload);

    // 打印调试信息，确认消息已发送
    qDebug() << "Message published to topic:" << topic;
}


// 连接服务器按钮O
void Widget::on_connect_clicked()
{
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
            ui->ui_statebox->setText("Connect Success");
            if (node)
                qDebug() << "A node object has been created";
            ui->singlelight->setStyleSheet("background-color: green;");
        }
        // 修正else if语句
        else if (state == QOpcUaClient::ClientState::Closing || state == QOpcUaClient::ClientState::Disconnected)
        {
            ui->singlelight->setStyleSheet("background-color:red");
            ui->ui_statebox->setText("Connect Failed");
        }
    }); // 检查连接状态并且获取特定的节点返回状态

    QObject::connect(client, &QOpcUaClient::endpointsRequestFinished,
                     [this](QList<QOpcUaEndpointDescription> endpoints) {
                         qDebug() << "Endpoints returned:" << endpoints.count();
                         if (!endpoints.isEmpty())
                             client->connectToEndpoint(endpoints.first()); // 收到请求，打印端点数并链接第一个端点
                     });

    QString url = ui->ui_urlad->text();
    client->requestEndpoints(QUrl(url)); // 发起请求
}





//读值按钮
// void Widget::on_readvalue_clicked() {
//     // 声明请求列表并添加读取项
//     QList<QOpcUaReadItem> request;
//     request.push_back(QOpcUaReadItem(selectedText, QOpcUa::NodeAttribute::Value));

//     // 发起读取请求
//     bool success = client->readNodeAttributes(request);
//     if (!success) {
//         // 处理请求未成功发出的情况
//         qDebug() << "Failed to initiate read operation";
//         return;
//     }

//     // 断开可能已经存在的连接，避免重复连接
//     disconnect(client, &QOpcUaClient::readNodeAttributesFinished, this, &Widget::handleReadNodeAttributesFinished);

//     // 连接信号到槽函数以处理读取结果
//    QObject::connect(client, &QOpcUaClient::readNodeAttributesFinished,this, &Widget::handleReadNodeAttributesFinished);
// }

//读值处理函数（废弃）
// void Widget::handleReadNodeAttributesFinished(const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult) {
//     if (serviceResult == QOpcUa::UaStatusCode::Good) {
//         const QOpcUaReadResult &result = results[0];
//         if (result.statusCode() == QOpcUa::UaStatusCode::Good) {
//             qDebug() << "Value read for :" << selectedText << "is" << result.value();
//             QString strvalue = result.value().toString();//读值转成字符串显示
//             //ui->nodevalue->setText(strvalue);
//             QString node = selectedText;
//             ui->ui_nodelable->setText(node);
//         } else {
//             qDebug() << "Failed to read value for '" << selectedText << "', error:" << result.statusCode();
//         }
//     } else {
//         qDebug() << "Read operation failed, serviceResult:" << serviceResult;
//     }
// }




//节点选择槽函数O
void Widget::on_nodelistbox_activated(int index)
{
    qDebug() << "Activated index:" << index;
    // ui->nodevalue->setText();
    selectedText = ui->nodelistbox->currentText();
}




// 写入确认槽按钮O
void Widget::on_ui_confirmtochange_clicked()
{
    QList<QOpcUaWriteItem> request;
    if (ui->nodevalue->text()=="true")
    {

        request.append(QOpcUaWriteItem(selectedText, QOpcUa::NodeAttribute::Value, ui->nodevalue->text(), QOpcUa::Types::Boolean));
    }
    else if (ui->nodevalue->text()=="false")
    {

        request.append(QOpcUaWriteItem(selectedText, QOpcUa::NodeAttribute::Value, ui->nodevalue->text(), QOpcUa::Types::Boolean));
    }
    else
    {
        w_nodevalue = ui->nodevalue->text().toInt();
        request.append(QOpcUaWriteItem(selectedText, QOpcUa::NodeAttribute::Value, w_nodevalue, QOpcUa::Types::Int16));
    }
    qDebug()<<selectedText<<w_nodevalue<<"**************************";

    // 发起写入请求
    bool success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    disconnect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished);
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished);
}




// 写入结果处理函数O
void Widget::handleWriteNodeAttributesFinished(const QList<QOpcUaWriteResult> &results, QOpcUa::UaStatusCode serviceResult)
{
    if (serviceResult == QOpcUa::UaStatusCode::Good) {
        // 写入成功
        qDebug() << "Write operation succeeded";
        ui->ui_statebox->setText( "Write operation succeeded");
    } else {
        // 写入失败，处理错误
        qDebug() << "Write operation failed, serviceResult:" << serviceResult;
        ui->ui_statebox->setText( "Write operation failed, serviceResult:"+serviceResult);
    }
}


// 写入结果处理函数O
void Widget::handleWriteNodeAttributesFinished4(const QList<QOpcUaWriteResult> &results, QOpcUa::UaStatusCode serviceResult)
{
    if (serviceResult == QOpcUa::UaStatusCode::Good) {
        // 写入成功
        qDebug() << "Write operation succeeded";
        ui->ui_statebox->setText( "Write operation succeeded");
    } else {
        // 写入失败，处理错误
        qDebug() << "Write operation failed, serviceResult:" << serviceResult;
        ui->ui_statebox->setText( "Write operation failed, serviceResult:"+serviceResult);
    }
}






//实时状态查询按钮O
void Widget::on_ui_getnodestate_clicked()
{
    timer->start(500);
    // extractNodeIds(ui->ui_nodestatevalue);
}




// 读值函数，一次性读取多个节点
void Widget::readNodeVlue() {
    QList<QOpcUaReadItem> request;
    for (int i = 0; i < 100; ++i) {
        QString nodeid = QString("ns=4;i=%1").arg(i); // 构建节点 ID
        request.push_back(QOpcUaReadItem(nodeid, QOpcUa::NodeAttribute::Value));
    }

    // 发起读取请求
    bool success = client->readNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate read operation";
        ui->ui_statebox->setText("Failed to initiate read operation");
        return;
    }

    // 连接信号到槽函数以处理读取结果
    disconnect(client, &QOpcUaClient::readNodeAttributesFinished, this, &Widget::handleReadNodeAttributesFinished1);
    connect(client, &QOpcUaClient::readNodeAttributesFinished, this, &Widget::handleReadNodeAttributesFinished1);
}






// 读值处理函数（槽函数）
void Widget::handleReadNodeAttributesFinished1(const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult) {
    if (serviceResult != QOpcUa::UaStatusCode::Good) {
        //qDebug() << "Read operation failed with serviceResult:" << serviceResult;
        ui->ui_statebox->setText("Read operation failed with serviceResult:" + QString::number(serviceResult));
        return;
    }

    for (const QOpcUaReadResult &result : results) {
        if (result.statusCode() == QOpcUa::UaStatusCode::Good) {
            nodeid = result.nodeId();
            strValue = result.value().toString();
            QString selecetrob = ui->ui_selectrob->currentText();

            //启动按钮ui
            if (nodeid == qlltrobstart&&strValue =="false"&&selecetrob=="前轮轮胎机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:red;");
            }
            else if(nodeid == "ns=4;i=24")
            {
                temper = strValue;
            }
            else if (nodeid == qlltrobstart&&strValue == "true"&&selecetrob=="前轮轮胎机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == qllsrobstart&&strValue =="false"&&selecetrob=="前轮螺栓机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qllsrobstart&&strValue == "true"&&selecetrob=="前轮螺栓机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == qllmrobstart&&strValue =="false"&&selecetrob=="前轮螺母机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qllmrobstart&&strValue == "true"&&selecetrob=="前轮螺母机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hlltrobstart&&strValue =="false"&&selecetrob=="后轮轮胎机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hlltrobstart&&strValue == "true"&&selecetrob=="后轮轮胎机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hllsrobstart&&strValue =="false"&&selecetrob=="后轮螺栓机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hllsrobstart&&strValue == "true"&&selecetrob=="后轮螺栓机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hllmrobstart&&strValue =="false"&&selecetrob=="后轮螺母机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hllmrobstart&&strValue == "true"&&selecetrob=="后轮螺母机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == zyazrobstart&&strValue =="false"&&selecetrob=="座椅安装机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == zyazrobstart&&strValue == "true"&&selecetrob=="座椅安装机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == zylsrobstart&&strValue =="false"&&selecetrob=="座椅螺丝机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == zylsrobstart&&strValue == "true"&&selecetrob=="座椅螺丝机器人")
            {
                ui->ui_robstartbutton->setStyleSheet("background-color:green;");
            }

            //停止按钮ui

            if (nodeid == qlltrobstop&&strValue =="false"&&selecetrob=="前轮轮胎机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qlltrobstop&&strValue == "true"&&selecetrob=="前轮轮胎机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == qllsrobstop&&strValue =="false"&&selecetrob=="前轮螺栓机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qllsrobstop&&strValue == "true"&&selecetrob=="前轮螺栓机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == qllmrobstop&&strValue =="false"&&selecetrob=="前轮螺母机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qllmrobstop&&strValue == "true"&&selecetrob=="前轮螺母机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hlltrobstop&&strValue =="false"&&selecetrob=="后轮轮胎机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hlltrobstop&&strValue == "true"&&selecetrob=="后轮轮胎机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hllsrobstop&&strValue =="false"&&selecetrob=="后轮螺栓机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hllsrobstop&&strValue == "true"&&selecetrob=="后轮螺栓机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hllmrobstop&&strValue =="false"&&selecetrob=="后轮螺母机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hllmrobstop&&strValue == "true"&&selecetrob=="后轮螺母机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == zyazrobstop&&strValue =="false"&&selecetrob=="座椅安装机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == zyazrobstop&&strValue == "true"&&selecetrob=="座椅安装机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == zylsrobstop&&strValue =="false"&&selecetrob=="座椅螺丝机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == zylsrobstop&&strValue == "true"&&selecetrob=="座椅螺丝机器人")
            {
                ui->ui_robstopbutton->setStyleSheet("background-color:green;");
            }

            //复位按钮
            if (nodeid == qlltrobhome&&strValue =="false"&&selecetrob=="前轮轮胎机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qlltrobhome&&strValue == "true"&&selecetrob=="前轮轮胎机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == qllsrobhome&&strValue =="false"&&selecetrob=="前轮螺栓机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qllsrobhome&&strValue == "true"&&selecetrob=="前轮螺栓机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == qllmrobhome&&strValue =="false"&&selecetrob=="前轮螺母机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qllmrobhome&&strValue == "true"&&selecetrob=="前轮螺母机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hlltrobhome&&strValue =="false"&&selecetrob=="后轮轮胎机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hlltrobhome&&strValue == "true"&&selecetrob=="后轮轮胎机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hllsrobhome&&strValue =="false"&&selecetrob=="后轮螺栓机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hllsrobhome&&strValue == "true"&&selecetrob=="后轮螺栓机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hllmrobhome&&strValue =="false"&&selecetrob=="后轮螺母机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hllmrobhome&&strValue == "true"&&selecetrob=="后轮螺母机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == zyazrobhome&&strValue =="false"&&selecetrob=="座椅安装机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == zyazrobhome&&strValue == "true"&&selecetrob=="座椅安装机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == zylsrobhome&&strValue =="false"&&selecetrob=="座椅螺丝机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == zylsrobhome&&strValue == "true"&&selecetrob=="座椅螺丝机器人")
            {
                ui->ui_robhomebutton->setStyleSheet("background-color:green;");
            }


            //急停按钮ui
            if (nodeid == qlltrobemstop&&strValue =="false"&&selecetrob=="前轮轮胎机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qlltrobemstop&&strValue == "true"&&selecetrob=="前轮轮胎机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == qllsrobemstop&&strValue =="false"&&selecetrob=="前轮螺栓机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qllsrobemstop&&strValue == "true"&&selecetrob=="前轮螺栓机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == qllmrobemstop&&strValue =="false"&&selecetrob=="前轮螺母机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == qllmrobemstop&&strValue == "true"&&selecetrob=="前轮螺母机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hlltrobemstop&&strValue =="false"&&selecetrob=="后轮轮胎机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hlltrobemstop&&strValue == "true"&&selecetrob=="后轮轮胎机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hllsrobemstop&&strValue =="false"&&selecetrob=="后轮螺栓机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hllsrobemstop&&strValue == "true"&&selecetrob=="后轮螺栓机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == hllmrobemstop&&strValue =="false"&&selecetrob=="后轮螺母机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == hllmrobemstop&&strValue == "true"&&selecetrob=="后轮螺母机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == zyazrobemstop&&strValue =="false"&&selecetrob=="座椅安装机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == zyazrobemstop&&strValue == "true"&&selecetrob=="座椅安装机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == zylsrobemstop&&strValue =="false"&&selecetrob=="座椅螺丝机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:red;");
            }
            else if (nodeid == zylsrobemstop&&strValue == "true"&&selecetrob=="座椅螺丝机器人")
            {
                ui->ui_robemstopbutton->setStyleSheet("background-color:green;");
            }
            else if (nodeid == allstart&&strValue =="false")
            {
                ui->ui_allstart->setStyleSheet("background-color:grey;");
            }
            else if (nodeid == allstart&&strValue =="true")
            {
                ui->ui_allstart->setStyleSheet("background-color:green;");
            }
            else if (nodeid == allhome&&strValue =="false")
            {
                ui->ui_allhome->setStyleSheet("background-color:grey;");
            }
            else if (nodeid == allhome&&strValue =="true")
            {
                ui->ui_allhome->setStyleSheet("background-color:green;");
            }
            else if (nodeid == allstop&&strValue =="false")
            {
                ui->ui_allstop->setStyleSheet("background-color:grey;");
            }
            else if (nodeid == allstop&&strValue =="true")
            {
                ui->ui_allstop->setStyleSheet("background-color:red;");
            }
            else if (nodeid == conveyorstart&&strValue =="false")
            {
                ui->ui_conveyorstart->setStyleSheet("background-color:grey;");
            }
            else if (nodeid == conveyorstart&&strValue =="true")
            {
                ui->ui_conveyorstart->setStyleSheet("background-color:green;");
            }
            else if (nodeid == conveyorstop&&strValue =="false")
            {
                ui->ui_conveyorstop->setStyleSheet("background-color:grey;");
            }
            else if (nodeid == conveyorstop&&strValue =="true")
            {
                ui->ui_conveyorstop->setStyleSheet("background-color:red;");
            }
            else if (nodeid == allemstop&&strValue =="false")
            {
                ui->ui_allemstop->setStyleSheet("background-color:grey;");
            }
            else if (nodeid == allemstop&&strValue =="true")
            {
                ui->ui_allemstop->setStyleSheet("background-color:red;");
            }
            else if(nodeid==qlltrobstart&&strValue=="true")
            {
                sendrobstate("前轮轮胎安装机器人启动");
            }
            else if(nodeid==qllmrobstart&&strValue=="true")
            {
                sendrobstate("前轮螺母安装机器人启动");
            }
            else if(nodeid==qllsrobstart&&strValue=="true")
            {
                sendrobstate("前轮螺栓安装机器人启动");
            }

            else if(nodeid==hlltrobstart&&strValue=="true")
            {
                sendrobstate("后轮轮胎安装机器人启动");
            }
            else if(nodeid==hllmrobstart&&strValue=="true")
            {
                sendrobstate("后轮螺母安装机器人启动");
            }
            else if(nodeid==hllsrobstart&&strValue=="true")
            {
                sendrobstate("后轮螺栓安装机器人启动");
            }
            else if(nodeid==zyazrobstart&&strValue=="true")
            {
                sendrobstate("座椅安装机器人启动");
            }
            else if(nodeid==zylsrobstart&&strValue=="true")
            {
                sendrobstate("座椅螺丝安装机器人启动");
            }
            else if(nodeid=="ns=4;i=67"&&strValue=="true")
            {
                QOpcUaWriteItem writeItem(allstart, QOpcUa::NodeAttribute::Value, false);
                QList<QOpcUaWriteItem> request;
                request.append(writeItem);
                client->writeNodeAttributes(request);
            }
            else if(nodeid =="ns=4;i=69")
            {
                shuchu = strValue.toInt();
            }
            else if(nodeid =="ns=4;i=68")
            {
                wancheng = strValue.toInt();
            }
            else if(nodeid == "ns=4;i=25")
            {
                shengchan = strValue.toInt();
                if(shuchu==shengchan)
                {
                    QOpcUaWriteItem writeItem("ns=4;i=71", QOpcUa::NodeAttribute::Value, true);
                    QList<QOpcUaWriteItem> request;
                    request.append(writeItem);
                    client->writeNodeAttributes(request);
                }
                else if(wancheng==shengchan)
                {
                    QOpcUaWriteItem writeItem("ns=4;i=70", QOpcUa::NodeAttribute::Value, true);
                    QList<QOpcUaWriteItem> request;
                    request.append(writeItem);
                    client->writeNodeAttributes(request);
                }
            }

            addUniqueValue(nodeid, strValue);
        } else {
            //qDebug() << "Failed to read value for node ID:" << result.nodeId() << ", error:" << result.statusCode();
        }
    }
}



//用Qset函数筛除要更新的节点值的变化更新到qtextedit上O
void Widget::updateOrAddValue(QTextEdit* textEdit, const QString& nodeId, const QString& newValue) {
    QString textToFind = nodeId + ":"; // 构建查找文本，假设格式为 "nodeId:"
    bool found = false;

    QTextCursor cursor(textEdit->document());
    cursor.movePosition(QTextCursor::Start);

    while (!cursor.atEnd()) {
        cursor.select(QTextCursor::LineUnderCursor);
        if (cursor.selectedText().contains(textToFind)) {
            // 找到包含节点ID的行，更新整行数值
            cursor.removeSelectedText(); // 先删除选中的文本
            n = nodeId;
            QString nav2 = nodeId + ": " + newValue;
            cursor.insertText(nav2); // 然后插入新文本
            found = true;
            break;
        }
        cursor.movePosition(QTextCursor::NextBlock);
    }

    if (!found) {
        // 如果没有找到，添加新行到文本末尾
        textEdit->append(nodeId + ": " + newValue);
    }
}



//更新或添加新值到列表，更新id到qcomboboxO
void Widget::addUniqueValue(const QString& nodeId, const QString& newValue) {
    // 更新或添加值到 QTextEdit
    updateOrAddValue(ui->ui_nodestatevalue, nodeId, newValue);

    // 更新 QComboBox 以包含新的 nodeId
    QString currentSelectedText = ui->nodelistbox->currentText();
    extractNodeIds();
    ui->nodelistbox->setCurrentText(currentSelectedText);//为了防止刷新掉原来选中的节点
}



//延迟选择节点，一直刷新会出现问题O
void Widget::someFunction() {
    QTimer::singleShot(7000, this, &Widget::extractNodeIds);
}




//测试bool的写入功能（废弃）O
void Widget::on_ui_booltestbutton_clicked()
{

}






//正则表达式查询节点id添加到combobox控件O
void Widget::extractNodeIds() {
    QStringList nodeIds;
    // 正则表达式查找所有 nodeId
    QRegularExpression nodeIdPattern("ns=4;i=\\d+");
    QRegularExpressionMatchIterator matches = nodeIdPattern.globalMatch(ui->ui_nodestatevalue->toPlainText());

    while (matches.hasNext()) {
        QRegularExpressionMatch match = matches.next();
        nodeIds.append(match.captured());
    }

    // 清空现有的 QComboBox 项目
    ui->nodelistbox->clear();

    // 添加新的项目到 QComboBox
    ui->nodelistbox->addItems(nodeIds);
}


//断开连接按钮O
void Widget::on_ui_disconnectbutton_clicked()
{
    if (client && client->state() != QOpcUaClient::ClientState::Disconnected) {
        client->disconnectFromEndpoint(); // 断开连接
        ui->ui_statebox->setText("Disconnected"); // 更新状态信息
        ui->singlelight->setStyleSheet("background-color: gray;"); // 更新UI元素颜色

        // 可以在这里添加其他需要在断开连接时执行的代码
    }
}





//连接按钮槽函数
void Widget::on_ui_connectbutton_clicked()
{
    if (m_client->state() == QMqttClient::Disconnected) {
        ui->ui_lineEditHost->setEnabled(false);
        ui->ui_spinBoxport->setEnabled(false);
        ui->ui_connectbutton->setText(tr("断开连接"));
        m_client->connectToHost();
        qDebug()<<"连接服务器成功";
    } else {
        ui->ui_lineEditHost->setEnabled(true);
        ui->ui_spinBoxport->setEnabled(true);
        ui->ui_connectbutton->setText(tr("连接"));
        m_client->disconnectFromHost();
        qDebug()<<"连接服务器成功";
    }
}

//更新log槽函数
void Widget::updateLogStateChange()
{
    const QString content = QDateTime::currentDateTime().toString()
    + QLatin1String(": State Change")
        + QString::number(m_client->state())
        + QLatin1Char('\n');
    ui->ui_editlog->insertPlainText(content);
}

//取消连接服务器槽函数
void Widget::brokerDisconnected()
{
    ui->ui_lineEditHost->setEnabled(true);
    ui->ui_spinBoxport->setEnabled(true);
    ui->ui_connectbutton->setText(tr("Connect"));
}

//更改端口号槽函数
void Widget::setClientPort(int p)
{
     m_client->setPort(p);
}

//发布按钮槽函数
void Widget::on_ui_publishbutton_clicked()
{
    if (m_client->publish(ui->ui_lineedittopic->text(), ui->ui_lineeditMessage->text().toUtf8()) == -1)
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not publish message"));
    qDebug()<<"发布成功";
}


//订阅槽函数
void Widget::on_ui_subscribebutton_clicked()
{
    auto subscription = m_client->subscribe(ui->ui_lineedittopic->text());
    if (!subscription) {
        QMessageBox::critical(this, QLatin1String("Error"), QLatin1String("Could not subscribe. Is there a valid connection?"));
        return;
    }
    qDebug()<<"订阅成功";
}

//重绘图修复背景图片显示
void Widget::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

//跳转槽函数
void Widget::showme()
{
    this->show();
}
void Widget::on_ui_back_clicked()
{
    this->hide();
    emit showmain();
}





//机器人按钮更改值的槽函数
void Widget::handleWriteNodeAttributesFinished2(const QList<QOpcUaWriteResult> &results, QOpcUa::UaStatusCode serviceResult)
{
    if (serviceResult == QOpcUa::UaStatusCode::Good) {
        // 写入成功
        qDebug() << "Write operation succeeded";
        ui->ui_statebox->setText("Write operation succeeded");
    } else {
        // 写入失败，处理错误
        qDebug() << "Write operation failed, serviceResult:" << serviceResult;
        ui->ui_statebox->setText("Write operation failed, serviceResult:"+serviceResult);
    }
}

//机器人启动按下按钮
void Widget::on_ui_robstartbutton_pressed()
{
    QString robotType = ui->ui_selectrob->currentText();
    QString nodeIdentifier;
//判断选的机器人是什么
    if (robotType == "前轮轮胎机器人") {
        nodeIdentifier = qlltrobstart;
    } else if (robotType == "前轮螺栓机器人") {
        nodeIdentifier = qllsrobstart;
    } else if (robotType == "前轮螺母机器人"){
        nodeIdentifier = qllmrobstart;
    }
    else if (robotType == "后轮轮胎机器人"){
        nodeIdentifier = hlltrobstart;
    }else if (robotType == "后轮螺栓机器人"){
        nodeIdentifier = hllsrobstart;
    }else if (robotType == "后轮螺母机器人"){
        nodeIdentifier = hllmrobstart;
    }else if (robotType == "座椅安装机器人"){
        nodeIdentifier = zyazrobstart;
    }else if (robotType == "座椅螺丝机器人"){
        nodeIdentifier = zylsrobstart;
    }
    else if(robotType == "座椅安装机器人") {
        nodeIdentifier = zyazrobstart;
    }
    else if(robotType == "座椅螺丝机器人")
    {
        nodeIdentifier = zylsrobstart;
    }


    QList<QOpcUaWriteItem> request;
    bool success = prepareWriteRequest(nodeIdentifier, request);
    if (!success) {
        qDebug() << "Failed to prepare write operation";
        ui->ui_statebox->setText("Failed to prepare write operation");
        return;
    }
//成功就写入
    success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}
//更改true函数
bool Widget::prepareWriteRequest(const QString& nodeIdentifier, QList<QOpcUaWriteItem>& request)
{
    request.append(QOpcUaWriteItem(nodeIdentifier, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
    return true; // 返回准备请求的结果
}

//机器人启动松开按钮
void Widget::on_ui_robstartbutton_released()
{
    QString robotType = ui->ui_selectrob->currentText();
    QString nodeIdentifier;

    if (robotType == "前轮轮胎机器人") {
        nodeIdentifier = qlltrobstart;
    } else if (robotType == "前轮螺栓机器人") {
        nodeIdentifier = qllsrobstart;
    } else if (robotType == "前轮螺母机器人"){
        nodeIdentifier = qllmrobstart;
    }
    else if (robotType == "后轮轮胎机器人"){
        nodeIdentifier = hlltrobstart;
    }else if (robotType == "后轮螺栓机器人"){
        nodeIdentifier = hllsrobstart;
    }else if (robotType == "后轮螺母机器人"){
        nodeIdentifier = hllmrobstart;
    }
    else if(robotType == "座椅安装机器人") {
        nodeIdentifier = zyazrobstart;
    }
    else if(robotType == "座椅螺丝机器人")
    {
        nodeIdentifier = zylsrobstart;
    }

    QList<QOpcUaWriteItem> request;
    bool success = prepareWriteRequest2(nodeIdentifier, request);
    if (!success) {
        qDebug() << "Failed to prepare write operation";
        ui->ui_statebox->setText("Failed to prepare write operation");
        return;
    }

    success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}

//更改false函数
bool Widget::prepareWriteRequest2(const QString& nodeIdentifier, QList<QOpcUaWriteItem>& request)
{
    request.append(QOpcUaWriteItem(nodeIdentifier, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
    return true; // 返回准备请求的结果
}


void Widget::on_ui_robstopbutton_clicked()
{
    QString robotType = ui->ui_selectrob->currentText();
    QString nodeIdentifier;

    if (robotType == "前轮轮胎机器人") {
        nodeIdentifier = qlltrobstop;
    } else if (robotType == "前轮螺栓机器人") {
        nodeIdentifier = qllsrobstop;
    } else if (robotType == "前轮螺母机器人"){
        nodeIdentifier = qllmrobstop;
    }
    else if (robotType == "后轮轮胎机器人"){
        nodeIdentifier = hlltrobstop;
    }else if (robotType == "后轮螺栓机器人"){
        nodeIdentifier = hllsrobstop;
    }else if (robotType == "后轮螺母机器人"){
        nodeIdentifier = hllmrobstop;
    }
    else if(robotType == "座椅安装机器人") {
        nodeIdentifier = zyazrobstop;
    }
    else if(robotType == "座椅螺丝机器人")
    {
        nodeIdentifier = zylsrobstop;
    }
    buttonstate =!buttonstate;
    QList<QOpcUaWriteItem> request;
    request.append(QOpcUaWriteItem(nodeIdentifier, QOpcUa::NodeAttribute::Value, buttonstate, QOpcUa::Types::Boolean));
    qDebug()<<selectedText<<w_nodevalue<<"**************************";
    // 发起写入请求
    bool success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    disconnect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished);
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished);
}



//机器人复位按钮按下
void Widget::on_ui_robhomebutton_pressed()
{
    QString robotType = ui->ui_selectrob->currentText();
    QString nodeIdentifier;

    if (robotType == "前轮轮胎机器人") {
        nodeIdentifier = qlltrobhome;
    } else if (robotType == "前轮螺栓机器人") {
        nodeIdentifier = qllsrobhome;
    } else if (robotType == "前轮螺母机器人"){
        nodeIdentifier = qllmrobhome;
    }
    else if (robotType == "后轮轮胎机器人"){
        nodeIdentifier = hlltrobhome;
    }else if (robotType == "后轮螺栓机器人"){
        nodeIdentifier = hllsrobhome;
    }else if (robotType == "后轮螺母机器人"){
        nodeIdentifier = hllmrobhome;
    }else if (robotType == "座椅安装机器人"){
        nodeIdentifier = zyazrobhome;
    }else if (robotType == "座椅螺丝机器人"){
        nodeIdentifier = zylsrobhome;
    }
    else if(robotType == "座椅安装机器人") {
        nodeIdentifier = zyazrobhome;
    }
    else if(robotType == "座椅螺丝机器人")
    {
        nodeIdentifier = zylsrobhome;
    }


    QList<QOpcUaWriteItem> request;
    bool success = prepareWriteRequest(nodeIdentifier, request);
    if (!success) {
        qDebug() << "Failed to prepare write operation";
        ui->ui_statebox->setText("Failed to prepare write operation");
        return;
    }

    success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}

//机器人复位按钮抬起
void Widget::on_ui_robhomebutton_released()
{
    QString robotType = ui->ui_selectrob->currentText();
    QString nodeIdentifier;

    if (robotType == "前轮轮胎机器人") {
        nodeIdentifier = qlltrobhome;
    } else if (robotType == "前轮螺栓机器人") {
        nodeIdentifier = qllsrobhome;
    } else if (robotType == "前轮螺母机器人"){
        nodeIdentifier = qllmrobhome;
    }
    else if (robotType == "后轮轮胎机器人"){
        nodeIdentifier = hlltrobhome;
    }else if (robotType == "后轮螺栓机器人"){
        nodeIdentifier = hllsrobhome;
    }else if (robotType == "后轮螺母机器人"){
        nodeIdentifier = hllmrobhome;
    }else if (robotType == "座椅安装机器人"){
        nodeIdentifier = zyazrobhome;
    }else if (robotType == "座椅螺丝机器人"){
        nodeIdentifier = zylsrobhome;
    }
    else if(robotType == "座椅安装机器人") {
        nodeIdentifier = zyazrobhome;
    }
    else if(robotType == "座椅螺丝机器人")
    {
        nodeIdentifier = zylsrobhome;
    }


    QList<QOpcUaWriteItem> request;
    bool success = prepareWriteRequest2(nodeIdentifier, request);
    if (!success) {
        qDebug() << "Failed to prepare write operation";
        ui->ui_statebox->setText("Failed to prepare write operation");
        return;
    }

    success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}





 void Widget::changestate(QString nodeid,QString value)
{
    QList<QOpcUaWriteItem> request;
    request.append(QOpcUaWriteItem(nodeid, QOpcUa::NodeAttribute::Value, value, QOpcUa::Types::Boolean));
}

void Widget::writeNodeValueWithDelay(const QString &nodeId, const QVariant &value, int delayMillis)
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


void Widget::writeNodeValue(const QString &nodeId, const QVariant &value) {
    // 创建写入请求项
    QOpcUaWriteItem writeItem(nodeId, QOpcUa::NodeAttribute::Value, value);
    QList<QOpcUaWriteItem> request;
    request.append(writeItem);
    // 执行写入操作
    client->writeNodeAttributes(request);
}





void Widget::on_ui_robemstopbutton_released()
{
    QString robotType = ui->ui_selectrob->currentText();
    QString nodeIdentifier;

    if (robotType == "前轮轮胎机器人") {
        nodeIdentifier = qlltrobemstop;
    } else if (robotType == "前轮螺栓机器人") {
        nodeIdentifier = qllsrobemstop;
    } else if (robotType == "前轮螺母机器人"){
        nodeIdentifier = qllmrobemstop;
    }
    else if (robotType == "后轮轮胎机器人"){
        nodeIdentifier = hlltrobemstop;
    }else if (robotType == "后轮螺栓机器人"){
        nodeIdentifier = hllsrobemstop;
    }else if (robotType == "后轮螺母机器人"){
        nodeIdentifier = hllmrobemstop;
    }else if (robotType == "座椅安装机器人"){
        nodeIdentifier = zyazrobemstop;
    }else if (robotType == "座椅螺丝机器人"){
        nodeIdentifier = zylsrobemstop;
    }
    else if(robotType == "座椅安装机器人") {
        nodeIdentifier = zyazrobemstop;
    }
    else if(robotType == "座椅螺丝机器人")
    {
        nodeIdentifier = zylsrobemstop;
    }


    QList<QOpcUaWriteItem> request;
    bool success = prepareWriteRequest2(nodeIdentifier, request);
    if (!success) {
        qDebug() << "Failed to prepare write operation";
        ui->ui_statebox->setText("Failed to prepare write operation");
        return;
    }

    success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}


void Widget::on_ui_robemstopbutton_pressed()
{
    QString robotType = ui->ui_selectrob->currentText();
    QString nodeIdentifier;

    if (robotType == "前轮轮胎机器人") {
        nodeIdentifier = qlltrobemstop;
    } else if (robotType == "前轮螺栓机器人") {
        nodeIdentifier = qllsrobemstop;
    } else if (robotType == "前轮螺母机器人"){
        nodeIdentifier = qllmrobemstop;
    }
    else if (robotType == "后轮轮胎机器人"){
        nodeIdentifier = hlltrobemstop;
    }else if (robotType == "后轮螺栓机器人"){
        nodeIdentifier = hllsrobemstop;
    }else if (robotType == "后轮螺母机器人"){
        nodeIdentifier = hllmrobemstop;
    }else if (robotType == "座椅安装机器人"){
        nodeIdentifier = zyazrobemstop;
    }else if (robotType == "座椅螺丝机器人"){
        nodeIdentifier = zylsrobemstop;
    }
    else if(robotType == "座椅安装机器人") {
        nodeIdentifier = zyazrobemstop;
    }
    else if(robotType == "座椅螺丝机器人")
    {
        nodeIdentifier = zylsrobemstop;
    }


    QList<QOpcUaWriteItem> request;
    bool success = prepareWriteRequest(nodeIdentifier, request);
    if (!success) {
        qDebug() << "Failed to prepare write operation";
        ui->ui_statebox->setText("Failed to prepare write operation");
        return;
    }

    success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}






//传送带启动按钮
void Widget::on_ui_conveyorstart_clicked()
{
    constart=!constart;
    QList<QOpcUaWriteItem> request;
    request.append(QOpcUaWriteItem(conveyorstart, QOpcUa::NodeAttribute::Value, constart, QOpcUa::Types::Boolean));
    bool success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}
//传送带停止按钮
void Widget::on_ui_conveyorstop_clicked()
{

    QList<QOpcUaWriteItem> request;
    request.append(QOpcUaWriteItem(conveyorstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
    bool success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }
    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
    writeNodeValueWithDelay(conveyorstop,false,1000);
}


//产线开始按钮
void Widget::on_ui_allstart_clicked()
{
    allstartb =!allstartb;
    QList<QOpcUaWriteItem> request;
    request.append(QOpcUaWriteItem(allstart, QOpcUa::NodeAttribute::Value, allstartb, QOpcUa::Types::Boolean));
    bool success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}

//产线停止按钮
void Widget::on_ui_allstop_clicked()
{
    allstopb = !allstopb;
    QList<QOpcUaWriteItem> request;
    request.append(QOpcUaWriteItem(allstop, QOpcUa::NodeAttribute::Value, allstopb, QOpcUa::Types::Boolean));
    bool success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}


//产线复位按钮按下
void Widget::on_ui_allhome_pressed()
{
    QList<QOpcUaWriteItem> request;
    request.append(QOpcUaWriteItem(allhome, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
    bool success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}
void Widget::on_ui_allhome_released()
{
    QList<QOpcUaWriteItem> request;
    request.append(QOpcUaWriteItem(allhome, QOpcUa::NodeAttribute::Value, false, QOpcUa::Types::Boolean));
    bool success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
}




//产线急停止按下
void Widget::on_ui_allemstop_clicked()
{
    QList<QOpcUaWriteItem> request;
    request.append(QOpcUaWriteItem(allemstop, QOpcUa::NodeAttribute::Value, true, QOpcUa::Types::Boolean));
    bool success = client->writeNodeAttributes(request);
    if (!success) {
        qDebug() << "Failed to initiate write operation";
        ui->ui_statebox->setText("Failed to initiate write operation");
        return;
    }

    // 连接信号到槽函数以处理写入结果
    connect(client, &QOpcUaClient::writeNodeAttributesFinished, this, &Widget::handleWriteNodeAttributesFinished2);
    writeNodeValueWithDelay(allemstop,false,1000);
}


