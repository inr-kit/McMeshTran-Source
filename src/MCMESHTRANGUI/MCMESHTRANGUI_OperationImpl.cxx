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
#include "MCMESHTRANGUI_OperationImpl.h"
#include "MCMESHTRANGUI_Dialogs.h"
#include "MCMESHTRANGUI_DataModel.h"
#include "MCMESHTRANGUI.h"
#include "MCMESHTRANGUI_DataObject.h"

#include <LightApp_Module.h>
#include <LightApp_Application.h>
#include <SUIT_Desktop.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Tools.h>
#include "utilities.h"

#include <qinputdialog.h>

//###########CreateGroupOp#################

MCMESHTRANGUI_CreateGroupOp::MCMESHTRANGUI_CreateGroupOp()
: MCMESHTRANGUI_Operation(),
  LastName("NewGroup")
{
}

MCMESHTRANGUI_CreateGroupOp::~MCMESHTRANGUI_CreateGroupOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_CreateGroupOp::startOperation()
{
    bool bOk;
    QString aName = QInputDialog::getText( module()->getApp()->desktop(),
                                           tr( "Create Mesh Group" ),
                                           tr( "Group Name"),
                                           QLineEdit::Normal,
                                           LastName, &bOk);
    if (aName.trimmed().isEmpty() || bOk == false)
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Name invalid or empty!"));
        abort();
        return;
    }
    LastName = aName;
    // call generate group
    if (dataModel()->generateGroup(aName))
        commit();
    else
        abort();
}

//###########ReadMCNPFmeshOp#################

MCMESHTRANGUI_ReadMCNPFmeshOp::MCMESHTRANGUI_ReadMCNPFmeshOp()
  : MCMESHTRANGUI_Operation()
//    myDlg( 0 )
{
}
/*! Destructor */
MCMESHTRANGUI_ReadMCNPFmeshOp::~MCMESHTRANGUI_ReadMCNPFmeshOp()
{
//  if ( myDlg )
//    delete myDlg;
}
/*! Returns Operation dialog */
MCMESHTRANGUI_TemplateDlg* MCMESHTRANGUI_ReadMCNPFmeshOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCMESHTRANGUI_ReadMCNPFmeshOp*>( this )->myDlg
          = new MCMESHTRANGUI_ReadMCNPFmeshDlg( module()->getApp()->desktop(), dataModel() );
  return myDlg;
}

void MCMESHTRANGUI_ReadMCNPFmeshOp::onApply()
{
    MCMESHTRANGUI_ReadMCNPFmeshDlg * Dialog = dynamic_cast< MCMESHTRANGUI_ReadMCNPFmeshDlg*> (dlg()) ;
    QString InputFileName = Dialog->getInputFileName();
    QString MeshtalFileName = Dialog->getMeshtalFileName();
//    QString InputFileName = "/home/qiu/Desktop/MCNPFmesh/3.NMT+XYZ+E"; //for debugging
//    QString MeshtalFileName = "/home/qiu/Desktop/MCNPFmesh/3.NMT+XYZ+E.msh";
    if (InputFileName.trimmed().isEmpty() || MeshtalFileName.trimmed().isEmpty())
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "File name empty!"));
        abort();
        return;
    }
    //call the method
    if (!dataModel()->readMCNPFmesh(InputFileName, MeshtalFileName))
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Read MCNP mesh tally failed! "));
        abort();
    }
}

//###########SumMeshesOp#################

MCMESHTRANGUI_SumMeshesOp::MCMESHTRANGUI_SumMeshesOp()
  : MCMESHTRANGUI_Operation()
//    myDlg( 0 )
{
}
/*! Destructor */
MCMESHTRANGUI_SumMeshesOp::~MCMESHTRANGUI_SumMeshesOp()
{
//  if ( myDlg )
//    delete myDlg;
}
/*! Returns Operation dialog */
MCMESHTRANGUI_TemplateDlg* MCMESHTRANGUI_SumMeshesOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCMESHTRANGUI_SumMeshesOp*>( this )->myDlg
          = new MCMESHTRANGUI_SumMeshesDlg( module()->getApp()->desktop(), dataModel() );
  return myDlg;
}

