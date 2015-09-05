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

#include "MCMESHTRAN_MCNPFmeshReader.hxx"

#include <iostream>
#include <sstream>
#include <algorithm>
#include <math.h>
#ifdef WIN32
#include <stdlib.h>
#endif

MCNPFmeshReader::MCNPFmeshReader()
{
	 
    m_CurrentLine = "";
	m_LineIndex = 0;
}

/*!
 * \brief read MCNP input file and construct new meshtallies
 *  the code first check Fmesh card and put all the line belonging to
 *  one Fmesh together into a string \c tmpBuffer, and put into a array \c FmeshBuffer
 *  then for each Fmesh \c tmpBuffer, decode the parameters and store into a MCNPFmesh instance
 * \param McnpInputFileName
 * \param MeshtalAarray a c++ vector array to contain new MCNPFmesh instance generated in this method
 * \return \c true if success
 */
bool MCNPFmeshReader::ReadMeshConfig(const string McnpInputFileName, vector<MCNPFmesh> & MeshtalAarray)
{
    string aLine;  // get a line
    string tmpBuffer; //put stuff in one FMESH into the Buffer
    vector<string> FmeshBuffer; // put all tmpBuffer into this array
    ifstream McnpInput;
    int i;

    McnpInput.open(McnpInputFileName.c_str());
    if(McnpInput.is_open())
        cout << "File " << McnpInputFileName << " is open.\n";
    else
    {
        cout << "Error opening " << McnpInputFileName << ".\n";
        return false;

    }
    tmpBuffer.clear();
//    getline(McnpInput,aLine);
    getLine(McnpInput,aLine);  //2015-08-31

    //##########go into the file to find Fmesh cards###########
    // then load  FMESH specifications into  strings
    while(!McnpInput.eof())
    {
//        cout<<aLine<<endl;

        CutoffComment(aLine); //cut off $ comments
        if(!isCommentLine(aLine))
        {
            if(isWithFmeshCard(aLine))
            {
                tmpBuffer.append(aLine);
			//    getline(McnpInput,aLine);
				getLine(McnpInput,aLine);  //2015-08-31

                //go into the a Fmesh card to fetch continuing line
                while(!McnpInput.eof())
                {
//                    cout<<aLine<<endl;

                    CutoffComment(aLine);
                    if(!isCommentLine(aLine))
                    {
                        if(isFirst5Space(aLine))  //continuing line if first 5 columns is space, append into Buffer and continue searching
                        {
                            tmpBuffer.append(aLine);
						//    getline(McnpInput,aLine);
							getLine(McnpInput,aLine);  //2015-08-31
                        }
                        else
                        {
                            FmeshBuffer.push_back(tmpBuffer);  //push Buffer into FmeshBuffer array, first Fmesh finished
                            tmpBuffer.clear();
                            break;
                        }
                    }
                    else
                    {
                        //    getline(McnpInput,aLine);
						getLine(McnpInput,aLine);  //2015-08-31
                        continue;
                    }
                }
            }
            else
            {
                //    getline(McnpInput,aLine);
				getLine(McnpInput,aLine);  //2015-08-31
                continue;
            }
        }
        else
        {
		//    getline(McnpInput,aLine);
			getLine(McnpInput,aLine);  //2015-08-31
            continue;
        }
    }

    //deal with the last line
    //see if the last line belongs to the last Fmesh card
    if (!aLine.empty() &&
            !isCommentLine(aLine)&&
            !isWithFmeshCard(aLine) &&  //should not be a new Fmesh card
            isFirst5Space(aLine)  &&
            (aLine.find("geom") >= 0 ||
             aLine.find("origin") >= 0 ||
             aLine.find("axs") >= 0 ||
             aLine.find("vec") >= 0 ||
             aLine.find("emesh") >= 0 ||
             aLine.find("eints") >= 0 ||
             aLine.find("factor") >= 0 ||
             aLine.find("out") >= 0)
            )
    {
        if (!tmpBuffer.empty())
            tmpBuffer.append(aLine);
    }
//        cout << "Warning## MCNPFmeshReader::ReadMeshConfig: Last line is highly recommended to be empty line!"<<endl;

    if (!tmpBuffer.empty())
        FmeshBuffer.push_back(tmpBuffer);  //incase lost the last one
    tmpBuffer.clear();

    //if last line is a Fmesh card, push back also
    if (!aLine.empty() &&
            !isCommentLine(aLine)&&
            isWithFmeshCard(aLine) //should not be a new Fmesh card
            )
        FmeshBuffer.push_back(aLine);

    //if no FMESH specification
    if (FmeshBuffer.size() == 0)
    {
        cout << "Warning## MCNPFmeshReader::ReadMeshConfig: No Fmesh card found in MCNP input !!" <<endl;
        return false;
    }

//    //test
//    cout << "Fmesh info :"<< endl<<endl;
//    for (i=0; i<FmeshBuffer.size(); i++)
//        cout << FmeshBuffer[i]<<endl;


    //replace "=" with " "(space),
    string space = " ";
    for (i=0; i<FmeshBuffer.size(); i++)
    {
        int tmpPos;
        tmpPos = FmeshBuffer[i].find('=');
        while(tmpPos >= 0)
        {
            FmeshBuffer[i].replace(tmpPos, 1, space);
            tmpPos = FmeshBuffer[i].find('=');
        }
    }

//    //test
//    cout << "Fmesh info :"<< endl<<endl;
//    for (i=0; i<FmeshBuffer.size(); i++)
//        cout << FmeshBuffer[i]<<endl;

    //#####star to read parameters from the FMESH specification####


    for (i=0; i<FmeshBuffer.size(); i++)
    {
        MCNPFmesh meshtal;
        stringstream tmpsstr (stringstream::in | stringstream::out);

        //switch to lower case, easier for comparison
        transform(FmeshBuffer[i].begin(), FmeshBuffer[i].end(), FmeshBuffer[i].begin(),::tolower);

        //obtaining the tally number and tally  particle
        int tmpPos1 = FmeshBuffer[i].find("fmesh");
        int tmpPos2 = FmeshBuffer[i].find(":");
        tmpsstr << FmeshBuffer[i].substr( tmpPos1+5, tmpPos2 - tmpPos1);
        tmpsstr >> meshtal.TallyNum ;

        if (FmeshBuffer[i][tmpPos2+1] == 'n')
        {
            meshtal.TallyParticle = 1;
        }
        else if (FmeshBuffer[i][tmpPos2+1] == 'p')
        {
            meshtal.TallyParticle = 2;
            meshtal.isEnergyMesh = true; //it is import that, photon tally is by default having a energy bin, which will affect output format
        }
        else
        {
             cout << "Warning## MCNPFmeshReader::ReadMeshConfig: Tally particle is missing in FMESH" << meshtal.TallyNum << endl;
             return false;
        }
        tmpsstr.str("");
        tmpsstr.clear();

        //put string into sstream
        tmpsstr << FmeshBuffer[i];

        //
        while(!tmpsstr.eof())
        {
            tmpsstr >> tmpBuffer ;// load one "word" for tmpBuffer, tmpBuffer is reused

            if (tmpBuffer.compare("geom") == 0)
            {
                tmpsstr >> tmpBuffer ;
                if(tmpBuffer.compare("xyz") == 0)
                {
                    meshtal.Coordinate = 1;
                }
                else if (tmpBuffer.compare("rec") == 0)
                {
                    meshtal.Coordinate = 1;
                }
                else if (tmpBuffer.compare("rzt") == 0)
                {
                    meshtal.Coordinate = 2;
                }
                else if (tmpBuffer.compare("cyl") == 0)
                {
                    meshtal.Coordinate = 2;
                }
                else
                {
                    cout << "Warning## MCNPFmeshReader::ReadMeshConfig: Geometry coordinate is missing!!";
                    return false;
                }

            }
            else if (tmpBuffer.compare("origin") == 0)
            {
//                cout <<tmpsstr <<endl;  //tmpadd
                tmpsstr >> meshtal.Origin[0] >> meshtal.Origin[1] >> meshtal.Origin[2];
//                cout <<meshtal.Origin[0] <<  " "<<meshtal.Origin[1] <<" "<<meshtal.Origin[2];//tmpadd
            }
            else if (tmpBuffer.compare("axs") == 0)
            {
                tmpsstr >> meshtal.Axs[0] >> meshtal.Axs[1] >> meshtal.Axs[2];
            }
            else if (tmpBuffer.compare("vec") == 0)
            {
                tmpsstr >> meshtal.Vec[0] >> meshtal.Vec[1] >> meshtal.Vec[2];
            }
            else if (tmpBuffer.compare("emesh") == 0)
            {
                meshtal.isEnergyMesh = true;
            }
            else if (tmpBuffer.compare("eints") == 0)
            {
                meshtal.isEnergyMesh = true;
            }
            else if (tmpBuffer.compare("factor") == 0)
            {
                cout << "Comment## MCNPFmeshReader::ReadMeshConfig:FACTOR is using in FMESH card" << meshtal.TallyNum << endl;
            }
            else if (tmpBuffer.compare("out") == 0)
            {
                tmpsstr >> tmpBuffer ;

                if(tmpBuffer.compare("ij") == 0)
                {
                    meshtal.OutputFormat = 2;
                }
                else if (tmpBuffer.compare("ik") == 0)
                {
                    meshtal.OutputFormat = 3;
                }
                else if (tmpBuffer.compare("jk") == 0)
                {
                    meshtal.OutputFormat = 4;
                }
                else
                {
                    cout << "Warning## MCNPFmeshReader::ReadMeshConfig: Output format is missing!!";
                    return false;
                }
            }
            else if (tmpBuffer.compare("tr") == 0)
            {
                cout << "Comment## MCNPFmeshReader::ReadMeshConfig:TR is using in FMESH card" << meshtal.TallyNum << endl;
            }
            else
            {
//                cout << "Dumping -" << tmpBuffer << endl;
            }

        }


        //push a mesh tally into the Fmesharray
        MeshtalAarray.push_back(meshtal);


    }

    return true;

}

