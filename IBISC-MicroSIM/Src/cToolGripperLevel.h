#ifndef CToolGripperLevel
#define CToolGripperLevel

#include "cGenericLevel.h"

class cToolGripperLevel : public cGenericLevel
{


public:
    cToolGripperLevel(const std::string a_resourceRoot,
        const int a_numDevices,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice1);

    virtual ~cToolGripperLevel() {};


public:
    virtual void close();

public:
    //! table of tools
    cToolGripper* m_tools[2];

    //! tool 0
    cToolGripper* m_tool0;

    //! tool 1
    cToolGripper* m_tool1;

    double toolRadius;
};

#endif

