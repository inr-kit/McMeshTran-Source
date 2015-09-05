/********************************************************************************
** Form generated from reading UI file 'RotateMesh.ui'
**
** Created: Tue May 5 17:14:54 2015
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_ROTATEMESH_H
#define UI_ROTATEMESH_H

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

class Ui_MCMESHTRANGUI_RotateMeshDlg
{
public:
    QGroupBox *rt_gb_rt;
    QLineEdit *rt_le_cz;
    QLineEdit *rt_le_cy;
    QLineEdit *rt_le_cx;
    QLineEdit *rt_le_vz;
    QLineEdit *rt_le_vy;
    QLineEdit *rt_le_vx;
    QLineEdit *rt_le_ag;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLabel *label_8;
    QLabel *label_9;
    QLabel *label_10;
    QLabel *label_11;
    QLabel *label_12;
    QGroupBox *groupBox_3;
    QComboBox *rt_cb_mt;
    QLabel *label_16;
    QLabel *label_15;
    QLineEdit *rt_le_nmt;
    QLabel *label_17;
    QPushButton *rt_pb_gpsl;
    QComboBox *rt_cb_gp;

    void setupUi(QDialog *MCMESHTRANGUI_RotateMeshDlg)
    {
        if (MCMESHTRANGUI_RotateMeshDlg->objectName().isEmpty())
            MCMESHTRANGUI_RotateMeshDlg->setObjectName(QString::fromUtf8("MCMESHTRANGUI_RotateMeshDlg"));
        MCMESHTRANGUI_RotateMeshDlg->resize(500, 434);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCMESHTRANGUI_RotateMeshDlg->sizePolicy().hasHeightForWidth());
        MCMESHTRANGUI_RotateMeshDlg->setSizePolicy(sizePolicy);
        MCMESHTRANGUI_RotateMeshDlg->setMinimumSize(QSize(500, 434));
        rt_gb_rt = new QGroupBox(MCMESHTRANGUI_RotateMeshDlg);
        rt_gb_rt->setObjectName(QString::fromUtf8("rt_gb_rt"));
        rt_gb_rt->setGeometry(QRect(20, 180, 461, 191));
        rt_gb_rt->setCheckable(false);
        rt_gb_rt->setChecked(false);
        rt_le_cz = new QLineEdit(rt_gb_rt);
        rt_le_cz->setObjectName(QString::fromUtf8("rt_le_cz"));
        rt_le_cz->setGeometry(QRect(360, 30, 91, 27));
        rt_le_cy = new QLineEdit(rt_gb_rt);
        rt_le_cy->setObjectName(QString::fromUtf8("rt_le_cy"));
        rt_le_cy->setGeometry(QRect(230, 30, 91, 27));
        rt_le_cx = new QLineEdit(rt_gb_rt);
        rt_le_cx->setObjectName(QString::fromUtf8("rt_le_cx"));
        rt_le_cx->setGeometry(QRect(100, 30, 91, 27));
        rt_le_vz = new QLineEdit(rt_gb_rt);
        rt_le_vz->setObjectName(QString::fromUtf8("rt_le_vz"));
        rt_le_vz->setGeometry(QRect(360, 90, 91, 27));
        rt_le_vy = new QLineEdit(rt_gb_rt);
        rt_le_vy->setObjectName(QString::fromUtf8("rt_le_vy"));
        rt_le_vy->setGeometry(QRect(230, 90, 91, 27));
        rt_le_vx = new QLineEdit(rt_gb_rt);
        rt_le_vx->setObjectName(QString::fromUtf8("rt_le_vx"));
        rt_le_vx->setGeometry(QRect(100, 90, 91, 27));
        rt_le_ag = new QLineEdit(rt_gb_rt);
        rt_le_ag->setObjectName(QString::fromUtf8("rt_le_ag"));
        rt_le_ag->setGeometry(QRect(230, 150, 91, 27));
        label_4 = new QLabel(rt_gb_rt);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(340, 30, 20, 27));
        label_5 = new QLabel(rt_gb_rt);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(80, 30, 20, 27));
        label_6 = new QLabel(rt_gb_rt);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(210, 30, 20, 27));
        label_7 = new QLabel(rt_gb_rt);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(210, 90, 20, 27));
        label_8 = new QLabel(rt_gb_rt);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(80, 90, 20, 27));
        label_9 = new QLabel(rt_gb_rt);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(340, 90, 20, 27));
        label_10 = new QLabel(rt_gb_rt);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(20, 30, 62, 21));
        QFont font;
        font.setFamily(QString::fromUtf8("Bitstream Charter"));
        font.setPointSize(12);
        label_10->setFont(font);
        label_11 = new QLabel(rt_gb_rt);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(20, 90, 62, 21));
        label_11->setFont(font);
        label_12 = new QLabel(rt_gb_rt);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(20, 150, 111, 21));
        label_12->setFont(font);
        groupBox_3 = new QGroupBox(MCMESHTRANGUI_RotateMeshDlg);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(20, 20, 461, 151));
        rt_cb_mt = new QComboBox(groupBox_3);
        rt_cb_mt->setObjectName(QString::fromUtf8("rt_cb_mt"));
        rt_cb_mt->setGeometry(QRect(120, 70, 161, 31));
        label_16 = new QLabel(groupBox_3);
        label_16->setObjectName(QString::fromUtf8("label_16"));
        label_16->setGeometry(QRect(20, 70, 91, 31));
        label_15 = new QLabel(groupBox_3);
        label_15->setObjectName(QString::fromUtf8("label_15"));
        label_15->setGeometry(QRect(20, 30, 91, 31));
        rt_le_nmt = new QLineEdit(groupBox_3);
        rt_le_nmt->setObjectName(QString::fromUtf8("rt_le_nmt"));
        rt_le_nmt->setGeometry(QRect(120, 30, 161, 27));
        label_17 = new QLabel(groupBox_3);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(20, 110, 91, 31));
        rt_pb_gpsl = new QPushButton(groupBox_3);
        rt_pb_gpsl->setObjectName(QString::fromUtf8("rt_pb_gpsl"));
        rt_pb_gpsl->setGeometry(QRect(290, 110, 71, 31));
        rt_cb_gp = new QComboBox(groupBox_3);
        rt_cb_gp->setObjectName(QString::fromUtf8("rt_cb_gp"));
        rt_cb_gp->setGeometry(QRect(120, 110, 161, 31));

        retranslateUi(MCMESHTRANGUI_RotateMeshDlg);

        QMetaObject::connectSlotsByName(MCMESHTRANGUI_RotateMeshDlg);
    } // setupUi

    void retranslateUi(QDialog *MCMESHTRANGUI_RotateMeshDlg)
    {
        MCMESHTRANGUI_RotateMeshDlg->setWindowTitle(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Move and Rotate ", 0, QApplication::UnicodeUTF8));
        rt_gb_rt->setTitle(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Rotate ", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Z", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "X", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Y", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Y", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "X", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Z", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Center", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Vector", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Angle (radian)", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Mesh ", 0, QApplication::UnicodeUTF8));
        rt_cb_mt->clear();
        rt_cb_mt->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label_16->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Mesh ", 0, QApplication::UnicodeUTF8));
        label_15->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "New Name", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Group", 0, QApplication::UnicodeUTF8));
        rt_pb_gpsl->setText(QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "Select", 0, QApplication::UnicodeUTF8));
        rt_cb_gp->clear();
        rt_cb_gp->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_RotateMeshDlg", "------", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class MCMESHTRANGUI_RotateMeshDlg: public Ui_MCMESHTRANGUI_RotateMeshDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_ROTATEMESH_H
