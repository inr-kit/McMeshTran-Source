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
#include "MCMESHTRAN_TRIPOLIFmeshReader.hxx"


#include "utilities.h"
#include <QStringList>

MCMESHTRAN_TRIPOLIFmeshReader::MCMESHTRAN_TRIPOLIFmeshReader()
{
    myFileName = "";
    currentLine = "";
    myBatch = 1;
    mySize = 1;
    EndOfFile = false;
    iBatch = 0;
}

MCMESHTRAN_TRIPOLIFmeshReader::~MCMESHTRAN_TRIPOLIFmeshReader()
{
    //decrease the references
  for (int i=0; i<myMeshList.size(); i++)
      myMeshList.at(i)->decrRef();
  for (int i=0; i<myTallyErrorList.size(); i++)
      myTallyErrorList.at(i)->decrRef();
  for (int i=0; i<myFieldList.size(); i++)
      myFieldList.at(i)->decrRef();
}

/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::loadFile
 *  load TRIPOLI output file with mesh tallies
 * \param FileName the full file path and name
 * \return false if errors
 */
bool  MCMESHTRAN_TRIPOLIFmeshReader::loadFile(const QString &FileName,  const bool & isVolAvg )
{
    //open the file
    myFileName = FileName;
    myFile.open(myFileName.toLatin1());
    if(myFile.is_open())
        cout << "File " << myFileName.toStdString() << " is open.\n";
    else    {
        cout << "Error opening " << myFileName.toStdString()  << ".\n";
        return false;
    }


    //Start to read the file
    bool ResponseReady = false, GridsReady = false, ScoresReady = false, SimReady = false;
    T4Block aBlockKey = getLineAndCheck();
    while (!EndOfFile)
    {
        switch (aBlockKey)
        {
        case BLOCK_Responses:
            if (!ResponseReady) //to avoid reading twice
                ResponseReady = readResponses();
            if (!ResponseReady) return false;  //if error, quit
            break;
        case BLOCK_Grids:
            if (!GridsReady)//to avoid reading twice
                GridsReady = readGrids();
            if (!GridsReady) return false;  //if error, quit
            break;
        case BLOCK_Scores:
            if (!ScoresReady)//to avoid reading twice
                ScoresReady = readScores();
            if (!ScoresReady) return false;  //if error, quit
            break;
        case BLOCK_Tally:
            if (ResponseReady && GridsReady && ScoresReady) {
                //TRIPOLI will output results in  every 100 batch
                //it is possible that the results is in 100 batch or in the final batch
                //try to load the results in every 100 batch and also the final batch
                //the later result will ovewrite the older result
//comment out, new way of implelementation. if (iBatch == myBatch || (iBatch % 100) == 0) //search for the last batch
                    if (!readTallies()) return false;
            }
            else {
                MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::loadFile: found tally section, but mesh info is not ready!")
                return false;
            }
            break;
        case BLOCK_Simulation:
            if (!SimReady)//IMPORTANT, because SIMULATION keyword appreas several times in the ouput file
                SimReady = readSimulation();
            if (!SimReady) return false;  //if error, quit
            break;
        default:  //for BLOCK_unknown, continue
            break;
        }

//        //if mesh info is ready, process it and then read tally results
//        if (ResponseReady && GridsReady && ScoresReady && !procMeshesReady) {
//            procMeshesReady = processMeshes();
//            if (!procMeshesReady) return false;
//        }
        aBlockKey = getLineAndCheck(); //?? suitable places?
    }

    //process results
    if (!processMeshes()) return false;
    if (!processResults(isVolAvg)) return false;

    return true;
}

/*!
 * \brief return a list of field get by this reader, these mesh should decrRef() after used.
 * \return return a list of field, these mesh should decrRef() after used.
 */
vector <const MEDCouplingFieldDouble *> MCMESHTRAN_TRIPOLIFmeshReader::getFieldList()
{
    vector <const MEDCouplingFieldDouble*> aTmpList ;
    for (int i=0; i<myFieldList.size(); i++)    {
        myFieldList[i]->incrRef();    //increase reference because myFieldList will be destroy when the reader is deleted,
        aTmpList.push_back(myFieldList[i]);
    }
    return aTmpList;
}



T4Block     MCMESHTRAN_TRIPOLIFmeshReader::getLineAndCheck()
{
    getLine();
    return checkLine();
}

void MCMESHTRAN_TRIPOLIFmeshReader::getLine()
{
    currentLine.clear();
    getline(myFile, currentLine);
    if(myFile.eof()) EndOfFile = true;   //to inform that file is ended
//    cout <<currentLine<<endl;//for testing
//    ++lineNb;

}

T4Block      MCMESHTRAN_TRIPOLIFmeshReader::checkLine()
{
    //for checking if the final batch is reach
    //important because each 100 batch will automatic output results.
    if (currentLine.find("batch number :") != string::npos)
        iBatch ++;

    //keeping in mind that the "END_RESPONSES" etc. also satisfied
    //therefore pass these "END_**" because calling this


    if (currentLine.find("RESPONSES") != string::npos)
        return BLOCK_Responses;
    else if (currentLine.find("GRID_LIST") != string::npos)
        return BLOCK_Grids;
    else if (currentLine.find("SCORE") != string::npos)
        return BLOCK_Scores;
    else if (currentLine.find("SIMULATION") != string::npos)
        return BLOCK_Simulation;
    else if (currentLine.find("Results on a mesh") != string::npos)
        return BLOCK_Tally;
    else return BLOCK_Unknown;
}

