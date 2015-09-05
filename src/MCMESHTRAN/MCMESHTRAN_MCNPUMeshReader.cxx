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
#include "MCMESHTRAN_MCNPUMeshReader.hxx"
#include <stdio.h>
#include <math.h>

#include <QStringList>
#include <QFile>
#include <QFileInfo>

#include <utilities.h>
#include <MEDCouplingUMesh.hxx>

MCMESHTRAN_MCNPUMeshReader::MCMESHTRAN_MCNPUMeshReader()
{
    myNbNodes       = 0;
    myNbInstance    = 0;
    myNbElmTotal    = 0;
    for (int i=0; i<6; i++) {
        myNbElements[i] = 0;
    }


    myNbComposits   = 0;
    myNPS           = 0;
    myNbRegEdit     = 0;
    myNbCompEdit    = 0;
    myLengthFactor  = 1.0;

    removeMultiplier= false;  //default: does not remove the multiplier
    keepInstances   = false;    //default: discard to speed up the data processing

    myUMeshOutFileName      = "";
    EndOfFile               = false;
    Identifiers.hasTitle    = false;
    Identifiers.NbDataRec   = 0;
    Identifiers.DataType    = Data_unkown;
    Identifiers.SizeofType  = 0;
    Identifiers.NbItems     = 0;
    Identifiers.ParseLength = 0;
    lineNb         = 0;
    lineNbProc     = 0;
    currentLine     = "";
    EditPrefix      = "";
    NodeCoordX      = DataArrayDouble::New();
    NodeCoordY      = DataArrayDouble::New();
    NodeCoordZ      = DataArrayDouble::New();
    ElmConn         = DataArrayInt::New();
    ElmConnIdx      = DataArrayInt::New();

    ElmNodeCnt[Elm_1st_tet]     = 4;
    ElmNodeCnt[Elm_1st_pent]    = 6;
    ElmNodeCnt[Elm_1st_hexa]    = 8;
    ElmNodeCnt[Elm_2nd_tet]     = 10;
    ElmNodeCnt[Elm_2nd_pent]    = 15;
    ElmNodeCnt[Elm_2nd_hexa]    = 20;

    MEDElmType[Elm_1st_tet]     = INTERP_KERNEL::NORM_TETRA4;
    MEDElmType[Elm_1st_pent]    = INTERP_KERNEL::NORM_PENTA6;
    MEDElmType[Elm_1st_hexa]    = INTERP_KERNEL::NORM_HEXA8;
    MEDElmType[Elm_2nd_tet]     = INTERP_KERNEL::NORM_TETRA10;
    MEDElmType[Elm_2nd_pent]    = INTERP_KERNEL::NORM_PENTA15;
    MEDElmType[Elm_2nd_hexa]    = INTERP_KERNEL::NORM_HEXA20;
//    ParticleType[1]               = "Neutron";
//    ParticleType[2]               = "Photon";
//    ParticleType[9]               = "Proton";
    ParticleType[1]               = "N";
    ParticleType[2]               = "P";
    ParticleType[9]               = "H";

    MeshConnOrder = Unknown_Ordered;
    myMCNPVersion = MCNP_unknown;

}
MCMESHTRAN_MCNPUMeshReader::~MCMESHTRAN_MCNPUMeshReader()
{
    NodeCoordX->decrRef();//to decrese the reference incase memory leakage
    NodeCoordY->decrRef();
    NodeCoordZ->decrRef();
//    NodeCoord->decrRef();
    ElmConn->decrRef();
    ElmConnIdx->decrRef();
    for (int i=0; i<myFieldList.size(); i++)  //decrease reference,
        myFieldList[i]->decrRef();
    for (int i=0; i<DataList.size(); i++)
        DataList[i]->decrRef();

    if (InstanceList.size() != 0) {  //if has instance, delete them
        for (int i=0; i<InstanceList.size(); i++) {
            delete InstanceList[i];
        }
    }

}

/*!
 * \brief load and process a MCNP UMesh eeout file
 * \return false if failed
 */
bool MCMESHTRAN_MCNPUMeshReader::load_file(const QString &UMeshOutFileName)
{
    //open the file
    myUMeshOutFileName = UMeshOutFileName.toStdString();
    myUMeshOutFile.open(myUMeshOutFileName.c_str());
    if(myUMeshOutFile.is_open())
        cout << "File " << myUMeshOutFileName << " is open.\n";
    else    {
        cout << "Error opening " << myUMeshOutFileName  << ".\n";
        return false;
    }

    //skip the first line which is no useful
    get_aLine();
    if (EndOfFile) {
        MESSAGE("File ended unexpectedly!" );
        return false;
    }

    //start the loop
    bool Ok = true;
    get_aLine(); //get the first identification line
    while(!EndOfFile)
    {
        if (!get_aBlock()) {  //get a block
            MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                    ": Get block error!" );
            return false;
        }
        UMeshBlockType theBlockType = get_block_type();  //get the block type
        switch (theBlockType)
        {
        case Block_first_set:
            break; //skip the block
        case Block_calling_code:
            Ok = read_calling_code();
            break;
        case Block_int_paras:
            Ok = read_int_paras();
            break;
        case Block_real_paras:
            Ok = read_real_paras();
            break;
        case Block_particle_list:
            break;  //skip the block
        case Block_edit_description:
            Ok = read_edit_description();
            break;
        case Block_composite_edit_limit:
            break;  //skip the block
        case Block_materials:
            break;  //skip the block
        case Block_instance_cumulative:
            break;  //skip the block
        case Block_instance_names:
            Ok = read_instance_names();
            break;
        case Block_instance_element_type:
            Ok = read_instance_element_type();
            break;
        case Block_node_x:
            Ok = read_nodes_x();
            break;
        case Block_node_y:
            Ok = read_nodes_y();
            break;
        case Block_node_z:
            Ok = read_nodes_z();
            break;
        case Block_element_type:
            break;  //skip the block
        case Block_element_material:
//            Ok = read_material();
            break;
        case Block_conn_1st_tet:
            Ok = read_connectivities(Elm_1st_tet);
            break;
        case Block_conn_1st_penta:
            Ok = read_connectivities(Elm_1st_pent);
            break;
        case Block_conn_1st_hexa:
            Ok = read_connectivities(Elm_1st_hexa);
            break;
        case Block_conn_2nd_tet:
            Ok = read_connectivities(Elm_2nd_tet);
            break;
        case Block_conn_2nd_penta:
            Ok = read_connectivities(Elm_2nd_pent);
            break;
        case Block_conn_2nd_hexa:
            Ok = read_connectivities(Elm_2nd_hexa);
            break;
        case Block_nearest_neighbor:
            break;  //skip the block
        case Block_edit_output:
            Ok = read_edit_output();
            break;
        case Block_edit_set:
            Ok = read_edit_sets();
            break;
        case Block_centroid:
            break; //skip the block
        case Block_density:
            break; //skip the block
        case Block_volume:
            break; //skip the block
        default:
            MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                    ": Unknown data block!" );
            return false;
        }
        if (!Ok) {
            MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                    ": Read file failed!" );
            return false;
        }
        get_aLine(); // get the new identification line
    }

    if (!process_data()) {
        MESSAGE("Error: Process data failed!" );
        return false;
    }

    return true;
}