void MCMESHTRANGUI_SumMeshesOp::onApply()
{
    MCMESHTRANGUI_SumMeshesDlg * Dialog = dynamic_cast < MCMESHTRANGUI_SumMeshesDlg * > (dlg());
    QString aName = Dialog->getNewName();
    QString aEntry = Dialog->getEntry1();
    QString bEntry = Dialog->getEntry2();
    int aGroupID = Dialog->getGroupID();

    if (aName.trimmed().isEmpty() || aEntry.trimmed().isEmpty() || bEntry.trimmed().isEmpty() || aGroupID < 0)
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Name empty,  or no mesh or group selected!"));
        abort();
        return;
    }
    //call the method
    if (!dataModel()->sumMeshes(aEntry, bEntry, aName, aGroupID ))
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Operation failed! "));
        abort();
    }
}

//###########AverageMeshesOp#################

MCMESHTRANGUI_AverageMeshesOp::MCMESHTRANGUI_AverageMeshesOp()
  : MCMESHTRANGUI_Operation()
//    myDlg( 0 )
{
}
/*! Destructor */
MCMESHTRANGUI_AverageMeshesOp::~MCMESHTRANGUI_AverageMeshesOp()
{
//  if ( myDlg )
//    delete myDlg;
}
/*! Returns Operation dialog */
MCMESHTRANGUI_TemplateDlg* MCMESHTRANGUI_AverageMeshesOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCMESHTRANGUI_AverageMeshesOp*>( this )->myDlg
          = new MCMESHTRANGUI_AverageMeshesDlg( module()->getApp()->desktop(), dataModel() );
  return myDlg;
}

void MCMESHTRANGUI_AverageMeshesOp::onApply()
{
    MCMESHTRANGUI_AverageMeshesDlg * Dialog = dynamic_cast < MCMESHTRANGUI_AverageMeshesDlg * > (dlg());
    QString aName = Dialog->getNewName();
    QString aEntry = Dialog->getEntry1();
    QString bEntry = Dialog->getEntry2();
    int aGroupID = Dialog->getGroupID();

    if (aName.trimmed().isEmpty() || aEntry.trimmed().isEmpty() || bEntry.trimmed().isEmpty() || aGroupID < 0)
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Name empty,  or no mesh or group selected!"));
        abort();
        return;
    }
    //call the method
    if (!dataModel()->averageMeshes(aEntry, bEntry, aName, aGroupID ))
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Operation failed! "));
        abort();
    }
}


//###########TranslateMeshOp#################

MCMESHTRANGUI_TranslateMeshOp::MCMESHTRANGUI_TranslateMeshOp()
  : MCMESHTRANGUI_Operation()
//    myDlg( 0 )
{
}
/*! Destructor */
MCMESHTRANGUI_TranslateMeshOp::~MCMESHTRANGUI_TranslateMeshOp()
{
//  if ( myDlg )
//    delete myDlg;
}
/*! Returns Operation dialog */
MCMESHTRANGUI_TemplateDlg* MCMESHTRANGUI_TranslateMeshOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCMESHTRANGUI_TranslateMeshOp*>( this )->myDlg
          = new MCMESHTRANGUI_TranslateMeshDlg( module()->getApp()->desktop(), dataModel() );
  return myDlg;
}

void MCMESHTRANGUI_TranslateMeshOp::onApply()
{
    MCMESHTRANGUI_TranslateMeshDlg * Dialog = dynamic_cast < MCMESHTRANGUI_TranslateMeshDlg * > (dlg());
    QString aName = Dialog->getNewName();
    QString aEntry = Dialog->getEntry1();
    int aGroupID = Dialog->getGroupID();
    const double * aVector = Dialog->getVector();

    if (aName.trimmed().isEmpty() || aEntry.trimmed().isEmpty()  || aGroupID < 0)
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Name empty,  or no mesh or group selected!"));
        abort();
        return;
    }
    if (aVector[0] >= Null || aVector[1] >= Null || aVector[2] >= Null )
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Error in input vector!"));
        abort();
        return;
    }
    //call the method
    if (!dataModel()->translateMesh(aEntry, aVector, aName, aGroupID))
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Operation failed! "));
        delete [] aVector ;
        abort();
        return;
    }
    delete [] aVector ;
}


//###########RotateMeshOp#################

