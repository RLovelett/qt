#include "textedit.h"

TextEdit::TextEdit(QWidget *parent) :
    QTextEdit(parent)
{
}

void TextEdit::keyPressEvent(QKeyEvent *event)
{
    keyPressed(event);
    QTextEdit::keyPressEvent(event);
}
