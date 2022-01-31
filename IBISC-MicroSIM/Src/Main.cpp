//==============================================================================
/*
    Software License Agreement (BSD License)
    Copyright (c) 2003-2016, CHAI3D.
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
    \version   3.2.0 $Rev: 1659 $
*/
//==============================================================================

//------------------------------------------------------------------------------
#include "chai3d.h"
//------------------------------------------------------------------------------
#include <GLFW/glfw3.h>
//------------------------------------------------------------------------------
#include "COculus.h"
//------------------------------------------------------------------------------

#include "cGenericLevel.h"
#include "cFirstLevel.h"
#include "cLevelHandler.h"
#include "cGridLevel.h"
#include "cHomeLevel.h"
#include "cAroundTheClockLevel.h"
//------------------------------------------------------------------------------
using namespace chai3d;
using namespace std;
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// CHAI3D
//------------------------------------------------------------------------------

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
shared_ptr<cGenericHapticDevice> m_hapticDevice0;
shared_ptr<cGenericHapticDevice> m_hapticDevice1;

// flag to indicate if the haptic simulation currently running
bool simulationRunning = false;

// flag to indicate if the haptic simulation has terminated
bool simulationFinished = false;

bool camSim = false;

// haptic thread
cThread* hapticsThread;

//Arduino Thread
cThread* ArduinoThread;

// a handle to window display context
GLFWwindow* window = NULL;

// current width of window
int width = 0;

// current height of window
int height = 0;

//STATES
//------------------------------------------------------------------------------
enum MouseStates
{
    MOUSE_IDLE,
    MOUSE_MOVE_CAMERA
};

// mouse state
MouseStates mouseState = MOUSE_IDLE;

// last mouse position
double mouseX, mouseY;

string NumCandidate;

int swapInterval = 1;
bool fullscreen = false;
bool arduino = false;


//---------------------------------------------------------------------------
// DEMOS
//---------------------------------------------------------------------------

//! currently active camera

cLevelHandler* levelHandler;

cFirstLevel* m_first;

cGridLevel* m_grid;

cHomeLevel* m_home;

cAroundTheClockLevel* m_around;

//------------------------------------------------------------------------------
// OCULUS RIFT
//------------------------------------------------------------------------------

// display context
cOVRRenderContext renderContext;

// oculus device
cOVRDevice oculusVR;

char com_port[] = "\\\\.\\COM ";

//------------------------------------------------------------------------------
// DECLARED MACROS
//------------------------------------------------------------------------------

// convert to resource path
#define RESOURCE_PATH(p)    (char*)((resourceRoot+string(p)).c_str())


//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// function that closes the application
void close(void);

// main haptics simulation loop
void updateHaptics(void);

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

// callback to handle mouse click
void mouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods);

// callback to handle mouse motion
void mouseMotionCallback(GLFWwindow* a_window, double a_posX, double a_posY);

void ZoomCam(void);
void UpdatePreferences(string num);
char* FetchPreferences(void);

void ReadPort();



//==============================================================================
/*
    DEMO:   03-exploration.cpp

    This example illustrates how to use ODE to create a environment which
    contains dynamic objects.
*/
//==============================================================================

