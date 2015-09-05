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
#ifndef _MCMESHTRAN_HXX_
#define _MCMESHTRAN_HXX_

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(MCMESHTRAN)
#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServant)
//#include CORBA_CLIENT_HEADER(SMESH_Gen)
#include "SALOME_Component_i.hxx"

#include "MCMESHTRAN_MCNPFmesh.hxx"
#include <map>

#ifdef WIN32
#  ifndef Standard_EXPORT
#    define Standard_EXPORT __declspec( dllexport )
#  endif
#else
#  define Standard_EXPORT
#endif

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

class Mesh;
class MeshGroup;

//############### Engine ####################

class Standard_EXPORT MCMESHTRAN: public virtual POA_MCMESHTRAN_ORB::MCMESHTRAN_Gen,
        public Engines_Component_i
{

public:
    MCMESHTRAN(CORBA::ORB_ptr orb,
               PortableServer::POA_ptr poa,
               PortableServer::ObjectId * contId,
               const char *instanceName,
               const char *interfaceName);
    MCMESHTRAN();
    virtual ~MCMESHTRAN();

    CORBA::Long genId ();

    CORBA::Boolean                  setData(CORBA::Long studyID, const MCMESHTRAN_ORB::GroupList& theData) ;
    MCMESHTRAN_ORB::GroupList*      getData(CORBA::Long studyID) ;
    MCMESHTRAN_ORB::MeshGroup_ptr   generateGroup(const char* Name)                                     throw (SALOME::SALOME_Exception) ;
    MCMESHTRAN_ORB::Mesh_ptr        generateMesh(const char* Name, const char* Type, CORBA::Double nps = 0)     throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::MeshGroup_ptr   getGroup(CORBA::Long studyID, CORBA::Long GroupID) ;
    MCMESHTRAN_ORB::Mesh_ptr        getMesh(CORBA::Long studyID, CORBA::Long GroupID, CORBA::Long MeshID) ;
    MCMESHTRAN_ORB::Mesh_ptr        deepCopyMesh (MCMESHTRAN_ORB::Mesh_ptr aMesh)                       throw (SALOME::SALOME_Exception);
    CORBA::Boolean                  appendGroup(CORBA::Long studyID,
                                                MCMESHTRAN_ORB::MeshGroup_ptr aGroup) ;//                  throw (SALOME::SALOME_Exception);
    CORBA::Boolean                  insertGroup(CORBA::Long studyID,
                                                MCMESHTRAN_ORB::MeshGroup_ptr aGroup, CORBA::Long GroupID);//      throw (SALOME::SALOME_Exception);
    CORBA::Boolean                  deleteGroup(CORBA::Long studyID,
                                                CORBA::Long GroupID);//                                    throw (SALOME::SALOME_Exception) ;
    CORBA::Boolean                  clearGroup(CORBA::Long studyID) ;

    MCMESHTRAN_ORB::MeshGroup_ptr   readMCNPFmesh(const char* InputFileName, const char* MeshtalFileName) throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::MeshGroup_ptr   readMCNPUmesh(const char* UMeshOutFileName, CORBA::Boolean isKeepInstance, CORBA::Boolean isRemoveMultiplier) throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::MeshGroup_ptr   readTRIPOLIFmesh(const char* TRIPOLIFileName , CORBA::Boolean isVolAvg) throw (SALOME::SALOME_Exception);
    CORBA::Boolean                  checkMeshConsistency(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                                         MCMESHTRAN_ORB::Mesh_ptr bMesh) throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::Mesh_ptr        sumMeshes(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                              MCMESHTRAN_ORB::Mesh_ptr bMesh, const char* Name)         throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::Mesh_ptr        averageMeshes(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                                  MCMESHTRAN_ORB::Mesh_ptr bMesh, const char* Name)     throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::Mesh_ptr        translateMesh(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                                  const MCMESHTRAN_ORB::FixArray3 vector, const char* Name)     throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::Mesh_ptr        rotateMesh(MCMESHTRAN_ORB::Mesh_ptr aMesh, const
                                               MCMESHTRAN_ORB::FixArray3 center,
                                               const MCMESHTRAN_ORB::FixArray3 vector,
                                               CORBA::Double AngleInRadian, const char* Name)           throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::Mesh_ptr        scaleMesh(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                              CORBA::Double Factor, const char* Name)                   throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::Mesh_ptr        multiplyFactor(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                                   CORBA::Double Factor, const char* Name)              throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::Mesh_ptr        interpolateMEDMesh(MCMESHTRAN_ORB::Mesh_ptr source,
                                                    MCMESHTRAN_ORB::Mesh_ptr target, const char* Name,
                                                       MCMESHTRAN_ORB::SolutionLoc solloc)              throw (SALOME::SALOME_Exception);
    void                            export2VTK(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName)    throw (SALOME::SALOME_Exception);
    void                            export2MED(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName)    throw (SALOME::SALOME_Exception);

#ifdef WITH_CGNS
    MCMESHTRAN_ORB::Mesh_ptr        interpolateCGNSMesh(MCMESHTRAN_ORB::Mesh_ptr source,
                                                    MCMESHTRAN_ORB::Mesh_ptr target, const char* Name,
                                                       MCMESHTRAN_ORB::SolutionLoc solloc)              throw (SALOME::SALOME_Exception);
    void                            export2CGNS(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName)   throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::MeshGroup_ptr   importCGNS(const char* CGNSFileName)                                throw (SALOME::SALOME_Exception);
#endif

    void                            export2CSV(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName)    throw (SALOME::SALOME_Exception);
    void                            export2FLUENT(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName) throw (SALOME::SALOME_Exception);
    void                            export2CFX(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName)    throw (SALOME::SALOME_Exception);

    MCMESHTRAN_ORB::MeshGroup_ptr   importMED(const char* MEDFileName)                                  throw (SALOME::SALOME_Exception);
    MCMESHTRAN_ORB::MeshGroup_ptr   importAbaqus(const char* AbaqusFileName)                            throw (SALOME::SALOME_Exception);
    void                            exportMesh2Abaqus(const MCMESHTRAN_ORB::MeshList& aMeshList,const char* FileName ) throw (SALOME::SALOME_Exception);
    void                            unittest();

public:
    MCMESHTRAN_ORB::Mesh_ptr        cnvMCNPFmesh2Mesh(MCNPFmesh &meshtal)                               throw (SALOME::SALOME_Exception);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr cnvMCNPFmesh2MEDCoupling (MCNPFmesh &meshtal)  throw (SALOME::SALOME_Exception);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr interpolateWithRemapper (
            SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr srcField,
//            SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr tgtField,
            SALOME_MED::MEDCouplingMeshCorbaInterface_ptr tgtMesh,
            const char * FieldName, MCMESHTRAN_ORB::SolutionLoc solloc)                              throw (SALOME::SALOME_Exception);
//    SMESH::SMESH_Gen_ptr getSMESHEngine();



private:
    std::map<long, MCMESHTRAN_ORB::GroupList*> myData;
    CORBA::Long myMaxId;
};

