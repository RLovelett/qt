/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

/********************************************************************************
** Form generated from reading ui file 'default.ui'
**
** Created: Mon Sep 1 09:31:03 2008
**      by: Qt User Interface Compiler version 4.5.0
**
** WARNING! All changes made in this file will be lost when recompiling ui file!
********************************************************************************/

#ifndef DEFAULT_H
#define DEFAULT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QGridLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QStatusBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *exitAction;
    QAction *aboutQtAction;
    QAction *editStyleAction;
    QAction *aboutAction;
    QWidget *centralwidget;
    QGridLayout *gridLayout;
    QLabel *nameLabel;
    QComboBox *nameCombo;
    QSpacerItem *spacerItem;
    QRadioButton *femaleRadioButton;
    QCheckBox *agreeCheckBox;
    QRadioButton *maleRadioButton;
    QLabel *genderLabel;
    QSpinBox *ageSpinBox;
    QDialogButtonBox *buttonBox;
    QLabel *ageLabel;
    QLabel *passwordLabel;
    QLineEdit *passwordEdit;
    QLabel *label;
    QLabel *countryLabel;
    QListWidget *professionList;
    QComboBox *countryCombo;
    QMenuBar *menubar;
    QMenu *menu_File;
    QMenu *menu_Help;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(388, 413);
        exitAction = new QAction(MainWindow);
        exitAction->setObjectName(QString::fromUtf8("exitAction"));
        aboutQtAction = new QAction(MainWindow);
        aboutQtAction->setObjectName(QString::fromUtf8("aboutQtAction"));
        editStyleAction = new QAction(MainWindow);
        editStyleAction->setObjectName(QString::fromUtf8("editStyleAction"));
        aboutAction = new QAction(MainWindow);
        aboutAction->setObjectName(QString::fromUtf8("aboutAction"));
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        gridLayout = new QGridLayout(centralwidget);
#ifndef Q_OS_MAC
        gridLayout->setSpacing(6);
#endif
#ifndef Q_OS_MAC
        gridLayout->setMargin(9);
#endif
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        nameLabel = new QLabel(centralwidget);
        nameLabel->setObjectName(QString::fromUtf8("nameLabel"));

        gridLayout->addWidget(nameLabel, 0, 0, 1, 1);

        nameCombo = new QComboBox(centralwidget);
        nameCombo->setObjectName(QString::fromUtf8("nameCombo"));
        nameCombo->setEditable(true);

        gridLayout->addWidget(nameCombo, 0, 1, 1, 3);

        spacerItem = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        gridLayout->addItem(spacerItem, 1, 3, 1, 1);

        femaleRadioButton = new QRadioButton(centralwidget);
        femaleRadioButton->setObjectName(QString::fromUtf8("femaleRadioButton"));

        gridLayout->addWidget(femaleRadioButton, 1, 2, 1, 1);

        agreeCheckBox = new QCheckBox(centralwidget);
        agreeCheckBox->setObjectName(QString::fromUtf8("agreeCheckBox"));

        gridLayout->addWidget(agreeCheckBox, 6, 0, 1, 4);

        maleRadioButton = new QRadioButton(centralwidget);
        maleRadioButton->setObjectName(QString::fromUtf8("maleRadioButton"));

        gridLayout->addWidget(maleRadioButton, 1, 1, 1, 1);

        genderLabel = new QLabel(centralwidget);
        genderLabel->setObjectName(QString::fromUtf8("genderLabel"));

        gridLayout->addWidget(genderLabel, 1, 0, 1, 1);

        ageSpinBox = new QSpinBox(centralwidget);
        ageSpinBox->setObjectName(QString::fromUtf8("ageSpinBox"));
        ageSpinBox->setMinimum(12);
        ageSpinBox->setValue(22);

        gridLayout->addWidget(ageSpinBox, 2, 1, 1, 3);

        buttonBox = new QDialogButtonBox(centralwidget);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::NoButton|QDialogButtonBox::Ok);

        gridLayout->addWidget(buttonBox, 7, 2, 1, 2);

        ageLabel = new QLabel(centralwidget);
        ageLabel->setObjectName(QString::fromUtf8("ageLabel"));

        gridLayout->addWidget(ageLabel, 2, 0, 1, 1);

        passwordLabel = new QLabel(centralwidget);
        passwordLabel->setObjectName(QString::fromUtf8("passwordLabel"));

        gridLayout->addWidget(passwordLabel, 3, 0, 1, 1);

        passwordEdit = new QLineEdit(centralwidget);
        passwordEdit->setObjectName(QString::fromUtf8("passwordEdit"));
        passwordEdit->setEchoMode(QLineEdit::Password);

        gridLayout->addWidget(passwordEdit, 3, 1, 1, 3);

        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 5, 0, 1, 1);

        countryLabel = new QLabel(centralwidget);
        countryLabel->setObjectName(QString::fromUtf8("countryLabel"));

        gridLayout->addWidget(countryLabel, 4, 0, 1, 1);

        professionList = new QListWidget(centralwidget);
        new QListWidgetItem(professionList);
        new QListWidgetItem(professionList);
        new QListWidgetItem(professionList);
        professionList->setObjectName(QString::fromUtf8("professionList"));

        gridLayout->addWidget(professionList, 5, 1, 1, 3);

        countryCombo = new QComboBox(centralwidget);
        countryCombo->setObjectName(QString::fromUtf8("countryCombo"));

        gridLayout->addWidget(countryCombo, 4, 1, 1, 3);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 388, 21));
        menu_File = new QMenu(menubar);
        menu_File->setObjectName(QString::fromUtf8("menu_File"));
        menu_Help = new QMenu(menubar);
        menu_Help->setObjectName(QString::fromUtf8("menu_Help"));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);
