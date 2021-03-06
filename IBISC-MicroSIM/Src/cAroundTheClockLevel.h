#ifndef CAroundTheClock
#define CAroundTheClock

#include "cToolGripperLevel.h"
#include <CODE.h>

enum cNeedleSideMode
{
    NONE,
    END1,
    END2
};

class cAroundTheClockLevel : public cToolGripperLevel
{


public:
    cAroundTheClockLevel(const std::string a_resourceRoot,
        const int a_numDevices,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
        std::shared_ptr<cGenericHapticDevice> a_hapticDevice1,
        std::string NC);

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
    void SaveData(void);
    void SaveResults(void);
    void Start(void);
    void ResetSim(void);
    virtual void init();

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
    cODEGenericBody* ODENeedle;
    cODEGenericBody* ODENeedleNoRotationSupport;
    cODEGenericBody* ODERing[12];
    cMesh* object0;
    cMesh* objecttest;
    cMesh* object1[12];
    cMesh* stylusFingerBody[MAX_DEVICES];
    cMesh* stylusThumbBody[MAX_DEVICES];;
    cMesh* ground;
    cMesh* resetButton;
    cMesh* hapticPlane;
    // ODE objects
    cODEGenericBody* ODEGround;

    cCollisionRecorder recorder;
    cCollisionSettings settings;
    cMesh* DetectSphere[5];
    cMesh* CollisionSphereBack[8];
    cMesh* CollisionSphereFront[8];
    cMesh* DetectionPlanes[12];
    bool DetectionPlanesFinished[12];
    bool crossing[12];
    bool end1[12];
    bool end2[12];
    cNeedleSideMode needleFirstSide; //side of the needle that touches the circle first: false if end1, true if end2
    double sphereSize = 0.01;

    bool start;
    double timeInterval;
    double lastSave;
    double timerNum = 0.0;
    int handSwaps = 0;
    int grabNumber = 0;
    double timerHandSwaps = 0.0;
    int ringsCrossed = 0;
    bool firstCatch =true;
    //Sauvegarde
    std::tuple<double, cVector3d> posData[MAX_DEVICES];
    std::string NumCandidate;
    std::ofstream myfile[MAX_DEVICES];
    std::ofstream tempfile[MAX_DEVICES];
    std::string pathname;
    bool saved;
};

#endif