/*!
 * \brief Read meshtal file to get meshtal results
 * \param MeshtalFileName
 * \param MeshtalAarray  array to store results
 * \return \c true if success
 */
bool MCNPFmeshReader::ReadMeshtal (const string MeshtalFileName, vector<MCNPFmesh> & MeshtalAarray)
{

    ifstream MeshtalFile;
    int i;  //iterator
    long long int NHistory;
	m_LineIndex = 0;


    MeshtalFile.open(MeshtalFileName.c_str());

    if(!MeshtalFile.is_open())
    {
        cout << "Error opening " << MeshtalFileName <<"!!" << endl;
        return false;
    }
    else if (MeshtalFile.peek() == EOF)
    {
        cout << "File : " << MeshtalFileName<< " is empty!!" << endl;
        return false;
    }
    else
    {
        cout << "File " << MeshtalFileName << " is open."<< endl;
    }

    //read meshtal file head, get neutron history data
    if (! ReadMeshtalFileHead(MeshtalFile, NHistory)) {
		cout <<"#ERROR in File\t"<<MeshtalFileName<<"\t Line \t"<<m_LineIndex<<endl;
        return false;
	}
    for (int i=0; i<MeshtalAarray.size(); i++)
        MeshtalAarray[i].NHistory = NHistory;

    //########### Reading Mesh tally result ############
    //###########        Attention          ############
    // 1. if the mcnp input file is used to construct the
    //    mesh tally, it should exactly match  with the
    //    meshtal file;
    // 2.

    for (i=0; i<MeshtalAarray.size(); i++)
    {
        cout << "Reading mesh Fmesh" << MeshtalAarray[i].TallyNum<< "......" ;
        // read mesh tally head for coordinates
        ReadMeshtalHead(MeshtalFile, MeshtalAarray[i]);

        if (MeshtalAarray[i].OutputFormat == 1)
        {
//			cout << "reading meshtal in default format"<<endl; //2015-09-01 for testing 
            if (! ReadDefaultMeshtal(MeshtalFile, MeshtalAarray[i])) {
				cout <<"#ERROR in File\t"<<MeshtalFileName<<"\t Line \t"<<m_LineIndex<<endl;
				return false;
			}
        }
        else if (MeshtalAarray[i].OutputFormat == 2 || MeshtalAarray[i].OutputFormat == 3 || MeshtalAarray[i].OutputFormat == 4)
        {
//			cout << "reading meshtal in matrix format"<<endl; //2015-09-01 for testing 
            if (! ReadMatrixMeshtal(MeshtalFile, MeshtalAarray[i])){
				cout <<"#ERROR in File\t"<<MeshtalFileName<<"\t Line \t"<<m_LineIndex<<endl;
				return false;
			}
        }
        else
		{
			cout <<"#ERROR in File\t"<<MeshtalFileName<<"\t Line \t"<<m_LineIndex<<endl;
			return false;
		}

        cout <<"done." <<endl;
    }

    cout <<endl<< "*************Success reading meshtal file!**************"<<endl;

    return true;
}