/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::readResponses
 *  read the RESPONSES section, the responses will be restored
 *  in class member \a myResponses
 * \return false if error
 */
bool        MCMESHTRAN_TRIPOLIFmeshReader::readResponses()
{
    QString aSection;
    //push all the staff into one string
    while (currentLine.find("END_RESPONSES") == string::npos) {  //while section not ended
        aSection += QString(currentLine.c_str()) + " ";  // " " is a delimiter
        getLine();
        if (EndOfFile) {
            MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::readResponses: file ended unexpectedly!")
            return false;
        }
    }
    aSection += QString(currentLine.c_str()) + " "; //push also the last one

    //deal with this whole  section
    QStringList aStringList = aSection.split(" ",QString::SkipEmptyParts);
    int idx = 1; //index for aStringList, skip the first keyword
    int NbResponses = aStringList.at(idx++).toInt(); //get the Number of responses
    while (idx < aStringList.size() -1)  //skip also the last keyword
    {
        T4RESPONSE aResponse;
        if (aStringList.at(idx) == "NAME") {
            aResponse.Name = aStringList.at(++idx).trimmed(); //save the name,
            idx ++; //jump two words
        }
        aResponse.Type = aStringList.at(idx++).trimmed() ;
        aResponse.Particle = aStringList.at(idx++).trimmed();
        myResponses.push_back(aResponse);
    }
    if (myResponses.size() != NbResponses) {
        MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::readResponses: Not correct number of responses read!")
        return false;
    }
    return true;
}

bool         MCMESHTRAN_TRIPOLIFmeshReader::readGrids()
{
    QString aSection;
    //push all the staff into one string
    while (currentLine.find("END_GRID_LIST") == string::npos) {  //while section not ended
        aSection += QString(currentLine.c_str()) + " ";  // " " is a delimiter
        getLine();
        if (EndOfFile) {
            MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::readGrids: file ended unexpectedly!")
            return false;
        }
    }
    aSection += QString(currentLine.c_str()) + " "; //push also the last one


    //deal with this whole  section
    QStringList aStringList = aSection.split(" ",QString::SkipEmptyParts);
    int idx = 1; //index for aStringList, skip the first keyword
    int NbGrids = aStringList.at(idx++).toInt(); //get the Number of grids
    while (idx < aStringList.size() -1)  //skip also the last keyword
    {
        T4GRID aGrid;
        aGrid.Name = aStringList.at(idx++).trimmed();
        int NbInterval = aStringList.at(idx++).trimmed().toInt();
        for (int i=0; i<NbInterval; i++) { //push back the intervals
            aGrid.List.push_back(aStringList.at(idx++).trimmed().toDouble());
        }
        myGrids.push_back(aGrid);
    }
    if (myGrids.size() != NbGrids) {
        MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::readGrids: Not correct number of grids read!")
        return false;
    }
    return true;
}


/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::readScores
 *  read the SCORE section and create scores
 *  the mesh scores is restored in \a myScores
 * \return false if error
 */
bool  MCMESHTRAN_TRIPOLIFmeshReader::readScores()
{
    QString aSection;
    //push all the staff into one string
    while (currentLine.find("END_SCORE") == string::npos) {  //while section not ended
        aSection += QString(currentLine.c_str()) + " ";  // " " is a delimiter
        getLine();
        if (EndOfFile) {
            MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::readScores: file ended unexpectedly!")
            return false;
        }
    }
    aSection += QString(currentLine.c_str()) + " "; //push also the last one

    //deal with this whole  section
    QStringList aStringList = aSection.split(" ",QString::SkipEmptyParts);
    int idx = 1; //index for aStringList, skip the first keyword
    int NbScores = aStringList.at(idx++).toInt(); //get the Number of scores

    while (idx < aStringList.size() -1)  //skip also the last keyword
    {
        //put one Score section into a string
        QString aScoreStr;
        aScoreStr += aStringList.at(idx++).trimmed() + " "; //push the first four words
        aScoreStr += aStringList.at(idx++).trimmed() + " ";
        aScoreStr += aStringList.at(idx++).trimmed() + " ";
        aScoreStr += aStringList.at(idx++).trimmed() + " ";
        while (1) {
            QString aTmpStr = aStringList.at(idx).trimmed();
            if (aTmpStr == "NAME")  //found next score with NAME
                break;
            if (aTmpStr == "COLL" || aTmpStr == "TRACK" ||  aTmpStr == "FLUX_PT" || aTmpStr == "SURF"  ) {//found next score with SCORE type
                idx --; //return back to the position where the Response id or name is given
                break;
            }
//            if (aTmpStr == "END_SCORE")  //found end of the score
            if (idx == (aStringList.size() -1) )//found end of the score
                break;
            //else push this word into the string
            aScoreStr += aStringList.at(idx++).trimmed() + " ";
        }//loop out after next

        //process this Score
        if (!processAScore(aScoreStr)) return false;
    }

    if (myScores.size() != NbScores) {
        MESSAGE ("##Warning MCMESHTRAN_TRIPOLIFmeshReader::readScores: Some scores are not processed!")
    }

    return true;
}

