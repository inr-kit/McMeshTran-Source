#include "TestMCMESHTRAN_Engine.hxx"

void TestMCMESHTRAN_Engine::TestEngineBasic()
{
    MESSAGE("TestEngineBasic");
    int tmpID, tmpIF;
    int studyID = 1;
    MCMESHTRAN aEngine;
    //test of generate a group, append a group and get agroup
    MCMESHTRAN_ORB::MeshGroup_var aGroup = aEngine.generateGroup("groupA");
    tmpID = aGroup->getID();
    aEngine.appendGroup(studyID, aGroup._retn());
    MCMESHTRAN_ORB::MeshGroup_var bGroup = aEngine.getGroup(studyID, tmpID);
    CPPUNIT_ASSERT (strcmp(bGroup->getName(), "groupA") ==0);
    MCMESHTRAN_ORB::MeshGroup_var cGroup ;
    CPPUNIT_ASSERT (aEngine.appendGroup(studyID, cGroup._retn()) ==false );
    try
    {
        MCMESHTRAN_ORB::MeshGroup_var dGroup = aEngine.generateGroup("");
    }
    catch (SALOME::SALOME_Exception &ex)   {        cerr << ex.details.text <<endl;    }

    //test of generate a mesh, get a mesh
    MCMESHTRAN_ORB::Mesh_var aMesh = aEngine.generateMesh("meshA", "MCNPFmesh", 1000);
    tmpIF = aMesh->getID();
    MCMESHTRAN_ORB::MeshGroup_var eGroup = aEngine.getGroup(studyID, tmpID);
    tmpID = eGroup->getID();
    eGroup->appendMesh(aMesh._retn());
    MCMESHTRAN_ORB::Mesh_var bMesh = aEngine.getMesh(studyID, tmpID, tmpIF) ;
    CPPUNIT_ASSERT (strcmp(bMesh->getName(), "meshA") ==0);

    //test of insert a group and delete a group, clear groups
    MCMESHTRAN_ORB::MeshGroup_var fGroup = aEngine.generateGroup("groupB");
    aEngine.insertGroup(studyID, fGroup, tmpID);
    MCMESHTRAN_ORB::MeshGroup_var gGroup = (*aEngine.getData(studyID))[0];  // should be at first
    CPPUNIT_ASSERT (strcmp(gGroup->getName(), "groupB") ==0);
    MCMESHTRAN_ORB::MeshGroup_var hGroup = aEngine.generateGroup("groupC");
    CPPUNIT_ASSERT (aEngine.insertGroup(studyID, hGroup,100) == false);
    MCMESHTRAN_ORB::MeshGroup_var iGroup = (*aEngine.getData(studyID))[2];
    CPPUNIT_ASSERT (strcmp(iGroup->getName(), "groupC") ==0);
    tmpID = iGroup->getID();
    aEngine.deleteGroup(studyID, tmpID);
    CPPUNIT_ASSERT(aEngine.getData(studyID)->length() == 2);
    CPPUNIT_ASSERT (aEngine.getGroup(studyID, tmpID)->_is_nil() == true);
}

void TestMCMESHTRAN_Engine::TestEngineClear()
{
    MESSAGE("TestEngineClear");
    int tmpID;
    int studyID = 1;
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var aGroup = aEngine.generateGroup("groupA");
    tmpID = aGroup->getID();
    aEngine.appendGroup(studyID, aGroup._retn());
    CPPUNIT_ASSERT(aEngine.getData(studyID)->length() == 1);
    aEngine.clearGroup(studyID);
    CPPUNIT_ASSERT(aEngine.getData(studyID)->length() == 0);
}


void TestMCMESHTRAN_Engine::TestEngineCnvMCNPFmesh2MEDCouplingXYZ()
{
    MESSAGE("TestEngineCnvMCNPFmesh2MEDCouplingXYZ");
    MCMESHTRAN aEngine;
    vector <MCNPFmesh> MeshTallies;
    MCNPFmeshReader aReader ;
    aReader.ReadMeshConfig("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", MeshTallies);
    aReader.ReadMeshtal("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh", MeshTallies);
    CPPUNIT_ASSERT (MeshTallies.size() == 1);
    MCNPFmesh * meshtal = &MeshTallies[0];
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var aFieldIOR = aEngine.cnvMCNPFmesh2MEDCoupling( *meshtal);
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aFieldIOR);

    //check mesh
    ParaMEDMEM::MEDCouplingUMesh * aUmesh = (ParaMEDMEM::MEDCouplingUMesh *) aField->getMesh();
    ParaMEDMEM::DataArrayDouble* aCoord = aUmesh->getCoords();
    int len = meshtal->CoordinateI.size() * meshtal->CoordinateJ.size() *meshtal->CoordinateK.size() ;