/*!
 * \brief MCMESHTRAN_MCNPUMeshReader::read_calling_code
 *  read the code version
 * \return
 */
bool MCMESHTRAN_MCNPUMeshReader::read_calling_code()
{
    for (int i=0; i<currentBlock.size();i++) {
        QStringList aStringList = get_line_in_block(i).split(":", QString::SkipEmptyParts);
        if (aStringList.at(0).contains("Code Version")) {
            if (aStringList.at(1).contains("1.0"))
                myMCNPVersion = MCNP6_1_0;
            else if (aStringList.at(1).contains("6.1.1b"))
                myMCNPVersion = MCNP6_1_1_b;
            else {
                myMCNPVersion = MCNP_unknown;
            return false;
            }

            break;
        }
    }
    return true;

}

/*!
 * \brief read integer parameters
 * \return  false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::read_int_paras()
{
    // the integer data block should contain 14 lines
    if (currentBlock.size() != 14) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": number of lines in Integer data block is not correct!" );
        return false;
    }
    for (int i=0; i<14; i++) {
        QStringList aStringList = get_line_in_block(i).split(":", QString::SkipEmptyParts);
        bool Ok = true;
        switch (i)
        {
        case 0 :  //particle number
            break;
        case 1 :  //nb of nodes
            myNbNodes      = aStringList[1].toInt(&Ok);
            break;
        case 2 :  //nb of materials
             break;
        case 3 :  //nb of instances
            myNbInstance   = aStringList[1].toInt(&Ok);
            break;
        case 4 :  //1st tet
            myNbElements[0]= aStringList[1].toInt(&Ok);
            break;
        case 5 :  //1st pent
            myNbElements[1]= aStringList[1].toInt(&Ok);
            break;
        case 6 :  //1st hexa
            myNbElements[2]= aStringList[1].toInt(&Ok);
            break;
        case 7 :  //2nd tet
            myNbElements[3]= aStringList[1].toInt(&Ok);
            break;
        case 8 :  //2nd pent
            myNbElements[4]= aStringList[1].toInt(&Ok);
            break;
        case 9 :  //2nd hexa
            myNbElements[5]= aStringList[1].toInt(&Ok);
            break;
        case 10:  //nb of componsites
            myNbComposits  = aStringList[1].toInt(&Ok);
            break;
        case 11:  //nb of histories
            myNPS          = aStringList[1].toLongLong(&Ok);
            break;
        case 12:  //nb of regular edits
            myNbRegEdit    = aStringList[1].toInt(&Ok);
            break;
        case 13:  //nb of composite edits
            myNbCompEdit   = aStringList[1].toInt(&Ok);
            break;
        }
        if (!check_convert(Ok)) return false;
    }
    myNbElmTotal = 0;
    for (int i=0; i<6; i++)  //calculate the total elements
        myNbElmTotal += myNbElements[i];
    return true;
}

/*!
 * \brief read the real parameters
 * \return false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::read_real_paras()
{
    QStringList aStringList = get_line_in_block(0).split(":", QString::SkipEmptyParts); //read the length factor
    bool Ok ;
    myLengthFactor = aStringList[1].toDouble(&Ok);
    if (!check_convert(Ok)) return false;

    return true;
}
//bool MCMESHTRAN_MCNPUMeshReader::read_paticle_list()
//{
//    return true;
//}
/*!
 * \brief skip the edit description blocks
 * \return false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::read_edit_description()
{
    QStringList aStringList = get_line_in_block(1).split(" ", QString::SkipEmptyParts);  //split the second line
    bool Ok;
    int theEditCnt = aStringList[1].toInt(&Ok);  //obtain the second figure: edit number
    if (!check_convert(Ok)) return false;

    //for each edit skip 4 data block
    int skipBlocks;
    if (myMCNPVersion == MCNP6_1_0)
        skipBlocks = 4;
    else if (myMCNPVersion == MCNP6_1_1_b)
        skipBlocks = 5;
    else {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": Unknown MCNP version!" );
        return false;
    }
    for (int i=0; i<theEditCnt*skipBlocks; i++) {
        get_aLine(); //get a identification line
        if (EndOfFile) {
            MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                    ": file ended unexpectedly!" );
            return false;
        }
        if (!get_aBlock()) {  //get a block
            MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                    ": Get block error!" );
            return false;
        }
    }
    return true;
}

/*!
 * \brief read the instance name, create instances and push to InstanceList
 * \return false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::read_instance_names()
{
    if (currentBlock.size() != myNbInstance + 1) {// if the instance name block lines does not match the Nb of instances + one title
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": instance name block contains lines more than instance count!" );
        return false;
    }
    //clear the instancesList first
    if (InstanceList.size() != 0) {
        for (int i=0; i<InstanceList.size(); i++)
            delete InstanceList[i];
    }
    InstanceList.clear();

    // create instance with the name, and push to the instanceList
    for (int i=0; i<myNbInstance; i++) {
        Instance * aInstance = new Instance(get_line_in_block(i+1)) ; //start from the second line
        InstanceList.push_back(aInstance);
    }
    return true;
}

/*!
 * \brief read the element type for each instance, put them into each instance
 * \return false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::read_instance_element_type()
{
    if (currentBlock.size() != myNbInstance + 1) {// if the block lines does not match the Nb of instances + one title line
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": error in lines of the instance element type block!" );
        return false;
    }

    for (int i=0; i<myNbInstance; i++) {
        QStringList aStringList = get_line_in_block(i+1).split(" ", QString::SkipEmptyParts); //from the second line, split the data
        if (aStringList.size() != 12) {// if data != 6*2
            MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                    ": unexpected number of data in this line, should be 12!" );
            return false;
        }
        for (int j=0; j<6; j++) {
            bool Ok1, Ok2;
            InstanceList[i]->myFirstElement[j] = aStringList[j*2].toInt(&Ok1);  // the even data
            InstanceList[i]->myLastElement [j] = aStringList[j*2+1].toInt(&Ok2); // the odd data
            if (!check_convert(Ok1) || !check_convert(Ok2)) return false;
        }
    }

    return true;
}

/*!
 * \brief read the node coordinates x
 * \return  false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::read_nodes_x()
{
//    NodeCoordX.clear();
    NodeCoordX->alloc(0,1);
    int theLineCnt = currentBlock.size()-1;
    for (int i=0; i<theLineCnt; i++) {  //traverse the lines
        QStringList aStringList = get_line_in_block(i+1).split(" ", QString::SkipEmptyParts); //from the second line, split the data
        for (int j=0; j<aStringList.size(); j++) {
            bool Ok;
//            NodeCoordX.push_back(aStringList[j].toDouble(&Ok));
            NodeCoordX->pushBackSilent(aStringList[j].toDouble(&Ok) / myLengthFactor);
            if (!check_convert(Ok)) return false;
        }
    }
    //check if the data read match with NbNodes
//    if (NodeCoordX.size() != myNbNodes)  {
    if (NodeCoordX->getNumberOfTuples() != myNbNodes)  {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": Coordinates read does not match with Nb of nodes!" );
        return false;
    }
    return true;
}
/*!
 * \brief read the node coordinates y
 * \return  false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::read_nodes_y()
{
//    NodeCoordY.clear();
    NodeCoordY->alloc(0,1);
    int theLineCnt = currentBlock.size()-1;
    for (int i=0; i<theLineCnt; i++) {  //traverse the lines
        QStringList aStringList = get_line_in_block(i+1).split(" ", QString::SkipEmptyParts); //from the second line, split the data
        for (int j=0; j<aStringList.size(); j++) {
            bool Ok;
//            NodeCoordY.push_back(aStringList[j].toDouble(&Ok));
            NodeCoordY->pushBackSilent(aStringList[j].toDouble(&Ok) / myLengthFactor);
            if (!check_convert(Ok)) return false;
        }
    }
    //check if the data read match with NbNodes
//    if (NodeCoordY.size() != myNbNodes)  {
    if (NodeCoordY->getNumberOfTuples() != myNbNodes)  {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": Coordinates read does not match with Nb of nodes!" );
        return false;
    }
    return true;
}
/*!
 * \brief read the node coordinates z
 * \return  false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::read_nodes_z()
{
//    NodeCoordZ.clear();
    NodeCoordZ->alloc(0,1);
    int theLineCnt = currentBlock.size()-1;
    for (int i=0; i<theLineCnt; i++) {  //traverse the lines
        QStringList aStringList = get_line_in_block(i+1).split(" ", QString::SkipEmptyParts); //from the second line, split the data
        for (int j=0; j<aStringList.size(); j++) {
            bool Ok;
//            NodeCoordZ.push_back(aStringList[j].toDouble(&Ok));
            NodeCoordZ->pushBackSilent(aStringList[j].toDouble(&Ok) / myLengthFactor);
            if (!check_convert(Ok)) return false;
        }
    }
    //check if the data read match with NbNodes
//    if (NodeCoordZ.size() != myNbNodes)  {
    if (NodeCoordZ->getNumberOfTuples() != myNbNodes)  {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": Coordinates read does not match with Nb of nodes!" );
        return false;
    }
    return true;
}

/*!
 * \brief read the element material as field data
 * \return false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::read_material()
{
    DataArrayDouble * aDataArray = DataArrayDouble::New();  //for storing the material data
    aDataArray->setName("Material");
    aDataArray->alloc(0,1);  // for specified nb of components in this array
    int theLineCnt = currentBlock.size()-1;
    for (int i=0; i<theLineCnt; i++) {  //traverse the lines
        QStringList aStringList = get_line_in_block(i+1).split(" ", QString::SkipEmptyParts); //from the second line, split the data
        for (int j=0; j<aStringList.size(); j++) {
            bool Ok;
            aDataArray->pushBackSilent(aStringList[j].toDouble(&Ok));
            if (!check_convert(Ok)) return false;
        }
    }
    //check if the data read match with myNbElmTotal
    if (aDataArray->getNumberOfTuples() != myNbElmTotal)  {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": Material data read not match with Nb of elements!" );
        return false;
    }
    DataList.push_back(aDataArray);  //push into the field list
    return true;
}

/*!
 * \brief this function read connctivities in any type, and append them into the main conn array
 * \param theElmType the element type
 * \return false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::read_connectivities(const UMeshElmType &theElmType)
{
    //allocate at the first time
    if (!ElmConn->isAllocated())
        ElmConn->alloc(0,1);
    if (!ElmConnIdx->isAllocated())
        ElmConnIdx->alloc(0,1);
    //two tmp array for data exchange
    DataArrayInt * aTmpArray = DataArrayInt::New();
//    DataArrayInt * bTmpArray;  //to get transpose data from aTmpArray
    aTmpArray->alloc(0,1);

    //fetch the data
    int theLineCnt = currentBlock.size()-1;
    for (int i=0; i<theLineCnt; i++) {  //traverse the lines
        QStringList aStringList = get_line_in_block(i+1).split(" ", QString::SkipEmptyParts); //from the second line, split the data
        for (int j=0; j<aStringList.size(); j++) {
            bool Ok;
            aTmpArray->pushBackSilent(aStringList[j].toInt(&Ok));
            if (!check_convert(Ok)) return false;
            }
    }

    //check if the data is correct or not
    int theNbElm = myNbElements[(int) theElmType]; //convert the theElmType to int for indexing
    int theElmNodeCnt = ElmNodeCnt.value(theElmType, 0) ;//get the node count for this type of element
    if (aTmpArray->getNumberOfTuples() != theElmNodeCnt*theNbElm){
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": not getting the connectivity data correctly" );
        return false;
    }


    if  (MeshConnOrder == Node_Ordered) {
    //transpose the array to switch from node ordered to element ordered
    aTmpArray->rearrange(theNbElm);  //at the begin it is node ordered, each componenet contains theNbElm items
    //for testing
//    vector <int > aTestArray;
//    aTestArray.resize(theNbElm);
//    aTmpArray->getTuple(0, aTestArray.data());
//    for (int i=0; i<aTestArray.size(); i++) cout <<aTestArray[i] <<"\t"; cout <<endl;
    //end testing
 //   bTmpArray = aTmpArray->toNoInterlace(); //transpose the matrix
    aTmpArray = aTmpArray->toNoInterlace(); //transpose the matrix
    //for testing
//    bTmpArray->getTuple(0, aTestArray.data());
//    for (int i=0; i<aTestArray.size(); i++) cout <<aTestArray[i] <<"\t"; cout <<endl;
    //end testing
 //   bTmpArray->transpose();         //this only rearrange the array, make each component contain theElmNodeCnt items
    aTmpArray->transpose();         //this only rearrange the array, make each component contain theElmNodeCnt items
    //for testing
//    aTestArray.resize(theElmNodeCnt);
//    bTmpArray->getTuple(0, aTestArray.data());
//    for (int i=0; i<aTestArray.size(); i++) cout <<aTestArray[i] <<"\t"; cout <<endl;
    //end testing
    }
    else if (MeshConnOrder == Elm_Ordered) {
        aTmpArray->rearrange(theElmNodeCnt);

    }
    else {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": Connectivity order is unknown" );
    }


    //copy the data
    if (ElmConn->getNumberOfTuples() == 0)  //if the first time for filling data
        ElmConnIdx->pushBackSilent(0);  //only do this at the conn section
    vector <int> aTupe;  //temp array
    aTupe.resize(theElmNodeCnt);  //each component should include theElmNodeCnt items
//    for (int i=0; i<bTmpArray->getNumberOfTuples(); i++) {
      for (int i=0; i<aTmpArray->getNumberOfTuples(); i++) {
        ElmConn->pushBackSilent(MEDElmType.value(theElmType, 0));      //the element type, 0-if error
//        bTmpArray->getTuple(i, aTupe.data()  );
        aTmpArray->getTuple(i, aTupe.data()  );
        for (int j=0; j<theElmNodeCnt; j++) {
            ElmConn->pushBackSilent(aTupe[j] -1 ); //-1: MCNP node id start with 1, MED start with 0
        }
        ElmConnIdx->pushBackSilent(ElmConn->getNumberOfTuples());
    }
    aTmpArray->decrRef();
//    bTmpArray->decrRef();
    return true;
}

bool MCMESHTRAN_MCNPUMeshReader::read_edit_output()
{
    EditPrefix.clear();
    //split the line using ";"
    QStringList aStringList = get_line_in_block(0).split(";", QString::SkipEmptyParts);
    if (aStringList.size() != 3) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": unexpect content in this line, should contain particle, edit, and tally number" );
        return false;
    }
    //first deal with particle type
    QStringList bStringList = aStringList[0].split(":", QString::SkipEmptyParts);
    if (bStringList.size() != 2) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": unexpect content in this line, should contain particle number" );
        return false;
    }
    QStringList cStringList = bStringList[1].split("_", QString::SkipEmptyParts);
    bool Ok;
    if (cStringList.size() == 1) {//if only one particle
        EditPrefix += ParticleType.value(cStringList[0].toInt(&Ok), "unknown") /*+ "_"*/;
        if (!check_convert(Ok)) return false;
    }
    else if (cStringList.size() > 1) { //in case of total value of more than one particle
        for (int i=1; i<cStringList.size(); i++) {  //start from the second one
            EditPrefix += ParticleType.value(cStringList[i].toInt(&Ok), "unknown") /*+ "_"*/;
            if (!check_convert(Ok)) return false;
        }
    }

