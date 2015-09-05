/********************************************************************************
** Form generated from reading UI file 'ScaleMesh.ui'
**
** Created: Thu Mar 21 09:43:22 2013
**      by: Qt User Interface Compiler version 4.6.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCALEMESH_H
#define UI_SCALEMESH_H

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

class Ui_MCMESHTRANGUI_ScaleMeshDlg
{
public:
    QGroupBox *groupBox;
    QComboBox *scl_cb_mt;
    QLabel *label;
    QLabel *label_3;
    QLineEdit *scl_le_nmt;
    QLineEdit *scl_le_fct;
    QLabel *label_4;
    QLabel *label_17;
    QPushButton *scl_pb_gpsl;
    QComboBox *scl_cb_gp;

    void setupUi(QDialog *MCMESHTRANGUI_ScaleMeshDlg)
    {
        if (MCMESHTRANGUI_ScaleMeshDlg->objectName().isEmpty())
            MCMESHTRANGUI_ScaleMeshDlg->setObjectName(QString::fromUtf8("MCMESHTRANGUI_ScaleMeshDlg"));
        MCMESHTRANGUI_ScaleMeshDlg->resize(400, 279);
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(MCMESHTRANGUI_ScaleMeshDlg->sizePolicy().hasHeightForWidth());
        MCMESHTRANGUI_ScaleMeshDlg->setSizePolicy(sizePolicy);
        MCMESHTRANGUI_ScaleMeshDlg->setMinimumSize(QSize(400, 279));
        groupBox = new QGroupBox(MCMESHTRANGUI_ScaleMeshDlg);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(10, 20, 381, 201));
        scl_cb_mt = new QComboBox(groupBox);
        scl_cb_mt->setObjectName(QString::fromUtf8("scl_cb_mt"));
        scl_cb_mt->setGeometry(QRect(120, 80, 161, 31));
        label = new QLabel(groupBox);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(20, 80, 91, 31));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 40, 91, 31));
        scl_le_nmt = new QLineEdit(groupBox);
        scl_le_nmt->setObjectName(QString::fromUtf8("scl_le_nmt"));
        scl_le_nmt->setGeometry(QRect(120, 40, 161, 27));
        scl_le_fct = new QLineEdit(groupBox);
        scl_le_fct->setObjectName(QString::fromUtf8("scl_le_fct"));
        scl_le_fct->setGeometry(QRect(120, 120, 161, 27));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(20, 120, 91, 31));
        label_17 = new QLabel(groupBox);
        label_17->setObjectName(QString::fromUtf8("label_17"));
        label_17->setGeometry(QRect(20, 160, 91, 31));
        scl_pb_gpsl = new QPushButton(groupBox);
        scl_pb_gpsl->setObjectName(QString::fromUtf8("scl_pb_gpsl"));
        scl_pb_gpsl->setGeometry(QRect(290, 160, 71, 31));
        scl_cb_gp = new QComboBox(groupBox);
        scl_cb_gp->setObjectName(QString::fromUtf8("scl_cb_gp"));
        scl_cb_gp->setGeometry(QRect(120, 160, 161, 31));

        retranslateUi(MCMESHTRANGUI_ScaleMeshDlg);

        QMetaObject::connectSlotsByName(MCMESHTRANGUI_ScaleMeshDlg);
    } // setupUi

    void retranslateUi(QDialog *MCMESHTRANGUI_ScaleMeshDlg)
    {
        MCMESHTRANGUI_ScaleMeshDlg->setWindowTitle(QApplication::translate("MCMESHTRANGUI_ScaleMeshDlg", "Scale Mesh tallies", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("MCMESHTRANGUI_ScaleMeshDlg", "Mesh and factor", 0, QApplication::UnicodeUTF8));
        scl_cb_mt->clear();
        scl_cb_mt->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_ScaleMeshDlg", "------", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("MCMESHTRANGUI_ScaleMeshDlg", "Mesh", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("MCMESHTRANGUI_ScaleMeshDlg", "New Name", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("MCMESHTRANGUI_ScaleMeshDlg", "Factor", 0, QApplication::UnicodeUTF8));
        label_17->setText(QApplication::translate("MCMESHTRANGUI_ScaleMeshDlg", "Group", 0, QApplication::UnicodeUTF8));
        scl_pb_gpsl->setText(QApplication::translate("MCMESHTRANGUI_ScaleMeshDlg", "Select", 0, QApplication::UnicodeUTF8));
        scl_cb_gp->clear();
        scl_cb_gp->insertItems(0, QStringList()
         << QApplication::translate("MCMESHTRANGUI_ScaleMeshDlg", "------", 0, QApplication::UnicodeUTF8)
        );
    } // retranslateUi

};

namespace Ui {
    class MCMESHTRANGUI_ScaleMeshDlg: public Ui_MCMESHTRANGUI_ScaleMeshDlg {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCALEMESH_H
