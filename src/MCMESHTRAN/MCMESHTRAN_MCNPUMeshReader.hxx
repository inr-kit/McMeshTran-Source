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
#ifndef MCMESHTRAN_MCNPUMESHREADER_HXX
#define MCMESHTRAN_MCNPUMESHREADER_HXX
/*
 * To be improved
 * 1. the exceptions from using MED method should be take care of
 * 2. release the pointers in case of error return
 * 3. deal with negative volume
 */

#include <string>
#include <vector>
#include <fstream>

#include <MEDCouplingUMesh.hxx>
#include <MEDCouplingMemArray.hxx>
#include <MEDCouplingFieldDouble.hxx>


#include <QString>
#include <QMap>

using namespace std;
using namespace ParaMEDMEM;

enum UMeshBlockType{
    Block_first_set,
    Block_calling_code,
    Block_int_paras,
    Block_real_paras,
    Block_particle_list,
    Block_edit_description,
    Block_composite_edit_limit,
    Block_materials,
    Block_instance_cumulative,
    Block_instance_names,
    Block_instance_element_type,
    Block_node_x,
    Block_node_y,
    Block_node_z,
    Block_element_type,
    Block_element_material,
    Block_conn_1st_tet,
    Block_conn_1st_penta,
    Block_conn_1st_hexa,
    Block_conn_2nd_tet,
    Block_conn_2nd_penta,
    Block_conn_2nd_hexa,
    Block_nearest_neighbor,
    Block_edit_output,
    Block_edit_set,
    Block_centroid,
    Block_density,
    Block_volume,
    Block_unknown
};

enum UMeshDataType {
    Data_unkown = 0,
    Data_string  = 1,
    Data_int  = 2,
    Data_real = 3

};

enum UMeshNodeCoord {
    Node_x,
    Node_y,
    Node_z
};

enum UMeshElmType {
    Elm_1st_tet =  0,
    Elm_1st_pent = 1,
    Elm_1st_hexa = 2,
    Elm_2nd_tet  = 3,
    Elm_2nd_pent = 4,
    Elm_2nd_hexa = 5
};
enum UMeshOrder {
    Unknown_Ordered = 0,
    Node_Ordered = 1,
    Elm_Ordered = 2
};
enum MCNPVersion {
    MCNP6_1_0 = 1,
    MCNP6_1_1_b = 2,
    MCNP_unknown = 0
};

struct BlockIdentifier {
    bool            hasTitle; //has title or not
    int             NbDataRec; //number of data record
    UMeshDataType   DataType; // data type of the data record
    int             SizeofType; 	//size in Byte of the data type
    int             NbItems;  // Number of items in each data record
    int             ParseLength; //parsh length in each line

};
class Instance;
class MCMESHTRAN_MCNPUMeshReader
{
public:
    MCMESHTRAN_MCNPUMeshReader();
    ~MCMESHTRAN_MCNPUMeshReader();
    bool            load_file(const QString & UMeshOutFileName);  //load and process the file
    vector <const MEDCouplingFieldDouble *> getFieldList();  //get the field list, whole field and also separate instances
    long long int   get_NPS () {return myNPS; }             //return the NPS
    void            setRemoveMultiplier (const bool & isRemoveMultiplier) {removeMultiplier = isRemoveMultiplier ;}
    void            setKeepInstances (const bool &isKeepInstances) {keepInstances = isKeepInstances ;}


private:
    // For UMeshoutput info
    int             myNbNodes;      // integer for number of nodes
    int             myNbInstance;   // number of instances
    int             myNbElements[6];// Number of elements for type : 1st TETS, 1st PENTS, 1st HEXS, 2nd TETS, 2nd PENTS, and 2nd HEXS;
    int             myNbElmTotal;   //total number of elements
    int             myNbComposits;  //number of Composition, in case of energy tally of n, p
    long long int   myNPS;          // number of particle history;
    int             myNbRegEdit;    //number of regular edits;
    int             myNbCompEdit;   //number of composite edits;
    double          myLengthFactor; //the length conversion factor;

