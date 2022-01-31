#pragma once
#ifndef CHomeLevel  
#define CHomeLevel

#include "cToolCursorLevel.h"
#include "cLevelHandler.h"
#include "chai3d.h"
#include "cGridLevel.h"
#include "cAroundTheClockLevel.h"

class cHomeLevel : public cToolCursorLevel
{


public:
    cHomeLevel(const std::string a_resourceRoot,
        const int a_numDevices,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice1,
        cLevelHandler* levelHandler,
        std::string NC);

    virtual ~cHomeLevel() {};


public:
    virtual void moveCamera();
    virtual void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);
    virtual void updateGraphics();
    virtual void updateHaptics();


public:

    cLevelHandler* m_levelHandler;
    cGridLevel* m_grid;
    cAroundTheClockLevel* m_around;

    cMesh* gridLevelButton;
    cMesh* aroundLevelButton;
    
    cMode state[MAX_DEVICES];
    cGenericObject* selectedObject[MAX_DEVICES];
    cTransform world_T_object[MAX_DEVICES];
    cVector3d InitialPos[MAX_DEVICES];

    cTransform tool_T_object[MAX_DEVICES];
    cTransform tool_T_world[MAX_DEVICES];
    cTransform world_T_tool[MAX_DEVICES];
    cTransform parent_T_world[MAX_DEVICES];
    cTransform parent_T_object[MAX_DEVICES];
    double force[MAX_DEVICES];
    double size[MAX_DEVICES];
    double distance[MAX_DEVICES];

};

#endif

