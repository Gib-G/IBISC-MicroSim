#ifndef CDemo4H
#define CDemo4H
//---------------------------------------------------------------------------
#include <chai3d.h>

#include "CGenericDemo.h"
//---------------------------------------------------------------------------

class cDemo4 : public cGenericDemo
{
    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

public:

    //! Constructor of cDemo4.
    cDemo4(const std::string a_resourceRoot,
        const int a_numDevices,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice);

    //! Destructor of cDemo4.
    virtual ~cDemo4() {};


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

    //! ODE objects
    cODEGenericBody* m_ODEBody0;
    cODEGenericBody* m_ODEBody1;
    cODEGenericBody* m_ODEBody2;

    cODEGenericBody* m_ODEBase0;
    cODEGenericBody* m_ODEBase1;
    cODEGenericBody* m_ODEBase2;
    cODEGenericBody* m_ODEBase3;
    cODEGenericBody* m_ODEBase4;
    cODEGenericBody* m_ODEBase5;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
