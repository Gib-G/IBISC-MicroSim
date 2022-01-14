#ifndef CDemo2H
#define CDemo2H
//---------------------------------------------------------------------------
#include <chai3d.h>
#include "CGenericDemo.h"
//---------------------------------------------------------------------------

class cDemo2 : public cGenericDemo
{    
    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

public:

    //! Constructor of cDemo1.
    cDemo2(const std::string a_resourceRoot,
           const int a_numDevices,
           std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
           std::shared_ptr<cGenericHapticDevice> a_hapticDevice);

    //! Destructor of cDemo1.
    virtual ~cDemo2() {};


    //-----------------------------------------------------------------------
    // PUBLIC METHODS:
    //-----------------------------------------------------------------------

public:

    //! Initialize demo
    virtual void init();

    //! Set stiffness
    virtual void setStiffness(double a_stiffness);


    //-----------------------------------------------------------------------
    // PUBLIC MEMBERS:
    //-----------------------------------------------------------------------

public:

    //! bullet objects
    cBulletSphere* m_ODEBody0;
    cBulletSphere* m_ODEBody1;
    cBulletSphere* m_ODEBody2;

    cBulletCylinder* m_ODEBase0;
    cBulletCylinder* m_ODEBase1;
    cBulletCylinder* m_ODEBase2;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
