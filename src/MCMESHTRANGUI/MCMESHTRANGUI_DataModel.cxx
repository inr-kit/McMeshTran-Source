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

#include "MCMESHTRANGUI_DataModel.h"
//#include "MCMESHTRANGUI_DataObject.h"
//#include "MCMESHTRANGUI.h"
#include <SalomeApp_Application.h>
#include <SALOME_LifeCycleCORBA.hxx>
#include <SUIT_Session.h>
#include <SUIT_DataObjectIterator.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Tools.h>
#include <LightApp_Study.h>
#include <CAM_Module.h>
#include <CAM_Application.h>
#include "utilities.h"
#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServant)
#include CORBA_CLIENT_HEADER(SMESH_Gen)
#include CORBA_CLIENT_HEADER(MCMESHTRAN)
#include <QFile>
#include <string>
#include <QString>

#include <SMESH_Client.hxx>

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

/*! Constructor */
MCMESHTRANGUI_DataModel::MCMESHTRANGUI_DataModel( CAM_Module* module )
  : LightApp_DataModel( module ),
    myStudyURL("")
{
    CopyList = new MCMESHTRAN_ORB::MeshList;
    CutList  = new MCMESHTRAN_ORB::MeshList;
    count = 1;  //for test
}

/*! Destructor */
MCMESHTRANGUI_DataModel::~MCMESHTRANGUI_DataModel()
{
    CopyList->length(0);
    CutList ->length(0);
}

/*!
  \brief Open data model (read data from the files). This method is load
  when a saved study is loaded
  \param url study file path
  \param study study pointer
  \param listOfFiles list of the (temporary) files with data
  \return operation status (\c true on success and \c false on error)
*/
bool MCMESHTRANGUI_DataModel::open( const QString& url, CAM_Study* study, QStringList listOfFiles )
{
    LightApp_Study* aDoc = dynamic_cast<LightApp_Study*>( study );
    if ( !aDoc )
        return false;

    LightApp_DataModel::open( url, aDoc, listOfFiles );

    return true;
}

/*!
  \brief Save data model (write data to the files).
  \param listOfFiles returning list of the (temporary) files with saved data
  \return operation status (\c true on success and \c false on error)
*/
bool MCMESHTRANGUI_DataModel::save( QStringList& theListOfFiles )
{
//    bool isMultiFile = true;

    LightApp_DataModel::save( theListOfFiles );
    return true;
}

/*!
  \brief Save data model (write data to the files).
  \param url study file path
  \param study study pointer
  \param listOfFiles returning list of the (temporary) files with saved data
  \return operation status (\c true on success and \c false on error)
*/
bool MCMESHTRANGUI_DataModel::saveAs( const QString& url, CAM_Study* study, QStringList& theListOfFiles )
{
    myStudyURL = url;
    return save( theListOfFiles );
}

/*!
  \brief Close data model (remove all relevant data).
  \return operation status (\c true on success and \c false on error)
*/
bool MCMESHTRANGUI_DataModel::close()
{
    return LightApp_DataModel::close();
}

/*!
  \brief Create empty data model.
  \param study study pointer
  \return operation status (\c true on success and \c false on error)
*/
bool MCMESHTRANGUI_DataModel::create( CAM_Study* study )
{
    return true;
}

/*!
  \brief Get 'modified' flag.
  \return \c true if data is changed from the last saving
*/
bool MCMESHTRANGUI_DataModel::isModified() const
{
    // return false to avoid masking study's isModified()
    return false;
}

/*!
  \brief Get 'saved' flag.
  \return \c true if data has been saved
*/
bool MCMESHTRANGUI_DataModel::isSaved() const
{
    // return true to avoid masking study's isSaved()
    return true;
}

/*!
  \brief Update data model.
  \param obj data object (starting for the update)
  \param study study pointer
*/
void MCMESHTRANGUI_DataModel::update( LightApp_DataObject* /*obj*/, LightApp_Study* /*study*/ )
{
    // Nothing to do here: we always keep the data tree in the up-to-date state
    // The only goal of this method is to hide default behavior from LightApp_DataModel
    return;
}

bool MCMESHTRANGUI_DataModel::generateGroup(const QString&  aGroupName)
{
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        //generate a group inside the engine
        MCMESHTRAN_ORB::GroupList * aGroupList = engine->getData(studyID);
        if (!aGroupList)
        {
            MESSAGE("Group list is emtpy");
//            SUIT_MessageBox::warning(desktop(), QString("Warning"), QString( "MCMESHTRANGUI_DataModel::generateGroup: Group list is emtpy"));
            return false;
        }
        aGroupList->length(aGroupList->length()+1);// increase a space for new group
        MCMESHTRAN_ORB::MeshGroup_var aGroup = engine->generateGroup(aGroupName.toLatin1());
        (*aGroupList)[aGroupList->length()-1] = aGroup;
        return true;
     }
    return false;
}

int  MCMESHTRANGUI_DataModel::getStudyID()
{
//    LightApp_Study* study = getStudy();
    LightApp_Study* study = dynamic_cast<LightApp_Study*>(
                module()->application()->activeStudy() ); //obtain the current study
    if (study)
        return study->id();
    else
        return 0;
}

/*!
 * \brief get the group id from the entry
 * \param aEntry
 * \return group id
 */
int  MCMESHTRANGUI_DataModel::getGroupID(const QString&  aEntry )
{
    QString aIdStr = aEntry.split("_").at(1); //obtain the string for group id, see XYSMESHGUI_DataObject::entry()
    if (aIdStr == "root")
        return -1;   //not a group id

    bool  isOK;
    int aId = aIdStr.toInt(&isOK);
    if (!isOK)
        return -1;
    return aId;
}
/*!
 * \brief get the mesh id from the entry
 * \param aEntry
 * \return mesh id
 */
int  MCMESHTRANGUI_DataModel::getMeshID (const QString&  aEntry )
{
    QStringList aStrList = aEntry.split("_");
    if (aStrList.size() != 3)  //has no mesh id
        return -1;  //indicate a group or the root

    bool  isOK;
    int aId = aStrList.at(2).toInt(& isOK);
    if (!isOK)
        return -1;
    return aId;
}

/*!
 * \brief find the data object according to the entry
 * \param aEntry
 * \return data object
 */