//    //second deal with the edit Number
//    bStringList = aStringList[1].split(":", QString::SkipEmptyParts);
//    if (bStringList.size() != 2) {
//        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
//                ": unexpect content in this line, should contain edit list number" );
//        return false;
//    }
////    EditPrefix += "Edit" + bStringList[1].trimmed() +"_";
//        EditPrefix += "Ed" + bStringList[1].trimmed() +"_";

    //third due with tally number
    bStringList = aStringList[2].split(":", QString::SkipEmptyParts);
    if (bStringList.size() != 2) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": unexpect content in this line, should contain tally number" );
        return false;
    }
    cStringList = bStringList[1].split("_", QString::SkipEmptyParts);
    if (cStringList.size() != 2) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": unexpect content in this line, should contain tally type and type number" );
        return false;
    }
//    EditPrefix += cStringList[0].trimmed().toLower() + cStringList[1].trimmed() + "_";
//    EditPrefix += QString("TAL") + cStringList[1].trimmed() + "_";
        EditPrefix +=  cStringList[1].trimmed() /*+ "_"*/;

    return true;
}

/*!
 * \brief read the field data results, each data set build one field component
 * \return
 */
bool MCMESHTRAN_MCNPUMeshReader::read_edit_sets()
{
    QString theFieldName =  EditPrefix; //obtaint eh prefix
    double EnergyMultiplier = 1; //energy multiplier, default 1;
    double TimeMultiplier = 1;//time multiplier, default 1;
    bool Ok;
    bool isTotal = false;  //if total then true
    QStringList aStringList = get_line_in_block(0).split(";", QString::SkipEmptyParts);
    if (aStringList.size() != 6) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": unexpect content in this line!" );
        return false;
    }

    //****build the field name******
    //first check the result type
    QStringList bStringList = aStringList[0].split(":", QString::SkipEmptyParts);
    if (bStringList.size() != 2) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": unexpect content in this line, should contain time bin number" );
        return false;
    }
    if (bStringList[0].indexOf("SQR") != -1) //if found SQR
        theFieldName += QString("SQR") /*+ "_"*/;
    else if (bStringList[0].indexOf("ERROR") != -1) //if found ERROR