/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::processAScore
 *  process a score using a score string,
 *  push a mesh tally score into \a myScores, and dump other kind of scores
 * \param aScore a string contains the configuration of the score
 * \return false if error
 */
bool        MCMESHTRAN_TRIPOLIFmeshReader::processAScore(const QString & aScoreStr)
{
    //check if the score is what we need
    if (aScoreStr.indexOf("MESH") == -1) {  //if MESH not found
        MESSAGE ("##Message MCMESHTRAN_TRIPOLIFmeshReader::processAScore: Skip a non-mesh score!")
                return true;
    }
    else if (aScoreStr.indexOf("TIME") != -1 || aScoreStr.indexOf("ANGULAR") != -1 || aScoreStr.indexOf("2D_ANGULAR") != -1  ) {
        //if found a mesh tally with time/angular bins, give up because difficult to deal with
        MESSAGE ("##Warning MCMESHTRAN_TRIPOLIFmeshReader::processAScore: Not able to process a mesh tally with TIME/ANGULAR/2D_ANGULAR bins!")
                return true;
    }
    else if (aScoreStr.indexOf("TRIANGLEZ") != -1 ) {
        MESSAGE ("##Warning MCMESHTRAN_TRIPOLIFmeshReader::processAScore: Not able to process a mesh tally with TRIANGLEZ! ")
                return true;
    }
    else if (aScoreStr.indexOf("SPHERE") != -1 ) {
        MESSAGE ("##Warning MCMESHTRAN_TRIPOLIFmeshReader::processAScore: Not able to process a mesh tally with SPHERE coordinates! ")
                return true;
    }

    //deal with this whole  section
    QStringList aStringList = aScoreStr.split(" ",QString::SkipEmptyParts);
    int idx = 0; //index for aStringList

    T4SCORE aScore;
    if (aStringList.at(idx) == "NAME") {  //save the name //idx=0,use 0
        aScore.Name = aStringList.at(++idx);//idx=1,use 1
        aScore.Response = findResponse(aStringList.at(++idx));//idx=2,use 2
        aScore.Type = aStringList.at(++idx);//idx=3,use 3
        idx++; //jump to next keyword
    }
    else {
        aScore.Name = "";
        aScore.Response = findResponse(aStringList.at(idx++)); //use 0, idx=1
        aScore.Type = aStringList.at(idx++);//use 1 idx=2
    }
//    while (idx < aStringList.size()) {
    while (idx < aStringList.size()-1) {
        if (aStringList.at(idx) == "GRID") {
            aScore.Grid = findGrid(aStringList.at(++idx));  //idx=idx+1, use idx+1
            idx++;
        }
        if (aStringList.at(idx) == "MESH") {
            if (aStringList.at(++idx) != "ENTROPY") idx--; //skip useless keyword
            if (aStringList.at(++idx) != "MESH_INFO") idx--;//check if has MESH_INFO
            else aScore.Mesh.hasMESH_INFO = true;
            aScore.MeshType = "MESH"; //mesh type
            aScore.Coordinate = "CARTESIAN";
            aScore.Mesh.Interval.A = aStringList.at(++idx).toInt();
            aScore.Mesh.Interval.B = aStringList.at(++idx).toInt();
            aScore.Mesh.Interval.C = aStringList.at(++idx).toInt();
            aScore.Mesh.Delta.A = aStringList.at(++idx).toDouble();
            aScore.Mesh.Delta.B = aStringList.at(++idx).toDouble();
            aScore.Mesh.Delta.C = aStringList.at(++idx).toDouble();
            idx++; //skip the key word "FRAME"
            aScore.Mesh.RefPoint.A = aStringList.at(++idx).toDouble();
            aScore.Mesh.RefPoint.B = aStringList.at(++idx).toDouble();
            aScore.Mesh.RefPoint.C = aStringList.at(++idx).toDouble();
//            idx++;
            if (idx < aStringList.size()-1 )idx++;
        }
        if (aStringList.at(idx) == "EXTENDED_MESH") {
            aScore.MeshType = "EXTENDED_MESH";
            idx++; //jump to next key word
            //using redundant way to avoid using loop
            if (aStringList.at(idx) == "WINDOW") {
                aScore.ExtMesh.LowerPoint.A = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.LowerPoint.B = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.LowerPoint.C = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.UpperPoint.A = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.UpperPoint.B = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.UpperPoint.C = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.Interval.A = aStringList.at(++idx).toInt();
                aScore.ExtMesh.Interval.B = aStringList.at(++idx).toInt();
                aScore.ExtMesh.Interval.C = aStringList.at(++idx).toInt();
  //            idx++;
                if (idx < aStringList.size()-1 )idx++;
            }
            if (aStringList.at(idx) == "FRAME") {
                aScore.Coordinate = aStringList.at(++idx);
                aScore.ExtMesh.Origin.A = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.Origin.B = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.Origin.C = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.V1.A = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.V1.B = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.V1.C = aStringList.at(++idx).toDouble();
                if (aScore.Coordinate == "CARTESIAN") {
                    aScore.ExtMesh.V2.A = aStringList.at(++idx).toDouble();
                    aScore.ExtMesh.V2.B = aStringList.at(++idx).toDouble();
                    aScore.ExtMesh.V2.C = aStringList.at(++idx).toDouble();
                }
                aScore.ExtMesh.V3.A = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.V3.B = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.V3.C = aStringList.at(++idx).toDouble();
//                idx++;
                if (idx < aStringList.size()-1 )idx++;
            }
            if (aStringList.at(idx) == "WINDOW") {
                aScore.ExtMesh.LowerPoint.A = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.LowerPoint.B = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.LowerPoint.C = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.UpperPoint.A = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.UpperPoint.B = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.UpperPoint.C = aStringList.at(++idx).toDouble();
                aScore.ExtMesh.Interval.A = aStringList.at(++idx).toInt();
                aScore.ExtMesh.Interval.B = aStringList.at(++idx).toInt();
                aScore.ExtMesh.Interval.C = aStringList.at(++idx).toInt();
//                idx++;
                if (idx < aStringList.size()-1 )idx++;
            }
        }
    }

    //finish filling the score
    myScores.push_back(aScore);
    return true;
}