int main(int argc, char** argv)
{

    //--------------------------------------------------------------------------
    // INITIALIZATION
    //--------------------------------------------------------------------------

    cout << endl;
    cout << "-----------------------------------" << endl;
    cout << "CHAI3D" << endl;
    cout << "Demo: 03-exploration" << endl;
    cout << "Copyright 2003-2016" << endl;
    cout << "-----------------------------------" << endl << endl << endl;
    cout << "Keyboard Options:" << endl << endl;
    cout << "[ ] - Recenter view point" << endl;
    cout << "[1] - Select Demo 1" << endl;
    cout << "[2] - Select Demo 2" << endl;
    cout << "[3] - Select Demo 3" << endl;
    cout << "[4] - Select Demo 4" << endl;
    cout << "[q] - Exit application" << endl;
    cout << endl << endl;

    // parse first arg to try and locate resources
    string resourceRoot = string(argv[0]).substr(0, string(argv[0]).find_last_of("/\\") + 1) + "../../../";

    cout << resourceRoot << endl;

    cStereoMode stereoMode = C_STEREO_DISABLED;

    cout << "Type the ID of the candidate and press Enter to submit it." << endl;
    cout << "Submitting no ID launches training mode" << endl;
    cout << "Candidate ID : ";
    getline(cin, NumCandidate);
    cout << endl;

    //--------------------------------------------------------------------------
    // SETUP DISPLAY CONTEXT
    //--------------------------------------------------------------------------

    // initialize GLFW library
    if (!glfwInit())
    {
        cout << "failed initialization" << endl;
        cSleepMs(1000);
        return 1;
    }

    // set error callback
    glfwSetErrorCallback(errorCallback);

    // compute desired size of window
    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    int w = 0.8 * mode->height;
    int h = 0.5 * mode->height;
    int x = 0.5 * (mode->width - w);
    int y = 0.5 * (mode->height - h);


    // set OpenGL version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    // set active stereo mode
    if (stereoMode == C_STEREO_ACTIVE)
    {
        glfwWindowHint(GLFW_STEREO, GL_TRUE);
    }
    else
    {
        glfwWindowHint(GLFW_STEREO, GL_FALSE);
    }

    // create display context
    window = glfwCreateWindow(w, h, "CHAI3D", NULL, NULL);
    if (!window)
    {
        cout << "failed to create window" << endl;
        cSleepMs(1000);
        glfwTerminate();
        return 1;
    }

    // get width and height of window
    glfwGetWindowSize(window, &width, &height);

    // set position of window
    glfwSetWindowPos(window, x, y);

    // set key callback
    glfwSetKeyCallback(window, keyCallback);

    // set mouse position callback
    glfwSetCursorPosCallback(window, mouseMotionCallback);

    // set mouse button callback
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    // set current display context
    glfwMakeContextCurrent(window);

    // sets the swap interval for the current display context
    glfwSwapInterval(0);

#ifdef GLEW_VERSION
    // initialize GLEW library
    if (glewInit() != GLEW_OK)
    {
        oculusVR.destroyVR();
        renderContext.destroy();
        glfwTerminate();
        return 1;
    }
#endif

    // initialize oculus
    if (!oculusVR.initVR())
    {
        cout << "failed to initialize Oculus" << endl;
        camSim = true;
    }

    // get oculus display resolution
    ovrSizei hmdResolution = oculusVR.getResolution();

    // setup mirror display on computer screen

    ovrSizei windowSize;

    if (!camSim) {
        // setup mirror display on computer screen
        windowSize = { hmdResolution.w / 2, hmdResolution.h / 2 };
    }
    else {
        windowSize = { width,height };
    }

    // inialize buffers
    if (!oculusVR.initVRBuffers(windowSize.w, windowSize.h) && !camSim)
    {
        cout << "failed to initialize Oculus buffers" << endl;
        cSleepMs(1000);
        oculusVR.destroyVR();
        renderContext.destroy();
        glfwTerminate();
        return 1;
    }

    // set window size
    glfwSetWindowSize(window, windowSize.w, windowSize.h);


    //-----------------------------------------------------------------------
    // HAPTIC DEVICES 
    //-----------------------------------------------------------------------

    // create a haptic device handler
    handler = new cHapticDeviceHandler();

    // get number of haptic devices
    int numDevices = handler->getNumDevices();

    cout << "numDevices:" << numDevices << endl;

    // default stiffness of scene objects
    double maxStiffness = 1000.0;

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


    //-----------------------------------------------------------------------
    // DEMOS
    //-----------------------------------------------------------------------
    levelHandler = new cLevelHandler(&oculusVR);
    //m_grid = new cGridLevel(resourceRoot, numDevices, m_hapticDevice0, m_hapticDevice1,NumCandidate);
    m_home = new cHomeLevel(resourceRoot, numDevices, m_hapticDevice0, m_hapticDevice1, levelHandler,NumCandidate);
    //m_around = new cAroundTheClockLevel(resourceRoot, numDevices, m_hapticDevice0, m_hapticDevice1,NumCandidate);
    levelHandler->setLevel(m_home);

    //--------------------------------------------------------------------------
    // START SIMULATION
    //--------------------------------------------------------------------------

    // create a thread which starts the main haptics rendering loop
    hapticsThread = new cThread();
    hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

    ArduinoThread = new cThread();
    ArduinoThread->start(ReadPort, CTHREAD_PRIORITY_GRAPHICS);

    // setup callback when application exits
    atexit(close);


    //--------------------------------------------------------------------------
    // MAIN GRAPHIC LOOP
    //--------------------------------------------------------------------------

    // recenter oculus
    oculusVR.recenterPose();

    // main graphic rendering loop
    while (!glfwWindowShouldClose(window) && !simulationFinished)
    {
        glfwGetFramebufferSize(window, &width, &height);

        // update shadow maps (if any)
        levelHandler->mainLevel->m_world->updateShadowMaps(false, false);


        if (!camSim) {
            // start rendering
            oculusVR.onRenderStart();

            // render frame for each eye
            for (int eyeIndex = 0; eyeIndex < ovrEye_Count; eyeIndex++)
            {
                // retrieve projection and modelview matrix from oculus
                cTransform projectionMatrix, modelViewMatrix;
                oculusVR.onEyeRender(eyeIndex, projectionMatrix, modelViewMatrix);

                levelHandler->mainLevel->m_camera->m_useCustomProjectionMatrix = true;
                levelHandler->mainLevel->m_camera->m_projectionMatrix = projectionMatrix;

                levelHandler->mainLevel->m_camera->m_useCustomModelViewMatrix = true;
                levelHandler->mainLevel->m_camera->m_modelViewMatrix = modelViewMatrix;

                if (arduino)levelHandler->mainLevel->ZoomCamera();
                // render world
                ovrSizei size = oculusVR.getEyeTextureSize(eyeIndex);
                levelHandler->mainLevel->m_camera->renderView(size.w, size.h, C_STEREO_LEFT_EYE, false);

                // finalize rendering  
                oculusVR.onEyeRenderFinish(eyeIndex);
            }

            // update frames
            oculusVR.submitFrame();
            oculusVR.blitMirror();
        }
        else {
            //cout << width << " " << height << endl;
            if (arduino)levelHandler->mainLevel->ZoomCamera();
            levelHandler->mainLevel->m_camera->renderView(width, height);
            levelHandler->mainLevel->updateGraphics();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // cleanup
    oculusVR.destroyVR();
    renderContext.destroy();

    glfwDestroyWindow(window);

    // exit glfw
    glfwTerminate();

    return (0);
}

//------------------------------------------------------------------------------

void errorCallback(int a_error, const char* a_description)
{
    cout << "Error: " << a_description << endl;
}

//------------------------------------------------------------------------------

void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods)
{

    // option - exit
    if (a_key == GLFW_KEY_Q)
    {
        glfwSetWindowShouldClose(a_window, GLFW_TRUE);

    }
    else if (a_key == GLFW_KEY_ESCAPE) {
        levelHandler->setLevel(m_home);
    }

    // option - spacebar
    else if (a_key == GLFW_KEY_ENTER)
    {
        levelHandler->mainLevel->m_camera->setLocalPos(levelHandler->mainLevel->defaultPos);
        oculusVR.recenterPose();

    }
    else if (a_key == GLFW_KEY_F && a_action == GLFW_PRESS)
    {
        // toggle state variable
        fullscreen = !fullscreen;

        // get handle to monitor
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();

        // get information about monitor
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        // set fullscreen or window mode
        if (fullscreen)
        {
            glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            glfwSwapInterval(swapInterval);
        }
        else
        {
            int w = 0.8 * mode->height;
            int h = 0.5 * mode->height;
            int x = 0.5 * (mode->width - w);
            int y = 0.5 * (mode->height - h);
            glfwSetWindowMonitor(window, NULL, x, y, w, h, mode->refreshRate);
            glfwSwapInterval(swapInterval);
        }
    }

    levelHandler->mainLevel->keyCallback(a_window, a_key, a_scancode, a_action, a_mods);


}

void close(void)
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

    // delete resources
    delete hapticsThread;
    delete ArduinoThread;
    delete levelHandler;
    delete m_first;
    delete m_grid;
    delete m_home;
    delete handler;
}