//        theFieldName += QString("Error") + "_";
        theFieldName += QString("ERR")/* + "_"*/;
    else
//        theFieldName += QString("Result") + "_";
    theFieldName += QString("RES") /*+ "_"*/;

//    //then check the time value
//    bStringList = aStringList[1].split(":", QString::SkipEmptyParts);
//    if (bStringList.size() != 2) {
//        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
//                ": unexpect content in this line, should contain time value" );
//        return false;
//    }
//    if(bStringList[1].trimmed() == "N/A") {  //if the total time result
if(bStringList[1].trimmed() == "TOTAL") {  //if the total time result
    //        theFieldName += QString("Time:Total") + "_";
                theFieldName += QString("T:Tt") /*+ "_"*/;
        isTotal = true;
    }
    else
//        theFieldName += "Time:" + bStringList[1].trimmed() + "_";
    theFieldName += "T:" + bStringList[1].trimmed() /*+ "_"*/;

    //then the time multiplier
    bStringList = aStringList[2].split(":", QString::SkipEmptyParts);
    if (bStringList.size() != 2) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": unexpect content in this line, should contain time multiplier" );
        return false;
    }
    if(bStringList[1].trimmed() == "N/A" ) {   //if the total
        if (removeMultiplier)
            MESSAGE("Warning : The multiplier is will not been removed for the TOTAL result!" );
           TimeMultiplier = 1;                 //if the total, then ignore removing the multiplier
    }
    else {
        TimeMultiplier = bStringList[1].toDouble(&Ok);
        if (!check_convert(Ok)) return false;
    }

    //then the energy value