MCMESHTRANGUI_RotateMeshOp::MCMESHTRANGUI_RotateMeshOp()
  : MCMESHTRANGUI_Operation()
//    myDlg( 0 )
{
}
/*! Destructor */
MCMESHTRANGUI_RotateMeshOp::~MCMESHTRANGUI_RotateMeshOp()
{
//  if ( myDlg )
//    delete myDlg;
}
/*! Returns Operation dialog */
MCMESHTRANGUI_TemplateDlg* MCMESHTRANGUI_RotateMeshOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCMESHTRANGUI_RotateMeshOp*>( this )->myDlg
          = new MCMESHTRANGUI_RotateMeshDlg( module()->getApp()->desktop(), dataModel() );
  return myDlg;
}

void MCMESHTRANGUI_RotateMeshOp::onApply()
{
    MCMESHTRANGUI_RotateMeshDlg * Dialog = dynamic_cast < MCMESHTRANGUI_RotateMeshDlg * > (dlg());
    QString aName = Dialog->getNewName();
    QString aEntry = Dialog->getEntry1();
    int aGroupID = Dialog->getGroupID();
    const double * aVector = Dialog->getVector();
    const double * aCenter = Dialog->getCenter();
    const double   aAngle  = Dialog->getAngle();
    if (aName.trimmed().isEmpty() || aEntry.trimmed().isEmpty()  || aGroupID < 0)
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Name empty,  or no mesh or group selected!"));
        abort();
        return;
    }
    if (aVector[0] >= Null || aVector[1] >= Null || aVector[2] >= Null  ||
        aCenter[0] >= Null || aCenter[1] >= Null || aCenter[2] >= Null  || aAngle >= Null )
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Error in input vector, Center or Angle!"));
        abort();
        return;
    }
    //call the method
    if (!dataModel()->rotateMesh(aEntry, aCenter, aVector, aAngle, aName, aGroupID))
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Operation failed! "));
        delete [] aVector ;
        delete [] aCenter ;
        abort();
        return;
    }
    delete [] aVector ;
    delete [] aCenter ;
}

//###########ScaleMeshOp#################

MCMESHTRANGUI_ScaleMeshOp::MCMESHTRANGUI_ScaleMeshOp()
  : MCMESHTRANGUI_Operation()
//    myDlg( 0 )
{
}
/*! Destructor */
MCMESHTRANGUI_ScaleMeshOp::~MCMESHTRANGUI_ScaleMeshOp()
{
//  if ( myDlg )
//    delete myDlg;
}
/*! Returns Operation dialog */
MCMESHTRANGUI_TemplateDlg* MCMESHTRANGUI_ScaleMeshOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCMESHTRANGUI_ScaleMeshOp*>( this )->myDlg
          = new MCMESHTRANGUI_ScaleMeshDlg( module()->getApp()->desktop(), dataModel() );
  return myDlg;
}

void MCMESHTRANGUI_ScaleMeshOp::onApply()
{
    MCMESHTRANGUI_ScaleMeshDlg * Dialog = dynamic_cast < MCMESHTRANGUI_ScaleMeshDlg * > (dlg());
    QString aName = Dialog->getNewName();
    QString aEntry = Dialog->getEntry1();
    int aGroupID = Dialog->getGroupID();
    double aFactor = Dialog->getFactor();

    if (aName.trimmed().isEmpty() || aEntry.trimmed().isEmpty() || aGroupID < 0)
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Name empty,  or no mesh or group selected!"));
        abort();
        return;
    }
    //call the method
    if (!dataModel()->scaleMesh(aEntry, aFactor, aName, aGroupID ))
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Operation failed! "));
        abort();
    }
}

//###########MultiplyFactorOp#################

MCMESHTRANGUI_MultiplyFactorOp::MCMESHTRANGUI_MultiplyFactorOp()
  : MCMESHTRANGUI_Operation()
//    myDlg( 0 )
{
}
/*! Destructor */
MCMESHTRANGUI_MultiplyFactorOp::~MCMESHTRANGUI_MultiplyFactorOp()
{
//  if ( myDlg )
//    delete myDlg;
}
/*! Returns Operation dialog */
MCMESHTRANGUI_TemplateDlg* MCMESHTRANGUI_MultiplyFactorOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCMESHTRANGUI_MultiplyFactorOp*>( this )->myDlg
          = new MCMESHTRANGUI_MultiplyFactorDlg( module()->getApp()->desktop(), dataModel() );
  return myDlg;
}

