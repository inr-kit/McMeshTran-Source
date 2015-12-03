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

//myself
#include "MCMESHTRANGUI.h"
#include "MCMESHTRANGUI_DataModel.h"
#include "MCMESHTRANGUI_Displayer.h"
#include "MCMESHTRANGUI_Selection.h"
#include "MCMESHTRANGUI_Dialogs.h"
#include "MCMESHTRANGUI_OperationImpl.h"

//MEDCoupling
#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingMeshServant.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"
#include "MEDCouplingAutoRefCountObjectPtr.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDLoader.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingField.hxx"
#include "MEDCouplingFieldTemplateClient.hxx"
//SALOME
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_Tools.h>
#include <SVTK_ViewModel.h>
#include <SVTK_ViewWindow.h>
//#include <SALOME_Actor.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Study.h>
#include <LightApp_DataOwner.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_Preferences.h>

#include "SALOME_InteractiveObject.hxx"
#include "SALOMEDSClient_SObject.hxx"
#include "SALOMEDS_SObject.hxx"


// QT includes
#include <qinputdialog.h>
#include <qaction.h>
#include <qstringlist.h>
#include <QtxPopupMgr.h>

// VTK includes
#include <vtkActorCollection.h>
#include <vtkRenderer.h>

//GEOM
#include <GEOM_Client.hxx>
#include <GeometryGUI.h>

//stardart c++
#include <iostream>
#include <string>


using namespace std;
#define MESSAGE(msg) {MESS_BEGIN("- Trace ") << msg << MESS_END}

MCMESHTRAN_ORB::MCMESHTRAN_Gen_var MCMESHTRANGUI::myEngine = MCMESHTRAN_ORB::MCMESHTRAN_Gen::_nil();

// Constructor
MCMESHTRANGUI::MCMESHTRANGUI() :
    SalomeApp_Module( "MCMESHTRAN" ), // default name
//    LightApp_Module( "MCMESHTRAN" ), // default name //2013-05-23 modif to adapt change in SALOME 7.2.0

    LastDir("~/")

{
}


// returns a custom displayer object
LightApp_Displayer* MCMESHTRANGUI::displayer()
{
  return new MCMESHTRANGUI_Displayer();
}