/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::processMeshes
 *  process the mesh in the scores and create MEDCouplingUMESH
 *  each score creates a mesh
 * \return false if error
 */
bool        MCMESHTRAN_TRIPOLIFmeshReader::processMeshes()
{
    //loop for the scores
    for (int n=0; n<myScores.size(); n++)
    {
        vector <double> aCoordVec; //Vector to store coordinates of points
        T4SCORE aScore = myScores.at(n);
        if (aScore.MeshType == "MESH")
        {
            //The mesh is in Cartesian coordinate,
            //calculate the mesh origin
            //because the reference point is the cell center of the reference cell
            //the origin is calculated by deducting half-delta
            double OriginX, OriginY, OriginZ;
            OriginX =  aScore.Mesh.RefPoint.A - aScore.Mesh.Delta.A/2;
            OriginY =  aScore.Mesh.RefPoint.B - aScore.Mesh.Delta.B/2;
            OriginZ =  aScore.Mesh.RefPoint.C - aScore.Mesh.Delta.C/2;
            //push points into an array
            //the order is x->y->z
            //because number of node in x/y/z = interval+1, using "<=" to create the last point
            for (int i=0; i<=aScore.Mesh.Interval.A; i++) { // loop for x
                for (int j=0; j<=aScore.Mesh.Interval.B; j++) { //loop for y
                    for (int k=0; k<=aScore.Mesh.Interval.C; k++) { //loop for z
                        double AA, BB, CC;
                        aCoordVec.push_back(AA=  OriginX+ i*aScore.Mesh.Delta.A);
                        aCoordVec.push_back(BB=  OriginY+ j*aScore.Mesh.Delta.B);
                        aCoordVec.push_back(CC=  OriginZ+ k*aScore.Mesh.Delta.C);
//                        cout << "i,j,k \t" << i<<"\t"<<j<<"\t"<<k<<"\t"<<AA<<"\t"<<BB<<"\t"<<CC<<"\t"<<endl;//for testing
                    }
                }
            }
        }
        else if (aScore.MeshType == "EXTENDED_MESH")
        {
            //normalized and orthogonlize the Frame
            processFrame(aScore);

            //deal with different coordinates
            if (aScore.Coordinate == "CARTESIAN")
            {
                //calculate the delta
                double DeltaX, DeltaY, DeltaZ;
                DeltaX = (aScore.ExtMesh.UpperPoint.A - aScore.ExtMesh.LowerPoint.A) / aScore.ExtMesh.Interval.A;
                DeltaY = (aScore.ExtMesh.UpperPoint.B - aScore.ExtMesh.LowerPoint.B) / aScore.ExtMesh.Interval.B;
                DeltaZ = (aScore.ExtMesh.UpperPoint.C - aScore.ExtMesh.LowerPoint.C) / aScore.ExtMesh.Interval.C;
                //push the points into an array
                //the order is x->y->z
                //because number of node in x/y/z = interval+1, using "<=" to create the last point
                Vec3Double aTmpPoint;
                for (int i=0; i<=aScore.ExtMesh.Interval.A; i++) { // loop for x
                    for (int j=0; j<=aScore.ExtMesh.Interval.B; j++) { //loop for y
                        for (int k=0; k<=aScore.ExtMesh.Interval.C; k++) { //loop for z
                            aTmpPoint.A = aScore.ExtMesh.LowerPoint.A+ i*DeltaX;
                            aTmpPoint.B = aScore.ExtMesh.LowerPoint.B+ j*DeltaY;
                            aTmpPoint.C = aScore.ExtMesh.LowerPoint.C+ k*DeltaZ;
                            transformPoint(aScore, aTmpPoint); //transform the point
                            aCoordVec.push_back( aTmpPoint.A );
                            aCoordVec.push_back( aTmpPoint.B );
                            aCoordVec.push_back( aTmpPoint.C );
                        }
                    }
                }
            }
            else if (aScore.Coordinate == "CYLINDER")
            {
                //calculate the delta
                double DeltaR, DeltaT, DeltaZ;
                DeltaR = (aScore.ExtMesh.UpperPoint.A - aScore.ExtMesh.LowerPoint.A) / aScore.ExtMesh.Interval.A;
                DeltaT = (aScore.ExtMesh.UpperPoint.B - aScore.ExtMesh.LowerPoint.B) / aScore.ExtMesh.Interval.B;
                DeltaZ = (aScore.ExtMesh.UpperPoint.C - aScore.ExtMesh.LowerPoint.C) / aScore.ExtMesh.Interval.C;
                //push the points into an array
                //the order is x->y->z
                //because number of node in x/y/z = interval+1, using "<=" to create the last point
                for (int i=0; i<=aScore.ExtMesh.Interval.A; i++) { // loop for R
                    for (int j=0; j<=aScore.ExtMesh.Interval.B; j++) { //loop for T
                        for (int k=0; k<=aScore.ExtMesh.Interval.C; k++) { //loop for Z
                            double CylR, CylT, CylZ;
                            Vec3Double aTmpPoint;
                            CylR = aScore.ExtMesh.LowerPoint.A+ i*DeltaR;
                            CylT = aScore.ExtMesh.LowerPoint.B+ j*DeltaT; //Theta angle unit: Radian
                            CylZ = aScore.ExtMesh.LowerPoint.C+ k*DeltaZ;
                            //transform the coordinate
                            //x=r*cos(theta)
                            //y=r*sin(theta)
                            //z=h
                            aTmpPoint.A = CylR * cos(CylT);
                            aTmpPoint.B = CylR * sin(CylT);
                            aTmpPoint.C = CylZ;
                            transformPoint(aScore,aTmpPoint);//transform according to the FRAME
                            aCoordVec.push_back( aTmpPoint.A );
                            aCoordVec.push_back( aTmpPoint.B );
                            aCoordVec.push_back( aTmpPoint.C );
                        }
                    }
                }
            }
            else
            {
                MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::processMeshes: Only capabal of dealing with CARTESIAN/CYLINDER coordinates!");
                return false;
            }
        }
        //create the mesh
        if (!createMesh(aScore, aCoordVec)) return false;

    }  //end of loop scores
    return true;
}

