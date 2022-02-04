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
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice1,
        std::string NC);

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
    void RotateCanvas();
    void ResetSim(int pattern);
    void SaveData();
    virtual void init();


public:
    cMultiMesh* Objects;
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
    cMesh* rotateButton;
    cMesh* stylus[MAX_DEVICES];

    cVector3d canvasPos;
    // copy of blank canvas texture
    cImagePtr canvasOriginal;
    cImagePtr canvasTraining;
    cImagePtr customPattern;

    // selected paint color
    cColorb paintColor;
    cColorb errorColor;
    cColorb warningColor;

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
    bool resetHit;
    bool pressed[MAX_DEVICES];
    bool touching[MAX_DEVICES];

    int pattern;
    float cubesize; 
    int numCube;

    double K_SIZE;
    int BRUSH_SIZE;

    std::tuple<float, cVector3d> posData[MAX_DEVICES];
    float errorPixel;
    float totalColoredPixels;
    float errorPercent = 0;
    float greenPixels;
    float goalPixels;
    float forcePixels;
    float correctPercent = 0;
    float forcePercent = 0;

    int MAX_PATTERN = 2;
    float MAX_FORCE = .8;
    float lastSave = 0.0f;

    std::string m_resourceRoot;

    int rotation;

    std::string NumCandidate;
    std::ofstream myfile[MAX_DEVICES];
    std::ofstream tempfile[MAX_DEVICES];
    std::string pathname;
};

#endif

