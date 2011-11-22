#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class QLabel;

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = 0);

signals:

public slots:
    void keyPressEvent(QKeyEvent *event);

private:
    QLabel *label1;
    QLabel *label2;
};

#endif // WIDGET_H
