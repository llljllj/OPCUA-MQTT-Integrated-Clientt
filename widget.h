#ifndef WIDGET_H
#define WIDGET_H
#include <QOpcUaType.h>
#include <QOpcUaProvider>
#include <QWidget>
#include <QOpcUaClient.h>
#include <QOpcUaClient>
#include <QOpcUaNode>
#include <QComboBox>
#include <QTimer>
#include <qopcuanode.h>
#include <QQueue>
#include <qopcuanodeids.h>
#include <QTextEdit>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QThread>
#include <QtCore/QDateTime>
#include <QtMqtt/QMqttClient>
#include <QtWidgets/QMessageBox>
#include <QMetaObject>
#include <QDebug>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
#include <string>
#include <string.h>
QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT
signals:
    void showmain();


public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    //MQTT
    QMqttClient *m_client = nullptr;
    void updateLogStateChange();//测试
    void sendnodestateontime();//定时给终端发消息
    std::string recivemessage = "";
    QString recivetopic = "";
    //OPCUA
    //前轮机器人节点
    QString qlltrobstart = "ns=4;i=29";
    QString qlltrobstop = "ns=4;i=30";
    QString qlltrobhome = "ns=4;i=31";
    QString qlltrobemstop = "ns=4;i=51";
    //前轮螺栓机器人节点
    QString qllsrobstart = "ns=4;i=41";
    QString qllsrobstop = "ns=4;i=42";
    QString qllsrobhome = "ns=4;i=43";
    QString qllsrobemstop = "ns=4;i=55";
    //前轮螺母机器人节点
    QString qllmrobstart = "ns=4;i=35";
    QString qllmrobstop = "ns=4;i=36";
    QString qllmrobhome = "ns=4;i=37";
    QString qllmrobemstop = "ns=4;i=53";
    //后轮机器人节点
    QString hlltrobstart = "ns=4;i=26";
    QString hlltrobstop = "ns=4;i=27";
    QString hlltrobhome = "ns=4;i=28";
    QString hlltrobemstop = "ns=4;i=50";
    //后轮螺栓机器人节点
    QString hllsrobstart = "ns=4;i=38";
    QString hllsrobstop = "ns=4;i=39";
    QString hllsrobhome ="ns=4;i=40";
    QString hllsrobemstop ="ns=4;i=54";
    //后轮螺母机器人节点
    QString hllmrobstart = "ns=4;i=32";
    QString hllmrobstop = "ns=4;i=33";
    QString hllmrobhome ="ns=4;i=34";
    QString hllmrobemstop ="ns=4;i=50";
    //座椅安装机器人
    QString zyazrobstart = "ns=4;i=44";
    QString zyazrobstop = "ns=4;i=45";
    QString zyazrobhome = "ns=4;i=46";
    QString zyazrobemstop = "ns=4;i=56";
    //座椅螺丝刀机器人
    QString zylsrobstart ="ns=4;i=47";
    QString zylsrobstop = "ns=4;i=48";
    QString zylsrobhome ="ns=4;i=49";
    QString zylsrobemstop ="ns=4;i=57";
    //产线节点
    QString allstart = "ns=4;i=59";
    QString allemstop = "ns=4;i=58";
    QString allstop = "ns=4;i=64";
    QString allhome = "ns=4;i=62";
    //传送带节点
    QString conveyorstart ="ns=4;i=60";
    QString conveyorstop ="ns=4;i=61";

    int shuchu = 0;
    int shengchan = 0;
    int wancheng = 0;
    bool homebuttonstate = false;
    struct NodeStatus {
        QString nodeid;
        QString value;
    };//存值结构体（没啥用）
    QString temper = NULL;
    QString nodeid=NULL;
    QString strValue = NULL;
    void paintEvent(QPaintEvent *e);//重绘图函数
    QStringList nodeIds;//存储需要更新到qcombobox的节点
    QOpcUaClient *client = nullptr;//客户端的宏用于返回状态消息
    QString n;
    QOpcUaProvider provider;//后端的宏
    QOpcUaNode *node = nullptr;//存储当前的节点对象
    QString selectedText;//存储节点文本
    int w_nodevalue=0;//存储写入节点的值
    QTimer *timer = new QTimer(this);//qtimer对象
    int nodeindex = 3;//父节点起始点
    QSet<QString> uniqueValues;//使用Qset存储数据的唯一值用于处理实时节点更新
    void addUniqueValue(const QString &nodeId, const QString &newValue);//实时更新状态到选择框和textedit
    bool buttonstate=false;//存储测试按钮状态
    void updateTextEditWithNewValue(QTextEdit* textEdit, const QString& nodeId, const QString& newValue);
    void updateOrAddValue(QTextEdit* textEdit, const QString& nodeId, const QString& newValue);//实时更新节点状态到textedit
    void extractNodeIds();//实时更新状态到选择框
    void someFunction();//延迟函数，没用上
    bool b=false;
    bool b1=false;
    bool allstartb = false;
    bool allstopb = false;
    bool constart = false;
    void changestate(QString nodeid,QString value);//通用修改bool值
    void writeNodeValueWithDelay(const QString &nodeId, const QVariant &value, int delayMillis);//实现PLC的延迟开关
    void sendrobstate(QString message);//发送实时状态给终端
