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
#include "MCMESHTRANGUI_Dialogs.h"
#include "MCMESHTRANGUI.h"
#include "MCMESHTRANGUI_DataModel.h"
#include "MCMESHTRANGUI_Displayer.h"
#include <LightApp_Study.h>
#include <SUIT_Tools.h>
#include <SUIT_MessageBox.h>
#include <SUIT_DataObjectIterator.h>
#include <QMap>

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QLayout>
#include <QVariant>
#include <QToolTip>
#include <QWhatsThis>
#include <QApplication>
#include <QGroupBox>


//###########TemplateDlg###############//

MCMESHTRANGUI_TemplateDlg::MCMESHTRANGUI_TemplateDlg(QWidget* parent)
: LightApp_Dialog( parent, 0, false, true, OK | Apply | Close ),
  LastDir("/home/"),
  aEntry (""),
  bEntry(""),
//  gEntry(""),
  newName(""),
  dm(0),
  aCombobox(0),
  bCombobox(0),
  gCombobox(0),
  aLineEdit(0)
{
}

MCMESHTRANGUI_TemplateDlg::~MCMESHTRANGUI_TemplateDlg()
{
}

/*! return the last visit dir*/
QString     MCMESHTRANGUI_TemplateDlg::getLastDir()
{
    // if not dir name, chop the filename to get dir name
    LastDir = SUIT_Tools::dir(LastDir, /*absolute path*/ true);
    if (LastDir.trimmed().isEmpty())
        LastDir = "/home/";
    return LastDir;
}