/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::processFrame
 *  process the mesh frame in EXTENDED_MESH,
 *  normalized the vector, and orthogonlize them
 * \param aScore the score to be process
 * \return false if error
 */
void        MCMESHTRAN_TRIPOLIFmeshReader::processFrame(T4SCORE & aScore)
{
    //first orthogonlize the frame if in cylinderical coordinates
    if (aScore.Coordinate == "CYLINDER") {
        orthogonlizeVec3(aScore.ExtMesh.V3 /*axis */, aScore.ExtMesh.V1 , aScore.ExtMesh.V2 /*unfill_vector*/);
    }
    //normalize those vectors
    normalizeVec3(aScore.ExtMesh.V1);
    normalizeVec3(aScore.ExtMesh.V2);
    normalizeVec3(aScore.ExtMesh.V3);
}

/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::normalizeVec3
 *  To normlize a vector
 * \param aVec3 a vector to be normalied
 */
void        MCMESHTRAN_TRIPOLIFmeshReader::normalizeVec3 (Vec3Double & aVec3)
{
    double theModulus = sqrt(aVec3.A*aVec3.A + aVec3.B*aVec3.B + aVec3.C*aVec3.C);
    aVec3.A = aVec3.A / theModulus;
    aVec3.B = aVec3.B / theModulus;
    aVec3.C = aVec3.C / theModulus;
}

/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::orthogonlizeVec3
 *  To orthogonlize the coordinate vector, mostly needed for cylinderical coordinates
 * \param RefVec the vector to be kept. for cylinder, it is the axis vector
 * \param V2 The second vector. For cylinder, it is the vector where theta starts
 * \param V3 The third vector, to be calculated
 */
void  MCMESHTRAN_TRIPOLIFmeshReader::orthogonlizeVec3(const Vec3Double & RefVec, Vec3Double & V2, Vec3Double & V3)
{
    //the method used in here is :
    //first, RefVec cross product with V2 to get V3;
    //second, V3 cross product with RefVec to change V2.

    //cross product equation:
    //    vector.x = Ay*Bz - By*Az;
    //    vector.y = Bx*Az - Ax*Bz;
    //    vector.z = Ax*By - Ay*Bx;

    V3.A = RefVec.B * V2.C - V2.B * RefVec.C;
    V3.B = V2.A * RefVec.C - RefVec.A * V2.C;
    V3.C = RefVec.A * V2.B - V2.A * RefVec.B;

    V2.A = V3.B * RefVec.C - RefVec.B * V3.C;
    V2.B = RefVec.A * V3.C - V3.A * RefVec.C;
    V2.C = V3.A * RefVec.B - RefVec.A * V3.B;
}

/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::transformPoint
 *   transforming point
 * \param aScore for fetching the translation and rotation matrix
 * \param aPoint the point to be transform and returned
 */
