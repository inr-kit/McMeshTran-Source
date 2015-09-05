// Copyright (C) 2014-2015  KIT-INR/NK
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//
#ifndef MCMESHTRANGUI_DIALOGS_H
#define MCMESHTRANGUI_DIALOGS_H
#include <LightApp_Dialog.h>
#include "MCMESHTRANGUI_DataModel.h"
#include "ui_ReadMCNPFmesh.h"
#include "ui_AverageMeshes.h"
#include "ui_InterpolateMesh.h"
#include "ui_MultiplyFactor.h"
#include "ui_ReadMCNPFmesh.h"
#include "ui_ScaleMesh.h"
#include "ui_SumMeshes.h"
#include "ui_TranslateMesh.h"
#include "ui_RotateMesh.h"
#include "ui_ReadMCNPUmesh.h"

#include <QDialog>
#include <QWidget>
#include <QFileDialog>
#include <QPushButton>
#include <QComboBox>
#include <QStringList>
const double Null = 4.44444444444e34;

//###########TemplateDlg###############//

class  MCMESHTRANGUI_TemplateDlg : public LightApp_Dialog
{
    Q_OBJECT
public:
     MCMESHTRANGUI_TemplateDlg(QWidget *parent);
     virtual            ~MCMESHTRANGUI_TemplateDlg();

protected:
     QString            getLastDir();
public:
     void               updateNameList();
     QString            getEntry1() ;
     QString            getEntry2() ;
     QString            getNewName() { return aLineEdit ?  aLineEdit->text() : "";}
     int                getGroupID();
protected slots:
     void               OnSelectGroup();

protected:
    QString             newName;
    QString             aEntry;  //for mesh 1
    QString             bEntry;  //for mesh 2
//    QString             gEntry;  // for group
    QString             LastDir;
    //the following members should be initialized by child class
    MCMESHTRANGUI_DataModel * dm ;
    QComboBox *         aCombobox;  //for initializing and updating
    QComboBox *         bCombobox;
    QLineEdit *         aLineEdit;
    QComboBox *         gCombobox;  //group combobox


};

//###########ReadMCNPFmeshDlg###############//
class MCMESHTRANGUI_ReadMCNPFmeshDlg: public MCMESHTRANGUI_TemplateDlg, Ui::MCMESHTRANGUI_ReadMCNPFmeshDlg
{
    Q_OBJECT

public:
    MCMESHTRANGUI_ReadMCNPFmeshDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm );
    virtual             ~MCMESHTRANGUI_ReadMCNPFmeshDlg();
    QString             getInputFileName() { return rmf_le_inp->text(); }
    QString             getMeshtalFileName() { return rmf_le_meshtal->text(); }

private slots:
    void                onInputBrowse();
    void                onMeshtalBrowse();
};

//###########SumMeshesDlg###############//
class MCMESHTRANGUI_SumMeshesDlg : public  MCMESHTRANGUI_TemplateDlg, Ui::MCMESHTRANGUI_SumMeshesDlg
{
    Q_OBJECT
public:
    MCMESHTRANGUI_SumMeshesDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm );
    virtual             ~MCMESHTRANGUI_SumMeshesDlg();
};

//###########AverageMeshesDlg###############//
class MCMESHTRANGUI_AverageMeshesDlg : public  MCMESHTRANGUI_TemplateDlg, Ui::MCMESHTRANGUI_AverageMeshesDlg
{
    Q_OBJECT
public:
    MCMESHTRANGUI_AverageMeshesDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm );
    virtual             ~MCMESHTRANGUI_AverageMeshesDlg();
};

//###########ScaleMeshDlg###############//
class MCMESHTRANGUI_ScaleMeshDlg : public  MCMESHTRANGUI_TemplateDlg, Ui::MCMESHTRANGUI_ScaleMeshDlg
{
    Q_OBJECT
public:
    MCMESHTRANGUI_ScaleMeshDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm );
    virtual             ~MCMESHTRANGUI_ScaleMeshDlg();
    double              getFactor()  ;
};

//###########MultiplyFactorDlg###############//
class MCMESHTRANGUI_MultiplyFactorDlg : public  MCMESHTRANGUI_TemplateDlg, Ui::MCMESHTRANGUI_MultiplyFactorDlg
{
    Q_OBJECT
public:
    MCMESHTRANGUI_MultiplyFactorDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm );
    virtual             ~MCMESHTRANGUI_MultiplyFactorDlg();
    double              getFactor()  ;
};

//###########TranslateMeshDlg###############//
class MCMESHTRANGUI_TranslateMeshDlg : public  MCMESHTRANGUI_TemplateDlg, Ui::MCMESHTRANGUI_TranslateMeshDlg
{
    Q_OBJECT
public:
    MCMESHTRANGUI_TranslateMeshDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm );
    virtual             ~MCMESHTRANGUI_TranslateMeshDlg();
    double *            getVector ();
};


//###########RotateMeshDlg###############//
class MCMESHTRANGUI_RotateMeshDlg : public  MCMESHTRANGUI_TemplateDlg, Ui::MCMESHTRANGUI_RotateMeshDlg
{
    Q_OBJECT
public:
    MCMESHTRANGUI_RotateMeshDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm );
    virtual             ~MCMESHTRANGUI_RotateMeshDlg();
    double *            getVector ();
    double *            getCenter ();
    double              getAngle  ();
};

//###########InterpolateMeshDlg###############//
class MCMESHTRANGUI_InterpolateMeshDlg : public  MCMESHTRANGUI_TemplateDlg, Ui::MCMESHTRANGUI_InterpolateMeshDlg
{
    Q_OBJECT
public:
    MCMESHTRANGUI_InterpolateMeshDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm );
    virtual             ~MCMESHTRANGUI_InterpolateMeshDlg();
    int                 getIntpOption();

//    QString             getTargetMeshFileName() { return int_le_target->text(); }
private slots:
//    void                onBrowse();
    void                  onMedCheck();

};

//###########ReadMCNPUmeshDlg###############//
class MCMESHTRANGUI_ReadMCNPUmeshDlg: public MCMESHTRANGUI_TemplateDlg, Ui::MCMESHTRANGUI_ReadMCNPUmeshDlg
{
    Q_OBJECT

public:
    MCMESHTRANGUI_ReadMCNPUmeshDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm );
    virtual             ~MCMESHTRANGUI_ReadMCNPUmeshDlg();
    QString             getUMeshOutFileName() { return rmu_le_umo->text(); }
    bool                getIsKeepInstance();
    bool                getIsRemoveMultiplier();

private slots:
    void                onUMeshOutBrowse();
};


class QSlider;

//=================================================================================
// class    : MCMEHSTRANGUI_TransparencyDlg
// purpose  : dialog box that allows to modify transparency of displayed objects
//=================================================================================
class MCMESHTRANGUI_TransparencyDlg : public QDialog
{
    Q_OBJECT

public:
    MCMESHTRANGUI_TransparencyDlg( QWidget* parent, const QStringList& entries );

private:
    QSlider*  mySlider;
    QStringList myEntries;

private slots:
  void ClickOnHelp();
  void ValueHasChanged( int ) ;

};


#endif   //MCMESHTRANGUI_DIALOGS_H