/*!
 * \brief this function intent to make Axs, Vec, and Ort to be perpendicular to each other,
 * then form a new right-handed Cartesian coordinate
 * \param meshtal Fmesh to be processed
 */
void MCNPFmeshReader::Orthogonalize(MCNPFmesh &meshtal)
{
    cout << "Orthogonalizing Fmesh" <<meshtal.TallyNum <<"......";
    double modulus; //mode of vector

    //the method used in here is :
    //first, Axs cross product with Vec to get Ort;
    //second, Ort cross product with Axs to change Vec.

    //cross product equation:
    //    vector.x = Ay*Bz - By*Az;
    //    vector.y = Bx*Az - Ax*Bz;
    //    vector.z = Ax*By - Ay*Bx;

    // Ort = Axs x Vec
    meshtal.Ort [0] = meshtal.Axs [1]*meshtal.Vec [2] - meshtal.Vec [1]*meshtal.Axs [2];
    meshtal.Ort [1] = meshtal.Vec [0]*meshtal.Axs [2] - meshtal.Axs [0]*meshtal.Vec [2];
    meshtal.Ort [2] = meshtal.Axs [0]*meshtal.Vec [1] - meshtal.Vec [0]*meshtal.Axs [1];

    // new Vec = Ort x Axs// important!! not Axs x Ort
    meshtal.Vec [0] = meshtal.Ort [1]*meshtal.Axs [2] - meshtal.Axs [1]*meshtal.Ort [2];
    meshtal.Vec [1] = meshtal.Axs [0]*meshtal.Ort [2] - meshtal.Ort [0]*meshtal.Axs [2];
    meshtal.Vec [2] = meshtal.Ort [0]*meshtal.Axs [1] - meshtal.Axs [0]*meshtal.Ort [1];

    //####### Nornalize Axs, Vec and Ort######
    //for easy calculating the transformation
    //equations:  (ex,ey,ez) = (x,y,z)/sqrt(x^2+y^2+z^2)

    modulus = sqrt(meshtal.Axs [0]*meshtal.Axs [0] + meshtal.Axs [1]*meshtal.Axs [1] + meshtal.Axs [2]*meshtal.Axs [2] );
    meshtal.Axs [0] = meshtal.Axs [0] / modulus;
    meshtal.Axs [1] = meshtal.Axs [1] / modulus;
    meshtal.Axs [2] = meshtal.Axs [2] / modulus;

    modulus = sqrt(meshtal.Vec [0]*meshtal.Vec [0] + meshtal.Vec [1]*meshtal.Vec [1] + meshtal.Vec [2]*meshtal.Vec [2] );
    meshtal.Vec [0] = meshtal.Vec [0] / modulus;
    meshtal.Vec [1] = meshtal.Vec [1] / modulus;
    meshtal.Vec [2] = meshtal.Vec [2] / modulus;

    modulus = sqrt(meshtal.Ort [0]*meshtal.Ort [0] + meshtal.Ort [1]*meshtal.Ort [1] + meshtal.Ort [2]*meshtal.Ort [2] );
    meshtal.Ort [0] = meshtal.Ort [0] / modulus;
    meshtal.Ort [1] = meshtal.Ort [1] / modulus;
    meshtal.Ort [2] = meshtal.Ort [2] / modulus;
    cout << "done." <<endl;
}


