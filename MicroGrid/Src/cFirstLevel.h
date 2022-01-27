#ifndef CFirstLevel
#define CFirstLevel

#include "cGenericLevel.h"
#include "cLevelHandler.h"
#include "chai3d.h"


class cFirstLevel : public cGenericLevel
{


public:
    cFirstLevel(const std::string a_resourceRoot,
        const int a_numDevices,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice1,
        cLevelHandler* a_levelHandler);

    virtual ~cFirstLevel() {};


public:
    virtual void moveCamera();
    virtual void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);
    virtual void updateGraphics();
    virtual void updateHaptics();


public:
    cGenericLevel* m_second;
    cLevelHandler* m_levelHandler;
    //! table of tools
    cToolGripper* m_tools[2];

    //! tool 0
    cToolGripper* m_tool0;

    //! tool 1
    cToolGripper* m_tool1;
};

#endif