//    CPPUNIT_ASSERT_EQUAL (len*3, aCoord->getNbOfElems()); //contains x, y, z  //2013-05-23 modif to adapt change in SALOME 7.2.0


    Point aPoint ;
    double * aTuple  = new double [3]; // a tuple with 3 components: X, Y, Z
    for (int k=0; k<meshtal->CoordinateK.size(); k++) {
        for (int j=0; j<meshtal->CoordinateJ.size(); j++) {
            for (int i=0; i<meshtal->CoordinateI.size(); i++) {
                aPoint = meshtal->getOnePoint(i, j, k);
                aCoord->getTuple(k*meshtal->CoordinateJ.size()*meshtal->CoordinateI.size() + j*meshtal->CoordinateI.size() + i, aTuple);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.x, aTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.y, aTuple[1], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.z, aTuple[2], 1e-7);
            }
        }
    }
    delete [] aTuple ;

    //check result
    ParaMEDMEM::DataArrayDouble* aTE = aField->getArray();  //get field result
    len = (meshtal->CoordinateI.size()-1) * (meshtal->CoordinateJ.size()-1) *(meshtal->CoordinateK.size()-1) ;
//    CPPUNIT_ASSERT_EQUAL (len*2, aTE->getNbOfElems()); //contains Tally and Error //2013-05-23 modif to adapt change in SALOME 7.2.0


    double * bTuple  = new double [2]; // a tuple with 2 components: Tally and error
    for (int k=0; k<meshtal->CoordinateK.size()-1; k++) {
        for (int j=0; j<meshtal->CoordinateJ.size()-1; j++) {
            for (int i=0; i<meshtal->CoordinateI.size() -1; i++) {
                aTE->getTuple(k*(meshtal->CoordinateJ.size()-1)*(meshtal->CoordinateI.size()-1) + j*(meshtal->CoordinateI.size()-1) + i, bTuple);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (meshtal->getOneTallyResult(i, j, k), bTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (meshtal->getOneErrorResult(i, j, k), bTuple[1], 1e-7);
            }
        }
    }
    delete [] bTuple;
}

void TestMCMESHTRAN_Engine::TestEngineCnvMCNPFmesh2MEDCouplingRZT()
{
    MESSAGE("TestEngineCnvMCNPFmesh2MEDCouplingRZT");
    MCMESHTRAN aEngine;
    vector <MCNPFmesh> MeshTallies;
    MCNPFmeshReader aReader ;
    aReader.ReadMeshConfig("./MCNPFmesh/2.1MT+RZT+0E+DF+WOFM+N", MeshTallies);
    aReader.ReadMeshtal("./MCNPFmesh/2.1MT+RZT+0E+DF+WOFM+N.msh", MeshTallies);
    CPPUNIT_ASSERT (MeshTallies.size() == 1);
    MCNPFmesh * meshtal = &MeshTallies[0];
    SALOME_MED::MEDCouplingFieldDoubleCorbaInterface_var aFieldIOR = aEngine.cnvMCNPFmesh2MEDCoupling( *meshtal);
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aFieldIOR);

    //check mesh
    ParaMEDMEM::MEDCouplingUMesh * aUmesh = (ParaMEDMEM::MEDCouplingUMesh *) aField->getMesh();
    ParaMEDMEM::DataArrayDouble* aCoord = aUmesh->getCoords();
    int len = meshtal->CoordinateI.size() * meshtal->CoordinateJ.size() *meshtal->CoordinateK.size() ;