//

/*!
 * \brief cut the mesh rind is if neccessary, especially in Cylindrical coordinate
 * the theta should end at 1 ROUND in MCNP5, which will bring unuseful mesh at last
 * and also because R start at 0.0, the first R mesh will also uneccessary;
 * this will be automatically cut when first or last mesh interval is much larger than second or last-1 ones
 * \param meshtal
 * \return
 */
bool MCNPFmeshReader::CutMeshRind(MCNPFmesh &meshtal, double ratio)
{
    int firstI = 0, lastI = 0;
    int firstJ = 0, lastJ = 0;
    int firstK = 0, lastK = 0;
    int i, j, k;
//    const double ratio = 10.0;  //if the mesh interval ratio larger than "ratio", these mesh rind will be cut

    //see if rind is neccessary cut or not
    if (meshtal.Coordinate == 1)  //Cartesian coordinate are often not neccessary to do this
    {
        cout << "Warning## MCNPFmeshReader::CutMeshRind: Cutting rinds in Cartesian coordinate !!" << endl;
    }

    if (meshtal.CoordinateI.size() < 3 || meshtal.CoordinateJ.size() < 3 || meshtal.CoordinateK.size() < 3  )
    {
        cout << "Error## MCNPFmeshReader::CutMeshRind: Mesh size too small !!" << endl;
        return false;
    }

    if (((meshtal.CoordinateI[1]-meshtal.CoordinateI[0]) / (meshtal.CoordinateI[2]-meshtal.CoordinateI[1])) > ratio)
        firstI = 1;
    if (((meshtal.CoordinateI[meshtal.CoordinateI.size()-1]-meshtal.CoordinateI[meshtal.CoordinateI.size()-2]) /
         (meshtal.CoordinateI[meshtal.CoordinateI.size()-2]-meshtal.CoordinateI[meshtal.CoordinateI.size()-3])) > ratio)
        lastI = 1;

    if (((meshtal.CoordinateJ[1]-meshtal.CoordinateJ[0]) / (meshtal.CoordinateJ[2]-meshtal.CoordinateJ[1])) > ratio)
        firstJ = 1;
    if (((meshtal.CoordinateJ[meshtal.CoordinateJ.size()-1]-meshtal.CoordinateJ[meshtal.CoordinateJ.size()-2]) /
         (meshtal.CoordinateJ[meshtal.CoordinateJ.size()-2]-meshtal.CoordinateJ[meshtal.CoordinateJ.size()-3])) > ratio)
        lastJ = 1;

    if (((meshtal.CoordinateK[1]-meshtal.CoordinateK[0]) / (meshtal.CoordinateK[2]-meshtal.CoordinateK[1])) > ratio)
        firstK = 1;
    if (((meshtal.CoordinateK[meshtal.CoordinateK.size()-1]-meshtal.CoordinateK[meshtal.CoordinateK.size()-2]) /
         (meshtal.CoordinateK[meshtal.CoordinateK.size()-2]-meshtal.CoordinateK[meshtal.CoordinateK.size()-3])) > ratio)
        lastK = 1;

    //if no cutting needed, return
    if (firstI==0 && lastI==0 && firstJ==0 && lastJ==0 && firstK==0 && lastK==0 )
    {
        cout << "Warning## MCNPFmeshReader::CutMeshRind: No rind needs to be cut !!" << endl;
        return true;
    }

    //else cut the rind by moving elements to another MCNPFmesh
    MCNPFmesh tmp_meshtal;
    CopyMeshtalParas(meshtal,tmp_meshtal);  //copy basic parameter to tmp_meshtal

    //copy coordinates
    for (i=firstI; i<meshtal.CoordinateI.size()-lastI; i++)
        tmp_meshtal.CoordinateI.push_back(meshtal.CoordinateI[i]);

    for (j=firstJ; j<meshtal.CoordinateJ.size()-lastJ; j++)
        tmp_meshtal.CoordinateJ.push_back(meshtal.CoordinateJ[j]);

    for (k=firstK; k<meshtal.CoordinateK.size()-lastK; k++)
        tmp_meshtal.CoordinateK.push_back(meshtal.CoordinateK[k]);

    int tmpNumI = tmp_meshtal.CoordinateI.size()-1;
    int tmpNumJ = tmp_meshtal.CoordinateJ.size()-1;
    int tmpNumK = tmp_meshtal.CoordinateK.size()-1;
    int NumI = meshtal.CoordinateI.size()-1;
    int NumJ = meshtal.CoordinateJ.size()-1;
    int NumK = meshtal.CoordinateK.size()-1;

    //copy mesh tally and error data
    tmp_meshtal.TallyArray.assign(tmpNumI * tmpNumJ * tmpNumK, 0);  //initialize tally array
    tmp_meshtal.ErrorArray.assign(tmpNumI * tmpNumJ * tmpNumK, 1);  //initialize error array

    for (k=firstK; k<NumK-lastK; k++)
    {
        for (j=firstJ; j<NumJ-lastJ; j++)
        {
            for (i=firstI; i<NumI-lastI; i++)
            {
                tmp_meshtal.changeOneTallyResult(i-firstI, j-firstJ, k-firstK, meshtal.getOneTallyResult(i, j, k));
                tmp_meshtal.changeOneErrorResult(i-firstI, j-firstJ, k-firstK, meshtal.getOneErrorResult(i, j, k));
            }
        }
    }
    meshtal = tmp_meshtal; //done!
    return true;
}


