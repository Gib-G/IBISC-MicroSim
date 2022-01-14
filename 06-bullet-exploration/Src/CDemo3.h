#ifndef CDemo3H
#define CDemo3H
//---------------------------------------------------------------------------
#include <chai3d.h>
#include "CGenericDemo.h"
//---------------------------------------------------------------------------

class cDemo3 : public cGenericDemo
{
    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

public:

    //! Constructor of cDemo1.
    cDemo3(const std::string a_resourceRoot,
           const int a_numDevices,
           std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
           std::shared_ptr<cGenericHapticDevice> a_hapticDevice);

    //! Destructor of cDemo1.
    virtual ~cDemo3() {};


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
    cBulletBox* m_ODEBody0;

    cBulletBox* m_ODEBase0;
    cBulletBox* m_ODEBase1;
    cBulletBox* m_ODEBase2;
    cBulletBox* m_ODEBase3;
    cBulletBox* m_ODEBase4;
    cBulletBox* m_ODEBase5;
    cBulletBox* m_ODEBase6;
    cBulletBox* m_ODEBase7;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
