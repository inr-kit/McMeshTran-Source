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
#include "MCMESHTRAN_MCNPFmesh.hxx"
#include <iostream>
#include <math.h>

MCNPFmesh::MCNPFmesh()
{
    //construction function
    TallyNum = 4;       //tally number
    TallyParticle = 1; //1-n, 2-p, 3-e, 4-mix/averaged
    Coordinate = 1; //1-xyz, 2-rzt

    Origin[0] = 0;  //initial origin = 0 0 0
    Origin[1] = 0;
    Origin[2] = 0;
    Axs [0] = 0; //initial axis = 0 0 1
    Axs [1] = 0;
    Axs [2] = 1;
    Vec [0] = 1; //initial vector = 1 0 0
    Vec [1] = 0;
    Vec [2] = 0;
    Ort [0] = 0; //initial orthogonal vector = 0 1 0
    Ort [1] = 1;
    Ort [2] = 0;

    isEnergyMesh = false ; //is there any energy mesh
    OutputFormat = 1; //1-default, 2-ij, 3-ik, 4-jk
    NHistory = 0;  //nuetron history, using long long int to keep consistensy in 32bit/64bit system
    NumE = 0;  //size of energy bins
}

// return tally results according to the index
double MCNPFmesh::getOneTallyResult (int i, int j, int k)
{
    int NumI = CoordinateI.size() - 1;
    int NumJ = CoordinateJ.size() - 1;
    int NumK = CoordinateK.size() - 1;

    if (OutputFormat == 1)
    {
        return TallyArray[i*NumJ*NumK + j*NumK + k];
    }
    else if (OutputFormat == 2)
    {
        return TallyArray[k*NumI*NumJ + j*NumI + i];
    }
    else if (OutputFormat == 3)
    {
        return TallyArray[j*NumI*NumK + k*NumI + i];
    }
    else if (OutputFormat ==4)
    {
        return TallyArray[i*NumJ*NumK + k*NumJ + j];
    }
    else
    {
        cout<< "Error## MCNPFmesh::getOneTallyResult(): Output format not found!!"<<endl;
        return 0;
    }
}

//return relative errors according to the index
double MCNPFmesh::getOneErrorResult (int i, int j, int k)
{
    int NumI = CoordinateI.size() - 1;
    int NumJ = CoordinateJ.size() - 1;
    int NumK = CoordinateK.size() - 1;

    if (OutputFormat == 1)
    {
        return ErrorArray[i*NumJ*NumK + j*NumK + k];
    }
    else if (OutputFormat == 2)
    {
        return ErrorArray[k*NumI*NumJ + j*NumI + i];
    }
    else if (OutputFormat == 3)
    {
        return ErrorArray[j*NumI*NumK + k*NumI + i];
    }
    else if (OutputFormat ==4)
    {
        return ErrorArray[i*NumJ*NumK + k*NumJ + j];
    }
    else
    {
        cout<< "Error## MCNPFmesh::getOneErrorResult(): Output format not found!!"<<endl;
        return 0;
    }
}

//change the tally result according to the index
void MCNPFmesh::changeOneTallyResult (int i, int j, int k, const double tmpReal)
{

    int NumI = CoordinateI.size() - 1;
    int NumJ = CoordinateJ.size() - 1;
    int NumK = CoordinateK.size() - 1;

    if (OutputFormat == 1)
    {
         TallyArray[i*NumJ*NumK + j*NumK + k] = tmpReal;
    }
    else if (OutputFormat == 2)
    {
         TallyArray[k*NumI*NumJ + j*NumI + i] = tmpReal;
    }
    else if (OutputFormat == 3)
    {
         TallyArray[j*NumI*NumK + k*NumI + i] = tmpReal;
    }
    else if (OutputFormat ==4)
    {
         TallyArray[i*NumJ*NumK + k*NumJ + j] = tmpReal;
    }
    else
    {
        cout<< "Error## MCNPFmesh::changeOneTallyResult: Output format not found!!"<<endl;
    }
}

