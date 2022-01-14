#include "CDemo4.h"

using namespace std;
//---------------------------------------------------------------------------

//===========================================================================
/*!
    Constructor of cDemo4.
*/
//===========================================================================
cDemo4::cDemo4(const string a_resourceRoot,
    const int a_numDevices,
    shared_ptr<cGenericHapticDevice> a_hapticDevice0,
    shared_ptr<cGenericHapticDevice> a_hapticDevice1) :cGenericDemo(a_resourceRoot, a_numDevices, a_hapticDevice0, a_hapticDevice1)
{
    cMaterial matBase;
    matBase.setGrayLevel(0.3);
    matBase.setStiffness(1500);

    m_ODEBase0 = new cODEGenericBody(m_ODEWorld);
    cMesh* base0 = new cMesh();
    cCreateCylinder(base0, 0.15, 0.01, 12, 1, 1, true, true, cVector3d(0, 0, -0.075), cIdentity3d(), cColorf(1.0, 1.0, 1.0));
    m_ODEBase0->createDynamicCylinder(0.01, 0.15, true);
    base0->createAABBCollisionDetector(m_toolRadius);
    base0->setMaterial(matBase);
    m_ODEBase0->setImageModel(base0);
    m_ODEBase0->setLocalPos(0.015, 0.0, -0.02);

    // create a new ODE object that is automatically added to the ODE world
    m_ODEBody0 = new cODEGenericBody(m_ODEWorld);
    cMesh* object0 = new cMesh();
    cCreateRing(object0, 0.01, 0.025, 12, 16);
    object0->createAABBCollisionDetector(m_toolRadius);

    // define some material properties for each cube
    cMaterial mat;
    mat.setWhite();
    mat.m_specular.set(0.0, 0.0, 0.0);
    mat.setStiffness(1000);
    mat.setDynamicFriction(0.6);
    mat.setStaticFriction(0.6);
    object0->setMaterial(mat);
    m_ODEBody0->setImageModel(object0);
    m_ODEBody0->createDynamicMesh();
    m_ODEBody0->setMass(0.02);
    m_ODEBody0->setLocalPos(0.0, 0.0, 0.2);

    // initialize
    init();
};


//===========================================================================
/*!
    Set stiffness of environment

    \param  a_stiffness  Stiffness value [N/m]
*/
//===========================================================================
void cDemo4::setStiffness(double a_stiffness)
{
    // set ground
    m_ground->setStiffness(a_stiffness, true);

    // set objects
    m_ODEBody0->m_imageModel->setStiffness(a_stiffness);
    m_ODEBase0->m_imageModel->setStiffness(a_stiffness);
};


//===========================================================================
/*!
    Initialize position of objects
*/
//===========================================================================
void cDemo4::init()
{
    m_ODEBody0->setLocalPos(0.04, -0.05, 0.04);
}