//    CPPUNIT_ASSERT_EQUAL (len*3, aCoord->getNbOfElems()); //contains x, y, z  //2013-05-23 modif to adapt change in SALOME 7.2.0


    Point aPoint ;
    double * aTuple  = new double [3]; // a tuple with 3 components: X, Y, Z
    for (int k=0; k<meshtal->CoordinateK.size(); k++) {
        for (int j=0; j<meshtal->CoordinateJ.size(); j++) {
            for (int i=0; i<meshtal->CoordinateI.size(); i++) {
                aPoint = meshtal->getOnePoint(i, j, k);
                aCoord->getTuple(k*meshtal->CoordinateJ.size()*meshtal->CoordinateI.size() + j*meshtal->CoordinateI.size() + i, aTuple);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.x, aTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.y, aTuple[1], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.z, aTuple[2], 1e-7);
            }
        }
    }
    delete [] aTuple ;

    //check result
    ParaMEDMEM::DataArrayDouble* aTE = aField->getArray();  //get field result
    len = (meshtal->CoordinateI.size()-1) * (meshtal->CoordinateJ.size()-1) *(meshtal->CoordinateK.size()-1) ;
//    CPPUNIT_ASSERT_EQUAL (len*2, aTE->getNbOfElems()); //contains Tally and Error //2013-05-23 modif to adapt change in SALOME 7.2.0


    double * bTuple  = new double [2]; // a tuple with 2 components: Tally and error
    for (int k=0; k<meshtal->CoordinateK.size()-1; k++) {
        for (int j=0; j<meshtal->CoordinateJ.size()-1; j++) {
            for (int i=0; i<meshtal->CoordinateI.size() -1; i++) {
                aTE->getTuple(k*(meshtal->CoordinateJ.size()-1)*(meshtal->CoordinateI.size()-1) + j*(meshtal->CoordinateI.size()-1) + i, bTuple);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (meshtal->getOneTallyResult(i, j, k), bTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (meshtal->getOneErrorResult(i, j, k), bTuple[1], 1e-7);
            }
        }
    }
    delete [] bTuple;
}

void TestMCMESHTRAN_Engine::TestEngineCnvMCNPFmesh2Mesh()
{
    MESSAGE("TestEngineCnvMCNPFmesh2Mesh");
    MCMESHTRAN aEngine;
    vector <MCNPFmesh> MeshTallies;
    MCNPFmeshReader aReader ;
    aReader.ReadMeshConfig("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", MeshTallies);
    aReader.ReadMeshtal("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh", MeshTallies);
    CPPUNIT_ASSERT (MeshTallies.size() == 1);
    MCNPFmesh * meshtal = &MeshTallies[0];
    MCMESHTRAN_ORB::Mesh_var aMesh;
    try
    { aMesh = aEngine.cnvMCNPFmesh2Mesh(*meshtal);}
    catch (SALOME::SALOME_Exception &ex)
    {cout << ex.details.text <<endl;}

    CPPUNIT_ASSERT (strcmp (aMesh->getName(), "MFmesh4") == 0);
    CPPUNIT_ASSERT (strcmp (aMesh->getType(), "MCNPFmesh") == 0);
    CPPUNIT_ASSERT ((long long int)aMesh->getNPS() == meshtal->NHistory);
    CPPUNIT_ASSERT (aMesh->getField()->_is_nil() == false);
}

void TestMCMESHTRAN_Engine::TestEngineReadMCNPFmesh()
{
    MESSAGE("TestEngineReadMCNPFmesh");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;

    //test 1
    try     {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", "./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh");
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }
    CPPUNIT_ASSERT (aGroup->getMeshList()->length() == 1);
    CPPUNIT_ASSERT (strcmp ((*aGroup->getMeshList())[0] ->getName(), "MFmesh4") == 0);
    CPPUNIT_ASSERT ((*aGroup->getMeshList())[0] ->getField()->_is_nil() == false);

    //test 2
    try     {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/3.NMT+XYZ+E", "./MCNPFmesh/3.NMT+XYZ+E.msh");
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }
    CPPUNIT_ASSERT (aGroup->getMeshList()->length() == 4);
    CPPUNIT_ASSERT (strcmp ((*aGroup->getMeshList())[0] ->getName(), "MFmesh4") == 0);
    CPPUNIT_ASSERT ((*aGroup->getMeshList())[0] ->getField()->_is_nil() == false);
}

