/********************************************************************************
** Form generated from reading UI file 'xfilebrowserlocal.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XFILEBROWSERLOCAL_H
#define UI_XFILEBROWSERLOCAL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "xtablewidget.h"
#include "xtreewidget.h"

QT_BEGIN_NAMESPACE

class Ui_xFileBrowserLocal
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLineEdit *plineEditpwd;
    QLineEdit *plineEditFind;
    QHBoxLayout *horizontalLayout_2;
    xTreeWidget *ptree;
    xTableWidget *ptab;

    void setupUi(QWidget *xFileBrowserLocal)
    {
        if (xFileBrowserLocal->objectName().isEmpty())
            xFileBrowserLocal->setObjectName(QStringLiteral("xFileBrowserLocal"));
        xFileBrowserLocal->resize(823, 624);
        verticalLayout = new QVBoxLayout(xFileBrowserLocal);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        plineEditpwd = new QLineEdit(xFileBrowserLocal);
        plineEditpwd->setObjectName(QStringLiteral("plineEditpwd"));

        horizontalLayout->addWidget(plineEditpwd);

        plineEditFind = new QLineEdit(xFileBrowserLocal);
        plineEditFind->setObjectName(QStringLiteral("plineEditFind"));
        plineEditFind->setMaximumSize(QSize(200, 16777215));

        horizontalLayout->addWidget(plineEditFind);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        ptree = new xTreeWidget(xFileBrowserLocal);
        ptree->setObjectName(QStringLiteral("ptree"));
        ptree->setMaximumSize(QSize(200, 16777215));

        horizontalLayout_2->addWidget(ptree);

        ptab = new xTableWidget(xFileBrowserLocal);
        ptab->setObjectName(QStringLiteral("ptab"));

        horizontalLayout_2->addWidget(ptab);


        verticalLayout->addLayout(horizontalLayout_2);


        retranslateUi(xFileBrowserLocal);

        QMetaObject::connectSlotsByName(xFileBrowserLocal);
    } // setupUi

    void retranslateUi(QWidget *xFileBrowserLocal)
    {
        xFileBrowserLocal->setWindowTitle(QApplication::translate("xFileBrowserLocal", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class xFileBrowserLocal: public Ui_xFileBrowserLocal {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XFILEBROWSERLOCAL_H