// Module's initialization
void MCMESHTRANGUI::initialize( CAM_Application* app )
{

    SalomeApp_Module::initialize( app );

    InitMCMESHTRANGen( dynamic_cast<SalomeApp_Application*>( app ) );
    if (CORBA::is_nil( myEngine ))
        cout <<"initialize() load engine failed! "<<endl;

    SUIT_Desktop* aParent = app->desktop();
    //  SUIT_ResourceMgr* aResourceMgr = app->resourceMgr();

    createAction( unittest,"UnitTest", QIcon(), "UnitTest",
                  "UnitTest", 0, aParent, false, this, SLOT( OnUnitTest() ) );
    createAction( lgReadMCNPFmesh,              //required action ID
                  tr("Read MCNP mesh tally"),   // tooltip text
                  QIcon(),                      //action icon
                  tr("Read MCNP mesh tally"),          //menu text
                  tr("Read MCNP mesh tally"),          //status bar tip
                  0,                            //keyboard accelerator
                  aParent,                      //parent object
                  false,                        //if true, the action will be toggled
                  this,                         //action activation signal receiver object
                  SLOT( onProcess() ) );      //action activation signal receiver slot
    createAction( lgSumMeshes,"Sum Meshes", QIcon(), "Sum Meshes",
                  "Sum Meshes", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgAverageMeshes,"Average Meshes", QIcon(), "Average Meshes",
                  "Average Meshes", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgTranslateMesh,"Translate Mesh", QIcon(), "Translate Mesh",
                  "Translate Mesh", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgRotateMesh,"Rotate Mesh", QIcon(), "Rotate Mesh",
                  "Rotate Mesh", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgScaleMesh,"Scale Mesh", QIcon(), "Scale Mesh",
                  "Scale Mesh", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgMultiplyFactor,"Multiply Factor", QIcon(), "Multiply Factor",
                  "MultiplyFactor", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgInterpolateMesh,"Interpolate Mesh", QIcon(), "Interpolate Mesh",
                  "Interpolate Mesh", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgExport2VTK,"Export to VTK", QIcon(), "Export to VTK",
                  "Export to VTK", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgExport2MED,"Export to MED", QIcon(), "Export to MED",
                  "Export to MED", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgExport2CSV,"Export to CSV", QIcon(), "Export to CSV",
                  "Export to CSV", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgImportMED,"Import MED", QIcon(), "Import MED",
                  "Import MED", 0, aParent, false, this, SLOT( onProcess() ) );
#ifdef WITH_CGNS
    createAction( lgExport2CGNS,"Export to CGNS", QIcon(), "Export to CGNS",
                  "Export to CGNS", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgImportCGNS,"Import CGNS", QIcon(), "Import CGNS",
                  "Import CGNS", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgImportCGNSMesh,"Import CGNSMesh", QIcon(), "Import CGNSMesh",
                  "Import CGNSMesh", 0, aParent, false, this, SLOT( onImportMesh() ) );
#endif //WITH_CGNS

    createAction( lgImportUNVMesh,"Import UNV Mesh", QIcon(), "Import UNV Mesh",
                  "Import UNV Mesh", 0, aParent, false, this, SLOT( onImportMesh() ) );
    createAction( lgImportSAUVMesh,"Import SAUV Mesh", QIcon(), "Import SAUV Mesh",
                  "Import SAUV Mesh", 0, aParent, false, this, SLOT(onImportMesh() ) );
    createAction( lgImportSTLMesh,"Import STL Mesh", QIcon(), "Import STL Mesh",
                  "Import STL Mesh", 0, aParent, false, this, SLOT( onImportMesh() ) );
    createAction( lgImportGMFMesh,"Import GMF Mesh", QIcon(), "Import GMF Mesh",
                  "Import GMF Mesh", 0, aParent, false, this, SLOT( onImportMesh() ) );
    createAction( lgExport2FLUENT,"Export to FLUENT", QIcon(), "Export to FLUENT",
                  "Export to FLUENT", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgExport2CFX,"Export to CFX", QIcon(), "Export to CFX",
                  "Export to CFX", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgImportAbaqus,"Import Abaqus Mesh", QIcon(), "Import Abaqus mesh",
                  "Import Abaqus mesh", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgReadMCNPUmesh,
                  tr("Read MCNP6 UMesh eeout file"),QIcon(), tr("Read MCNP6 UMesh eeout file"),
                  tr("Read MCNP6 UMesh eeout file"),0, aParent,false,this,SLOT( onProcess() ) );
    createAction( lgExportMesh2Abaqus,"Export to Abaqus", QIcon(), "Export to Abaqus",
                  "Export to Abaqus", 0, aParent, false, this, SLOT( onProcess() ) );

    createAction( lgSend2SMESH,"Send to SMESH", QIcon(), "Send to SMESH",
                  "Send to SMESH", 0, aParent, false, this, SLOT( onSend2SMESH() ) );
    createAction( lgReadTRIPOLIFmesh,
                  tr("Read TRIPOLI output file"),QIcon(), tr("Read TRIPOLI output file"),
                  tr("Read TRIPOLI output file"),0, aParent,false,this,SLOT( onProcess() ) );
    createAction( lgCompareDifference,"Compute Difference", QIcon(), "Compute Difference",
                  "Compute Difference", 0, aParent, false, this, SLOT( onProcess() ) );



    createAction( lgCreateGroup,"Create Group", QIcon(), "Create Group",
                  "Create Group", 0, aParent, false, this, SLOT( onProcess() ) );
    createAction( lgDeleteGroups,"Delete Groups", QIcon(), "Delete",
                  "Delete Groups", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgDeleteMeshes,"Delete mesh(es)", QIcon(), "Delete",
                  "Delete mesh(es)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgRenameGroup,"Rename Group", QIcon(), "Rename ",
                  "Rename Group", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgRenameMesh,"Rename Mesh", QIcon(), "Rename ",
                  "Rename Mesh", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgCopyMeshes,"Copy mesh(es)", QIcon(), "Copy",
                  "Copy mesh(es)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgCutMeshes,"Cut mesh(es)", QIcon(), "Cut",
                  "Cut mesh(es)", 0, aParent, false, this, SLOT( onOperation() ) );
    createAction( lgPasteMeshes,"Paste mesh(es)", QIcon(), "Paste",
                  "Paste mesh(es)", 0, aParent, false, this, SLOT( onOperation() ) );

    createAction( lgDisplayInPV,"Display in Paraview", QIcon(), "Display in Paraview",
                  "Display in Paraview", 0, aParent, false, this, SLOT( onDisplayPVIS() ) );
    createAction( lgDisplayGEOMObjInPV,"Display GEOM Object in Paraview", QIcon(), "Display GEOM Object in Paraview",
                  "Display GEOM Object in Paraview", 0, aParent, false, this, SLOT( onDisplayGEOMObjInPVIS() ) );
    createAction( lgDisplay,"Display", QIcon(), "Display",
                  "Display", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgErase,"Erase", QIcon(), "Erase",
                  "Erase", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgShading,"Shading", QIcon(), "Shading",
                  "Shading", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgWireframe,"Wireframe", QIcon(), "Wireframe",
                  "Wireframe", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgPointsMode,"PointMode", QIcon(), "PointMode",
                  "PointMode", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgTransparency,"Transparency", QIcon(), "Transparency",
                  "Transparency", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgShadingWithWireframe,"Shading with Edge", QIcon(), "Shading with Edge",
                  "Shading with Edge", 0, aParent, false, this, SLOT( OnDisplayerCommand() ) );
    createAction( lgExperiment,"Experiment", QIcon(), "Experiment",
                  "Experiment", 0, aParent, false, this, SLOT( OnExperiment() ) );


    // create menus
    int aFileMnu = createMenu( tr( "File" ), -1, -1 );
    int aImportMCMnu = createMenu( tr( "Import MC Data" ), aFileMnu, -1, 10 );
    int aImportExtDataMnu = createMenu( tr( "Import External Data" ), aFileMnu, -1, 10 );
    int aImportExtMeshMnu = createMenu( tr( "Import External Mesh" ), aFileMnu, -1, 10 );
    int aExportMnu = createMenu( tr( "Export Data" ), aFileMnu, -1, 10 );
    int aMenuId = createMenu( "McMeshTran", -1, -1, 30 );

    createMenu( unittest, aMenuId, 10 );

//    createMenu( lgCreateGroup, aMenuId, 10 );
    createMenu( lgReadMCNPFmesh, aImportMCMnu, 10 );
    createMenu( lgImportMED,aImportExtDataMnu , 10 );
#ifdef WITH_CGNS
    createMenu( lgImportCGNS, aImportExtDataMnu, 10 );
    createMenu( lgImportCGNSMesh,aImportExtMeshMnu , 10 );
    createMenu( lgExport2CGNS,aExportMnu , 10 );
#endif
    createMenu( lgImportUNVMesh, aImportExtMeshMnu, 10 );
    createMenu( lgImportSAUVMesh,aImportExtMeshMnu , 10 );
    createMenu( lgImportSTLMesh, aImportExtMeshMnu, 10 );
    createMenu( lgImportAbaqus,  aImportExtMeshMnu , 10 );
//    createMenu( lgImportGMFMesh,aImportExtMeshMnu , 10 );
    createMenu( lgExport2VTK,aExportMnu , 10 );
    createMenu( lgExport2MED,aExportMnu , 10 );
    createMenu( lgExport2CSV, aExportMnu, 10 );
    createMenu( lgExport2FLUENT, aExportMnu, 10 );
    createMenu( lgExport2CFX, aExportMnu, 10 );
    createMenu( lgExportMesh2Abaqus, aExportMnu, 10 );
    createMenu( lgReadMCNPUmesh, aImportMCMnu, 10 );
    createMenu( lgReadTRIPOLIFmesh, aImportMCMnu, 10 );

    createMenu( lgSumMeshes, aMenuId, 10 );
    createMenu( lgAverageMeshes, aMenuId, 10 );
    createMenu( lgTranslateMesh, aMenuId, 10 );
    createMenu( lgRotateMesh, aMenuId, 10 );
    createMenu( lgScaleMesh, aMenuId, 10 );
    createMenu( lgMultiplyFactor, aMenuId, 10 );
    createMenu( lgInterpolateMesh, aMenuId, 10 );
    createMenu( lgCompareDifference, aMenuId, 10 );


    createMenu( lgExperiment, aMenuId, 10 );

    QtxPopupMgr* mgr = popupMgr();

    mgr->insert( action( lgRenameGroup ), -1, 0 );
    mgr->insert( action( lgRenameMesh ), -1, 0 );
    mgr->insert( action( lgCopyMeshes ), -1, 0 );
    mgr->insert( action( lgCutMeshes ), -1, 0 );
    mgr->insert( action( lgPasteMeshes ), -1, 0 );
    mgr->insert( action( lgDeleteGroups ), -1, 0 );
    mgr->insert( action( lgDeleteMeshes ), -1, 0 );
    mgr->insert( separator(),            -1, 0  );
    mgr->insert( action( lgCreateGroup ), -1, 0 );
    mgr->insert( separator(),            -1, 0  );
    mgr->insert( action( lgDisplay ), -1, 0 );
    mgr->insert( action( lgErase ), -1, 0 );
    int dispmodeId = mgr->insert(  tr( "Display Mode" ), -1, -1 ); // display mode menu
    mgr->insert( action(  lgPointsMode ), dispmodeId, -1 );
    mgr->insert( action(  lgWireframe ), dispmodeId, -1 );
    mgr->insert( action(  lgShading ), dispmodeId, -1 );
    mgr->insert( action(  lgShadingWithWireframe ), dispmodeId, -1 );
    mgr->insert( action(  lgTransparency ), -1, 0 );

    mgr->insert( action( lgDisplayInPV ), -1, 0 );
    mgr->insert( action( lgSend2SMESH ), -1, 0 );
    mgr->insert( action( lgDisplayGEOMObjInPV), -1, 0 );
    mgr->insert( separator(),            -1, 0  );
    int aOpId = mgr->insert("Operation", -1, -1);
    mgr->insert(action(lgSumMeshes), aOpId , 0);
    mgr->insert(action(lgAverageMeshes),aOpId , 0);
    mgr->insert(action(lgScaleMesh),aOpId , 0);
    mgr->insert(action(lgMultiplyFactor),aOpId , 0);
    mgr->insert(action(lgTranslateMesh),aOpId , 0);
    mgr->insert(action(lgRotateMesh),aOpId , 0);
    mgr->insert(action(lgInterpolateMesh),aOpId , 0);
    mgr->insert(action(lgCompareDifference), aOpId , 0);

    int aExpId = mgr->insert("Export", -1, -1);
    mgr->insert(action(lgExport2MED), aExpId, 0);
#ifdef WITH_CGNS
    mgr->insert(action(lgExport2CGNS), aExpId, 0);
#endif
    mgr->insert(action(lgExport2VTK), aExpId, 0);
    mgr->insert(action(lgExport2CSV), aExpId, 0);
    mgr->insert(action(lgExport2FLUENT), aExpId, 0);
    mgr->insert(action(lgExport2CFX), aExpId, 0);
    mgr->insert(action(lgExportMesh2Abaqus), aExpId, 0);

    QString rule = "(client='ObjectBrowser') and activeModule='MCMESHTRAN' ";
    mgr->setRule( action( lgCreateGroup ), rule );
//                  "selcount=0 or (!isMesh and !isGroup)");
    mgr->setRule( action( lgDeleteGroups ), rule +
                  " and isGroup and selcount>0");
    mgr->setRule( action( lgRenameGroup ), rule +
                  " and isGroup and selcount=1");

    mgr->setRule( action( lgRenameMesh ), rule +
                  " and isMesh and selcount=1");
    mgr->setRule( action( lgCopyMeshes ), rule +
                  " and isMesh and selcount>0");
    mgr->setRule( action( lgCutMeshes ), rule +
                  " and isMesh and selcount>0");
    mgr->setRule( action( lgPasteMeshes ), rule +
                  " and selcount=1 and isPastable");
    mgr->setRule( action( lgDeleteMeshes ), rule +
                  " and isMesh and selcount>0");
    rule = "(client='ObjectBrowser') and activeModule='MCMESHTRAN' ";
    mgr->setRule( action( lgExport2MED ), rule +
                  " and isMesh and selcount=1 " );
#ifdef WITH_CGNS
    mgr->setRule( action( lgExport2CGNS ), rule +
                  " and isMesh and selcount=1 " );
#endif
    mgr->setRule( action( lgExport2VTK ), rule +
                  " and isMesh and selcount=1 " );
    mgr->setRule( action( lgExport2CSV ), rule +
                  " and isMesh and selcount=1 " );
    mgr->setRule( action( lgExport2FLUENT ), rule +
                  " and isMesh and selcount=1 " );
    mgr->setRule( action( lgExport2CFX ), rule +
                  " and isMesh and selcount=1 " );
    mgr->setRule( action( lgExportMesh2Abaqus ), rule +
                  " and isMesh and selcount>0 " );

    mgr->setRule( action( lgSumMeshes ), rule +
                  " and isMesh and selcount=2 " );
    mgr->setRule( action( lgAverageMeshes ), rule +
                  " and isMesh and selcount=2 " );
    mgr->setRule( action( lgScaleMesh ), rule +
                  " and isMesh and selcount=1 " );
    mgr->setRule( action( lgMultiplyFactor ), rule +
                  " and isMesh and selcount=1 " );
    mgr->setRule( action( lgTranslateMesh ), rule +
                  " and isMesh and selcount=1 " );
    mgr->setRule( action( lgRotateMesh ), rule +
                  " and isMesh and selcount=1 " );
    mgr->setRule( action( lgInterpolateMesh ), rule +
                  " and isMesh and selcount=2 " );
    mgr->setRule( action( lgCompareDifference ), rule +
                  " and isMesh and selcount=2 " );


    rule = "(client='ObjectBrowser' or client='VTKViewer') and activeModule='MCMESHTRAN' ";
    mgr->setRule( action( lgDisplayInPV ), rule +
                  " and isMesh and selcount>0 and true in $canBeDisplayed");
    mgr->setRule( action( lgDisplay ), rule +
                  " and isMesh and selcount>0 and true in $canBeDisplayed and !isVisible");
    mgr->setRule( action( lgErase ), rule +
                  " and isMesh and selcount>0 and true in $canBeDisplayed and isVisible");
    mgr->setRule( action( lgShading ), rule +
                  " and isMesh and selcount>0 and true in $canBeDisplayed and isVisible");
    mgr->setRule( action( lgWireframe ), rule +
                  " and isMesh and selcount>0 and true in $canBeDisplayed and isVisible");
    mgr->setRule( action( lgPointsMode ), rule +
                  " and isMesh and selcount>0 and true in $canBeDisplayed and isVisible");
    mgr->setRule( action( lgShadingWithWireframe ), rule +
                  " and isMesh and selcount>0 and true in $canBeDisplayed and isVisible");
    mgr->setRule( action( lgTransparency ), rule +
                  " and isMesh and selcount>0 and true in $canBeDisplayed and isVisible");
    mgr->setRule( action( lgDisplayGEOMObjInPV ), rule +
                  " and (!isMesh and !isGroup) and selcount=1 ");
    mgr->setRule( action( lgSend2SMESH ), rule +
                  " and isMesh and selcount>0");}