    //for user interaction
    bool            removeMultiplier;  // does the multiplier need to be remove or not, yes-remove, no-no
    bool            keepInstances;     // does the independent instance is keep or no, yes-keep, no-discard and return only the whole mesh

    //for the interface code
    string          myUMeshOutFileName; // the file name (include path)
    fstream         myUMeshOutFile;
    bool            EndOfFile;          //indicate if the file ended or not, yes-ended; no-no
    BlockIdentifier Identifiers;        // identification of data block
    long long int   lineNb;         // the current line Nb by getLine().
    long long int   lineNbProc;     // the current line Nb being processed
    string          currentLine;    //currentLine by getLine()
    vector<QString> currentBlock;   // the current data block, fetch all string of one block
    DataArrayDouble * NodeCoordX;   // X coordinates for nodes
    DataArrayDouble * NodeCoordY;
    DataArrayDouble * NodeCoordZ;

    DataArrayInt*   ElmConn; // element connectivities
    DataArrayInt*   ElmConnIdx;// element connectivity index, see MEDCoupling document for more information
    vector <const DataArrayDouble * >  DataList; //a list of data array where result field is append
    vector <const MEDCouplingFieldDouble * > myFieldList; // field, including mesh and result data,

    QMap <UMeshElmType, int> ElmNodeCnt;  //how many nodes for each type of element
    QMap <UMeshElmType, int> MEDElmType;  //the match UMeshElmType with MED elmenet type
    QMap <int, QString > ParticleType; //mapping particle IPT (MCNP term) with Particle type
    QString         EditPrefix;    // the prefix for the current edit output

    vector <Instance*> InstanceList; // a list of instances, should be delete at the end

    UMeshOrder               MeshConnOrder; //connectivity order: ordered with node or element
    MCNPVersion              myMCNPVersion; // the MCNP version for this processing

private:

    bool			read_calling_code();
    bool			read_int_paras();
    bool			read_real_paras();
//    bool			read_paticle_list();
    bool			read_edit_description();
    bool			read_instance_names();
    bool			read_instance_element_type();
    bool            read_nodes_x();
    bool            read_nodes_y();
    bool            read_nodes_z();
    bool			read_material();
    bool			read_connectivities(const UMeshElmType & theElmType);
    bool			read_edit_output();
    bool			read_edit_sets();
    bool            process_data();
    void            get_aLine();   //get a line and increase lineCnt;
    bool            get_identifiers();  //get identifiers from the currentLine;
    int             cal_block_lines();  //calculate how many lines in this block
    bool            get_aBlock();  //get a block
    UMeshBlockType  get_block_type(); //read the first line of the currentBlock, recognize the block type
    bool            check_convert(const bool & Ok) ; //if Ok=false, show error message of data convertion
    QString         get_line_in_block(const int & lineIdx); //get the lineIdx line in currentBlock,

};

class Instance
{
public:
    QString         myName;
    int             myFirstElement [6];  //first element of type : 1st TETS, 1st PENTS, 1st HEXS, 2nd TETS, 2nd PENTS, and 2nd HEXS;
    int             myLastElement  [6];  //last  element of type : 1st TETS, 1st PENTS, 1st HEXS, 2nd TETS, 2nd PENTS, and 2nd HEXS;
private:
    QMap <UMeshElmType, int> ElmNodeCnt;  //how many nodes for each type of element
    QMap <UMeshElmType, int> MEDElmType;  //the match UMeshElmType with MED elmenet type

public:
    Instance(const QString & aName);
    ~Instance();
    const MEDCouplingFieldDouble * extractSubField(const MEDCouplingFieldDouble * theWholeField); //extract the sub field from the whole field



};



#endif // MCMESHTRAN_MCNPUMESHREADER_HXX
