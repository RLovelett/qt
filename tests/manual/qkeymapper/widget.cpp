#include "widget.h"
#include "textedit.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>
#include <QKeyEvent>

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    TextEdit *e = new TextEdit();
    connect(e, SIGNAL(keyPressed(QKeyEvent *)), this, SLOT(keyPressEvent(QKeyEvent*)));
    label1 = new QLabel("keyPressEvent key: ");
    label2 = new QLabel("keyPressEvent text: ");

    QVBoxLayout *l = new QVBoxLayout(this);
    l->addWidget(e);
    l->addWidget(label1);
    l->addWidget(label2);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    label1->setText("keyPressEvent key: " + QString::number(event->key()));
    label2->setText("keyPressEvent text: " + event->text());
}