//reimplement to avoid calling dm->build()
void MCMESHTRANGUI::updateObjBrowser(bool isUpdateDataModel, SUIT_DataObject *)
{
    MCMESHTRANGUI_DataModel * dm = dynamic_cast <MCMESHTRANGUI_DataModel *> (dataModel());
    if (!dm)
    {
        MESSAGE("get Data Model failed");
        return;
    }
    dm->buildTree();
    SalomeApp_Module::updateObjBrowser(false);
}

// Default windows
void MCMESHTRANGUI::windows( QMap<int, int>& theMap ) const
{
    theMap.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
    theMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
}

/*! Returns list of entities of selected objects. */
void MCMESHTRANGUI::selected( QStringList& entries, const bool multiple )
{
    LightApp_Application* app = getApp();
    LightApp_SelectionMgr* mgr = app ? app->selectionMgr() : 0;
    if( !mgr )
        return;

    SUIT_DataOwnerPtrList anOwnersList;
    mgr->selected( anOwnersList );

    for ( int i = 0; i < anOwnersList.size(); i++ )
    {
        const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( anOwnersList[ i ].get() );
        QStringList es = owner->entry().split( "_" );
        if ( es.count() > 1 && es[ 0 ] == "MCMESHTRANGUI" && es[ 1 ] != "root" )
        {
            if ( !entries.contains( owner->entry() ) )
                entries.append( owner->entry() );
            if( !multiple )
                break;
        }
    }
}

