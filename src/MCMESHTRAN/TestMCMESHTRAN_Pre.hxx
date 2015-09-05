#ifndef TESTMCMESHTRAN_PRE_HXX
#define TESTMCMESHTRAN_PRE_HXX

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

//MEDCoupling
#include "MEDCouplingFieldDoubleClient.hxx"
#include "MEDCouplingMeshClient.hxx"
#include "MEDCouplingMeshServant.hxx"
#include "MEDCouplingFieldDoubleServant.hxx"
#include "MEDCouplingAutoRefCountObjectPtr.hxx"
#include "MEDCouplingMemArray.hxx"
#include "MEDCouplingUMesh.hxx"
#include "MEDCouplingRemapper.hxx"
#include "MEDLoader.hxx"
#include CORBA_SERVER_HEADER(MCMESHTRAN)
#include CORBA_CLIENT_HEADER(MEDCouplingCorbaServant)
#include "MCMESHTRAN.hxx"
#include "MCMESHTRAN_MCNPFmesh.hxx"
#include "MCMESHTRAN_MCNPFmeshReader.hxx"
#include "MCMESHTRAN_AbaqusMeshReader.hxx"
#include "MCMESHTRAN_MCNPUMeshReader.hxx"




#include <stdio.h>
#include <vector>
#include <iomanip>

using namespace std;
static int Id = 0;

class TestMCMESHTRAN_Pre : public CppUnit::TestFixture
{

public:
    static ParaMEDMEM::MEDCouplingUMesh *build3DSourceMesh_1();
    static ParaMEDMEM::MEDCouplingFieldDouble *build3Dfield_1();
    static SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr buildFieldIOR();
    static MCMESHTRAN_ORB::Mesh_ptr  genNewMesh();
    static MCNPFmesh * genMFmeshXYZ();
    static MCMESHTRAN genEngine();




public:
    static int genId();


};

#endif // TESTMCMESHTRAN_PRE_HXX
