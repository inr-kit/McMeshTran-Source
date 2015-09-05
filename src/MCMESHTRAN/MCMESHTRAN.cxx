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

#include "MCMESHTRAN.hxx"
//#include "TestMCMESHTRAN.hxx"

//MEDCoupling
#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingFieldTemplateClient.hxx"
#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingMeshServant.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"
#include "MEDCouplingFieldTemplateServant.hxx"
#include "MEDCouplingAutoRefCountObjectPtr.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingRemapper.hxx"
#include "MEDLoader.hxx"
#include "MEDFileMesh.hxx"
#include "MEDFileField.hxx"

//Reader header
#include "MCMESHTRAN_MCNPFmeshReader.hxx"
#include "MCMESHTRAN_AbaqusMeshReader.hxx"
#include "MCMESHTRAN_AbaqusMeshWriter.hxx"
#include "MCMESHTRAN_MCNPUMeshReader.hxx"
#include "MCMESHTRAN_TRIPOLIFmeshReader.hxx"
//SALOME
#include "Utils_CorbaException.hxx"
#include "utilities.h"



#include <stdio.h>
#include <stdlib.h>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <QStringList>
#include <math.h>
#include <string>
#include <vector>
#include <iomanip>
#include <QFileInfo>

using namespace std;

////convert int to char *
//char* itostr( int i)
//{
//    char * str = new char [10];
//    sprintf(str, "%d", i);
//    return str;
//}


MCMESHTRAN::MCMESHTRAN(CORBA::ORB_ptr orb,
                       PortableServer::POA_ptr poa,
                       PortableServer::ObjectId * contId,
                       const char *instanceName,
                       const char *interfaceName) :
    Engines_Component_i(orb, poa, contId, instanceName, interfaceName)
{
    MESSAGE("activate object");
    _thisObj = this ;
    _id = _poa->activate_object(_thisObj);
    myMaxId = 0;
}

//MCMESHTRAN::MCMESHTRAN()
//{
//    //for unit test, no meaning
//}

MCMESHTRAN::~MCMESHTRAN()
{
    //delete all the pointers. neccessary?
    for (std::map<long, MCMESHTRAN_ORB::GroupList*>::iterator it=myData.begin(); it!=myData.end(); ++it)
        delete it->second;
}

/*!
 * \brief generate a new id
 * \return  new id
 */
CORBA::Long MCMESHTRAN::genId()
{
    return ++myMaxId ;
}

/*!
 * \brief set data to the specific study
 * \param studyID study ID
 * \param theData a List of Mesh Group
 * \return \c true if success
 */
CORBA::Boolean MCMESHTRAN::setData ( CORBA::Long studyID, const MCMESHTRAN_ORB::GroupList & theData)
{
    const int n = theData.length();
    MCMESHTRAN_ORB::GroupList_var aGroupList = new MCMESHTRAN_ORB::GroupList();
    aGroupList->length(n);

    for (int i=0; i<n; i++)
        aGroupList[i] = theData[i];  //deep copy

    myData[studyID] = aGroupList._retn();
    return (bool) n;
}

/*!
 * \brief get the group of mesh according to the \a studyID
 * \param studyID identifier of the study
 * \param outData output mesh group list
 * \return \c true if success
 */
MCMESHTRAN_ORB::GroupList *  MCMESHTRAN::getData (CORBA::Long studyID)
{
    if ( myData.find( studyID ) != myData.end() ) //find the list according to the list
    {
        return myData[ studyID ];
    }
    myData[ studyID ] = new MCMESHTRAN_ORB::GroupList;  //if the list was not created, create it
    return myData[ studyID ];
}

/*!
 * \brief generate a new Mesh Group
 * \param Name
 * \param MeshGroupId
 * \return the new mesh group
 */
MCMESHTRAN_ORB::MeshGroup_ptr MCMESHTRAN::generateGroup(const char* Name) throw (SALOME::SALOME_Exception)
{
    if (Name[0] == 0)
        THROW_SALOME_CORBA_EXCEPTION("Invalid group name", SALOME::BAD_PARAM);
//    ASSERT(Name[0] != 0); //if the name is empty, quit
    MESSAGE("Generate Group: " <<Name);
    MeshGroup * aGroup = new MeshGroup(Name, genId());
    return aGroup->_this();
}

/*!
 * \brief generate a new mesh
 * \param Name mesh name
 * \param Type mesh type: MCNPFmesh, MCNPUmesh, TRIPOLIFmesh, EmptyMesh, IntMesh
 * \param nps neutron history for averaging
 * \return
 */
MCMESHTRAN_ORB::Mesh_ptr MCMESHTRAN::generateMesh(const char* Name, const char* Type, CORBA::Double nps) throw (SALOME::SALOME_Exception)
{
    if (Name[0] == 0 || Type [0] == 0)
        THROW_SALOME_CORBA_EXCEPTION("Invalid mesh name or type", SALOME::BAD_PARAM);
//    ASSERT(Name[0] != 0); //if the name is empty, quit
    MESSAGE("Generate Mesh: " <<Name);
    Mesh * aMesh = new Mesh(Name, genId(), Type, nps);
    return aMesh->_this();
}

/*!
 * \brief get a mesh group from the study according to the \a GroupId
 * \param studyID
 * \param GroupID
 * \return \c true if success
 */
MCMESHTRAN_ORB::MeshGroup_ptr   MCMESHTRAN::getGroup(CORBA::Long studyID, CORBA::Long GroupID)
{
    if ( myData.find( studyID ) != myData.end() ) //find the list according to the list
    {
        MCMESHTRAN_ORB::GroupList * aGroupList = myData[studyID];

        const int n = aGroupList->length();
        for (int i = 0; i < n; i++)
        {
            MCMESHTRAN_ORB::MeshGroup_var  aGroup = (*aGroupList)[i];
            if (GroupID == aGroup->getID())
                return aGroup._retn(); //if find the Group, return the pointer
        }
        return MCMESHTRAN_ORB::MeshGroup::_nil();  //else return NULL
    }
    return MCMESHTRAN_ORB::MeshGroup::_nil();
}

/*!
 * \brief get a Mesh from the mesh group list of the study
 * \param studyID
 * \param MeshGroupId
 * \param MeshId
 * \return the Mesh
 */
MCMESHTRAN_ORB::Mesh_ptr MCMESHTRAN::getMesh(CORBA::Long studyID, CORBA::Long GroupID, CORBA::Long MeshID)
{
    MCMESHTRAN_ORB::MeshGroup_var aGroup = getGroup(studyID, GroupID) ;
    return aGroup->getMesh(MeshID);
}

/*!
 * \brief deep copy of a mesh
 * \param aMesh mesh to be copy
 * \return a new mesh just different in ID
 */
MCMESHTRAN_ORB::Mesh_ptr MCMESHTRAN::deepCopyMesh (MCMESHTRAN_ORB::Mesh_ptr aMesh)
throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Copy a nil mesh", SALOME::BAD_PARAM);
    MESSAGE("Copy Mesh: " <<aMesh->getName());
    Mesh * aNewMesh = new Mesh(aMesh->getName(), genId(), aMesh->getType(), aMesh->getNPS());
    if (strcmp(aMesh->getType(), "EmptyMesh") == 0)
        aNewMesh->setField(aMesh->getFieldTemplate());
    else
        aNewMesh->setField(aMesh->getField());
    return aNewMesh->_this();
}


/*!
 * \brief append new MeshGroup into the Group List
 * \param aMeshGroup
 * \return
 */
CORBA::Boolean MCMESHTRAN::appendGroup(CORBA::Long studyID, MCMESHTRAN_ORB::MeshGroup_ptr aGroup)// throw (SALOME::SALOME_Exception)
{
    if (aGroup->_is_nil())
    {
//        ASSERT(!aGroup->_is_nil());
        return false;
    }
    MESSAGE("Append a group: " << aGroup->getName());
    if (myData.find(studyID) == myData.end())  //if no grouplist in this study, create one
        myData[studyID] = new MCMESHTRAN_ORB::GroupList;
    const int n = myData[studyID]->length() + 1;
    myData[studyID]->length(n);

    //duplicate because the aGroup might be destroy outside
    MCMESHTRAN_ORB::MeshGroup_var aGroup_var = MCMESHTRAN_ORB::MeshGroup::_duplicate(aGroup);
    (*myData[studyID])[n-1] = aGroup_var._retn();

    return true;
}

/*!
 * \brief insert a Group into a study
 * \param studyID the study ID
 * \param aGroup the group to be insert
 * \param GroupID insert before the group with ID=GroupID
 * \return \c true if succeed
 */
CORBA::Boolean  MCMESHTRAN::insertGroup(CORBA::Long studyID, MCMESHTRAN_ORB::MeshGroup_ptr aGroup, CORBA::Long GroupID)// throw (SALOME::SALOME_Exception)
{
    if (aGroup->_is_nil())
    {
//        ASSERT(!aGroup->_is_nil());
        return false;
    }
    MESSAGE("Insert a group: " << aGroup->getName());
    //if no data in this study, create a group list and append the group into the list
    if (myData.find(studyID) == myData.end())
    {
        myData[studyID] = new MCMESHTRAN_ORB::GroupList;
    }
    if (0 == myData[studyID]->length())
    {
        appendGroup(studyID, aGroup);
        return false;  //insert failed
    }
    //if can not find the group, then append it
    if (getGroup(studyID, GroupID)->_is_nil())
    {
        appendGroup(studyID, aGroup);
        return false; //insert failed
    }

    const int n = myData[studyID]->length();
    myData[studyID]->length(n+1) ;  //increase the length with one element
    int i;
    for (i=0; i<n; i++)  //search for group
    {
        if (!CORBA::is_nil((*myData[studyID])[i]))
        {
            MCMESHTRAN_ORB::MeshGroup_var  aGroup_var = (*myData[studyID])[i];
            if (GroupID == aGroup_var->getID())
            {
                for (int j=n; j>i; j--)
                {
                    (*myData[studyID])[j] = (*myData[studyID])[j-1]; //see the detail design documents
                }
                break; //important
            }
        }
    }
    if (i == n)// cannot find the mesh
    {
        myData[studyID]->length(n);//recover the length if not insert
        return false;
    }

    //2013-03-22 duplicate because aGroup might be delete outside
    (*myData[studyID])[i] = MCMESHTRAN_ORB::MeshGroup::_duplicate(aGroup);  //copy the mesh to be insert into the list
    return true;
}

CORBA::Boolean  MCMESHTRAN::deleteGroup(CORBA::Long studyID, CORBA::Long GroupID) //throw (SALOME::SALOME_Exception)
{
    //similar with deleteMesh()
    MCMESHTRAN_ORB::MeshGroup_var aTmpGroup = getGroup(studyID, GroupID);
    if (aTmpGroup->_is_nil())
    {
//        ASSERT(!aTmpGroup->_is_nil());
        return false;
    }
    MESSAGE("Delete  group: " << aTmpGroup->getName());

    const int n = myData[studyID]->length();
    int i;
    for (i=0; i<n; i++)
    {
        if (!CORBA::is_nil((*myData[studyID])[i]))
        {
            MCMESHTRAN_ORB::MeshGroup_var  aGroup_var = (*myData[studyID])[i];
            if (GroupID == aGroup_var->getID())
            {
                for (int j=i; j<n-1; j++)
                {
                    (*myData[studyID])[j] = (*myData[studyID])[j+1]; //see the detail design documents
                }
                break;//important
            }
        }
    }
    if (i == n)// cannot find the mesh
        return false;

    myData[studyID]->length(n-1);  //reduce the lenght therefore delete on item
    return true;

}


CORBA::Boolean  MCMESHTRAN::clearGroup(CORBA::Long studyID)
{
    myData[studyID]->length(0);
    return true;
}

/*!
 * \brief read the MCNP5 Mesh tally output
 *  The mesh tallies are first configured by input file parameter setting
 *  then get data and store into these mesh tallies,
 *  if cylindrical mesh, the coordinate at the origin should be orthoganized
 *  and also useless mesh should be cut.
 * \param InputFileName mcnp input file
 * \param MeshtalFileName mcnp mesh tally output file
 * \return a group of mesh read from the above file
 */