public slots:
    //MQTT
   // void uidateLogStateChange();
    void showme();
    void brokerDisconnected();//连接服务器处理槽函数
    void setClientPort(int p);//设置客户端端口槽函数
    void on_ui_connectbutton_clicked();//连接按钮槽函数
    void on_ui_publishbutton_clicked();//发布按钮函数
    void on_ui_subscribebutton_clicked();//订阅按钮槽函数



    //OPCUA
    void handleWriteNodeAttributesFinished4(const QList<QOpcUaWriteResult> &results, QOpcUa::UaStatusCode serviceResult);
    void on_connect_clicked();//服务器链接按钮
    // void on_readvalue_clicked();//单独读值函数（废弃）
    void on_nodelistbox_activated(int index);//节点选择框状态改变槽函数
    // void handleReadNodeAttributesFinished(const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult);
    void handleWriteNodeAttributesFinished(const QList<QOpcUaWriteResult> &results, QOpcUa::UaStatusCode serviceResult);//修改值处理槽函数
    //void refreshstate(const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult);
    void on_ui_confirmtochange_clicked();//确认更改值按钮
    void readNodeVlue();//实时读取节点状态
    void on_ui_getnodestate_clicked();//实时查询按钮
    void handleReadNodeAttributesFinished1(const QList<QOpcUaReadResult> &results, QOpcUa::UaStatusCode serviceResult);//读值处理槽函数
    void handleWriteNodeAttributesFinished2(const QList<QOpcUaWriteResult> &results, QOpcUa::UaStatusCode serviceResult);//bool测试处理函数
    void on_ui_booltestbutton_clicked();//bool测试按钮
    void on_ui_disconnectbutton_clicked();//断开服务器按钮
    void writeNodeValue(const QString &nodeId, const QVariant &value);
private slots:
    void on_ui_back_clicked();

    bool prepareWriteRequest(const QString& nodeIdentifier, QList<QOpcUaWriteItem>& request);

    void on_ui_robstartbutton_pressed();

    void on_ui_robstartbutton_released();
    bool prepareWriteRequest2(const QString& nodeIdentifier, QList<QOpcUaWriteItem>& request);
    void on_ui_robstopbutton_clicked();
    void on_ui_robhomebutton_pressed();
    void on_ui_robhomebutton_released();
    void on_ui_allemstop_clicked();
    void on_ui_allhome_pressed();
    void on_ui_allhome_released();
    void on_ui_allstop_clicked();
    void on_ui_conveyorstop_clicked();
    void on_ui_robemstopbutton_pressed();
    void on_ui_robemstopbutton_released();
    void on_ui_allstart_clicked();
    void on_ui_conveyorstart_clicked();

  //  void on_ui_robstartbutton_clicked();

private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
