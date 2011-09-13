#include "gridwidget.h"
#include <QGridLayout>
#include <QPushButton>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>
#include <QCheckBox>

GridWidget::GridWidget(QWidget *parent) :
    QWidget(parent)
{
    QGridLayout *hb = new QGridLayout(this);
    QComboBox *combo = new QComboBox();
    combo->addItem("123");
    QComboBox *combo2 = new QComboBox();
    combo2->setEditable(true);
    combo2->addItem("123");

    hb->addWidget(new QLabel("123"), 0, 0);
    hb->addWidget(new QLabel("123"), 0, 1);
    hb->addWidget(new QLineEdit("123"), 1, 0);
    hb->addWidget(new QLineEdit("123"), 1, 1);
    hb->addWidget(new QCheckBox("123"), 0, 2);
    hb->addWidget(new QCheckBox("123"), 1, 2);
    hb->addWidget(combo, 0, 3);
    hb->addWidget(combo2, 1, 3);
    hb->addWidget(new QDateTimeEdit(), 0, 4);
    hb->addWidget(new QPushButton("123"), 1, 4);
    hb->addWidget(new QSpinBox(), 0, 5);
    hb->addWidget(new QSpinBox(), 1, 5);
}