// Module's engine IOR
QString MCMESHTRANGUI::engineIOR() const
{
    cout <<"****first call of engineIOR()"<<endl;
    CORBA::String_var anIOR = getApp()->orb()->object_to_string( GetMCMESHTRANGen() );
    return QString( anIOR.in() );
}

// Initialize a reference to the module's engine
void MCMESHTRANGUI::InitMCMESHTRANGen( SalomeApp_Application* app )
{
    if ( !app )
    {
        myEngine = MCMESHTRAN_ORB::MCMESHTRAN_Gen::_nil();
    }
    else
    {
        Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component( "FactoryServer", "MCMESHTRAN" );
//        if ( CORBA::is_nil(comp  ) )
//            cout << "InitMCMESHTRANGen(): the Component is nil!"<<endl;
        MCMESHTRAN_ORB::MCMESHTRAN_Gen_ptr mcmeshtran_gen = MCMESHTRAN_ORB::MCMESHTRAN_Gen::_narrow(comp);
        ASSERT( !CORBA::is_nil( mcmeshtran_gen ) );
        myEngine = mcmeshtran_gen;
    }
}

// Gets an reference to the module's engine
MCMESHTRAN_ORB::MCMESHTRAN_Gen_var MCMESHTRANGUI::GetMCMESHTRANGen()
{
    if ( CORBA::is_nil( myEngine ) ) {
        SUIT_Application* suitApp = SUIT_Session::session()->activeApplication();
        SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( suitApp );
        InitMCMESHTRANGen( app );
    }
    return myEngine;
}

///*!
// * \brief return a Engines::EngineComponent_var that is SMESH::SMESH_Gen_var
// *  before used this should be narrowed to SMESH::SMESH_Gen_var
// * \return a Engines::EngineComponent_var
// */
//Engines::EngineComponent_var MCMESHTRANGUI::GetSMESHGen()
//{
//    if ( CORBA::is_nil( mySMESHGen ) ) {
//        SUIT_Application* suitApp = SUIT_Session::session()->activeApplication();
//        SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( suitApp );
//        if ( !app )
//        {
//            mySMESHGen =Engines::EngineComponent::_nil();
//        }
//        else
//        {
//            Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component( "FactoryServer", "SMESH" );
////            SMESH::SMESH_Gen_ptr smesh_gen = SMESH::SMESH_Gen::_narrow(comp);
////            ASSERT( !CORBA::is_nil( smesh_gen ) );
//            mySMESHGen = comp._retn();  // return the father class instance
//        }
//    }
//    return mySMESHGen;
//}

///*!
// * \brief return a Engines::EngineComponent_var that is PARAVIS::PARAVIS_Gen_var
// *  before used this should be narrowed to PARAVIS::PARAVIS_Gen_var
// * \return a Engines::EngineComponent_var
// */
//Engines::EngineComponent_var MCMESHTRANGUI::GetPVGen()
//{
//    if ( CORBA::is_nil( myPVGen ) ) {
//        SUIT_Application* suitApp = SUIT_Session::session()->activeApplication();
//        SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( suitApp );
//        if ( !app )
//        {
//            myPVGen = Engines::EngineComponent::_nil();
//        }
//        else
//        {
//            Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component( "FactoryServer", "PARAVIS" );
////            PARAVIS::PARAVIS_Gen_ptr pv_gen = PARAVIS::PARAVIS_Gen::_narrow(comp);
////            ASSERT( !CORBA::is_nil( pv_gen ) );
//            myPVGen = comp._retn();  // return the father class instance
//        }
//    }
//    return myPVGen;
//}

// Default view managers
void MCMESHTRANGUI::viewManagers( QStringList& theViewMgrs ) const
{
  theViewMgrs.append( SVTK_Viewer::Type() );
}

