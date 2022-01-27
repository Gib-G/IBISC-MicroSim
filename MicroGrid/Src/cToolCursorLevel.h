#ifndef cToolLevel
#define cToolLevel

#include "cGenericLevel.h"

enum cMode
{
    IDLE,
    SELECTION
};

const int MAX_DEVICES = 2;

class cToolCursorLevel : public cGenericLevel
{


public:
    cToolCursorLevel(const std::string a_resourceRoot,
        const int a_numDevices,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice1);

    virtual ~cToolCursorLevel() {};


public:
    virtual void close();

public:
    //! table of tools
    cToolCursor* m_tools[2];

    //! tool 0
    cToolCursor* m_tool0;

    //! tool 1
    cToolCursor* m_tool1;

    double toolRadius;
};

#endif