//change the tally error according to the index
void MCNPFmesh::changeOneErrorResult (int i, int j, int k, const double tmpReal)
{
    int NumI = CoordinateI.size() - 1;
    int NumJ = CoordinateJ.size() - 1;
    int NumK = CoordinateK.size() - 1;
    if (OutputFormat == 1)
    {
         ErrorArray[i*NumJ*NumK + j*NumK + k] = tmpReal;
    }
    else if (OutputFormat == 2)
    {
         ErrorArray[k*NumI*NumJ + j*NumI + i] = tmpReal;
    }
    else if (OutputFormat == 3)
    {
         ErrorArray[j*NumI*NumK + k*NumI + i] = tmpReal;
    }
    else if (OutputFormat ==4)
    {
         ErrorArray[i*NumJ*NumK + k*NumJ + j] = tmpReal;
    }
    else
    {
        cout<< "Error## MCNPFmesh::changeOneErrorResult: Output format not found!!"<<endl;
    }
}

//get a point (vertex) from the coordinates
//!!!!!!Attention: the Axs, Vec and Ort should be orthogonalized
// before get points from cylindrical tally
Point MCNPFmesh::getOnePoint(int i, int j, int k)
{
    Point tmpPoint1, tmpPoint2;

    //first get the point from original coordinates
    tmpPoint1.x = CoordinateI [i];
    tmpPoint1.y = CoordinateJ [j];
    tmpPoint1.z = CoordinateK [k];
//    cout << tmpPoint1.x << "\t " << tmpPoint1.y << "\t "<< tmpPoint1.z <<endl;


    if (Coordinate == 2)  //cylindrical coordinate
    {

        //########## cylindrical coordinate transformation##########
        //1. transform cylindrical coordinate into Cartisan coordinate formed by vector Axs and Vec;
        //2. if no movement or rotation of the orginal coordinate, then transform the Vec-Ort-Axs Cartisan
        //   coordinate into original coordinate, this including rotation and movement two steps;
        //!!this is because all rotation is carried out at the 0,0,0 point of Vec-Ort-Axs coordinate system!!

        //#########1. transform cylindrical coordinate into Cartisan coordinate, equations:
        //x=r*cos(theta)
        //y=r*sin(theta)
        //z=h
        //noted that z is store in second index in cylindrical coordinate, theta is in the unit of CYCLE
        tmpPoint2.x = tmpPoint1.x * cos(tmpPoint1.z * 2 * PI);
        tmpPoint2.y = tmpPoint1.x * sin(tmpPoint1.z * 2 * PI);
        tmpPoint2.z = tmpPoint1.y;//!!!!!!!here is important, the coordinate Z and theta switched!!!!!!
        tmpPoint1 = tmpPoint2;

        //#########2. if no movement or rotation of the orginal coordinate, then transform the Vec-Ort-Axs Cartisan
        //in Vec-Ort-Axs Cartisan coordinate, Vec is X, Ort is Y, Axs is Z


        //first rotate according to the Vec-Ort-Axs
        //because here the Vec-Ort-Axs (V-O-A) is "OLD" and original coordinate (X-Y-Z) is the "NEW" coordinate,
        //and vector X =1 0 0, Y=0 1 0, Z=0 0 1, and Vec, Ort, Axs is normalized
        //thus cos(X,V)= Vec[0]  cos(X,O)= Ort[0]  cos(X,A)= Axs[0]
        //     cos(Y,V)= Vec[1]  cos(Y,O)= Ort[1]  cos(Y,A)= Axs[1]
        //     cos(Z,V)= Vec[2]  cos(Z,O)= Ort[2]  cos(Z,A)= Axs[2]

        tmpPoint2.x = tmpPoint1.x * Vec[0] + tmpPoint1.y * Ort[0] + tmpPoint1.z * Axs[0];
        tmpPoint2.y = tmpPoint1.x * Vec[1] + tmpPoint1.y * Ort[1] + tmpPoint1.z * Axs[1];
        tmpPoint2.z = tmpPoint1.x * Vec[2] + tmpPoint1.y * Ort[2] + tmpPoint1.z * Axs[2];
        tmpPoint1 = tmpPoint2;

        //then moving according to the ORIGIN,
        //should plus with ORIGIN because original coordinate (X-Y-Z) is the "NEW" coordinate.
        tmpPoint1.x = tmpPoint1.x + Origin [0];
        tmpPoint1.y = tmpPoint1.y + Origin [1];
        tmpPoint1.z = tmpPoint1.z + Origin [2];

    }
//    cout << tmpPoint1.x << "\t " << tmpPoint1.y << "\t "<< tmpPoint1.z <<endl;
    return tmpPoint1;
}


Point::Point()
{
    x = 0.0;
    y = 0.0;
    z = 0.0;
}