// Create custom data model
CAM_DataModel* MCMESHTRANGUI::createDataModel()
{
    return new MCMESHTRANGUI_DataModel( this );
}

/*! Instantiation of a custom Operation object - component's action manager. */
LightApp_Operation* MCMESHTRANGUI::createOperation( const int id ) const
{
  switch( id )
  {
  case lgCreateGroup:
    return new MCMESHTRANGUI_CreateGroupOp();
  case lgReadMCNPFmesh:
    return new MCMESHTRANGUI_ReadMCNPFmeshOp();
  case lgSumMeshes:
    return new MCMESHTRANGUI_SumMeshesOp();
  case lgAverageMeshes:
    return new MCMESHTRANGUI_AverageMeshesOp();
  case lgTranslateMesh:
    return new MCMESHTRANGUI_TranslateMeshOp();
  case lgRotateMesh:
    return new MCMESHTRANGUI_RotateMeshOp();
  case lgScaleMesh:
    return new MCMESHTRANGUI_ScaleMeshOp();
  case lgMultiplyFactor:
    return new MCMESHTRANGUI_MultiplyFactorOp();
  case lgInterpolateMesh:
    return new MCMESHTRANGUI_InterpolateMeshOp();
  case lgExport2VTK:
    return new MCMESHTRANGUI_Export2VTKOp();
  case lgExport2MED:
    return new MCMESHTRANGUI_Export2MEDOp();
  case lgExport2CSV:
    return new MCMESHTRANGUI_Export2CSVOp();
#ifdef WITH_CGNS
  case lgExport2CGNS:
    return new MCMESHTRANGUI_Export2CGNSOp();
  case lgImportCGNS:
      return new MCMESHTRANGUI_ImportCGNSOp();
#endif
  case lgImportMED:
    return new MCMESHTRANGUI_ImportMEDOp();

  case lgExport2FLUENT:
      return new MCMESHTRANGUI_Export2FLUENTOp();
  case lgExport2CFX:
      return new MCMESHTRANGUI_Export2CFXOp();
  case lgImportAbaqus:
      return new MCMESHTRANGUI_ImportAbaqusOp();
  case lgReadMCNPUmesh:
    return new MCMESHTRANGUI_ReadMCNPUmeshOp();
  case lgReadTRIPOLIFmesh:
    return new MCMESHTRANGUI_ReadTRIPOLIFmeshOp();
  case lgExportMesh2Abaqus:
      return new MCMESHTRANGUI_ExportMesh2AbaqusOp();
  case lgCompareDifference:
    return new MCMESHTRANGUI_CompareDifferenceOp();

  default:
      return 0;
  }
}

// Create custom selection object
LightApp_Selection* MCMESHTRANGUI::createSelection() const
{
  return new MCMESHTRANGUI_Selection();
}

// Module's deactivation
bool MCMESHTRANGUI::deactivateModule( SUIT_Study* theStudy )
{
    setMenuShown( false );
    setToolShown( false );

    return SalomeApp_Module::deactivateModule( theStudy );
}

// Module's activation
bool MCMESHTRANGUI::activateModule( SUIT_Study* theStudy )
{
    bool bOk = SalomeApp_Module::activateModule( theStudy );

    setMenuShown( true );
    setToolShown( true );

    return bOk;
}

// Study closed callback
void MCMESHTRANGUI::studyClosed( SUIT_Study* theStudy )
{
    //FOLLOWING IS COMMENT OUT BECAUSE IT CAUSE FATAL ERROR WHEN CLOSE THE STUDY
//    if ( theStudy ) {
//        MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = GetMCMESHTRANGen();
//        MCMESHTRAN_ORB::GroupList lst;
//        lst.length( 0 );
//        engine->setData( theStudy->id(), lst );
//    }

    SalomeApp_Module::studyClosed( theStudy );
}

//emit selection change signal for use
void    MCMESHTRANGUI::selectionChanged()
{
    emit selectChg();
}

void    MCMESHTRANGUI::OnUnitTest()
{
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = GetMCMESHTRANGen();
    engine->unittest();


}

/*!
 * \brief generate a new mesh
 */
void MCMESHTRANGUI::onProcess()
{
    if( sender() && sender()->inherits( "QAction" ) )
    {
      int id = actionId( ( QAction* )sender() );
      startOperation( id );
    }
}

void    MCMESHTRANGUI::onOperation()
{
    if( sender() && sender()->inherits( "QAction" ) )
    {
        int id = actionId( ( QAction* )sender() );
        //get selected objects
        QStringList aList ;
        selected(aList, true);

        MCMESHTRANGUI_DataModel * dm = dynamic_cast <MCMESHTRANGUI_DataModel *> (dataModel());
        if (!dm)
        {
            MESSAGE("get Data Model failed");
            return;
        }
        if (aList.size() == 0)
        {
            MESSAGE("Nothing selected!");
            return;
        }

        switch ( id )
        {
        case lgDeleteGroups :
        {
            //first erase then delete
            MCMESHTRANGUI_Displayer d;
            for ( QStringList::const_iterator it = aList.begin(), last = aList.end(); it != last; it++ )
            {
                MCMESHTRANGUI_DataObject * aGroupObj = dm->findObject(*it);
                MCMESHTRAN_ORB::MeshList * atmpMeshList = aGroupObj->getGroup()->getMeshList();
                const int n = atmpMeshList->length();
                for (int i=0; i<n; i++)
                {
                    QString atmpEntry = "MCMESHTRANGUI_"
                            + QString("%1").arg(aGroupObj->getGroupID())
                            + "_" + QString("%1").arg((int)(*atmpMeshList)[i]->getID());
                    d.Erase(atmpEntry.toLatin1(),/*force*/true, /*updateviewer=*/false );
                }
            }
            dm->deleteGroups(aList);
        } break;
        case lgRenameGroup :
        {
            bool bOk;
            QString aName = QInputDialog::getText( application()->desktop(),
                                                   tr( "Rename Group" ),
                                                   tr( "Group Name"),
                                                   QLineEdit::Normal,
                                                   "", &bOk);
            if (aName.trimmed().isEmpty() || bOk == false)
            {
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "Name invalid or empty!"));
                return;
            }
            dm->renameGroup(aList.at(0), aName);

        } break;
        case lgDeleteMeshes :
        {
            //first erase then delete
            MCMESHTRANGUI_Displayer d;
            for ( QStringList::const_iterator it = aList.begin(), last = aList.end(); it != last; it++ )
                d.Erase(it->toLatin1(),/*force*/true, /*updateviewer=*/false );
            dm->deleteMeshes(aList);
        } break;
        case lgRenameMesh :
        {
            bool bOk;
            QString aName = QInputDialog::getText( application()->desktop(),
                                                   tr( "Rename Mesh" ),
                                                   tr( "Mesh Name"),
                                                   QLineEdit::Normal,
                                                   "", &bOk);
            if (aName.trimmed().isEmpty() || bOk == false)
            {
                SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                          QString( "Name invalid or empty!"));
                return;
            }
            dm->renameMesh(aList.at(0), aName);
        } break;
        case lgCopyMeshes :
        {
            dm->copyMeshes(aList);
        } break;
        case lgCutMeshes :
        {
            //first erase then cut
            MCMESHTRANGUI_Displayer d;
            for ( QStringList::const_iterator it = aList.begin(), last = aList.end(); it != last; it++ )
                d.Erase(it->toLatin1(),/*force*/true, /*updateviewer=*/false );
            dm->cutMeshes(aList);
        } break;
        case lgPasteMeshes :
        {
            dm->pasteMeshes(aList.at(0));
        } break;
        default: MESSAGE ("Action not found!"); break;

        }
        updateObjBrowser();
    }
}