// see if it is comment line
bool MCNPFmeshReader::isCommentLine(string& line)
{
    char a = line[0];
    char b = line[1];

    if ((a == 'c'||a == 'C') && b == ' ') return true;
    else return false;
}

// cut off the comment behind "$", line will be modified if $ is found
void MCNPFmeshReader::CutoffComment(string& line)
{
    int n = line.find('$');
    if (n > 0)
    {
        //        cout<< "found $ in position :"<<n<<endl;
        string tmpstr(line,0, n);
        line = tmpstr;
    }
}

// see if the first five columns is space, means a continous line
bool MCNPFmeshReader::isFirst5Space(string& line)
{
    string tmpstr(line,0,5);
    if (tmpstr.compare("     ") == 0) return true;
    else return false;
}

// see if includes FMESH card
bool MCNPFmeshReader::isWithFmeshCard (string& line)
{
    transform(line.begin(), line.end(), line.begin(),::tolower);  //transform string to lower case
    int pos = line.find("fmesh");
    //first 10 charactor is enough for FMESH card judgement
    if (pos >= 0 && pos < 5) return true;  //only if "fmesh" is found within first five columns is valid.
    else return false;
}



/*!
 * \brief read head of meshtal file, obtaining neutron history data
 * \param MeshtalFile Mesh tally file stream
 * \param NHistory return the neutron histroy
 * \return
 */
bool MCNPFmeshReader::ReadMeshtalFileHead (ifstream &MeshtalFile,long long int &NHistory )
{
    string aLine;
    string dump;
    stringstream tmpsstr;

    //getline(MeshtalFile, aLine);  //codename, version , time
    //getline(MeshtalFile, aLine);  // title
    //getline(MeshtalFile, aLine);  // neutron history
	getLine(MeshtalFile, aLine);  //codename, version , time
    getLine(MeshtalFile, aLine);  // title
    getLine(MeshtalFile, aLine);  // neutron history

    tmpsstr << aLine;
    while (!tmpsstr.eof())
    {
        tmpsstr >> dump;
    }
#ifdef WIN32
	NHistory = atol(dump.c_str()); //string to long long int
#else
    NHistory = atoll(dump.c_str()); //string to long long int
#endif


    if (NHistory <= 0)
    {
        cout << "Error## MCNPFmeshReader::ReadMeshtalHead: Neutron history data is missing !!" << endl;
        return false;
    }
    return true;
}


/*!
 * \brief read head of each mesh tally, obtaining the coordinates
 * \param MeshtalFile Mesh tally file stream
 * \param meshtal the mesh tally where data to be store
 * \return \c true if not error
 */
