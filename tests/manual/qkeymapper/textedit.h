#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>

class TextEdit : public QTextEdit
{
    Q_OBJECT
public:
    explicit TextEdit(QWidget *parent = 0);

signals:
    void keyPressed(QKeyEvent* e);

public slots:
    void keyPressEvent(QKeyEvent *event);
};

#endif // TEXTEDIT_H
