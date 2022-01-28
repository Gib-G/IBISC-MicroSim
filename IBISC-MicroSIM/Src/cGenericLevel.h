
//---------------------------------------------------------------------------
#ifndef CGenericLevelH
#define CGenericLevelH
//---------------------------------------------------------------------------
#include "chai3d.h"
#include <GLFW/glfw3.h>
using namespace chai3d;
//---------------------------------------------------------------------------

const int MAX_DEVICES = 2;

class cGenericLevel
{
    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

public:

    //! Constructor of cGenericLevel.
    cGenericLevel(const std::string a_resourceRoot,
        const int a_numDevices,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice);

    //! Destructor of cGenericLevel.
    virtual ~cGenericLevel() {};


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

    //! Set torque gain
    virtual void setTorqueGain(double a_torqueGain);

    virtual void moveCamera() {};

    virtual void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods) {};

    virtual void updateGraphics() {};

    virtual void close() {};


    //-----------------------------------------------------------------------
    // PUBLIC MEMBERS:
    //-----------------------------------------------------------------------

public:

    //! virtual world
    cWorld* m_world;

    cSpotLight* light;

    //! camera
    cCamera* m_camera;

    //! light source 0
    cSpotLight* m_light0;

    //! light source 1
    cDirectionalLight* m_light1;

    //! radius of tools
    double m_toolRadius;

    //! table 
    cMesh* m_ground;

    //! base
    cMultiMesh* m_base;

    //! simulation clock
    cPrecisionClock simClock;

    //! number of tools
    int m_numTools;

    //! torque gain
    double m_torqueGain;

    //! mirroed display
    bool m_mirroredDisplay;

    cVector3d movementVector;

    int pressedKey;

    double moveSpeed;

    double deltaTime;
    double currentFrame;
    double lastFrame;

    cVector3d defaultPos;

};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