void TestMCMESHTRAN_Engine::TestEngineCheckMeshConsistency()
{
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;
    MCMESHTRAN_ORB::MeshGroup_var  bGroup;
    MCMESHTRAN_ORB::MeshGroup_var  cGroup;
    MCMESHTRAN_ORB::MeshGroup_var  dGroup;
    try     {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", "./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh");
        bGroup = aEngine.readMCNPFmesh("./MCNPFmesh/2.1MT+RZT+0E+DF+WOFM+N", "./MCNPFmesh/2.1MT+RZT+0E+DF+WOFM+N.msh");
        cGroup = aEngine.readMCNPFmesh("./MCNPFmesh/3.NMT+XYZ+E", "./MCNPFmesh/3.NMT+XYZ+E.msh");
        dGroup = aEngine.readMCNPFmesh("./MCNPFmesh/4.NMT+RZT+E", "./MCNPFmesh/4.NMT+RZT+E.msh");
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }
    MCMESHTRAN_ORB::Mesh_var aMesh, bMesh, cMesh, dMesh;
    aMesh = (*aGroup->getMeshList())[0];
    bMesh = (*bGroup->getMeshList())[0];
    cMesh = (*cGroup->getMeshList())[0];
    dMesh = (*dGroup->getMeshList())[0];

    try
    {
                CPPUNIT_ASSERT (aEngine.checkMeshConsistency(aMesh.in(), bMesh.in()) == false);
                CPPUNIT_ASSERT (aEngine.checkMeshConsistency(aMesh.in(), cMesh.in()) == true);
                CPPUNIT_ASSERT (aEngine.checkMeshConsistency(bMesh.in(), dMesh.in()) == true);
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }

}

