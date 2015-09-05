/********************************************************************************
** Form generated from reading UI file 'ReadMCNPFmesh.ui'
**
** Created: Thu Mar 21 13:34:44 2013
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_READMCNPFMESH_H
#define UI_READMCNPFMESH_H

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

QT_BEGIN_NAMESPACE

class Ui_MCMESHTRANGUI_ReadMCNPFmeshDlg
{
public:
    QGroupBox *groupBox;
    QLabel *label_6;
    QLabel *label_7;
    QLineEdit *rmf_le_inp;
    QLineEdit *rmf_le_meshtal;
    QPushButton *rmf_pb_inpbrw;
    QPushButton *rmf_pb_meshtalbrw;

    void setupUi(QDialog *MCMESHTRANGUI_ReadMCNPFmeshDlg)
    {
        if (MCMESHTRANGUI_ReadMCNPFmeshDlg->objectName().isEmpty())
            MCMESHTRANGUI_ReadMCNPFmeshDlg->setObjectName(QString::fromUtf8("MCMESHTRANGUI_ReadMCNPFmeshDlg"));
        MCMESHTRANGUI_ReadMCNPFmeshDlg->resize(568, 195);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCMESHTRANGUI_ReadMCNPFmeshDlg->sizePolicy().hasHeightForWidth());
        MCMESHTRANGUI_ReadMCNPFmeshDlg->setSizePolicy(sizePolicy);
        MCMESHTRANGUI_ReadMCNPFmeshDlg->setMinimumSize(QSize(568, 195));
        MCMESHTRANGUI_ReadMCNPFmeshDlg->setBaseSize(QSize(0, 0));
        groupBox = new QGroupBox(MCMESHTRANGUI_ReadMCNPFmeshDlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(20, 20, 531, 131));
        label_6 = new QLabel(groupBox);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(10, 50, 91, 27));
        label_7 = new QLabel(groupBox);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 90, 91, 27));
        rmf_le_inp = new QLineEdit(groupBox);
        rmf_le_inp->setObjectName(QString::fromUtf8("rmf_le_inp"));
        rmf_le_inp->setGeometry(QRect(120, 50, 281, 27));
        rmf_le_inp->setMaximumSize(QSize(300, 16777215));
        rmf_le_inp->setReadOnly(false);
        rmf_le_meshtal = new QLineEdit(groupBox);
        rmf_le_meshtal->setObjectName(QString::fromUtf8("rmf_le_meshtal"));
        rmf_le_meshtal->setGeometry(QRect(120, 90, 281, 27));
        rmf_le_meshtal->setMaximumSize(QSize(300, 16777215));
        rmf_le_meshtal->setReadOnly(false);
        rmf_pb_inpbrw = new QPushButton(groupBox);
        rmf_pb_inpbrw->setObjectName(QString::fromUtf8("rmf_pb_inpbrw"));
        rmf_pb_inpbrw->setGeometry(QRect(420, 50, 96, 27));
        rmf_pb_meshtalbrw = new QPushButton(groupBox);
        rmf_pb_meshtalbrw->setObjectName(QString::fromUtf8("rmf_pb_meshtalbrw"));
        rmf_pb_meshtalbrw->setGeometry(QRect(420, 90, 96, 27));

        retranslateUi(MCMESHTRANGUI_ReadMCNPFmeshDlg);

        QMetaObject::connectSlotsByName(MCMESHTRANGUI_ReadMCNPFmeshDlg);
    } // setupUi

    void retranslateUi(QDialog *MCMESHTRANGUI_ReadMCNPFmeshDlg)
    {
        MCMESHTRANGUI_ReadMCNPFmeshDlg->setWindowTitle(QApplication::translate("MCMESHTRANGUI_ReadMCNPFmeshDlg", "Load File", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MCMESHTRANGUI_ReadMCNPFmeshDlg", "MCNP Input and Meshtal file", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPFmeshDlg", "Input File", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPFmeshDlg", "Meshtal File", 0, QApplication::UnicodeUTF8));
        rmf_pb_inpbrw->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPFmeshDlg", "Browse", 0, QApplication::UnicodeUTF8));
        rmf_pb_meshtalbrw->setText(QApplication::translate("MCMESHTRANGUI_ReadMCNPFmeshDlg", "Browse", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class MCMESHTRANGUI_ReadMCNPFmeshDlg: public Ui_MCMESHTRANGUI_ReadMCNPFmeshDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_READMCNPFMESH_H