//------------------------------------------------------------------------------

void updateHaptics(void)
{
    // simulation in now running
    simulationRunning = true;


    //------------------------------------------------------------------------------

        // main haptic simulation loop
    while (simulationRunning)
    {
        levelHandler->mainLevel->updateHaptics();
    }

    levelHandler->mainLevel->close();

    // exit haptics thread
    simulationFinished = true;
}

void UpdatePreferences(string ComPort) {
    std::stringstream temp;
    std::ofstream UserInfo;
    UserInfo.open(ROOT_DIR "Resources/CSV/Temp/UserInfo.txt");
    UserInfo << ComPort << endl;
    UserInfo << NumCandidate << endl;
    UserInfo.close();
}
char* FetchPreferences() {
    std::ifstream UserInfo;
    std::string comport;
    UserInfo.open(ROOT_DIR "Resources/CSV/Temp/UserInfo.txt");
    getline(UserInfo, comport);
    //getline(UserInfo, NumCandidate);
    UserInfo.close();
    for (int i = 0; i < comport.length(); i++) {
        com_port[7 + i] = comport[i];
    }
    for (int j = comport.length() + 7; j < 9; j++) {
        com_port[j] = '\0';
    }
    return com_port;
}

void ReadPort() {
    int reply_wait_time = 1;
    string incoming;
    int nb_info = 2;
    DWORD COM_BAUD_RATE = CBR_19200;
    SimpleSerial Serial(FetchPreferences(), COM_BAUD_RATE);
    if (!Serial.connected_) {
        string num;
        cout << "Current Arduino Port : ";
        getline(cin, num);
        if (num != "") {
            UpdatePreferences(num);
        }
        SimpleSerial Serial(FetchPreferences(), COM_BAUD_RATE);
        if (Serial.connected_) {
            cout << "Zoom through Arduino enabled" << endl;
        }
        else {
            cout << "No Arduino detected" << endl;
        }
    }
    else {
        cout << "Zoom through Arduino enabled" << endl;
    }
    while (!simulationFinished && Serial.connected_) {
        arduino = true;
        incoming = Serial.ReadSerialPort(reply_wait_time);
        if (incoming.length() > 0) {
            if (incoming == "1") {
                levelHandler->mainLevel->Zoom_Out = 0;
                levelHandler->mainLevel->Zoom_In = 1;
            }
            else if (incoming == "2") {
                levelHandler->mainLevel->Zoom_In = 0;
                levelHandler->mainLevel->Zoom_Out = 1;
            }
            else {
                levelHandler->mainLevel->Zoom_Out = 1;
                levelHandler->mainLevel->Zoom_In = 1;
            }
        }
    }
    if (Serial.connected_) Serial.CloseSerialPort();
}

