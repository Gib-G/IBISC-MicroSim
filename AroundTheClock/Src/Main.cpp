#include <chai3d.h>
//---------------------------------------------------------------------------
#include <GLFW/glfw3.h>
//---------------------------------------------------------------------------
using namespace chai3d;
using namespace std; // Pas bien ça... :(
//---------------------------------------------------------------------------
#include <CODE.h>
#include <stdlib.h>
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// GENERAL SETTINGS
//---------------------------------------------------------------------------

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


//---------------------------------------------------------------------------
// CHAI3D VARIABLES
//---------------------------------------------------------------------------

// a world that contains all objects of the virtual environment
cWorld* world;

// a camera to render the world in the window display
cCamera* camera;

// a light source to illuminate the objects in the world
cSpotLight* light;

// a haptic device handler
cHapticDeviceHandler* handler;

// a pointer to the current haptic device
shared_ptr<cGenericHapticDevice> hapticDevice;

// a virtual tool representing the haptic device in the scene
cToolGripper* tool;

// a label to display the rate [Hz] at which the simulation is running
cLabel* labelRates;

bool previousframecaught;
cMatrix3d startrotGripper;
cVector3d startposGripper;
cVector3d startposCube;
cMatrix3d startrotCube;

//---------------------------------------------------------------------------
// ODE MODULE VARIABLES
//---------------------------------------------------------------------------

// ODE world
cODEWorld* ODEWorld;

// ODE objects
cODEGenericBody* ODEBody0;
cODEGenericBody* ODEBody1;
cODEGenericBody* ODEBody2;
cMesh* object0;
cMesh* object1;
cMultiMesh* object2;
// ODE objects
cODEGenericBody* ODEGPlane0;
cODEGenericBody* ODEGPlane1;
cODEGenericBody* ODEGPlane2;
cODEGenericBody* ODEGPlane3;
cODEGenericBody* ODEGPlane4;
cODEGenericBody* ODEGPlane5;


//---------------------------------------------------------------------------
// GENERAL VARIABLES
//---------------------------------------------------------------------------

// flag to indicate if the haptic simulation currently running
bool simulationRunning = false;

// flag to indicate if the haptic simulation has terminated
bool simulationFinished = true;

// a frequency counter to measure the simulation graphic rate
cFrequencyCounter freqCounterGraphics;

// a frequency counter to measure the simulation haptic rate
cFrequencyCounter freqCounterHaptics;

// haptic thread
cThread* hapticsThread;

// a handle to window display context
GLFWwindow* window = NULL;

// current width of window
int width = 0;

// current height of window
int height = 0;

// swap interval for the display context (vertical synchronization)
int swapInterval = 1;

//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// callback when the window display is resized
void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height);

// callback when an error GLFW occurs
void errorCallback(int error, const char* a_description);

// callback when a key is pressed
void keyCallback(GLFWwindow* a_window, int a_key, int a_scancode, int a_action, int a_mods);

// this function renders the scene
void updateGraphics(void);

// this function contains the main haptics simulation loop
void updateHaptics(void);

// this function closes the application
void close(void);

cVector3d toAxisAngleVec(cMatrix3d m);
double toAxisAngleAngle(cMatrix3d m);

//===========================================================================
/*
    DEMO:    01-ODE-cubic.cpp

    This example illustrates the use of the ODE framework for simulating
    haptic interaction with dynamic bodies. In this scene we create 3
    cubic meshes that we individually attach to ODE bodies. Haptic interactions
    are computer by using the finger-proxy haptic model and forces are
    propagated to the ODE representation.
 */
 //===========================================================================

