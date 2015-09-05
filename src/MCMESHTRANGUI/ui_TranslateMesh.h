/********************************************************************************
** Form generated from reading UI file 'TranslateMesh.ui'
**
** Created: Thu Mar 21 09:43:22 2013
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRANSLATEMESH_H
#define UI_TRANSLATEMESH_H

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

class Ui_MCMESHTRANGUI_TranslateMeshDlg
{
public:
    QGroupBox *tr_gb_mv;
    QLineEdit *tr_le_dx;
    QLineEdit *tr_le_dy;
    QLineEdit *tr_le_dz;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QGroupBox *groupBox_3;
    QComboBox *tr_cb_mt;
    QLabel *label_16;
    QLabel *label_15;
    QLineEdit *tr_le_nmt;
    QComboBox *tr_cb_gp;
    QLabel *label_17;
    QPushButton *tr_pb_gpsl;

    void setupUi(QDialog *MCMESHTRANGUI_TranslateMeshDlg)
    {
        if (MCMESHTRANGUI_TranslateMeshDlg->objectName().isEmpty())
            MCMESHTRANGUI_TranslateMeshDlg->setObjectName(QString::fromUtf8("MCMESHTRANGUI_TranslateMeshDlg"));
        MCMESHTRANGUI_TranslateMeshDlg->resize(500, 308);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCMESHTRANGUI_TranslateMeshDlg->sizePolicy().hasHeightForWidth());
        MCMESHTRANGUI_TranslateMeshDlg->setSizePolicy(sizePolicy);
        MCMESHTRANGUI_TranslateMeshDlg->setMinimumSize(QSize(500, 308));
        tr_gb_mv = new QGroupBox(MCMESHTRANGUI_TranslateMeshDlg);
        tr_gb_mv->setObjectName(QString::fromUtf8("tr_gb_mv"));
        tr_gb_mv->setGeometry(QRect(20, 180, 461, 71));
        tr_gb_mv->setCheckable(false);
        tr_gb_mv->setChecked(false);
        tr_le_dx = new QLineEdit(tr_gb_mv);
        tr_le_dx->setObjectName(QString::fromUtf8("tr_le_dx"));
        tr_le_dx->setGeometry(QRect(60, 30, 91, 27));
        tr_le_dy = new QLineEdit(tr_gb_mv);
        tr_le_dy->setObjectName(QString::fromUtf8("tr_le_dy"));
        tr_le_dy->setGeometry(QRect(200, 30, 91, 27));
        tr_le_dz = new QLineEdit(tr_gb_mv);
        tr_le_dz->setObjectName(QString::fromUtf8("tr_le_dz"));
        tr_le_dz->setGeometry(QRect(350, 30, 91, 27));
        label = new QLabel(tr_gb_mv);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 30, 31, 27));
        label_2 = new QLabel(tr_gb_mv);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(170, 30, 31, 27));
        label_3 = new QLabel(tr_gb_mv);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(320, 30, 31, 27));
        groupBox_3 = new QGroupBox(MCMESHTRANGUI_TranslateMeshDlg);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 20, 461, 151));
        tr_cb_mt = new QComboBox(groupBox_3);
        tr_cb_mt->setObjectName(QString::fromUtf8("tr_cb_mt"));
        tr_cb_mt->setGeometry(QRect(120, 70, 161, 31));
        label_16 = new QLabel(groupBox_3);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(20, 70, 91, 31));
        label_15 = new QLabel(groupBox_3);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(20, 30, 91, 31));
        tr_le_nmt = new QLineEdit(groupBox_3);
        tr_le_nmt->setObjectName(QString::fromUtf8("tr_le_nmt"));
        tr_le_nmt->setGeometry(QRect(120, 30, 161, 27));
        tr_cb_gp = new QComboBox(groupBox_3);
        tr_cb_gp->setObjectName(QString::fromUtf8("tr_cb_gp"));
        tr_cb_gp->setGeometry(QRect(120, 110, 161, 31));
        label_17 = new QLabel(groupBox_3);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(20, 110, 91, 31));
        tr_pb_gpsl = new QPushButton(groupBox_3);
        tr_pb_gpsl->setObjectName(QString::fromUtf8("tr_pb_gpsl"));
        tr_pb_gpsl->setGeometry(QRect(300, 110, 93, 31));

        retranslateUi(MCMESHTRANGUI_TranslateMeshDlg);

        QMetaObject::connectSlotsByName(MCMESHTRANGUI_TranslateMeshDlg);
    } // setupUi

    void retranslateUi(QDialog *MCMESHTRANGUI_TranslateMeshDlg)
    {
        MCMESHTRANGUI_TranslateMeshDlg->setWindowTitle(QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "Move and Rotate ", 0, QApplication::UnicodeUTF8));
        tr_gb_mv->setTitle(QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "Move Vector", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "Dx", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "Dy", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "Dz", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "Mesh ", 0, QApplication::UnicodeUTF8));
        tr_cb_mt->clear();
        tr_cb_mt->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label_16->setText(QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "Mesh ", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "New Name", 0, QApplication::UnicodeUTF8));
        tr_cb_gp->clear();
        tr_cb_gp->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label_17->setText(QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "Group", 0, QApplication::UnicodeUTF8));
        tr_pb_gpsl->setText(QApplication::translate("MCMESHTRANGUI_TranslateMeshDlg", "Select", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCMESHTRANGUI_TranslateMeshDlg: public Ui_MCMESHTRANGUI_TranslateMeshDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRANSLATEMESH_H