void MCMESHTRANGUI_TemplateDlg::updateNameList()
{
    //get the seleted entry
    aEntry = "";
    bEntry = "";
    MCMESHTRANGUI * aModule = dm->getMCMESHTRANGUImodule();
    if (aModule)
    {
        //get the selected object
        QStringList /*allobject */aList,
                /*mesh object */bList;
//                /*group object */cList;
        aModule->selected(aList, true);
        //see if a group object
        for (int i=0; i<aList.size(); i++)
        {
            if (dm->findObject(aList.at(i))->isMesh())
                bList.append(aList.at(i));
//            else if (dm->findObject(aList.at(i))->isMeshGroup())
//                cList.append(aList.at(i));
        }
        if (bList.size() != 0)
        {
            aEntry = bList.at(0);
            if (bList.size() > 1)
                bEntry = bList.at(1);
        }
//        if (cList.size() != 0)
//        {
//            gEntry = cList.at(0);
//        }
    }
    // update the combo box with the map
    if(aCombobox)
    {
        aCombobox->clear();
        aCombobox->addItem("------");  //first item
        for ( SUIT_DataObjectIterator it( dm->root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
        {
            MCMESHTRANGUI_DataObject* obj = dynamic_cast<MCMESHTRANGUI_DataObject*>( it.current() );
            if (obj && obj->isMesh())
                aCombobox->addItem(obj->name(), obj->entry());
        }
        // select the selected object in combo box
        aCombobox->setCurrentIndex(0);
        //if entry not empty and can find the object
        if (!aEntry.trimmed().isEmpty() && dm->findObject(aEntry))
        {
            int aIndex = aCombobox->findData(aEntry);
            if (aIndex > 0)
                aCombobox->setCurrentIndex(aIndex);
        }
    }
    if(bCombobox)
    {
        bCombobox->clear();
        bCombobox->addItem("------");  //first item
        for ( SUIT_DataObjectIterator it( dm->root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
        {
            MCMESHTRANGUI_DataObject* obj = dynamic_cast<MCMESHTRANGUI_DataObject*>( it.current() );
            if (obj && obj->isMesh())
                bCombobox->addItem(obj->name(), obj->entry());
        }
        // select the selected object in combo box
        bCombobox->setCurrentIndex(0);
        //if entry not empty and can find the object
        if (!bEntry.trimmed().isEmpty() && dm->findObject(bEntry))
        {
            int aIndex = bCombobox->findData(bEntry);
            if (aIndex > 0)
                bCombobox->setCurrentIndex(aIndex);
        }
    }
    if (gCombobox)
    {
        gCombobox->clear();
        gCombobox->addItem("------");  //first item
        for ( SUIT_DataObjectIterator it( dm->root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
        {
            MCMESHTRANGUI_DataObject* obj = dynamic_cast<MCMESHTRANGUI_DataObject*>( it.current() );
            if (obj && obj->isMeshGroup() )
                gCombobox->addItem(obj->name(), obj->getGroupID());
        }
        // select the selected object in combo box
        gCombobox->setCurrentIndex(0);
        //if entry not empty and can find the object
        if (!aEntry.trimmed().isEmpty() && dm->findObject(aEntry))
        {
            int aIndex = gCombobox->findData(dm->getGroupID(aEntry));
            if (aIndex > 0)
                gCombobox->setCurrentIndex(aIndex);
        }
    }
}

QString MCMESHTRANGUI_TemplateDlg::getEntry1()
{
    if (!aCombobox)
        return "";
    int aIndex = aCombobox->currentIndex();
    if (aIndex == 0)
        return "";
    return aCombobox->itemData(aIndex).toString();
}

QString MCMESHTRANGUI_TemplateDlg::getEntry2()
{
    if (!bCombobox)
        return "";
    int aIndex = bCombobox->currentIndex();
    if (aIndex == 0)
        return "";
    return bCombobox->itemData(aIndex).toString();
}

int  MCMESHTRANGUI_TemplateDlg::getGroupID()
{
    if (!gCombobox)
        return -1;
    int aIndex = gCombobox->currentIndex();
    if (aIndex == 0)
        return -1;
    bool isOk;
    int tmpInt = gCombobox->itemData(aIndex).toInt(&isOk);
    return isOk ? tmpInt : -1;
}

void MCMESHTRANGUI_TemplateDlg::OnSelectGroup()
{
    MCMESHTRANGUI * aModule = dm->getMCMESHTRANGUImodule();
    if (aModule)
    {
        if (gCombobox)
        {
            //get the selected object
            QStringList aList;
            aModule->selected(aList, true);
            if (aList.size() == 0)
                return;
            QString Entry = aList.at(0);

            if (!Entry.trimmed().isEmpty() && dm->findObject(Entry))
            {
                gCombobox->setCurrentIndex(0);
                int aIndex = gCombobox->findData(dm->getGroupID(Entry));
                if (aIndex > 0)
                    gCombobox->setCurrentIndex(aIndex);
            }
        }
    }
}

//###########ReadMCNPFmeshDlg###############//

MCMESHTRANGUI_ReadMCNPFmeshDlg::MCMESHTRANGUI_ReadMCNPFmeshDlg(QWidget* parent, MCMESHTRANGUI_DataModel * dm )
    :MCMESHTRANGUI_TemplateDlg(parent)
{
    setupUi(this);
    this->dm  = dm;
    connect(rmf_pb_inpbrw,SIGNAL(clicked()),this,SLOT(onInputBrowse()));
    connect(rmf_pb_meshtalbrw,SIGNAL(clicked()),this,SLOT(onMeshtalBrowse()));
}

MCMESHTRANGUI_ReadMCNPFmeshDlg::~MCMESHTRANGUI_ReadMCNPFmeshDlg()
{
}

void MCMESHTRANGUI_ReadMCNPFmeshDlg::onInputBrowse()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open MCNP input File"), getLastDir());
    LastDir = fileName;
    rmf_le_inp->setText(fileName);
}

void MCMESHTRANGUI_ReadMCNPFmeshDlg::onMeshtalBrowse()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open MCNP meshtal File"),getLastDir());
    LastDir = fileName;
    rmf_le_meshtal->setText(fileName);
}


//###########SumMeshesDlg###############//

MCMESHTRANGUI_SumMeshesDlg::MCMESHTRANGUI_SumMeshesDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm )
    :MCMESHTRANGUI_TemplateDlg(parent)
{
    setupUi(this);
    this->dm = dm;
    aCombobox = sum_cb_mt1;
    bCombobox = sum_cb_mt2;
    aLineEdit = sum_le_nmt;
    gCombobox = sum_cb_gp;
//    updateNameList();
    connect(this->dm->getMCMESHTRANGUImodule(),SIGNAL(selectChg()),this,SLOT(OnSelectGroup()) ,Qt::UniqueConnection);

}
MCMESHTRANGUI_SumMeshesDlg::~MCMESHTRANGUI_SumMeshesDlg()
{
}

//###########AverageMeshesDlg###############//
MCMESHTRANGUI_AverageMeshesDlg::MCMESHTRANGUI_AverageMeshesDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm )
    :MCMESHTRANGUI_TemplateDlg(parent)
{
    setupUi(this);
    this->dm = dm;
    aCombobox = avg_cb_mt1;
    bCombobox = avg_cb_mt2;
    aLineEdit = avg_le_nmt;
    gCombobox = avg_cb_gp;
    updateNameList();
    connect(this->dm->getMCMESHTRANGUImodule(),SIGNAL(selectChg()),this,SLOT(OnSelectGroup()) ,Qt::UniqueConnection);
}
MCMESHTRANGUI_AverageMeshesDlg::~MCMESHTRANGUI_AverageMeshesDlg()
{
}

//###########ScaleMeshDlg###############//
MCMESHTRANGUI_ScaleMeshDlg::MCMESHTRANGUI_ScaleMeshDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm )
    :MCMESHTRANGUI_TemplateDlg(parent)
{
    setupUi(this);
    this->dm = dm;
    aCombobox = scl_cb_mt;
    aLineEdit = scl_le_nmt;
    gCombobox = scl_cb_gp;
    updateNameList();
    connect(this->dm->getMCMESHTRANGUImodule(),SIGNAL(selectChg()),this,SLOT(OnSelectGroup()) ,Qt::UniqueConnection);
}
MCMESHTRANGUI_ScaleMeshDlg::~MCMESHTRANGUI_ScaleMeshDlg()
{
}
/*! return factor, is error return 0. */
double   MCMESHTRANGUI_ScaleMeshDlg::getFactor()
{
    bool isOK;
    double tempDouble = scl_le_fct->text().toDouble(&isOK);
    return isOK ? tempDouble : 0.0 ;
}
//###########MultiplyFactorDlg###############//
MCMESHTRANGUI_MultiplyFactorDlg::MCMESHTRANGUI_MultiplyFactorDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm )
    :MCMESHTRANGUI_TemplateDlg(parent)
{
    setupUi(this);
    this->dm = dm;
    aCombobox = mti_cb_mt;
    aLineEdit = mti_le_nmt;
    gCombobox = mti_cb_gp;
    updateNameList();
    connect(this->dm->getMCMESHTRANGUImodule(),SIGNAL(selectChg()),this,SLOT(OnSelectGroup()) ,Qt::UniqueConnection);
}
MCMESHTRANGUI_MultiplyFactorDlg::~MCMESHTRANGUI_MultiplyFactorDlg()
{
}
/*! return factor, is error return 0. */
double   MCMESHTRANGUI_MultiplyFactorDlg::getFactor()
{
    bool isOK;
    double tempDouble = mti_le_fct->text().toDouble(&isOK);
    return isOK ? tempDouble : 0.0 ;
}

