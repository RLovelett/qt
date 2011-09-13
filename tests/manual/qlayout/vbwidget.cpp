#include "vbwidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>

VbWidget::VbWidget(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *hb = new QVBoxLayout(this);
    QComboBox *combo = new QComboBox(this);
    combo->addItem("123");
    QComboBox *combo2 = new QComboBox();
    combo2->setEditable(true);
    combo2->addItem("123");

    hb->addWidget(new QLabel("123"));
    hb->addWidget(new QLineEdit("123"));
    hb->addWidget(combo);
    hb->addWidget(combo2);
    hb->addWidget(new QCheckBox("123"));
    hb->addWidget(new QDateTimeEdit());
    hb->addWidget(new QPushButton("123"));
    hb->addWidget(new QSpinBox());
}
