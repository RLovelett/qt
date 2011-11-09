#include "widget.h"
#include "ui_widget.h"
#include <QWidget>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    // Setting font size and indent width are used to verify fix for QTBUG-5111
    connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(setFontPointSize(int)));
    connect(ui->spinBox_2, SIGNAL(valueChanged(int)), this, SLOT(setIndentWidth(int)));

    QTextDocument* d = ui->textEdit->document();
    QTextCursor* cursor = new QTextCursor(d);

    // Initialize text list with different formats and layouts
    QTextListFormat listFormat;
    // disc
    listFormat.setStyle(QTextListFormat::ListDisc);
    cursor->insertList(listFormat);
    cursor->insertText("disc one");
    cursor->insertText("\ndisc two");
    // 1., 2.
    listFormat.setStyle(QTextListFormat::ListDecimal);
    cursor->insertList(listFormat);
    cursor->insertText("decimal one");
    cursor->insertText("\ndecimal two");
    // a., b.
    listFormat.setStyle(QTextListFormat::ListLowerAlpha);
    cursor->insertList(listFormat);
    cursor->insertText("lower alpha one");
    cursor->insertText("\nlower alpha two");
    // A., B.
    listFormat.setStyle(QTextListFormat::ListUpperAlpha);
    cursor->insertList(listFormat);
    cursor->insertText("upper alpha one");
    cursor->insertText("\nupper alpha two");
    // Indent 1
    listFormat.setStyle(QTextListFormat::ListDisc);
    listFormat.setIndent(1);
    cursor->insertList(listFormat);
    cursor->insertText("indent 1 one");
    cursor->insertText("\nindent 2 two");
    // Indent 2
    listFormat.setIndent(2);
    cursor->insertList(listFormat);
    cursor->insertText("indent 2 one");
    cursor->insertText("\nindent 2 two");
    // Indent 3
    listFormat.setIndent(3);
    cursor->insertList(listFormat);
    cursor->insertText("indent 3 one");
    cursor->insertText("\nindent 3 two");
    // right to left: disc
    listFormat.setIndent(1);
    listFormat.setStyle(QTextListFormat::ListDisc);
    cursor->insertList(listFormat);
    cursor->insertText(QChar( 0x05d0)); // use Hebrew aleph to create a right-to-left layout
    cursor->insertText("\n" + QString(QChar( 0x05d0)));
    // right to left: 1., 2.
    listFormat.setStyle(QTextListFormat::ListLowerAlpha);
    cursor->insertList(listFormat);
    cursor->insertText(QChar( 0x05d0)); // use Hebrew aleph to create a right-to-left layout
    cursor->insertText("\n" + QString(QChar( 0x05d0)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::setFontPointSize(int value)
{
    ui->textEdit->selectAll();
    ui->textEdit->setFontPointSize(value);
}

void Widget::setIndentWidth(int value)
{
    ui->textEdit->document()->setIndentWidth(value);
}