void MCMESHTRANGUI_MultiplyFactorOp::onApply()
{
    MCMESHTRANGUI_MultiplyFactorDlg * Dialog = dynamic_cast < MCMESHTRANGUI_MultiplyFactorDlg * > (dlg());
    QString aName = Dialog->getNewName();
    QString aEntry = Dialog->getEntry1();
    int aGroupID = Dialog->getGroupID();
    double aFactor = Dialog->getFactor();

    if (aName.trimmed().isEmpty() || aEntry.trimmed().isEmpty() || aGroupID < 0)
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Name empty,  or no mesh or group selected!"));
        abort();
        return;
    }
    //call the method
    if (!dataModel()->multiplyFactor(aEntry, aFactor, aName, aGroupID ))
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Operation failed! "));
        abort();
    }
}

//###########InterpolateMeshOp#################

MCMESHTRANGUI_InterpolateMeshOp::MCMESHTRANGUI_InterpolateMeshOp()
  : MCMESHTRANGUI_Operation()
//    myDlg( 0 )
{
}
/*! Destructor */
MCMESHTRANGUI_InterpolateMeshOp::~MCMESHTRANGUI_InterpolateMeshOp()
{
//  if ( myDlg )
//    delete myDlg;
}
/*! Returns Operation dialog */
MCMESHTRANGUI_TemplateDlg* MCMESHTRANGUI_InterpolateMeshOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCMESHTRANGUI_InterpolateMeshOp*>( this )->myDlg
          = new MCMESHTRANGUI_InterpolateMeshDlg( module()->getApp()->desktop(), dataModel() );
  return myDlg;
}

void MCMESHTRANGUI_InterpolateMeshOp::onApply()
{
    MCMESHTRANGUI_InterpolateMeshDlg * Dialog = dynamic_cast < MCMESHTRANGUI_InterpolateMeshDlg * > (dlg());
    QString aName = Dialog->getNewName();
    QString aEntry = Dialog->getEntry1();
    QString bEntry = Dialog->getEntry2();
    int aGroupID = Dialog->getGroupID();
    int theIntpOption = Dialog->getIntpOption();

//    QString aMeshFileName = Dialog->getTargetMeshFileName();

    if (aName.trimmed().isEmpty() || aEntry.trimmed().isEmpty() || bEntry.trimmed().isEmpty() || aGroupID < 0)
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Name empty, no mesh or group selected!"));
        abort();
        return;
    }
    if (!dataModel()->interpolateMesh(aEntry, bEntry, aName, aGroupID, theIntpOption ))
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Operation failed! "));
        abort();
    }
}

//###############Export2VTKOp#############//

MCMESHTRANGUI_Export2VTKOp::MCMESHTRANGUI_Export2VTKOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/untitled.vtk")

{
}

MCMESHTRANGUI_Export2VTKOp::~MCMESHTRANGUI_Export2VTKOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_Export2VTKOp::startOperation()
{

    QString fileName = QFileDialog::getSaveFileName(module()->getApp()->desktop(),
                                                    tr("Save to VTK file"),
                                                    LastDir,
                                                    tr("VTK (*.vtk);;All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
        //no need to warn
//        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
//                                  QString( "File directory invalid or name empty!"));
        abort();
        return;
    }
    if (SUIT_Tools::extension(fileName).toLower() != "vtk")
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "The extension will be changed to *.vtk"));
        fileName = SUIT_Tools::addSlash(SUIT_Tools::dir(fileName)) + SUIT_Tools::file(fileName, false); //get a filename without extension
        fileName += ".vtk";
    }

    MCMESHTRANGUI * aModule = dataModel()->getMCMESHTRANGUImodule();
    if (!aModule)
    {
        MESSAGE ("cannnot find the module!");
        abort();
        return;
    }
    QStringList aList;
    aModule->selected(aList, /*multiple object */ false);

        // call export to vtk
    if (dataModel()->export2VTK(aList.at(0), fileName))
        commit();
    else
        abort();
}

//###############Export2MED#############//

