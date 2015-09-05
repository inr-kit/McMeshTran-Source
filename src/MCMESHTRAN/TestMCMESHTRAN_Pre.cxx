#include "TestMCMESHTRAN_Pre.hxx"

ParaMEDMEM::MEDCouplingUMesh *TestMCMESHTRAN_Pre::build3DSourceMesh_1()
{
  double sourceCoords[27]={ 0.0, 0.0, 200.0, 0.0, 0.0, 0.0, 0.0, 200.0, 200.0, 0.0, 200.0, 0.0, 200.0, 0.0, 200.0,
                            200.0, 0.0, 0.0, 200.0, 200.0, 200.0, 200.0, 200.0, 0.0, 100.0, 100.0, 100.0 };
  int sourceConn[48]={8,1,7,3, 6,0,8,2, 7,4,5,8, 6,8,4,7, 6,8,0,4, 6,8,7,3, 8,1,3,0, 4,1,5,8, 1,7,5,8, 0,3,8,2, 8,1,0,4, 3,6,8,2};
  ParaMEDMEM::MEDCouplingUMesh *sourceMesh=ParaMEDMEM::MEDCouplingUMesh::New();
  sourceMesh->setMeshDimension(3);
  sourceMesh->allocateCells(12);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+4);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+8);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+12);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+16);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+20);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+24);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+28);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+32);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+36);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+40);
  sourceMesh->insertNextCell(INTERP_KERNEL::NORM_TETRA4,4,sourceConn+44);
  sourceMesh->finishInsertingCells();
  ParaMEDMEM::DataArrayDouble *myCoords=ParaMEDMEM::DataArrayDouble::New();
  myCoords->alloc(9,3);
  std::copy(sourceCoords,sourceCoords+27,myCoords->getPointer());
  sourceMesh->setCoords(myCoords);
  myCoords->decrRef();
  return sourceMesh;
}

 ParaMEDMEM::MEDCouplingFieldDouble *TestMCMESHTRAN_Pre::build3Dfield_1()
{
    ParaMEDMEM::MEDCouplingUMesh * aUmesh = build3DSourceMesh_1();
    ParaMEDMEM::MEDCouplingFieldDouble * aField = ParaMEDMEM::MEDCouplingFieldDouble::New(ParaMEDMEM::ON_CELLS,ParaMEDMEM::NO_TIME);
    aField->setName("my3DTallyField");
    aField->setMesh(aUmesh);

    ParaMEDMEM::DataArrayDouble * aTE = ParaMEDMEM::DataArrayDouble::New();
    aTE->alloc(aUmesh->getNumberOfCells(), 2);  //two solutions: tally and error
    aTE->setInfoOnComponent(0, "Tally");
    aTE->setInfoOnComponent(1, "Error");
    aTE->fillWithValue(1);
    aField->setArray(aTE);
    aTE->decrRef();
    aUmesh->decrRef();
    return  aField;

}

SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr TestMCMESHTRAN_Pre::buildFieldIOR()
{
    ParaMEDMEM::MEDCouplingFieldDouble * aField = build3Dfield_1();
    aField->checkCoherency();
    ParaMEDMEM::MEDCouplingFieldDoubleServant *myFieldDoubleI=new ParaMEDMEM::MEDCouplingFieldDoubleServant(aField);
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_ptr myFieldIOR = myFieldDoubleI->_this();
    return myFieldIOR;
}


MCMESHTRAN_ORB::Mesh_ptr  TestMCMESHTRAN_Pre::genNewMesh()
{
    Mesh * aMesh = new Mesh ("Me", genId() , "MCNPFmesh", 1000);
    return aMesh->_this();
}


MCNPFmesh * TestMCMESHTRAN_Pre::genMFmeshXYZ()
{
    /*
    i->	0	1	2			i->	0	1	2			j->	0	1	2			k->	0	1	2
k=0	j=0	1	2	3		j=0	k=0	1	2	3		i=0	k=0	1	2	3		i=0	j=0	1	2	3
    j=1	4	5	6			k=1	4	5	6			k=1	4	5	6			j=1	4	5	6
    j=2	7	8	9			k=2	7	8	9			k=2	7	8	9			j=2	7	8	9
k=1	j=0	10	11	12		j=1	k=0	10	11	12		i=1	k=0	10	11	12		i=1	j=0	10	11	12
    j=1	13	14	15			k=1	13	14	15			k=1	13	14	15			j=1	13	14	15
    j=2	16	17	18			k=2	16	17	18			k=2	16	17	18			j=2	16	17	18
k=2	j=0	19	20	21		j=2	k=0	19	20	21		i=2	k=0	19	20	21		i=2	j=0	19	20	21
    j=1	22	23	24			k=1	22	23	24			k=1	22	23	24			j=1	22	23	24
    j=2	25	26	27			k=2	25	26	27			k=2	25	26	27			j=2	25	26	27
     */
    const int n=4;
    MCNPFmesh * aMFmesh = new MCNPFmesh ;
    for (int i=0; i<n; i++)
    {
        aMFmesh->CoordinateI.push_back(i);
        aMFmesh->CoordinateJ.push_back(i*10);
        aMFmesh->CoordinateK.push_back(i*100);
    }

    for (int i=1; i<=(n-1)*(n-1)*(n-1); i++)
    {
        aMFmesh->TallyArray.push_back(i);
        aMFmesh->ErrorArray.push_back(i);
    }
    return aMFmesh;
}

MCMESHTRAN TestMCMESHTRAN_Pre::genEngine()
{
//    CORBA::ORB_ptr orb;
//    PortableServer::POA_ptr poa;
//    PortableServer::ObjectId * contId = new  PortableServer::ObjectId ;
//    const char *instanceName = "aInstance";
//    const char *interfaceName = "aInterface";

//    MCMESHTRAN aEngine (orb, poa, contId, instanceName, interfaceName);
    MCMESHTRAN aEngine ;
    return aEngine;
}

int TestMCMESHTRAN_Pre::genId()
{
    srand(Id);
    Id = rand()%111 +1;
    return Id;
}

