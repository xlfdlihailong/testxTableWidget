/********************************************************************************
** Form generated from reading UI file 'xTableWidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XTABLEWIDGET_H
#define UI_XTABLEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QHBoxLayout *horizontalLayout;
    QTableWidget *tableWidget;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(1345, 781);
        QFont font;
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        Form->setFont(font);
        horizontalLayout = new QHBoxLayout(Form);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        tableWidget = new QTableWidget(Form);
        tableWidget->setObjectName(QStringLiteral("tableWidget"));
        tableWidget->setMinimumSize(QSize(55, 55));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        tableWidget->setFont(font1);
        tableWidget->setWordWrap(false);
        tableWidget->setColumnCount(0);

        horizontalLayout->addWidget(tableWidget);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XTABLEWIDGET_H