MCMESHTRANGUI_Export2MEDOp::MCMESHTRANGUI_Export2MEDOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/untitled.med")

{
}

MCMESHTRANGUI_Export2MEDOp::~MCMESHTRANGUI_Export2MEDOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_Export2MEDOp::startOperation()
{

    QString fileName = QFileDialog::getSaveFileName(module()->getApp()->desktop(),
                                                    tr("Save to MED file"),
                                                    LastDir,
                                                    tr("MED (*.med);;All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
//        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
//                                  QString( "File directory invalid or name empty!"));
        abort();
        return;
    }
    if (SUIT_Tools::extension(fileName).toLower() != "med")
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "The extension will be changed to *.med"));
        fileName = SUIT_Tools::addSlash(SUIT_Tools::dir(fileName)) + SUIT_Tools::file(fileName, false); //get a filename without extension
        fileName += ".med";
    }

    MCMESHTRANGUI * aModule = dataModel()->getMCMESHTRANGUImodule();
    if (!aModule)
    {
        MESSAGE ("cannnot find the module!");
        abort();
        return;
    }
    QStringList aList;
    aModule->selected(aList, /*multiple object */ false);

        // call export to MED
    if (dataModel()->export2MED(aList.at(0), fileName))
        commit();
    else
        abort();
}

//###############Export2CSVOp#############//

MCMESHTRANGUI_Export2CSVOp::MCMESHTRANGUI_Export2CSVOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/untitled.csv")

{
}

MCMESHTRANGUI_Export2CSVOp::~MCMESHTRANGUI_Export2CSVOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_Export2CSVOp::startOperation()
{

    QString fileName = QFileDialog::getSaveFileName(module()->getApp()->desktop(),
                                                    tr("Save to CSV file"),
                                                    LastDir,
                                                    tr("CSV (*.csv);;All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
//        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
//                                  QString( "File directory invalid or name empty!"));
        abort();
        return;
    }
    if (SUIT_Tools::extension(fileName).toLower() != "csv")
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "The extension will be changed to *.csv"));
        fileName = SUIT_Tools::addSlash(SUIT_Tools::dir(fileName)) + SUIT_Tools::file(fileName, false); //get a filename without extension
        fileName += ".csv";
    }

    MCMESHTRANGUI * aModule = dataModel()->getMCMESHTRANGUImodule();
    if (!aModule)
    {
        MESSAGE ("cannnot find the module!");
        abort();
        return;
    }
    QStringList aList;
    aModule->selected(aList, /*multiple object */ false);

        // call export to CSV
    if (dataModel()->export2CSV(aList.at(0), fileName))
        commit();
    else
        abort();
}

//###############Export2CGNSOp#############//
#ifdef WITH_CGNS

MCMESHTRANGUI_Export2CGNSOp::MCMESHTRANGUI_Export2CGNSOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/untitled.cgns")

{
}

MCMESHTRANGUI_Export2CGNSOp::~MCMESHTRANGUI_Export2CGNSOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_Export2CGNSOp::startOperation()
{

    QString fileName = QFileDialog::getSaveFileName(module()->getApp()->desktop(),
                                                    tr("Save to CGNS file"),
                                                    LastDir,
                                                    tr("CGNS (*.cgns);;All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
//        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
//                                  QString( "File directory invalid or name empty!"));
        abort();
        return;
    }
    if (SUIT_Tools::extension(fileName).toLower() != "cgns")
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "The extension will be changed to *.cgns"));
        fileName = SUIT_Tools::addSlash(SUIT_Tools::dir(fileName)) + SUIT_Tools::file(fileName, false); //get a filename without extension
        fileName += ".cgns";
    }

    MCMESHTRANGUI * aModule = dataModel()->getMCMESHTRANGUImodule();
    if (!aModule)
    {
        MESSAGE ("cannnot find the module!");
        abort();
        return;
    }
    QStringList aList;
    aModule->selected(aList, /*multiple object */ false);

        // call export to CGNS
    if (dataModel()->export2CGNS(aList.at(0), fileName))
        commit();
    else
        abort();
}
#endif //WITH_CGNS

//###############Export2FLUENTOp#############//

MCMESHTRANGUI_Export2FLUENTOp::MCMESHTRANGUI_Export2FLUENTOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/untitled.udf")

{
}