//------------------------------------------------------------------------------

void mouseButtonCallback(GLFWwindow* a_window, int a_button, int a_action, int a_mods)
{
    if (a_button == GLFW_MOUSE_BUTTON_RIGHT && a_action == GLFW_PRESS)
    {
        // store mouse position
        glfwGetCursorPos(window, &mouseX, &mouseY);

        // update mouse state
        mouseState = MOUSE_MOVE_CAMERA;
    }

    else
    {
        // update mouse state
        mouseState = MOUSE_IDLE;
    }
}

//------------------------------------------------------------------------------

void mouseMotionCallback(GLFWwindow* a_window, double a_posX, double a_posY)
{
    if (mouseState == MOUSE_MOVE_CAMERA)
    {
        // compute mouse motion
        int dx = a_posX - mouseX;
        int dy = a_posY - mouseY;
        mouseX = a_posX;
        mouseY = a_posY;
        cVector3d cameraPos = levelHandler->mainLevel->m_camera->getLocalPos();


        // compute new camera angles
        double azimuthDeg = levelHandler->mainLevel->m_camera->getSphericalAzimuthDeg() - 0.5 * dx / 3;
        double polarDeg = levelHandler->mainLevel->m_camera->getSphericalPolarDeg() - 0.5 * dy / 3;

        // assign new angles
        levelHandler->mainLevel->m_camera->setSphericalAzimuthDeg(azimuthDeg);
        levelHandler->mainLevel->m_camera->setSphericalPolarDeg(polarDeg);

        levelHandler->mainLevel->m_camera->setLocalPos(cameraPos);

    }
}