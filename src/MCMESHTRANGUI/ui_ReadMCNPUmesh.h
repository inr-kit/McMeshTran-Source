/********************************************************************************
** Form generated from reading UI file 'ReadMCNPUmesh.ui'
**
** Created: Sun Nov 10 16:54:11 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_READMCNPUMESH_H
#define UI_READMCNPUMESH_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>

QT_BEGIN_NAMESPACE

class Ui_MCMESHTRANGUI_ReadMCNPUmeshDlg
{
public:
    QGroupBox *groupBox;
    QLabel *label_6;
    QLineEdit *rmu_le_umo;
    QPushButton *rmu_pb_umobrw;
    QGroupBox *rmu_gb_rmml;
    QRadioButton *rmu_rb_rmml_yes;
    QRadioButton *rmu_rb_rmml_no;
    QGroupBox *rmu_gb_kpInst;
    QRadioButton *rmu_rb_kpInst_yes;
    QRadioButton *rmu_rb_kpInst_no;
    QLabel *label;

    void setupUi(QDialog *MCMESHTRANGUI_ReadMCNPUmeshDlg)
    {
        if (MCMESHTRANGUI_ReadMCNPUmeshDlg->objectName().isEmpty())
            MCMESHTRANGUI_ReadMCNPUmeshDlg->setObjectName(QString::fromUtf8("MCMESHTRANGUI_ReadMCNPUmeshDlg"));
        MCMESHTRANGUI_ReadMCNPUmeshDlg->resize(568, 260);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCMESHTRANGUI_ReadMCNPUmeshDlg->sizePolicy().hasHeightForWidth());
        MCMESHTRANGUI_ReadMCNPUmeshDlg->setSizePolicy(sizePolicy);
        MCMESHTRANGUI_ReadMCNPUmeshDlg->setMinimumSize(QSize(568, 260));
        MCMESHTRANGUI_ReadMCNPUmeshDlg->setBaseSize(QSize(0, 0));
        groupBox = new QGroupBox(MCMESHTRANGUI_ReadMCNPUmeshDlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 20, 531, 81));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 40, 91, 27));
        rmu_le_umo = new QLineEdit(groupBox);
        rmu_le_umo->setObjectName(QString::fromUtf8("rmu_le_umo"));
        rmu_le_umo->setGeometry(QRect(120, 40, 281, 27));
        rmu_le_umo->setMaximumSize(QSize(300, 16777215));
        rmu_le_umo->setReadOnly(false);
        rmu_pb_umobrw = new QPushButton(groupBox);
        rmu_pb_umobrw->setObjectName(QString::fromUtf8("rmu_pb_umobrw"));
        rmu_pb_umobrw->setGeometry(QRect(420, 40, 96, 27));
        rmu_gb_rmml = new QGroupBox(MCMESHTRANGUI_ReadMCNPUmeshDlg);
        rmu_gb_rmml->setObjectName(QString::fromUtf8("rmu_gb_rmml"));
        rmu_gb_rmml->setGeometry(QRect(20, 160, 551, 51));
        rmu_rb_rmml_yes = new QRadioButton(rmu_gb_rmml);
        rmu_rb_rmml_yes->setObjectName(QString::fromUtf8("rmu_rb_rmml_yes"));
        rmu_rb_rmml_yes->setGeometry(QRect(40, 30, 112, 22));
        rmu_rb_rmml_yes->setChecked(false);
        rmu_rb_rmml_no = new QRadioButton(rmu_gb_rmml);
        rmu_rb_rmml_no->setObjectName(QString::fromUtf8("rmu_rb_rmml_no"));
        rmu_rb_rmml_no->setGeometry(QRect(190, 30, 112, 22));
        rmu_rb_rmml_no->setCheckable(true);
        rmu_rb_rmml_no->setChecked(true);
        rmu_gb_kpInst = new QGroupBox(MCMESHTRANGUI_ReadMCNPUmeshDlg);
        rmu_gb_kpInst->setObjectName(QString::fromUtf8("rmu_gb_kpInst"));
        rmu_gb_kpInst->setGeometry(QRect(20, 110, 551, 51));
        rmu_rb_kpInst_yes = new QRadioButton(rmu_gb_kpInst);
        rmu_rb_kpInst_yes->setObjectName(QString::fromUtf8("rmu_rb_kpInst_yes"));
        rmu_rb_kpInst_yes->setGeometry(QRect(40, 30, 112, 22));
        rmu_rb_kpInst_yes->setChecked(true);
        rmu_rb_kpInst_no = new QRadioButton(rmu_gb_kpInst);
        rmu_rb_kpInst_no->setObjectName(QString::fromUtf8("rmu_rb_kpInst_no"));
        rmu_rb_kpInst_no->setGeometry(QRect(190, 30, 112, 22));
        label = new QLabel(MCMESHTRANGUI_ReadMCNPUmeshDlg);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(200, 220, 351, 21));

        retranslateUi(MCMESHTRANGUI_ReadMCNPUmeshDlg);

        QMetaObject::connectSlotsByName(MCMESHTRANGUI_ReadMCNPUmeshDlg);
    } // setupUi

    void retranslateUi(QDialog *MCMESHTRANGUI_ReadMCNPUmeshDlg)
    {
        MCMESHTRANGUI_ReadMCNPUmeshDlg->setWindowTitle(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "Load File", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "MCNP6 Umesh eeout file", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "Input File", 0, QApplication::UnicodeUTF8));
        rmu_pb_umobrw->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "Browse", 0, QApplication::UnicodeUTF8));
        rmu_gb_rmml->setTitle(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "Remove Multiplier", 0, QApplication::UnicodeUTF8));
        rmu_rb_rmml_yes->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "Yes", 0, QApplication::UnicodeUTF8));
        rmu_rb_rmml_no->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "No", 0, QApplication::UnicodeUTF8));
        rmu_gb_kpInst->setTitle(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "Keep Instances", 0, QApplication::UnicodeUTF8));
        rmu_rb_kpInst_yes->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "Yes", 0, QApplication::UnicodeUTF8));
        rmu_rb_kpInst_no->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "No", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPUmeshDlg", "* The multiplier in total data set will not be removed", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCMESHTRANGUI_ReadMCNPUmeshDlg: public Ui_MCMESHTRANGUI_ReadMCNPUmeshDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_READMCNPUMESH_H