//    bStringList = aStringList[4].split(":", QString::SkipEmptyParts);
        bStringList = aStringList[3].split(":", QString::SkipEmptyParts);
    if (bStringList.size() != 2) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": unexpect content in this line, should contain energy value" );
        return false;
    }
//    if(bStringList[1].trimmed() == "N/A") {  //if the total energy result
    if(bStringList[1].trimmed() == "TOTAL") {  //if the total energy result
//        theFieldName += "Energy:Total" ;
        theFieldName += "E:Tt" ;
        isTotal = true;
    }
    else
//        theFieldName += "Energy:" + bStringList[1].trimmed() ;
        theFieldName += "E:" + bStringList[1].trimmed() ;

    //then the energy multiplier
    bStringList = aStringList[5].split(":", QString::SkipEmptyParts);
    if (bStringList.size() != 2) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": unexpect content in this line, should contain energy multiplier" );
        return false;
    }
    if(bStringList[1].trimmed() == "N/A" ) {   //if the total
           if (removeMultiplier)
               MESSAGE("Warning : The multiplier is will not been removed for the TOTAL result!" );
           EnergyMultiplier = 1;                 //if the total, then ignore removing the multiplier
    }
    else {
        EnergyMultiplier = bStringList[1].toDouble(&Ok);
        if (!check_convert(Ok)) return false;
    }

    //add the end with if multiplier removed or not
    if (!isTotal && removeMultiplier)
//        theFieldName =+ "_" + QString("MultiplierRemoved");
        theFieldName +=/*+ "_"+*/  QString("RM");


    //****fetch the data******
    DataArrayDouble * aDataArray = DataArrayDouble::New();  //for storing the  data
    aDataArray->setName(theFieldName.toStdString());
    aDataArray->alloc(0,1);  // for specified nb of components in this array
    int theLineCnt = currentBlock.size()-1;
    //!!the lenght for the data set is = theElmCnt +1!!
    //remove the first value because it belongs to none
    aStringList = get_line_in_block(1).split(" ", QString::SkipEmptyParts); //from the second line, split the data
    for (int j=1; j<aStringList.size(); j++) {  //skip the first value
        if (!removeMultiplier)
            aDataArray->pushBackSilent(aStringList[j].toDouble(&Ok));
        else  //remove the multiplier
            aDataArray->pushBackSilent(aStringList[j].toDouble(&Ok) / TimeMultiplier /EnergyMultiplier);
        if (!check_convert(Ok)) return false;
    }
    //start from the third line, traverse the lines
    for (int i=1; i<theLineCnt; i++) {
        aStringList = get_line_in_block(i+1).split(" ", QString::SkipEmptyParts); //from the second line, split the data
        for (int j=0; j<aStringList.size(); j++) {
            if (!removeMultiplier)
                aDataArray->pushBackSilent(aStringList[j].toDouble(&Ok));
            else  //remove the multiplier
                aDataArray->pushBackSilent(aStringList[j].toDouble(&Ok) / TimeMultiplier /EnergyMultiplier);
            if (!check_convert(Ok)) return false;
        }
    }
    //check if the data read match with myNbElmTotal
    if (aDataArray->getNumberOfTuples() != myNbElmTotal)  {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": field data read not match with Nb of elements!" );
        return false;
    }
    DataList.push_back(aDataArray);  //push into the field list
    return true;
}


/*!
 * \brief process the nodes, elements and field data to build a field
 * \return false if error
 */
bool MCMESHTRAN_MCNPUMeshReader::process_data()
{
    //check the data consistency
    if (NodeCoordX->getNumberOfTuples() != myNbNodes ||
            NodeCoordY->getNumberOfTuples() != myNbNodes ||
            NodeCoordZ->getNumberOfTuples() != myNbNodes){
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": Coordinates read not match with number of nodes!" );
        return false;
    }
    for (int i=0; i<DataList.size(); i++) {
        if (DataList[i]->getNumberOfTuples() != myNbElmTotal){
            MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                    ": field data read not match with total number of elements!" );
            return false;
        }
    }
    if (ElmConnIdx->getNumberOfTuples()-1 != myNbElmTotal) {  //CONN index should = myNbElmTotal +1
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": connectivity data error!" );
        return false;
    }

    //meld the node array into one
    vector <const DataArrayDouble * > aNodeCoordList;
    aNodeCoordList.push_back(NodeCoordX);
    aNodeCoordList.push_back(NodeCoordY);
    aNodeCoordList.push_back(NodeCoordZ);
    DataArrayDouble * NodeCoord = DataArrayDouble::Meld(aNodeCoordList);

    if (NodeCoord->getNumberOfComponents() != 3) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": Meld the node coordinate list failed!" );
        return false;
    }
    //meld the field data into one:
    DataArrayDouble * allFieldData = DataArrayDouble::Meld(DataList);
    for (int i=0; i<DataList.size(); i++)
        allFieldData->setInfoOnComponent(i, DataList[i]->getName().c_str());

    //build the field