void TestMCMESHTRAN_Engine::TestEngineSumMeshes()
{
    MESSAGE("TestEngineSumMeshes");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;
    vector <MCNPFmesh> MeshTallies;
    MCNPFmeshReader aReader ;

    //test reading meshtal
    aReader.ReadMeshConfig("./MCNPFmesh/5.NMT+XYZ+P", MeshTallies);
    aReader.ReadMeshtal("./MCNPFmesh/5.NMT+XYZ+P.msh", MeshTallies);
    CPPUNIT_ASSERT (MeshTallies.size() == 4);
    MCNPFmesh * meshtalA = &MeshTallies[0];  //photon
    MCNPFmesh * meshtalB = &MeshTallies[2];  //neutron
    MCMESHTRAN_ORB::Mesh_var aMesh, bMesh, cMesh;

    try    {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/5.NMT+XYZ+P","./MCNPFmesh/5.NMT+XYZ+P.msh" );
        aMesh = (*aGroup->getMeshList())[0];
        bMesh = (*aGroup->getMeshList())[2];
        cMesh = aEngine.sumMeshes(aMesh, bMesh, "sum");
    }
    catch (SALOME::SALOME_Exception &ex)
    { cout << ex.details.text <<endl;}
    catch(...)
    {cout <<" I don't know what it is the problem. " <<endl;}

    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(cMesh->getField());
    double * aTuple = new double [2];
    for (int k=0; k<meshtalA->CoordinateK.size()-1; k++) {
        for (int j=0; j<meshtalA->CoordinateJ.size()-1; j++) {
            for (int i=0; i<meshtalA->CoordinateI.size()-1; i++) {
                aField_ptr->getArray()->getTuple(k*(meshtalA->CoordinateJ.size()-1)*(meshtalA->CoordinateI.size()-1)
                                                 + j*(meshtalA->CoordinateI.size()-1) + i, aTuple);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (meshtalA->getOneTallyResult(i, j, k) +
                                              meshtalB->getOneTallyResult(i, j, k), aTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (max(meshtalA->getOneErrorResult(i, j, k),
                                                  meshtalB->getOneErrorResult(i, j, k)), aTuple[1], 1e-7);
            }
        }
    }

    CPPUNIT_ASSERT(aMesh->_is_nil() == false);
    CPPUNIT_ASSERT(aMesh->getField()->_is_nil() == false);
    CPPUNIT_ASSERT(bMesh->_is_nil() == false);
    CPPUNIT_ASSERT(bMesh->getField()->_is_nil() == false);
    CPPUNIT_ASSERT(cMesh->_is_nil() == false);
    delete [] aTuple ;
//    aField_ptr->decrRef();
}


void TestMCMESHTRAN_Engine::TestEngineAverageMeshes()
{
    MESSAGE("TestEngineAverageMeshes");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;
    vector <MCNPFmesh> MeshTallies;
    MCNPFmeshReader aReader ;

    //test reading meshtal
    aReader.ReadMeshConfig("./MCNPFmesh/5.NMT+XYZ+P", MeshTallies);
    aReader.ReadMeshtal("./MCNPFmesh/5.NMT+XYZ+P.msh", MeshTallies);
    CPPUNIT_ASSERT (MeshTallies.size() == 4);
    MCNPFmesh * meshtalA = &MeshTallies[0];  //photon1
    MCNPFmesh * meshtalB = &MeshTallies[1];  //photon2
    MCMESHTRAN_ORB::Mesh_var aMesh, bMesh, cMesh;

    try    {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/5.NMT+XYZ+P","./MCNPFmesh/5.NMT+XYZ+P.msh" );
        aMesh = (*aGroup->getMeshList())[0];
        bMesh = (*aGroup->getMeshList())[1];
        cMesh = aEngine.averageMeshes(aMesh, bMesh, "average");
    }
    catch (SALOME::SALOME_Exception &ex)
    { cout << ex.details.text <<endl;}
    catch(...)
    {cout <<" I don't know what it is the problem. " <<endl;}

    CPPUNIT_ASSERT (cMesh->getField()->_is_nil() == false);
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(cMesh->getField());
    double * aTuple = new double [2];
    double NPSA = static_cast <double > (meshtalA->NHistory );
    double NPSB = static_cast <double > (meshtalB->NHistory);
    double NPS =NPSA + NPSB;

    for (int k=0; k<meshtalA->CoordinateK.size()-1; k++) {
        for (int j=0; j<meshtalA->CoordinateJ.size()-1; j++) {
            for (int i=0; i<meshtalA->CoordinateI.size()-1; i++) {
                aField_ptr->getArray()->getTuple(k*(meshtalA->CoordinateJ.size()-1)*(meshtalA->CoordinateI.size()-1)
                                                 + j*(meshtalA->CoordinateI.size()-1) + i, aTuple);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (((meshtalA->getOneTallyResult(i, j, k) * NPSA ) +
                                              ( meshtalB->getOneTallyResult(i, j, k) * NPSB)) / NPS, aTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (((meshtalA->getOneErrorResult(i, j, k) * NPSA)+
                                               (meshtalB->getOneErrorResult(i, j, k) * NPSB)) / NPS, aTuple[1], 1e-7);
            }
        }
    }
    delete [] aTuple;
}


void TestMCMESHTRAN_Engine::TestEngineTranslateMesh()
{
    MESSAGE("TestEngineTranslateMesh");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;
    vector <MCNPFmesh> MeshTallies;
    MCNPFmeshReader aReader ;

    //test reading meshtal
    aReader.ReadMeshConfig("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", MeshTallies);
    aReader.ReadMeshtal("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh", MeshTallies);
    CPPUNIT_ASSERT (MeshTallies.size() == 1);
    MCNPFmesh * meshtal = &MeshTallies[0];

    try     {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", "./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh");
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }
    MCMESHTRAN_ORB::FixArray3_var aVector = new MCMESHTRAN_ORB::FixArray3;
    aVector[0] = 100;
    aVector[1] = 200;
    aVector[2] = 300;

    MCMESHTRAN_ORB::Mesh_var aMesh = aEngine.translateMesh((*aGroup->getMeshList())[0],aVector, "translate");
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());

    double * aTuple = new double [3];
    Point aPoint ;
    for (int k=0; k<meshtal->CoordinateK.size(); k++) {
        for (int j=0; j<meshtal->CoordinateJ.size(); j++) {
            for (int i=0; i<meshtal->CoordinateI.size(); i++) {
                aPoint = meshtal->getOnePoint(i, j, k);
                aField_ptr->getMesh()->getCoordinatesAndOwner()->getTuple(k*(meshtal->CoordinateJ.size())*(meshtal->CoordinateI.size())
                                                 + j*(meshtal->CoordinateI.size()) + i, aTuple);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.x +100, aTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.y +200, aTuple[1], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.z +300, aTuple[2], 1e-7);
            }
        }
    }
    delete [] aTuple;
}