MCMESHTRANGUI_Export2FLUENTOp::~MCMESHTRANGUI_Export2FLUENTOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_Export2FLUENTOp::startOperation()
{

    QString fileName = QFileDialog::getSaveFileName(module()->getApp()->desktop(),
                                                    tr("Save to UDF file"),
                                                    LastDir,
                                                    tr("UDF (*.udf);;All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
//        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
//                                  QString( "File directory invalid or name empty!"));
        abort();
        return;
    }
    if (SUIT_Tools::extension(fileName).toLower() != "udf")
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "The extension will be changed to *.udf"));
        fileName = SUIT_Tools::addSlash(SUIT_Tools::dir(fileName)) + SUIT_Tools::file(fileName, false); //get a filename without extension
        fileName += ".udf";
    }

    MCMESHTRANGUI * aModule = dataModel()->getMCMESHTRANGUImodule();
    if (!aModule)
    {
        MESSAGE ("cannnot find the module!");
        abort();
        return;
    }
    QStringList aList;
    aModule->selected(aList, /*multiple object */ false);

        // call export to udf
    if (dataModel()->export2FLUENT(aList.at(0), fileName))
        commit();
    else
        abort();
}

//###############Export2CFXOp#############//

MCMESHTRANGUI_Export2CFXOp::MCMESHTRANGUI_Export2CFXOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/untitled.udf")

{
}

MCMESHTRANGUI_Export2CFXOp::~MCMESHTRANGUI_Export2CFXOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_Export2CFXOp::startOperation()
{

    QString fileName = QFileDialog::getSaveFileName(module()->getApp()->desktop(),
                                                    tr("Save to CFX UserFortran file"),
                                                    LastDir,
                                                    tr("UserFortran (*.F);;All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
//        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
//                                  QString( "File directory invalid or name empty!"));
        abort();
        return;
    }
    if (SUIT_Tools::extension(fileName).toLower() != "f")
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "The extension will be changed to *.F"));
        fileName = SUIT_Tools::addSlash(SUIT_Tools::dir(fileName)) + SUIT_Tools::file(fileName, false); //get a filename without extension
        fileName += ".F";
    }

    MCMESHTRANGUI * aModule = dataModel()->getMCMESHTRANGUImodule();
    if (!aModule)
    {
        MESSAGE ("cannnot find the module!");
        abort();
        return;
    }
    QStringList aList;
    aModule->selected(aList, /*multiple object */ false);

        // call export to User fortran
    if (dataModel()->export2CFX(aList.at(0), fileName))
        commit();
    else
        abort();
}
//###############ImportMEDOp#############//

MCMESHTRANGUI_ImportMEDOp::MCMESHTRANGUI_ImportMEDOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/")

{
}

MCMESHTRANGUI_ImportMEDOp::~MCMESHTRANGUI_ImportMEDOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_ImportMEDOp::startOperation()
{

    QString fileName = QFileDialog::getOpenFileName(module()->getApp()->desktop(),
                                                    tr("Open MED File"),
                                                    LastDir,
                                                    tr("MED (*.med);;All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
//        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
//                                  QString( "File directory invalid or name empty!"));
        abort();
        return;
    }
    if (SUIT_Tools::extension(fileName).toLower() != "med")
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "The extension is not *.med, might be a mistake. "));
    }
    LastDir = SUIT_Tools::dir(fileName);  //save the dir

    //get group id if selected
    QStringList aList ;
    MCMESHTRANGUIModule()->selected(aList, true);
       // call import MED
    bool isOk ;
    if (aList.size() > 0)
        isOk = dataModel()->importMED(fileName, dataModel()->getGroupID(aList.at(0)));
    else
        isOk = dataModel()->importMED(fileName);

    if (isOk)
        commit();
    else
        abort();
}

//###############ImportCGNSOp#############//

#ifdef WITH_CGNS

MCMESHTRANGUI_ImportCGNSOp::MCMESHTRANGUI_ImportCGNSOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/")

{
}