int main(int argc, char* argv[])
{
    //-----------------------------------------------------------------------
    // INITIALIZATION
    //-----------------------------------------------------------------------

    cout << endl;
    cout << "-----------------------------------" << endl;
    cout << "CHAI3D" << endl;
    cout << "Demo: 01-ODE-cubic" << endl;
    cout << "Copyright 2003-2016" << endl;
    cout << "-----------------------------------" << endl << endl << endl;
    cout << "Keyboard Options:" << endl << endl;
    cout << "[g] - Enable/Disable gravity" << endl;
    cout << "[f] - Enable/Disable full screen mode" << endl;
    cout << "[m] - Enable/Disable vertical mirroring" << endl;
    cout << "[q] - Exit application" << endl;
    cout << endl << endl;


    //--------------------------------------------------------------------------
    // OPEN GL - WINDOW DISPLAY
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

    // set resize callback
    glfwSetWindowSizeCallback(window, windowSizeCallback);

    // set current display context
    glfwMakeContextCurrent(window);

    // sets the swap interval for the current display context
    glfwSwapInterval(swapInterval);

    // initialize GLEW library
#ifdef GLEW_VERSION
    if (glewInit() != GLEW_OK)
    {
        cout << "failed to initialize GLEW library" << endl;
        glfwTerminate();
        return 1;
    }
#endif


    //-----------------------------------------------------------------------
    // WORLD - CAMERA - LIGHTING
    //-----------------------------------------------------------------------

    // create a new world.
    world = new cWorld();

    // set the background color of the environment
    world->m_backgroundColor.setWhite();

    // create a camera and insert it into the virtual world
    camera = new cCamera(world);
    world->addChild(camera);

    // position and orient the camera
    camera->set(cVector3d(2.5, 0.0, 0.3),    // camera position (eye)
        cVector3d(0.0, 0.0, -0.5),    // lookat position (target)
        cVector3d(0.0, 0.0, 1.0));   // direction of the "up" vector

// set the near and far clipping planes of the camera
    camera->setClippingPlanes(0.01, 10.0);

    // set stereo mode
    camera->setStereoMode(stereoMode);

    // set stereo eye separation and focal length (applies only if stereo is enabled)
    camera->setStereoEyeSeparation(0.02);
    camera->setStereoFocalLength(2.0);

    // set vertical mirrored display mode
    camera->setMirrorVertical(mirroredDisplay);

    // create a light source
    light = new cSpotLight(world);

    // attach light to camera
    world->addChild(light);

    // enable light source
    light->setEnabled(true);

    // position the light source
    light->setLocalPos(0.0, 0.0, 1.2);

    // define the direction of the light beam
    light->setDir(0.0, 0.0, -1.0);

    // set uniform concentration level of light 
    light->setSpotExponent(0.0);

    // enable this light source to generate shadows
    light->setShadowMapEnabled(true);

    // set the resolution of the shadow map
    light->m_shadowMap->setQualityLow();
    //light->m_shadowMap->setQualityMedium();

    // set light cone half angle
    light->setCutOffAngleDeg(45);


    //-----------------------------------------------------------------------
    // HAPTIC DEVICES / TOOLS
    //-----------------------------------------------------------------------
    previousframecaught = false;
    // create a haptic device handler
    handler = new cHapticDeviceHandler();

    // get access to the first available haptic device found
    handler->getDevice(hapticDevice, 0);

    // retrieve information about the current haptic device
    cHapticDeviceInfo hapticDeviceInfo = hapticDevice->getSpecifications();

    // create a tool (gripper or pointer)
    if (1)
    {
        tool = new cToolGripper(world);
    }
    else
    {
        // tool = new cToolCursor(world);
    }

    // insert tool into world
    world->addChild(tool);

    // connect the haptic device to the virtual tool
    tool->setHapticDevice(hapticDevice);

    // map the physical workspace of the haptic device to a larger virtual workspace.
    tool->setWorkspaceRadius(1.3);

    // define a radius for the virtual tool contact points (sphere)
    double toolRadius = 0.02;
    tool->setRadius(toolRadius, toolRadius);
    tool->setGripperWorkspaceScale(0.25);
    // enable if objects in the scene are going to rotate of translate
    // or possibly collide against the tool. If the environment
    // is entirely static, you can set this parameter to "false"
    tool->enableDynamicObjects(true);

    // haptic forces are enabled only if small forces are first sent to the device;
    // this mode avoids the force spike that occurs when the application starts when 
    // the tool is located inside an object for instance. 
    tool->setWaitForSmallForce(true);

    // start the haptic tool
    tool->start();


    //--------------------------------------------------------------------------
    // WIDGETS
    //--------------------------------------------------------------------------

    // create a font
    cFontPtr font = NEW_CFONTCALIBRI20();

    // create a label to display the haptic and graphic rate of the simulation
    labelRates = new cLabel(font);
    labelRates->m_fontColor.setBlack();
    camera->m_frontLayer->addChild(labelRates);


    //-----------------------------------------------------------------------
    // CREATE ODE WORLD AND OBJECTS
    //-----------------------------------------------------------------------

    //////////////////////////////////////////////////////////////////////////
    // ODE WORLD
    //////////////////////////////////////////////////////////////////////////

    // read the scale factor between the physical workspace of the haptic
    // device and the virtual workspace defined for the tool
    double workspaceScaleFactor = tool->getWorkspaceScaleFactor();

    // stiffness properties
    double maxStiffness = hapticDeviceInfo.m_maxLinearStiffness / workspaceScaleFactor;

    // create an ODE world to simulate dynamic bodies
    ODEWorld = new cODEWorld(world);

    // add ODE world as a node inside world
    world->addChild(ODEWorld);

    // set some gravity
    ODEWorld->setGravity(cVector3d(0.00, 0.00, -9.81));

    // define damping properties
    ODEWorld->setAngularDamping(1);
    ODEWorld->setLinearDamping(0.0002);


    //////////////////////////////////////////////////////////////////////////
    // 3 ODE BLOCKS
    //////////////////////////////////////////////////////////////////////////

    // create a new ODE object that is automatically added to the ODE world
    ODEBody0 = new cODEGenericBody(ODEWorld);
    ODEBody1 = new cODEGenericBody(ODEWorld);
    ODEBody2 = new cODEGenericBody(ODEWorld);

    // create a virtual mesh  that will be used for the geometry representation of the dynamic body
    object0 = new cMesh();
    object1 = new cMesh();
    object2 = new cMultiMesh();

    // create a cube mesh
    double size = 0.40;
    cCreateBox(object0, size * 2, size * 0.1, size * 0.1);
    object0->createAABBCollisionDetector(toolRadius);

    cCreateRing(object1, 0.01, 0.1);
    object1->createAABBCollisionDetector(toolRadius);

    //cCreateBox(object2, size, size, size);
    bool fileload;
    fileload = object2->loadFromFile("../Resources/Models/Needle/needle_model.obj"); // À remplacer par loadFromFile(ROOT_DIR "Resources/Models/Needle/needle_model.obj") !
    if (!fileload) {
        cout << "arg";
        TCHAR NPath[MAX_PATH];
        GetCurrentDirectory(MAX_PATH, NPath);
        cout << NPath;
    }
    object2->createAABBCollisionDetector(toolRadius);

    // define some material properties for each cube
    cMaterial mat0, mat1, mat2;
    mat0.setRedIndian();
    mat0.setStiffness(0.3 * maxStiffness);
    mat0.setDynamicFriction(3);
    mat0.setStaticFriction(3);
    object0->setMaterial(mat0);

    mat1.setBlueRoyal();
    mat1.setStiffness(0.3 * maxStiffness);
    mat1.setDynamicFriction(3);
    mat1.setStaticFriction(3);
    object1->setMaterial(mat1);

    mat2.setGreenDarkSea();
    mat2.setStiffness(0.3 * maxStiffness);
    mat2.setDynamicFriction(3);
    mat2.setStaticFriction(3);
    object2->setMaterial(mat2);

    // add mesh to ODE object
    ODEBody0->setImageModel(object0);
    ODEBody1->setImageModel(object1);
    ODEBody2->setImageModel(object2);

    // create a dynamic model of the ODE object. Here we decide to use a box just like
    // the object mesh we just defined
    ODEBody2->createDynamicBox(size, size, size);
    ODEBody1->createDynamicMesh();
    ODEBody0->createDynamicBox(size * 2, size * 0.1, size * 0.1);

    // define some mass properties for each cube
    ODEBody0->setMass(0.05);
    ODEBody1->setMass(0.05);
    ODEBody2->setMass(0.05);

    // set position of each cube
    ODEBody0->setLocalPos(0.0, -0.6, -0.5);
    ODEBody2->setLocalPos(0.0, 0.0, -5);

    // rotate central cube 45 degrees around z-axis
    //ODEBody0->rotateAboutGlobalAxisDeg(0,0,1, 45);
    ODEBody2->rotateAboutGlobalAxisDeg(cVector3d(1, 0, 0), 90);


    //////////////////////////////////////////////////////////////////////////
    // 6 ODE INVISIBLE WALLS
    //////////////////////////////////////////////////////////////////////////

    // we create 6 static walls to contains the 3 cubes within a limited workspace
  //  ODEGPlane0 = new cODEGenericBody(ODEWorld);
    ODEGPlane1 = new cODEGenericBody(ODEWorld);
    // ODEGPlane2 = new cODEGenericBody(ODEWorld);
   //  ODEGPlane3 = new cODEGenericBody(ODEWorld);
    // ODEGPlane4 = new cODEGenericBody(ODEWorld);
   //  ODEGPlane5 = new cODEGenericBody(ODEWorld);

    w = 1.0;
    //ODEGPlane0->createStaticPlane(cVector3d(0.0, 0.0, 2.0 * w), cVector3d(0.0, 0.0, -1.0));
    ODEGPlane1->createStaticPlane(cVector3d(0.0, 0.0, -w), cVector3d(0.0, 0.0, 1.0));
    // ODEGPlane2->createStaticPlane(cVector3d(0.0, w, 0.0), cVector3d(0.0, -1.0, 0.0));
    // ODEGPlane3->createStaticPlane(cVector3d(0.0, -w, 0.0), cVector3d(0.0, 1.0, 0.0));
   //  ODEGPlane4->createStaticPlane(cVector3d(w, 0.0, 0.0), cVector3d(-1.0, 0.0, 0.0));
    // ODEGPlane5->createStaticPlane(cVector3d(-0.8 * w, 0.0, 0.0), cVector3d(1.0, 0.0, 0.0));


     //////////////////////////////////////////////////////////////////////////
     // GROUND
     //////////////////////////////////////////////////////////////////////////

     // create a mesh that represents the ground
    cMesh* ground = new cMesh();
    ODEWorld->addChild(ground);

    // create a plane
    double groundSize = 3.0;
    cCreatePlane(ground, groundSize, groundSize);

    // position ground in world where the invisible ODE plane is located (ODEGPlane1)
    ground->setLocalPos(0.0, 0.0, -1.0);

    // define some material properties and apply to mesh
    cMaterial matGround;
    matGround.setStiffness(0.3 * maxStiffness);
    matGround.setDynamicFriction(0.2);
    matGround.setStaticFriction(0.0);
    matGround.setWhite();
    matGround.m_emission.setGrayLevel(0.3);
    ground->setMaterial(matGround);

    // setup collision detector
    ground->createAABBCollisionDetector(toolRadius);

    //-----------------------------------------------------------------------
    // START SIMULATION
    //-----------------------------------------------------------------------

    // create a thread which starts the main haptics rendering loop
    hapticsThread = new cThread();
    hapticsThread->start(updateHaptics, CTHREAD_PRIORITY_HAPTICS);

    // setup callback when application exits
    atexit(close);


    //--------------------------------------------------------------------------
    // MAIN GRAPHIC LOOP
    //--------------------------------------------------------------------------

    // call window size callback at initialization
    windowSizeCallback(window, width, height);

    // main graphic loop
    while (!glfwWindowShouldClose(window))
    {
        // get width and height of window
        glfwGetWindowSize(window, &width, &height);

        // render graphics
        updateGraphics();

        // swap buffers
        glfwSwapBuffers(window);

        // process events
        glfwPollEvents();

        // signal frequency counter
        freqCounterGraphics.signal(1);
    }

    // close window
    glfwDestroyWindow(window);

    // terminate GLFW library
    glfwTerminate();

    // exit
    return 0;
}