//    QStringList aStringList = QString::fromStdString(myUMeshOutFileName).split("/", QString::SkipEmptyParts);
//    QString aTmpName = aStringList[aStringList.size() -1].split(".").at(0); //get the file name without path, only works in Linux
	//2015-09-02 to make possible Windows
	QString aTmpName = QFileInfo (myUMeshOutFileName.c_str()).fileName().split(".").at(0);
	MEDCouplingUMesh *aMesh = MEDCouplingUMesh::New(aTmpName.toStdString(), 3);
    aMesh->setCoords(NodeCoord);
    aMesh->setConnectivity(ElmConn, ElmConnIdx, false);

    MEDCouplingFieldDouble* aField = MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::ONE_TIME);
    aField->setName(aTmpName.toStdString());
    aField->setNature(ParaMEDMEM::RevIntegral);  //volumetric field
    aField->setMesh(aMesh);
    aField->setArray(allFieldData);
    NodeCoord->decrRef();    //release the unncesssary local pointers
    allFieldData->decrRef();
    aMesh->decrRef();
    myFieldList.push_back( aField );  //append the field

    //get instance fields if necessary
    if (keepInstances ) {
        for (int i=0; i<InstanceList.size(); i++) {
            const MEDCouplingFieldDouble* aInstanceField =
                    InstanceList[i]->extractSubField(aField);
            if (aInstanceField == NULL) {
                MESSAGE("Error : Error in building instance field!" );
                return false;
            }
            myFieldList.push_back(aInstanceField);
        }
    }

    return true;
}


/*!
 * \brief return a list of field get by this reader, these mesh should decrRef() after used.
 * \return return a list of field, these mesh should decrRef() after used.
 */
vector <const MEDCouplingFieldDouble *> MCMESHTRAN_MCNPUMeshReader::getFieldList()
{
    vector <const MEDCouplingFieldDouble*> aTmpList ;
    for (int i=0; i<myFieldList.size(); i++)    {
        myFieldList[i]->incrRef();    //increase reference because myFieldList will be destroy when the reader is deleted,
        aTmpList.push_back(myFieldList[i]);
    }
    return aTmpList;
}

/*!
 * \brief get a line and return the line to currentLine;
 */
void  MCMESHTRAN_MCNPUMeshReader::get_aLine()   //get a line and increase lineCnt;
{
    currentLine.clear();
    getline(myUMeshOutFile, currentLine);
    if(myUMeshOutFile.eof()) EndOfFile = true;   //to inform that file is ended
//    cout <<currentLine<<endl;//for testing
    ++lineNb;
}

/*!
 * \brief get identifiers from currentLine, fill the array \param Identifiers[6]
 * \return \c false if error
 */
bool  MCMESHTRAN_MCNPUMeshReader::get_identifiers()
{
    //first check the line length, the identifier line contain exactly 90 chars; //to be confirm??
    QString aTmpStr = QString::fromStdString(currentLine);
    if (aTmpStr.length() != 90)
        return false;
    aTmpStr = aTmpStr.trimmed();  //first judge length and then trimmed() the line

    //get the paras
    QStringList aStringList = aTmpStr.split(" ", QString::SkipEmptyParts);
    if (aStringList.size() != 6)
        return false;

    bool Ok = true;
    Identifiers.hasTitle = (bool) aStringList[0].toInt(&Ok);
    if (!check_convert(Ok)) return false;
    Identifiers.NbDataRec =  aStringList[1].toInt(&Ok);
    if (!check_convert(Ok)) return false;
    Identifiers.DataType = (UMeshDataType) aStringList[2].toInt(&Ok);
    if (!check_convert(Ok)) return false;
    Identifiers.SizeofType =  aStringList[3].toInt(&Ok);
    if (!check_convert(Ok)) return false;
    Identifiers.NbItems =  aStringList[4].toInt(&Ok);
    if (!check_convert(Ok)) return false;
    Identifiers.ParseLength =  aStringList[5].toInt(&Ok);
    if (!check_convert(Ok)) return false;

    return true;
}

/*!
 * \brief calculate how many lines in this block
 * \return  line amount
 */
int MCMESHTRAN_MCNPUMeshReader::cal_block_lines()
{
    int aLineCnt = 0;
    if (Identifiers.hasTitle) aLineCnt ++; //if has title, ++

    if (Identifiers.NbDataRec == 0) //if no data rec, return
        return aLineCnt;
    else if (Identifiers.NbDataRec > 0  ) {
        if (Identifiers.DataType == Data_string) { //if string type, each line is a record
                aLineCnt += Identifiers.NbDataRec;
        }
        //if int or real, each data record contain NbItems items, and each line includes ParseLength items
        else if (Identifiers.DataType == Data_int || Identifiers.DataType == Data_real ) {
            //make conversion to avoid int/int
            aLineCnt += (int)ceil((double)Identifiers.NbItems / (double)Identifiers.ParseLength) * Identifiers.NbDataRec;
        }
        else return 0;
    }
    else return 0;

    return aLineCnt;
}

/*!
 * \brief get a block, dump the line in this block into \param currentBlock
 * \return false if error
 */
bool  MCMESHTRAN_MCNPUMeshReader::get_aBlock()
{
//    get_a_line(); //first get the block identifier line
//    if (EndOfFile) return false;
    if (!get_identifiers()) return false;

    //calculate how many line should be read
    int aLineCnt = cal_block_lines();
    if (aLineCnt == 0) return false;
    // dump the lines into the currentBlock;
    currentBlock.clear();  //clear the data for previous block
    for (int i=0; i<aLineCnt; i++) {
        get_aLine();
        if (EndOfFile) return false; //if not enough line get, return an error
        currentBlock.push_back(QString::fromStdString(currentLine).trimmed()); //trimmed incase of platform independent
    }
    return true;
}
/*!
 * \brief read the first line of the \param currentBlock, recognize the block type
 * \return enum UMeshBlock type
 */
