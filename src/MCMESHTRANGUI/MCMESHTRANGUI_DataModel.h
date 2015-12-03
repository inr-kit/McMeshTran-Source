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

#if !defined(MCMESHTRANGUI_DATAMODEL_H)
#define MCMESHTRANGUI_DATAMODEL_H
#include "MCMESHTRANGUI.h"
#include "MCMESHTRANGUI_DataObject.h"
#include <LightApp_DataModel.h>
#include <SUITApp_Application.h>
#include <SUIT_Desktop.h>
#include <CAM_Module.h>
#include <CAM_Application.h>
#include <vector>

using namespace std;

class CAM_Module;

#ifdef WITH_CGNS
#   include <cgnslib.h>
#   if CGNS_VERSION < 3100
#       define cgsize_t int
#   else
#       if CG_BUILD_SCOPE
#           error enumeration scoping needs to be off
#       endif
#   endif
#endif

/*!
 * Class       : MCMESHTRANGUI_DataModel
 * Description : Data Model of ATOMIC component
 */



class MCMESHTRANGUI_DataModel : public LightApp_DataModel
{
public:
  MCMESHTRANGUI_DataModel ( CAM_Module* );
  virtual ~MCMESHTRANGUI_DataModel();

  virtual bool          open(const QString&url, CAM_Study*study, QStringList listOfFiles);
  virtual bool          save(QStringList& theListOfFiles);
  virtual bool          saveAs(const QString&url, CAM_Study*study, QStringList& theListOfFiles);
  virtual bool          close();
  virtual bool          create(CAM_Study* study);
  virtual bool          isModified() const;
  virtual bool          isSaved() const;
  virtual void          update( LightApp_DataObject* = 0, LightApp_Study* = 0 );

  bool                  generateGroup(const QString& aGroupName);
  int                   getStudyID();
  static int            getGroupID(const QString& aEntry );
  static int            getMeshID (const QString& aEntry );
  MCMESHTRANGUI_DataObject * findObject(const QString& aEntry);
  SUIT_Desktop*         desktop() { return module()->application()->desktop(); }
//  SUIT_Study*           study()   {return module()->application()->activeStudy();}
  MCMESHTRANGUI *       getMCMESHTRANGUImodule() { return dynamic_cast< MCMESHTRANGUI*> ( module() );}
  int                   getPasteListSize() { return CopyList->length() + CutList->length() ;}
//  QString               getTmpDir();

  bool                  readMCNPFmesh(const QString& InputFileName, const QString& MeshtalFileName);
  bool                  readMCNPUmesh(const QString& UMeshOutFileName, const bool & isKeepInstance, const bool & isRemoveMultiplier);
  bool                  readTRIPOLIFmesh(const QString& TRIPOLIFileName, const bool &isVolumAverage);
  bool                  sumMeshes(const QString& aEntry,  const QString& bEntry, const QString& Name, const int GroupID  );
  bool                  averageMeshes(const QString& aEntry,  const QString& bEntry, const QString& Name, const int GroupID  );
  bool                  translateMesh(const QString& aEntry, const double * vector , const QString& Name, const int GroupID  );
  bool                  rotateMesh (const QString& aEntry, const double * center , const double * vector,
                                    const double AngleInDegree, const QString& Name , const int GroupID );
  bool                  scaleMesh (const QString& aEntry, const double Factor, const QString& Name , const int GroupID  );
  bool                  multiplyFactor(const QString& aEntry, const double Factor, const QString& Name , const int GroupID  );
  bool                  interpolateMesh(const QString& aEntry,  const QString& bEntry, const QString& Name, const int GroupID, int Option = 1 );
  bool                  export2VTK (const QString& aEntry, const QString& FileName);
  bool                  export2MED (const QString& aEntry, const QString& FileName);
  bool                  export2CSV (const QString& aEntry, const QString& FileName);
  bool                  export2FLUENT (const QString& aEntry, const QString& FileName);
  bool                  export2CFX (const QString& aEntry, const QString& FileName);
  bool                  exportMesh2Abaqus (const QStringList& aList, const QString& FileName);

  bool                  importMED  (const QString& FileName,const int GroupID = -1 );
#ifdef WITH_CGNS
  bool                  export2CGNS (const QString aEntry, const QString FileName);
  bool                  importCGNS (const QString& FileName, const int GroupID = -1);
#endif
  bool                  importAbaqus (const QString& FileName, const int GroupID = -1);
  bool                  importUNVMeshOnly (const QString& FileName,const int GroupID = -1 );
  bool                  importSAUVMeshOnly (const QString& FileName,const int GroupID = -1 );
  bool                  importSTLMeshOnly (const QString& FileName,const int GroupID = -1 );

  bool                  sendMesh2SMESH(const QStringList& aList);
  bool                  compareDifference(const QString& aEntry,  const QString& bEntry, const QString& Name, const int GroupID  );


  void                  deleteGroup(const QString& aEntry);
  void                  deleteGroups(const QStringList& aList);
  void                  renameGroup(const QString& aEntry, const QString& newName);
  void                  deleteMesh (const QString& aEntry);
  void                  deleteMeshes (const QStringList& aList);
  void                  renameMesh(const QString& aEntry, const QString& newName);
  void                  copyMesh(const QString& aEntry);
  void                  copyMeshes(const QStringList& aList);
  void                  cutMesh(const QString& aEntry);
  void                  cutMeshes(const QStringList& aList);
  void                  pasteMeshes(const QString& aEntry);

public:
  virtual void          build();
  void          buildTree();

private:
   MCMESHTRAN_ORB::MeshList_var CopyList ;
   MCMESHTRAN_ORB::MeshList_var CutList ;
   QString myStudyURL;  //store the path of the study
   //defined for test purpose
   int count;


};

#endif // MCMESHTRANGUI_DATAMODEL_H