//---------------------------------------------------------------------------

void windowSizeCallback(GLFWwindow* a_window, int a_width, int a_height)
{
    // update window size
    width = a_width;
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

    // option - exit
    else if ((a_key == GLFW_KEY_ESCAPE) || (a_key == GLFW_KEY_Q))
    {
        glfwSetWindowShouldClose(a_window, GLFW_TRUE);
    }

    // option - enable/disable gravity
    else if (a_key == GLFW_KEY_G)
    {
        if (ODEWorld->getGravity().length() > 0.0)
        {
            ODEWorld->setGravity(cVector3d(0.0, 0.0, 0.0));
        }
        else
        {
            ODEWorld->setGravity(cVector3d(0.0, 0.0, -9.81));
        }
    }

    // option - toggle fullscreen
    else if (a_key == GLFW_KEY_F)
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

    // option - toggle vertical mirroring
    else if (a_key == GLFW_KEY_M)
    {
        mirroredDisplay = !mirroredDisplay;
        camera->setMirrorVertical(mirroredDisplay);
    }
}

//---------------------------------------------------------------------------

void close(void)
{
    // stop the simulation
    simulationRunning = false;

    // wait for graphics and haptics loops to terminate
    while (!simulationFinished) { cSleepMs(100); }

    // close haptic device
    tool->stop();

    // delete resources
    delete hapticsThread;
    delete world;
    delete handler;
}