void TestMCMESHTRAN_Engine::TestEngineRotateMesh()
{
    MESSAGE("TestEngineRotateMesh");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;
    vector <MCNPFmesh> MeshTallies;
    MCNPFmeshReader aReader ;

    //test reading meshtal
    aReader.ReadMeshConfig("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", MeshTallies);
    aReader.ReadMeshtal("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh", MeshTallies);
    CPPUNIT_ASSERT (MeshTallies.size() == 1);
    MCNPFmesh * meshtal = &MeshTallies[0];

    try     {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", "./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh");
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }
    MCMESHTRAN_ORB::FixArray3_var aCenter = new MCMESHTRAN_ORB::FixArray3;
    MCMESHTRAN_ORB::FixArray3_var aVector = new MCMESHTRAN_ORB::FixArray3;
    aCenter[0] = 0;
    aCenter[1] = 0;
    aCenter[2] = 0;
    aVector[0] = 0;
    aVector[1] = 0;
    aVector[2] = 100;
    double aAngle = PI / 4; // 45 degree

    MCMESHTRAN_ORB::Mesh_var aMesh = aEngine.rotateMesh((*aGroup->getMeshList())[0], aCenter, aVector, aAngle, "rotate");
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());

    double * aTuple = new double [3];
    double aDouble = sqrt(2.0) / 2.0;
    Point aPoint ;
    for (int k=0; k<meshtal->CoordinateK.size(); k++) {
        for (int j=0; j<meshtal->CoordinateJ.size(); j++) {
            for (int i=0; i<meshtal->CoordinateI.size(); i++) {
                aPoint = meshtal->getOnePoint(i, j, k);
                aField_ptr->getMesh()->getCoordinatesAndOwner()->getTuple(k*(meshtal->CoordinateJ.size())*(meshtal->CoordinateI.size())
                                                 + j*(meshtal->CoordinateI.size()) + i, aTuple);
                //for 2D rotate, x'= x*cos(a) - y*sin(a), y'= x*sin(a) + y*cos(a)
                //when a = PI/4, cos(a) = sin(a) = sqrt(2)/2
                CPPUNIT_ASSERT_DOUBLES_EQUAL ((aPoint.x - aPoint.y  ) * aDouble, aTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL ((aPoint.x + aPoint.y  ) * aDouble, aTuple[1], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.z , aTuple[2], 1e-7);
//                cout << "x = "<<(aPoint.y + aPoint.x) * aDouble << "\t y = "<< (aPoint.y - aPoint.x) * aDouble << "\t z = "<<aPoint.z<<endl;
//                cout << "X = "<<aTuple[0] << "\t Y = "<<aTuple[1] << "\t Z = "<<aTuple[2]<<endl;
            }
        }
    }
    delete [] aTuple;
}

void TestMCMESHTRAN_Engine::TestEngineScale()
{
    MESSAGE("TestEngineScale");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;
    vector <MCNPFmesh> MeshTallies;
    MCNPFmeshReader aReader ;

    //test reading meshtal
    aReader.ReadMeshConfig("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", MeshTallies);
    aReader.ReadMeshtal("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh", MeshTallies);
    CPPUNIT_ASSERT (MeshTallies.size() == 1);
    MCNPFmesh * meshtal = &MeshTallies[0];

    try     {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", "./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh");
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }

    MCMESHTRAN_ORB::Mesh_var aMesh = aEngine.scaleMesh((*aGroup->getMeshList())[0], 10, "scale");
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());

    double * aTuple = new double [3];
    Point aPoint ;
    for (int k=0; k<meshtal->CoordinateK.size(); k++) {
        for (int j=0; j<meshtal->CoordinateJ.size(); j++) {
            for (int i=0; i<meshtal->CoordinateI.size(); i++) {
                aPoint = meshtal->getOnePoint(i, j, k);
                aField_ptr->getMesh()->getCoordinatesAndOwner()->getTuple(k*(meshtal->CoordinateJ.size())*(meshtal->CoordinateI.size())
                                                 + j*(meshtal->CoordinateI.size()) + i, aTuple);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.x *10, aTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.y *10, aTuple[1], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (aPoint.z *10, aTuple[2], 1e-7);
            }
        }
    }
    delete [] aTuple;
}

