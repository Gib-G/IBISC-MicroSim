#ifndef CGenericDemoH
#define CGenericDemoH
//---------------------------------------------------------------------------
#include <chai3d.h>
#include <CBullet.h>
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

    //! Update graphics
    virtual void updateGraphics(int a_width, int a_height);

    //! Set stiffness
    virtual void setStiffness(double a_stiffness){};

    //! Set offset
    virtual void setOffset(double a_offset);

    //! Set torque gain
    virtual void setTorqueGain(double a_torqueGain);


    //-----------------------------------------------------------------------
    // PUBLIC MEMBERS:
    //-----------------------------------------------------------------------

public:

    //! bullet world
    cBulletWorld* m_bulletWorld;

    //! camera
    cCamera* m_camera;

    //! light source 0
    cSpotLight *m_light0;

    //! light source 1
    cDirectionalLight *m_light1;

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

    //! bullet planes
    cBulletStaticPlane* m_bulletInvisibleWall0;
    cBulletStaticPlane* m_bulletInvisibleWall1;
    cBulletStaticPlane* m_bulletInvisibleWall2;
    cBulletStaticPlane* m_bulletInvisibleWall3;
    cBulletStaticPlane* m_bulletInvisibleWall4;
    cBulletStaticPlane* m_bulletInvisibleWall5;

    //! torque gain
    double m_torqueGain;

    //! mirroed display
    bool m_mirroredDisplay;
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