//###########TranslateMeshDlg###############//
MCMESHTRANGUI_TranslateMeshDlg::MCMESHTRANGUI_TranslateMeshDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm )
    :MCMESHTRANGUI_TemplateDlg(parent)
{
    setupUi(this);
    this->dm = dm;
    aCombobox = tr_cb_mt;
    aLineEdit = tr_le_nmt;
    gCombobox = tr_cb_gp;
    updateNameList();
    connect(this->dm->getMCMESHTRANGUImodule(),SIGNAL(selectChg()),this,SLOT(OnSelectGroup()) ,Qt::UniqueConnection);
}
MCMESHTRANGUI_TranslateMeshDlg::~MCMESHTRANGUI_TranslateMeshDlg()
{
}
/*! get 3D vector for translation*/
double * MCMESHTRANGUI_TranslateMeshDlg::getVector ()
{
    double * aVector = new double [3];

    bool aOK,bOK,cOK;
    aVector [0] = tr_le_dx->text().toDouble(&aOK);
    aVector [1] = tr_le_dy->text().toDouble(&bOK);
    aVector [2] = tr_le_dz->text().toDouble(&cOK);
    //if not ok, assign a ugly value
    aVector [0] = aOK ? aVector [0] : Null;
    aVector [1] = bOK ? aVector [1] : Null;
    aVector [2] = cOK ? aVector [2] : Null;
    return aVector;
}

