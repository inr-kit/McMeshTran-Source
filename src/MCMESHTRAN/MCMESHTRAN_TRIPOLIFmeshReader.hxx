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
#ifndef MCMESHTRAN_TRIPOLIFMESHREADER_HXX
#define MCMESHTRAN_TRIPOLIFMESHREADER_HXX

#include <QString>
#include <vector>
#include <fstream>
#include <MEDCouplingUMesh.hxx>
#include <MEDCouplingMemArray.hxx>
#include <MEDCouplingFieldDouble.hxx>

using namespace std;
using namespace ParaMEDMEM;
//Data structure for the Response
struct T4RESPONSE
{
    QString Name ;
    QString Type ;
    QString Particle ; //particle type: NEUTRON, PHOTON
};

struct T4GRID
{
    QString Name ;
    vector <double > List;
};

struct Vec3Double //a vector with 3 component
{
    double A;
    double B;
    double C;
};

struct Vec3Int //a vector with 3 component
{
    int A;
    int B;
    int C;
};

struct T4MESH // a TRIPOLI MESH keyword,
{
    bool hasMESH_INFO; //if has MESH_INFO keyword
    Vec3Int Interval; //How many segment
    Vec3Double Delta;    //each segment
    Vec3Double RefPoint ; //reference point: center of the reference cell!!
};

struct T4EXTMESH
{
    Vec3Double LowerPoint ; //lower point of the "box"
    Vec3Double UpperPoint; //upper point of the "box"
    Vec3Int Interval;//How many segment
    Vec3Double Origin;  //transform origin
    Vec3Double V1;      //transform axis 1
    Vec3Double V2;    //transform axis 2
    Vec3Double V3;    //transform axis 3

};

struct T4SCORE
{
    QString Name;
    QString Type;   //estimator type
    QString Coordinate; //CARTESIAN/CYLINDER
    T4RESPONSE Response; //a id or name links with response
    T4GRID Grid;       //a name links with grid
    QString MeshType; //MESH or EXTENDED_MESH
    T4MESH  Mesh;
    T4EXTMESH ExtMesh;
};

enum T4Block
{
    BLOCK_Responses,
    BLOCK_Grids,
    BLOCK_Scores,
    BLOCK_Tally,
    BLOCK_Simulation,
    BLOCK_Unknown
};


class MCMESHTRAN_TRIPOLIFmeshReader
{

public:
    MCMESHTRAN_TRIPOLIFmeshReader();
    ~MCMESHTRAN_TRIPOLIFmeshReader();
    bool        loadFile (const QString & FileName, const bool & isVolAvg = false);
    vector <const MEDCouplingFieldDouble *> getFieldList();  //get the field list, whole field and also separate instances
    long long int   get_NPS () {return myBatch*mySize; }             //return the NPS=batch*size


private:

    bool        readResponses();
    bool        readGrids();
    bool        readScores();
    bool        processAScore(const QString & aScoreStr);
    bool        processMeshes();
    void        processFrame(T4SCORE &aScore);
    void        orthogonlizeVec3(const Vec3Double &RefVec, Vec3Double &V2, Vec3Double &V3);
    void        normalizeVec3 (Vec3Double & aVec3);
    void        transformPoint (const T4SCORE & aScore, Vec3Double & aPoint);
    bool        createMesh (const T4SCORE & aScore, vector <double > & aCoordVec);
    bool        readTallies();
    bool        processResults( const bool & isVolAvg );
    T4RESPONSE  findResponse(const QString & aNameOrID);
    T4GRID      findGrid(const QString & aName);
    bool        readSimulation();

    T4Block     getLineAndCheck();
    void        getLine();
    T4Block     checkLine();

private:
    QString myFileName;
    ifstream myFile;        // abaqus file
    vector <T4RESPONSE>  myResponses;   // a list of response
    vector <T4GRID>      myGrids;   // a list of grids
    vector <T4SCORE>     myScores;  // a  list of scores
    vector <const MEDCouplingUMesh*> myMeshList;//for saving UMesh construct from instances, the merge field will append at the end
    vector < DataArrayDouble *> myTallyErrorList;//for saving tallies and errors, each array includes a list of tally and error
    vector <const MEDCouplingFieldDouble * > myFieldList; // field, including mesh and result data,
    long long int   myBatch;          // number of batch;
    long long int   mySize;          // number of size per batch;


    bool                EndOfFile; //true if EOF reach
    string              currentLine;    //currentLine by getLine()
    //for checking if the last batch is reach
    //the results before the final batch will be skip
    int                 iBatch;






};


//not implemeneted:
//3. connection to mcmceshtaran














#endif // MCMESHTRAN_TRIPOLIFMESHREADER_HXX
