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
    \version   3.2.0 $Rev: 2007 $
*/
//===========================================================================

//---------------------------------------------------------------------------
#include "chai3d.h"
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
//---------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//---------------------------------------------------------------------------
#include "CODE.h"
#include "Pick-and-Place.h"
#include "CGenericDemo.h"
#include "CDemo1.h"
//---------------------------------------------------------------------------

//------------------------------------------------------------------------------
// GENERAL SETTINGS AND VARIABLES FOR APP DLL
//------------------------------------------------------------------------------

//char stdOutFileName[31];
//char stdErrFileName[31];

bool m_debugConsole = false;
bool m_showGraphics = false;
bool m_debugGraphics = false;

// Serial Controller for angle adquisition
//SerialController* m_serialController;

//------------------------------------------------------------------------------
// GENERAL SETTINGS AND VARIABLES FOR WINDOW WITHOUT HMD
//------------------------------------------------------------------------------

// stereo Mode
/*
    C_STEREO_DISABLED:            Stereo is disabled
    C_STEREO_ACTIVE:              Active stereo for OpenGL NVDIA QUADRO cards
    C_STEREO_PASSIVE_LEFT_RIGHT:  Passive stereo where L/R images are rendered next to each other
    C_STEREO_PASSIVE_TOP_BOTTOM:  Passive stereo where L/R images are rendered above each other
*/
cStereoMode stereoMode = C_STEREO_DISABLED;

// fullscreen mode
bool fullscreen = false;

// mirrored display
bool mirroredDisplay = false;

// a handle to window display context
GLFWwindow* window = NULL;

// current width of window
int width = 0;

// current height of window
int height = 0;

// swap interval for the display context (vertical synchronization)
int swapInterval = 1;

//---------------------------------------------------------------------------
// CHAI3D 
//---------------------------------------------------------------------------

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
shared_ptr<cGenericHapticDevice> m_hapticDevice0;
shared_ptr<cGenericHapticDevice> m_hapticDevice1;

// flag to indicate if the haptic simulation currently running
bool simulationRunning = false;
bool simulationGraphicsRunning = false;

// flag to indicate if the haptic simulation has terminated
bool simulationFinished = false;
bool simulationGraphicsFinished = false;

// haptic thread
cThread* hapticsThread;

// default stiffness of scene objects
double maxStiffness = 1000.0;

//---------------------------------------------------------------------------
// DEMOS
//---------------------------------------------------------------------------

//! currently active camera
cGenericDemo* m_demo;

//! Demos
cDemo1* m_demo1;

// initialize demos
void initDemo1();

// callback when the window display is resized
void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

//===========================================================================
/*
    DEMO:    06-ODE-explorations.cpp

    This example illustrates the use of the ODE framework for simulating
    haptic interaction with dynamic bodies. 
 */
//===========================================================================

int InitializeApp(bool debugConsole, bool showGraphics, bool debugGraphics, int arduinoSerialPort, int arduinoBaudRate) {

    char stdOutFileName[31];
    char stdErrFileName[31];

    /*if (!debugConsole) {
        time_t rawtime;
        struct tm* timeinfo;

        time(&rawtime);
        timeinfo = localtime(&rawtime);
        strftime(stdOutFileName, 80, "logOut_VIRTSA_%Y-%m-%d_%H-%M", timeinfo);
        strftime(stdErrFileName, 80, "logErr_VIRTSA_%Y-%m-%d_%H-%M", timeinfo);

        freopen(stdOutFileName, "w", stdout);
        freopen(stdErrFileName, "w", stderr);
    }*/

    //cout << "VIRTSA --version " << PICK_AND_PLACE_MAYOR_VERSION << "." << PICK_AND_PLACE_MINOR_VERSION << "\n\tInitialization . . ." << endl;

    //cout << "\tSet serial comm with arduino" << endl;

    //m_serialController = new SerialController(arduinoSerialPort, arduinoBaudRate);

    return 0;
}

//---------------------------------------------------------------------------

int SetVirtualScene() {
    int res = SetUpDisplayAndHapticsContext();
    SetUpGameScene(res, maxStiffness);
    StartSimulation();

    return res;
}

//---------------------------------------------------------------------------