MCMESHTRANGUI_DataObject * MCMESHTRANGUI_DataModel::findObject(const QString&  aEntry)
{
    for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
    {
        MCMESHTRANGUI_DataObject* obj = dynamic_cast<MCMESHTRANGUI_DataObject*>( it.current() );
        if (obj && obj->entry() == aEntry)
            return obj;
    }
    MESSAGE("Cannot find object!")
    return 0;
}

//QString  MCMESHTRANGUI_DataModel::getTmpDir()
//{

//}


/*!
 * \brief read MCNP5 mesh tallies
 *  this function needs MCNP input file and meshtal file, which are extractly match
 * \param InputFileName mcnp input file
 * \param MeshtalFileName mcnp meshtal file
 * \return \c true if succeed
 */
bool MCMESHTRANGUI_DataModel::readMCNPFmesh(const QString&  InputFileName, const QString&  MeshtalFileName)
{
    if (InputFileName.trimmed().isEmpty() || MeshtalFileName.trimmed().isEmpty())
    {
        MESSAGE("File name empty!");
        return false;
    }
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        MCMESHTRAN_ORB::GroupList * aGroupList = engine->getData(studyID);
        if (!aGroupList)
        {
            MESSAGE("Group list is emtpy!")
            return false;
        }
        MCMESHTRAN_ORB::MeshGroup_var aGroup;
        try
        {
             aGroup = engine->readMCNPFmesh(InputFileName.toLatin1(), MeshtalFileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Read mesh tally failed: " << ex.details.text)  ;
            return false;
        }
        if (aGroup->_is_nil())
        {
            MESSAGE("Return a nil group!")
            return false;
        }
        aGroupList->length(aGroupList->length()+1);// increase a space for new group
        (*aGroupList)[aGroupList->length()-1] = aGroup._retn();
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}

/*!
 * \brief read MCNP6 unstructured mesh eeout file
 *  this function needs MCNP6 UMesh eeout file
 * \param UMeshOutFileName mcnp eeout file
 * \param isKeepInstance if the results for each instance should be kept. \a false would keeps the entire mesh
 * \param isRemoveMultiplier if the multiplier should be remove from the results.
 * \return \c true if succeed
 */
bool MCMESHTRANGUI_DataModel::readMCNPUmesh(const QString&  UMeshOutFileName, const bool &isKeepInstance, const bool &isRemoveMultiplier)
{
    if (UMeshOutFileName.trimmed().isEmpty())
    {
        MESSAGE("File name empty!");
        return false;
    }
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        MCMESHTRAN_ORB::GroupList * aGroupList = engine->getData(studyID);
        if (!aGroupList)
        {
            MESSAGE("Group list is emtpy!")
            return false;
        }
        MCMESHTRAN_ORB::MeshGroup_var aGroup;
        try
        {
             aGroup = engine->readMCNPUmesh(UMeshOutFileName.toLatin1(),isKeepInstance, isRemoveMultiplier);
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Read UMesh output failed: " << ex.details.text)  ;
            return false;
        }
        if (aGroup->_is_nil())
        {
            MESSAGE("Return a nil group!")
            return false;
        }
        aGroupList->length(aGroupList->length()+1);// increase a space for new group
        (*aGroupList)[aGroupList->length()-1] = aGroup._retn();
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}

/*!
 * \brief MCMESHTRANGUI_DataModel::readTRIPOLIFmesh
 *  For reading TRIPOLI output file, process the results an generate the mesh
 * \param TRIPOLIFmesh TRIPOLI output file name
 * \return false if error
 */
bool MCMESHTRANGUI_DataModel::readTRIPOLIFmesh(const QString& TRIPOLIFileName, const bool & isVolumAverage)
{
    if (TRIPOLIFileName.trimmed().isEmpty())
    {
        MESSAGE("File name empty!");
        return false;
    }
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        MCMESHTRAN_ORB::GroupList * aGroupList = engine->getData(studyID);
        if (!aGroupList)
        {
            MESSAGE("Group list is emtpy!")
            return false;
        }
        MCMESHTRAN_ORB::MeshGroup_var aGroup;
        try
        {
             aGroup = engine->readTRIPOLIFmesh(TRIPOLIFileName.toLatin1(), isVolumAverage);
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Read UMesh output failed: " << ex.details.text)  ;
            return false;
        }
        if (aGroup->_is_nil())
        {
            MESSAGE("Return a nil group!")
            return false;
        }
        aGroupList->length(aGroupList->length()+1);// increase a space for new group
        (*aGroupList)[aGroupList->length()-1] = aGroup._retn();
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}


/*!
 * \brief sum two meshes
 *  the tally result will be summed, the largest error in each mesh cell will be kept
 * \param aEntry a object
 * \param bEntry b object
 * \param Name Name for new object
 * \param GroupID group to be append
 * \return \c true if succeed
 */
bool MCMESHTRANGUI_DataModel::sumMeshes(const QString&  aEntry,  const QString&  bEntry, const QString&  Name, const int GroupID  )
{
    if (Name.trimmed().isEmpty() )
    {
        MESSAGE("Name is empty!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        //find object and get the mesh
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        MCMESHTRANGUI_DataObject * bObj = findObject(bEntry);
        if (!aObj || !bObj)
        {
            MESSAGE("Cannot find object in study!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        MCMESHTRAN_ORB::Mesh_var bMesh = bObj->getMesh();
        if (aMesh->_is_nil() || bMesh->_is_nil())
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        //get the return mesh after summing
        MCMESHTRAN_ORB::Mesh_var cMesh;
        try
        {
            cMesh = engine->sumMeshes(aMesh, bMesh, Name.toLatin1());
            if (cMesh->_is_nil())
            {
                MESSAGE("Return mesh is nil!");
                return false;
            }
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Sum failed: " << ex.details.text)  ;
            return false;
        }
        //get the group which the mesh to be append
        MCMESHTRAN_ORB::MeshGroup_var aGroup = engine->getGroup(studyID, GroupID);  //find the group
        if (aGroup->_is_nil())
        {
            MESSAGE("Cannot find the group with GroupID!");
            return false;
        }
        aGroup->appendMesh(cMesh._retn());
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}

/*!
 * \brief average two meshes
 *  the tally and error result will be averaged weighted by partical history;
 * \param aEntry a object
 * \param bEntry b object
 * \param Name Name for new object
 * \param GroupID group to be append
 * \return \c true if succeed
 */
bool MCMESHTRANGUI_DataModel::averageMeshes(const QString&  aEntry,  const QString&  bEntry, const QString&  Name, const int GroupID  )
{
    if (Name.trimmed().isEmpty() )
    {
        MESSAGE("Name is empty!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        //find object and get the mesh
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        MCMESHTRANGUI_DataObject * bObj = findObject(bEntry);
        if (!aObj || !bObj)
        {
            MESSAGE("Cannot find object in study!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        MCMESHTRAN_ORB::Mesh_var bMesh = bObj->getMesh();
        if (aMesh->_is_nil() || bMesh->_is_nil())
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        //get the return mesh after averaging
        MCMESHTRAN_ORB::Mesh_var cMesh;
        try
        {
            cMesh = engine->averageMeshes(aMesh, bMesh, Name.toLatin1());
            if (cMesh->_is_nil())
            {
                MESSAGE("Return mesh is nil!");
                return false;
            }
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Average failed: " << ex.details.text)  ;
            return false;
        }
        //get the group which the mesh to be append
        MCMESHTRAN_ORB::MeshGroup_var aGroup = engine->getGroup(studyID, GroupID);  //find the group
        if (aGroup->_is_nil())
        {
            MESSAGE("Cannot find the group with GroupID!");
            return false;
        }
        aGroup->appendMesh(cMesh._retn());
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}

/*!
 * \brief move the mesh according to the 3D vector
 * \param aEntry the object
 * \param vector 3D vector
 * \param Name new Name for the mesh
 * \return \c true if succeed
 */
bool MCMESHTRANGUI_DataModel::translateMesh(const QString&  aEntry, const double * vector , const QString&  Name, const int GroupID)
{
    if (Name.trimmed().isEmpty() )
    {
        MESSAGE("Name is empty!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        //find object and get the mesh
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        if (!aObj)
        {
            MESSAGE("Cannot find object in study!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        if (aMesh->_is_nil())
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        //get the return mesh after operation
        MCMESHTRAN_ORB::FixArray3_var aVec = new MCMESHTRAN_ORB::FixArray3;
        aVec [0] = vector[0];
        aVec [1] = vector[1];
        aVec [2] = vector[2];
        MCMESHTRAN_ORB::Mesh_var cMesh;
        try
        {
            cMesh = engine->translateMesh(aMesh, aVec, Name.toLatin1());
            if (cMesh->_is_nil())
            {
                MESSAGE("Return mesh is nil!");
                return false;
            }
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Translate failed: " << ex.details.text)  ;
            return false;
        }
        //get the group which the mesh to be append
        MCMESHTRAN_ORB::MeshGroup_var aGroup = engine->getGroup(studyID, GroupID);  //find the group
        if (aGroup->_is_nil())
        {
            MESSAGE("Cannot find the group with GroupID!");
            return false;
        }
        aGroup->appendMesh(cMesh._retn());
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}

/*!
 * \brief rotate the Mesh
 *  rotate the mesh according to a axis and a angle,
 *  the axis is form by a fix point called \a center and a 3D \a vector
 *  the angle is in radian, 2PI for 360degree.
 * \param aMesh mesh to be rotate
 * \param center fix point in the axis
 * \param vector direction of the axis, doesn't matter if normalized or not
 * \param AngleInRadian angle in radian
 * \param Name new name
 * \param GroupID group to be append
 * \return \c true if succeed
 */
bool MCMESHTRANGUI_DataModel::rotateMesh(const QString&  aEntry, const double * center , const double * vector,
                                         const double AngleInDegree, const QString&  Name , const int GroupID  )
{
    if (Name.trimmed().isEmpty() )
    {
        MESSAGE("Name is empty!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        //find object and get the mesh
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        if (!aObj)
        {
            MESSAGE("Cannot find object in study!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        if (aMesh->_is_nil())
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        //get the return mesh after  operation
        MCMESHTRAN_ORB::FixArray3_var aVec = new MCMESHTRAN_ORB::FixArray3;
        MCMESHTRAN_ORB::FixArray3_var aCtr = new MCMESHTRAN_ORB::FixArray3;
        aVec [0] = vector[0];
        aVec [1] = vector[1];
        aVec [2] = vector[2];
        aCtr [0] = center[0];
        aCtr [1] = center[1];
        aCtr [2] = center[2];
        MCMESHTRAN_ORB::Mesh_var cMesh;
        try
        {
            cMesh = engine->rotateMesh(aMesh, aCtr, aVec, AngleInDegree, Name.toLatin1());
            if (cMesh->_is_nil())
            {
                MESSAGE("Return mesh is nil!");
                return false;
            }
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Rotate failed: " << ex.details.text)  ;
            return false;
        }
        //get the group which the mesh to be append
        MCMESHTRAN_ORB::MeshGroup_var aGroup = engine->getGroup(studyID, GroupID);  //find the group
        if (aGroup->_is_nil())
        {
            MESSAGE("Cannot find the group with GroupID!");
            return false;
        }
        aGroup->appendMesh(cMesh._retn());
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}

/*!
 * \brief scale the mesh coordinate
 *  the scaling takes the global origin as center
 * \param aEntry the object
 * \param Factor scale factor
 * \param Name name for new mesh
 * \param GroupID group with GroupID to be append
 * \return \c true if succeed
 */
bool MCMESHTRANGUI_DataModel::scaleMesh (const QString&  aEntry, const double Factor, const QString&  Name , const int GroupID  )
{
    if (Name.trimmed().isEmpty() )
    {
        MESSAGE("Name is empty!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        //find object and get the mesh
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        if (!aObj)
        {
            MESSAGE("Cannot find object in study!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        if (aMesh->_is_nil())
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        //get the return mesh after operation
        MCMESHTRAN_ORB::Mesh_var cMesh;
        try
        {
            cMesh = engine->scaleMesh(aMesh, Factor, Name.toLatin1());
            if (cMesh->_is_nil())
            {
                MESSAGE("Return mesh is nil!");
                return false;
            }
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Scale failed: " << ex.details.text)  ;
            return false;
        }
        //get the group which the mesh to be append
        MCMESHTRAN_ORB::MeshGroup_var aGroup = engine->getGroup(studyID, GroupID);  //find the group
        if (aGroup->_is_nil())
        {
            MESSAGE("Cannot find the group with GroupID!");
            return false;
        }
        aGroup->appendMesh(cMesh._retn());
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}

/*!
 * \brief multiply the tally result with a const factor,
 *  can be used to convert the unit
 * \param aEntry the object
 * \param Factor multiply factor
 * \param Name name for new mesh
 * \param GroupID group with GroupID to be append
 * \return \c true if succeed
 */
bool MCMESHTRANGUI_DataModel::multiplyFactor(const QString&  aEntry, const double Factor, const QString&  Name , const int GroupID  )
{
    if (Name.trimmed().isEmpty() )
    {
        MESSAGE("Name is empty!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        //find object and get the mesh
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        if (!aObj)
        {
            MESSAGE("Cannot find object in study!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        if (aMesh->_is_nil())
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        //get the return mesh after operation
        MCMESHTRAN_ORB::Mesh_var cMesh;
        try
        {
            cMesh = engine->multiplyFactor(aMesh, Factor, Name.toLatin1());
            if (cMesh->_is_nil())
            {
                MESSAGE("Return mesh is nil!");
                return false;
            }
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Multiply failed: " << ex.details.text)  ;
            return false;
        }
        //get the group which the mesh to be append
        MCMESHTRAN_ORB::MeshGroup_var aGroup = engine->getGroup(studyID, GroupID);  //find the group
        if (aGroup->_is_nil())
        {
            MESSAGE("Cannot find the group with GroupID!");
            return false;
        }
        aGroup->appendMesh(cMesh._retn());
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}



/*!
 *\brief interpolate meshes. both MED and CGNS is implemented, indicate by the option
* \param aEntry  source mesh with solution data
* \param bEntry  target mesh, with or without solution data
* \param Name   name for new mesh
* \param GroupID group to be append
* \param Option, 1-MED+CC, 2-MED+NODE 3-CGNS+CC, 4-CGNS+NODE
* \return  \c true if succeed
*/
bool MCMESHTRANGUI_DataModel::interpolateMesh(const QString&  aEntry,  const QString&  bEntry, const QString&  Name, const int GroupID , int Option )
{
    if (Name.trimmed().isEmpty() )
    {
        MESSAGE("Name is empty!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    MCMESHTRAN_ORB::Mesh_var aMesh = findObject(aEntry)->getMesh();
    MCMESHTRAN_ORB::Mesh_var bMesh = findObject(bEntry)->getMesh();
    if ( studyID && !CORBA::is_nil( engine ) && !aMesh->_is_nil() && !bMesh->_is_nil())
    {
        MCMESHTRAN_ORB::MeshGroup_var theGroup = engine->getGroup(studyID, GroupID);
        if (theGroup->_is_nil())
        {
            MESSAGE("Cannot find the group!")  ;
            return false;
        }
        try {
            MCMESHTRAN_ORB::SolutionLoc aSolloc;
            MCMESHTRAN_ORB::Mesh_var cMesh;
            if (Option == 1) //MED method, solution located in cell center
            {
                aSolloc = MCMESHTRAN_ORB::on_cells;
                cMesh = engine->interpolateMEDMesh(aMesh, bMesh, Name.toLatin1(),aSolloc);
            }
            else if (Option == 2) //MED method, solution located in nodes
            {
                aSolloc = MCMESHTRAN_ORB::on_nodes;
                cMesh = engine->interpolateMEDMesh(aMesh, bMesh, Name.toLatin1(),aSolloc);
            }
#ifdef WITH_CGNS
            else if (Option == 3) //CGNS method, solution located in cell center
            {
                aSolloc = MCMESHTRAN_ORB::on_cells;
                cMesh = engine->interpolateCGNSMesh(aMesh, bMesh, Name.toLatin1(),aSolloc);
            }
            else if (Option == 4) //CGNS method, solution located in nodes
            {
                aSolloc = MCMESHTRAN_ORB::on_nodes;
                cMesh = engine->interpolateCGNSMesh(aMesh, bMesh, Name.toLatin1(),aSolloc);
            }
#endif
            else
            {
                MESSAGE("Unknown option!");
                return false;
            }
            theGroup->appendMesh(cMesh._retn());

        }
        catch (SALOME::SALOME_Exception &ex) {
            MESSAGE("Interpolate failed: " << ex.details.text)  ;
            return false;
        }
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}



/*!
 * \brief export to VTK format
 * \param aEntry the mesh to be export
 * \param FileName file name
 * \return
 */
bool MCMESHTRANGUI_DataModel::export2VTK (const QString&  aEntry, const QString&  FileName)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    QString newFileName;
    if (FileName.indexOf(".vtk", 1, Qt::CaseInsensitive) < 0)
        //if no .vtk surfix, add it
        newFileName = FileName + ".vtk";
    else
        newFileName = FileName;

    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    if ( !CORBA::is_nil( engine ) )
    {
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        if (!aObj || aObj->isMeshGroup())
        {
            MESSAGE("Cannot find the object, or it is invalid!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        if (aMesh->_is_nil() )
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        try
        {
            engine->export2VTK(aMesh, newFileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Export failed: " << ex.details.text)  ;
            return false;
        }
        return true;
    }
    MESSAGE("Errors in engine!");
    return false;
}
/*!
 * \brief export to MED format
 * \param aEntry the mesh to be export
 * \param FileName file name
 * \return
 */
bool MCMESHTRANGUI_DataModel::export2MED (const QString&  aEntry, const QString&  FileName)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    QString newFileName;
    if (FileName.indexOf(".med", 1, Qt::CaseInsensitive) < 0)
        //if no .med surfix, add it
        newFileName = FileName + ".med";
    else
        newFileName = FileName;
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    if ( !CORBA::is_nil( engine ) )
    {
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        if (!aObj || aObj->isMeshGroup())
        {
            MESSAGE("Cannot find the object, or it is invalid!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        if (aMesh->_is_nil() )
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        try
        {
            engine->export2MED(aMesh, newFileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("In " << ex.details.lineNumber);
            MESSAGE("Export failed: in " << ex.details.text)  ;
            return false;
        }
        return true;
    }
    MESSAGE("Errors in engine!");
    return false;
}
/*!
 * \brief export to Comma Separate Value (CSV) format,
 *  a text format which data is arranged in column with coomma as separator
 * \param aEntry the mesh to be export
 * \param FileName file name
 * \return
 */
bool MCMESHTRANGUI_DataModel::export2CSV (const QString&  aEntry, const QString&  FileName)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    QString newFileName;
    if (FileName.indexOf(".csv", 1, Qt::CaseInsensitive) < 0)
        //if no .csv surfix, add it
        newFileName = FileName + ".csv";
    else
        newFileName = FileName;
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    if ( !CORBA::is_nil( engine ) )
    {
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        if (!aObj || aObj->isMeshGroup())
        {
            MESSAGE("Cannot find the object, or it is invalid!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        if (aMesh->_is_nil() )
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        try
        {
            engine->export2CSV(aMesh, newFileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Export failed: " << ex.details.text)  ;
            return false;
        }
        return true;
    }
    MESSAGE("Errors in engine!");
    return false;
}


#ifdef WITH_CGNS
bool  MCMESHTRANGUI_DataModel::export2CGNS (const QString aEntry, const QString FileName)
{
    if (FileName.trimmed().isEmpty() ||  aEntry.trimmed().isEmpty())
    {
        MESSAGE("File name or entry is empty!");
        return false;
    }
    QString newFileName;
    if (FileName.indexOf(".cgns", 1, Qt::CaseInsensitive) < 0)
        //if no .cgns surfix, add it
        newFileName = FileName + ".cgns";
    else
        newFileName = FileName;
    //get the field first
    if (!findObject(aEntry) || !findObject(aEntry)->isMesh() ||
            findObject(aEntry)->getMesh()->_is_nil() )
    {
        MESSAGE("Error in the entry, or is not mesh, or mesh is nil, or field is nil!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    if ( !CORBA::is_nil( engine ) )
    {
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        if (!aObj || aObj->isMeshGroup())
        {
            MESSAGE("Cannot find the object, or it is invalid!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        if (aMesh->_is_nil() )
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        try
        {
            engine->export2CGNS(aMesh, newFileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Export failed: " << ex.details.text)  ;
            return false;
        }
        return true;
    }
    MESSAGE("Errors in engine!");
    return false;
}
#endif //WITH_CGNS

/*!
 * \brief export to FLUENT format
 * \param aEntry the mesh to be export
 * \param FileName file name
 * \return
 */
bool MCMESHTRANGUI_DataModel::export2FLUENT (const QString&  aEntry, const QString&  FileName)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    QString newFileName;
    if (FileName.indexOf(".udf", 1, Qt::CaseInsensitive) < 0)
        //if no .udf surfix, add it
        newFileName = FileName + ".udf";
    else
        newFileName = FileName;

    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    if ( !CORBA::is_nil( engine ) )
    {
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        if (!aObj || aObj->isMeshGroup())
        {
            MESSAGE("Cannot find the object, or it is invalid!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        if (aMesh->_is_nil() )
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        try
        {
            engine->export2FLUENT(aMesh, newFileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Export failed: " << ex.details.text)  ;
            return false;
        }
        return true;
    }
    MESSAGE("Errors in engine!");
    return false;
}

/*!
 * \brief export to CFX format
 * \param aEntry the mesh to be export
 * \param FileName file name
 * \return
 */
bool MCMESHTRANGUI_DataModel::export2CFX (const QString&  aEntry, const QString&  FileName)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    QString newFileName;
    if (FileName.indexOf(".F", 1, Qt::CaseInsensitive) < 0)
        //if no .F surfix, add it
        newFileName = FileName + ".F";
    else
        newFileName = FileName;

    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    if ( !CORBA::is_nil( engine ) )
    {
        MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
        if (!aObj || aObj->isMeshGroup())
        {
            MESSAGE("Cannot find the object, or it is invalid!");
            return false;
        }
        MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
        if (aMesh->_is_nil() )
        {
            MESSAGE("Mesh is nil!");
            return false;
        }
        try
        {
            engine->export2CFX(aMesh, newFileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Export failed: " << ex.details.text)  ;
            return false;
        }
        return true;
    }
    MESSAGE("Errors in engine!");
    return false;
}

/*!
 * \brief export several meshes into a abaqus file
 * \param aList the mesh to be export
 * \param FileName export file name
 * \return  false if failed
 */
bool  MCMESHTRANGUI_DataModel::exportMesh2Abaqus (const QStringList& aList, const QString& FileName)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    if (aList.size() == 0) {
        MESSAGE("No object to be export!");
        return false;
    }
    QString newFileName;
    if (FileName.indexOf(".inp", 1, Qt::CaseInsensitive) < 0)
        //if no .inp surfix, add it
        newFileName = FileName + ".inp";
    else
        newFileName = FileName;

    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    if ( !CORBA::is_nil( engine ) )
    {
        MCMESHTRAN_ORB::MeshList_var aMeshList = new MCMESHTRAN_ORB::MeshList;
        //append all the mesh in to the list
        for (int i=0; i<aList.size(); i++) {
            MCMESHTRANGUI_DataObject * aObj = findObject(aList[i]);
            if (!aObj || aObj->isMeshGroup())
            {
                MESSAGE("Cannot find the object, or it is invalid!");
                return false;
            }
            MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
            if (aMesh->_is_nil() )
            {
                MESSAGE("Mesh is nil!");
                return false;
            }
            aMeshList->length(aMeshList->length() +1);
            aMeshList[aMeshList->length() -1] = aMesh._retn();
        }
        try
        {
            engine->exportMesh2Abaqus(aMeshList, newFileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Export failed: " << ex.details.text)  ;
            return false;
        }
        return true;
    }
    MESSAGE("Errors in engine!");
    return false;
}


/*!
 * \brief import MED mesh
 *  mesh in MED file can be pure mesh, or with field data
 * \param FileName
 * \return \c true if succeed
 */
bool MCMESHTRANGUI_DataModel::importMED  (const QString&  FileName,const int GroupID)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        MCMESHTRAN_ORB::GroupList * aGroupList = engine->getData(studyID);
        if (!aGroupList)
        {
            MESSAGE("Group list is emtpy!")
            return false;
        }
        MCMESHTRAN_ORB::MeshGroup_var aGroup;
        try
        {
             aGroup = engine->importMED(FileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Import failed: " << ex.details.text)  ;
            return false;
        }
        if (aGroup->_is_nil())
        {
            MESSAGE("Return a nil group!")
            return false;
        }
        if (GroupID < 0) //create new group
        {
            aGroupList->length(aGroupList->length()+1);// increase a space for new group
            (*aGroupList)[aGroupList->length()-1] = aGroup._retn();
        }
        else
        {
            MCMESHTRAN_ORB::MeshGroup_var bGroup;
            bGroup = engine->getGroup(studyID, GroupID);
            bGroup->appendMeshInGroup(aGroup);
        }
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}


#ifdef WITH_CGNS
bool  MCMESHTRANGUI_DataModel::importCGNS (const QString& FileName, const int GroupID  )
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        MCMESHTRAN_ORB::GroupList * aGroupList = engine->getData(studyID);
        if (!aGroupList)
        {
            MESSAGE("Group list is emtpy!")
            return false;
        }
        MCMESHTRAN_ORB::MeshGroup_var aGroup;
        try
        {
             aGroup = engine->importCGNS(FileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Import failed: " << ex.details.text)  ;
            return false;
        }
        if (aGroup->_is_nil())
        {
            MESSAGE("Return a nil group!")
            return false;
        }
        if (GroupID < 0) //create new group
        {
            aGroupList->length(aGroupList->length()+1);// increase a space for new group
            (*aGroupList)[aGroupList->length()-1] = aGroup._retn();
        }
        else
        {
            MCMESHTRAN_ORB::MeshGroup_var bGroup;
            bGroup = engine->getGroup(studyID, GroupID);
            bGroup->appendMeshInGroup(aGroup);
        }
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}
#endif //WITH_CGNS


bool  MCMESHTRANGUI_DataModel::importAbaqus (const QString& FileName, const int GroupID  )
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    //get engine and studyID
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        MCMESHTRAN_ORB::GroupList * aGroupList = engine->getData(studyID);
        if (!aGroupList)
        {
            MESSAGE("Group list is emtpy!")
            return false;
        }
        MCMESHTRAN_ORB::MeshGroup_var aGroup;
        try
        {
             aGroup = engine->importAbaqus(FileName.toLatin1());
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Import failed: " << ex.details.text)  ;
            return false;
        }
        if (aGroup->_is_nil())
        {
            MESSAGE("Return a nil group!")
            return false;
        }
        if (GroupID < 0) //create new group
        {
            aGroupList->length(aGroupList->length()+1);// increase a space for new group
            (*aGroupList)[aGroupList->length()-1] = aGroup._retn();
        }
        else
        {
            MCMESHTRAN_ORB::MeshGroup_var bGroup;
            bGroup = engine->getGroup(studyID, GroupID);
            bGroup->appendMeshInGroup(aGroup);
        }
        return true;
    }
    MESSAGE("Errors in study or in engine!");
    return false;
}

bool  MCMESHTRANGUI_DataModel::importUNVMeshOnly (const QString& FileName,const int GroupID)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    //get the SMESH Engine
    SUIT_Application* suitApp = SUIT_Session::session()->activeApplication();
    SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( suitApp );
    if (!app)
    {
        MESSAGE("Error in getting application!");
        return false;
    }
    Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component("FactoryServer", "SMESH" );
    if ( CORBA::is_nil(comp  ) )
        MESSAGE("Please Open SMESH Component!");
    SMESH::SMESH_Gen_var aSMESHGen = SMESH::SMESH_Gen::_narrow(comp);
    if ( aSMESHGen->_is_nil() ) return false;
    MESSAGE("Obtain SMESH Engine OK!");

    //load the UNV file
    try {
            SMESH::SMESH_Mesh_var aSMESH_var  = aSMESHGen->CreateMeshesFromUNV(FileName.toLatin1());
            QString aTmpDir = SUIT_Tools::addSlash(SUIT_Tools::tmpDir())  + SUIT_Tools::file(FileName, false) + QString(".med");
            QFile::remove(aTmpDir);
            aSMESH_var->ExportMED(aTmpDir.toLatin1(), true); //first export to MED mesh file
            //2013-06-03 clear the mesh in SMESH after loading, if not will spend a lot of memory
            aSMESH_var->Clear();
            if (!importMED(aTmpDir, GroupID))                  //then import it
            {
                MESSAGE("Error in importing temporary MED mesh!");
                return false;
            }
            QFile::remove(aTmpDir);
    }
    catch (SALOME::SALOME_Exception &ex)
    {
        MESSAGE("Export failed: " << ex.details.text);
        return false;
    }
    return true;
}

bool  MCMESHTRANGUI_DataModel::importSAUVMeshOnly (const QString& FileName,const int GroupID)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    //get the SMESH Engine
    SUIT_Application* suitApp = SUIT_Session::session()->activeApplication();
    SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( suitApp );
    if (!app)
    {
        MESSAGE("Error in getting application!");
        return false;
    }
    Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component("FactoryServer", "SMESH" );
//    Engines::EngineComponent_var comp = MCMESHTRANGUI::GetSMESHGen();
    if ( CORBA::is_nil(comp  ) )
        MESSAGE("Please Open SMESH Component!");
    SMESH::SMESH_Gen_var aSMESHGen = SMESH::SMESH_Gen::_narrow(comp);
    if ( aSMESHGen->_is_nil() ) return false;
    MESSAGE("Obtain SMESH Engine OK!");

    //load the SAUV file and
    SMESH::DriverMED_ReadStatus aStatus;
    try {
        SMESH::mesh_array_var aSMESHList = aSMESHGen->CreateMeshesFromSAUV(FileName.toLatin1(), aStatus);
        if (aStatus != SMESH::DRS_OK )
        {
            MESSAGE("problems in reading!" << aStatus);
            return false;
        }
        QString aTmpDir = SUIT_Tools::addSlash(SUIT_Tools::tmpDir())  + SUIT_Tools::file(FileName, false) + QString(".med");
        for (int i=0; i<aSMESHList->length(); i++)
        {
//            SMESH::SMESH_Mesh_var aSMESH_var = (*aSMESHList)[i];
            SMESH::SMESH_Mesh_var aSMESH_var = aSMESHList[i];
            QFile::remove(aTmpDir);
            aSMESH_var->ExportMED(aTmpDir.toLatin1(), true); //first export to MED mesh file
            //2013-06-03 clear the mesh in SMESH after loading, if not will spend a lot of memory
            aSMESH_var->Clear();
            if (!importMED(aTmpDir, GroupID))                  //then import it
            {
                MESSAGE("Error in importing temporary MED mesh!" << aStatus);
                return false;
            }
            QFile::remove(aTmpDir);
        }
    }
    catch (SALOME::SALOME_Exception &ex)
    {
        MESSAGE("Export failed: " << ex.details.text);
        return false;
    }
    return true;
}

bool  MCMESHTRANGUI_DataModel::importSTLMeshOnly (const QString& FileName,const int GroupID)
{
    if (FileName.trimmed().isEmpty() )
    {
        MESSAGE("File name is empty!");
        return false;
    }
    //get the SMESH Engine
    SUIT_Application* suitApp = SUIT_Session::session()->activeApplication();
    SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( suitApp );
    if (!app)
    {
        MESSAGE("Error in getting application!");
        return false;
    }
    Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component("FactoryServer", "SMESH" );
    if ( CORBA::is_nil(comp  ) )
        MESSAGE("Please Open SMESH Component!");
    SMESH::SMESH_Gen_var aSMESHGen = SMESH::SMESH_Gen::_narrow(comp);
    if ( aSMESHGen->_is_nil() ) return false;
    MESSAGE("Obtain SMESH Engine OK!");

    //load the STL file
    try {
            SMESH::SMESH_Mesh_var aSMESH_var  = aSMESHGen->CreateMeshesFromSTL(FileName.toLatin1());
            QString aTmpDir = SUIT_Tools::addSlash(SUIT_Tools::tmpDir()) + SUIT_Tools::file(FileName, false) + QString(".med");
            QFile::remove(aTmpDir);
            aSMESH_var->ExportMED(aTmpDir.toLatin1(), true); //first export to MED mesh file
            //2013-06-03 clear the mesh in SMESH after loading, if not will spend a lot of memory
            aSMESH_var->Clear();
            if (!importMED(aTmpDir, GroupID))                  //then import it
            {
                MESSAGE("Error in importing temporary MED mesh!" );
                return false;
            }
            QFile::remove(aTmpDir);
    }
    catch (SALOME::SALOME_Exception &ex)
    {
        MESSAGE("Export failed: " << ex.details.text);
        return false;
    }
    return true;
}


void  MCMESHTRANGUI_DataModel::deleteGroup(const QString& aEntry)
{
    MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() || !aObj || !aObj->isMeshGroup())
    {
        MESSAGE("Error in Entry");
        return;
    }
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        if (!engine->deleteGroup(studyID, getGroupID(aEntry)))
            MESSAGE("Delete group failed!");
        //remove the object
        aObj->parent()->removeChild(aObj);
        return;
    }
    MESSAGE("Error in study or engine!");
}

void  MCMESHTRANGUI_DataModel::deleteGroups(const QStringList& aList)
{
    if (aList.size() == 0)
    {
        MESSAGE("No group to be deleted");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        deleteGroup(aList.at(i));
}

void MCMESHTRANGUI_DataModel::renameGroup(const QString& aEntry, const QString& newName)
{
    MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() ||  newName.trimmed().isEmpty() ||
            !aObj || !aObj->isMeshGroup())
    {
        MESSAGE("Error in Entry or name!");
        return;
    }

    aObj->getGroup()->setName(newName.toLatin1());
}

void  MCMESHTRANGUI_DataModel::deleteMesh (const QString& aEntry)
{
    MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
    if(aEntry.trimmed().isEmpty() || !aObj || !aObj->isMesh())
    {
        MESSAGE("Error in Entry");
        return;
    }

    if(!aObj->getGroup()->deleteMesh( getMeshID(aEntry )))
        MESSAGE("Delete mesh failed!");
    //remove the object from the tree
    aObj->parent()->removeChild(aObj);

}

void  MCMESHTRANGUI_DataModel::deleteMeshes (const QStringList& aList)
{
    if (aList.size() == 0)
    {
        MESSAGE("No mesh to be deleted");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        deleteMesh(aList.at(i));
}

void MCMESHTRANGUI_DataModel::renameMesh(const QString& aEntry, const QString& newName)
{
    if(aEntry.trimmed().isEmpty() ||  newName.trimmed().isEmpty() ||
            !findObject(aEntry) || !findObject(aEntry)->isMesh())
    {
        MESSAGE("Error in Entry or Name!");
        return;
    }

    findObject(aEntry)->getMesh()->setName(newName.toLatin1());
}

void  MCMESHTRANGUI_DataModel::copyMesh(const QString& aEntry)
{
    if(aEntry.trimmed().isEmpty() ||  !findObject(aEntry) || !findObject(aEntry)->isMesh())
    {
        MESSAGE("Error in Entry !");
        return;
    }
//    CopyList->length(CopyList->length()+1); //increase a space
//    CopyList[CopyList->length()-1]  = findObject(aEntry)->getMesh();
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    if (!CORBA::is_nil(engine))
    {
        try
        {
            CopyList->length(CopyList->length()+1); //increase a space
            CopyList[CopyList->length()-1]  =
                    engine->deepCopyMesh((findObject(aEntry)->getMesh()));
        }
        catch (SALOME::SALOME_Exception &ex)
        {
            MESSAGE("Copy failed: " << ex.details.text)  ;
        }
        return;
    }
    MESSAGE("Error in engine !");
}

void  MCMESHTRANGUI_DataModel::copyMeshes (const QStringList& aList)
{
    CopyList->length(0);  //clear
    if (aList.size() == 0)
    {
        MESSAGE("No mesh to be copy");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        copyMesh(aList.at(i));
}

void   MCMESHTRANGUI_DataModel::cutMesh(const QString& aEntry)
{
    if(aEntry.trimmed().isEmpty() ||  !findObject(aEntry) || !findObject(aEntry)->isMesh())
    {
        MESSAGE("Error in Entry !");
        return;
    }
    CutList->length(CutList->length()+1); //increase a space
    CutList[CutList->length()-1]  = findObject(aEntry)->getMesh();

    deleteMesh(aEntry);  //delete after cut
}

void   MCMESHTRANGUI_DataModel::cutMeshes(const QStringList& aList)
{
//    CutList->length(0); //no need to be clear
    if (aList.size() == 0)
    {
        MESSAGE("No mesh to be cut");
        return;
    }
    for (int i=0; i<aList.size(); i++)
        cutMesh(aList.at(i));
}

//paste mesh before aEntry
void  MCMESHTRANGUI_DataModel::pasteMeshes(const QString& aEntry)
{
    if(aEntry.trimmed().isEmpty() ||  !findObject(aEntry) )
    {
        MESSAGE("Error in Entry !");
        return;
    }

    MCMESHTRANGUI_DataObject * aObj = findObject(aEntry);
    if (aObj ->isMeshGroup())
    {
        for (int i=0; i<CopyList->length(); i++)
            aObj->getGroup()->appendMesh(CopyList[i]);
        for (int i=0; i<CutList->length(); i++)
            aObj->getGroup()->appendMesh(CutList[i]);
        CopyList->length(0);
        CutList->length(0);

    }
    else if(aObj->isMesh())
    {
        for (int i=0; i<CopyList->length(); i++)
            aObj->getGroup()->insertMesh(CopyList[i], aObj->getMeshID());
        for (int i=0; i<CutList->length(); i++)
            aObj->getGroup()->insertMesh(CutList[i], aObj->getMeshID());
        CopyList->length(0);
        CutList->length(0);
    }
    else
        MESSAGE("Unable to paste here!");
}


/*! Called on update of the structure of Data Objects */
void MCMESHTRANGUI_DataModel::build()
{
    buildTree();
}

void MCMESHTRANGUI_DataModel::buildTree()

{
//    if (count != 5) {
    MCMESHTRANGUI_ModuleObject* modelRoot = dynamic_cast<MCMESHTRANGUI_ModuleObject*>( root() );
    if( !modelRoot )  {  // root is not set yet
        modelRoot = new MCMESHTRANGUI_ModuleObject( this, 0 );
        setRoot( modelRoot );
    }

    // create 'molecule' objects under model root object and 'atom' objects under 'molecule'-s
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( studyID && !CORBA::is_nil( engine ) )
    {
        MCMESHTRAN_ORB::GroupList * aGroupList = engine->getData(studyID);
        if (!aGroupList)
        {
            //SUIT_MessageBox::warning(desktop(), tr("Warning"), tr( "MCMESHTRANGUI_DataModel::build: Group list is emtpy"));
            MESSAGE("Group list is emtpy")
            return;
        }
        const int n = aGroupList->length();
        for (int i=0; i<n; i++)
        {
            MCMESHTRAN_ORB::MeshGroup_var aGroup = (*aGroupList)[i];
            MCMESHTRANGUI_DataObject * aGroupObject = NULL;
            for ( SUIT_DataObjectIterator it( modelRoot, SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
            {
                MCMESHTRANGUI_DataObject* obj = dynamic_cast<MCMESHTRANGUI_DataObject*>( it.current() );
                if (obj && obj->isMeshGroup() && obj->getGroupID() == aGroup->getID())
                {
                    aGroupObject = obj;
                    break;
                }
            }
            if (aGroupObject == NULL)
                aGroupObject = new MCMESHTRANGUI_DataObject (modelRoot,aGroup->getID());

//            MCMESHTRAN_ORB::MeshList_var aMeshList = aGroup->getMeshes();
            MCMESHTRAN_ORB::MeshList * aMeshList = aGroup->getMeshList();
            const int m = aMeshList->length();
            for (int j=0; j<m; j++)
            {
                MCMESHTRAN_ORB::Mesh_var aMesh = (*aMeshList)[j];
                MCMESHTRANGUI_DataObject * aMeshObject = NULL;
                for ( SUIT_DataObjectIterator it( modelRoot, SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it )
                {
                    MCMESHTRANGUI_DataObject* obj = dynamic_cast<MCMESHTRANGUI_DataObject*>( it.current() );
                    if (obj && obj->isMesh() && obj->getMeshID() == aMesh->getID())
                    {
                        aMeshObject = obj;
                        break;
                    }
                }
                if (aMeshObject == NULL)
                    aMeshObject = new MCMESHTRANGUI_DataObject(aGroupObject,aGroup->getID(), aMesh->getID() );
            }
        }
    }
    modelRoot->setDataModel( this );
    setRoot( modelRoot );
//    count ++;
    return;
//    }

}

bool  MCMESHTRANGUI_DataModel::sendMesh2SMESH(const QStringList& aList)
{
    //checking
    if (aList.isEmpty()) {
        MESSAGE("the list of Mesh is empty!");
        return false;
    }
    //get the smesh engine
    //get the SMESH Engine
    SUIT_Application* suitApp = SUIT_Session::session()->activeApplication();
    SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( suitApp );
    if (!app)
    {
        MESSAGE("Error in getting application!");
        return false;
    }
    Engines::EngineComponent_var comp = app->lcc()->FindOrLoad_Component("FactoryServer", "SMESH" );
    if ( CORBA::is_nil(comp  ) )
        MESSAGE("Please Open SMESH Component!");
    SMESH::SMESH_Gen_var aSMESHGen = SMESH::SMESH_Gen::_narrow(comp);
    if (aSMESHGen->_is_nil()) {
        MESSAGE("Please open the SMESH module before this operation!");
        return false;
    }
    //get MCMESHTRAN engine
    MCMESHTRAN_ORB::MCMESHTRAN_Gen_var engine = MCMESHTRANGUI::GetMCMESHTRANGen();
    const int studyID = getStudyID();
    if ( !studyID || CORBA::is_nil( engine ) ) return false;

    for (int i=0; i<aList.size(); i++) {
        MCMESHTRANGUI_DataObject * aObj = findObject(aList[i]);
        if (!aObj) continue;  //if cannot get the object, next
        if (aObj->isMesh()) {
            MCMESHTRAN_ORB::Mesh_var aMesh = aObj->getMesh();
            if (aMesh->_is_nil()) {
                MESSAGE("This object has a nil Mesh!");
                continue;
            }

            //get the mesh
//            ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingUMesh> aUMesh
//                    =ParaMEDMEM::MEDCouplingUMeshClient::New(aUMeshIOR);
            //write the mesh into a temp file
            QString  aTempDir = "tmpfilemcmeshtran2smesh.med";
            QFile::remove(aTempDir); //remove the file first if any
            try {
                engine->export2MED(aMesh, aTempDir.toLatin1());

                SMESH::DriverMED_ReadStatus aStatus;
                aSMESHGen->CreateMeshesFromMED(aTempDir.toLatin1(), aStatus);
                if (aStatus != SMESH::DRS_OK )
                {
                    MESSAGE("problems in reading!" << aStatus);
                    QFile::remove(aTempDir); //remove if any
                    return false;
                }
            }
            catch (SALOME::SALOME_Exception &ex)
            {
                QFile::remove(aTempDir); //remove if any
                MESSAGE("Export failed: " << ex.details.text);
                return false;
            }
            QFile::remove(aTempDir); //remove if any
        }
        else {
            MESSAGE("This object type has no mesh!");
            continue;
        }
    }
    return true;
}