void        MCMESHTRAN_TRIPOLIFmeshReader::transformPoint (const T4SCORE & aScore, Vec3Double & aPoint)
{

    //first rotate according to the FRAME vector
    //because here the V1-V2-V3 is "OLD" and original coordinate (X-Y-Z) is the "NEW" coordinate,
    //and vector X =1 0 0, Y=0 1 0, Z=0 0 1, and V1, V2, V3 is normalized
    //thus cos(X,V1)= V1.A  cos(X,V2)= V2.A  cos(X,V3)= V3.A
    //     cos(Y,V1)= V1.B  cos(Y,V2)= V2.B  cos(Y,V3)= V3.B
    //     cos(Z,V1)= V1.C  cos(Z,V2)= V2.C  cos(Z,V3)= V3.C

    Vec3Double bPoint;

    bPoint.A = aPoint.A * aScore.ExtMesh.V1.A + aPoint.B * aScore.ExtMesh.V2.A + aPoint.C * aScore.ExtMesh.V3.A;
    bPoint.B = aPoint.A * aScore.ExtMesh.V1.B + aPoint.B * aScore.ExtMesh.V2.B + aPoint.C * aScore.ExtMesh.V3.B;
    bPoint.C = aPoint.A * aScore.ExtMesh.V1.C + aPoint.B * aScore.ExtMesh.V2.C + aPoint.C * aScore.ExtMesh.V3.C;
    aPoint = bPoint;
    //then moving according to the ORIGIN,
    //should plus with ORIGIN because original coordinate (X-Y-Z) is the "NEW" coordinate.
    aPoint.A = aPoint.A + aScore.ExtMesh.Origin.A;
    aPoint.B = aPoint.B + aScore.ExtMesh.Origin.B;
    aPoint.C = aPoint.C + aScore.ExtMesh.Origin.C;

}

/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::createMesh
 *  create a unstructured mesh using a list of coordinates
 *  actually it is a structured mesh, just using unstructred way to discribe it
 * \param aCoordVec a list of coordinates, x, y, z, x, y, z, ...
 * \return false if error
 */
bool        MCMESHTRAN_TRIPOLIFmeshReader::createMesh (const T4SCORE & aScore, vector <double > & aCoordVec)
{
    //copy the coordinates to a MED array
    //array for MEDCoupling mesh
    DataArrayDouble * arrayCor = DataArrayDouble::New();
    arrayCor->alloc(aCoordVec.size()/3 , 3);  //a data array with 3 components (MED term)
    copy(aCoordVec.begin(), aCoordVec.end(), arrayCor->getPointer());


    //make an array of connectivities and connectivity indices
    vector <int> Connectivity ; //nodal connectivity
    vector <int> NodalConnIndex;
    int Imax, Jmax, Kmax;
    if (aScore.MeshType == "MESH") {
        Imax = aScore.Mesh.Interval.A +1; //+1 to keep consist with MCNP mesh tally reading method
        Jmax = aScore.Mesh.Interval.B +1;
        Kmax = aScore.Mesh.Interval.C +1;
    }
    else if (aScore.MeshType == "EXTENDED_MESH") {
        Imax = aScore.ExtMesh.Interval.A +1;
        Jmax = aScore.ExtMesh.Interval.B +1;
        Kmax = aScore.ExtMesh.Interval.C +1;
    }
    //see http://www.grc.nasa.gov/WWW/cgns/CGNS_docs_current/user/started.html#sec:unstr
//    for (int k=0; k<Kmax-1; k++)  //k=0~Kmax-2,   like MCNP mesh tally reading method
//    {
//        for (int j=0; j<Jmax-1; j++)
//        {
//            for (int i=0; i<Imax-1; i++)
//            {
//                int firstNode = i + j*Imax + k*Imax*Jmax;
//                Connectivity.push_back(INTERP_KERNEL::NORM_HEXA8); // for each cell, first put the cell type
//                NodalConnIndex.push_back(Connectivity.size() - 1); // put the index of this cell in Connectivity array into index array

//                Connectivity.push_back(firstNode);
//                Connectivity.push_back(firstNode + 1);
//                Connectivity.push_back(firstNode + 1 + Imax);
//                Connectivity.push_back(firstNode + Imax);
//                Connectivity.push_back(firstNode + Jmax * Imax);
//                Connectivity.push_back(firstNode + Jmax * Imax + 1);
//                Connectivity.push_back(firstNode + Jmax * Imax + 1 + Imax);
//                Connectivity.push_back(firstNode + Jmax * Imax + Imax);
//            }
//        }
//    }
    for (int i=0; i<Imax-1; i++)
    {
        for (int j=0; j<Jmax-1; j++)
        {
            for (int k=0; k<Kmax-1; k++)  //k=0~Kmax-2,   like MCNP mesh tally reading method
            {
                int firstNode = i*Kmax*Jmax + j*Kmax + k;
                Connectivity.push_back(INTERP_KERNEL::NORM_HEXA8); // for each cell, first put the cell type
                NodalConnIndex.push_back(Connectivity.size() - 1); // put the index of this cell in Connectivity array into index array

                Connectivity.push_back(firstNode);
                Connectivity.push_back(firstNode + 1);
                Connectivity.push_back(firstNode + 1 + Kmax);
                Connectivity.push_back(firstNode + Kmax);
                Connectivity.push_back(firstNode + Jmax * Kmax);
                Connectivity.push_back(firstNode + Jmax * Kmax + 1);
                Connectivity.push_back(firstNode + Jmax * Kmax + 1 + Kmax);
                Connectivity.push_back(firstNode + Jmax * Kmax + Kmax);
            }
        }
    }
    //the last element of NodalConnIndex is Connectivity.size(), important!!
    //see:http://docs.salome-platform.org/salome_6_6_0/gui/MED/MEDCouplingUMeshPage.html
    NodalConnIndex.push_back(Connectivity.size());

    //copy nodal connectivity
    DataArrayInt * aConn = DataArrayInt::New();
    aConn->alloc(Connectivity.size(),1);
    copy(Connectivity.begin(), Connectivity.end(), aConn->getPointer());

    //copy nodal connectivity index, see MEDCoupling document
    DataArrayInt * aConnI = DataArrayInt::New();
    aConnI->alloc(NodalConnIndex.size(),1);
    copy(NodalConnIndex.begin(), NodalConnIndex.end(), aConnI->getPointer());


    //creat a mesh and assign data for it
    MEDCouplingUMesh *aMesh=MEDCouplingUMesh::New("TRIPOLI_MESH", 3);
    aMesh->setConnectivity(aConn, aConnI, false);
    aMesh->setCoords(arrayCor);

    arrayCor->decrRef();
    aConn->decrRef();
    aConnI->decrRef();

    //append the mesh to the list
    myMeshList.push_back(aMesh);
//should never do this!!    aMesh->decrRef();

    return true;
}

