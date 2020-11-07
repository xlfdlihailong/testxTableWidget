/********************************************************************************
** Form generated from reading UI file 'xtreewidget.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_XTREEWIDGET_H
#define UI_XTREEWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "qtreewidgetexx.h"

QT_BEGIN_NAMESPACE

class Ui_xTreeWidget
{
public:
    QVBoxLayout *verticalLayout;
    QTreeWidgetExx *treeWidget;

    void setupUi(QWidget *xTreeWidget)
    {
        if (xTreeWidget->objectName().isEmpty())
            xTreeWidget->setObjectName(QStringLiteral("xTreeWidget"));
        xTreeWidget->resize(751, 640);
        verticalLayout = new QVBoxLayout(xTreeWidget);
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        treeWidget = new QTreeWidgetExx(xTreeWidget);
        treeWidget->setObjectName(QStringLiteral("treeWidget"));
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(11);
        font.setBold(true);
        font.setWeight(75);
        treeWidget->setFont(font);

        verticalLayout->addWidget(treeWidget);


        retranslateUi(xTreeWidget);

        QMetaObject::connectSlotsByName(xTreeWidget);
    } // setupUi

    void retranslateUi(QWidget *xTreeWidget)
    {
        xTreeWidget->setWindowTitle(QApplication::translate("xTreeWidget", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class xTreeWidget: public Ui_xTreeWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_XTREEWIDGET_H