UMeshBlockType  MCMESHTRAN_MCNPUMeshReader::get_block_type()
{
    QString aTmpStr = get_line_in_block(0);
    //start to judge the key word
    if (aTmpStr.indexOf("EEOUT") != -1)                         return Block_first_set;
    if (aTmpStr.indexOf("CALLING") != -1)                       return Block_calling_code;
    if (aTmpStr.indexOf("NUMBER OF") != -1)                     return Block_int_paras;
    if (aTmpStr.indexOf("LENGTH") != -1)                        return Block_real_paras;
    if (aTmpStr.indexOf("PARTICLE LIST") != -1)                 return Block_particle_list;
    if (aTmpStr.indexOf("EDIT DESCRIPTION") != -1)              return Block_edit_description;
    if (aTmpStr.indexOf("COMPOSITE EDIT LIMITS") != -1)         return Block_composite_edit_limit;
    if (aTmpStr.indexOf("MATERIALS") != -1)                     return Block_materials;
    if (aTmpStr.indexOf("INSTANCE CUMMULATIVE") != -1)          return Block_instance_cumulative;
    if (aTmpStr.indexOf("INSTANCE ELEMENT NAMES") != -1)        return Block_instance_names;
    if (aTmpStr.indexOf("INSTANCE ELEMENT TYPE") != -1)         return Block_instance_element_type;
    if (aTmpStr.indexOf("NODES X") != -1)                       return Block_node_x;
    if (aTmpStr.indexOf("NODES Y") != -1)                       return Block_node_y;
    if (aTmpStr.indexOf("NODES Z") != -1)                       return Block_node_z;
    if (aTmpStr.indexOf("ELEMENT TYPE") != -1)                  return Block_element_type;
    if (aTmpStr.indexOf("ELEMENT MATERIAL") != -1)              return Block_element_material;
    if (aTmpStr.indexOf("CONNECTIVITY DATA 1ST ORDER TETS") != -1) {
        if (aTmpStr.indexOf("NODE ORDERED") != -1)
            MeshConnOrder = Node_Ordered;
        else if (aTmpStr.indexOf("ELEMENT ORDERED") != -1)
            MeshConnOrder = Elm_Ordered;
        else {
            MeshConnOrder = Unknown_Ordered;
            MESSAGE("Error : Unknown Element order!" );
        }

        return Block_conn_1st_tet;
    }
    if (aTmpStr.indexOf("CONNECTIVITY DATA 1ST ORDER PENTS") != -1) {
        if (aTmpStr.indexOf("NODE ORDERED") != -1)
            MeshConnOrder = Node_Ordered;
        else if (aTmpStr.indexOf("ELEMENT ORDERED") != -1)
            MeshConnOrder = Elm_Ordered;
        else {
            MeshConnOrder = Unknown_Ordered;
            MESSAGE("Error : Unknown Element order!" );
        }
        return Block_conn_1st_penta;
    }
    if (aTmpStr.indexOf("CONNECTIVITY DATA 1ST ORDER HEXS") != -1)  {
        if (aTmpStr.indexOf("NODE ORDERED") != -1)
            MeshConnOrder = Node_Ordered;
        else if (aTmpStr.indexOf("ELEMENT ORDERED") != -1)
            MeshConnOrder = Elm_Ordered;
        else {
            MeshConnOrder = Unknown_Ordered;
            MESSAGE("Error : Unknown Element order!" );
        }
        return Block_conn_1st_hexa;
    }
    if (aTmpStr.indexOf("CONNECTIVITY DATA 2ND ORDER TETS") != -1)  {
        if (aTmpStr.indexOf("NODE ORDERED") != -1)
            MeshConnOrder = Node_Ordered;
        else if (aTmpStr.indexOf("ELEMENT ORDERED") != -1)
            MeshConnOrder = Elm_Ordered;
        else {
            MeshConnOrder = Unknown_Ordered;
            MESSAGE("Error : Unknown Element order!" );
        }
        return Block_conn_2nd_tet;
    }
    if (aTmpStr.indexOf("CONNECTIVITY DATA 2ND ORDER PENTS") != -1) {
        if (aTmpStr.indexOf("NODE ORDERED") != -1)
            MeshConnOrder = Node_Ordered;
        else if (aTmpStr.indexOf("ELEMENT ORDERED") != -1)
            MeshConnOrder = Elm_Ordered;
        else {
            MeshConnOrder = Unknown_Ordered;
            MESSAGE("Error : Unknown Element order!" );
        }
        return Block_conn_2nd_penta;
    }
    if (aTmpStr.indexOf("CONNECTIVITY DATA 2ND ORDER HEXS") != -1)  {
        if (aTmpStr.indexOf("NODE ORDERED") != -1)
            MeshConnOrder = Node_Ordered;
        else if (aTmpStr.indexOf("ELEMENT ORDERED") != -1)
            MeshConnOrder = Elm_Ordered;
        else {
            MeshConnOrder = Unknown_Ordered;
            MESSAGE("Error : Unknown Element order!" );
        }
        return Block_conn_2nd_hexa;
    }
    if (aTmpStr.indexOf("NEAREST NEIGHBOR") != -1)                  return Block_nearest_neighbor;
    if (aTmpStr.indexOf("DATA OUTPUT") != -1)                       return Block_edit_output;
    if (aTmpStr.indexOf("DATA SETS") != -1)                         return Block_edit_set;
    if (aTmpStr.indexOf("CENTROIDS") != -1)                         return Block_centroid;
    if (aTmpStr.indexOf("DENSITY") != -1)                           return Block_density;
    if (aTmpStr.indexOf("VOLUMES") != -1)                           return Block_volume;
    return Block_unknown;
}

/*!
 * \brief check the conversion, if failed show message
 * \param Ok bool from QString conversion
 * \return  false if failed
 */
bool  MCMESHTRAN_MCNPUMeshReader::check_convert(const bool & Ok)
{
    if (!Ok) {
        MESSAGE("Error in File "<<myUMeshOutFileName <<endl<<"line "<< lineNbProc <<
                ": converting QString to int/double failed!" );
        return false;
    }
    return true;
}

/*!
 * \brief get the lineIdx line in currentBlock
 *  because the lineNb increase by getLine() is unable to indicate the current line
 *  to be proccessed in code, using this function to replease currentBlock[lineIdx] to
 *  increase lineNbProc, to show the current processing line and report error correctly
 * \param lineIdx line Nb (start from 0)to be return in currentBlock
 * \return the lineIdx line
 */
QString MCMESHTRAN_MCNPUMeshReader::get_line_in_block(const int & lineIdx)
{
    lineNbProc = lineNb - currentBlock.size() + lineIdx + 1;
    return currentBlock[lineIdx];
}



