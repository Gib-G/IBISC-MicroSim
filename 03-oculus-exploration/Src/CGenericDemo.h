#ifndef CGenericDemoH
#define CGenericDemoH
//---------------------------------------------------------------------------
#include <chai3d.h>
#include <CODE.h>

using namespace chai3d;
//---------------------------------------------------------------------------

class cGenericDemo
{
    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

public:

    //! Constructor of cGenericDemo.
    cGenericDemo(const std::string a_resourceRoot,
        const int a_numDevices,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice);

    //! Destructor of cGenericDemo.
    virtual ~cGenericDemo() {};


    //-----------------------------------------------------------------------
    // PUBLIC METHODS:
    //-----------------------------------------------------------------------

public:

    //! Initialize to demo
    virtual void init() {};

    //! Update haptics
    virtual void updateHaptics();

    //! Set stiffness
    virtual void setStiffness(double a_stiffness) {};

    //! Set offset
    virtual void setOffset(double a_offset);

    //! Set torque gain
    virtual void setTorqueGain(double a_torqueGain);


    //-----------------------------------------------------------------------
    // PUBLIC MEMBERS:
    //-----------------------------------------------------------------------

public:

    //! virtual world
    cWorld* m_world;

    //! camera
    cCamera* m_camera;

    //! light source 0
    cSpotLight* m_light0;

    //! light source 1
    cDirectionalLight* m_light1;

    //! radius of tools
    double m_toolRadius;

    //! tool 0
    cToolGripper* m_tool0;

    //! tool 1
    cToolGripper* m_tool1;

    //! table 
    cMesh* m_ground;

    //! base
    cMultiMesh* m_base;

    //! simulation clock
    cPrecisionClock simClock;

    //! number of tools
    int m_numTools;

    //! table of tools
    cToolGripper* m_tools[2];

    //! ODE world
    cODEWorld* m_ODEWorld;

    //! ODE planes
    cODEGenericBody* m_ODEGPlane0;
    cODEGenericBody* m_ODEGPlane1;
    cODEGenericBody* m_ODEGPlane2;
    cODEGenericBody* m_ODEGPlane3;
    cODEGenericBody* m_ODEGPlane4;
    cODEGenericBody* m_ODEGPlane5;

    //! torque gain
    double m_torqueGain;

    //! mirroed display
    bool m_mirroredDisplay;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