void   MCMESHTRANGUI:: onImportMesh()
{
    if( sender() && sender()->inherits( "QAction" ) )
    {
        MCMESHTRANGUI_DataModel * dm = dynamic_cast <MCMESHTRANGUI_DataModel *> (dataModel());
        if (!dm)
        {
            MESSAGE("get Data Model failed");
            return;
        }

        //get selected group
        QStringList aList ;
        selected(aList, true);
        int aGroupID;
        if (aList.size() > 0)
            aGroupID = dm->getGroupID(aList.at(0));
        else
            aGroupID = -1;

        int id = actionId( ( QAction* )sender() );
        QString fileName = QFileDialog::getOpenFileName(getApp()->desktop(),
                                                        tr("Open MED File"),
                                                        LastDir,
                                                        tr("CGNS (*.cgns);;UNV (*.unv);;SAUV (*.sauv);;STL(*.stl);;All(*.*)"));
        if (fileName.trimmed().isEmpty() || SUIT_Tools::file(fileName).trimmed().isEmpty())
        {
            SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                      QString( "File directory invalid or name empty!"));
            return;
        }
        LastDir = SUIT_Tools::dir(fileName);  //save the dir



        //switch actions
        switch ( id )
        {
#ifdef WITH_CGNS
        case lgImportCGNSMesh :
        {
            if (SUIT_Tools::extension(fileName).toLower() != "cgns")
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "The extension is not *.cgns, might be a mistake. "));
            if (!dm->importCGNS(fileName, aGroupID))
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "Import CGNS mesh failed! "));
        } break;
#endif
        case lgImportUNVMesh :
        {
            if (SUIT_Tools::extension(fileName).toLower() != "unv")
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "The extension is not *.unv, might be a mistake. "));
            if (!dm->importUNVMeshOnly(fileName, aGroupID))
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "Import UNV mesh failed! "));
        } break;
        case lgImportSAUVMesh :
        {
            if (SUIT_Tools::extension(fileName).toLower() != "sauv")
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "The extension is not *.sauv, might be a mistake. "));
            if (!dm->importSAUVMeshOnly(fileName, aGroupID))
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "Import sauv mesh failed! "));
        } break;
        case lgImportSTLMesh :
        {
            if (SUIT_Tools::extension(fileName).toLower() != "stl")
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "The extension is not *.stl, might be a mistake. "));
            if (!dm->importSTLMeshOnly(fileName, aGroupID))
                SUIT_MessageBox::warning( getApp()->desktop(), QString("Warning"),
                                          QString( "Import STL mesh failed! "));
        } break;
        default: MESSAGE ("Function not available!"); break;
        }
//        updateObjBrowser(true);
        updateObjBrowser();
    }

}


//find http://www.salome-platform.org/forum/forum_12/842743364 for detail introduction
//this function is changed because it cause trouble loading ParaVIS libraries preceed ParaVIS loading them by itself.
/*void    MCMESHTRANGUI::onDisplayPVIS()
{
    //get A selected objects
    QStringList aList ;
    selected(aList, false);
    if (aList.size() == 0)
    {
        MESSAGE("No object selected");
        return;
    }
    MCMESHTRANGUI_DataModel * dm = dynamic_cast <MCMESHTRANGUI_DataModel *> (dataModel());
    if (!dm)
    {
        MESSAGE("get Data Model failed");
        return;
    }

    MCMESHTRANGUI_DataObject * aObj = dm->findObject(aList.at(0));
    if (aObj && aObj->isMesh())
    {
        Engines::EngineComponent_var comp = getApp()->lcc()->FindOrLoad_Component("FactoryServer", "PARAVIS" );
        PARAVIS::PARAVIS_Gen_var  aPVGen = PARAVIS::PARAVIS_Gen::_narrow(comp);
        if (!aPVGen->_is_nil())
        {
            MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
            std::ostringstream aScript;
            char *IOR;
            if (strcmp(aMesh->getType(), "EmptyMesh") == 0)
            {
                SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_var aFieldTemplate =
                        aMesh->getFieldTemplate();
                if (!aFieldTemplate->_is_nil())
                    IOR = getApp()->orb()->object_to_string(aFieldTemplate);
            }
            else
            {
                SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var aField =
                        aMesh->getField();
                if (!aField->_is_nil())
                    IOR =getApp()->orb()->object_to_string(aField);
            }
            if (*IOR == 0)
            {
                MESSAGE("Emtpy IOR!")
                return;
            }
            aScript << "src1 = ParaMEDCorbaPluginSource()\nsrc1.IORCorba = '" << IOR
                    << "'\nasc=GetAnimationScene()\nrw=GetRenderView()\ndr=Show()\ndr.Visibility = 1\nRender()";
            CORBA::string_free(IOR);
            aPVGen->ExecuteScript(aScript.str().c_str());
            return;
        }
        MESSAGE("Please open ParaVIS before this operation!");
        return;
    }
    MESSAGE("Cannot find object, or object is not a mesh!");
}
*/

