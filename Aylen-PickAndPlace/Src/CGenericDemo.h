//===========================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2003-2016, CHAI3D
    (www.chai3d.org)

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:

    * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above
    copyright notice, this list of conditions and the following
    disclaimer in the documentation and/or other materials provided
    with the distribution.

    * Neither the name of CHAI3D nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
    LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
    LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
    ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE. 

    \author    <http://www.chai3d.org>
    \author    Francois Conti
    \version   3.2.0 $Rev: 1869 $
*/
//===========================================================================

//---------------------------------------------------------------------------
#ifndef CGenericDemoH
#define CGenericDemoH
//---------------------------------------------------------------------------
#include "chai3d.h"
#include "CODE.h"
using namespace chai3d;
//---------------------------------------------------------------------------

class cGenericDemo
{   
    //-----------------------------------------------------------------------
    // CONSTRUCTOR & DESTRUCTOR:
    //-----------------------------------------------------------------------

public:

    //! Constructor of cGenericDemo.
    cGenericDemo(const int a_numDevices, 
                 std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
                 std::shared_ptr<cGenericHapticDevice> a_hapticDevice);
                 //bool a_visualization = true);

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
    //! Set orbit camera angle step
    //virtual void setAngleStep(double a_angleStep);

    //! Set serialController
    //virtual void setSerialController(SerialController* a_serialController);

    // get tools endpoints proxy positions
    virtual void getPointsProxysPositions(double outPosArray[]);

    // get tools endpoints goal positions
    virtual void getPointsGoalsPositions(double outPosArray[]);

    // get tools positions
    virtual void getDevicesPositions(double outPosArray[]);

    //! Update haptics
    virtual void updateCamera(int dest);

    virtual void debugTemp() {};

    virtual int GetRigidObjects(double outInformation[]) { return 0; };

    virtual int GetInteractObjects(double outInformation[]) { return 0; };

    virtual void UpdateGameScenePosition(cVector3d a_pos);

    virtual void SetHapticForces(bool enable);

    virtual void resetObjectsPositions() {};

    virtual void resetObjectPosition(int c) {};

    virtual int getHapticDevicesCount() { return m_numTools; };

    virtual int GetHapticInformation(double outInformation[]) { return 0; };

    virtual int updateDynamics(int a_cube, bool a_state);

    virtual int updateDynamicsCube(int a_cube, bool a_state) { return 0; };

    virtual bool isObjectAvailableForMovement(cODEGenericBody* body) { return 0; };

    //-----------------------------------------------------------------------
    // PUBLIC MEMBERS:
    //-----------------------------------------------------------------------

public:

    //! virtual world
    cWorld* m_world;

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

    //! table //
    //cMesh* m_ground;

    //! base //
    //cMultiMesh* m_base;

    //! simulation clock
    cPrecisionClock simClock;

    //! number of tools
    int m_numTools;

    //! table of tools
    cToolGripper* m_tools[2];

    //! ODE world
    cODEWorld* m_ODEWorld;

    //! ODE planes //
    /*cODEGenericBody* m_ODEGPlane0;
    cODEGenericBody* m_ODEGPlane1;
    cODEGenericBody* m_ODEGPlane2;
    cODEGenericBody* m_ODEGPlane3;
    cODEGenericBody* m_ODEGPlane4;
    cODEGenericBody* m_ODEGPlane5;*/

    //! torque gain
    double m_torqueGain;

    //! mirroed display
    bool m_mirroredDisplay;


    //! ODE plane
    cODEGenericBody* m_ODEGPlane;

    //! offset tool 0
    cGenericObject* axis_tool0;

    //! offset tool 1
    cGenericObject* axis_tool1;

    //! virtual scene position
    cGenericObject* m_sceneAxis;

    //! virtual table
    cMesh* m_workingSpace;

    //! virtual scanner table
    cMesh* m_platformSpace;

    //! serial controller
    //SerialController* m_serialController;

    //! camera orbit angle step 
    double m_angleStep;

    //! camera orbit axes
    cGenericObject* cameraOrbitAxisZ;
    cGenericObject* cameraOrbitAxisY;

    //! flag to know if graphical visualization is enabled
    bool m_visualization;

    // forceps angles from arduino
    //double valuesRaw[TOOLS_NUMBER];
    //double anglesCalculated[TOOLS_NUMBER];

    bool m_updateCubes = false;
    bool m_state = false;
    int m_cube = 0;

    //-----------------------------------------------------------------------
    // PRIVATE MEMBERS:
    //-----------------------------------------------------------------------

private:

    //! serial information constants to calculate tools gripper angle
    const double MAX_ABSOLUT_OPENING = 33;
    const double INITIAL_OPENING_LEFT = 2.456;
    const double INITIAL_OPENING_RIGHT = 2.456;
    const double DATA_RESOLUTION = 1000;

    // a label to display the rate [Hz] at which the simulation is running
    cLabel* labelRate;

    // a frequency counter to measure the simulation haptic rate
    cFrequencyCounter freqCounterHaptics;

};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
