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

#ifndef MCMESHTRAN_MCNPFMESH_H
#define MCMESHTRAN_MCNPFMESH_H
#include <vector>

using namespace std;

const double PI = 3.141592653589793;
const double Null = 4.44444444444e34;

class Point;
class MCNPFmesh
{
public:
    int TallyNum;       //tally number
    int TallyParticle; //1-n, 2-p, 3-e, 4-mix/averaged
    int Coordinate; //1-xyz, 2-rzt
    double Origin [3]; // origin
    double Axs [3]; //axis for cylinderical coordinate
    double Vec [3];  //vector for plane which Theta=0, for cylinderical coordinate
    double Ort [3];  //vector which is perpendicular to Axs and Vec, for cylindrical coordinate transformation
    bool isEnergyMesh; //is there any energy mesh
    int OutputFormat; //1-default, 2-ij, 3-ik, 4-jk
    long long int NHistory;  //nuetron history, using long long int to keep consistensy in 32bit/64bit system
    int NumE;  //size of energy bins
    vector< double > CoordinateI;  //vector for coordinate in x/r
    vector< double > CoordinateJ;  //vector for coordinate in y/z
    vector< double > CoordinateK;  //vector for coordinate in z/theta
    vector< double > TallyArray;   //array for tally results
    vector< double > ErrorArray;   //array for relative errors


public:
    MCNPFmesh();

    // return tally results according to the index
    double getOneTallyResult (int i, int j, int k);

    //return relative errors according to the index
    double getOneErrorResult (int i, int j, int k);

    //change the tally result according to the index
    void changeOneTallyResult (int i, int j, int k, const double tmpReal);

    //change the tally error according to the index
    void changeOneErrorResult (int i, int j, int k, const double tmpReal);

    //get a point (vertex) from the coordinates
    //!!!!!!Attention: the Axs, Vec and Ort should be orthogonalized
    // before get points from cylindrical tally
    Point getOnePoint(int i, int j, int k);


};


class Point
{
public:
    double x;
    double y;
    double z;
public:
    Point();
};

#endif // MCMESHTRAN_MCNPFMESH_H