#ifndef QT_NO_SHORTCUT
        nameLabel->setBuddy(nameCombo);
        ageLabel->setBuddy(ageSpinBox);
        passwordLabel->setBuddy(passwordEdit);
        label->setBuddy(professionList);
        countryLabel->setBuddy(professionList);
#endif // QT_NO_SHORTCUT
        QWidget::setTabOrder(maleRadioButton, femaleRadioButton);
        QWidget::setTabOrder(femaleRadioButton, ageSpinBox);
        QWidget::setTabOrder(ageSpinBox, passwordEdit);
        QWidget::setTabOrder(passwordEdit, professionList);
        QWidget::setTabOrder(professionList, agreeCheckBox);

        menubar->addAction(menu_File->menuAction());
        menubar->addAction(menu_Help->menuAction());
        menu_File->addAction(editStyleAction);
        menu_File->addSeparator();
        menu_File->addAction(exitAction);
        menu_Help->addAction(aboutAction);
        menu_Help->addSeparator();
        menu_Help->addAction(aboutQtAction);

        retranslateUi(MainWindow);

        nameCombo->setCurrentIndex(-1);
        professionList->setCurrentRow(1);
        countryCombo->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QApplication::translate("MainWindow", "MainWindow", 0, QApplication::UnicodeUTF8));
        exitAction->setText(QApplication::translate("MainWindow", "&Exit", 0, QApplication::UnicodeUTF8));
        aboutQtAction->setText(QApplication::translate("MainWindow", "About Qt", 0, QApplication::UnicodeUTF8));
        editStyleAction->setText(QApplication::translate("MainWindow", "Edit &Style", 0, QApplication::UnicodeUTF8));
        aboutAction->setText(QApplication::translate("MainWindow", "About", 0, QApplication::UnicodeUTF8));
        nameLabel->setText(QApplication::translate("MainWindow", "&Name:", 0, QApplication::UnicodeUTF8));
        nameCombo->clear();
        nameCombo->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Girish", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Jasmin", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Simon", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Zack", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        nameCombo->setToolTip(QApplication::translate("MainWindow", "Specify your name", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        femaleRadioButton->setToolTip(QApplication::translate("MainWindow", "Check this if you are female", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        femaleRadioButton->setText(QApplication::translate("MainWindow", "&Female", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        agreeCheckBox->setToolTip(QApplication::translate("MainWindow", "Please read the license before checking this", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        agreeCheckBox->setText(QApplication::translate("MainWindow", "I &accept the terms and conditions", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        maleRadioButton->setToolTip(QApplication::translate("MainWindow", "Check this if you are male", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        maleRadioButton->setText(QApplication::translate("MainWindow", "&Male", 0, QApplication::UnicodeUTF8));
        genderLabel->setText(QApplication::translate("MainWindow", "Gender:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ageSpinBox->setToolTip(QApplication::translate("MainWindow", "Specify your age", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        ageSpinBox->setStatusTip(QApplication::translate("MainWindow", "Specify your age here", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        ageLabel->setText(QApplication::translate("MainWindow", "&Age:", 0, QApplication::UnicodeUTF8));
        passwordLabel->setText(QApplication::translate("MainWindow", "&Password:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        passwordEdit->setToolTip(QApplication::translate("MainWindow", "Specify your password", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        passwordEdit->setStatusTip(QApplication::translate("MainWindow", "Specify your password here", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        passwordEdit->setText(QApplication::translate("MainWindow", "Password", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MainWindow", "Profession", 0, QApplication::UnicodeUTF8));
        countryLabel->setText(QApplication::translate("MainWindow", "&Country", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = professionList->isSortingEnabled();
        professionList->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = professionList->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("MainWindow", "Developer", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem1 = professionList->item(1);
        ___qlistwidgetitem1->setText(QApplication::translate("MainWindow", "Student", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem2 = professionList->item(2);
        ___qlistwidgetitem2->setText(QApplication::translate("MainWindow", "Fisherman", 0, QApplication::UnicodeUTF8));
        professionList->setSortingEnabled(__sortingEnabled);

#ifndef QT_NO_TOOLTIP
        professionList->setToolTip(QApplication::translate("MainWindow", "Select your profession", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        professionList->setStatusTip(QApplication::translate("MainWindow", "Select your profession", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_WHATSTHIS
        professionList->setWhatsThis(QApplication::translate("MainWindow", "Select your profession", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_WHATSTHIS
        countryCombo->clear();
        countryCombo->insertItems(0, QStringList()
         << QApplication::translate("MainWindow", "Germany", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "India", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "Norway", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "United States Of America", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("MainWindow", "United Kingdom", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        countryCombo->setToolTip(QApplication::translate("MainWindow", "Specify your country", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        countryCombo->setStatusTip(QApplication::translate("MainWindow", "Specify your country here", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        menu_File->setTitle(QApplication::translate("MainWindow", "&File", 0, QApplication::UnicodeUTF8));
        menu_Help->setTitle(QApplication::translate("MainWindow", "&Help", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // DEFAULT_H