MCMESHTRANGUI_ImportCGNSOp::~MCMESHTRANGUI_ImportCGNSOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_ImportCGNSOp::startOperation()
{

    QString fileName = QFileDialog::getOpenFileName(module()->getApp()->desktop(),
                                                    tr("Open CGNS File"),
                                                    LastDir,
                                                    tr("CGNS (*.cgns);;All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
//        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
//                                  QString( "File directory invalid or name empty!"));
        abort();
        return;
    }
    if (SUIT_Tools::extension(fileName).toLower() != "cgns")
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "The extension is not *.cgns, might be a mistake. "));
    }
    LastDir = SUIT_Tools::dir(fileName);  //save the dir

    //get group id if selected
    QStringList aList ;
    MCMESHTRANGUIModule()->selected(aList, true);
       // call import CGNS
    bool isOk ;
    if (aList.size() > 0)
        isOk = dataModel()->importCGNS(fileName, dataModel()->getGroupID(aList.at(0)));
    else
        isOk = dataModel()->importCGNS(fileName);

    if (isOk)
        commit();
    else
        abort();
}

#endif //WITH_CGNS


//###############ImportAbaqusOp#############//

MCMESHTRANGUI_ImportAbaqusOp::MCMESHTRANGUI_ImportAbaqusOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/")

{
}

MCMESHTRANGUI_ImportAbaqusOp::~MCMESHTRANGUI_ImportAbaqusOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_ImportAbaqusOp::startOperation()
{

    QString fileName = QFileDialog::getOpenFileName(module()->getApp()->desktop(),
                                                    tr("Open Abaqus File"),
                                                    LastDir,
                                                    tr("Abaqus (*.inp);;All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
//        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
//                                  QString( "File directory invalid or name empty!"));
        abort();
        return;
    }
    if (SUIT_Tools::extension(fileName).toLower() != "inp")
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "The extension is not *.inp, might be a mistake. "));
    }
    LastDir = SUIT_Tools::dir(fileName);  //save the dir

    //get group id if selected
    QStringList aList ;
    MCMESHTRANGUIModule()->selected(aList, true);
       // call import Abaqus
    bool isOk ;
    if (aList.size() > 0)
        isOk = dataModel()->importAbaqus(fileName, dataModel()->getGroupID(aList.at(0)));
    else
        isOk = dataModel()->importAbaqus(fileName);

    if (isOk)
        commit();
    else
        abort();
}


//###########ReadMCNPUmeshOp#################

MCMESHTRANGUI_ReadMCNPUmeshOp::MCMESHTRANGUI_ReadMCNPUmeshOp()
  : MCMESHTRANGUI_Operation()
//    myDlg( 0 )
{
}
/*! Destructor */
MCMESHTRANGUI_ReadMCNPUmeshOp::~MCMESHTRANGUI_ReadMCNPUmeshOp()
{
//  if ( myDlg )
//    delete myDlg;
}
/*! Returns Operation dialog */
MCMESHTRANGUI_TemplateDlg* MCMESHTRANGUI_ReadMCNPUmeshOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCMESHTRANGUI_ReadMCNPUmeshOp*>( this )->myDlg
          = new MCMESHTRANGUI_ReadMCNPUmeshDlg( module()->getApp()->desktop(), dataModel() );
  return myDlg;
}

void MCMESHTRANGUI_ReadMCNPUmeshOp::onApply()
{
    MCMESHTRANGUI_ReadMCNPUmeshDlg * Dialog = dynamic_cast< MCMESHTRANGUI_ReadMCNPUmeshDlg*> (dlg()) ;
    QString UMeshOutFileName = Dialog->getUMeshOutFileName();

    if (UMeshOutFileName.trimmed().isEmpty() )
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "File name empty!"));
        abort();
        return;
    }
    //call the method
    if (!dataModel()->readMCNPUmesh(UMeshOutFileName, Dialog->getIsKeepInstance(), Dialog->getIsRemoveMultiplier()))
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Read MCNP6 UMesh eeout file failed! "));
        abort();
    }
}

//###############ReadTRIPOLIFmeshOp#############//

MCMESHTRANGUI_ReadTRIPOLIFmeshOp::MCMESHTRANGUI_ReadTRIPOLIFmeshOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/")

{
}