//---------------------------------------------------------------------------

void updateGraphics(void)
{
    /////////////////////////////////////////////////////////////////////
    // UPDATE WIDGETS
    /////////////////////////////////////////////////////////////////////

    // update haptic and graphic rate data
    labelRates->setText(cStr(freqCounterGraphics.getFrequency(), 0) + " Hz / " +
        cStr(freqCounterHaptics.getFrequency(), 0) + " Hz");

    // update position of label
    labelRates->setLocalPos((int)(0.5 * (width - labelRates->getWidth())), 15);


    /////////////////////////////////////////////////////////////////////
    // RENDER SCENE
    /////////////////////////////////////////////////////////////////////

    // update shadow maps (if any)
    world->updateShadowMaps(false, mirroredDisplay);

    // render world
    camera->renderView(width, height);

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
    simulationFinished = false;

    // reset clock
    cPrecisionClock clock;
    clock.reset();

    // main haptic simulation loop
    while (simulationRunning)
    {

        ODEBody1->setLocalPos(0.0, 0, 0);
        ODEBody1->setLocalRot(cMatrix3d(1, 0, 1, M_PI));
        /////////////////////////////////////////////////////////////////////
        // SIMULATION TIME
        /////////////////////////////////////////////////////////////////////

        // stop the simulation clock
        clock.stop();

        // read the time increment in seconds
        double timeInterval = cClamp(clock.getCurrentTimeSeconds(), 0.0001, 0.001);

        // restart the simulation clock
        clock.reset();
        clock.start();

        // signal frequency counter
        freqCounterHaptics.signal(1);


        /////////////////////////////////////////////////////////////////////
        // HAPTIC FORCE COMPUTATION
        /////////////////////////////////////////////////////////////////////

        // compute global reference frames for each object
        world->computeGlobalPositions(true);


        // update position and orientation of tool
        tool->updateFromDevice();

        // compute interaction forces
        tool->computeInteractionForces();

        // send forces to haptic device
        tool->applyToDevice();

        /////////////////////////////////////////////////////////////////////
        // DYNAMIC SIMULATION
        /////////////////////////////////////////////////////////////////////

        // for each interaction point of the tool we look for any contact events
        // with the environment and apply forces accordingly
        bool caught = true;
        int numInteractionPoints = tool->getNumHapticPoints();
        for (int i = 0; i < numInteractionPoints; i++)
        {
            // get pointer to next interaction point of tool
            cHapticPoint* interactionPoint = tool->getHapticPoint(i);
            //bool temp = false;
            //for (int j = 0; j < object2->getNumMeshes();j++) {
            //    if (interactionPoint->isInContact( object2->getMesh(j) )) {
            //        temp = true;
            //    }
            //}
            //if (!temp) {
            //    caught = false;
            //}
            if (!interactionPoint->isInContact(object0)) {
                caught = false;
            }

            if (caught && i == 1) {
                if (!previousframecaught) {
                    startrotGripper.copyfrom(tool->getDeviceGlobalRot());
                    startrotCube.copyfrom(ODEBody0->getLocalRot());
                    startposGripper.copyfrom(tool->getDeviceLocalPos());
                    startposCube.copyfrom(ODEBody0->getLocalPos());
                    previousframecaught = true;
                    ODEWorld->setGravity(cVector3d(0, 0, 0));
                }
                cMatrix3d ObjT0Invert;
                cMatrix3d ArmT;
                cMatrix3d ArmT0Invert;
                cMatrix3d ObjT0;
                ObjT0Invert.copyfrom(startrotCube);
                ObjT0Invert.invert();
                ArmT.copyfrom(tool->getDeviceLocalRot());
                ArmT0Invert.copyfrom(startrotGripper);
                ArmT0Invert.invert();
                ObjT0.copyfrom(startrotCube);

                ODEBody0->setLocalRot(ObjT0Invert * ArmT * ArmT0Invert * ObjT0);

                //cMatrix3d invert;
                //startrotGripper.copyto(invert);
                //invert.invert();
                //cMatrix3d toollocalrot;
                //toollocalrot.copyfrom(tool->getDeviceGlobalRot());
                //cVector3d rotvec = toAxisAngleVec(invert * toollocalrot);
                //double rotangle = toAxisAngleAngle(invert * toollocalrot);
                //if (isnan(rotangle) ) {
                //    rotangle = 0;
                //}

                //ODEBody0->setLocalPos(startposCube + tool->getDeviceLocalPos() - startposGripper);
                //ODEBody0->setLocalRot(startrotCube);
                //ODEBody0->rotateAboutGlobalAxisRad(rotvec, rotangle );
                //startrotGripper.copyfrom(toollocalrot);
                //startrotCube.copyfrom(ODEBody0->getLocalRot());
                //startposGripper.copyfrom(tool->getDeviceLocalPos());
                ////startposCube.copyfrom(ODEBody0->getLocalPos());
                //cMatrix3d temp;
                //temp.copyfrom(ODEBody0->getLocalRot());
                //temp.invert();
                //cMatrix3d temp2;
                //ODEBody1->setLocalRot(  temp * toollocalrot * startrotCube  * invert* toollocalrot  );

            }
            if (!caught && i == 1) {
                previousframecaught = false;
                ODEWorld->setGravity(cVector3d(0, 0, -9.81));
            }
            // check all contact points
            int numContacts = interactionPoint->getNumCollisionEvents();
            for (int i = 0; i < numContacts; i++)
            {
                cCollisionEvent* collisionEvent = interactionPoint->getCollisionEvent(i);

                // given the mesh object we may be touching, we search for its owner which
                // could be the mesh itself or a multi-mesh object. Once the owner found, we
                // look for the parent that will point to the ODE object itself.
                cGenericObject* object = collisionEvent->m_object->getOwner()->getOwner();


                // cast to ODE object
                cODEGenericBody* ODEobject = dynamic_cast<cODEGenericBody*>(object);

                // if ODE object, we apply interaction forces
                if (ODEobject != NULL)
                {

                    ODEobject->addExternalForceAtPoint(-3 * interactionPoint->getLastComputedForce(),
                        collisionEvent->m_globalPos);
                }
            }

        }

        // update simulation
        ODEWorld->updateDynamics(timeInterval);
    }

    // exit haptics thread
    simulationFinished = true;
}


