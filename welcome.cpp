#include "welcome.h"
#include "ui_welcome.h"

welcome::welcome(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::welcome)
{
    ui->setupUi(this);
    ui->ui_password->setEchoMode(QLineEdit::Password);

}

welcome::~welcome()
{
    delete ui;
}


void welcome::paintEvent(QPaintEvent *e)
{
    QStyleOption opt;
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void welcome::on_welcomebutton_clicked()
{
    if (ui->ui_id->text()=="admin"&&ui->ui_password->text()=="123456")
    {
        this->hide();
        emit showad();
    }
    else if (ui->ui_id->text()=="root"&&ui->ui_password->text()=="123456")
    {
        this->hide();
        emit showwid();
    }
}

void welcome::showme()
{
    this->show();
}

