#include "CDemo3.h"

using namespace std;
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cDemo3.
*/
//===========================================================================
cDemo3::cDemo3(const string a_resourceRoot,
               const int a_numDevices,
               shared_ptr<cGenericHapticDevice> a_hapticDevice0,
               shared_ptr<cGenericHapticDevice> a_hapticDevice1):cGenericDemo(a_resourceRoot, a_numDevices, a_hapticDevice0, a_hapticDevice1)
{
    cMaterial matBase;
    matBase.setGrayLevel(0.3);
    matBase.setStiffness(1500);

    double offset = 0.02;

    cMatrix3d rot0;
    rot0.identity();
    rot0.rotateAboutGlobalAxisDeg (cVector3d(0,0,1), 45);


    m_ODEBase0 = new cBulletBox(m_bulletWorld, 0.005, 0.05, 0.01);
    m_bulletWorld->addChild(m_ODEBase0);
    m_ODEBase0->createAABBCollisionDetector(m_toolRadius);
    m_ODEBase0->setMaterial(matBase);
    m_ODEBase0->buildDynamicModel();
    m_ODEBase0->setLocalPos(0.0+ offset, 0.0 + offset, 0.050);
    m_ODEBase0->setLocalRot(rot0);

    m_ODEBase1 = new cBulletBox(m_bulletWorld, 0.005, 0.05, 0.01);
    m_bulletWorld->addChild(m_ODEBase1);
    m_ODEBase1->createAABBCollisionDetector(m_toolRadius);
    m_ODEBase1->setMaterial(matBase);
    m_ODEBase1->buildDynamicModel();
    m_ODEBase1->setLocalPos(0.0+ offset, 0.0+ offset, 0.020);
    m_ODEBase1->setLocalRot(rot0);

    m_ODEBase2 = new cBulletBox(m_bulletWorld,0.005, 0.005, 0.125);
    m_bulletWorld->addChild(m_ODEBase2);
    m_ODEBase2->createAABBCollisionDetector(m_toolRadius);
    m_ODEBase2->setMaterial(matBase);
    m_ODEBase2->buildDynamicModel();
    m_ODEBase2->setLocalPos(0.010+ offset,-0.010+ offset, 0.0);
    m_ODEBase2->setLocalRot(rot0);

    m_ODEBase3 = new cBulletBox(m_bulletWorld, 0.005, 0.005, 0.125);
    m_bulletWorld->addChild(m_ODEBase3);
    m_ODEBase3->createAABBCollisionDetector(m_toolRadius);
    m_ODEBase3->setMaterial(matBase);
    m_ODEBase3->buildDynamicModel();
    m_ODEBase3->setLocalPos(-0.01+ offset,0.01+ offset, 0.0);
    m_ODEBase3->setLocalRot(rot0);

    m_ODEBase4 = new cBulletBox(m_bulletWorld, 0.005, 0.05, 0.01);
    m_bulletWorld->addChild(m_ODEBase4);
    m_ODEBase4->createAABBCollisionDetector(m_toolRadius);
    m_ODEBase4->setMaterial(matBase);
    m_ODEBase4->buildDynamicModel();
    m_ODEBase4->setLocalPos(0.0- offset, 0.0 - offset, 0.050);
    m_ODEBase4->setLocalRot(rot0);

    m_ODEBase5 = new cBulletBox(m_bulletWorld,0.005, 0.05, 0.01);
    m_bulletWorld->addChild(m_ODEBase5);
    m_ODEBase5->createAABBCollisionDetector(m_toolRadius);
    m_ODEBase5->setMaterial(matBase);
    m_ODEBase5->buildDynamicModel();
    m_ODEBase5->setLocalPos(0.0- offset, 0.0 - offset, 0.020);
    m_ODEBase5->setLocalRot(rot0);

    m_ODEBase6 = new cBulletBox(m_bulletWorld, 0.005, 0.005, 0.125);
    m_bulletWorld->addChild(m_ODEBase6);
    m_ODEBase6->createAABBCollisionDetector(m_toolRadius);
    m_ODEBase6->setMaterial(matBase);
    m_ODEBase6->buildDynamicModel();
    m_ODEBase6->setLocalPos(0.010- offset,-0.010 - offset, 0.0);
    m_ODEBase6->setLocalRot(rot0);

    m_ODEBase7 = new cBulletBox(m_bulletWorld, 0.005, 0.005, 0.125);
    m_bulletWorld->addChild(m_ODEBase7);
    m_ODEBase7->createAABBCollisionDetector(m_toolRadius);
    m_ODEBase7->setMaterial(matBase);
    m_ODEBase7->buildDynamicModel();
    m_ODEBase7->setLocalPos(-0.01- offset,0.01 - offset, 0.0);
    m_ODEBase7->setLocalRot(rot0);


    // create a new ODE object that is automatically added to the ODE world
    m_ODEBody0 = new cBulletBox(m_bulletWorld, 0.075, 0.017, 0.017);
    m_bulletWorld->addChild(m_ODEBody0);

    m_ODEBody0->createAABBCollisionDetector(m_toolRadius);

    cMaterial mat;
    mat.setRedIndian();
    mat.m_specular.set(0.0, 0.0, 0.0);
    mat.setStiffness(1000);
    mat.setDynamicFriction(2);
    mat.setStaticFriction(2);
    m_ODEBody0->setMaterial(mat);

    m_ODEBody0->setMass(0.04);
 
    m_ODEBody0->estimateInertia();

    m_ODEBody0->buildDynamicModel();

    // initialize
    init();
};


//===========================================================================
/*!
    Set stiffness of environment

    \param  a_stiffness  Stiffness value [N/m]
*/
//===========================================================================
void cDemo3::setStiffness(double a_stiffness)
{
    // set ground
    m_ground->setStiffness(a_stiffness, true);
    
    // set objects
    m_ODEBody0->setStiffness(a_stiffness);

    m_ODEBase0->setStiffness(a_stiffness);
    m_ODEBase1->setStiffness(a_stiffness);
    m_ODEBase2->setStiffness(a_stiffness);
    m_ODEBase3->setStiffness(a_stiffness);
    m_ODEBase3->setStiffness(a_stiffness);
    m_ODEBase4->setStiffness(a_stiffness);
    m_ODEBase5->setStiffness(a_stiffness);
    m_ODEBase6->setStiffness(a_stiffness);
    m_ODEBase7->setStiffness(a_stiffness);
}


//===========================================================================
/*!
    Initialize position of objects
*/
//===========================================================================
void cDemo3::init()
{
    m_ODEBody0->setLocalPos(0.00, 0.07, 0.04);
}