//###########RotateMeshDlg###############//
MCMESHTRANGUI_RotateMeshDlg::MCMESHTRANGUI_RotateMeshDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm )
    :MCMESHTRANGUI_TemplateDlg(parent)
{
    setupUi(this);
    this->dm = dm;
    aCombobox = rt_cb_mt;
    aLineEdit = rt_le_nmt;
    gCombobox = rt_cb_gp;
    updateNameList();
    connect(this->dm->getMCMESHTRANGUImodule(),SIGNAL(selectChg()),this,SLOT(OnSelectGroup()) ,Qt::UniqueConnection);
}
MCMESHTRANGUI_RotateMeshDlg::~MCMESHTRANGUI_RotateMeshDlg()
{
}
/*! get vector for the axis */
double * MCMESHTRANGUI_RotateMeshDlg::getVector ()
{
    double * aVector = new double [3];

    bool aOK,bOK,cOK;
    aVector [0] = rt_le_vx->text().toDouble(&aOK);
    aVector [1] = rt_le_vy->text().toDouble(&bOK);
    aVector [2] = rt_le_vz->text().toDouble(&cOK);
    //if not ok, assign a ugly value
    aVector [0] = aOK ? aVector [0] : Null;
    aVector [1] = bOK ? aVector [1] : Null;
    aVector [2] = cOK ? aVector [2] : Null;
    return aVector;
}

/*! get fix point of the rotate axis*/
double * MCMESHTRANGUI_RotateMeshDlg::getCenter()
{
    double * aCenter = new double [3];

    bool aOK,bOK,cOK;
    aCenter [0] = rt_le_cx->text().toDouble(&aOK);
    aCenter [1] = rt_le_cy->text().toDouble(&bOK);
    aCenter [2] = rt_le_cz->text().toDouble(&cOK);
    //if not ok, assign a ugly value
    aCenter [0] = aOK ? aCenter [0] : Null;
    aCenter [1] = bOK ? aCenter [1] : Null;
    aCenter [2] = cOK ? aCenter [2] : Null;
    return aCenter;
}

double MCMESHTRANGUI_RotateMeshDlg::getAngle()
{
    bool isOK;
    double aAngle = rt_le_ag->text().toDouble(&isOK);
    return isOK ? aAngle : Null;
}

