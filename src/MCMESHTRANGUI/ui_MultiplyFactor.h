/********************************************************************************
** Form generated from reading UI file 'MultiplyFactor.ui'
**
** Created: Thu Mar 21 13:21:25 2013
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MULTIPLYFACTOR_H
#define UI_MULTIPLYFACTOR_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_MCMESHTRANGUI_MultiplyFactorDlg
{
public:
    QGroupBox *groupBox;
    QComboBox *mti_cb_mt;
    QLabel *label;
    QLabel *label_3;
    QLineEdit *mti_le_nmt;
    QLineEdit *mti_le_fct;
    QLabel *label_4;
    QLabel *label_17;
    QComboBox *mti_cb_gp;
    QPushButton *mti_pb_gpsl;

    void setupUi(QDialog *MCMESHTRANGUI_MultiplyFactorDlg)
    {
        if (MCMESHTRANGUI_MultiplyFactorDlg->objectName().isEmpty())
            MCMESHTRANGUI_MultiplyFactorDlg->setObjectName(QString::fromUtf8("MCMESHTRANGUI_MultiplyFactorDlg"));
        MCMESHTRANGUI_MultiplyFactorDlg->resize(400, 276);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCMESHTRANGUI_MultiplyFactorDlg->sizePolicy().hasHeightForWidth());
        MCMESHTRANGUI_MultiplyFactorDlg->setSizePolicy(sizePolicy);
        MCMESHTRANGUI_MultiplyFactorDlg->setMinimumSize(QSize(400, 276));
        groupBox = new QGroupBox(MCMESHTRANGUI_MultiplyFactorDlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 20, 381, 201));
        mti_cb_mt = new QComboBox(groupBox);
        mti_cb_mt->setObjectName(QString::fromUtf8("mti_cb_mt"));
        mti_cb_mt->setGeometry(QRect(120, 80, 161, 31));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 80, 91, 31));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 40, 91, 31));
        mti_le_nmt = new QLineEdit(groupBox);
        mti_le_nmt->setObjectName(QString::fromUtf8("mti_le_nmt"));
        mti_le_nmt->setGeometry(QRect(120, 40, 161, 27));
        mti_le_fct = new QLineEdit(groupBox);
        mti_le_fct->setObjectName(QString::fromUtf8("mti_le_fct"));
        mti_le_fct->setGeometry(QRect(120, 120, 161, 27));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 120, 101, 31));
        label_17 = new QLabel(groupBox);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(20, 160, 91, 31));
        mti_cb_gp = new QComboBox(groupBox);
        mti_cb_gp->setObjectName(QString::fromUtf8("mti_cb_gp"));
        mti_cb_gp->setGeometry(QRect(120, 160, 161, 31));
        mti_pb_gpsl = new QPushButton(groupBox);
        mti_pb_gpsl->setObjectName(QString::fromUtf8("mti_pb_gpsl"));
        mti_pb_gpsl->setGeometry(QRect(290, 160, 71, 31));

        retranslateUi(MCMESHTRANGUI_MultiplyFactorDlg);

        QMetaObject::connectSlotsByName(MCMESHTRANGUI_MultiplyFactorDlg);
    } // setupUi

    void retranslateUi(QDialog *MCMESHTRANGUI_MultiplyFactorDlg)
    {
        MCMESHTRANGUI_MultiplyFactorDlg->setWindowTitle(QApplication::translate("MCMESHTRANGUI_MultiplyFactorDlg", "Multiply with Constant Factor", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MCMESHTRANGUI_MultiplyFactorDlg", "Mesh and factor", 0, QApplication::UnicodeUTF8));
        mti_cb_mt->clear();
        mti_cb_mt->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_MultiplyFactorDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("MCMESHTRANGUI_MultiplyFactorDlg", "Mesh ", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MCMESHTRANGUI_MultiplyFactorDlg", "New Name", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MCMESHTRANGUI_MultiplyFactorDlg", "Factor", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MCMESHTRANGUI_MultiplyFactorDlg", "Group", 0, QApplication::UnicodeUTF8));
        mti_cb_gp->clear();
        mti_cb_gp->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_MultiplyFactorDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        mti_pb_gpsl->setText(QApplication::translate("MCMESHTRANGUI_MultiplyFactorDlg", "Select", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCMESHTRANGUI_MultiplyFactorDlg: public Ui_MCMESHTRANGUI_MultiplyFactorDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MULTIPLYFACTOR_H