MCMESHTRANGUI_ReadTRIPOLIFmeshOp::~MCMESHTRANGUI_ReadTRIPOLIFmeshOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_ReadTRIPOLIFmeshOp::startOperation()
{

    QString fileName = QFileDialog::getOpenFileName(module()->getApp()->desktop(),
                                                    tr("Open TRIPOLI Output File"),
                                                    LastDir,
                                                    tr("All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
//        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
//                                  QString( "File directory invalid or name empty!"));
        abort();
        return;
    }

    LastDir = SUIT_Tools::dir(fileName);  //save the dir

    QMessageBox::StandardButton aBtn = SUIT_MessageBox::information(application()->desktop(),QString("Message"),
                                                                    QString( "Calculate volume average results?"),QMessageBox::Yes, QMessageBox::No);
    bool isVolumeAverage = false;
    if (aBtn == QMessageBox::Yes)
        isVolumeAverage = true;

       // call import Abaqus
    bool isOk ;

    isOk = dataModel()->readTRIPOLIFmesh(fileName, isVolumeAverage);

    if (isOk)
        commit();
    else
        abort();
}



//###############ExportMesh2AbaqusOp#############//

MCMESHTRANGUI_ExportMesh2AbaqusOp::MCMESHTRANGUI_ExportMesh2AbaqusOp()
: MCMESHTRANGUI_Operation(),
  LastDir("/home/untitled.inp")

{
}

MCMESHTRANGUI_ExportMesh2AbaqusOp::~MCMESHTRANGUI_ExportMesh2AbaqusOp()
{
}
/*! Operation itself. */
void MCMESHTRANGUI_ExportMesh2AbaqusOp::startOperation()
{

    QString fileName = QFileDialog::getSaveFileName(module()->getApp()->desktop(),
                                                    tr("Save to Abaqus file"),
                                                    LastDir,
                                                    tr("Abaqus(*.inp);;All(*.*)"));
    if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
    {
        abort();
        return;
    }
    if (SUIT_Tools::extension(fileName).toLower() != "inp")
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "The extension will be changed to *.inp"));
        fileName = SUIT_Tools::addSlash(SUIT_Tools::dir(fileName)) + SUIT_Tools::file(fileName, false); //get a filename without extension
        fileName += ".inp";
    }

    MCMESHTRANGUI * aModule = dataModel()->getMCMESHTRANGUImodule();
    if (!aModule)
    {
        MESSAGE ("cannnot find the module!");
        abort();
        return;
    }
    QStringList aList;
    aModule->selected(aList, /*multiple object */ true);

        // call export to User fortran
    if (dataModel()->exportMesh2Abaqus(aList, fileName))
        commit();
    else
        abort();
}



//###########CompareDifferenceOp#################

MCMESHTRANGUI_CompareDifferenceOp::MCMESHTRANGUI_CompareDifferenceOp()
  : MCMESHTRANGUI_Operation()
//    myDlg( 0 )
{
}
/*! Destructor */
MCMESHTRANGUI_CompareDifferenceOp::~MCMESHTRANGUI_CompareDifferenceOp()
{
//  if ( myDlg )
//    delete myDlg;
}
/*! Returns Operation dialog */
MCMESHTRANGUI_TemplateDlg* MCMESHTRANGUI_CompareDifferenceOp::dlg() const
{
  if ( !myDlg )
    const_cast<MCMESHTRANGUI_CompareDifferenceOp*>( this )->myDlg
          = new MCMESHTRANGUI_CompareDifferenceDlg( module()->getApp()->desktop(), dataModel() );
  return myDlg;
}

void MCMESHTRANGUI_CompareDifferenceOp::onApply()
{
    MCMESHTRANGUI_CompareDifferenceDlg * Dialog = dynamic_cast < MCMESHTRANGUI_CompareDifferenceDlg * > (dlg());
    QString aName = Dialog->getNewName();
    QString aEntry = Dialog->getEntry1();
    QString bEntry = Dialog->getEntry2();
    int aGroupID = Dialog->getGroupID();

    if (aName.trimmed().isEmpty() || aEntry.trimmed().isEmpty() || bEntry.trimmed().isEmpty() || aGroupID < 0)
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Name empty,  or no mesh or group selected!"));
        abort();
        return;
    }
    //call the method
    if (!dataModel()->compareDifference(aEntry, bEntry, aName, aGroupID ))
    {
        SUIT_MessageBox::warning( module()->getApp()->desktop(), QString("Warning"),
                                  QString( "Operation failed! "));
        abort();
    }
}