//############### MeshGroup ####################
class Standard_EXPORT MeshGroup: public POA_MCMESHTRAN_ORB::MeshGroup
{
public:

    MeshGroup(const char * Name,  const CORBA::Long id);
    virtual         ~MeshGroup();

    char*           getName() ;
    void            setName(const char* Name) ;
    CORBA::Long     getID()              {return myID; }
    MCMESHTRAN_ORB::MeshList*  getMeshList()    {return myList; }
    CORBA::Boolean  setMeshList(const MCMESHTRAN_ORB::MeshList& aMeshList) ;
    MCMESHTRAN_ORB::Mesh_ptr        getMesh(CORBA::Long MeshID) ;
    CORBA::Boolean  appendMesh(MCMESHTRAN_ORB::Mesh_ptr aMesh) ;
    CORBA::Boolean  deleteMesh(CORBA::Long MeshID) ;
    CORBA::Boolean  insertMesh(MCMESHTRAN_ORB::Mesh_ptr aMesh, CORBA::Long MeshID) ;
    CORBA::Boolean  clearMeshes() ;
    void            appendMeshInGroup(MCMESHTRAN_ORB::MeshGroup_ptr aGroup);

private:

    CORBA::Long  myID;
    char * myName;
    MCMESHTRAN_ORB::MeshList * myList; //MeshList is a CORBA sequence of Mesh type

};

//############### Mesh ####################

class Standard_EXPORT Mesh:public POA_MCMESHTRAN_ORB::Mesh
{

public:

    Mesh (const char * Name,const CORBA::Long id, const char * Type, CORBA::Double nps = 0);
    virtual         ~Mesh();

    char*           getName()   {return CORBA::string_dup(myName);}
    void            setName(const char* Name) ;
    CORBA::Long     getID()     {return myID ; }
    char*           getType()   {return CORBA::string_dup(myType); }
    CORBA::Double   getNPS()    {return myNPS; }
    void            setField(SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr aField) ;
    void            setField(SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr aField) ;
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr getField() ;
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr getFieldTemplate() ;
private:

    int             myID; // an unique integer for identification, assign when mesh instance is generated
    char *          myName;  //Name for displaying in the GUI
    char *          myType; //MCNPFmesh, MCNPUmesh, TRIPOLIFmesh, ExtMesh, IntMesh,
    // a reference of a MED Mesh and field
    // here use the _ptr type, because the _ptr should be return by getField which has also an _ptr type
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myField;
    SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr myFieldTemplate;  //for empty mesh

    CORBA::Double   myNPS;  //optional parameter, neutron history
};

extern "C"
Standard_EXPORT PortableServer::ObjectId * MCMESHTRANEngine_factory(CORBA::ORB_ptr orb,
                                                    PortableServer::POA_ptr poa,
                                                    PortableServer::ObjectId * contId,
                                                    const char *instanceName,
                                                    const char *interfaceName);

#endif