//###########InterpolateMeshDlg###############//
MCMESHTRANGUI_InterpolateMeshDlg::MCMESHTRANGUI_InterpolateMeshDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm )
    :MCMESHTRANGUI_TemplateDlg(parent)
{
    setupUi(this);
//    connect(int_pb_brw,SIGNAL(clicked()),this,SLOT(onBrowse()));
    this->dm = dm;
    aCombobox = int_cb_mt;
    bCombobox = int_cb_mt2;
    aLineEdit = int_le_nmt;
    gCombobox = int_cb_gp;
    updateNameList();
    connect(this->dm->getMCMESHTRANGUImodule(),SIGNAL(selectChg()),this,SLOT(OnSelectGroup()) ,Qt::UniqueConnection);
//    connect(this,SIGNAL(this->int_rb_vex->toggled(true)),this,SLOT(onMedCheck()), Qt::UniqueConnection);
#ifndef WITH_CGNS
 int_rb_cgns->setCheckable(false);
#endif

}
MCMESHTRANGUI_InterpolateMeshDlg::~MCMESHTRANGUI_InterpolateMeshDlg()
{
}

int MCMESHTRANGUI_InterpolateMeshDlg::getIntpOption()
{
    if (int_rb_vex->isChecked() && int_rb_cgns->isChecked())  //if want to locate solution in vertex
        return 4;
    else if (int_rb_vex->isChecked() && int_rb_med->isChecked())  //if want to locate solution in vertex
        return 2;
    if (int_rb_cc->isChecked() && int_rb_cgns->isChecked())  //if want to locate solution in vertex
        return 3;
    else if (int_rb_cc->isChecked() && int_rb_med->isChecked())  //if want to locate solution in vertex
        return 1;
    else return 0;
}

void  MCMESHTRANGUI_InterpolateMeshDlg::onMedCheck()
{
    //comment out because MED interpolation now support ON_NODES interpolation
//    int_rb_vex->setCheckable(false);

}

//###########ReadMCNPUmeshDlg###############//

MCMESHTRANGUI_ReadMCNPUmeshDlg::MCMESHTRANGUI_ReadMCNPUmeshDlg(QWidget* parent, MCMESHTRANGUI_DataModel * dm )
    :MCMESHTRANGUI_TemplateDlg(parent)
{
    setupUi(this);
    this->dm  = dm;
    connect(rmu_pb_umobrw,SIGNAL(clicked()),this,SLOT(onUMeshOutBrowse()));
}

MCMESHTRANGUI_ReadMCNPUmeshDlg::~MCMESHTRANGUI_ReadMCNPUmeshDlg()
{
}

void MCMESHTRANGUI_ReadMCNPUmeshDlg::onUMeshOutBrowse()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open MCNP6 Umesh eeout File"), getLastDir());
    LastDir = fileName;
    rmu_le_umo->setText(fileName);
}

bool MCMESHTRANGUI_ReadMCNPUmeshDlg::getIsKeepInstance()
{
    if (rmu_rb_kpInst_yes->isChecked()) return true;
    else if (rmu_rb_kpInst_no->isChecked()) return false;
    else return true;
}
bool MCMESHTRANGUI_ReadMCNPUmeshDlg::getIsRemoveMultiplier()
{
    if (rmu_rb_rmml_yes->isChecked()) return true;
    else if (rmu_rb_rmml_no->isChecked()) return false;
    else return true;
}