MCMESHTRAN_ORB::MeshGroup_ptr   MCMESHTRAN::readMCNPFmesh(const char* InputFileName,const char* MeshtalFileName)
throw (SALOME::SALOME_Exception)
{
//    ASSERT((*InputFileName) != 0 ||  (*MeshtalFileName) != 0); //check if empty
    if (*InputFileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("Invalid input file name", SALOME::BAD_PARAM);  //THROW_SALOME_CORBA_EXCEPTION a utility for exception throwing
    if (*MeshtalFileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("Invalid mesh tally output file name", SALOME::BAD_PARAM);
    MESSAGE("MCNPFmeshReader: Reading " <<InputFileName<< " \n and "<< MeshtalFileName );

    vector <MCNPFmesh> MCNPFmeshVector ;
    MCNPFmeshReader aReader ;
    //read mesh tally configuration
    if (!aReader.ReadMeshConfig(InputFileName, MCNPFmeshVector))
        THROW_SALOME_CORBA_EXCEPTION("Read MCNP input file failed", SALOME::BAD_PARAM);
    //read mesh tally output file
    if (!aReader.ReadMeshtal(MeshtalFileName, MCNPFmeshVector))
        THROW_SALOME_CORBA_EXCEPTION("Read MCNP mes tally output file failed", SALOME::BAD_PARAM);

    for (int i=0; i<MCNPFmeshVector.size(); i++)
    {
        //orthogonlizing the coordinate in origin, necceesary for coordinate tranform
        if (MCNPFmeshVector[i].Coordinate == 2)
        {
            //orthogonalize the coordinate in origin point
            aReader.Orthogonalize(MCNPFmeshVector[i]);
            //cut the useless mesh
            aReader.CutMeshRind(MCNPFmeshVector[i]); // although the return is bool, this is not fatal to be an exception
        }
    }

    //convert a MCNPFmesh object to Mesh object, and append to the group
//    QStringList atmpList = QString(MeshtalFileName).split('/');  //cause Windows error 2015-09-01
	QFileInfo aFileInfo(MeshtalFileName); //for obtaining the file name
//	QFile aMeshtalFile (MeshtalFileName);
    MeshGroup * aGroup = new MeshGroup (/*atmpList.at(atmpList.size()-1).toLatin1()*/  aFileInfo.fileName().toLatin1(), genId()); //use file name as group name
    for (int i=0; i<MCNPFmeshVector.size(); i++)
    {
//		cout << "Convert mesh tally: "<<i<<endl; //for testing
        MCMESHTRAN_ORB::Mesh_var aNewMesh = cnvMCNPFmesh2Mesh(MCNPFmeshVector[i]);
        if (aNewMesh->_is_nil())
            THROW_SALOME_CORBA_EXCEPTION("A nil Mesh return", SALOME::BAD_PARAM);
        aGroup->appendMesh(aNewMesh._retn());
//		cout << "Convert mesh tally: "<<i<<" finished"<<endl; //for testing
    }
    return aGroup->_this();
}

/*!
 * \brief read the MCNP6 UMesh eeout file
 *  this file describe the mesh and result, it is a self-describing file;
 *  first the whole mesh is output by merging all the instances
 *  if required, field for each instance is also returned
 * \param UMeshOutFileName  the UMesh output file name
 * \param keepInstance  is the Instances filed data keep, \c false return only field for the whole mesh, \c true return the whole mesh and instances
 * \param removeMultiplier is the multiplier to be remove, \c true remove, \c not. keeping in mind that the multiplier for \a total field result is not removed.
 * \return a group of mesh
 */
MCMESHTRAN_ORB::MeshGroup_ptr   MCMESHTRAN::readMCNPUmesh(const char* UMeshOutFileName, CORBA::Boolean isKeepInstance, CORBA::Boolean isRemoveMultiplier)
throw (SALOME::SALOME_Exception)
{
    if (*UMeshOutFileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("Invalid UMesh output file name", SALOME::BAD_PARAM);
    MESSAGE("MCNPUmeshReader: Reading " <<UMeshOutFileName );

    //using reader to process the file
    MCMESHTRAN_MCNPUMeshReader aUMeshReader;
    aUMeshReader.setRemoveMultiplier(isRemoveMultiplier);
    aUMeshReader.setKeepInstances(isKeepInstance);
    if (!aUMeshReader.load_file(UMeshOutFileName))
        THROW_SALOME_CORBA_EXCEPTION("Read MCNP UMesh eeout failed!", SALOME::BAD_PARAM);


    //QStringList aStringList = QString::fromStdString(UMeshOutFileName).split("/", QString::SkipEmptyParts);
    //QString aTmpName = aStringList[aStringList.size() -1].split(".").at(0); //get the file name without path, only works in Linux
	//2015-09-02 to make possible Windows
	QString aTmpName = QFileInfo (UMeshOutFileName).fileName().split(".").at(0);
    MeshGroup * aGroup = new MeshGroup (aTmpName.toLatin1(), genId()); //use file name as group name
    vector <const MEDCouplingFieldDouble*> aTmpList = aUMeshReader.getFieldList();
    for (int i=0; i< aTmpList.size(); i++ )
    {
        //create field IOR
        ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI =
                new ParaMEDMEM::MEDCouplingFieldDoubleServant(
                    const_cast <MEDCouplingFieldDouble* > (aTmpList[i]));
        aTmpList[i]->decrRef(); //should derease reference here
        SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();
        //create new Mesh and append to the group
        Mesh * aNewMesh = new Mesh (aTmpList[i]->getName().c_str(), genId(), "MCNPUmesh", aUMeshReader.get_NPS());
        aNewMesh->setField(myFieldIOR);
        aGroup->appendMesh(aNewMesh->_this());
    }
    return aGroup->_this();
}

/*!
 * \brief MCMESHTRAN::readTRIPOLIFmesh
 *  Reader function for TRIPOLI mesh tally output,
 *  the results of mesh tallies will be processed
 * \param TRIPOLIFileName The TRIPOLI output file name
 * \return a group of mesh, which includes mesh and results
 */
MCMESHTRAN_ORB::MeshGroup_ptr   MCMESHTRAN::readTRIPOLIFmesh(const char* TRIPOLIFileName, CORBA::Boolean isVolAvg) throw (SALOME::SALOME_Exception)
{
    if (*TRIPOLIFileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("Invalid TRIPOLI output file name", SALOME::BAD_PARAM);
    MESSAGE("TRIPOLIReader: Reading " <<TRIPOLIFileName );

    MCMESHTRAN_TRIPOLIFmeshReader aTRIPOLIReader;
    if (!aTRIPOLIReader.loadFile(TRIPOLIFileName, isVolAvg))
        THROW_SALOME_CORBA_EXCEPTION("Read TRIPOLI output file failed!", SALOME::BAD_PARAM);

    //creat the group and get the field list
//    QStringList aStringList = QString::fromStdString(TRIPOLIFileName).split("/", QString::SkipEmptyParts);
//    QString aTmpName = aStringList[aStringList.size() -1].split(".").at(0); //get the file name without path, only works in Linux
	//2015-09-02 to make possible Windows
	QString aTmpName = QFileInfo (TRIPOLIFileName).fileName().split(".").at(0);
    MeshGroup * aGroup = new MeshGroup (aTmpName.toLatin1(), genId()); //use file name as group name
    vector <const MEDCouplingFieldDouble*> aTmpList = aTRIPOLIReader.getFieldList();

    for (int i=0; i< aTmpList.size(); i++ )
    {
        //create field IOR
        ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI =
                new ParaMEDMEM::MEDCouplingFieldDoubleServant(
                    const_cast <MEDCouplingFieldDouble* > (aTmpList[i]));
        aTmpList[i]->decrRef(); //should derease reference here
        SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();
        //create new Mesh and append to the group
        cout <<aTmpList[i]->getName().c_str() <<endl;
        Mesh * aNewMesh = new Mesh (aTmpList[i]->getName().c_str(), genId(), "TRIPOLIFmesh", aTRIPOLIReader.get_NPS());
        aNewMesh->setField(myFieldIOR);
        aGroup->appendMesh(aNewMesh->_this());
    }

    return aGroup->_this();

}


/*!
 * \brief check the two mesh to see if MEDCoupling mesh is identical
 *  this method provide checking before summing or averaging two mesh
 * \param aMesh  Mesh A
 * \param bMesh  Mesh B
 * \return  \c true if identical, \c false if not
 * \see  SALOME CALCULATOR::add()
 */
CORBA::Boolean  MCMESHTRAN::checkMeshConsistency(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                     MCMESHTRAN_ORB::Mesh_ptr bMesh)  throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() || bMesh->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil", SALOME::BAD_PARAM);
    if (aMesh->getField()->_is_nil() || bMesh->getField()->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Mesh field is nil", SALOME::BAD_PARAM);

    //see SALOME CALCULATOR::add()
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> bField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(bMesh->getField());

//    //see SALOME XYZMESHGUI::OnUnitTest()
//    ParaMEDMEM::MEDCouplingFieldDouble * aField = (ParaMEDMEM::MEDCouplingFieldDouble *)aField_ptr;   //??
//    ParaMEDMEM::MEDCouplingFieldDouble * bField = (ParaMEDMEM::MEDCouplingFieldDouble *)bField_ptr;

    //check if mesh type is Unstructured, see ParaMEDMEM::MEDCouplingMeshType for detail
    if ( aField_ptr->getMesh()->getType() != ParaMEDMEM::UNSTRUCTURED || bField_ptr->getMesh()->getType() != ParaMEDMEM::UNSTRUCTURED)
        THROW_SALOME_CORBA_EXCEPTION("Mesh Type is not Ustructred Mesh", SALOME::BAD_PARAM);

    ParaMEDMEM::MEDCouplingMesh * aUMesh =  const_cast<ParaMEDMEM::MEDCouplingMesh * > (aField_ptr->getMesh());
    ParaMEDMEM::MEDCouplingMesh * bUMesh =  const_cast<ParaMEDMEM::MEDCouplingMesh * > (bField_ptr->getMesh());

    string aTmpStr;
    bool isEq;
    try
    {
        //check equal or not
        isEq = aUMesh->isEqualIfNotWhy(bUMesh, 1e-12, aTmpStr);
    }
    catch(INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION( ex.what(), SALOME::BAD_PARAM);
    }
    if (!isEq)
        cout << "Not equal: " << aTmpStr << endl;

    //not need to do this
//    aField_ptr->decrRef();//destroy the reference copy
//    bField_ptr->decrRef();

    return isEq;
}


/*!
 * \brief sum up field on two Meshes, used in sum up neutron and photon
 *  the mesh should be identical
 * \param aMesh Mesh A
 * \param bMesh Mesh B
 * \param Name  Name for new Mesh
 * \return new mesh
 * \see SALOME CALCULATOR::add()
 * \todo Deal with NPS,
 */
MCMESHTRAN_ORB::Mesh_ptr        MCMESHTRAN::sumMeshes(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                          MCMESHTRAN_ORB::Mesh_ptr bMesh, const char* Name)  throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() || bMesh->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("sum with nil mesh!", SALOME::BAD_PARAM);
    if (*Name == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty Name!", SALOME::BAD_PARAM);
    MESSAGE ("Sum Meshes: " << aMesh->getName() << " and " << bMesh->getName());

    //perform a series of checking
    if (strcmp(aMesh->getType(), bMesh->getType()) != 0)
        THROW_SALOME_CORBA_EXCEPTION("Two Meshes have different type", SALOME::BAD_PARAM);
    if (aMesh->getField()->_is_nil() ||  bMesh->getField()->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh has nil field!", SALOME::BAD_PARAM);
    if (!checkMeshConsistency(aMesh, bMesh))
        THROW_SALOME_CORBA_EXCEPTION("Two Meshes are not consistent", SALOME::BAD_PARAM);


    //see SALOME CALCULATOR::add()
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> bField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(bMesh->getField());
    //copy the mesh and name
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble>  newField_ptr= aField_ptr->deepCpy();
    newField_ptr->setName(Name);
    vector <int > aVec , bVec;
    aVec.push_back(0);
    bVec.push_back(1);

    ParaMEDMEM::DataArrayDouble * aArray1 = static_cast <DataArrayDouble * > (aField_ptr->getArray()->keepSelectedComponents(aVec));
    ParaMEDMEM::DataArrayDouble * aArray2 = static_cast <DataArrayDouble * > (aField_ptr->getArray()->keepSelectedComponents(bVec));
    ParaMEDMEM::DataArrayDouble * bArray1 = static_cast <DataArrayDouble * > (bField_ptr->getArray()->keepSelectedComponents(aVec));
    ParaMEDMEM::DataArrayDouble * bArray2 = static_cast <DataArrayDouble * > (bField_ptr->getArray()->keepSelectedComponents(bVec));
    ParaMEDMEM::DataArrayDouble * cArray1  ;
    ParaMEDMEM::DataArrayDouble * cArray2  ;
    ParaMEDMEM::DataArrayDouble * cArray  ;
    //sum the tally result , but keep the maximum error
    try
    {
        cArray1 = ParaMEDMEM::DataArrayDouble::Add(aArray1, bArray1); //sum the tally result
        aArray1->decrRef();
        bArray1->decrRef();
        cArray2 = ParaMEDMEM::DataArrayDouble::Max(aArray2, bArray2); //get max error
        aArray2->decrRef();
        bArray2->decrRef();
        cArray =  ParaMEDMEM::DataArrayDouble::Meld(cArray1, cArray2); //merge the tally and error result in one array
        cArray1->decrRef();
        cArray2->decrRef();
        newField_ptr->setArray(cArray);
        cArray->decrRef();
//        newField_ptr = (*aField_ptr) + (*bField_ptr);  //addition of two fields
    }
    catch(INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION( ex.what(), SALOME::BAD_PARAM);
    }

    // create CORBA field from c++ toField. give property to servant (true)
    ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(newField_ptr);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();

    //create new mesh
    //!!!!!!!ATTENTION!!!!!!!!!!
    //here we consider summing Neutron and Photon Mesh,
    //therefore we use the smaller NPS as the NPS in new mesh
    Mesh * aNewMesh = new Mesh (Name, genId(), aMesh->getType(), min(aMesh->getNPS(), bMesh->getNPS()));
    aNewMesh->setField(myFieldIOR);

//    aField_ptr->decrRef();//???????
//    bField_ptr->decrRef();

    return aNewMesh->_this();
}

/*!
 * \brief NPS weighted averaging of two mesh
 *   (aField* aNPS + bField*bNPS) /(aNPS + bNPS)
 * \param aMesh  Mesh A
 * \param bMesh  Mesh B
 * \param Name  new mesh name
 * \return the new mesh
 * \see SALOME CALCULATOR::applyLin()
 */
MCMESHTRAN_ORB::Mesh_ptr        MCMESHTRAN::averageMeshes(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                              MCMESHTRAN_ORB::Mesh_ptr bMesh, const char* Name)  throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() || bMesh->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Averaging nil meshes!", SALOME::BAD_PARAM);
    if (*Name == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty Name!", SALOME::BAD_PARAM);
    MESSAGE ("Average Meshes: " << aMesh->getName() << " and " << bMesh->getName());

    //perform a series of checking
    if (strcmp(aMesh->getType(), bMesh->getType()) != 0)
        THROW_SALOME_CORBA_EXCEPTION("Two Meshes have different type", SALOME::BAD_PARAM);
    if (!checkMeshConsistency(aMesh, bMesh))
        THROW_SALOME_CORBA_EXCEPTION("Two Meshes are not consistent", SALOME::BAD_PARAM);
    if (aMesh->getNPS() == 0 || bMesh->getNPS() == 0)
        THROW_SALOME_CORBA_EXCEPTION("NPS must not equal 0", SALOME::BAD_PARAM);
    if (aMesh->getField()->_is_nil() ||  bMesh->getField()->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh has nil field!", SALOME::BAD_PARAM);

    //see SALOME CALCULATOR::add()
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> bField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(bMesh->getField());

    // see SALOME CALCULATOR::add()
    //see http://docs.salome-platform.org/salome_6_6_0/gui/MED/classParaMEDMEM_1_1MEDCouplingFieldDouble.html#a96e5a6edc59c5e701ea2eddac215eeaa
    // the mesh pointer will be the same with source field when using clone()
//    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble>  aTmpField_ptr = aField_ptr->deepCpy();
//    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble>  bTmpField_ptr = bField_ptr->deepCpy();
    //for copying the mesh and field info
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble>  newField_ptr = aField_ptr->deepCpy() ;
    newField_ptr->setName(Name);

    ParaMEDMEM::DataArrayDouble * aArray = aField_ptr->getArray()->deepCpy();
    ParaMEDMEM::DataArrayDouble * bArray = bField_ptr->getArray()->deepCpy();
    ParaMEDMEM::DataArrayDouble * cArray  ;
    try
    {
        // method: (aField* aNPS + bField*bNPS) /(aNPS + bNPS)
        //see SALOME CALCULATOR::applyLin()
        //this is not working because aTmpField_ptr and bTmpField_ptr should have the same mesh pointer
//        aTmpField_ptr->getArray()->rearrange(1); //rearrange to set No of Componenet to be 1
//        bTmpField_ptr->getArray()->rearrange(1);
//        aTmpField_ptr->applyLin(aMesh->getNPS(), 0 , 0); //afield * aNPS, both tally and error
//        bTmpField_ptr->applyLin(bMesh->getNPS(), 0 , 0); //bfield * bNPS, both tally and error
//        newField_ptr = (*aTmpField_ptr) + (*bTmpField_ptr);
//        newField_ptr->applyLin( 1/(aMesh->getNPS() + bMesh->getNPS()), 0, 0);  // newfiled * 1/(aNPS + bNPS)
//        newField_ptr->getArray()->rearrange(2);
        aArray->applyLin(aMesh->getNPS(), 0 );
        bArray->applyLin(bMesh->getNPS(), 0 );
        cArray = ParaMEDMEM::DataArrayDouble::Add(aArray, bArray);
        cArray->applyLin( 1/(aMesh->getNPS() + bMesh->getNPS()), 0);
        aArray->decrRef();
        bArray->decrRef();
        newField_ptr->setArray(cArray);
        cArray->decrRef();
    }
    catch(INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION( ex.what(), SALOME::BAD_PARAM);
    }

    // create CORBA field from c++ toField. give property to servant (true)
    ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(newField_ptr);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();

    //create new mesh
    //!!!!!!!ATTENTION!!!!!!!!!!
    //here we consider average Neutron and Neutron Mesh, or Photon and Photon
    //therefore we use the sum of NPS
    Mesh * aNewMesh = new Mesh (Name, genId(), aMesh->getType(), (aMesh->getNPS() + bMesh->getNPS()));
    aNewMesh->setField(myFieldIOR);

//    aField_ptr->decrRef();//???????
//    bField_ptr->decrRef();
//    aTmpField_ptr->decrRef();
//    bTmpField_ptr->decrRef();

    return aNewMesh->_this();
}

/*!
 * \brief translate the mesh according to the vector
 * \param aMesh mesh to be translate
 * \param vector moving vector
 * \param Name name for the new mesh
 * \return the new mesh
 */
MCMESHTRAN_ORB::Mesh_ptr        MCMESHTRAN::translateMesh(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                         const MCMESHTRAN_ORB::FixArray3 vector, const char* Name)  throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*Name == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty Name!", SALOME::BAD_PARAM);
    if (aMesh->getField()->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Mesh has nil field!", SALOME::BAD_PARAM);
    MESSAGE ("Translate Meshes: " << aMesh->getName() );
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
    //copy the field
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble>  newField_ptr =  aField_ptr->deepCpy();
    newField_ptr->setName(Name);
    ParaMEDMEM::MEDCouplingMesh * aUmesh = const_cast<ParaMEDMEM::MEDCouplingMesh *>( newField_ptr->getMesh());

    const double vec [3] = {vector[0], vector[1], vector[2]};
    try
    {
        //translate (move) the mesh
        aUmesh->translate(vec); //
    }
    catch(INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION( ex.what(), SALOME::BAD_PARAM);
    }

    // create CORBA field from c++ toField. give property to servant (true)
    ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(newField_ptr);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();

    Mesh * aNewMesh = new Mesh (Name, genId(), aMesh->getType(), aMesh->getNPS());
    aNewMesh->setField(myFieldIOR);

//    aField_ptr->decrRef();
    return aNewMesh->_this();
}

/*!
 * \brief rotate the mesh according to a center, a vector and an angle
 * \param aMesh the mesh to be rotate
 * \param center one fix point on the rotate axis
 * \param vector vector of the rotate axis
 * \param AngleInDegree rotate angle in degree
 * \param Name Name for new mesh
 * \return the new mesh
 */
MCMESHTRAN_ORB::Mesh_ptr        MCMESHTRAN::rotateMesh(MCMESHTRAN_ORB::Mesh_ptr aMesh, const
                                           MCMESHTRAN_ORB::FixArray3 center,
                                           const MCMESHTRAN_ORB::FixArray3 vector,
                                           CORBA::Double AngleInDegree, const char* Name)  throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*Name == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty Name!", SALOME::BAD_PARAM);
    if (aMesh->getField()->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Mesh has nil field!", SALOME::BAD_PARAM);
    MESSAGE ("Rotate Meshes: " << aMesh->getName() );
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
    //copy the field
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble>  newField_ptr =  aField_ptr->deepCpy();
    newField_ptr->setName(Name);
    ParaMEDMEM::MEDCouplingMesh * aUmesh = const_cast<ParaMEDMEM::MEDCouplingMesh *>( newField_ptr->getMesh());

    const double cntr [3] = {center[0], center[1], center[2]};
    const double vec [3] =  {vector[0], vector[1], vector[2]};
    try
    {
        //rotate the mesh
        aUmesh->rotate(cntr, vec, AngleInDegree * PI /180);
    }
    catch(INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION( ex.what(), SALOME::BAD_PARAM);
    }

    // create CORBA field from c++ toField. give property to servant (true)
    ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(newField_ptr);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();

    Mesh * aNewMesh = new Mesh (Name, genId(), aMesh->getType(), aMesh->getNPS());
    aNewMesh->setField(myFieldIOR);

//    aField_ptr->decrRef();
    return aNewMesh->_this();
}

/*!
 * \brief scale the mesh dimension with a factor
 * \param aMesh  mesh to be scale
 * \param Factor a const factor
 * \param Name Name for new mesh
 * \return  the new mesh
 */
MCMESHTRAN_ORB::Mesh_ptr        MCMESHTRAN::scaleMesh(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                          CORBA::Double Factor, const char* Name)  throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*Name == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty Name!", SALOME::BAD_PARAM);
    if (Factor == 0)
        THROW_SALOME_CORBA_EXCEPTION("Factor should not be zero!", SALOME::BAD_PARAM);
    if (aMesh->getField()->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Mesh has nil field!", SALOME::BAD_PARAM);
    MESSAGE ("Scale the Mesh:  " << aMesh->getName() );
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
    //copy the field
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble>  newField_ptr =  aField_ptr->deepCpy();
    newField_ptr->setName(Name);
    ParaMEDMEM::MEDCouplingMesh * aUmesh = const_cast<ParaMEDMEM::MEDCouplingMesh *>( newField_ptr->getMesh());

    const double cntr [3] = {0, 0, 0};  //scale the mesh based on the origin.
    try
    {
        //Scale the mesh
        aUmesh->scale(cntr, Factor);
    }
    catch(INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
    }

    // create CORBA field from c++ toField. give property to servant (true)
    ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(newField_ptr);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();

    Mesh * aNewMesh = new Mesh (Name, genId(), aMesh->getType(), aMesh->getNPS());
    aNewMesh->setField(myFieldIOR);

//    aField_ptr->decrRef();
    return aNewMesh->_this();

}


/*!
 * \brief multiply a Factor with the field
 * \param aMesh the mesh field to be multiplied
 * \param Factor the const factor
 * \param Name Name for the new mesh
 * \return  the new mesh
 */
MCMESHTRAN_ORB::Mesh_ptr        MCMESHTRAN::multiplyFactor(MCMESHTRAN_ORB::Mesh_ptr aMesh,
                                               CORBA::Double Factor, const char* Name)  throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*Name == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty Name!", SALOME::BAD_PARAM);
    if (Factor == 0)
        THROW_SALOME_CORBA_EXCEPTION("Factor should not be zero!", SALOME::BAD_PARAM);
    if (aMesh->getField()->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Mesh has nil field!", SALOME::BAD_PARAM);

    MESSAGE ("Multiply the field with a factor: " << aMesh->getName() );
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
    //copy the field
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble>  newField_ptr =  aField_ptr->deepCpy();
    newField_ptr->setName(Name);

    //    if (newField_ptr->getArray()->getNumberOfComponents() != 2)  // check if contains both Tally and Error data
    //        THROW_SALOME_CORBA_EXCEPTION("Abnormal Field for factor multiplication", SALOME::BAD_PARAM);

    try
    {
        //2014-2-6 qiu for temporary used
        for (int i=0; i<newField_ptr->getNumberOfComponents(); i++) {
            newField_ptr->applyLin(Factor, 0, i);     //only with the tally2 014-2-6 qiu for temporary used
        }
    }
    catch(INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION( ex.what(), SALOME::BAD_PARAM);
    }

    // create CORBA field from c++ toField. give property to servant (true)
    ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(newField_ptr);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();

    Mesh * aNewMesh = new Mesh (Name, genId(), aMesh->getType(), aMesh->getNPS());
    aNewMesh->setField(myFieldIOR);

//    aField_ptr->decrRef();
    return aNewMesh->_this();
}

/*!
 * \brief perform conservative interpolation of field
 *  this method try to use MEDCouplingRemapper class
 * \param source source mesh with field
 * \param target target mesh
 * \param Name name for the new mesh
 * \return the new mesh
 */
MCMESHTRAN_ORB::Mesh_ptr        MCMESHTRAN::interpolateMEDMesh(MCMESHTRAN_ORB::Mesh_ptr source,
                                                MCMESHTRAN_ORB::Mesh_ptr target, const char* Name,
                                                MCMESHTRAN_ORB::SolutionLoc solloc)  throw (SALOME::SALOME_Exception)
{
    //check
    if (source->_is_nil() || target->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*Name == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty Name!", SALOME::BAD_PARAM);
    if (source->getField()->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Source mesh has nil field!", SALOME::BAD_PARAM);
    if (target->getFieldTemplate()->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Target mesh has nil mesh!", SALOME::BAD_PARAM);

    MESSAGE ("Interpolate Meshes: " << source->getName() << " to " << target->getName());
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR;
    try
    {
        //first try with MEDCouplingRemapper
        myFieldIOR = interpolateWithRemapper(source->getField(), target->getFieldTemplate()->getMesh(), Name, solloc);
    }
    catch (SALOME::SALOME_Exception)
    {
        THROW_SALOME_CORBA_EXCEPTION("Interpolation using MEDCouplingRemapper failed", SALOME::BAD_PARAM);
    }
    Mesh * aNewMesh = new Mesh (Name, genId(), "IntMesh", source->getNPS());
    aNewMesh->setField(myFieldIOR);
    return aNewMesh->_this();
}


#ifdef WITH_CGNS
/*!
 * \brief perform conservative interpolation of field
 *  this method try to use CGNS utilities
 * \param source source mesh with field
 * \param target target mesh
 * \param Name name for the new mesh
 * \return the new mesh
 */
MCMESHTRAN_ORB::Mesh_ptr        MCMESHTRAN::interpolateCGNSMesh(MCMESHTRAN_ORB::Mesh_ptr source,
                                                MCMESHTRAN_ORB::Mesh_ptr target, const char* Name,
                                                MCMESHTRAN_ORB::SolutionLoc solloc)  throw (SALOME::SALOME_Exception)
{
    //check
    if (source->_is_nil() || target->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*Name == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty Name!", SALOME::BAD_PARAM);
    if (source->getField()->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Source mesh has nil field!", SALOME::BAD_PARAM);
    if (target->getFieldTemplate()->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Target mesh has nil mesh!", SALOME::BAD_PARAM);

    MESSAGE ("Interpolate Meshes: " << source->getName() << " to " << target->getName());
    QString aTmpDir = "/tmp/";
    QString atmpCGNSField = aTmpDir + QString("tmpfileInterpolateMeshField.cgns");
    QString atmpCGNSMesh = aTmpDir + QString ("tmpfileInterpolateMeshMesh.cgns");
    QString atmpCGNSOutField = aTmpDir + QString("tmpfileInterpolateMeshOutField.cgns");
    //export mesh and field to CGNS files
    export2CGNS(source, atmpCGNSField.toLatin1());
    export2CGNS(target, atmpCGNSMesh.toLatin1());
    //write the script
    //the executable should be put under $PATH after salome.sh is executed
    QString aCommand = "interpolatecgns -w " + atmpCGNSField + " " + atmpCGNSMesh + " "+ atmpCGNSOutField;
    if (system(aCommand.toLatin1()))  //if  exit with 0
        THROW_SALOME_CORBA_EXCEPTION("Interpolation failed!", SALOME::BAD_PARAM);
    QFile::remove (atmpCGNSField);
    QFile::remove (atmpCGNSMesh);

    if (solloc == MCMESHTRAN_ORB::on_cells)
    {
        aCommand = "convertlocation -c -w " +  atmpCGNSOutField;
        if (system(aCommand.toLatin1()))  //if  exit with 0
            THROW_SALOME_CORBA_EXCEPTION("Convert solution location failed!", SALOME::BAD_PARAM);
    }
    //import the interpolated CGNS file
    MCMESHTRAN_ORB::MeshGroup_var atmpGroup = importCGNS(atmpCGNSOutField.toLatin1());
    QFile::remove (atmpCGNSOutField);

    if (atmpGroup->getMeshList()->length() == 0)
        THROW_SALOME_CORBA_EXCEPTION("Error in getting interpolated field!!", SALOME::BAD_PARAM);

    MCMESHTRAN_ORB::Mesh_var atmpMesh = (*atmpGroup->getMeshList())[0];
    Mesh * aNewMesh = new Mesh (Name, genId(), "IntMesh", source->getNPS());
    aNewMesh->setField(atmpMesh->getField());
    return aNewMesh->_this();
}

#endif //WITH_CGNS


/*!
 * \brief export to VTK format
 * \param aMesh mesh to be exported
 * \param FileName vtk file name
 */
void MCMESHTRAN::export2VTK(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName)  throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*FileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Write Mesh: " << aMesh->getName() << " to " << FileName);

    if (strcmp(aMesh->getType(), "EmptyMesh") != 0)
    {
        if (aMesh->getField()->_is_nil())
            THROW_SALOME_CORBA_EXCEPTION("Write mesh failed: no field", SALOME::BAD_PARAM);
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
                =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
        std::vector<const ParaMEDMEM::MEDCouplingFieldDouble *>  aFieldVec ;
        aFieldVec.push_back( (ParaMEDMEM::MEDCouplingFieldDouble *) aField_ptr);  //explict cast incase any error
        try
        {
            //writeVTK is a static method
            ParaMEDMEM::MEDCouplingFieldDouble::WriteVTK(FileName, aFieldVec);
        }
        catch (INTERP_KERNEL::Exception &ex)
        {
            THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
        }
    }
    else
    {
        if (aMesh->getFieldTemplate()->_is_nil())
            THROW_SALOME_CORBA_EXCEPTION("Write mesh failed: no mesh", SALOME::BAD_PARAM);
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldTemplate> aFieldTemplate_ptr
                =ParaMEDMEM::MEDCouplingFieldTemplateClient::New(aMesh->getFieldTemplate());
        try
        {
            aFieldTemplate_ptr->getMesh()->writeVTK(FileName);
        }
        catch (INTERP_KERNEL::Exception &ex)
        {
            THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
        }
    }
}

/*!
 * \brief export to MED format
 * \param aMesh mesh to be exported
 * \param FileName MED File name
 */
void MCMESHTRAN::export2MED(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName)    throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*FileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Write Mesh: " << aMesh->getName() << " to " << FileName);

    if (strcmp(aMesh->getType(), "EmptyMesh") != 0)
    {
        if (aMesh->getField()->_is_nil())
            THROW_SALOME_CORBA_EXCEPTION("Write mesh failed: no field", SALOME::BAD_PARAM);

        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
                =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
//        //2013-05-29
//        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingUMesh> aUMesh_ptr
//                = static_cast <ParaMEDMEM::MEDCouplingUMesh *> (const_cast <ParaMEDMEM::MEDCouplingMesh *> (aField_ptr->getMesh()));
        try
        {
            MEDLoader::WriteField(FileName, (ParaMEDMEM::MEDCouplingFieldDouble *)aField_ptr, true);
            //2013-05-29 changed into Advance API
            //see http://docs.salome-platform.org/salome_6_6_0/gui/MED/MEDLoaderAdvancedAPIPage.html#AdvMEDLoaderAPIFieldWC
//            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDFileUMesh > aFileUMesh
//                    = ParaMEDMEM::MEDFileUMesh::New();
//            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDFileField1TS > aFileField
//                    = ParaMEDMEM::MEDFileField1TS::New();
//            aFileUMesh->setMeshAtLevel(0, aUMesh_ptr);
//            aFileField->setFieldNoProfileSBT(aField_ptr);
//            aFileUMesh->write(FileName, 2);
//            aFileField->write(FileName, 0);

        }
        catch (INTERP_KERNEL::Exception &ex)
        {
            THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
        }
    }
    else
    {
        if (aMesh->getFieldTemplate()->_is_nil())
            THROW_SALOME_CORBA_EXCEPTION("Write mesh failed: no mesh", SALOME::BAD_PARAM);
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldTemplate> aFieldTemplate_ptr
                =ParaMEDMEM::MEDCouplingFieldTemplateClient::New(aMesh->getFieldTemplate());
        //2013-05-29
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingUMesh> aUMesh_ptr
                = static_cast <ParaMEDMEM::MEDCouplingUMesh *> (const_cast <ParaMEDMEM::MEDCouplingMesh *> (aFieldTemplate_ptr->getMesh()));

        //first cast away const, then cast to child class
        //changed 2013-4-4 because this autoptr might cause segmentation fault if not appropriately released
//        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingUMesh> aUmesh_ptr
//                = static_cast <ParaMEDMEM::MEDCouplingUMesh *>
//                (const_cast <ParaMEDMEM::MEDCouplingMesh *> (aFieldTemplate_ptr->getMesh()));
        try
        {
//            MEDLoader::WriteUMesh(FileName, aUmesh_ptr, true);
//            MEDLoader::WriteUMesh(FileName,
//                                  static_cast <ParaMEDMEM::MEDCouplingUMesh *> (const_cast <ParaMEDMEM::MEDCouplingMesh *> (aFieldTemplate_ptr->getMesh()))
//                                  , true);

            //2013-05-29 changed into Advance API
            //see http://docs.salome-platform.org/salome_6_6_0/gui/MED/MEDLoaderAdvancedAPIPage.html#AdvMEDLoaderAPIFieldWC
            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDFileUMesh > aFileUMesh
                    = ParaMEDMEM::MEDFileUMesh::New();
            aFileUMesh->setMeshAtLevel(0, aUMesh_ptr);
            aFileUMesh->write(FileName, 2);
        }
        catch (INTERP_KERNEL::Exception &ex)
        {
            THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
        }
    }
}

#ifdef  WITH_CGNS
/*!
 * \brief export mesh into CGNS format
 * \param aMesh mesh to be export
 * \param FileName CGNS file name
 */
void MCMESHTRAN::export2CGNS(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName)   throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*FileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Write Mesh: " << aMesh->getName() << " to " << FileName);
    if (strcmp(aMesh->getType(), "EmptyMesh") == 0)
    {
        if (aMesh->getFieldTemplate()->_is_nil())
            THROW_SALOME_CORBA_EXCEPTION("Write mesh failed: no field", SALOME::BAD_PARAM);
    }
    else
    {
        if (aMesh->getField()->_is_nil())
            THROW_SALOME_CORBA_EXCEPTION("Write mesh failed: no field", SALOME::BAD_PARAM);
    }

    bool isOnlyMesh ; // true if the mesh is only mesh
    if (strcmp(aMesh->getType(), "EmptyMesh") == 0 )
        isOnlyMesh = true;
    //get the mesh, no matter isOnlyMesh is true or not
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingUMesh> aMesh_ptr
             = static_cast <ParaMEDMEM::MEDCouplingUMesh *> ( ParaMEDMEM::MEDCouplingMeshClient::New(aMesh->getFieldTemplate()->getMesh()));


    //************Start to write the CGNS file **************//
    //defind variable for reading the CGNS file
    int index_file,icelldim,iphysdim,index_base;
    int index_zone ,index_coord, index_sec, index_sol, index_field;
    string basename,zonename,solname,fieldname;
    //open file
    if (cg_open(FileName,CG_MODE_WRITE,&index_file)) cg_error_exit();
    /* create base (user can give any name) */
    basename = "Base";
    icelldim=3;
    iphysdim=3;
    if (cg_base_write(index_file,basename.c_str(),icelldim,iphysdim,&index_base)) cg_error_exit();
    /* define zone name (user can give any name) */
    zonename = "Zone 1" ;
    cgsize_t isize[3][3];
    isize [0][0] = aMesh_ptr->getNumberOfNodes();
    isize [0][1] = aMesh_ptr->getNumberOfCells();
    isize [0][2] = 0; //boundary vertex size (zero if elements not sorted)
    /* create zone */
    if (cg_zone_write(index_file,index_base,zonename.c_str(),*isize,Unstructured,&index_zone)) cg_error_exit();
    /* write grid coordinates (user must use SIDS-standard names here) */
    vector <double> XXX;
    vector <double> YYY;
    vector <double> ZZZ;
    XXX.resize(aMesh_ptr->getNumberOfNodes());
    YYY.resize(aMesh_ptr->getNumberOfNodes());
    ZZZ.resize(aMesh_ptr->getNumberOfNodes());

    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble > aVertiesArray
            =aMesh_ptr->getCoordinatesAndOwner();
//    double * aTuple = new double [3]; //for getting  coordinates x, y, z
    vector <double > aTuple;
    aTuple.resize(3);
    for (int i=0; i< aVertiesArray->getNumberOfTuples(); i++)
    {
        aVertiesArray->getTuple(i, aTuple.data());
        XXX[i] = aTuple[0];
        YYY[i] = aTuple[1];
        ZZZ[i] = aTuple[2];
    }
    if (cg_coord_write(index_file,index_base,index_zone,RealDouble,"CoordinateX",
                       XXX.data(),&index_coord)) cg_error_exit();
    if (cg_coord_write(index_file,index_base,index_zone,RealDouble,"CoordinateY",
                       YYY.data(),&index_coord)) cg_error_exit();
    if (cg_coord_write(index_file,index_base,index_zone,RealDouble,"CoordinateZ",
                       ZZZ.data(),&index_coord)) cg_error_exit();
    XXX.clear();
    YYY.clear();
    ZZZ.clear();

    /*write element connectivities. consider Mix cell type*/
    int irminEle, irmaxEle;
    irminEle = 1;
    irmaxEle = aMesh_ptr->getNumberOfCells();
    vector <int> ElementVec;
    long long int Pos1 = 0;  //index for the long mElements array
    long long int Pos2 = 0;  //index for the long mElements array
    for (int i=0; i<irmaxEle; i++)
    {
        Pos1 = aMesh_ptr->getNodalConnectivityIndex()->getPointer()[i]; //begin of  node list of this element
        Pos2 = aMesh_ptr->getNodalConnectivityIndex()->getPointer()[i+1]; //end of  node list of this element
        int aMedEleType = aMesh_ptr->getNodalConnectivity()->getPointer()[Pos1]; //get the MED element type
        //we consider that there are no non-3D element
        int iElementType; //CGNS element type
        switch(aMedEleType)
        {
        case INTERP_KERNEL::NORM_TETRA4:
            iElementType =  TETRA_4 ; break;
        case INTERP_KERNEL::NORM_TETRA10 :
            iElementType = TETRA_10; break;
        case INTERP_KERNEL::NORM_PYRA5   :
            iElementType =  PYRA_5; break;
        case INTERP_KERNEL::NORM_PYRA13  :
            iElementType = PYRA_13; break;
        case INTERP_KERNEL::NORM_PENTA6  :
            iElementType =  PENTA_6; break;
        case INTERP_KERNEL::NORM_PENTA15 :
            iElementType = PENTA_15; break;
        case INTERP_KERNEL::NORM_HEXA8   :
            iElementType =  HEXA_8; break;
        case INTERP_KERNEL::NORM_HEXA20  :
            iElementType = HEXA_20; break;
        case INTERP_KERNEL::NORM_HEXA27  :
            iElementType = HEXA_27; break;
        default:
            THROW_SALOME_CORBA_EXCEPTION("Unnormal in judging Element type!", SALOME::BAD_PARAM);
        }
        ElementVec.push_back(iElementType); //first push a element type
        for (int j=Pos1+1; j<Pos2; j++) //then push element connectivities
            ElementVec.push_back(aMesh_ptr->getNodalConnectivity()->getPointer()[j] + 1); //because CGNS start the index at 1
    }
    if (Pos2 != aMesh_ptr->getNodalConnectivity()->getNbOfElems())
        THROW_SALOME_CORBA_EXCEPTION("Unnormal in mesh connectivities!", SALOME::BAD_PARAM);
    if (cg_section_write(index_file,index_base,index_zone,"ElemConn",MIXED,irminEle,irmaxEle,0,ElementVec.data(),&index_sec)) cg_error_exit();

    //if only mesh, save and exit
    if (isOnlyMesh)
    {
        cg_close(index_file);
        return;
    }

    //get the field if exist
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            = ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());

    /*write field data*/
    solname = aMesh->getName(); //use mesh name as solution name
    vector <double > theField;
    vector <double > bTuple;

    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble > aFieldDataArray
            = aField_ptr->getArray();
    bTuple.resize(aFieldDataArray->getNumberOfComponents());
    if (aField_ptr->getTypeOfField() != ParaMEDMEM::ON_CELLS && aField_ptr->getTypeOfField() != ParaMEDMEM::ON_NODES)
        THROW_SALOME_CORBA_EXCEPTION("Unsupported field type, should be ON_CELLS or ON_NODES!", SALOME::BAD_PARAM);
    GridLocation_t gridLoc = aField_ptr->getTypeOfField() == ParaMEDMEM::ON_CELLS ? CellCenter : Vertex;
    if (cg_sol_write(index_file, index_base, index_zone, solname.c_str(), gridLoc,&index_sol)) cg_error_exit();
    for (int i=0; i<aFieldDataArray->getNumberOfComponents(); i++)
    {
        theField.clear();
        for (int j=0; j<aFieldDataArray->getNumberOfTuples(); j++)
        {
            aFieldDataArray->getTuple(j, bTuple.data());
            theField.push_back(bTuple[i]);
        }
        fieldname = aFieldDataArray->getInfoOnComponent(i);
        if (cg_field_write(index_file, index_base, index_zone, index_sol, RealDouble,
                           fieldname.c_str(), theField.data(), &index_field)) cg_error_exit();
    }
    //close file
    if (cg_close(index_file)) cg_error_exit();
    return;
}

#endif //WITH_CGNS

/*!
 * \brief export a Mesh to CSV format
 *  CSV format arrange data in several columns, which data are separate by comma,
 *  for our case, the first three columns are X, Y, Z, the last two are Tally and Error
 * \param aMesh mesh to be export
 * \param FileName File name
 */
void MCMESHTRAN::export2CSV(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName)    throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*FileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Write Mesh: " << aMesh->getName() << " to " << FileName);
    if (strcmp(aMesh->getType(), "EmptyMesh") == 0)
        THROW_SALOME_CORBA_EXCEPTION("Field is empty! Only possible to write field at this moment.", SALOME::BAD_PARAM);
    if (aMesh->getField()->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Write mesh failed: no field", SALOME::BAD_PARAM);

    ofstream TextFile;
    string delimiter = ",";
    //open the text file for output, discard the old content
    TextFile.open(FileName ,ios_base::trunc);
    if(!TextFile.is_open())
        THROW_SALOME_CORBA_EXCEPTION("Write mesh failed: could not open file ", SALOME::BAD_PARAM);
    TextFile << "X,Y,Z,Tally,Error"<<endl;

    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            = ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
    //2013-05-28 change to get data ON_NODES
//    if (aField_ptr->getTypeOfField() != ParaMEDMEM::ON_CELLS)
//        THROW_SALOME_CORBA_EXCEPTION("field are not on cell! ", SALOME::BAD_PARAM);
    if (aField_ptr->getTypeOfField() == ParaMEDMEM::ON_CELLS)
    {
        //get the berycenter of each cell
        try
        {
            //2013-05-24
            static_cast <ParaMEDMEM::MEDCouplingUMesh *> (const_cast <ParaMEDMEM::MEDCouplingMesh *>
                                                          ( aField_ptr->getMesh()))->orientCorrectlyPolyhedrons();
            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble > aBeryCenterArray
                    //                = aField_ptr->getMesh()->getBarycenterAndOwner();
                    //2013-05-23 a problem occured when using above function, the barycenter is not centroid
                    //the method is update using a function in SALOME7.2.0
                    //                =aField_ptr->getMesh()->computeIsoBarycenterOfNodesPerCell();
                    //2013-05-24 above computeIsoBarycenterOfNodesPerCell still getting the incorrect centroid
                    //indicate by this link, the getBarycenterAndOwner is the correct function:http://www.salome-platform.org/forum/forum_12/734743549
                    //                =aField_ptr->getMesh()->getBarycenterAndOwner();
                    //2013-05-27 change because the computeIsoBarycenterOfNodesPerCell using the equation: (x1+x2+...+xn)/n
                    =aField_ptr->getMesh()->computeIsoBarycenterOfNodesPerCell();

            double * aTuple = new double [3]; //for getting  coordinates x, y, z
            double * bTuple = new double [2]; //for getting tally and error

            if (aField_ptr->getArray()->getNumberOfComponents() != 2)
                THROW_SALOME_CORBA_EXCEPTION("abnormal in field components: not two components ", SALOME::BAD_PARAM);
            if (aField_ptr->getArray()->getNumberOfTuples() != aBeryCenterArray->getNumberOfTuples())
                THROW_SALOME_CORBA_EXCEPTION("abnormal in field array size: not consistent with cell Nb. ", SALOME::BAD_PARAM);


            for (int i=0; i< aField_ptr->getArray()->getNumberOfTuples(); i++)
            {
                aBeryCenterArray->getTuple(i, aTuple);
                aField_ptr->getArray()->getTuple(i, bTuple);
                TextFile << aTuple[0]<< delimiter << aTuple[1]<<delimiter <<aTuple[2]<<delimiter <<bTuple[0]<< delimiter <<bTuple[1]<<endl;
            }
            delete [] aTuple;
            delete [] bTuple;
        }
        catch (INTERP_KERNEL::Exception &ex)
        {
            THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
        }
    }
    else if (aField_ptr->getTypeOfField() == ParaMEDMEM::ON_NODES)
    {
        try
        {
            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble > aCoordArray
                    =aField_ptr->getMesh()->getCoordinatesAndOwner();
            double * aTuple = new double [3]; //for getting  coordinates x, y, z
            double * bTuple = new double [2]; //for getting tally and error
            if (aField_ptr->getArray()->getNumberOfComponents() != 2)
                THROW_SALOME_CORBA_EXCEPTION("abnormal in field components: not two components ", SALOME::BAD_PARAM);
            if (aField_ptr->getArray()->getNumberOfTuples() != aCoordArray->getNumberOfTuples())
                THROW_SALOME_CORBA_EXCEPTION("abnormal in field array size: not consistent with node Nb. ", SALOME::BAD_PARAM);

            for (int i=0; i< aField_ptr->getArray()->getNumberOfTuples(); i++)
            {
                aCoordArray->getTuple(i, aTuple);
                aField_ptr->getArray()->getTuple(i, bTuple);
                TextFile << aTuple[0]<< delimiter << aTuple[1]<<delimiter <<aTuple[2]<<delimiter <<bTuple[0]<< delimiter <<bTuple[1]<<endl;
            }
            delete [] aTuple;
            delete [] bTuple;
        }
        catch (INTERP_KERNEL::Exception &ex)
        {
            THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
        }

    }
    else
        THROW_SALOME_CORBA_EXCEPTION("Type of field is not supported! ", SALOME::BAD_PARAM);

}


/*!
 * \brief export to FLUENT in way of UDF file
 *  write the mesh as a FLUENT UDF file for FLUENT to compile
 *  the FLUENT calculation should based on the same mesh
 *  This method only support ON_CELL data, not for ON_NODE
 * \param aMesh the mesh to be exported
 * \param FileName UDF file name
 */
void MCMESHTRAN::export2FLUENT(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName) throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*FileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Write Mesh: " << aMesh->getName() << " to " << FileName);
    if (strcmp(aMesh->getType(), "EmptyMesh") == 0)
        THROW_SALOME_CORBA_EXCEPTION("Field is empty! ", SALOME::BAD_PARAM);
    if (aMesh->getField()->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Write mesh failed: no field", SALOME::BAD_PARAM);

    //##########get the data##############
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            = ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
    if (aField_ptr->getTypeOfField() != ParaMEDMEM::ON_CELLS)
        THROW_SALOME_CORBA_EXCEPTION("Data should be located on cell! ", SALOME::BAD_PARAM);
//    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble> aPointList
//            = ParaMEDMEM::DataArrayDouble::New();
//    aPointList->alloc(aField_ptr->getMesh()->getNumberOfCells(), 4 ); //a array to store X, Y, Z, Power density
//    aPointList->setInfoOnComponent(0, "X"); //
//    aPointList->setInfoOnComponent(1, "Y");
//    aPointList->setInfoOnComponent(2, "Z");
//    aPointList->setInfoOnComponent(3, "PowerDensity");
    vector <MCMESHTRAN_ORB::PointData > aPointList;
//    vector <double> aTmpPointList;
    //get the berycenter of each cell
    try   {
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble > aBeryCenterArray
//                = aField_ptr->getMesh()->getBarycenterAndOwner();
                //2013-05-27 change to obtain the right position for cell-center
                =aField_ptr->getMesh()->computeIsoBarycenterOfNodesPerCell();

        double * aTuple = new double [3]; //for getting  coordinates x, y, z
        double * bTuple = new double [2]; //for getting tally and error

        if (aField_ptr->getArray()->getNumberOfComponents() != 2)
            THROW_SALOME_CORBA_EXCEPTION("abnormal in field components: not two components ", SALOME::BAD_PARAM);

        for (int i=0; i< aField_ptr->getArray()->getNumberOfTuples(); i++)
        {
            aBeryCenterArray->getTuple(i, aTuple);
            aField_ptr->getArray()->getTuple(i, bTuple);
//            aTmpPointList.push_back(aTuple[0]); //X
//            aTmpPointList.push_back(aTuple[1]);  //Y
//            aTmpPointList.push_back(aTuple[2]); //Z
//            aTmpPointList.push_back(bTuple[0]); //Tally
            MCMESHTRAN_ORB::PointData aPointData;
            aPointData.X = aTuple[0];
            aPointData.Y = aTuple[1];
            aPointData.Z = aTuple[2];
            aPointData.PowerDensity = bTuple[0];
            aPointList.push_back(aPointData);
        }
        delete [] aTuple;
        delete [] bTuple;

    }
    catch (INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
    }

    //##########get the max and min value on X Y Z##############
    double minX=1E14, minY=1E14, minZ=1E14, maxX=-1E14, maxY=-1E14, maxZ=-1E14;
    for (int i=0; i<aPointList.size(); i++)
    {
        minX = min(minX, aPointList[i].X);
        minY = min(minY, aPointList[i].Y);
        minZ = min(minZ, aPointList[i].Z);
        maxX = max(maxX, aPointList[i].X);
        maxY = max(maxY, aPointList[i].Y);
        maxZ = max(maxZ, aPointList[i].Z);
    }

    //##########obtain the optimized box size##############
    // for X
    int XBucketNum;
    int YBucketNum;
    int ZBucketNum;
    double XRounding = 1.0E-7;
    double YRounding = 1.0E-7;
    double ZRounding = 1.0E-7;
    // for X
    while ( int((maxX - minX)*XRounding) < 10 )
        XRounding *= 10.0;
    XBucketNum = int((maxX - minX)*XRounding);
    // for Y
    while ( int((maxY - minY)*YRounding) < 10 )
        YRounding *= 10.0;
    YBucketNum = int((maxY - minY)*YRounding);
    // for Z
    while ( int((maxZ - minZ)*ZRounding) < 10 )
        ZRounding *= 10.0;
    ZBucketNum = int((maxZ - minZ)*ZRounding);

    XBucketNum += 1;//must +1, because the range from (int)maxX ~maxX should have storage space
    YBucketNum += 1;
    ZBucketNum += 1;

    //##########Put PointData into boxes according to their coordinates##############
    vector< vector< vector< vector< MCMESHTRAN_ORB::PointData> > > >aPointBox;
    // Set up sizes of data array.
    aPointBox.resize(XBucketNum);
    for (int i = 0; i<XBucketNum; i++)
    {
        aPointBox[i].resize(YBucketNum);
        for (int j = 0; j<YBucketNum; j++)
        {
            aPointBox[i][j].resize(ZBucketNum);
        }
    }
    //put data into the box
    for(int i = 0; i < aPointList.size() ; i++) {
        int XBucketNo = int((aPointList[i].X - minX) * XRounding);
        int YBucketNo = int((aPointList[i].Y - minY) * YRounding);
        int ZBucketNo = int((aPointList[i].Z - minZ) * ZRounding);
        aPointBox[XBucketNo ][YBucketNo ][ZBucketNo ].push_back(aPointList[i]);

        //add a overlap layer, in case centroid calculated in FLUENT outside the Box which should belong
        int XBucketNoLow,XBucketNoHigh, YBucketNoLow,YBucketNoHigh, ZBucketNoLow,ZBucketNoHigh; // low, high boundary,
        XBucketNoLow = int((aPointList[i].X - minX  ) * XRounding - 0.02);
        XBucketNoHigh = int((aPointList[i].X - minX ) * XRounding + 0.02);
        YBucketNoLow = int((aPointList[i].Y - minY  ) * YRounding - 0.02);
        YBucketNoHigh = int((aPointList[i].Y - minY ) * YRounding + 0.02);
        ZBucketNoLow = int((aPointList[i].Z - minZ  ) * ZRounding - 0.02);
        ZBucketNoHigh = int((aPointList[i].Z - minZ ) * ZRounding + 0.02);
        int XLowChg, XHighChg,YLowChg, YHighChg,ZLowChg, ZHighChg;
        XLowChg = XBucketNoLow - XBucketNo; // possible -1, 0, 1
        XHighChg = XBucketNoHigh - XBucketNo;
        YLowChg = YBucketNoLow - YBucketNo;
        YHighChg = YBucketNoHigh - YBucketNo;
        ZLowChg = ZBucketNoLow - ZBucketNo;
        ZHighChg = ZBucketNoHigh - ZBucketNo;
        int XBucketChgNo, YBucketChgNo, ZBucketChgNo;
        // if changed
        if (XLowChg != 0)
            XBucketChgNo = XBucketNoLow;
        else if (XHighChg != 0 )
            XBucketChgNo = XBucketNoHigh;
        else //not changed
            XBucketChgNo = XBucketNo;

        if (YLowChg != 0)
            YBucketChgNo = YBucketNoLow;
        else if (YHighChg != 0 )
            YBucketChgNo = YBucketNoHigh;
        else
            YBucketChgNo = YBucketNo;

        if (ZLowChg != 0)
            ZBucketChgNo = ZBucketNoLow;
        else if (ZHighChg != 0 )
            ZBucketChgNo = ZBucketNoHigh;
        else
            ZBucketChgNo = ZBucketNo;

//        cout << "X= " <<aPointList[i].X << "\tY= " <<aPointList[i].Y << "\tZ= " <<aPointList[i].Z
//             << "\tXBucketNo= " << XBucketNo << "\tYBucketNo= "<< YBucketNo << "\tZBucketNo= "<< ZBucketNo
//             << "\tXBucketChgNo= " << XBucketChgNo << "\tYBucketChgNo= "<< YBucketChgNo  << "\tZBucketChgNo= "<< ZBucketChgNo <<endl;

        if (       (XBucketChgNo != XBucketNo && XBucketChgNo != -1 && XBucketChgNo < XBucketNum)
                || (YBucketChgNo != YBucketNo && YBucketChgNo != -1 && YBucketChgNo < YBucketNum)
                || (ZBucketChgNo != ZBucketNo && ZBucketChgNo != -1 && ZBucketChgNo < ZBucketNum))
            aPointBox[ XBucketChgNo][ YBucketChgNo][ZBucketChgNo ].push_back(aPointList[i]);
    }

    //##########print the UDF file #############
    ofstream aUDFFile;
    string delimiter = ",";
    //open the text file for output, discard the old content
    aUDFFile.open(FileName ,ios_base::trunc);
    if(!aUDFFile.is_open())
        THROW_SALOME_CORBA_EXCEPTION("Write FLUENT UDF failed: could not open file ", SALOME::BAD_PARAM);
    //output headers
    aUDFFile << "#include\"udf.h\"" <<endl;
    aUDFFile << "#include<stdio.h>" <<endl;
    aUDFFile << "#include<math.h>" <<endl;
    //output pre parameters
    aUDFFile << "double XRounding="<<XRounding<<";"<<endl;
    aUDFFile << "double YRounding="<<YRounding<<";"<<endl;
    aUDFFile << "double ZRounding="<<ZRounding<<";"<<endl;
    aUDFFile << "int XBucketNum="<<XBucketNum<<";"<<endl;
    aUDFFile << "int YBucketNum="<<YBucketNum<<";"<<endl;
    aUDFFile << "int ZBucketNum="<<ZBucketNum<<";"<<endl;
    aUDFFile << "double minX="<<minX<<";"<<endl;
    aUDFFile << "double minY="<<minY<<";"<<endl;
    aUDFFile << "double minZ="<<minZ<<";"<<endl;
    aUDFFile << "double deviation1 = 1e-7;" <<endl;
    aUDFFile << "double deviation2 = 1e-6;" <<endl;
    aUDFFile << "double deviation3 = 1e-5;" <<endl;
    aUDFFile << "real lastVal = 0;" <<endl;
    aUDFFile << "int i, aIndex, aSize;" <<endl;
    aUDFFile << "double XYZ[ND_ND];" <<endl;
    aUDFFile << "int XBucketNo, YBucketNo, ZBucketNo;" <<endl;
    //print size array-> record the points put in the box
    aUDFFile << "int DataSize[" <<XBucketNum<<"] [" <<YBucketNum <<"] [" <<ZBucketNum<<"] = {"<<endl;
    for (int i = 0; i<XBucketNum; i++)
    {
        for (int j = 0; j<YBucketNum; j++)
        {
            for (int k = 0; k<ZBucketNum; k++)
            {
                aUDFFile << aPointBox[i][j][k].size()<<delimiter;
            }
            aUDFFile <<endl;
        }
    }
    aUDFFile << "        };"<<endl;

    //print index array-> index for where the data begins. should *4 in DATA array
    long long int count=0;
    aUDFFile << "int DataIndex[" <<XBucketNum<<"] [" <<YBucketNum <<"] [" <<ZBucketNum<<"] = {"<<endl;
    for (int i = 0; i<XBucketNum; i++)
    {
        for (int j = 0; j<YBucketNum; j++)
        {
            for (int k = 0; k<ZBucketNum; k++)
            {
                aUDFFile << count<<delimiter;
                count += aPointBox[i][j][k].size();
            }
            aUDFFile <<endl;
        }
    }
    aUDFFile << "        };"<<endl;

    //print data array
    aUDFFile << "double Data[" << count * 4 <<"] = {"<<endl;
    for (int i = 0; i<XBucketNum; i++)
    {
        for (int j = 0; j<YBucketNum; j++)
        {
            for (int k = 0; k<ZBucketNum; k++)
            {
                for (int m = 0; m<aPointBox[i][j][k].size(); m++)
                {
                    aUDFFile << setprecision(8) << aPointBox[i][j][k][m].X <<delimiter
                                <<aPointBox[i][j][k][m].Y<<delimiter
                                  <<aPointBox[i][j][k][m].Z<<delimiter
                                    <<aPointBox[i][j][k][m].PowerDensity<<delimiter<<endl;
                }
            }
        }
    }
    aUDFFile << "        };"<<endl;

    aUDFFile << "DEFINE_SOURCE(T_SOURCE, c, t_cell, dS, eqn)                        " <<endl;
    aUDFFile << "{                                                                  " <<endl;
    aUDFFile << "    C_CENTROID(XYZ, c, t_cell);                                    " <<endl;
    aUDFFile << "    XBucketNo =(int)(( XYZ[0] - minX) * XRounding);                " <<endl;
    aUDFFile << "    YBucketNo =(int)(( XYZ[1] - minY) * YRounding);                " <<endl;
    aUDFFile << "    ZBucketNo =(int)(( XYZ[2] - minZ) * ZRounding);                " <<endl;
    aUDFFile << "    aIndex = DataIndex[XBucketNo][YBucketNo][ZBucketNo] * 4;       " <<endl;
    aUDFFile << "    aSize = DataSize[XBucketNo][YBucketNo][ZBucketNo];             " <<endl;
    aUDFFile << "                                                                   " <<endl;
    aUDFFile << "    for (i=0; i<aSize; i++)                                        " <<endl;
    aUDFFile << "    {                                                              " <<endl;
    aUDFFile << "        if(( fabs( Data[aIndex + i*4] -XYZ[0] ) < deviation1)      " <<endl;
    aUDFFile << "            &&(fabs(Data[aIndex + i*4 +1]-XYZ[1])<deviation1)      " <<endl;
    aUDFFile << "            &&(fabs(Data[aIndex + i*4 +2]-XYZ[2])<deviation1))     " <<endl;
    aUDFFile << "            {                                                      " <<endl;
    aUDFFile << "       lastVal =  Data[aIndex + i*4 +3];                           " <<endl;
    aUDFFile << "               return Data[aIndex + i*4 +3];                       " <<endl;
    aUDFFile << "             }                                                     " <<endl;
    aUDFFile << "    }                                                              " <<endl;
    aUDFFile << "    for (i=0; i<aSize; i++)                                        " <<endl;
    aUDFFile << "    {                                                              " <<endl;
    aUDFFile << "        if(( fabs( Data[aIndex + i*4] -XYZ[0] ) < deviation2)      " <<endl;
    aUDFFile << "            &&(fabs(Data[aIndex + i*4 +1]-XYZ[1])<deviation2)      " <<endl;
    aUDFFile << "            &&(fabs(Data[aIndex + i*4 +2]-XYZ[2])<deviation2))     " <<endl;
    aUDFFile << "            {                                                      " <<endl;
    aUDFFile << "       lastVal =  Data[aIndex + i*4 +3];                           " <<endl;
    aUDFFile << "               return Data[aIndex + i*4 +3];                       " <<endl;
    aUDFFile << "             }                                                     " <<endl;
    aUDFFile << "    }                                                              " <<endl;
    aUDFFile << "    for (i=0; i<aSize; i++)                                        " <<endl;
    aUDFFile << "    {                                                              " <<endl;
    aUDFFile << "        if(( fabs( Data[aIndex + i*4] -XYZ[0] ) < deviation3)      " <<endl;
    aUDFFile << "            &&(fabs(Data[aIndex + i*4 +1]-XYZ[1])<deviation3)      " <<endl;
    aUDFFile << "            &&(fabs(Data[aIndex + i*4 +2]-XYZ[2])<deviation3))     " <<endl;
    aUDFFile << "            {                                                      " <<endl;
    aUDFFile << "       lastVal =  Data[aIndex + i*4 +3];                           " <<endl;
    aUDFFile << "               return Data[aIndex + i*4 +3];                       " <<endl;
    aUDFFile << "             }                                                     " <<endl;
    aUDFFile << "    }                                                              " <<endl;
    aUDFFile << " return lastVal;                                                   " <<endl;
    aUDFFile << "}                                                                  " <<endl;

}

int getDigit(int aInt) //return the digit of an integer
{
	//2015-08-31 add (double) to convert the number
        return aInt == 0 ? 1:floor(log10((double)aInt)) +1;
}

/*!
 * \brief export to CFX in way of User Fortran file
 *  write the mesh as a User fortran file for CFX to compile
 *  the CFX calculation should based on the same mesh
 *  This method only support ON_NODE data, not for  ON_CELL
 * \param aMesh the mesh to be exported
 * \param FileName UDF file name
 */
void  MCMESHTRAN::export2CFX(MCMESHTRAN_ORB::Mesh_ptr aMesh, const char* FileName)    throw (SALOME::SALOME_Exception)
{
    if (aMesh->_is_nil() )
        THROW_SALOME_CORBA_EXCEPTION("Mesh is nil!", SALOME::BAD_PARAM);
    if (*FileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Write Mesh: " << aMesh->getName() << " to " << FileName);
    if (strcmp(aMesh->getType(), "EmptyMesh") == 0)
        THROW_SALOME_CORBA_EXCEPTION("Field is empty! ", SALOME::BAD_PARAM);
    if (aMesh->getField()->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION("Write mesh failed: no field", SALOME::BAD_PARAM);

    //##########get the data##############
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            = ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
//    if (aField_ptr->getTypeOfField() != ParaMEDMEM::ON_CELLS)
    if (aField_ptr->getTypeOfField() != ParaMEDMEM::ON_NODES)  //30-08-2013 the CFX requires data to be on nodes
        THROW_SALOME_CORBA_EXCEPTION("data should be on nodes! ", SALOME::BAD_PARAM);
    vector <MCMESHTRAN_ORB::PointData > aPointList;
    //get the berycenter of each cell
    try   {
//        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble > aBeryCenterArray
//                =aField_ptr->getMesh()->computeIsoBarycenterOfNodesPerCell();
        //30-08-2013
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::DataArrayDouble > aVertiesArray
                =aField_ptr->getMesh()->getCoordinatesAndOwner();

        double * aTuple = new double [3]; //for getting  coordinates x, y, z
        double * bTuple = new double [2]; //for getting tally and error

        if (aField_ptr->getArray()->getNumberOfComponents() != 2)
            THROW_SALOME_CORBA_EXCEPTION("abnormal in field components: not two components ", SALOME::BAD_PARAM);

        for (int i=0; i< aField_ptr->getArray()->getNumberOfTuples(); i++)
        {
            aVertiesArray->getTuple(i, aTuple);
            aField_ptr->getArray()->getTuple(i, bTuple);
            MCMESHTRAN_ORB::PointData aPointData;
            aPointData.X = aTuple[0];
            aPointData.Y = aTuple[1];
            aPointData.Z = aTuple[2];
            aPointData.PowerDensity = bTuple[0];
            aPointList.push_back(aPointData);
        }
        delete [] aTuple;
        delete [] bTuple;

    }
    catch (INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
    }

    //##########get the max and min value on X Y Z##############
    double minX=1E14, minY=1E14, minZ=1E14, maxX=-1E14, maxY=-1E14, maxZ=-1E14;
    for (int i=0; i<aPointList.size(); i++)
    {
        minX = min(minX, aPointList[i].X);
        minY = min(minY, aPointList[i].Y);
        minZ = min(minZ, aPointList[i].Z);
        maxX = max(maxX, aPointList[i].X);
        maxY = max(maxY, aPointList[i].Y);
        maxZ = max(maxZ, aPointList[i].Z);
    }

    //##########obtain the optimized box size##############
    // for X
    int XBucketNum;
    int YBucketNum;
    int ZBucketNum;
    double XRounding = 1.0E-7;
    double YRounding = 1.0E-7;
    double ZRounding = 1.0E-7;
    // for X
    while ( int((maxX - minX)*XRounding) < 10 )
        XRounding *= 10.0;
    XBucketNum = int((maxX - minX)*XRounding);
    // for Y
    while ( int((maxY - minY)*YRounding) < 10 )
        YRounding *= 10.0;
    YBucketNum = int((maxY - minY)*YRounding);
    // for Z
    while ( int((maxZ - minZ)*ZRounding) < 10 )
        ZRounding *= 10.0;
    ZBucketNum = int((maxZ - minZ)*ZRounding);

    XBucketNum += 1;//must +1, because the range from (int)maxX ~maxX should have storage space
    YBucketNum += 1;
    ZBucketNum += 1;

    //##########Put PointData into boxes according to their coordinates##############
    vector< vector< vector< vector< MCMESHTRAN_ORB::PointData> > > >aPointBox;
    // Set up sizes of data array.
    aPointBox.resize(XBucketNum);
    for (int i = 0; i<XBucketNum; i++)
    {
        aPointBox[i].resize(YBucketNum);
        for (int j = 0; j<YBucketNum; j++)
        {
            aPointBox[i][j].resize(ZBucketNum);
        }
    }
    //put data into the box
    for(int i = 0; i < aPointList.size() ; i++) {
        int XBucketNo = int((aPointList[i].X - minX) * XRounding);
        int YBucketNo = int((aPointList[i].Y - minY) * YRounding);
        int ZBucketNo = int((aPointList[i].Z - minZ) * ZRounding);
        aPointBox[XBucketNo ][YBucketNo ][ZBucketNo ].push_back(aPointList[i]);

        //add a overlap layer, in case centroid calculated in FLUENT outside the Box which should belong
        int XBucketNoLow,XBucketNoHigh, YBucketNoLow,YBucketNoHigh, ZBucketNoLow,ZBucketNoHigh; // low, high boundary,
        XBucketNoLow = int((aPointList[i].X - minX  ) * XRounding - 0.02);
        XBucketNoHigh = int((aPointList[i].X - minX ) * XRounding + 0.02);
        YBucketNoLow = int((aPointList[i].Y - minY  ) * YRounding - 0.02);
        YBucketNoHigh = int((aPointList[i].Y - minY ) * YRounding + 0.02);
        ZBucketNoLow = int((aPointList[i].Z - minZ  ) * ZRounding - 0.02);
        ZBucketNoHigh = int((aPointList[i].Z - minZ ) * ZRounding + 0.02);
        int XLowChg, XHighChg,YLowChg, YHighChg,ZLowChg, ZHighChg;
        XLowChg = XBucketNoLow - XBucketNo; // possible -1, 0, 1
        XHighChg = XBucketNoHigh - XBucketNo;
        YLowChg = YBucketNoLow - YBucketNo;
        YHighChg = YBucketNoHigh - YBucketNo;
        ZLowChg = ZBucketNoLow - ZBucketNo;
        ZHighChg = ZBucketNoHigh - ZBucketNo;
        int XBucketChgNo, YBucketChgNo, ZBucketChgNo;
        // if changed
        if (XLowChg != 0)
            XBucketChgNo = XBucketNoLow;
        else if (XHighChg != 0 )
            XBucketChgNo = XBucketNoHigh;
        else //not changed
            XBucketChgNo = XBucketNo;

        if (YLowChg != 0)
            YBucketChgNo = YBucketNoLow;
        else if (YHighChg != 0 )
            YBucketChgNo = YBucketNoHigh;
        else
            YBucketChgNo = YBucketNo;

        if (ZLowChg != 0)
            ZBucketChgNo = ZBucketNoLow;
        else if (ZHighChg != 0 )
            ZBucketChgNo = ZBucketNoHigh;
        else
            ZBucketChgNo = ZBucketNo;

//        cout << "X= " <<aPointList[i].X << "\tY= " <<aPointList[i].Y << "\tZ= " <<aPointList[i].Z
//             << "\tXBucketNo= " << XBucketNo << "\tYBucketNo= "<< YBucketNo << "\tZBucketNo= "<< ZBucketNo
//             << "\tXBucketChgNo= " << XBucketChgNo << "\tYBucketChgNo= "<< YBucketChgNo  << "\tZBucketChgNo= "<< ZBucketChgNo <<endl;

        if (       (XBucketChgNo != XBucketNo && XBucketChgNo != -1 && XBucketChgNo < XBucketNum)
                || (YBucketChgNo != YBucketNo && YBucketChgNo != -1 && YBucketChgNo < YBucketNum)
                || (ZBucketChgNo != ZBucketNo && ZBucketChgNo != -1 && ZBucketChgNo < ZBucketNum))
            aPointBox[ XBucketChgNo][ YBucketChgNo][ZBucketChgNo ].push_back(aPointList[i]);
    }

    //##########print the User Fortran file #############
    ofstream aUCFFile;
    string delimiter = ",";
    //open the text file for output, discard the old content
    aUCFFile.open(FileName ,ios_base::trunc);
    if(!aUCFFile.is_open())
        THROW_SALOME_CORBA_EXCEPTION("Write FLUENT UDF failed: could not open file ", SALOME::BAD_PARAM);

    aUCFFile << "#include \"cfx5ext.h\"                                            " << endl;
    aUCFFile << "dllexport(user_source)                                            " << endl;
    aUCFFile << "      SUBROUTINE USER_SOURCE (                                    " << endl;
    aUCFFile << "     &NLOC,NRET,NARG,RET,ARGS,CRESLT,CZ,DZ,IZ,LZ,RZ)              " << endl;
    aUCFFile << "      INTEGER NLOC, NRET, NARG                                    " << endl;
    aUCFFile << "      CHARACTER*4 CRESLT                                          " << endl;
    aUCFFile << "      REAL    RET(1:NLOC,1:NRET), ARGS(1:NLOC,1:NARG)             " << endl;
    aUCFFile << "      INTEGER IZ(*)                                               " << endl;
    aUCFFile << "      CHARACTER CZ(*)*(1)                                         " << endl;
    aUCFFile << "      DOUBLE PRECISION DZ(*)                                      " << endl;
    aUCFFile << "      LOGICAL LZ(*)                                               " << endl;
    aUCFFile << "      REAL RZ(*)                                                  " << endl;
    aUCFFile << "      CALL SET_A_0( RET, NLOC*NRET )                              " << endl;
    aUCFFile << "      CALL USER_SOURCE_SUB (NLOC,RET(1,1),ARGS(1,1),              " << endl;
    aUCFFile << "     &ARGS(1,2), ARGS(1,3))                                       " << endl;
    aUCFFile << "      CRESLT = 'GOOD'                                             " << endl;
    aUCFFile << "      END                                                         " << endl;
    aUCFFile << "C                                                                 " << endl;
    aUCFFile << "      SUBROUTINE USER_SOURCE_SUB (NLOC,SOURCE,X,Y,Z)              " << endl;
    aUCFFile << "      INTEGER NLOC, ILOC                                          " << endl;
    aUCFFile << "      REAL    SOURCE(NLOC), X(NLOC), Y(NLOC), Z(NLOC)             " << endl;
    aUCFFile << "      REAL  XRNDG, YRNDG, ZRNDG                                   " << endl;
    aUCFFile << "      DATA  XRNDG, YRNDG, ZRNDG/" <<XRounding<<", "<< YRounding<<", "<<ZRounding<<"/" <<endl;
    aUCFFile << "      INTEGER XBKNM, YBKNM, ZBKNM                                 " << endl;
    aUCFFile << "      DATA XBKNM, YBKNM, ZBKNM/" <<XBucketNum<<", "<< YBucketNum<<", "<<ZBucketNum<<"/" <<endl;
    aUCFFile << "      REAL  MINX, MINY, MINZ                                      " << endl;
    aUCFFile << "      DATA  MINX, MINY, MINZ/" <<minX<<", "<< minY<<", "<<minZ<<"/" <<endl;
    aUCFFile << "      REAL  DEV1, DEV2, DEV3                                      " << endl;
    aUCFFile << "      DATA  DEV1, DEV2, DEV3/1E-7, 1E-6, 1E-5/                    " << endl;
    aUCFFile << "      REAL  LSVAL                                                 " << endl;
    aUCFFile << "      DATA  LSVAL/0.0/                                            " << endl;
    aUCFFile << "      INTEGER*8 I                                                  " << endl;
    aUCFFile << "      INTEGER J, K, AINDEX, ASIZE                                 " << endl;
    aUCFFile << "      INTEGER XBKNO, YBKNO, ZBKNO                                 " << endl;
    aUCFFile << "      INTEGER DATSIZ(" <<XBucketNum<<", "<< YBucketNum<<", "<<ZBucketNum<<")" <<endl;
    aUCFFile << "      INTEGER DATIDX(" <<XBucketNum<<", "<< YBucketNum<<", "<<ZBucketNum<<")" <<endl;
//    aUCFFile << "      REAL    DATARR(3499324)                                     " << endl;

    //print the initialization of the arraies .
    //because the limitation of the column(72) and continue lines
    int ColumnLimit = 55;
    int LineLimit = 50;
    int ColumnCnt = 0;
    int LineCnt = 0;
    //print the data size array
    for (int i = 0; i<XBucketNum; i++)
    {
        for (int j = 0; j<YBucketNum; j++)
        {
            aUCFFile << "      DATA(((DATSIZ(I,J,K),K=1,"<<ZBucketNum<<"),J="<<j+1<<","<<j+1<<"),I="<<i+1<<","<<i+1<<")/"<<endl;
            aUCFFile <<"     &";
            for (int k = 0; k<ZBucketNum; k++)
            {
                ColumnCnt += getDigit(aPointBox[i][j][k].size()) + 1;  //accumulate the colume
                if (ColumnCnt > ColumnLimit)
                {
                    aUCFFile <<endl<<"     &";
                    ColumnCnt = 0;
                }
                aUCFFile << aPointBox[i][j][k].size();
                if (k < ZBucketNum -1)
                    aUCFFile <<delimiter;
            }
            aUCFFile <<"/"<<endl;
            ColumnCnt = 0;
        }
    }
    //print the data index array
    long long int count=1;  // initialize with 1 because fortran start the index at 1
    for (int i = 0; i<XBucketNum; i++)
    {
        for (int j = 0; j<YBucketNum; j++)
        {
            aUCFFile << "      DATA(((DATIDX(I,J,K),K=1,"<<ZBucketNum<<"),J="<<j+1<<","<<j+1<<"),I="<<i+1<<","<<i+1<<")/"<<endl;
            aUCFFile <<"     &";
            for (int k = 0; k<ZBucketNum; k++)
            {
                ColumnCnt += getDigit(count) + 1;  //accumulate the colume
                if (ColumnCnt > ColumnLimit)
                {
                    aUCFFile <<endl<<"     &";
                    ColumnCnt = 0;
                }
                aUCFFile << count;
                if (k < ZBucketNum -1)
                    aUCFFile <<delimiter;
                count += aPointBox[i][j][k].size();
            }
            aUCFFile <<"/"<<endl;
            ColumnCnt = 0;
        }
    }
    //print the data array
    long long int totalPnt = count-1;
    count = 0;
    aUCFFile << "      REAL    DATARR("<<(totalPnt)*4<<")" << endl;
    for (int i = 0; i<XBucketNum; i++)
    {
        for (int j = 0; j<YBucketNum; j++)
        {
            for (int k = 0; k<ZBucketNum; k++)
            {
                for (int m = 0; m<aPointBox[i][j][k].size(); m++)
                {
                    if (LineCnt == 0)
                    {
                        if ((totalPnt - count) >= LineLimit )
                            aUCFFile << "      DATA( DATARR (I) , I="<< count*4 +1 <<", "<< (count+LineLimit )*4 <<" ) /"<<endl;
                        else
                            aUCFFile << "      DATA( DATARR (I) , I="<< count*4 +1 <<", "<< totalPnt *4<< ") /"<<endl;

                    }
                    aUCFFile <<"     &";
                    aUCFFile << setprecision(8) << aPointBox[i][j][k][m].X <<delimiter
                                <<aPointBox[i][j][k][m].Y<<delimiter
                                  <<aPointBox[i][j][k][m].Z<<delimiter<<aPointBox[i][j][k][m].PowerDensity;
                    LineCnt ++;  //should ++ here because it will set 0 below.
                    count ++;
                    if (LineCnt != LineLimit && count != totalPnt)
                    {
                        aUCFFile<<delimiter<<endl;
                    }
                    else  //if the in last line, output the / mark and no deliminter
                    {
                        aUCFFile<<"/"<<endl;
                        LineCnt = 0;
                    }
                }
            }
        }
    }

    aUCFFile << "C                                                                 " << endl;
    aUCFFile << "      DO ILOC=1,NLOC                                              " << endl;
    aUCFFile << "        SOURCE(ILOC) = 0.0                                        " << endl;
    aUCFFile << "        XBKNO =INT(( X(ILOC) - MINX) * XRNDG) +1                  " << endl;  // Fluent start the index at 1
    aUCFFile << "        YBKNO =INT(( Y(ILOC) - MINY) * YRNDG) +1                  " << endl;
    aUCFFile << "        ZBKNO =INT(( Z(ILOC) - MINZ) * ZRNDG) +1                  " << endl;
    aUCFFile << "        AINDEX = DATIDX(XBKNO,YBKNO,ZBKNO) * 4 -3                 " << endl;
    aUCFFile << "        ASIZE = DATSIZ(XBKNO,YBKNO,ZBKNO)                         " << endl;
    aUCFFile << "                                                                  " << endl;
    aUCFFile << "        DO I=0,ASIZE-1                                            " << endl;
    aUCFFile << "            IF(( ABS( DATARR(AINDEX + I*4)-X(ILOC)) .LE. DEV1)    " << endl;
    aUCFFile << "     &.AND. ( ABS( DATARR(AINDEX + I*4+1)-Y(ILOC)) .LE. DEV1)     " << endl;
    aUCFFile << "     &.AND. ( ABS( DATARR(AINDEX + I*4+2)-Z(ILOC)) .LE. DEV1))    " << endl;
    aUCFFile << "     &      THEN                                                  " << endl;
    aUCFFile << "            LSVAL =  DATARR(AINDEX + I*4+3)                       " << endl;
    aUCFFile << "            SOURCE(ILOC) = DATARR(AINDEX + I*4+3)                 " << endl;
    aUCFFile << "            GOTO 999                                              " << endl;
    aUCFFile << "            END IF                                                " << endl;
    aUCFFile << "        END DO                                                    " << endl;
    aUCFFile << "        DO I=0,ASIZE-1                                            " << endl;
    aUCFFile << "            IF(( ABS( DATARR(AINDEX + I*4)-X(ILOC)) .LE. DEV2)    " << endl;
    aUCFFile << "     &.AND. ( ABS( DATARR(AINDEX + I*4+1)-Y(ILOC)) .LE. DEV2)     " << endl;
    aUCFFile << "     &.AND. ( ABS( DATARR(AINDEX + I*4+2)-Z(ILOC)) .LE. DEV2))    " << endl;
    aUCFFile << "     &      THEN                                                  " << endl;
    aUCFFile << "            LSVAL =  DATARR(AINDEX + I*4+3)                       " << endl;
    aUCFFile << "            SOURCE(ILOC) = DATARR(AINDEX + I*4+3)                 " << endl;
    aUCFFile << "            GOTO 999                                              " << endl;
    aUCFFile << "            END IF                                                " << endl;
    aUCFFile << "        END DO                                                    " << endl;
    aUCFFile << "        DO I=0,ASIZE-1                                            " << endl;
    aUCFFile << "            IF(( ABS( DATARR(AINDEX + I*4)-X(ILOC)) .LE. DEV3)    " << endl;
    aUCFFile << "     &.AND. ( ABS( DATARR(AINDEX + I*4+1)-Y(ILOC)) .LE. DEV3)     " << endl;
    aUCFFile << "     &.AND. ( ABS( DATARR(AINDEX + I*4+2)-Z(ILOC)) .LE. DEV3))    " << endl;
    aUCFFile << "     &      THEN                                                  " << endl;
    aUCFFile << "            LSVAL =  DATARR(AINDEX + I*4+3)                       " << endl;
    aUCFFile << "            SOURCE(ILOC) = DATARR(AINDEX + I*4+3)                 " << endl;
    aUCFFile << "            GOTO 999                                              " << endl;
    aUCFFile << "            END IF                                                " << endl;
    aUCFFile << "        END DO                                                    " << endl;
    aUCFFile << "        SOURCE(ILOC) =  LSVAL                                     " << endl;
    aUCFFile << "999   CONTINUE                                                    " << endl;
    aUCFFile << "C         print *,ILOC,'X',X(ILOC),'Y',Y(ILOC),'Z',               " << endl;
    aUCFFile << "C     &Z(ILOC),'S',SOURCE(ILOC)                                   " << endl;
    aUCFFile << "      END DO                                                      " << endl;
    aUCFFile << "      END                                                         " << endl;


}


/*!
 * \brief import a MED mesh with or without field
 * \param MEDFileName external MED file name
 * \return a new mesh
 * \see http://docs.salome-platform.org/salome_6_6_0/gui/MED/classMEDLoader.html
 * \todo only read field ON CELL, not ON NODE. Only 3D, not 2D
 */
MCMESHTRAN_ORB::MeshGroup_ptr  MCMESHTRAN::importMED(const char* MEDFileName)  throw (SALOME::SALOME_Exception)
{
    if (*MEDFileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Import Meshes from: " << MEDFileName);
    //new group for new meshes
    QStringList atmpList = QString(MEDFileName).split('/');
    atmpList = atmpList.at(atmpList.size()-1).split('.');
    MeshGroup * aGroup = new MeshGroup (atmpList.at(0).toLatin1(), genId());

    //######################
    //a MED file might contain more than one Mesh (at least one),
    //a Mesh might contain more than one field, or no field
    try
    {
        //first check the file
        MEDLoader::CheckFileForRead(MEDFileName);
        //get the mesh name
        vector <string> MeshNames = MEDLoader::GetMeshNames(MEDFileName);
        for(int i=0; i<MeshNames.size(); i++)
        {
            //get the field name
            vector <string> FieldNames = MEDLoader::GetFieldNamesOnMesh(ParaMEDMEM::ON_CELLS, MEDFileName, MeshNames[i].c_str());

            for (int j=0; j<FieldNames.size(); j++)
            {
                ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble>  newField_ptr;

//                newField_ptr = MEDLoader::ReadFieldCell(MEDFileName,MeshNames[i].c_str(),/*meshDimRelToMax*/0,  //see http://docs.salome-platform.org/salome_6_6_0/gui/MED/MEDLoaderAdvancedAPIPage.html
//                                                        FieldNames[j].c_str(), /*iteration*/ -1 , /*order*/-1  ); //iteration and order refer to time, no time step should assign -1, see: http://docs.salome-platform.org/salome_6_6_0/gui/MED/MEDLoaderBasicAPIPage.html
                ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDFileField1TS> aNewFileField
                        = ParaMEDMEM::MEDFileField1TS::New(MEDFileName,FieldNames[j].c_str(), -1, -1 );
                if (aNewFileField->getTypesOfFieldAvailable()[0] == ParaMEDMEM::ON_CELLS)
                {
                    newField_ptr = aNewFileField->getFieldAtLevel(ParaMEDMEM::ON_CELLS,0);
                    newField_ptr->setNature(ParaMEDMEM::RevIntegral);
                }
                else if (aNewFileField->getTypesOfFieldAvailable()[0] == ParaMEDMEM::ON_NODES)
                {
                    newField_ptr = aNewFileField->getFieldAtLevel(ParaMEDMEM::ON_NODES,0);
                    newField_ptr->setNature(ParaMEDMEM::ConservativeVolumic);
                }

                if (!newField_ptr)
                    THROW_SALOME_CORBA_EXCEPTION("get field error! should without time steps!", SALOME::BAD_PARAM);
                newField_ptr->setName(FieldNames[j].c_str());
//                newField_ptr->setNature(ParaMEDMEM::RevIntegral);
                if (newField_ptr->getMesh()->getType() != ParaMEDMEM::UNSTRUCTURED)
                    THROW_SALOME_CORBA_EXCEPTION("Not a Unstructured mesh!", SALOME::BAD_PARAM);
                if (newField_ptr->getArray()->getNumberOfComponents() != 2)
                    THROW_SALOME_CORBA_EXCEPTION("Not contain field of Tally and Error!", SALOME::BAD_PARAM);

                //create field IOR
                ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(newField_ptr);
                SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();
                //create new Mesh and append to the group
                Mesh * aNewMesh = new Mesh (FieldNames[j].c_str(), genId(), "ExtMesh", /*NPS*/ 0);
                aNewMesh->setField(myFieldIOR);
                aGroup->appendMesh(aNewMesh->_this());
            }
            //if no field, create a empty field
            if (FieldNames.size() == 0)
            {
                ParaMEDMEM::MEDCouplingFieldTemplate* newField_ptr =
                        ParaMEDMEM::MEDCouplingFieldTemplate::New(ParaMEDMEM::ON_CELLS);
                newField_ptr->setName(MeshNames[i].c_str());
//                newField_ptr->setNature(ParaMEDMEM::RevIntegral);  //comment because not suitable for ON_NODES
//                newField_ptr->setMesh(MEDLoader::ReadUMeshFromFile(MEDFileName, 0) );

                //using MEDloader Advance API for importing mesh
                ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDFileUMesh>  aNewFileUMesh
                        = ParaMEDMEM::MEDFileUMesh::New(MEDFileName);
                if (aNewFileUMesh ->getMeshDimension() != 3 ) //if not 3D mesh
                    THROW_SALOME_CORBA_EXCEPTION("Not a 3D mesh!", SALOME::BAD_PARAM);
                ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDCouplingUMesh> aNewUMesh
                        = aNewFileUMesh ->getMeshAtLevel(0);
                newField_ptr->setMesh(aNewUMesh);

//                //for test
//                ParaMEDMEM::DataArrayInt * atestCONN = aNewUMesh->getNodalConnectivity();
//                ParaMEDMEM::DataArrayInt * atestCONNI = aNewUMesh->getNodalConnectivityIndex();
//                int begin, end;
////                atestCONNI->getTuple(atestCONNI->getNumberOfTuples()-2, &begin);
////                atestCONNI->getTuple(atestCONNI->getNumberOfTuples()-1, &end);
//                atestCONNI->getTuple(0, &begin);
//                atestCONNI->getTuple(1, &end);

//                for (int k=begin; k<end; k++ )
//                {
//                    int result ;
//                    atestCONN->getTuple(k, &result);
//                    cout <<result<<"\t";
//                }
//                cout <<endl;
//                //end test

                if (newField_ptr->getMesh()->getType() != ParaMEDMEM::UNSTRUCTURED)
                    THROW_SALOME_CORBA_EXCEPTION("Not a Unstructured mesh!", SALOME::BAD_PARAM);
                //create field IOR
                ParaMEDMEM::MEDCouplingFieldTemplateServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldTemplateServant(newField_ptr);
                SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();
                //create new Mesh and append to the group
                Mesh * aNewMesh = new Mesh (MeshNames[i].c_str(), genId(), "EmptyMesh", /*NPS*/ 0);
                aNewMesh->setField(myFieldIOR);
                aGroup->appendMesh(aNewMesh->_this());
            }
        }
    }
    catch (INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION( ex.what(), SALOME::BAD_PARAM);
    }

    return aGroup->_this();
}

#ifdef WITH_CGNS
MCMESHTRAN_ORB::MeshGroup_ptr  MCMESHTRAN::importCGNS(const char* CGNSFileName)  throw (SALOME::SALOME_Exception)
{
    if (*CGNSFileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Import Meshes from: " << CGNSFileName);
    //new group for new meshes
    QStringList atmpList = QString(CGNSFileName).split('/');
    atmpList = atmpList.at(atmpList.size()-1).split('.');
    MeshGroup * aGroup = new MeshGroup (atmpList.at(0).toLatin1(), genId());

    //**********start to read the CGNS file ***********

    //defind variable for reading the CGNS file
    int index_file,icelldim,iphysdim,index_base;
    int index_zone, index_sol, index_field,index_sec;
    int BaseCnt, ZoneCnt, SolutionCnt, FieldCnt;
    char basename[33],zonename[33],solname[33],fieldname[33], buffer[33];

    //open file
    if (cg_open(CGNSFileName,CG_MODE_READ,&index_file)) cg_error_exit();
    //see if only one base
    if (cg_nbases(index_file, &BaseCnt)) cg_error_exit();
    if (BaseCnt > 1)
        MESSAGE("More than one Base found, the first will be read!");

    //*********read the base*********
    index_base = 1; //only one base is permitted
    if (cg_base_read(index_file,index_base,basename,&icelldim,&iphysdim)) cg_error_exit();
    if (icelldim != 3 || iphysdim != 3)
        THROW_SALOME_CORBA_EXCEPTION("Error in dimension: should be 3D !", SALOME::BAD_PARAM);

    //*********read the zone*********
    index_zone = 1;
    if (cg_nzones(index_file,index_base,&ZoneCnt)) cg_error_exit();
    if (ZoneCnt > 1)
        MESSAGE("More than one zone found, the first will be read!");
    //obtain zone type
    ZoneType_t mZoneType;
    if (cg_zone_type(index_file,index_base,index_zone,&mZoneType)) cg_error_exit();
    //read the zone
    cgsize_t  isize[9];
    if (cg_zone_read(index_file,index_base,index_zone,zonename,isize)) cg_error_exit() ;
    MESSAGE("Reading Zone: "<<zonename<<endl);
    cgsize_t irmaxCor, irminCor, irmaxCells; //min and max coordinates index
    irminCor = 1;
    irmaxCor = mZoneType==Unstructured ? isize[0] : isize [0]*isize [1]*isize [2]; //unstr:isize[0]; str. isize [0]*isize [1]*isize [2]
    irmaxCells = mZoneType==Unstructured ? isize[1] : isize [3]*isize [4]*isize [5]; //unstr:isize[1]; str. isize [3]*isize [4]*isize [5]
    //check data type
    DataType_t type;
    if (cg_coord_info(index_file,index_base,index_zone,1,&type,buffer)) cg_error_exit(); //assume that three coordinates are using the same data type
    void *vXXX, *vYYY, *vZZZ; //buffer array
    if (type == RealDouble)
    {
        vXXX = new double [irmaxCor];
        vYYY = new double [irmaxCor];
        vZZZ = new double [irmaxCor];
    }
    else if (type == RealSingle)
    {
        vXXX = new float [irmaxCor];
        vYYY = new float[irmaxCor];
        vZZZ = new float[irmaxCor];
    }
    else
        THROW_SALOME_CORBA_EXCEPTION("Data type are not supported!", SALOME::BAD_PARAM);
    //read coordiates
    if (cg_coord_read(index_file,index_base,index_zone,"CoordinateX",type,&irminCor,&irmaxCor,vXXX)) cg_error_exit();
    if (cg_coord_read(index_file,index_base,index_zone,"CoordinateY",type,&irminCor,&irmaxCor,vYYY)) cg_error_exit();
    if (cg_coord_read(index_file,index_base,index_zone,"CoordinateZ",type,&irminCor,&irmaxCor,vZZZ)) cg_error_exit();

    vector <double > aCorVec;   // vector for storing coordinates
    vector <int> Connectivity ; //nodal connectivity
    vector <int> NodalConnIndex;

    //copy data  to the coordinate array
    if (type == RealDouble)
    {
        for (long long int i=0; i<irmaxCor; i++)
        {
            aCorVec.push_back( ((double*) vXXX) [i] );
            aCorVec.push_back( ((double*) vYYY) [i] );
            aCorVec.push_back( ((double*) vZZZ) [i] );
        }
        delete [] (double*) vXXX;
        delete [] (double*) vYYY;
        delete [] (double*) vZZZ;

    }
    else if (type == RealSingle)
    {
        for (long long int i=0; i<irmaxCor; i++)
        {
            aCorVec.push_back (static_cast <double>  (((float*) vXXX) [i])); //use static cast to avoid problem in convertion
            aCorVec.push_back (static_cast <double>  (((float*) vYYY) [i])); //use static cast to avoid problem in convertion
            aCorVec.push_back (static_cast <double>  (((float*) vZZZ) [i])); //use static cast to avoid problem in convertion
        }
        delete [] (float*) vXXX;
        delete [] (float*) vYYY;
        delete [] (float*) vZZZ;
    }

    //*********read grid connectivities****
    if (mZoneType == Unstructured)
    {
        //see how many sections.
        int nSecs;
        if (cg_nsections(index_file,index_base,index_zone,&nSecs)) cg_error_exit();
        ElementType_t mElementType;
        cgsize_t irmaxEle, irminEle;
        int nbndry, ParentFlag;
        cgsize_t mElementSize;
        long long int totalElmCount = 0; //total element counter, for compared with total amount of element in size[]
        //for each section, read element data
        for (index_sec = 1; index_sec <=nSecs; index_sec++)
        {
            if (cg_section_read(index_file,index_base,index_zone,index_sec,buffer,&mElementType,&irminEle,&irmaxEle,&nbndry,&ParentFlag)) cg_error_exit();
            if(ParentFlag > 0 )//parent data are no zero
            {
                cout << "Warning## RWFunctions::WriteUnstrCGNS2Text: Parent data will not be take cared!!" <<endl;
            }
            cout << "Section name: "<<buffer<<"  element type: " <<mElementType<<endl;
            cg_ElementDataSize(index_file,index_base,index_zone,index_sec,&mElementSize); //check the size of ELEMENT[][] array
            cgsize_t * mElements;  //array  ELEMENT[][] for storing nodes for elements
            mElements = new cgsize_t [mElementSize];
            if (cg_elements_read(index_file,index_base,index_zone,index_sec,mElements,NULL)) cg_error_exit();
            long long int index_node, index_cell, CellSize;// index of nodes, cell and cell size (amount)in this section

            //obtaining nodes for a kind of element
            int nNodes =1; //nodes for a kind of element, at least one
            int aMedEleType; //MED element type
            //for the normal element type
            if (mElementType <= 19 || mElementType == 21)
            {
                //get number of nodes and number of corner nodes
                //more info in http://www.grc.nasa.gov/WWW/cgns/CGNS_docs_current/sids/conv.html#unstructgrid
                if (mElementType ==NODE || mElementType ==BAR_2 || mElementType ==BAR_3 ||mElementType ==TRI_3 ||mElementType ==TRI_6
                        ||mElementType ==QUAD_4 ||mElementType ==QUAD_8 ||mElementType ==QUAD_9)
                    continue; //because we don't what non-3D elements

                switch(mElementType)
                {
                case TETRA_4  :
                    nNodes = 4; aMedEleType = INTERP_KERNEL::NORM_TETRA4; break;
                case TETRA_10 :
                    nNodes = 10; aMedEleType = INTERP_KERNEL::NORM_TETRA10; break;
                case PYRA_5   :
                    nNodes = 5; aMedEleType = INTERP_KERNEL::NORM_PYRA5; break;
                case PYRA_13  :
                    nNodes = 13; aMedEleType = INTERP_KERNEL::NORM_PYRA13; break;
                    //                case PYRA_14  :
                    //                    nNodes = 14; aMedEleType = INTERP_KERNEL::NORM_; break;
                case PENTA_6  :
                    nNodes = 6; aMedEleType = INTERP_KERNEL::NORM_PENTA6; break;
                case PENTA_15 :
                    nNodes = 15; aMedEleType = INTERP_KERNEL::NORM_PENTA15; break;
                    //                case PENTA_18 :
                    //                    nNodes = 18; aMedEleType = INTERP_KERNEL::NORM_; break;
                case HEXA_8   :
                    nNodes = 8; aMedEleType = INTERP_KERNEL::NORM_HEXA8; break;
                case HEXA_20  :
                    nNodes = 20; aMedEleType = INTERP_KERNEL::NORM_HEXA20; break;
                case HEXA_27  :
                    nNodes = 27; aMedEleType = INTERP_KERNEL::NORM_HEXA27; break;

                default:
                    THROW_SALOME_CORBA_EXCEPTION("Unnormal in judging Element type!", SALOME::BAD_PARAM);
                }
                CellSize = mElementSize/nNodes;  //cell size in this
                if (CellSize != (irmaxEle-irminEle+1))
                    THROW_SALOME_CORBA_EXCEPTION("Cell size in this section is unnormal!!", SALOME::BAD_PARAM);
                //traverse of elements(cells)
                for (int index_cell=0; index_cell<CellSize; index_cell++)
                {
                    NodalConnIndex.push_back(Connectivity.size()); // put the index of this cell in Connectivity array into index array
                    Connectivity.push_back(aMedEleType); //push first the element type
                    for (int j=0; j< nNodes; j++) // cycle of nodes in a element
                    {
                        index_node = mElements [index_cell*nNodes + j] ;  //node ID
                        Connectivity.push_back(index_node -1 );  // should -1, because CGNS index start with 1, but MED start with 0
                    }
                    totalElmCount ++;
                }

            }
            else if (mElementType == 20) //for the MIXED type of elements
            {
                long long int Pos = 0;  //index for the long mElements array
                int iElementType; //element type for each element in this section
                CellSize = irmaxEle - irminEle + 1; //get number of cells

                iElementType = mElements[Pos];  //Obtaining Element Type for this cell
                //if the first element is non-3D element, then we consider this section is not 3D element section and break
                if (iElementType ==NODE || iElementType ==BAR_2 || iElementType ==BAR_3 ||iElementType ==TRI_3 ||iElementType ==TRI_6
                        ||iElementType ==QUAD_4 ||iElementType ==QUAD_8 ||iElementType ==QUAD_9)
                    continue;

                for (index_cell = 0; index_cell < CellSize; index_cell++)
                {
                    iElementType = mElements[Pos];  //Obtaining Element Type for this cell

                    switch(iElementType)
                    {
                    case TETRA_4  :
                        nNodes = 4; aMedEleType = INTERP_KERNEL::NORM_TETRA4; break;
                    case TETRA_10 :
                        nNodes = 10; aMedEleType = INTERP_KERNEL::NORM_TETRA10; break;
                    case PYRA_5   :
                        nNodes = 5; aMedEleType = INTERP_KERNEL::NORM_PYRA5; break;
                    case PYRA_13  :
                        nNodes = 13; aMedEleType = INTERP_KERNEL::NORM_PYRA13; break;;
                    case PENTA_6  :
                        nNodes = 6; aMedEleType = INTERP_KERNEL::NORM_PENTA6; break;
                    case PENTA_15 :
                        nNodes = 15; aMedEleType = INTERP_KERNEL::NORM_PENTA15; break;
                    case HEXA_8   :
                        nNodes = 8; aMedEleType = INTERP_KERNEL::NORM_HEXA8; break;
                    case HEXA_20  :
                        nNodes = 20; aMedEleType = INTERP_KERNEL::NORM_HEXA20; break;
                    case HEXA_27  :
                        nNodes = 27; aMedEleType = INTERP_KERNEL::NORM_HEXA27; break;
                    default:
                        THROW_SALOME_CORBA_EXCEPTION("Unnormal in judging Element type!", SALOME::BAD_PARAM);
                    }

                    NodalConnIndex.push_back(Connectivity.size()); // put the index of this cell in Connectivity array into index array
                    Connectivity.push_back(aMedEleType); //push first the element type
                    for (int j=0; j< nNodes; j++) // cycle of nodes in a element
                    {
                        index_node = mElements [Pos+j+1] ;  //node ID
                        Connectivity.push_back(index_node -1 );// should -1, because CGNS index start with 1, but MED start with 0
                    }
                    totalElmCount ++;
                    Pos = Pos + nNodes + 1; // go to next cell, one element type plus nNodes nodes
                } //end of cell loop
            }//end of mix type
            else
                THROW_SALOME_CORBA_EXCEPTION("Element type  is not yet supported!!", SALOME::BAD_PARAM);
            delete [] mElements;
        }
        NodalConnIndex.push_back(Connectivity.size()); // should push the last one, required by MED
        if (totalElmCount != irmaxCells)  // if total proc elements not equal total element amount, then return false
            THROW_SALOME_CORBA_EXCEPTION("Total element acount not equal to element read !!", SALOME::BAD_PARAM);
    }//end of unstructured mesh
    else if (mZoneType == Structured)
    {
        //structure mesh contain no mesh connectivitiy info, should be built oneself
        int Imax = isize [0];
        int Jmax = isize [1];
        int Kmax = isize [2];

        for (int k=0; k<Kmax-1; k++)  //k=0~Kmax-2, see http://www.grc.nasa.gov/WWW/cgns/CGNS_docs_current/user/started.html#sec:unstr
        {
            for (int j=0; j<Jmax-1; j++)
            {
                for (int i=0; i<Imax-1; i++)
                {
                    long long int firstNode = i + j*Imax + k*Imax*Jmax;
                    NodalConnIndex.push_back(Connectivity.size()); // put the index of this cell in Connectivity array into index array
                    Connectivity.push_back(INTERP_KERNEL::NORM_HEXA8); // for each cell, first put the cell type

                    Connectivity.push_back(firstNode);
                    Connectivity.push_back(firstNode + 1);
                    Connectivity.push_back(firstNode + 1 + Imax);
                    Connectivity.push_back(firstNode + Imax);
                    Connectivity.push_back(firstNode + Jmax * Imax);
                    Connectivity.push_back(firstNode + Jmax * Imax + 1);
                    Connectivity.push_back(firstNode + Jmax * Imax + 1 + Imax);
                    Connectivity.push_back(firstNode + Jmax * Imax + Imax);
                }
            }
        }
        NodalConnIndex.push_back(Connectivity.size()); // should push the last one, required by MED
    } //end of strutured mesh

    //*********build the mesh **************
    //copy coordinates
    ParaMEDMEM::DataArrayDouble * arrayCor = ParaMEDMEM::DataArrayDouble::New();
    arrayCor->alloc(aCorVec.size()/3 , 3);  //a data array with 3 components (MED term)
    copy(aCorVec.begin(), aCorVec.end(), arrayCor->getPointer());

    //copy nodal connectivity
    ParaMEDMEM::DataArrayInt * aConn = ParaMEDMEM::DataArrayInt::New();
    aConn->alloc(Connectivity.size(),1);
    copy(Connectivity.begin(), Connectivity.end(), aConn->getPointer());

    //copy nodal connectivity index, see MEDCoupling document
    ParaMEDMEM::DataArrayInt * aConnI = ParaMEDMEM::DataArrayInt::New();
    aConnI->alloc(NodalConnIndex.size(),1);
    copy(NodalConnIndex.begin(), NodalConnIndex.end(), aConnI->getPointer());
    //set mesh
    ParaMEDMEM::MEDCouplingUMesh *aMesh=ParaMEDMEM::MEDCouplingUMesh::New(atmpList.at(0).toStdString(), 3);
    aMesh->setConnectivity(aConn, aConnI, false);
    aMesh->setCoords(arrayCor);
    arrayCor->decrRef();
    aConn->decrRef();
    aConnI->decrRef();

    //*********read solutions*********
    if (cg_nsols(index_file,index_base,index_zone,&SolutionCnt)) cg_error_exit();
    if ( SolutionCnt > 1 )
    {
        THROW_SALOME_CORBA_EXCEPTION("more than one solution found!", SALOME::BAD_PARAM);
    }
    else if (SolutionCnt < 1 )  //only mesh, then save the mesh and exit
    {
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldTemplate> aFieldTmp_ptr=
                ParaMEDMEM::MEDCouplingFieldTemplate::New(ParaMEDMEM::ON_CELLS);
        aFieldTmp_ptr->setName(aMesh->getName());
        aFieldTmp_ptr->setMesh(aMesh);
        aMesh->decrRef();
        ParaMEDMEM::MEDCouplingFieldTemplateServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldTemplateServant(aFieldTmp_ptr);
        SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();

        Mesh * aNewMesh = new Mesh(atmpList.at(0).toLatin1(),genId(), "EmptyMesh",/*NPS*/ 0);
        aNewMesh->setField(myFieldIOR);
        aGroup->appendMesh(aNewMesh->_this());
        cg_close(index_file);
        return aGroup->_this();
    }

    index_sol = 1;
    GridLocation_t GridLoc; //see grid location type if it is vertex or CellCenter,or others
    if (cg_sol_info(index_file,index_base,index_zone,index_sol,solname,&GridLoc)) cg_error_exit();
    if (GridLoc != CellCenter && GridLoc != Vertex)
        THROW_SALOME_CORBA_EXCEPTION("Grid location type is not supported!!", SALOME::BAD_PARAM);
    if (cg_nfields(index_file,index_base,index_zone,index_sol,&FieldCnt)) cg_error_exit();
    if (FieldCnt <= 0)
        THROW_SALOME_CORBA_EXCEPTION("No field found!", SALOME::BAD_PARAM);
    //*********see if the solution is has Tally and Error;*********
    bool isTally =false, isError = false;
    cgsize_t irmaxSol, irminSol;  //data size of the solutin
    void * vTally; //for storing Tally
    void * vError;//for storing Error
    for (index_field = 1; index_field <=FieldCnt; index_field++)
    {
        if (cg_field_info(index_file,index_base,index_zone,index_sol,index_field,&type,fieldname)) cg_error_exit();
        // if Tally field found, read it

        if (strcmp(fieldname,"Tally") == 0 || (index_field == FieldCnt && isTally == false)) //if tally field found, or if the last field but still not found tally
        {
            irminSol = 1;
            irmaxSol = GridLoc==Vertex ? irmaxCor : irmaxCells; //vertex: irmaxCor; cellcenter:irmaxcells
            if (type == RealDouble)
                vTally = new double [irmaxSol];
            else if (type == RealSingle)
                vTally = new float [irmaxSol];
            //read the Tally field data
            if (cg_field_read(index_file,index_base,index_zone,index_sol,fieldname,type,&irminSol,&irmaxSol,vTally)) cg_error_exit();
            isTally = true;
        }
        if (strcmp(fieldname,"Error") == 0)
        {
            irminSol = 1;
            irmaxSol = GridLoc==Vertex ? irmaxCor : irmaxCells; //index_zone start from 1
            if (type == RealDouble)
                vError = new double [irmaxSol];
            else if (type == RealSingle)
                vError = new float [irmaxSol];
            //read the Error field data
            if (cg_field_read(index_file,index_base,index_zone,index_sol,fieldname,type,&irminSol,&irmaxSol,vError)) cg_error_exit();
            isError = true;
        }
        if (isTally && isError) //if both read
            break;
        else if (isTally && !isError && index_field == FieldCnt) //if no error data found
        {
            MESSAGE("No error data found, error is assume 0!!");
        }

    }
    //copy data from void pointer to vector, easy for using
    vector <double > theFieldVec;

    if (type == RealDouble)
    {
        for (long long int i=0; i<irmaxSol; i++)
        {
            theFieldVec.push_back( ((double*) vTally) [i] );
            theFieldVec.push_back(isError ? ((double*) vError) [i] : 0.0 );
        }
        delete [] (double*) vTally;
        if (isError)
            delete [] (double*) vError;
    }
    else if (type == RealSingle)
    {
        for (long long int i=0; i<irmaxSol; i++)
        {
            theFieldVec.push_back (static_cast <double>  (((float*) vTally) [i])); //use static cast to avoid problem in convertion
            theFieldVec.push_back (isError ? static_cast <double>  (((float*) vError) [i]) : 0.0); //use static cast to avoid problem in convertion
        }
        delete [] (float*) vTally;
        if (isError)
            delete [] (double*) vError;
    }
    //close file
    cg_close(index_file);

    //save the field data
    ParaMEDMEM::DataArrayDouble * arrayTE = ParaMEDMEM::DataArrayDouble::New();
    arrayTE->alloc(theFieldVec.size()/2, 2);  //two solutions: tally and error
    arrayTE->setInfoOnComponent(0, "Tally");
    arrayTE->setInfoOnComponent(1, "Error");
    copy(theFieldVec.begin(), theFieldVec.end(), arrayTE->getPointer());

    //**********creat new mesh and new field to save the data *******
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr;
    if (GridLoc == Vertex)
    {
//        aField_ptr = ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_NODES,ParaMEDMEM::NO_TIME); //2013-09-05
        aField_ptr = ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_NODES,ParaMEDMEM::ONE_TIME);
        aField_ptr->setNature(ParaMEDMEM::ConservativeVolumic);
    }
    else if (GridLoc == CellCenter)
    {
        aField_ptr =ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::NO_TIME);//2013-09-05
        aField_ptr =ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::ONE_TIME);
        aField_ptr->setNature(ParaMEDMEM::RevIntegral);
    }

    aField_ptr->setName(aMesh->getName());
//        aField_ptr->setNature(ParaMEDMEM::RevIntegral); //not in the case of vertex
    aField_ptr->setMesh(aMesh);
    aMesh->decrRef();  // pointer should be release
    aField_ptr->setArray(arrayTE);
    arrayTE->decrRef(); // pointer should be release
    ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(aField_ptr);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();

    Mesh * aNewMesh = new Mesh(atmpList.at(0).toLatin1(),genId(), "ExtMesh",/*NPS*/ 0);
    aNewMesh->setField(myFieldIOR);
    aGroup->appendMesh(aNewMesh->_this());
    return aGroup->_this();

}
#endif //WITH_CGNS



/*!
 * \brief import a Abaqus mesh file
 *  the abaqus file should contain parts, assembly and instances,
 *  the instances should inherit from parts, independent create of instances is not supported.
 *  part is creat as an independent mesh, the elset and nset is not token care
 * \param AbaqusFileName
 * \return
 */
MCMESHTRAN_ORB::MeshGroup_ptr   MCMESHTRAN::importAbaqus(const char* AbaqusFileName) throw (SALOME::SALOME_Exception)
{
    if (*AbaqusFileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    MESSAGE ("Import Meshes from: " << AbaqusFileName);
    //new group for new meshes
    QStringList atmpList = QString(AbaqusFileName).split('/');
    atmpList = atmpList.at(atmpList.size()-1).split('.');
    MeshGroup * aGroup = new MeshGroup (atmpList.at(0).toLatin1(), genId());

    //using reader to get the meshes
    MCMESHTRAN_AbaqusMeshReader aAbaqusReader ;
    if (aAbaqusReader.loadFile(AbaqusFileName) == Load_failed)
        THROW_SALOME_CORBA_EXCEPTION("Read Abaqus file failed!", SALOME::BAD_PARAM);

    vector <const MEDCouplingUMesh*> aTmpList = aAbaqusReader.getMeshList();
    for (int i=0; i< aTmpList.size(); i++ )
    {
        ParaMEDMEM::MEDCouplingFieldTemplate* newField_ptr =
                ParaMEDMEM::MEDCouplingFieldTemplate::New(ParaMEDMEM::ON_CELLS);
        newField_ptr->setName(aTmpList[i]->getName());

        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::MEDCouplingUMesh> aNewUMesh = const_cast<MEDCouplingUMesh *> (aTmpList[i]);
        newField_ptr->setMesh(aNewUMesh);
        //create field IOR
        ParaMEDMEM::MEDCouplingFieldTemplateServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldTemplateServant(newField_ptr);
        SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();
        //create new Mesh and append to the group
        Mesh * aNewMesh = new Mesh (aTmpList[i]->getName().c_str(), genId(), "EmptyMesh", /*NPS*/ 0);
        aNewMesh->setField(myFieldIOR);
        aGroup->appendMesh(aNewMesh->_this());
    }
    return aGroup->_this();
}


/*!
 * \brief export a list of mesh into a abaqus file, neccessary for MCNP6 calculation
 *  only the mesh is exported, and the field is discard
 * \param aMeshList a list of the mesh
 * \param FileName file path and name to export
 */
void  MCMESHTRAN::exportMesh2Abaqus(const MCMESHTRAN_ORB::MeshList& aMeshList,const char* FileName ) throw (SALOME::SALOME_Exception)
{
    if (*FileName == 0)
        THROW_SALOME_CORBA_EXCEPTION("a empty FileName!", SALOME::BAD_PARAM);
    if (aMeshList.length() == 0)
        THROW_SALOME_CORBA_EXCEPTION("No mesh is on the list to be exported!", SALOME::BAD_PARAM);
    MESSAGE ("Import Meshes from: " << FileName);

    //put the mesh all together into the a list
    vector <const MEDCouplingUMesh *> aUMeshList ;
    for (int i=0; i<aMeshList.length() ; i++) {
        MCMESHTRAN_ORB::Mesh_var aMesh = aMeshList[i];
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldTemplate> aFieldTmplate_ptr
                = ParaMEDMEM::MEDCouplingFieldTemplateClient::New(aMesh->getFieldTemplate());
        aUMeshList.push_back(dynamic_cast<MEDCouplingUMesh *> (
                                 const_cast<MEDCouplingMesh *> (aFieldTmplate_ptr->getMesh())));
        aUMeshList[i]->incrRef();  //increase reference because the aMesh will be destroy outside
    }

    //call the writer to write
    MCMESHTRAN_AbaqusMeshWriter aAbaqusMeshWriter ;
    aAbaqusMeshWriter.setMeshes(aUMeshList);
    if (!aAbaqusMeshWriter.exportFile(FileName))
        THROW_SALOME_CORBA_EXCEPTION("Export meshes to Abaqus file failed!", SALOME::BAD_PARAM);
    for (int i=0; i<aUMeshList.size(); i++) {
        aUMeshList[i]->decrRef();  //should increase reference
    }
}


void MCMESHTRAN::unittest()
{
//    if (test())
//        cout <<"Unit Test Failed!!"<<endl;
}


/*!
 * \brief convert a MCNPFmesh object to a Mesh object
 * \param meshtal the MCNPFmesh object
 * \return  a Mesh object reference
 */
MCMESHTRAN_ORB::Mesh_ptr  MCMESHTRAN::cnvMCNPFmesh2Mesh(MCNPFmesh &meshtal)  throw (SALOME::SALOME_Exception)
{
	cout << "Converting mesh tally...";
    //deal with name
	//2015-08-02 comment out because causing trouble in Windows
	    /*char * aTmpName = CORBA::string_dup ("MFmesh");
    char * aTmpStr = CORBA::string_alloc(10);
    sprintf(aTmpStr, "%d",meshtal.TallyNum );
    strcat( aTmpName , aTmpStr);
    CORBA::string_free(aTmpStr);*/
	QString aTmpName= QString ("MFmesh%1").arg(meshtal.TallyNum);

//    string aTmpName = "MFmesh";
//    aTmpName.append( itostr(meshtal.TallyNum) );

    // create a Mesh object
    //Mesh * aMesh = new Mesh ( aTmpName, genId(), "MCNPFmesh", meshtal.NHistory );
	Mesh * aMesh = new Mesh ( aTmpName.toLatin1(), genId(), "MCNPFmesh", meshtal.NHistory );
//	cout << "aTmpName\t" << aTmpName.toStdString().c_str() << "\tmeshtal.NHistory\t " << meshtal.NHistory  <<endl;
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var aFieldIOR;
    try
    {
        aFieldIOR = cnvMCNPFmesh2MEDCoupling(meshtal);
    }
    catch (SALOME::SALOME_Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION( ex.details.text, SALOME::BAD_PARAM);
    }

    if (aFieldIOR->_is_nil())
        THROW_SALOME_CORBA_EXCEPTION( "A nil field returned!", SALOME::BAD_PARAM);

    aMesh->setField(aFieldIOR._retn());
//    CORBA::string_free(aTmpName);  //should be free after CORBA::string_dup()
	cout <<"done."<<endl;

    return aMesh->_this();
}

/*!
 * \brief convert the MCNPFmesh object to MEDCoupling field
 * \param meshtal the mesh tally
 * \return a MEDCoupling field reference
 * \see http://docs.salome-platform.org/salome_6_6_0/gui/MED/MEDCouplingMeshesPage.html
 *  http://docs.salome-platform.org/salome_6_6_0/gui/MED/MEDCouplingFieldsPage.html
 *  http://www.grc.nasa.gov/WWW/cgns/CGNS_docs_current/user/started.html#sec:unstr
 */
SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MCMESHTRAN::cnvMCNPFmesh2MEDCoupling (MCNPFmesh &meshtal)
throw (SALOME::SALOME_Exception)
{
    //array for storing coordinates and results
    vector <double > aCorVec;   // vector for storing coordinates
    vector <double > aTEVec;    // vector for storing tally and error results
    vector <int> Connectivity ; //nodal connectivity
    vector <int> NodalConnIndex;

    //array for MEDCoupling mesh
    ParaMEDMEM::DataArrayDouble * arrayCor = ParaMEDMEM::DataArrayDouble::New();
    ParaMEDMEM::DataArrayDouble * arrayTE = ParaMEDMEM::DataArrayDouble::New();

    if (meshtal.CoordinateK.size() == 0)
        THROW_SALOME_CORBA_EXCEPTION( "Coordinate K is empty!", SALOME::BAD_PARAM);
    if ( meshtal.CoordinateJ.size() == 0)
        THROW_SALOME_CORBA_EXCEPTION( "Coordinate J is empty!", SALOME::BAD_PARAM);
    if ( meshtal.CoordinateI.size() == 0)
        THROW_SALOME_CORBA_EXCEPTION( "Coordinate I is empty!", SALOME::BAD_PARAM);

//    ASSERT (meshtal.CoordinateK.size() != 0);   //check the array  first
//    ASSERT (meshtal.CoordinateJ.size() != 0);
//    ASSERT (meshtal.CoordinateI.size() != 0);

    //########## Cartisan and Cylindrical Mesh to Unstructured Mesh##########

    //    ASSERT(!(meshtal.Coordinate != 1 && meshtal.Coordinate != 2))// check the Coordinate
    //    if (meshtal.Coordinate == 1) // if Cartisan mesh

    //1. transform coordinates of vertices in Cartisan Mesh
    //   into a sequence of vertices
    for ( int k=0; k<meshtal.CoordinateK.size(); k++)  //for Z
    {
        for (int j=0; j<meshtal.CoordinateJ.size(); j++)  //for Y
        {
            for (int i=0; i<meshtal.CoordinateI.size(); i++)  //for X
            {
                Point aPoint = meshtal.getOnePoint(i, j, k);
                aCorVec.push_back(aPoint.x);
                aCorVec.push_back(aPoint.y);
                aCorVec.push_back(aPoint.z);
            }
        }
    }
    arrayCor->alloc(aCorVec.size()/3 , 3);  //a data array with 3 components (MED term)
    copy(aCorVec.begin(), aCorVec.end(), arrayCor->getPointer());
    //2. storing the result
//    ASSERT(meshtal.TallyArray.size() == meshtal.ErrorArray.size() || meshtal.TallyArray.size() != 0); //check the array
    if (meshtal.TallyArray.size() != meshtal.ErrorArray.size() ) //check the array
        THROW_SALOME_CORBA_EXCEPTION( "Result and Error array size abnormal!", SALOME::BAD_PARAM);
    if (meshtal.TallyArray.size() == 0) //check the array
        THROW_SALOME_CORBA_EXCEPTION( "Result array is empty!", SALOME::BAD_PARAM);

    int NumI = meshtal.CoordinateI.size()-1;  //on cell
    int NumJ = meshtal.CoordinateJ.size()-1;
    int NumK = meshtal.CoordinateK.size()-1;

    for (int k=0; k<NumK; k++)  //for Z
    {
        for (int j=0; j<NumJ; j++)  //for Y
        {
            for (int i=0; i<NumI; i++)  //for X
            {
                aTEVec.push_back(meshtal.getOneTallyResult(i, j, k));
                aTEVec.push_back(meshtal.getOneErrorResult(i, j, k));
            }
        }
    }

    arrayTE->alloc(aTEVec.size()/2, 2);  //two solutions: tally and error
    arrayTE->setInfoOnComponent(0, "Tally");
    arrayTE->setInfoOnComponent(1, "Error");
    copy(aTEVec.begin(), aTEVec.end(), arrayTE->getPointer());

    //3. create mesh connectivity
    int Imax = meshtal.CoordinateI.size();
    int Jmax = meshtal.CoordinateJ.size();
    int Kmax = meshtal.CoordinateK.size();

    for (int k=0; k<Kmax-1; k++)  //k=0~Kmax-2, see http://www.grc.nasa.gov/WWW/cgns/CGNS_docs_current/user/started.html#sec:unstr
    {
        for (int j=0; j<Jmax-1; j++)
        {
            for (int i=0; i<Imax-1; i++)
            {
                int firstNode = i + j*Imax + k*Imax*Jmax;
                Connectivity.push_back(INTERP_KERNEL::NORM_HEXA8); // for each cell, first put the cell type
                NodalConnIndex.push_back(Connectivity.size() - 1); // put the index of this cell in Connectivity array into index array

                Connectivity.push_back(firstNode);
                Connectivity.push_back(firstNode + 1);
                Connectivity.push_back(firstNode + 1 + Imax);
                Connectivity.push_back(firstNode + Imax);
                Connectivity.push_back(firstNode + Jmax * Imax);
                Connectivity.push_back(firstNode + Jmax * Imax + 1);
                Connectivity.push_back(firstNode + Jmax * Imax + 1 + Imax);
                Connectivity.push_back(firstNode + Jmax * Imax + Imax);
            }
        }
    }
    //the last element of NodalConnIndex is Connectivity.size(), important!!
    //see:http://docs.salome-platform.org/salome_6_6_0/gui/MED/MEDCouplingUMeshPage.html
    NodalConnIndex.push_back(Connectivity.size());

    //copy nodal connectivity
    ParaMEDMEM::DataArrayInt * aConn = ParaMEDMEM::DataArrayInt::New();
    aConn->alloc(Connectivity.size(),1);
    copy(Connectivity.begin(), Connectivity.end(), aConn->getPointer());

    //copy nodal connectivity index, see MEDCoupling document
    ParaMEDMEM::DataArrayInt * aConnI = ParaMEDMEM::DataArrayInt::New();
    aConnI->alloc(NodalConnIndex.size(),1);
    copy(NodalConnIndex.begin(), NodalConnIndex.end(), aConnI->getPointer());


    //4. set mesh
    ParaMEDMEM::MEDCouplingUMesh *aMesh=ParaMEDMEM::MEDCouplingUMesh::New("MCNPFmesh", 3);
    aMesh->setConnectivity(aConn, aConnI, false);
    aMesh->setCoords(arrayCor);

    arrayCor->decrRef();
    aConn->decrRef();
    aConnI->decrRef();

    //5. set field
/*2015-09-02 comment out because causing trouble in Window
    char * aTmpName = CORBA::string_dup ("MFmesh");
    char * aTmpStr = CORBA::string_alloc(10);
    sprintf(aTmpStr, "%d",meshtal.TallyNum );
    strcat( aTmpName , aTmpStr);
    CORBA::string_free(aTmpStr);*/
	QString aTmpName= QString ("MFmesh%1").arg(meshtal.TallyNum);

//    ParaMEDMEM::MEDCouplingFieldDouble* fieldOnCells=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::NO_TIME);//2013-09-05
    ParaMEDMEM::MEDCouplingFieldDouble* fieldOnCells=ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::ONE_TIME);
//    fieldOnCells->setName(aTmpName);
    fieldOnCells->setName(aTmpName.toStdString());
    fieldOnCells->setNature(ParaMEDMEM::RevIntegral);
    fieldOnCells->setMesh(aMesh);
    aMesh->decrRef();
    fieldOnCells->setArray(arrayTE);
    arrayTE->decrRef();
    //end of field definition

    //6. load field into corba interface
    ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(fieldOnCells);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();

    ASSERT(!CORBA::is_nil(myFieldIOR));
    fieldOnCells->decrRef();
    ASSERT(!CORBA::is_nil(myFieldIOR));

    return myFieldIOR;
}

/*!
 * \brief interpolation using the MEDCouplingRemapper class
 * the interpolation algorithm : \see http://docs.salome-platform.org/salome_6_6_0/gui/MED/InterpKerRemapGlobal.html
 * the Nature of a Field should be paid attention! : \see http://docs.salome-platform.org/salome_6_6_0/gui/MED/group__NatureOfField.html
 * Here we perform interoplation of Flux Density or Power Density, thus using \c RevIntegral (volume weighted)method, and cell-cell method \c P0P0
 * \param srcField source field
 * \param tgtField target field
 * \return the new interpolated field
 * examples \see http://docs.salome-platform.org/salome_6_6_0/gui/MED/RemapperClasses.html
 */
SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr MCMESHTRAN::interpolateWithRemapper (SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr srcField,
//        SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr tgtField,
        SALOME_MED::MEDCouplingMeshCorbaInterface_ptr tgtMesh,
        const char *FieldName, MCMESHTRAN_ORB::SolutionLoc solloc)
throw (SALOME::SALOME_Exception)
{

    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> SrcField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(srcField);
    //30-08-2-13 changed to support ON_CELL and ON_NODE interpolation
//    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldTemplate> TgtField_ptr
//            =ParaMEDMEM::MEDCouplingFieldTemplateClient::New(tgtField);
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingMesh> TgtMesh_ptr
            =ParaMEDMEM::MEDCouplingMeshClient::New(tgtMesh);

    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble>  newField_ptr;

    //the following interpolation  valid in case of ON_CELLS and ON_NODES
    //the follwing line is commmend out because the source field is already assigned with nature. in this case both ON_CELLS and ON_NODES is supported
//    SrcField_ptr->setNature(ParaMEDMEM::RevIntegral);  //set as Reverse Integral field, because the field is Power Density or Flux Density
    ParaMEDMEM::MEDCouplingRemapper remapper;
    remapper.setPrecision(1e-12);
    remapper.setIntersectionType(INTERP_KERNEL::Triangulation);  //using Trianulation method, see http://docs.salome-platform.org/salome_6_6_0/gui/MED/InterpKerIntersectors.html#interpolation3D
    QString IntMethod;
    //source: volume average value on cells, target: on cell
    if (SrcField_ptr->getNature() == ParaMEDMEM::RevIntegral && solloc == MCMESHTRAN_ORB::on_cells)
    {
        IntMethod = "P0P0";
        remapper.setIntersectionType(INTERP_KERNEL::Triangulation);
    }
    //source: volume average value on cells, target: on nodes
    else if (SrcField_ptr->getNature() == ParaMEDMEM::RevIntegral && solloc == MCMESHTRAN_ORB::on_nodes)
    {
        IntMethod = "P0P1";
        remapper.setIntersectionType(INTERP_KERNEL::PointLocator);
    }
    //source: volume average value on node, target: on cell
    else if (SrcField_ptr->getNature() == ParaMEDMEM::ConservativeVolumic && solloc == MCMESHTRAN_ORB::on_cells)
    {
        IntMethod = "P1P0";
        remapper.setIntersectionType(INTERP_KERNEL::Triangulation);
    }
    //source: volume average value on node, target: on node
    else if (SrcField_ptr->getNature() == ParaMEDMEM::ConservativeVolumic && solloc == MCMESHTRAN_ORB::on_nodes)
    {
        IntMethod = "P1P1";
        remapper.setIntersectionType(INTERP_KERNEL::PointLocator);
    }
    else
       THROW_SALOME_CORBA_EXCEPTION( "Error in field Nature!", SALOME::BAD_PARAM);

    try
    {
        remapper.prepare(SrcField_ptr->getMesh(),TgtMesh_ptr, IntMethod.toStdString());  //using cell to cell remap
        newField_ptr = remapper.transferField(SrcField_ptr,/*default_value*/0.0);//Any target cell not intercepted by any source cell will have value set to 0.0
        newField_ptr->setName(FieldName);
    }
    catch(INTERP_KERNEL::Exception &ex)
    {
        THROW_SALOME_CORBA_EXCEPTION(ex.what(), SALOME::BAD_PARAM);
    }

    ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(newField_ptr);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();

    return myFieldIOR;
}

///*!
// * \brief get a SMESH engine for use
// * \return a SMESH engine, probably nil
// */
//SMESH::SMESH_Gen_ptr MCMESHTRAN::getSMESHEngine()
//{
//    Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component("FactoryServer", "VISU" );
//    if ( CORBA::is_nil(comp  ) )
//        cout << "onDisplayInVISU(): the Component is nil!"<<endl;

//    VISU::VISU_Gen_var aVISUGen = VISU::VISU_Gen::_narrow(comp);
//    ASSERT( !CORBA::is_nil( aVISUGen ) );
//}


//############### MeshGroup ####################


MeshGroup::MeshGroup(const char *Name,  CORBA::Long id)
{

    myName = CORBA::string_dup(Name);
    myID = id;
    myList = new MCMESHTRAN_ORB::MeshList ();
}

MeshGroup::~MeshGroup()
{
    CORBA::string_free(myName);
    // no need to delete
    //        delete myList;
}


/*!
 * \brief get Name
 * \return the name of the mesh
 */
char* MeshGroup::getName()
{
    //duplicate in case destroy outside
    return CORBA::string_dup(myName);
}
/*!
 * \brief set the mesh Name
 * \param Name
 */
void MeshGroup::setName(const char *Name)
{
    CORBA::string_free(myName);
    myName = CORBA::string_dup(Name);
}


/*!
 * \brief get the mesh according to the \a id
 * \param id mesh id
 * \return Mesh pointer
 */
MCMESHTRAN_ORB::Mesh_ptr MeshGroup::getMesh(CORBA::Long MeshID)
{
    const int n = myList->length();

    for (int i = 0; i < n; i++)
    {
        // ***********code segment for further reference**********
        if (!CORBA::is_nil((*myList)[i]))
        {
            MCMESHTRAN_ORB::Mesh_var  aMesh = (*myList)[i];
            if (MeshID == aMesh->getID())
                return aMesh._retn(); //if find the Mesh, return the pointer
        }

    }
    return MCMESHTRAN_ORB::Mesh::_nil();  //else return NULL
}

/*!
 * \brief set a list of Mesh into this group
 * \param Meshes a list of mesh
 * \return \c true if success
 */
CORBA::Boolean MeshGroup::setMeshList(const MCMESHTRAN_ORB::MeshList & aMeshList)
{
    const int n = aMeshList.length();
    myList->length(0);
    myList->length(n);

    for (int i = 0; i < n; i++ )
        (*myList)[i] = aMeshList[i];

    return (bool) n;
}

/*!
 * \brief append a new mesh into the end of the Mesh group
 * \param aMesh mesh to be append
 * \return \c true if is success
 */
CORBA::Boolean MeshGroup::appendMesh(MCMESHTRAN_ORB::Mesh_ptr aMesh)
{
    if (aMesh->_is_nil()) return false;
    MESSAGE("Append a mesh : " << aMesh->getName());
    int n = myList->length() + 1;
    myList->length(n);
    //2013-03-22 should use duplicate, other wise  segment fault when aMesh is delete outside
    (*myList)[n-1] = MCMESHTRAN_ORB::Mesh::_duplicate(aMesh);
    return true;
}

/*!
 * \brief delete one mesh from the list
 *  see the detail design documents for details
 * \param MeshID the mesh with \a MeshID to be delete
 * \return \c true is succeed
 */
CORBA::Boolean  MeshGroup::deleteMesh(CORBA::Long MeshID)
{
    const int n = myList->length();
    int i;
    for (i=0; i<n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            MCMESHTRAN_ORB::Mesh_var  aMesh = (*myList)[i];
            if (MeshID == aMesh->getID())
            {
                for (int j=i; j<n-1; j++)
                {
                    (*myList)[j] = (*myList)[j+1]; //see the detail design documents
                }
                break;//important
            }
        }
    }
    if (i == n)// cannot find the mesh
        return false;

    myList->length(n-1);  //reduce the lenght therefore delete on item
    return true;
}

/*!
 * \brief insert a Mesh into the list before the mesh with \a MeshID
 * \param aMesh the mesh to be inserted
 * \param MeshID put the \a aMesh before this mesh
 * \return \c true if succeed
 */
CORBA::Boolean  MeshGroup::insertMesh(MCMESHTRAN_ORB::Mesh_ptr aMesh, CORBA::Long MeshID)
{
    if (aMesh->_is_nil()) return false;
    MESSAGE("Insert a mesh : " << aMesh->getName());
    const int n = myList->length();
    //if no mesh in the group , just append it
    if (n ==0 )
    {
        appendMesh(aMesh);
        return true; //just append
    }
    //if the mesh with MeshID could not be found, append it
    if (getMesh(MeshID)->_is_nil())
    {
        appendMesh(aMesh);
        return true; //just append
    }
    //normal situation
    myList->length(n+1) ;  //increase the length with one elemetn
    int i;
    for (i=0; i<n; i++)
    {
        if (!CORBA::is_nil((*myList)[i]))
        {
            MCMESHTRAN_ORB::Mesh_var  aMesh = (*myList)[i];
            if (MeshID == aMesh->getID())
            {
                for (int j=n; j>i; j--)
                {
                    (*myList)[j] = (*myList)[j-1]; //see the detail design documents
                }
                break; //important
            }
        }
    }
    if (i == n)// cannot find the mesh
    {
        myList->length(n);//recover the length if not insert
        return false;
    }

    //2013-03-22 should use duplicate, other wise  segment fault when aMesh is delete outside
    (*myList)[i] = MCMESHTRAN_ORB::Mesh::_duplicate(aMesh);  //copy the mesh to be insert into the list
    return true;

}
/*!
 * \brief clear all Meshes
 * \return
 */
CORBA::Boolean  MeshGroup::clearMeshes()
{
    myList->length(0);
    return true;
}

/*!
 * \brief append all Meshes In another Group
 * \param aGroup
 */
void MeshGroup::appendMeshInGroup(MCMESHTRAN_ORB::MeshGroup_ptr aGroup)
{
    if (aGroup->_is_nil() || aGroup->getMeshList()->length() == 0)
        return;
    const int n = aGroup->getMeshList()->length();
    for (int i=0; i<n; i++)
        this->appendMesh((*aGroup->getMeshList())[i]);
}

//############### Mesh ####################


Mesh::Mesh(const char * Name,const CORBA::Long id, const char * Type, CORBA::Double nps)
{
    myName = CORBA::string_dup(Name);
    myID = id;
    myType = CORBA::string_dup(Type);
    myNPS  = nps;
    myField = SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_nil();  //first assign a nil field
    myFieldTemplate = SALOME_MED::MEDCouplingFieldTemplateCorbaInterface::_nil();
}

Mesh::~Mesh()
{
    CORBA::string_free(myName) ;
    CORBA::string_free(myType) ;
    //2013-06-10 to see if release of these CORBA pointer can solve memory problem
    if (!myField->_is_nil())
    {
        myField->UnRegister();
        CORBA::release(myField);
    }
    if (!myFieldTemplate->_is_nil())
    {
        myFieldTemplate->UnRegister();
        CORBA::release(myFieldTemplate);
    }
}


/*!
 * \brief set the mesh Name
 * \param Name
 */
void Mesh::setName(const char *Name)
{

    CORBA::string_free(myName);
    myName = CORBA::string_dup(Name);

}
/*!
 * \brief set the field into the mesh
 * \param aField the field
 */
void  Mesh::setField(SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr aField)
{
    if(!aField->_is_nil())  //if not nil
        myField = aField;  //directly assign, not copy, thus aField should not be destroy outside
    else  //assign a nil field
        myField = SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_nil();
}

/*!
 * \brief set a emtpy field (named field template) into the mesh
 * \param aField the field template
 */
void  Mesh::setField(SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr aField)
{
    if(!aField->_is_nil())  //if not nil
        myFieldTemplate = aField;  //directly assign, not copy, thus aField should not be destroy outside
    else  //assign a nil field template
        myFieldTemplate = SALOME_MED::MEDCouplingFieldTemplateCorbaInterface::_nil();
}
/*!
 * \brief return a copy of IOR of myField
 *  here use duplicate because if myField is return,
 *  it might be destroy outside the instance
 * \return \a myField or nil
 */
SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr Mesh::getField()
{
    if (strcmp(myType, "EmptyMesh") != 0) //not empty mesh
    {
        if (!myField->_is_nil())  //if not nil
            return SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_duplicate(myField);
    }
    return SALOME_MED::MEDCouplingFieldDoubleCorbaInterface::_nil();
}

/*!
 * \brief get a empty field,
 *  if \a myFieldTemplate is not nil, return it; if nil, make a template and return it.
 * \return a template, or nil
 */
SALOME_MED::MEDCouplingFieldTemplateCorbaInterface_ptr  Mesh::getFieldTemplate()
{
    if (strcmp(myType, "EmptyMesh") == 0) // empty mesh
    {
        if (!myFieldTemplate->_is_nil())  //if not nil
            return SALOME_MED::MEDCouplingFieldTemplateCorbaInterface::_duplicate(myFieldTemplate);
        else if (!myField->_is_nil())  //if nil , make a template from myField and return it.
        {
            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
                    =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(myField);
            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldTemplate> aFieldTmp_ptr
//                    = ParaMEDMEM::MEDCouplingFieldTemplate::New (aField_ptr);
                    //2013-05-23 modif to adapt change in SALOME 7.2.0
                    = ParaMEDMEM::MEDCouplingFieldTemplate::New (*aField_ptr);

            ParaMEDMEM::MEDCouplingFieldTemplateServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldTemplateServant(aFieldTmp_ptr);
            myFieldTemplate = myFieldDoubleI->_this();
            return SALOME_MED::MEDCouplingFieldTemplateCorbaInterface::_duplicate(myFieldTemplate);
        }
        else
            return SALOME_MED::MEDCouplingFieldTemplateCorbaInterface::_nil();
    }
    else if (!myField->_is_nil())  //if other mesh
    {
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
                =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(myField);
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldTemplate> aFieldTmp_ptr
//                = ParaMEDMEM::MEDCouplingFieldTemplate::New (aField_ptr);
                //2013-05-23 modif to adapt change in SALOME 7.2.0
                = ParaMEDMEM::MEDCouplingFieldTemplate::New (*aField_ptr);

        ParaMEDMEM::MEDCouplingFieldTemplateServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldTemplateServant(aFieldTmp_ptr);
        myFieldTemplate = myFieldDoubleI->_this();
        return SALOME_MED::MEDCouplingFieldTemplateCorbaInterface::_duplicate(myFieldTemplate);
    }
    else
        return SALOME_MED::MEDCouplingFieldTemplateCorbaInterface::_nil();

}


extern "C"
{
Standard_EXPORT PortableServer::ObjectId * MCMESHTRANEngine_factory(
        CORBA::ORB_ptr orb,
        PortableServer::POA_ptr poa,
        PortableServer::ObjectId * contId,
        const char *instanceName,
        const char *interfaceName)
{
    cout << "PortableServer::ObjectId * MCMESHTRANEngine_factory()"<<endl;
    MESSAGE("PortableServer::ObjectId * MCMESHTRANEngine_factory()");
    SCRUTE(interfaceName);
    MCMESHTRAN * myMCMESHTRAN
            = new MCMESHTRAN(orb, poa, contId, instanceName, interfaceName);
    return myMCMESHTRAN->getId() ;
}
}
