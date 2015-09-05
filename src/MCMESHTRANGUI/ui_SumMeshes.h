/********************************************************************************
** Form generated from reading UI file 'SumMeshes.ui'
**
** Created: Thu Mar 21 09:43:22 2013
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SUMMESHES_H
#define UI_SUMMESHES_H

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

class Ui_MCMESHTRANGUI_SumMeshesDlg
{
public:
    QGroupBox *groupBox;
    QComboBox *sum_cb_mt1;
    QLabel *label;
    QComboBox *sum_cb_mt2;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *sum_le_nmt;
    QLabel *label_17;
    QPushButton *sum_pb_gpsl;
    QComboBox *sum_cb_gp;

    void setupUi(QDialog *MCMESHTRANGUI_SumMeshesDlg)
    {
        if (MCMESHTRANGUI_SumMeshesDlg->objectName().isEmpty())
            MCMESHTRANGUI_SumMeshesDlg->setObjectName(QString::fromUtf8("MCMESHTRANGUI_SumMeshesDlg"));
        MCMESHTRANGUI_SumMeshesDlg->resize(400, 286);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCMESHTRANGUI_SumMeshesDlg->sizePolicy().hasHeightForWidth());
        MCMESHTRANGUI_SumMeshesDlg->setSizePolicy(sizePolicy);
        MCMESHTRANGUI_SumMeshesDlg->setMinimumSize(QSize(400, 286));
        groupBox = new QGroupBox(MCMESHTRANGUI_SumMeshesDlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 20, 371, 211));
        sum_cb_mt1 = new QComboBox(groupBox);
        sum_cb_mt1->setObjectName(QString::fromUtf8("sum_cb_mt1"));
        sum_cb_mt1->setGeometry(QRect(120, 80, 161, 31));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 80, 91, 31));
        sum_cb_mt2 = new QComboBox(groupBox);
        sum_cb_mt2->setObjectName(QString::fromUtf8("sum_cb_mt2"));
        sum_cb_mt2->setGeometry(QRect(120, 120, 161, 31));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 120, 91, 31));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 40, 91, 31));
        sum_le_nmt = new QLineEdit(groupBox);
        sum_le_nmt->setObjectName(QString::fromUtf8("sum_le_nmt"));
        sum_le_nmt->setGeometry(QRect(120, 40, 161, 27));
        label_17 = new QLabel(groupBox);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(20, 160, 91, 31));
        sum_pb_gpsl = new QPushButton(groupBox);
        sum_pb_gpsl->setObjectName(QString::fromUtf8("sum_pb_gpsl"));
        sum_pb_gpsl->setGeometry(QRect(290, 160, 71, 31));
        sum_cb_gp = new QComboBox(groupBox);
        sum_cb_gp->setObjectName(QString::fromUtf8("sum_cb_gp"));
        sum_cb_gp->setGeometry(QRect(120, 160, 161, 31));

        retranslateUi(MCMESHTRANGUI_SumMeshesDlg);

        QMetaObject::connectSlotsByName(MCMESHTRANGUI_SumMeshesDlg);
    } // setupUi

    void retranslateUi(QDialog *MCMESHTRANGUI_SumMeshesDlg)
    {
        MCMESHTRANGUI_SumMeshesDlg->setWindowTitle(QApplication::translate("MCMESHTRANGUI_SumMeshesDlg", "Sum Mesh tallies", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MCMESHTRANGUI_SumMeshesDlg", "Meshes", 0, QApplication::UnicodeUTF8));
        sum_cb_mt1->clear();
        sum_cb_mt1->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_SumMeshesDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("MCMESHTRANGUI_SumMeshesDlg", "Mesh 1", 0, QApplication::UnicodeUTF8));
        sum_cb_mt2->clear();
        sum_cb_mt2->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_SumMeshesDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("MCMESHTRANGUI_SumMeshesDlg", "Mesh 2", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MCMESHTRANGUI_SumMeshesDlg", "New Name", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MCMESHTRANGUI_SumMeshesDlg", "Group", 0, QApplication::UnicodeUTF8));
        sum_pb_gpsl->setText(QApplication::translate("MCMESHTRANGUI_SumMeshesDlg", "Select", 0, QApplication::UnicodeUTF8));
        sum_cb_gp->clear();
        sum_cb_gp->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_SumMeshesDlg", "------", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class MCMESHTRANGUI_SumMeshesDlg: public Ui_MCMESHTRANGUI_SumMeshesDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SUMMESHES_H