//###########MCMEHSTRANGUI_TransparencyDlg###############//
MCMESHTRANGUI_TransparencyDlg::MCMESHTRANGUI_TransparencyDlg( QWidget* parent, const QStringList& entries )
  :QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint )
{
  myEntries = entries;

  resize(152, 107);
  setWindowTitle(tr("Transparency"));
  setSizeGripEnabled(TRUE);
  QGridLayout* lay = new QGridLayout(this);
  lay->setSpacing(6);
  lay->setMargin(11);

  /*************************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this );
  QGridLayout* GroupButtonsLayout = new QGridLayout( GroupButtons );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setSpacing( 6 );
  GroupButtonsLayout->setMargin( 11 );

  QPushButton* buttonOk = new QPushButton( GroupButtons );
  buttonOk->setText( tr( "OK" ) );
  buttonOk->setAutoDefault( TRUE );
  buttonOk->setDefault( TRUE );

  QPushButton* buttonHelp = new QPushButton( GroupButtons );
  buttonHelp->setText( tr( "Help" ) );
  buttonHelp->setAutoDefault( TRUE );
  buttonHelp->setDefault( TRUE );

  GroupButtonsLayout->addWidget( buttonOk, 0, 0 );
  GroupButtonsLayout->addItem( new QSpacerItem( 0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 0, 1 );
  GroupButtonsLayout->addWidget( buttonHelp, 0, 2 );

  /*************************************************************************/
  QGroupBox* GroupC1 = new QGroupBox( this );
  QGridLayout* GroupC1Layout = new QGridLayout( GroupC1 );
  GroupC1Layout->setAlignment( Qt::AlignTop );
  GroupC1Layout->setSpacing( 6 );
  GroupC1Layout->setMargin( 11 );
  GroupC1Layout->setColumnMinimumWidth(1, 100);

  QLabel* TextLabelOpaque = new QLabel( GroupC1 );
  TextLabelOpaque->setText( tr( "Transparent"  ) );
  TextLabelOpaque->setAlignment( Qt::AlignLeft );
  GroupC1Layout->addWidget( TextLabelOpaque, 0, 0 );
  GroupC1Layout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ), 0, 1 );

  QLabel* TextLabelTransparent = new QLabel( GroupC1 );
  TextLabelTransparent->setText( tr( "Opaque"  ) );
  TextLabelTransparent->setAlignment( Qt::AlignRight );
  GroupC1Layout->addWidget( TextLabelTransparent, 0, 2 );

  mySlider = new QSlider( Qt::Horizontal, GroupC1 );
  mySlider->setMinimum( 0 );
  mySlider->setMaximum( 10 );
  GroupC1Layout->addWidget( mySlider, 0, 1 );
  /*************************************************************************/

  lay->addWidget(GroupC1, 0,  0);
  lay->addWidget(GroupButtons, 1, 0);

  if ( myEntries.count() == 1 ) {
    int value = (int)(MCMESHTRANGUI_Displayer().getTransparency( myEntries[ 0 ] ) * 10.);
    if ( value > 0 )
      mySlider->setValue( value );
  }
  ValueHasChanged(mySlider->value());

  // signals and slots connections : after ValueHasChanged()
  connect(buttonOk, SIGNAL(clicked()), this, SLOT(accept()));
  connect(buttonHelp, SIGNAL(clicked()), this, SLOT(ClickOnHelp()));
  connect(mySlider, SIGNAL(valueChanged(int)), this, SLOT(ValueHasChanged(int)));
}

//=================================================================================
// function : ClickOnHelp()
// purpose  :
//=================================================================================
void MCMESHTRANGUI_TransparencyDlg::ClickOnHelp()
{
  // display a help in browser for example..
  // not implemented yet..
}

//=================================================================================
// function : ValueHasChanged()
// purpose  : Called when value of slider change
//          : or the first time as initilisation
//=================================================================================
void MCMESHTRANGUI_TransparencyDlg::ValueHasChanged( int newValue )
{
  MCMESHTRANGUI_Displayer().setTransparency( myEntries, (float)mySlider->value() * .1 );
}


//###########CompareDifferenceDlg###############//

MCMESHTRANGUI_CompareDifferenceDlg::MCMESHTRANGUI_CompareDifferenceDlg(QWidget*parent, MCMESHTRANGUI_DataModel * dm )
    :MCMESHTRANGUI_TemplateDlg(parent)
{
    setupUi(this);
    this->dm = dm;
    aCombobox = comp_cb_mt1;
    bCombobox = comp_cb_mt2;
    aLineEdit = comp_le_nmt;
    gCombobox = comp_cb_gp;
//    updateNameList();
    connect(this->dm->getMCMESHTRANGUImodule(),SIGNAL(selectChg()),this,SLOT(OnSelectGroup()) ,Qt::UniqueConnection);

}
MCMESHTRANGUI_CompareDifferenceDlg::~MCMESHTRANGUI_CompareDifferenceDlg()
{
}