/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::readTallies
 *  read A block of tally results and store in MED array
 * \return false if error
 */
bool MCMESHTRAN_TRIPOLIFmeshReader::readTallies()
{
    //TRIPOLI will output results in  every 100 batch
    //we have to clear the results before getting the result in the final batch
//    for (int i=0; i<myTallyErrorList.size(); i++)
//        myTallyErrorList.at(i)->decrRef();
//    myTallyErrorList.clear();

    //skip lines before the line "Energy range (in MeV)..."
    getLine();
    getLine();
    getLine();
    getLine();

    //locate the first line of the result
//    while (!EndOfFile && currentLine.find("Energy range") == string::npos)
//        getLine();

    while (!EndOfFile && currentLine.find("Energy range") != string::npos)
    {
        vector <double> TallyError; //a vector for storing tallies and errors
        //read until reach blank line
        while (1) {
            getLine();
            QString aLine = currentLine.c_str();
            //check if blank line, if yes read a line more (might be next Energy Bin) and quit this loop
            if (aLine.trimmed().isEmpty()) {
                getLine();
                break;
            }
            //get the last two value as Tally and Error
            QStringList aStringList = aLine.split("\t", QString::SkipEmptyParts);
            TallyError.push_back(aStringList.at(aStringList.size() -2).toDouble()); //tally result
            TallyError.push_back(aStringList.at(aStringList.size() -1).toDouble()); //Error result
        }
        //process this energy bin

        //copy the data into an array
        DataArrayDouble * arrayTE = DataArrayDouble::New();
        arrayTE->alloc(TallyError.size()/2, 2);  //two solutions: tally and error
        arrayTE->setInfoOnComponent(0, "Tally");
        arrayTE->setInfoOnComponent(1, "Error");
        copy(TallyError.begin(), TallyError.end(), arrayTE->getPointer());

        myTallyErrorList.push_back(arrayTE);
//should never do this!!    arrayTE->decrRef();
    }

    return true;
}

/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::processResults
 *  process the results , corresponding them with mesh,
 *  and form MED fields
 * \return false if error
 */