int SetUpDisplayAndHapticsContext() {

    //--------------------------------------------------------------------------
    // SETUP DISPLAY CONTEXT
    //--------------------------------------------------------------------------

    // initialize GLFW library
    if (!glfwInit())
    {
        cout << "failed initialization" << endl;
        cSleepMs(1000);
        return 0;
    }

    // set error callback
    glfwSetErrorCallback(errorCallback);

    // compute desired size of window
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int w = 0.8 * mode->height;
    int h = 0.8 * mode->height;
    int x = 0.5 * (mode->width - w);
    int y = 0.5 * (mode->height - h);

    // set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // create display context
    window = glfwCreateWindow(640, 480, "ViRTSA - Pick and Place", NULL, NULL);
    if (!window)
    {
        cout << "failed to create window" << endl;
        cSleepMs(1000);
        glfwTerminate();
        return 0;
    }

    // set key callback
    glfwSetKeyCallback(window, keyCallback);

    // set current display context
    glfwMakeContextCurrent(window);

    // sets the swap interval for the current display context
    glfwSwapInterval(0);

#ifdef GLEW_VERSION
    // initialize GLEW library
    if (glewInit() != GLEW_OK)
    {
        glfwTerminate();
        return 0;
    }
#endif

    // set active stereo mode
    if (stereoMode == C_STEREO_ACTIVE)
    {
        glfwWindowHint(GLFW_STEREO, GL_TRUE);
    }
    else
    {
        glfwWindowHint(GLFW_STEREO, GL_FALSE);
    }

    // get width and height of window
    glfwSetWindowSize(window, w, h);

    // get width and height of window
    glfwGetWindowSize(window, &width, &height);

    // set position of window
    glfwSetWindowPos(window, x, y);

    // set resize callback
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    // sets the swap interval for the current display context
    glfwSwapInterval(swapInterval);

    windowSizeCallback(window, width, height);

    //-----------------------------------------------------------------------
    // HAPTIC DEVICES 
    //-----------------------------------------------------------------------

    // create a haptic device handler
    handler = new cHapticDeviceHandler();

    // get number of haptic devices
    int numDevices = handler->getNumDevices();

    // default stiffness of scene objects
    maxStiffness = 1000.0;

    // get access to the haptic devices found
    if (numDevices > 0)
    {
        handler->getDevice(m_hapticDevice0, 0);
        maxStiffness = cMin(maxStiffness, 0.5 * m_hapticDevice0->getSpecifications().m_maxLinearStiffness);
    }

    if (numDevices > 1)
    {
        handler->getDevice(m_hapticDevice1, 1);
        maxStiffness = cMin(maxStiffness, 0.5 * m_hapticDevice1->getSpecifications().m_maxLinearStiffness);
    }

    return numDevices;
}

//---------------------------------------------------------------------------

void SetDownHapticDevices() {

    // stop the simulation
    simulationRunning = false;

    // wait for haptics loop to terminate
    while (!simulationFinished)
    {
        cSleepMs(100);
    }
}

void SetDownDisplayContext() {

    glfwDestroyWindow(window);

    // exit glfw
    glfwTerminate();
}

//---------------------------------------------------------------------------

void SetUpGameScene(int numDevices, double maxStiffness) {

    //-----------------------------------------------------------------------
    // GAME SCENES
    //-----------------------------------------------------------------------

    // parse first arg to try and locate resources
    //string resourceRoot = string(argv[0]).substr(0,string(argv[0]).find_last_of("/\\")+1);

    // setup demos
    m_demo1 = new cDemo1(numDevices, m_hapticDevice0, m_hapticDevice1);


    // set stereo mode
    m_demo1->m_camera->setStereoMode(stereoMode);

    // set object stiffness in demos
    m_demo1->setStiffness(maxStiffness);

    // initialize demo 1
    initDemo1();
}

//------------------------------------------------------------------------------

void StartSimulation() {
    //--------------------------------------------------------------------------
    // START SIMULATION
    //--------------------------------------------------------------------------

    // create a thread which starts the main haptics rendering loop
    hapticsThread = new cThread();
    hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);
   
    // setup callback when application exits
    //atexit(close);

    //--------------------------------------------------------------------------
    // MAIN GRAPHIC LOOP
    //--------------------------------------------------------------------------

    // main graphics rendering loop
    //if (m_showGraphics)
        updateGraphicsGeneral();
    //else
        //updateGraphicsVoid();
}

//---------------------------------------------------------------------------

void updateGraphicsGeneral(void)
{
    //--------------------------------------------------------------------------
    // MAIN GRAPHIC LOOP
    //--------------------------------------------------------------------------

    // call window size callback at initialization
    windowSizeCallback(window, width, height);

    while (!simulationGraphicsRunning)
    {
        Sleep(1000);
    };

    while (simulationGraphicsRunning)
    {
        // get width and height of window
        glfwGetWindowSize(window, &width, &height);

        // render graphics
        updateGraphics();

        // swap buffers
        glfwSwapBuffers(window);

        // process events
        glfwPollEvents();
    }

    // exit haptics thread
    simulationGraphicsFinished = true;
}