bool MCNPFmeshReader::ReadMeshtalHead(ifstream &MeshtalFile, MCNPFmesh &meshtal)
{
    string aLine;  // get a line
    string dump;
    stringstream tmpsstr;
    int tmpInt;
    int tmpPos;
    double tmpReal;

    while (aLine.find("Mesh Tally Number") == -1)  //cannot find
    {
        //getline (MeshtalFile,aLine);
        getLine (MeshtalFile,aLine);
        if (MeshtalFile.eof())
            return false;
    }

    //see if the mesh tally number is the same
    tmpsstr << aLine ;
    while (!tmpsstr.eof())
    {
        tmpsstr >> dump;
    }
    tmpInt = atoi(dump.c_str());
    if (tmpInt != meshtal.TallyNum)
    {
        cout << "Error## MCNPFmeshReader::ReadMeshtalHead: Mesh tally Number doesn't match !!" << endl;
        return false;
    }

    //jump to beginning of the coordinates
    while (aLine.find("Tally bin boundaries") == -1)
    {
        //getline (MeshtalFile,aLine);
		getLine (MeshtalFile,aLine);
        if (MeshtalFile.eof())
            return false;
    }

    //if cylinderical coordinates, jump one line more
    if (meshtal.Coordinate == 2)
        //getline (MeshtalFile,aLine);
		getLine (MeshtalFile,aLine);

    //#######  Start to read the coordinates#######
    tmpsstr.str("");
    tmpsstr.clear();
    //I coordinate
        //getline (MeshtalFile,aLine);
		getLine (MeshtalFile,aLine);
    tmpPos = aLine.find(':') + 1;
    //modified against a bug: lose last digit of a line
//    tmpsstr << aLine.substr(tmpPos, aLine.size()-tmpPos-1);
    tmpsstr << aLine.substr(tmpPos, aLine.size()-tmpPos);
    while (!tmpsstr.eof())
    {
        tmpsstr >> tmpReal;
        meshtal.CoordinateI.push_back(tmpReal);
    }
    tmpsstr.str("");
    tmpsstr.clear();

    //J coordinate
        //getline (MeshtalFile,aLine);
		getLine (MeshtalFile,aLine);
    tmpPos = aLine.find(':') + 1;
    tmpsstr << aLine.substr(tmpPos, aLine.size()-tmpPos);
    while (!tmpsstr.eof())
    {
        tmpsstr >> tmpReal;
        meshtal.CoordinateJ.push_back(tmpReal);
    }
    tmpsstr.str("");
    tmpsstr.clear();

    //K coordinate
        //getline (MeshtalFile,aLine);
		getLine (MeshtalFile,aLine);
    tmpPos = aLine.find(':') + 1;
    tmpsstr << aLine.substr(tmpPos, aLine.size()-tmpPos);
    while (!tmpsstr.eof())
    {
        tmpsstr >> tmpReal;
        meshtal.CoordinateK.push_back(tmpReal);
    }
    tmpsstr.str("");
    tmpsstr.clear();

    //get number of energy bins when energy tally is enable
    if (meshtal.isEnergyMesh == true)
    {
        //getline (MeshtalFile,aLine);
		getLine (MeshtalFile,aLine);
        tmpPos = aLine.find(':') + 1;
        tmpsstr << aLine.substr(tmpPos, aLine.size()-tmpPos);
        while (!tmpsstr.eof())
        {
            tmpsstr >> dump;
            meshtal.NumE ++;
        }
        meshtal.NumE --; //energy bin size = energy boundary size - 1
    }

    if (MeshtalFile.eof())
        return false; //should not be end of file

    return true;
}

//read mesh tally data with default format
bool MCNPFmeshReader::ReadDefaultMeshtal(ifstream &MeshtalFile, MCNPFmesh &meshtal)
{
    string aLine;  // get a line
    long long int i;
    long long int TotalLine;
    long long int SkipLine;

    int NumI = meshtal.CoordinateI.size() -1;
    int NumJ = meshtal.CoordinateJ.size() -1;
    int NumK = meshtal.CoordinateK.size() -1;

    //jump to the beginning of tally result
    while (aLine.find("Result") == -1)
    {
        //getline (MeshtalFile,aLine);
		getLine (MeshtalFile,aLine);
        if (MeshtalFile.eof())
            return false;
    }

    //#########  Start reading tally results  #############
    // Attention: this section will be execute thousands fold, keep it as simple as possible
    // rev:2012-06-05: there is posibilities energy column appeared without giving energy bin,
    // thus here judge by output information.

    if (meshtal.isEnergyMesh == false && aLine.find("Energy") == -1) //without energy bin, and cannot find Energy column
    {
        TotalLine =NumI * NumJ * NumK;
        for (i=0; i<TotalLine; i++ )
        {
        //getline (MeshtalFile,aLine);
		getLine (MeshtalFile,aLine);
            if (MeshtalFile.eof())
            {
                cout << "Error## MCNPFmeshReader::ReadDefaultMeshtal: Mesh tally result file is interrupted!!" << endl;
                return false;
            }
            else if (aLine.size() < 55)  // if read an imcomplete line
            {
                cout << "Error## MCNPFmeshReader::ReadDefaultMeshtal: Mesh tally result file with an imcomplete line : " << aLine << endl;
                return false;
            }
            else
            {
                meshtal.TallyArray.push_back(atof(aLine.substr(32, 11).c_str()));  //position for tally result
                meshtal.ErrorArray.push_back(atof(aLine.substr(44, 11).c_str()));  //position for error result
            }
        }
    }
    else if ((meshtal.isEnergyMesh == false && aLine.find("Energy") != -1)  //without energy bin, but find Energy column
             || (meshtal.isEnergyMesh == true && meshtal.NumE == 1)) //with one energy bin
    {
        TotalLine = NumI * NumJ * NumK;
        for (i=0; i<TotalLine; i++ )
        {
        //getline (MeshtalFile,aLine);
			getLine (MeshtalFile,aLine);
            if (MeshtalFile.eof())
            {
                cout << "Error## MCNPFmeshReader::ReadDefaultMeshtal: Mesh tally result file is interrupted!!" << endl;
                return false;
            }
            else if (aLine.size() < 65)  // if read an imcomplete line
            {
                cout << "Error## MCNPFmeshReader::ReadDefaultMeshtal: Mesh tally result file with read an imcomplete line: " << aLine << endl;
                return false;
            }
            else
            {
                meshtal.TallyArray.push_back(atof(aLine.substr(42, 11).c_str()));  //position for tally result
                meshtal.ErrorArray.push_back(atof(aLine.substr(54, 11).c_str()));  //position for error result
            }

        }

    }
    else //with more than one energy bin, find the word "total" then read
    {
        TotalLine = NumI * NumJ * NumK;
        SkipLine = TotalLine * meshtal.NumE ;

        //skip lines before the word "total"
        for (i=0; i<SkipLine; i++)
        {
        //getline (MeshtalFile,aLine);
			getLine (MeshtalFile,aLine);
            if (MeshtalFile.eof())
            {
                cout << "Error## MCNPFmeshReader::ReadDefaultMeshtal: Mesh tally result file is interrupted!!" << endl;
                return false;
            }
        }

        //read results
        for (i=0; i<TotalLine; i++ )
        {
        //getline (MeshtalFile,aLine);
			getLine (MeshtalFile,aLine);
            if (MeshtalFile.eof())
            {
                cout << "Error## MCNPFmeshReader::ReadDefaultMeshtal: Mesh tally result file is interrupted!!" << endl;
                return false;
            }
            else if (aLine.size() < 65)  // if read an imcomplete line
            {
                cout << "Error## MCNPFmeshReader::ReadDefaultMeshtal: Mesh tally result file with read an imcomplete line!!" << endl;
                return false;
            }
            else
            {
                meshtal.TallyArray.push_back(atof(aLine.substr(42, 11).c_str()));  //position for tally result
                meshtal.ErrorArray.push_back(atof(aLine.substr(54, 11).c_str()));  //position for error result
            }

        }
    }
    return true;
}

