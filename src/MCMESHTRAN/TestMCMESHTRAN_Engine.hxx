#ifndef TESTMCMESHTRAN_ENGINE_HXX
#define TESTMCMESHTRAN_ENGINE_HXX

#include "TestMCMESHTRAN_Pre.hxx"

class TestMCMESHTRAN_Engine : public TestMCMESHTRAN_Pre
{
    CPPUNIT_TEST_SUITE( TestMCMESHTRAN_Engine );
    CPPUNIT_TEST (TestEngineBasic);
    CPPUNIT_TEST (TestEngineClear);
    CPPUNIT_TEST (TestEngineCnvMCNPFmesh2MEDCouplingXYZ);
    CPPUNIT_TEST (TestEngineCnvMCNPFmesh2MEDCouplingRZT);
    CPPUNIT_TEST (TestEngineCnvMCNPFmesh2Mesh);
    CPPUNIT_TEST (TestEngineReadMCNPFmesh);
    CPPUNIT_TEST (TestEngineCheckMeshConsistency);
    CPPUNIT_TEST (TestEngineSumMeshes);
    CPPUNIT_TEST (TestEngineAverageMeshes);
    CPPUNIT_TEST (TestEngineTranslateMesh);
    CPPUNIT_TEST (TestEngineRotateMesh);
    CPPUNIT_TEST (TestEngineScale);
    CPPUNIT_TEST (TestEngineMultiply);
    CPPUNIT_TEST (TestEngineInterpolate);
    CPPUNIT_TEST (TestEngineInterpolate2);
    CPPUNIT_TEST (TestEngineExport2VTK);
    CPPUNIT_TEST (TestEngineExport2MED);
    CPPUNIT_TEST (TestEngineImportMED);
    CPPUNIT_TEST (TestEngineExport2CSV);

//    CPPUNIT_TEST (TestEngineCaseTest1);

    CPPUNIT_TEST_SUITE_END();

public:
    void TestEngineBasic();
    void TestEngineClear();
    void TestEngineCnvMCNPFmesh2MEDCouplingXYZ();
    void TestEngineCnvMCNPFmesh2MEDCouplingRZT();
    void TestEngineCnvMCNPFmesh2Mesh();
    void TestEngineReadMCNPFmesh();
    void TestEngineCheckMeshConsistency();
    void TestEngineSumMeshes();
    void TestEngineAverageMeshes();
    void TestEngineTranslateMesh();
    void TestEngineRotateMesh();
    void TestEngineScale();
    void TestEngineMultiply();
    void TestEngineInterpolate();
    void TestEngineInterpolate2();
    void TestEngineExport2VTK();
    void TestEngineExport2MED();
    void TestEngineImportMED();
    void TestEngineExport2CSV();
    void TestEngineCaseTest1();









};

#endif // TESTMCMESHTRAN_ENGINE_HXX