//from MED_SRC/src/MEDCalculator/MEDCalculatorDBField.cxx
void    MCMESHTRANGUI::onDisplayPVIS()
{
	cout << "Sending data to ParaView..."<<endl; //for testing. 
    //get A selected objects
    QStringList aList ;
//    selected(aList, false);
    selected(aList, true);
    if (aList.size() == 0)
    {
        MESSAGE("No object selected");
        return;
    }
    MCMESHTRANGUI_DataModel * dm = dynamic_cast <MCMESHTRANGUI_DataModel *> (dataModel());
    if (!dm)
    {
        MESSAGE("get Data Model failed");
        return;
    }

    for (int j=0; j<aList.size(); j++)
    {
        MCMESHTRANGUI_DataObject * aObj = dm->findObject(aList.at(j));
        if (aObj && aObj->isMesh())
        {
				cout << "Sending data to ParaView..."<<j<<endl; //for testing. 
            //Get paraVIS_Gen
            int argc=0;
            CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
            CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
            PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
            PortableServer::POAManager_var mgr=poa->the_POAManager();
            mgr->activate();
            SALOME_NamingService ns(orb);
            ns.Change_Directory("/Containers");
            //search for the ParaVIS Gen
            vector <string> aPathList = ns.list_directory_recurs();
            std::ostringstream path;
            for (int i=0; i<aPathList.size(); i++){
                string atmpStr = aPathList [i];
                if (atmpStr.find("PARAVIS") >= 0 && atmpStr.find("PARAVIS") <atmpStr.size()){
                    path << aPathList[i];
                    break;
                }
            }
            MESSAGE("path for PARAVIS: "<< path.str().c_str());
            CORBA::Object_var paravis=ns.Resolve(path.str().c_str());
            if (CORBA::is_nil(paravis))        {
                MESSAGE("Please open ParaVIS before this operation!");
                return;
            }
            CORBA::Request_var req=paravis->_request("ExecuteScript");
            CORBA::NVList_ptr args=req->arguments();
            CORBA::Any ob;
            std::ostringstream script;
            MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
            char *IOR;
            if (strcmp(aMesh->getType(), "EmptyMesh") == 0) {
                SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_var aFieldTemplate =
                        aMesh->getFieldTemplate();
                if (!aFieldTemplate->_is_nil())
                    IOR = orb->object_to_string(aFieldTemplate->getMesh());//only mesh can be export to the Plugin
            }
            else {
                SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var aField =
                        aMesh->getField();
                if (!aField->_is_nil())
                    IOR =orb->object_to_string(aField);
    //            aField->UnRegister();  //error to unregister!!!
            }
            if (*IOR == 0){
                MESSAGE("Emtpy IOR!");
//                return;
                continue;
            }
            script << "src1 = ParaMEDCorbaPluginSource(guiName = \""<<aObj->name().toStdString()<<"\")\nsrc1.IORCorba = '" << IOR << "'\nasc=GetAnimationScene()\nrw=GetRenderView()\ndr=Show()\ndr.Visibility = 1\n";
//            script << "src1 = ParaMEDCorbaPluginSource(guiName=\""<<aObj->name().toStdString()<<"\")\nsrc1.IORCorba=\""<< IOR << "\"\nasc=GetAnimationScene()\nrw=GetRenderView()\ndr=Show()\ndr.Visibility = 1\n";
//			cout << script.str().c_str() <<endl; // for testing
            CORBA::string_free(IOR);
            ob <<= script.str().c_str();
            args->add_value("script",ob,CORBA::ARG_IN);
            req->set_return_type(CORBA::_tc_void);
            req->invoke();
//            return;
        }
        else {
            MESSAGE("Cannot find object, or object is not a mesh!");

        }
    }
}

//copy from  SMESH_NumberFilter::getGeom
GEOM::GEOM_Object_ptr getGeom
  (const SUIT_DataOwner* theDataOwner, const bool extractReference )
{
  const LightApp_DataOwner* owner =
    dynamic_cast<const LightApp_DataOwner*>(theDataOwner);
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>
    (SUIT_Session::session()->activeApplication()->activeStudy());

  GEOM::GEOM_Object_var anObj;

  if (!owner || !appStudy)
    return GEOM::GEOM_Object::_nil();

  _PTR(Study) study = appStudy->studyDS();
  QString entry = owner->entry();

  _PTR(SObject) aSO( study->FindObjectID( entry.toLatin1().data() ) ), aRefSO;
  if( extractReference && aSO && aSO->ReferencedObject( aRefSO ) )
    aSO = aRefSO;

  if (!aSO)
    return GEOM::GEOM_Object::_nil();

  CORBA::Object_var anObject = _CAST(SObject,aSO)->GetObject();
  anObj = GEOM::GEOM_Object::_narrow(anObject);
  if (!CORBA::is_nil(anObj))
    return anObj._retn();

  return GEOM::GEOM_Object::_nil();
}