void TestMCMESHTRAN_Engine::TestEngineMultiply()
{
    MESSAGE("TestEngineMultiply");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;
    vector <MCNPFmesh> MeshTallies;
    MCNPFmeshReader aReader ;

    //test reading meshtal
    aReader.ReadMeshConfig("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", MeshTallies);
    aReader.ReadMeshtal("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh", MeshTallies);
    CPPUNIT_ASSERT (MeshTallies.size() == 1);
    MCNPFmesh * meshtal = &MeshTallies[0];  //photon

    try    {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", "./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh");
    }
    catch (SALOME::SALOME_Exception &ex)
    { cout << ex.details.text <<endl;}

    MCMESHTRAN_ORB::Mesh_var aMesh = aEngine.multiplyFactor((*aGroup->getMeshList())[0], 10, "multiply");
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());

    double * aTuple = new double [2];
    for (int k=0; k<meshtal->CoordinateK.size()-1; k++) {
        for (int j=0; j<meshtal->CoordinateJ.size()-1; j++) {
            for (int i=0; i<meshtal->CoordinateI.size()-1; i++) {
                aField_ptr->getArray()->getTuple(k*(meshtal->CoordinateJ.size()-1)*(meshtal->CoordinateI.size()-1)
                                                 + j*(meshtal->CoordinateI.size()-1) + i, aTuple);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (meshtal->getOneTallyResult(i, j, k)*10, aTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (meshtal->getOneErrorResult(i, j, k), aTuple[1], 1e-7);
            }
        }
    }

    delete [] aTuple ;
}

void TestMCMESHTRAN_Engine::TestEngineInterpolate()
{
    MESSAGE("TestEngineInterpolate");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;
    vector <MCNPFmesh> MeshTallies;
    MCNPFmeshReader aReader ;

    //test reading meshtal
    aReader.ReadMeshConfig("./MCNPFmesh/5.NMT+XYZ+P", MeshTallies);
    aReader.ReadMeshtal("./MCNPFmesh/5.NMT+XYZ+P.msh", MeshTallies);
    CPPUNIT_ASSERT (MeshTallies.size() == 4);
    MCNPFmesh * meshtalA = &MeshTallies[0];  //photon1
//    MCNPFmesh * meshtalB = &MeshTallies[1];  //photon2
    MCMESHTRAN_ORB::Mesh_var aMesh, bMesh, cMesh;

    try    {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/5.NMT+XYZ+P","./MCNPFmesh/5.NMT+XYZ+P.msh" );
        aMesh = (*aGroup->getMeshList())[0];
        bMesh = (*aGroup->getMeshList())[1];
        cMesh = aEngine.interpolateMEDMesh(aMesh, bMesh, "interpolate",MCMESHTRAN_ORB::on_cells);
    }
    catch (SALOME::SALOME_Exception &ex)
    { cout << ex.details.text <<endl;}
    catch(...)
    {cout <<" I don't know what it is the problem. " <<endl;}

    CPPUNIT_ASSERT (cMesh->getField()->_is_nil() == false);
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(cMesh->getField());
    double * aTuple = new double [2];

    //target mesh is identical to the source mesh,
    //therefore data should be the same after interpolation
    for (int k=0; k<meshtalA->CoordinateK.size()-1; k++) {
        for (int j=0; j<meshtalA->CoordinateJ.size()-1; j++) {
            for (int i=0; i<meshtalA->CoordinateI.size()-1; i++) {
                aField_ptr->getArray()->getTuple(k*(meshtalA->CoordinateJ.size()-1)*(meshtalA->CoordinateI.size()-1)
                                                 + j*(meshtalA->CoordinateI.size()-1) + i, aTuple);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (meshtalA->getOneTallyResult(i, j, k), aTuple[0], 1e-7);
                CPPUNIT_ASSERT_DOUBLES_EQUAL (meshtalA->getOneErrorResult(i, j, k), aTuple[1], 1e-7);
            }
        }
    }
    delete [] aTuple;
}

void TestMCMESHTRAN_Engine::TestEngineInterpolate2()
{
    MESSAGE("TestEngineInterpolate");
    MCMESHTRAN aEngine;

//    try    {
//        aEngine.interpolateCGNSMesh("/home/qiu/Desktop/MCNPFmesh/test-Li-1.cgns"
//                                    ,"/home/qiu/Desktop/MCNPFmesh/Li-mid-right-fine.cgns"
//                                    ,"/home/qiu/Desktop/MCNPFmesh/Li-cgns_int.cgns",MCMESHTRAN_ORB::on_cells );

//    }
//    catch (SALOME::SALOME_Exception &ex)
//    { cout << ex.details.text <<endl;}
//    catch(...)
//    {cout <<" I don't know what it is the problem. " <<endl;}

}