//####################INSTANCE CLASS#############
Instance::Instance(const QString &aName)
{
    myName = aName;
    for (int i=0; i<6; i++) {
        myFirstElement[i] = 0;
        myLastElement[i] = 0;
    }

    ElmNodeCnt[Elm_1st_tet]     = 4;
    ElmNodeCnt[Elm_1st_pent]    = 6;
    ElmNodeCnt[Elm_1st_hexa]    = 8;
    ElmNodeCnt[Elm_2nd_tet]     = 10;
    ElmNodeCnt[Elm_2nd_pent]    = 15;
    ElmNodeCnt[Elm_2nd_hexa]    = 20;

    MEDElmType[Elm_1st_tet]     = INTERP_KERNEL::NORM_TETRA4;
    MEDElmType[Elm_1st_pent]    = INTERP_KERNEL::NORM_PENTA6;
    MEDElmType[Elm_1st_hexa]    = INTERP_KERNEL::NORM_HEXA8;
    MEDElmType[Elm_2nd_tet]     = INTERP_KERNEL::NORM_TETRA10;
    MEDElmType[Elm_2nd_pent]    = INTERP_KERNEL::NORM_PENTA15;
    MEDElmType[Elm_2nd_hexa]    = INTERP_KERNEL::NORM_HEXA20;
}

Instance::~Instance()
{
}

/*!
 * \brief extract the sub field from the whole field
 * \param theWholeField the entire field from the file
 * \return sub field only for this instance, or NULL if error
 */
const MEDCouplingFieldDouble * Instance::extractSubField(const MEDCouplingFieldDouble * theWholeField )
{
    MEDCouplingUMesh *theWholeMesh = dynamic_cast <MEDCouplingUMesh *> (
                const_cast <MEDCouplingMesh *> (theWholeField->getMesh()));  //no need to decrRef() this
    MEDCouplingUMesh *aMesh = MEDCouplingUMesh::New(myName.toStdString(), 3); //mesh for this instance//qiu 28-08-14
    aMesh->setCoords(theWholeMesh->getCoordinatesAndOwner());  //set the same coordinates with whole mesh, share it but not copy it

    //**********constructured the connectivities**********
    //first calculate how many cells
    int CellCnt = 0;
    for (int i=0; i<6; i++)
        CellCnt += myLastElement[i] - myFirstElement[i] +1;  //attention, should +1
    if (CellCnt == 0){
        MESSAGE("Error : Instance contain no cell!" );
        return NULL;
    }
    aMesh->allocateCells(CellCnt);  //allocate cells for the mesh

    //copy the connectivities
    for (int i=0; i<6; i++) {
        if (myFirstElement[i] != 0) {  //skip all empty element type!!
            int theElmNodeCnt = ElmNodeCnt.value((UMeshElmType)i, 0); //Nb of nodes for this element
            if (theElmNodeCnt == 0){
                MESSAGE("Error : get Element node count error!" );
                return NULL;
            }
            vector <int > aConn;
    //        aConn.resize(theElmNodeCnt);
            for (int j = myFirstElement[i]-1;/*!! should <= */ j< myLastElement[i]; j++) {  //-1 : in MCNP, cell ID start with 1, in MED, start with 0
                theWholeMesh->getNodeIdsOfCell(j, aConn); //get the conn of this cell
//                for (int k=0; k<aConn.size(); k++) cout <<aConn[k] <<"\t" ;
//                cout <<endl;
                aMesh->insertNextCell((INTERP_KERNEL::NormalizedCellType)MEDElmType.value((UMeshElmType)i, 0),  //make a type conversion, 0-error
                                      theElmNodeCnt, aConn.data());  //insert into the instance mesh
                aConn.clear();  //clear to be append next cell
            }
        }
    }
    aMesh->finishInsertingCells(); //finished inserting cells
    aMesh->zipCoords();  //keep all the node in-use and discard others

//    //keep all the node in-use and discard others
//    int theNodeInUseCnt;  //total Nb node in used
//    DataArrayInt * aInUseNodeIds = aMesh->getNodeIdsInUse(theNodeInUseCnt);  //get ids of all the node in-use
//    DataArrayDouble * aNewCoords = DataArrayDouble::New(); //new coordinates array
//    aNewCoords->alloc(0, 1);
//    vector <double > aCoordVec;
//    aCoordVec.resize(3);  //for storing x, y, z
//    int tmpInt ; //for getting node id
//    for (int i=0; i<theNodeInUseCnt; i++) {
//        aInUseNodeIds->getTuple(i, &tmpInt); //get a id
//        theWholeMesh->getCoordinatesAndOwner()->getTuple(tmpInt, aCoordVec.data()); //fetch x, y, z of the node
//        for (int j=0; j<aCoordVec.size(); j++) {//append the node
//            aNewCoords->pushBackSilent(aCoordVec[j]);
//        }
//    }
//    aNewCoords->rearrange(3); //rearrange  the array to 3 componenet x, y, z
//    aMesh->setCoords(aNewCoords); //set the new coordinates
//    aInUseNodeIds->decrRef();
//    aNewCoords->decrRef();

    //copy the field data
    vector< pair< int, int > > aElmRanges; //elm ranges
    for (int i=0; i<6; i++) {
        if (myFirstElement[i] != 0 && myLastElement[i] !=0) {
            //!!!!IMPORTANT: selectByTupleRanges is selecting in [Beigin, End), End is not included!!
            pair <int, int> aPair (myFirstElement[i]-1, myLastElement[i] ); //-1 : in MCNP, cell ID start with 1, in MED, start with 0
            aElmRanges.push_back(aPair);
        }
    }
    if (aElmRanges.size() == 0) {
        MESSAGE("Error : Instance contain no cell!" );
        return NULL;
    }
    DataArrayDouble * aInstData = static_cast <DataArrayDouble *> (theWholeField->getArray()->selectByTupleRanges(aElmRanges) ); //fetch the data with specified ranges//qiu 28-08-14
    if (aInstData->getNumberOfTuples() != aMesh->getNumberOfCells()){
        MESSAGE("Error : Element and Data not matched!"<<aInstData->getNumberOfTuples()<<": " << aMesh->getNumberOfCells());
        return NULL;
    }
    //build the field
    MEDCouplingFieldDouble* aField = MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::ONE_TIME);
    aField->setName(myName.toStdString());//qiu 28-08-14
    aField->setNature(ParaMEDMEM::RevIntegral);  //volumetric field
    aField->setMesh(aMesh);
    aField->setArray(aInstData);
    aInstData->decrRef();
    aMesh->decrRef();
    return aField;
}