//display GEOM object in PVIS
//A OCC2VTK Plugin is programed for this application
void    MCMESHTRANGUI::onDisplayGEOMObjInPVIS()
{
    //get a selected objects
    LightApp_Application* app = getApp();
    LightApp_SelectionMgr* SltMgr = app ? app->selectionMgr() : 0;
    if( !SltMgr )
        return;
    SUIT_DataOwnerPtrList anOwnersList;
    SltMgr->selected( anOwnersList );
    if (anOwnersList.size() == 0 || anOwnersList.size() > 1)
    {
        MESSAGE("No or more than one object selected!");
        return;
    }

    const LightApp_DataOwner* owner = dynamic_cast<const LightApp_DataOwner*>( anOwnersList[ 0 ].get() );
    // Get geom object from IO
    GEOM::GEOM_Object_var aGeomObj = getGeom(owner, true);
    if (aGeomObj->_is_nil())
    {
        MESSAGE("the GEOM object is nil!");
        return;
    }

    //Get paraVIS_Gen
    int argc=0;
    CORBA::ORB_var orb=CORBA::ORB_init(argc,0);
    CORBA::Object_var obj=orb->resolve_initial_references("RootPOA");
    PortableServer::POA_var poa=PortableServer::POA::_narrow(obj);
    PortableServer::POAManager_var mgr=poa->the_POAManager();
    mgr->activate();
    SALOME_NamingService ns(orb);
    ns.Change_Directory("/Containers");
    //search for the ParaVIS Gen
    vector <string> aPathList = ns.list_directory_recurs();
    std::ostringstream path;
    for (int i=0; i<aPathList.size(); i++){
        string atmpStr = aPathList [i];
        if (atmpStr.find("PARAVIS") >= 0 && atmpStr.find("PARAVIS") <atmpStr.size()){
            path << aPathList[i];
            break;
        }
    }
    MESSAGE("path for PARAVIS: "<< path.str().c_str());
    CORBA::Object_var paravis=ns.Resolve(path.str().c_str());
    if (CORBA::is_nil(paravis))        {
        MESSAGE("Please open ParaVIS before this operation!");
        return;
    }
    CORBA::Request_var req=paravis->_request("ExecuteScript");
    CORBA::NVList_ptr args=req->arguments();
    CORBA::Any ob;
    std::ostringstream script;

    char *IOR;
    IOR = orb->object_to_string(aGeomObj);
    if (*IOR == 0){
        MESSAGE("Emtpy IOR!")
        return;
    }
//    cout <<IOR<<endl;
    script << "src1 = OCC2VTKCorbaPluginSource()\nsrc1.IORCorba = '" << IOR << "'\nasc=GetAnimationScene()\nrw=GetRenderView()\ndr=Show()\ndr.Visibility = 1\n";
//    script << "src1 = ParaMEDCorbaPluginSource()\nsrc1.IORCorba = '" << IOR << "'\nasc=GetAnimationScene()\nrw=GetRenderView()\ndr=Show()\ndr.Visibility = 1\n";

    CORBA::string_free(IOR);
    ob <<= script.str().c_str();
    args->add_value("script",ob,CORBA::ARG_IN);
    req->set_return_type(CORBA::_tc_void);
    req->invoke();
    cout <<"Invoked."<<endl;

    return;
}

void    MCMESHTRANGUI::onSend2SMESH()
{
    QStringList entries;
    selected ( entries, true );
    MCMESHTRANGUI_DataModel * dm = dynamic_cast <MCMESHTRANGUI_DataModel *> (dataModel());
    if (!dm->sendMesh2SMESH(entries))
        SUIT_MessageBox::warning( application()->desktop(), QString("Warning"),
                                  QString( "Failed to Send mesh!"));
}


void MCMESHTRANGUI::OnDisplayerCommand()
{
  const QObject* obj = sender();
  if ( obj && obj->inherits( "QAction" ) ) {
    const int id = actionId ( (QAction*)obj );
    switch ( id ) {
    case lgDisplay : {
      QStringList entries;
      selected ( entries, true );
      MCMESHTRANGUI_Displayer d;
      for ( QStringList::const_iterator it = entries.begin(), last = entries.end(); it != last; it++ )
        d.Display( it->toLatin1(), /*updateviewer=*/false, 0 );
      d.UpdateViewer();
    } break;
    case lgErase   : {
      QStringList entries;
      selected ( entries, true );
      MCMESHTRANGUI_Displayer d;
      for ( QStringList::const_iterator it = entries.begin(), last = entries.end(); it != last; it++ )
        d.Erase( *it, /*forced=*/true, /*updateViewer=*/false, 0 );
      d.UpdateViewer();
    } break;
    case lgShading   : {
      QStringList entries;
      selected ( entries, true );
      MCMESHTRANGUI_Displayer().setDisplayMode( entries, "Surface" );
    } break;
    case lgWireframe   : {
      QStringList entries;
      selected ( entries, true );
      MCMESHTRANGUI_Displayer().setDisplayMode( entries, "Wireframe" );
    } break;
    case lgPointsMode   : {
      QStringList entries;
      selected ( entries, true );
      MCMESHTRANGUI_Displayer().setDisplayMode( entries, "Points" );
    } break;
    case lgShadingWithWireframe   : {
      QStringList entries;
      selected ( entries, true );
      MCMESHTRANGUI_Displayer().setDisplayMode( entries, "SurfaceAndEdge" );
    } break;
    case lgTransparency   : {
      QStringList entries;
      selected ( entries, true );
      MCMESHTRANGUI_TransparencyDlg( getApp()->desktop(), entries ).exec();
    } break;
    default: printf( "ERROR: Action with ID = %d was not found in MCMESHTRANGUI\n", id ); break;
    }
  }
}

void MCMESHTRANGUI::OnExperiment()
{
    MCMESHTRANGUI_DataModel * dm = dynamic_cast <MCMESHTRANGUI_DataModel *> (dataModel());
//    dm->buildTree();
//    updateObjBrowser();
    QStringList entries;
    selected ( entries, true );
    double Vol = 0.;
    for (int i=0; i<entries.size(); i++) {
        MCMESHTRANGUI_DataObject * aObj = dm->findObject(entries[i]);
        if (aObj->isMesh()) {
            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldTemplate> aFieldTemplate_ptr
                    =ParaMEDMEM::MEDCouplingFieldTemplateClient::New(aObj->getMesh()->getFieldTemplate());
            //2013-05-29
            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingUMesh> aUMesh_ptr
                    = static_cast <ParaMEDMEM::MEDCouplingUMesh *> (const_cast <ParaMEDMEM::MEDCouplingMesh *> (aFieldTemplate_ptr->getMesh()));
            ParaMEDMEM::MEDCouplingFieldDouble * aVolumeField = aUMesh_ptr->getMeasureField(true);
            ParaMEDMEM::DataArrayDouble * aArray = aVolumeField->getArray();
            vector <double > aTuple;
            aTuple.resize(1);
            for (int j=0; j< aArray->getNumberOfTuples(); j++)
            {
                aArray->getTuple(j, aTuple.data());
                Vol += aTuple[0];
            }
        }
    }
    cout <<"Volume = " <<Vol <<endl;
}

// Export the module
extern "C" {
Standard_EXPORT CAM_Module* createModule()
{
    return new MCMESHTRANGUI();
}
}