//read mesh tally data with matrix pattern
bool MCNPFmeshReader::ReadMatrixMeshtal (ifstream &MeshtalFile, MCNPFmesh &meshtal)
{
    string aLine;  // get a line
//    streampos tmpFPos;
    int tmpInt = 0;
    int i;
    int NumBlock;  // number of matrix blocks, for outer iterator
    int NumLine;   // number of lines in the matrix, for inner iterator
    int NumColumn; // number of column in the matrix, for innner iterator

    int NumI = meshtal.CoordinateI.size() -1;
    int NumJ = meshtal.CoordinateJ.size() -1;
    int NumK = meshtal.CoordinateK.size() -1;

//	system("pause");
//	cout << "Reading position: "<<tmpFPos<<endl; //for testing
    //locate the first matrix
    while (aLine.find("Tally Results:") == -1)
    {
//        tmpFPos = MeshtalFile.tellg();  //get the current fstream pointer position
        //getline (MeshtalFile,aLine);
		getLine (MeshtalFile,aLine);
        if (MeshtalFile.eof())
            return false;
    }
//	cout << "Reading position2: "<<tmpFPos<<endl; //for testing

    //check if the output format is match
    if (aLine.find("X (across) by Y (down)") != -1 || aLine.find("R (across) by Z (down)") != -1)
        tmpInt = 2;
    else if (aLine.find("X (across) by Z (down)") != -1 || aLine.find("R (across) by Theta (down)") != -1)
        tmpInt = 3;
    else if (aLine.find("Y (across) by Z (down)") != -1 || aLine.find("Z (across) by Theta (down)") != -1)
        tmpInt = 4;
    else
    {
        cout << "Error## MCNPFmeshReader::ReadMatrixMeshtal: Read matrix format failed !!" << endl;
        return false;
    }

    if (tmpInt != meshtal.OutputFormat)
    {
        cout << "Error## MCNPFmeshReader::ReadMatrixMeshtal: Output format miss match !!" << endl;
        return false;
    }

    //#########  Start reading tally results  #############
    // Attention: this section will be execute thousands fold, keep it as simple as possible

//qiu    MeshtalFile.seekg(tmpFPos); //move back for one line
//qiu     aLine.clear();// very important because aLine has the key word "Tally Results:"
//	tmpFPos = MeshtalFile.tellg();
//	cout << "Reading position3: "<<tmpFPos<<endl; //for testing

    if (meshtal.OutputFormat == 2)
    {
        NumBlock = NumK;
        NumLine = NumJ;
        NumColumn = NumI;
    }
    else if (meshtal.OutputFormat == 3)
    {
        NumBlock = NumJ;
        NumLine = NumK;
        NumColumn = NumI;
    }
    else if (meshtal.OutputFormat == 4)
    {
        NumBlock = NumI;
        NumLine = NumK;
        NumColumn = NumJ;
    }
//	cout << "NumBlock\t"<<NumBlock<<"\tNumLine\t"<<NumLine<<"\tNumColumn\t"<<NumColumn<<endl; //2015-09-01 for testing 
    //if more than one energy bin, locate the total result
    if (meshtal.NumE > 1)
    {

        while (aLine.compare(0, 16, "Total Energy Bin") != 0)  //find the string in the first 16 position
        {
        //getline (MeshtalFile,aLine);
			getLine (MeshtalFile,aLine);
            if (MeshtalFile.eof())
                return false;
        }
    }


    for (i=0; i<NumBlock; i++)
    {
//		cout <<"Block i="<<i<<endl; //for testing
//		system("pause");
        //find Tally results
		if (i != 0 || meshtal.NumE > 1) {  //skip this searching for 
			while (aLine.find("Tally Results:") == -1)
			{
			//getline (MeshtalFile,aLine);
				getLine (MeshtalFile,aLine);
				if (MeshtalFile.eof())
					return false;
			}
		}

        //read a result block
        if (! ReadBlock(MeshtalFile, meshtal.TallyArray, NumLine, NumColumn))
            return false;
//		cout <<"Block error i="<<i<<endl; //for testing
//		system("pause");
        //find Tally relative errors
        while (aLine.find("Relative Errors") == -1)
        {
        //getline (MeshtalFile,aLine);
			getLine (MeshtalFile,aLine);
            if (MeshtalFile.eof())
                return false;
        }

        //read a relative error block
        if (! ReadBlock(MeshtalFile, meshtal.ErrorArray, NumLine, NumColumn))
            return false;
    }
    return true;
}




