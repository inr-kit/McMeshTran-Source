/********************************************************************************
** Form generated from reading UI file 'InterpolateMesh.ui'
**
** Created: Mon Jun 10 10:34:24 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INTERPOLATEMESH_H
#define UI_INTERPOLATEMESH_H

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
#include <QtGui/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_MCMESHTRANGUI_InterpolateMeshDlg
{
public:
    QGroupBox *groupBox_2;
    QComboBox *int_cb_mt;
    QLineEdit *int_le_nmt;
    QLabel *label_3;
    QLabel *label_4;
    QComboBox *int_cb_gp;
    QLabel *label_17;
    QLabel *label_2;
    QComboBox *int_cb_mt2;
    QPushButton *sum_pb_gpsl;
    QGroupBox *groupBox_3;
    QRadioButton *int_rb_vex;
    QRadioButton *int_rb_cc;
    QGroupBox *groupBox_4;
    QRadioButton *int_rb_cgns;
    QRadioButton *int_rb_med;

    void setupUi(QDialog *MCMESHTRANGUI_InterpolateMeshDlg)
    {
        if (MCMESHTRANGUI_InterpolateMeshDlg->objectName().isEmpty())
            MCMESHTRANGUI_InterpolateMeshDlg->setObjectName(QString::fromUtf8("MCMESHTRANGUI_InterpolateMeshDlg"));
        MCMESHTRANGUI_InterpolateMeshDlg->resize(400, 422);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCMESHTRANGUI_InterpolateMeshDlg->sizePolicy().hasHeightForWidth());
        MCMESHTRANGUI_InterpolateMeshDlg->setSizePolicy(sizePolicy);
        MCMESHTRANGUI_InterpolateMeshDlg->setMinimumSize(QSize(400, 422));
        groupBox_2 = new QGroupBox(MCMESHTRANGUI_InterpolateMeshDlg);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(20, 20, 361, 201));
        int_cb_mt = new QComboBox(groupBox_2);
        int_cb_mt->setObjectName(QString::fromUtf8("int_cb_mt"));
        int_cb_mt->setGeometry(QRect(110, 70, 161, 31));
        int_le_nmt = new QLineEdit(groupBox_2);
        int_le_nmt->setObjectName(QString::fromUtf8("int_le_nmt"));
        int_le_nmt->setGeometry(QRect(110, 30, 161, 27));
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 70, 91, 31));
        label_4 = new QLabel(groupBox_2);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 30, 91, 31));
        int_cb_gp = new QComboBox(groupBox_2);
        int_cb_gp->setObjectName(QString::fromUtf8("int_cb_gp"));
        int_cb_gp->setGeometry(QRect(110, 160, 161, 31));
        label_17 = new QLabel(groupBox_2);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(10, 160, 91, 31));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 110, 91, 31));
        int_cb_mt2 = new QComboBox(groupBox_2);
        int_cb_mt2->setObjectName(QString::fromUtf8("int_cb_mt2"));
        int_cb_mt2->setGeometry(QRect(110, 110, 161, 31));
        sum_pb_gpsl = new QPushButton(groupBox_2);
        sum_pb_gpsl->setObjectName(QString::fromUtf8("sum_pb_gpsl"));
        sum_pb_gpsl->setGeometry(QRect(280, 160, 71, 31));
        groupBox_3 = new QGroupBox(MCMESHTRANGUI_InterpolateMeshDlg);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(30, 310, 361, 61));
        int_rb_vex = new QRadioButton(groupBox_3);
        int_rb_vex->setObjectName(QString::fromUtf8("int_rb_vex"));
        int_rb_vex->setGeometry(QRect(160, 30, 121, 22));
        int_rb_vex->setChecked(false);
        int_rb_cc = new QRadioButton(groupBox_3);
        int_rb_cc->setObjectName(QString::fromUtf8("int_rb_cc"));
        int_rb_cc->setGeometry(QRect(20, 30, 151, 22));
        int_rb_cc->setChecked(true);
        groupBox_4 = new QGroupBox(MCMESHTRANGUI_InterpolateMeshDlg);
        groupBox_4->setObjectName(QString::fromUtf8("groupBox_4"));
        groupBox_4->setGeometry(QRect(30, 240, 361, 61));
        int_rb_cgns = new QRadioButton(groupBox_4);
        int_rb_cgns->setObjectName(QString::fromUtf8("int_rb_cgns"));
        int_rb_cgns->setGeometry(QRect(20, 30, 121, 22));
        int_rb_cgns->setChecked(true);
        int_rb_med = new QRadioButton(groupBox_4);
        int_rb_med->setObjectName(QString::fromUtf8("int_rb_med"));
        int_rb_med->setGeometry(QRect(160, 30, 151, 22));
        int_rb_med->setCheckable(true);
        int_rb_med->setChecked(false);

        retranslateUi(MCMESHTRANGUI_InterpolateMeshDlg);

        QMetaObject::connectSlotsByName(MCMESHTRANGUI_InterpolateMeshDlg);
    } // setupUi

    void retranslateUi(QDialog *MCMESHTRANGUI_InterpolateMeshDlg)
    {
        MCMESHTRANGUI_InterpolateMeshDlg->setWindowTitle(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "Interpolate Mesh", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "Mesh", 0, QApplication::UnicodeUTF8));
        int_cb_mt->clear();
        int_cb_mt->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label_3->setText(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "Source Mesh", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "New Name", 0, QApplication::UnicodeUTF8));
        int_cb_gp->clear();
        int_cb_gp->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label_17->setText(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "Group", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "Target Mesh", 0, QApplication::UnicodeUTF8));
        int_cb_mt2->clear();
        int_cb_mt2->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        sum_pb_gpsl->setText(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "Select", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "Solution location", 0, QApplication::UnicodeUTF8));
        int_rb_vex->setText(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "Vertex", 0, QApplication::UnicodeUTF8));
        int_rb_cc->setText(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "Cell-center", 0, QApplication::UnicodeUTF8));
        groupBox_4->setTitle(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "Interpolate tools", 0, QApplication::UnicodeUTF8));
        int_rb_cgns->setText(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "CGNS Utility", 0, QApplication::UnicodeUTF8));
        int_rb_med->setText(QApplication::translate("MCMESHTRANGUI_InterpolateMeshDlg", "MED Utility", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCMESHTRANGUI_InterpolateMeshDlg: public Ui_MCMESHTRANGUI_InterpolateMeshDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INTERPOLATEMESH_H
