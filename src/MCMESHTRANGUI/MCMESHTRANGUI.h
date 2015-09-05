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

#ifndef _MCMESHTRANGUI_H_
#define _MCMESHTRANGUI_H_

#include <SalomeApp_Module.h>
#include <SALOMEconfig.h>
#include <SALOME_LifeCycleCORBA.hxx>
#include CORBA_CLIENT_HEADER(MCMESHTRAN)
#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServant)
// #include CORBA_CLIENT_HEADER(PARAVIS_Gen)
#include CORBA_CLIENT_HEADER(SMESH_Gen)


class SalomeApp_Application;

//static Engines::EngineComponent_var mySMESHGen;
//static Engines::EngineComponent_var myPVGen;

class MCMESHTRANGUI: public SalomeApp_Module
{
  Q_OBJECT

public:
  MCMESHTRANGUI();

  void    initialize( CAM_Application* );
  void    windows( QMap<int, int>& ) const;
  void    viewManagers( QStringList& ) const;

  void    selected( QStringList&, const bool multiple);

  virtual QString engineIOR() const;

  static void InitMCMESHTRANGen( SalomeApp_Application* );
  static MCMESHTRAN_ORB::MCMESHTRAN_Gen_var GetMCMESHTRANGen();
//  static Engines::EngineComponent_var GetSMESHGen();
//  static Engines::EngineComponent_var GetPVGen();

  virtual LightApp_Displayer* displayer();
  virtual void updateObjBrowser(bool isUpdateDataModel= true, SUIT_DataObject* = 0 );

protected:
  enum { unittest               = 901,
              lgReadMCNPFmesh        = 902,
              lgSumMeshes            = 903,
              lgAverageMeshes        = 904,
              lgTranslateMesh        = 905,
              lgRotateMesh           = 906,
              lgScaleMesh            = 907,
              lgMultiplyFactor       = 908,
              lgInterpolateMesh      = 909,
              lgExport2VTK           = 910,
              lgExport2MED           = 911,
              lgExport2CSV           = 912,
              lgImportMED            = 914,
   #ifdef WITH_CGNS
              lgExport2CGNS          = 913,
              lgImportCGNS           = 915,
              lgImportCGNSMesh       = 916,
   #endif
              lgImportUNVMesh        = 917,
              lgImportSAUVMesh       = 918,
              lgImportSTLMesh        = 919,
              lgImportGMFMesh        = 920,
              lgExport2FLUENT        = 921,
              lgExport2CFX           = 922,
              lgImportAbaqus         = 923,
              lgReadMCNPUmesh        = 924,
              lgExportMesh2Abaqus    = 925,
              lgSend2SMESH           = 926,
              lgReadTRIPOLIFmesh        = 927,


              lgCreateGroup             = 1001,
              lgDeleteGroups            = 1002,
              lgDeleteMeshes            = 1003,
              lgRenameGroup             = 1004,
              lgRenameMesh              = 1005,
              lgCopyMeshes              = 1006,
              lgCutMeshes               =1007,
              lgPasteMeshes             =1008,

              lgDisplayInPV             =1101,
              lgDisplay                 =1102,
              lgErase                   =1103,
              lgShading                 =1104,
              lgWireframe               =1105,
              lgPointsMode              =1106,
              lgTransparency            =1107,
              lgDisplayGEOMObjInPV      =1108,
              lgShadingWithWireframe    =1109,

              lgExperiment              =1999

       };

  virtual CAM_DataModel*      createDataModel();
  virtual LightApp_Operation* createOperation( const int ) const;
  virtual LightApp_Selection* createSelection() const;

public slots:
  virtual bool    deactivateModule( SUIT_Study* );
  virtual bool    activateModule( SUIT_Study* );
  virtual void    studyClosed( SUIT_Study* );
  virtual void    selectionChanged();

signals:
  void            selectChg();

protected slots:
  void    OnUnitTest();
  void    onProcess();
  void    onOperation();
  void    onImportMesh();
  void    onDisplayPVIS();
  void    onDisplayGEOMObjInPVIS();
  void    onSend2SMESH();

  void    OnDisplayerCommand();
  void    OnExperiment();


private:
  static MCMESHTRAN_ORB::MCMESHTRAN_Gen_var myEngine;
  QString LastDir;
};

#endif
