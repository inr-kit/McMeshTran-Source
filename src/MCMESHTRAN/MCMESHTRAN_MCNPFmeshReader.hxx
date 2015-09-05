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
#ifndef MCMESHTRAN_MCNPFMESHREADER_H
#define MCMESHTRAN_MCNPFMESHREADER_H
#include "MCMESHTRAN_MCNPFmesh.hxx"
#include <string>
#include <fstream>

using namespace std;

class MCNPFmeshReader
{
public:
    MCNPFmeshReader();


    // read MCNP input file and construct new meshtallies
    bool ReadMeshConfig(const string McnpInputFileName, vector<MCNPFmesh> & MeshtalAarray);

    //Read meshtal file to get meshtal results
    bool ReadMeshtal (const string MeshtalFileName, vector<MCNPFmesh> & MeshtalAarray);

    //this function intent to make Axs, Vec, and Ort to be perpendicular to each other,
    //then form a new right-handed Cartisan coordinate
    void Orthogonalize(MCNPFmesh &meshtal);

    //cut the mesh rind is if neccessary, especially in Cylindrical coordinate
    //the theta should end at 1 ROUND in MCNP5, which will bring unuseful mesh at last;
    //and also because R start at 0.0, the first R mesh will also uneccessary;
    //this will be automatically cut when first or last mesh interval is much larger than second or last-1 ones
    //if the mesh interval ratio larger than "ratio", these mesh rind will be cut, usually =10
    bool CutMeshRind(MCNPFmesh &meshtal, double ratio = 10.0 );


private:


    // see if it is comment line
    bool isCommentLine(string& line);
    // cut off the comment behind "$"
    void CutoffComment(string& line);
    // see if the first five columns is space
    bool isFirst5Space(string& line);
    // see if includes FMESH card
    bool isWithFmeshCard (string& line);

    //read head of meshtal file, obtaining neutron history data
    bool ReadMeshtalFileHead (ifstream &MeshtalFile, long long int &NHistory);

    //read head of a mesh tally, obtaining the coordinates
    bool ReadMeshtalHead(ifstream &MeshtalFile, MCNPFmesh &meshtal);

    //read mesh tally data with default format
    bool ReadDefaultMeshtal(ifstream &MeshtalFile, MCNPFmesh &meshtal);

    //read mesh tally data with matrix pattern
    bool ReadMatrixMeshtal (ifstream &MeshtalFile, MCNPFmesh &meshtal);

    //read a block of matrix
    bool ReadBlock (ifstream &MeshtalFile, vector<double> &Array, const int NumLine, const int NumColumn);

    //copy based parameter one meshtal1 to meshtal2, exclude coodinates, Tally and error results
    bool CopyMeshtalParas(MCNPFmesh &meshtal1, MCNPFmesh &meshtal2);

	//get a line and save to m_CurrentLine, and increase m_LineIndex;
	void getLine(ifstream &aFile, string & aLine); 

private:
    ifstream	m_MCNPInput; 
	ifstream	m_MeshtalFile; 
	string		m_CurrentLine;
	int			m_LineIndex;


};

#endif // MCNPFmeshReader_H