//read a block of matrix
bool MCNPFmeshReader::ReadBlock (ifstream &MeshtalFile, vector<double> &Array, const int NumLine, const int NumColumn)
{
    string aLine;  // get a line
    string dump;
    stringstream tmpsstr;
    double tmpReal;
    int i, j;

    //getline (MeshtalFile,aLine);
	getLine (MeshtalFile,aLine); //skip the line with coordinates
    if (MeshtalFile.eof())
    {
        cout << "Error## MCNPFmeshReader::ReadBlock: Matrix block interupted !!" << endl;
        return false;
    }

    for(i=0; i<NumLine; i++)
    {
       //getline (MeshtalFile,aLine);
		getLine (MeshtalFile,aLine);
         if (MeshtalFile.eof())
         {
             cout << "Error## MCNPFmeshReader::ReadBlock: Matrix block interupted !!" << endl;
             return false;
         }
        tmpsstr << aLine;
        tmpsstr >> dump; ////skip the column with coordinates

        for (j=0; j<NumColumn; j++)
        {
            if (tmpsstr.eof())
            {
                cout << "Error## MCNPFmeshReader::ReadBlock: Matrix line interupted !!" << endl;
                return false;
            }
            //temperory change 13-07-2012, due to the appear of NaN in mesh tally,
//            20-07-12 changed back
            tmpsstr >> tmpReal;
//            tmpsstr >> dump;
//            if (dump.find("NaN") != -1)
//            {
//                tmpReal = 0;
//            }
//            else
//            {
//                tmpReal = atof(dump.c_str());
//            }

            Array.push_back(tmpReal);
        }

        tmpsstr.str("");
        tmpsstr.clear();

    }
    return true;
}


//copy based parameter one meshtal1 to meshtal2, exclude coodinates, Tally and error results
bool MCNPFmeshReader::CopyMeshtalParas(MCNPFmesh &meshtal1, MCNPFmesh &meshtal2)
{
    meshtal2.TallyNum = meshtal1.TallyNum;
    meshtal2.TallyParticle = meshtal1.TallyParticle;
    meshtal2.Coordinate = meshtal1.Coordinate;

    meshtal2.Origin[0] = meshtal1.Origin[0];
    meshtal2.Origin[1] = meshtal1.Origin[1];
    meshtal2.Origin[2] = meshtal1.Origin[2];
    meshtal2.Axs [0] = meshtal1.Axs [0];
    meshtal2.Axs [1] = meshtal1.Axs [1];
    meshtal2.Axs [2] = meshtal1.Axs [2];
    meshtal2.Vec [0] = meshtal1.Vec [0];
    meshtal2.Vec [1] = meshtal1.Vec [1];
    meshtal2.Vec [2] = meshtal1.Vec [2];
    meshtal2.Ort [0] = meshtal1.Ort [0];
    meshtal2.Ort [1] = meshtal1.Ort [1];
    meshtal2.Ort [2] = meshtal1.Ort [2];

    meshtal2.isEnergyMesh = meshtal1.isEnergyMesh ;
    meshtal2.OutputFormat = meshtal1.OutputFormat;
    meshtal2.NHistory = meshtal1.NHistory;  //not neccessary be the same
    meshtal2.NumE = meshtal1.NumE;  //should be the same, especially in reading continual file
    return true;

}

//get a line and save to m_CurrentLine, and increase m_LineIndex;
void MCNPFmeshReader::getLine(ifstream &aFile, string & aLine)
{
	m_CurrentLine.clear();
    getline(aFile, m_CurrentLine);
	aLine = m_CurrentLine;
	m_LineIndex ++; 
//	cout <<"Line "<<m_LineIndex<<"\t"<<m_CurrentLine<<endl;  //for testing
}