void TestMCMESHTRAN_Engine::TestEngineExport2VTK()
{
    MESSAGE("TestEngineExport2VTK");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;

    try     {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", "./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh");
        MCMESHTRAN_ORB::Mesh_var aMesh = (*aGroup->getMeshList())[0];
        aEngine.export2VTK(aMesh, "test.vtk");
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }

}

void TestMCMESHTRAN_Engine::TestEngineExport2MED()
{
    MESSAGE("TestEngineExport2MED");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;
    try     {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", "./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh");
        MCMESHTRAN_ORB::Mesh_var aMesh = (*aGroup->getMeshList())[0];
        aEngine.export2MED(aMesh, "test.med");
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }

}

void TestMCMESHTRAN_Engine::TestEngineImportMED()
{
    MESSAGE("TestEngineImportMED");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup, bGroup;
    MCMESHTRAN_ORB::Mesh_var aMesh, bMesh;
    try     {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", "./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh");
        aMesh = (*aGroup->getMeshList())[0];
        aEngine.export2MED(aMesh, "test.med");
        bGroup = aEngine.importMED("test.med");
        bMesh = (*bGroup->getMeshList())[0];
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }


    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> aField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(aMesh->getField());
    ParaMEDMEM::MEDCouplingAutoRefCountObjectPtr<ParaMEDMEM::MEDCouplingFieldDouble> bField_ptr
            =ParaMEDMEM::MEDCouplingFieldDoubleClient::New(bMesh->getField());

    double * aTuple = new double [2];
    double * bTuple = new double [2];
    CPPUNIT_ASSERT ( aField_ptr->getArray()->getNumberOfTuples() == bField_ptr->getArray()->getNumberOfTuples());
    CPPUNIT_ASSERT ( aField_ptr->getArray()->getNbOfElems() == bField_ptr->getArray()->getNbOfElems());

    for (int i=0; i<aField_ptr->getNumberOfTuples() ; i++) {
        aField_ptr->getArray()->getTuple(i, aTuple);
        bField_ptr->getArray()->getTuple(i, bTuple);
        CPPUNIT_ASSERT_DOUBLES_EQUAL ( aTuple[0],bTuple[0], 1e-7);
        CPPUNIT_ASSERT_DOUBLES_EQUAL ( aTuple[1],bTuple[1], 1e-7);
    }
}

void TestMCMESHTRAN_Engine::TestEngineExport2CSV()
{
    MESSAGE("TestEngineExport2CSV");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup;
    try     {
        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N", "./MCNPFmesh/1.1MT+XYZ+0E+DF+WOFM+N.msh");
        MCMESHTRAN_ORB::Mesh_var aMesh = (*aGroup->getMeshList())[0];
        aEngine.export2CSV(aMesh, "test.csv");
    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }

}

void TestMCMESHTRAN_Engine::TestEngineCaseTest1()
{
    MESSAGE("TestEngineCaseTest1");
    MCMESHTRAN aEngine;
    MCMESHTRAN_ORB::MeshGroup_var  aGroup, bGroup;
    try
    {

        aGroup = aEngine.readMCNPFmesh("./MCNPFmesh/iTBM_fsrc_dimsh_f6_Li1x2-rev", "./MCNPFmesh/meshtal_fsrc_dimsh_f6_Li1x2-rev");
//        CPPUNIT_ASSERT (aGroup->getMeshList()->length() == 2);
        MCMESHTRAN_ORB::Mesh_var aMesh = aEngine.sumMeshes((*aGroup->getMeshList())[0],(*aGroup->getMeshList())[1], "sum" );
        MCMESHTRAN_ORB::Mesh_var bMesh = aEngine.scaleMesh(aMesh, 10, "scale10");

        bGroup = aEngine.importMED("./MCNPFmesh/Li.med");
        CPPUNIT_ASSERT (bGroup->getMeshList()->length() == 1);
        MCMESHTRAN_ORB::Mesh_var cMesh = (*bGroup->getMeshList())[0];

        MCMESHTRAN_ORB::Mesh_var dMesh = aEngine.interpolateMEDMesh(bMesh, cMesh, "interpolate", MCMESHTRAN_ORB::on_cells);
        aEngine.export2MED(dMesh, "./MCNPFmesh/Li-int.med");
        aEngine.export2VTK(dMesh, "./MCNPFmesh/Li-int.vtk");


    }
    catch (SALOME::SALOME_Exception &ex)    {
        cout << ex.details.text <<endl;
    }

}