cVector3d toAxisAngleVec(cMatrix3d m) {
    double angle, x, y, z; // variables for result
    double epsilon = 1e-15; // margin to allow for rounding errors
    double epsilon2 = 1e-15; // margin to distinguish between 0 and 180 degrees
    // optional check that input is pure rotation, 'isRotationMatrix' is defined at:
    // https://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/

    if ((abs(m(0, 1) - m(1, 0)) < epsilon)
        && (abs(m(0, 2) - m(2, 0)) < epsilon)
        && (abs(m(1, 2) - m(2, 1)) < epsilon)) {
        cout << "en dessous" << endl;
        // singularity found
        // first check for identity matrix which must have +1 for all terms
        //  in leading diagonaland zero in other terms
        if ((abs(m(0, 1) + m(1, 0)) < epsilon2)
            && (abs(m(0, 2) + m(2, 0)) < epsilon2)
            && (abs(m(1, 2) + m(2, 1)) < epsilon2)
            && (abs(m(0, 0) + m(1, 1) + m(2, 2) - 3) < epsilon2)) {
            // this singularity is identity matrix so angle = 0
            return cVector3d(1, 0, 0); // zero angle, arbitrary axis
        }
        // otherwise this singularity is angle = 180
        angle = M_PI;
        double xx = (m(0, 0) + 1) / 2;
        double yy = (m(1, 1) + 1) / 2;
        double zz = (m(2, 2) + 1) / 2;
        double xy = (m(0, 1) + m(1, 0)) / 4;
        double xz = (m(0, 2) + m(2, 0)) / 4;
        double yz = (m(1, 2) + m(2, 1)) / 4;
        if ((xx > yy) && (xx > zz)) { // m(0,0) is the largest diagonal term
            if (xx < epsilon) {
                x = 0;
                y = 0.7071;
                z = 0.7071;
            }
            else {
                x = sqrt(xx);
                y = xy / x;
                z = xz / x;
            }
        }
        else if (yy > zz) { // m(1,1) is the largest diagonal term
            if (yy < epsilon) {
                x = 0.7071;
                y = 0;
                z = 0.7071;
            }
            else {
                y = sqrt(yy);
                x = xy / y;
                z = yz / y;
            }
        }
        else { // m(2,2) is the largest diagonal term so base result on this
            if (zz < epsilon) {
                x = 0.7071;
                y = 0.7071;
                z = 0;
            }
            else {
                z = sqrt(zz);
                x = xz / z;
                y = yz / z;
            }
        }
        return cVector3d(x, y, z); // return 180 deg rotation
    }
    // as we have reached here there are no singularities so we can handle normally
    double s = sqrt((m(2, 1) - m(1, 2)) * (m(2, 1) - m(1, 2))
        + (m(0, 2) - m(2, 0)) * (m(0, 2) - m(2, 0))
        + (m(1, 0) - m(0, 1)) * (m(1, 0) - m(0, 1))); // used to normalise
    if (abs(s) < 0.001) s = 1;
    // prevent divide by zero, should not happen if matrix is orthogonal and should be
    // caught by singularity test above, but I've left it in just in case
    angle = acos((m(0, 0) + m(1, 1) + m(2, 2) - 1) / 2);
    x = (m(2, 1) - m(1, 2)) / s;
    y = (m(0, 2) - m(2, 0)) / s;
    z = (m(1, 0) - m(0, 1)) / s;
    return cVector3d(x, y, z);
}

