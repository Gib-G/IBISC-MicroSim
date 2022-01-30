#ifndef CAroundTheClock
#define CAroundTheClock

#include "cToolGripperLevel.h"
#include <CODE.h>

class cAroundTheClockLevel : public cToolGripperLevel
{


public:
    cAroundTheClockLevel(const std::string a_resourceRoot,
        const int a_numDevices,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice1);

    virtual ~cAroundTheClockLevel() {};


public:
    virtual void moveCamera();
    virtual void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);
    virtual void updateGraphics();
    virtual void updateHaptics();
    void InitializeNeedleDetect(void);
    void AddDetectionPlane(int i, cODEGenericBody* ring);
    void ComputeCrossing(cMesh* spheres[], int i);
    cVector3d toAxisAngleVec(cMatrix3d m);
    double toAxisAngleAngle(cMatrix3d m);

public:
    bool previousframecaught[MAX_DEVICES];
    int gripperCatchingIndex = -1;
    cMatrix3d startrotGripper[MAX_DEVICES];
    cVector3d startposGripper[MAX_DEVICES];
    cVector3d startposCube;
    cMatrix3d startrotCube;

    //---------------------------------------------------------------------------
    // ODE MODULE VARIABLES
    //---------------------------------------------------------------------------

    // ODE world
    cODEWorld* ODEWorld;

    // ODE objects
    cODEGenericBody* ODEBody0;
    cODEGenericBody* ODEBodytest;
    cODEGenericBody* ODEBody1[12];
    cODEGenericBody* ODEBody2[MAX_DEVICES];
    cODEGenericBody* ODEBody3[MAX_DEVICES];;
    cMesh* object0;
    cMesh* objecttest;
    cMesh* object1[12];
    cMesh* object2[MAX_DEVICES];
    cMesh* object3[MAX_DEVICES];;
    cMesh* ground;
    // ODE objects
    cODEGenericBody* ODEGPlane0;
    cODEGenericBody* ODEGPlane1;
    cODEGenericBody* ODEGPlane2;
    cODEGenericBody* ODEGPlane3;
    cODEGenericBody* ODEGPlane4;
    cODEGenericBody* ODEGPlane5;

    cCollisionRecorder recorder;
    cCollisionSettings settings;
    cMesh* DetectSphere[5];
    cMesh* DetectionPlanes[12];
    bool DetectionPlanesFinished[12];
    cMesh* testPlane;
    bool crossing[12];
    bool end1[12];
    bool end2[12];
    float sphereSize = 0.01;

    double timeInterval;
};

#endif

