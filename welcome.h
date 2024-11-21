#ifndef WELCOME_H
#define WELCOME_H

#include <QWidget>
#include <QPainter>
#include <QStyle>
#include <QStyleOption>
namespace Ui {
class welcome;
}

class welcome : public QWidget
{
    Q_OBJECT

signals:
    void showwid();
    void showad();


public:
    explicit welcome(QWidget *parent = nullptr);
    ~welcome();
    void paintEvent(QPaintEvent *e);
private slots:
    void on_welcomebutton_clicked();
    void showme();

private:
    Ui::welcome *ui;
};

#endif // WELCOME_H