//---------------------------------------------------------------------------

void updateGraphicsVoid(void)
{
    while (!simulationGraphicsRunning)
    {
        Sleep(1000);
    };

    while (simulationGraphicsRunning)
    {
        Sleep(2000);
    };

    // exit
    simulationGraphicsFinished = true;
}

//---------------------------------------------------------------------------

int StartApp() {
    // simulation in now running
    simulationGraphicsRunning = true;

    return 0;
}

int finishApp() {
    cout << "SETDOWN HAPTIC DEVICES AND GRAPHICS CONTEXT" << endl;
    SetDownHapticDevices();
    cout << "Ok" << endl;

    simulationGraphicsRunning = false;

    // wait for graphics loop to terminate
    while (!simulationGraphicsFinished)
    {
        Sleep(100);
    }

    // tear down window
    //if (m_showGraphics)
        SetDownDisplayContext();

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished && !simulationGraphicsFinished)
    {
        Sleep(100);
    }

    // delete resources
    //delete hapticsThread;
    delete m_demo1;

    //delete m_serialController;
    delete handler;

    /*if (!m_debugConsole) {
        fclose(stdout);
        fclose(stderr);
    }*/

    return 0;
}

//---------------------------------------------------------------------------

int UpdateGameScenePosition(double x, double y, double z) {
    cVector3d newPos = cVector3d(x, y, z);
    m_demo->UpdateGameScenePosition(newPos);
    return 0;
}

int SetHapticForces(bool enable) {
    m_demo->SetHapticForces(enable);
    return 0;
}

int resetObjectsPositions() {
    m_demo->resetObjectsPositions();
    return 0;
}

int resetObjectPosition(int cube) {
    m_demo->resetObjectPosition(cube);
    return 0;
}

int getNumberOfTools() {
    return m_demo->getHapticDevicesCount();
}

int updateDynamics(int cube_id, bool enable)
{
    return m_demo->updateDynamics(cube_id, enable);
}

int GetRigidObjects(double outInformation[]) {
    return m_demo->GetRigidObjects(outInformation);
}

int GetInteractObjects(double outInformation[]) {
    return m_demo->GetInteractObjects(outInformation);
}

int GetHapticInformation(double outInformation[]) {
    return m_demo->GetHapticInformation(outInformation);
}

//---------------------------------------------------------------------------

void initDemo1()
{
    m_demo = m_demo1;
    m_demo->init();
}

//---------------------------------------------------------------------------

void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
    // update window size
    width  = a_width;
    height = a_height;
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char* a_description)
{
    cout << "Error: " << a_description << endl;
}

//---------------------------------------------------------------------------

void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{
    // filter calls that only include a key press
    if ((a_action != GLFW_PRESS) && (a_action != GLFW_REPEAT))
    {
        return;
    }
    // option - debug
    else if (a_key == GLFW_KEY_UP)
    {
        m_demo->updateCamera(8);
    }

    // option - debug
    else if (a_key == GLFW_KEY_DOWN)
    {
        m_demo->updateCamera(2);
    }

    // option - debug
    else if (a_key == GLFW_KEY_LEFT)
    {
        m_demo->updateCamera(4);
    }

    // option - debug
    else if (a_key == GLFW_KEY_RIGHT)
    {
        m_demo->updateCamera(6);
    }

    // option - debug
    else if (a_key == GLFW_KEY_A)
    {
        m_demo->debugTemp();
    }
}

//---------------------------------------------------------------------------

void updateGraphics(void)
{
    // render world
    m_demo->updateGraphics(width, height);

    // wait until all GL commands are completed
    glFinish();

    // check for any OpenGL errors
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) printf("Error:  %s\n", gluErrorString(err));
}

//---------------------------------------------------------------------------

void updateHaptics(void)
{
    // simulation in now running
    simulationRunning = true;
    //simulationFinished = false;

    // main haptic simulation loop
    while(simulationRunning)
    {
        m_demo->updateHaptics();
    }

    // shutdown haptic devices
    if (m_demo->m_tool0 != NULL)
    {
        m_demo->m_tool0->stop();
    }
    if (m_demo->m_tool1 != NULL)
    {
        m_demo->m_tool1->stop();
    }

    // exit haptics thread
    simulationFinished = true;
}
