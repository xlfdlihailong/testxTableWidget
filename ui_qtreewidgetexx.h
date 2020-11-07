/********************************************************************************
** Form generated from reading UI file 'qtreewidgetexx.ui'
**
** Created by: Qt User Interface Compiler version 5.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_QTREEWIDGETEXX_H
#define UI_QTREEWIDGETEXX_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_QTreeWidgetExx
{
public:

    void setupUi(QWidget *QTreeWidgetExx)
    {
        if (QTreeWidgetExx->objectName().isEmpty())
            QTreeWidgetExx->setObjectName(QStringLiteral("QTreeWidgetExx"));
        QTreeWidgetExx->resize(400, 300);
        QFont font;
        font.setFamily(QString::fromUtf8("\346\245\267\344\275\223"));
        font.setPointSize(14);
        font.setBold(true);
        font.setWeight(75);
        QTreeWidgetExx->setFont(font);

        retranslateUi(QTreeWidgetExx);

        QMetaObject::connectSlotsByName(QTreeWidgetExx);
    } // setupUi

    void retranslateUi(QWidget *QTreeWidgetExx)
    {
        QTreeWidgetExx->setWindowTitle(QApplication::translate("QTreeWidgetExx", "Form", 0));
    } // retranslateUi

};

namespace Ui {
    class QTreeWidgetExx: public Ui_QTreeWidgetExx {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_QTREEWIDGETEXX_H