bool        MCMESHTRAN_TRIPOLIFmeshReader::processResults(const bool & isVolAvg)
{
    //check if the results array is matching with scores
    int NbTallies = 0; //for counting tallies
    for (int i=0; i<myScores.size(); i++) {
        //check also the energy bins
        NbTallies += myScores.at(i).Grid.List.size() -1;
    }
    if (myTallyErrorList.size() < NbTallies) {
        MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::processResults: processed number of mesh tallies might be wrong!");
        return false;
    }
    if (myTallyErrorList.size() != NbTallies) {
        MESSAGE ("##Warning MCMESHTRAN_TRIPOLIFmeshReader::processResults: Number of tallies read not match with what expected for scores! Only the results at last will be processed.");
//        return false;
    }

    //loop the score to build fields
//    int fieldIdx =0; //index for myTallyErrorList;
    int fieldIdx = myTallyErrorList.size() - NbTallies; //index for myTallyErrorList; change to be adaptive to strange output format in TRIPOLI
    for (int i=0; i<myScores.size(); i++)
    {
        QString aFieldName;
        aFieldName += myScores.at(i).Type + "_";
        if (myScores.at(i).Name.trimmed().isEmpty())
            aFieldName += "Score_" + QString("%1").arg(i+1) + "_";
        else
            aFieldName += "Score_" +myScores.at(i).Name.trimmed() + "_";
        //add response attribute
        aFieldName += myScores.at(i).Response.Particle + "_" + myScores.at(i).Response.Type + "_";

        //get the volume of all mesh cells
//        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <MEDCouplingFieldDouble > aVolumeField ;
        ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr <ParaMEDMEM::DataArrayDouble> aVolArray;
        bool isCalAvg =  //whether calculate the average depends on
                isVolAvg //if require to, and
                && (myScores.at(i).MeshType == "EXTENDED_MESH"  //is EXTENDED_MESH (which is not average by default)
                    || (myScores.at(i).MeshType == "MESH" && !myScores.at(i).Mesh.hasMESH_INFO)); //or is MESH and does not already averaged (if MESH_INFO keyword then it is averaged already)

        if (isCalAvg) { //if need to make volume average,
//        aVolumeField = myMeshList.at(i)->getMeasureField(true);
//        aVolArray = aVolumeField->getArray();
           aVolArray = myMeshList.at(i)->getMeasureField(true)->getArray();
        }

        //loop for the energy bins
        for (int j=0; j<myScores.at(i).Grid.List.size()-1; j++)
        {
            QString bFieldName = aFieldName + "Ebin_" + QString("%1").arg(j+1);
            //make a field
            MEDCouplingFieldDouble* fieldOnCells=MEDCouplingFieldDouble::New(ON_CELLS,ONE_TIME);
            fieldOnCells->setName(bFieldName.toStdString());
            fieldOnCells->setNature(RevIntegral);
            fieldOnCells->setMesh(myMeshList.at(i));

            //if need to make volume average
            if(isCalAvg) {
                if (aVolArray->getNumberOfTuples() != myTallyErrorList.at(fieldIdx)->getNumberOfTuples()) { //check
                    MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::processResults: Number of cells not match with number of resutls!");
                    return false;
                }
                for (int k=0; k<aVolArray->getNumberOfTuples(); k++) {
                    *(myTallyErrorList.at(fieldIdx)->getPointer()+k*2) =
                            *(myTallyErrorList.at(fieldIdx)->getPointer()+k*2) / *(aVolArray->getPointer() +k);  //only Tally divides with volume
                }
            }
            fieldOnCells->setArray(myTallyErrorList.at(fieldIdx));
            myFieldList.push_back(fieldOnCells);
            fieldIdx ++;
        }
    }

    return true;
}

/*!
 * \brief findResponse
 *  using the response name or ID (in string) to find a response
 * \param aNameOrID the Name or ID
 * \return a response
 */
T4RESPONSE  MCMESHTRAN_TRIPOLIFmeshReader::findResponse(const QString & aNameOrID)
{
    bool isOK;
    int ID = aNameOrID.toInt(&isOK);
    if (isOK) { //id is ok
        return myResponses.at(ID -1); //return the response with the id (order)
    }
    else { //it is a name
        for (int i=0; i<myResponses.size(); i++) {
            if (myResponses.at(i).Name == aNameOrID)
                return myResponses.at(i);
        }
    }
    MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::findResponse: Cannot find the response: "<<aNameOrID.toStdString() <<" !");
    T4RESPONSE dumy;
    return dumy;
}

/*!
 * \brief findGrid
 *  find the grid
 * \param aName the grid name
 * \return return the grid
 */
T4GRID      MCMESHTRAN_TRIPOLIFmeshReader::findGrid(const QString & aName)
{
    for (int i=0; i<myGrids.size(); i++) {
        if (myGrids.at(i).Name == aName)
            return myGrids.at(i);
    }
    MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::findGrid: Cannot find the Grid: "<<aName.toStdString() <<" !");
    T4GRID dumy;
    return dumy;
}



/*!
 * \brief MCMESHTRAN_TRIPOLIFmeshReader::ReadSimulation
 *  Read simulation part to get the how many particle is calculated (NPS)
 *  the myNPS is filled
 * \return false if error
 */
bool        MCMESHTRAN_TRIPOLIFmeshReader::readSimulation()
{
    QString aSection;
    //push all the staff into one string
    while (currentLine.find("END_SIMULATION") == string::npos) {  //while section not ended
        aSection += QString(currentLine.c_str()) + " ";  // " " is a delimiter
        getLine();
        if (EndOfFile) {
            MESSAGE ("##Error MCMESHTRAN_TRIPOLIFmeshReader::readResponses: file ended unexpectedly!")
            return false;
        }
    }
    aSection += QString(currentLine.c_str()) + " "; //push also the last one

    //deal with this whole  section
    QStringList aStringList = aSection.split(" ",QString::SkipEmptyParts);
    int idx = 1; //index for aStringList, skip the first keyword
    int NbBatch = 1; //default one batch
    long long int NbSize = 1; //default one particle per batch
    while (idx < aStringList.size() -1)  //skip also the last keyword
    {

        if (aStringList.at(idx) == "BATCH") {
            NbBatch = aStringList.at(++idx).trimmed().toInt(); //save the batch
        }
        if (aStringList.at(idx) == "SIZE") {
            NbSize = aStringList.at(++idx).trimmed().toLongLong(); //save the size
        }
        idx++;
    }
    myBatch = NbBatch ;
    mySize =  NbSize;
    return true;
}
















