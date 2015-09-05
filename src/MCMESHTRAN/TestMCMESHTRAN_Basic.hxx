#ifndef TESTMCMESHTRAN_BASIC_HXX
#define TESTMCMESHTRAN_BASIC_HXX

#include "TestMCMESHTRAN_Pre.hxx"

class TestMCMESHTRAN_Basic : public TestMCMESHTRAN_Pre
{

    CPPUNIT_TEST_SUITE( TestMCMESHTRAN_Basic );
    CPPUNIT_TEST (TestMesh);
    CPPUNIT_TEST (TestGroup);
    CPPUNIT_TEST (TestMCNPFmesh);
    CPPUNIT_TEST (TestMCNPFmeshReadConfig);
    CPPUNIT_TEST (TestMCNPFmeshReadXYZ);
    CPPUNIT_TEST (TestMCNPFmeshReadRZT);
    CPPUNIT_TEST (TestMCNPFmeshOrthogonize);
    CPPUNIT_TEST (TestMCNPFmeshCoordinateTransform);
    CPPUNIT_TEST (TestMCNPFmeshCutRind);
    CPPUNIT_TEST (TestMCNPFmeshReadXYZmore);
    CPPUNIT_TEST (TestMCNPFmeshReadRZTmore);
    CPPUNIT_TEST (TestMCNPFmeshReadPhoton);
    CPPUNIT_TEST (TestAbaqusReader1);
    CPPUNIT_TEST (TestAbaqusReader2);
    CPPUNIT_TEST (TestAbaqusReader3);
    CPPUNIT_TEST (TestMCNPUmeshReader1);


    CPPUNIT_TEST_SUITE_END();
public:
    void TestMesh ();
    void TestGroup ();
    void TestMCNPFmesh();
    void TestMCNPFmeshReadConfig();
    void TestMCNPFmeshReadXYZ();
    void TestMCNPFmeshReadRZT();
    void TestMCNPFmeshOrthogonize();
    void TestMCNPFmeshCoordinateTransform();
    void TestMCNPFmeshCutRind();
    void TestMCNPFmeshReadXYZmore();
    void TestMCNPFmeshReadRZTmore();
    void TestMCNPFmeshReadPhoton();
    void TestAbaqusReader1();
    void TestAbaqusReader2();
    void TestAbaqusReader3();
    void TestMCNPUmeshReader1();




};

#endif // TESTMCMESHTRAN_BASIC_HXX
