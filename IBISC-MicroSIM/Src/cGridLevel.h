#pragma once
#ifndef CGridLevel  
#define CGridLevel

#include "cToolCursorLevel.h"
#include "chai3d.h"
#include <iostream>
#include <fstream>


class cGridLevel : public cToolCursorLevel
{


public:
    cGridLevel(const std::string a_resourceRoot,
        const int a_numDevices,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice1);

    virtual ~cGridLevel() {};


public:
    virtual void moveCamera();
    virtual void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);
    virtual void updateGraphics();
    virtual void updateHaptics();
    bool PaintCanvas(int x, int y, int pattern);
    void ResetCanvas(int pattern);
    bool CompareVectors(cVector3d v1, cVector3d v2);
    void GetResult();
    void DisplayTimer(float time);
    void ChangePattern();
    void Start();
    void SaveCanvas();


public:
    cMesh* canvas;
    cMesh* board;
    cMesh* timer1, * timer2, * timer3, * timer4;
    cMultiMesh* timer;
    cMesh* plusButton;
    cMesh* minusButton;
    cMesh* resetButton;
    cMesh* saveButton;
    cMesh* startButton;
    cMesh* changeButton;
    // copy of blank canvas texture
    cImagePtr canvasOriginal;

    // selected paint color
    cColorb paintColor;
    cColorb errorColor;

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

    float timerNum;
    cFrequencyCounter frequencyCounter;
    bool start;
    bool pressed[MAX_DEVICES];
    bool touching[MAX_DEVICES];

    int pattern;
    float cubesize; 
    int numCube;

    double K_SIZE;
    int BRUSH_SIZE;

    std::map<float, cVector3d> posData[MAX_DEVICES];
    float errorPixel;
    float totalColoredPixels;
    float errorPercent = 0;
    float greenPixels;
    float goalPixels;
    float correctPercent = 0;

    int MAX_PATTERN = 1;

    std::string m_resourceRoot;


};

#endif
