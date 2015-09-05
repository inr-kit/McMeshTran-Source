/********************************************************************************
** Form generated from reading UI file 'AverageMeshes.ui'
**
** Created: Thu Mar 21 09:43:22 2013
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AVERAGEMESHES_H
#define UI_AVERAGEMESHES_H

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

class Ui_MCMESHTRANGUI_AverageMeshesDlg
{
public:
    QGroupBox *groupBox;
    QComboBox *avg_cb_mt1;
    QLabel *label;
    QComboBox *avg_cb_mt2;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *avg_le_nmt;
    QPushButton *avg_pb_gpsl;
    QComboBox *avg_cb_gp;
    QLabel *label_17;

    void setupUi(QDialog *MCMESHTRANGUI_AverageMeshesDlg)
    {
        if (MCMESHTRANGUI_AverageMeshesDlg->objectName().isEmpty())
            MCMESHTRANGUI_AverageMeshesDlg->setObjectName(QString::fromUtf8("MCMESHTRANGUI_AverageMeshesDlg"));
        MCMESHTRANGUI_AverageMeshesDlg->resize(400, 281);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCMESHTRANGUI_AverageMeshesDlg->sizePolicy().hasHeightForWidth());
        MCMESHTRANGUI_AverageMeshesDlg->setSizePolicy(sizePolicy);
        MCMESHTRANGUI_AverageMeshesDlg->setMinimumSize(QSize(400, 281));
        groupBox = new QGroupBox(MCMESHTRANGUI_AverageMeshesDlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 20, 371, 201));
        avg_cb_mt1 = new QComboBox(groupBox);
        avg_cb_mt1->setObjectName(QString::fromUtf8("avg_cb_mt1"));
        avg_cb_mt1->setGeometry(QRect(120, 80, 161, 31));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 80, 91, 31));
        avg_cb_mt2 = new QComboBox(groupBox);
        avg_cb_mt2->setObjectName(QString::fromUtf8("avg_cb_mt2"));
        avg_cb_mt2->setGeometry(QRect(120, 120, 161, 31));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(20, 120, 91, 31));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 40, 91, 31));
        avg_le_nmt = new QLineEdit(groupBox);
        avg_le_nmt->setObjectName(QString::fromUtf8("avg_le_nmt"));
        avg_le_nmt->setGeometry(QRect(120, 40, 161, 27));
        avg_pb_gpsl = new QPushButton(groupBox);
        avg_pb_gpsl->setObjectName(QString::fromUtf8("avg_pb_gpsl"));
        avg_pb_gpsl->setGeometry(QRect(290, 160, 71, 31));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(avg_pb_gpsl->sizePolicy().hasHeightForWidth());
        avg_pb_gpsl->setSizePolicy(sizePolicy1);
        avg_cb_gp = new QComboBox(groupBox);
        avg_cb_gp->setObjectName(QString::fromUtf8("avg_cb_gp"));
        avg_cb_gp->setGeometry(QRect(120, 160, 161, 31));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(avg_cb_gp->sizePolicy().hasHeightForWidth());
        avg_cb_gp->setSizePolicy(sizePolicy2);
        label_17 = new QLabel(groupBox);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(20, 160, 91, 31));
        sizePolicy2.setHeightForWidth(label_17->sizePolicy().hasHeightForWidth());
        label_17->setSizePolicy(sizePolicy2);

        retranslateUi(MCMESHTRANGUI_AverageMeshesDlg);

        QMetaObject::connectSlotsByName(MCMESHTRANGUI_AverageMeshesDlg);
    } // setupUi

    void retranslateUi(QDialog *MCMESHTRANGUI_AverageMeshesDlg)
    {
        MCMESHTRANGUI_AverageMeshesDlg->setWindowTitle(QApplication::translate("MCMESHTRANGUI_AverageMeshesDlg", "Averaging Mesh", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MCMESHTRANGUI_AverageMeshesDlg", "Meshes", 0, QApplication::UnicodeUTF8));
        avg_cb_mt1->clear();
        avg_cb_mt1->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_AverageMeshesDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("MCMESHTRANGUI_AverageMeshesDlg", "Mesh 1", 0, QApplication::UnicodeUTF8));
        avg_cb_mt2->clear();
        avg_cb_mt2->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_AverageMeshesDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label_2->setText(QApplication::translate("MCMESHTRANGUI_AverageMeshesDlg", "Mesh 2", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MCMESHTRANGUI_AverageMeshesDlg", "New Name", 0, QApplication::UnicodeUTF8));
        avg_pb_gpsl->setText(QApplication::translate("MCMESHTRANGUI_AverageMeshesDlg", "Select", 0, QApplication::UnicodeUTF8));
        avg_cb_gp->clear();
        avg_cb_gp->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_AverageMeshesDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label_17->setText(QApplication::translate("MCMESHTRANGUI_AverageMeshesDlg", "Group", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCMESHTRANGUI_AverageMeshesDlg: public Ui_MCMESHTRANGUI_AverageMeshesDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AVERAGEMESHES_H