double toAxisAngleAngle(cMatrix3d m) {
    double angle, x, y, z; // variables for result
    double epsilon = 1e-15; // margin to allow for rounding errors
    double epsilon2 = 1e-15; // margin to distinguish between 0 and 180 degrees
    // optional check that input is pure rotation, 'isRotationMatrix' is defined at:
    // https://www.euclideanspace.com/maths/algebra/matrix/orthogonal/rotation/

    if ((abs(m(0, 1) - m(1, 0)) < epsilon)
        && (abs(m(0, 2) - m(2, 0)) < epsilon)
        && (abs(m(1, 2) - m(2, 1)) < epsilon)) {
        // singularity found
        // first check for identity matrix which must have +1 for all terms
        //  in leading diagonaland zero in other terms
        if ((abs(m(0, 1) + m(1, 0)) < epsilon2)
            && (abs(m(0, 2) + m(2, 0)) < epsilon2)
            && (abs(m(1, 2) + m(2, 1)) < epsilon2)
            && (abs(m(0, 0) + m(1, 1) + m(2, 2) - 3) < epsilon2)) {
            // this singularity is identity matrix so angle = 0
            return 0; // zero angle, arbitrary axis
        }
        // otherwise this singularity is angle = 180
        angle = M_PI;
        double xx = (m(0, 0) + 1) / 2;
        double yy = (m(1, 1) + 1) / 2;
        double zz = (m(2, 2) + 1) / 2;
        double xy = (m(0, 1) + m(1, 0)) / 4;
        double xz = (m(0, 2) + m(2, 0)) / 4;
        double yz = (m(1, 2) + m(2, 1)) / 4;
        if ((xx > yy) && (xx > zz)) { // m(0,0) is the largest diagonal term
            if (xx < epsilon) {

                x = 0;
                y = 0.7071;
                z = 0.7071;
            }
            else {
                x = sqrt(xx);
                y = xy / x;
                z = xz / x;
            }
        }
        else if (yy > zz) { // m(1,1) is the largest diagonal term
            if (yy < epsilon) {

                x = 0.7071;
                y = 0;
                z = 0.7071;
            }
            else {
                y = sqrt(yy);
                x = xy / y;
                z = yz / y;
            }
        }
        else { // m(2,2) is the largest diagonal term so base result on this
            if (zz < epsilon) {

                x = 0.7071;
                y = 0.7071;
                z = 0;
            }
            else {
                z = sqrt(zz);
                x = xz / z;
                y = yz / z;
            }
        }
        return angle; // return 180 deg rotation
    }
    // as we have reached here there are no singularities so we can handle normally
    double s = sqrt((m(2, 1) - m(1, 2)) * (m(2, 1) - m(1, 2))
        + (m(0, 2) - m(2, 0)) * (m(0, 2) - m(2, 0))
        + (m(1, 0) - m(0, 1)) * (m(1, 0) - m(0, 1))); // used to normalise
    if (abs(s) < 0.001) s = 1;
    // prevent divide by zero, should not happen if matrix is orthogonal and should be
    // caught by singularity test above, but I've left it in just in case
    angle = acos((m(0, 0) + m(1, 1) + m(2, 2) - 1) / 2);
    x = (m(2, 1) - m(1, 2)) / s;
    y = (m(0, 2) - m(2, 0)) / s;
    z = (m(1, 0) - m(0, 1)) / s;
    return angle;
}