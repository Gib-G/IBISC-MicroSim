// Contains declarations of pick and place application
#ifdef PICK_AND_PLACE_EXPORTS
#define PICK_AND_PLACE_API __declspec(dllexport)
#else
#define PICK_AND_PLACE_API __declspec(dllimport)
#endif

#define PICK_AND_PLACE_MAYOR_VERSION "1"
#define PICK_AND_PLACE_MINOR_VERSION "0"

//------------------------------------------------------------------------------
// DECLARED FUNCTIONS
//------------------------------------------------------------------------------

// Determine app configuration:
//		debugConsole	-> if false then the print to console will be saved into logOut/logErr files
//		showGraphics	-> if true then a gl graphical application will be created
//		debugGraphics	-> TODO it will allow ODE and graphical debug
//		arduinoSerialPort	-> COM port for communicating the angle of the tools (FIXME -> make things work without it)
//		arduinoBaudRate	-> communication baud rate with the board
// Flags update, log file created (for std outputs if not debugConsole)
// SerialController thread launched
extern "C" PICK_AND_PLACE_API int InitializeApp(bool debugConsole, bool showGraphics, bool debugGraphics, int arduinoSerialPort, int arduinoBaudRate);

// Initializes haptic (not the thread)
// Sets game scene
extern "C" PICK_AND_PLACE_API int SetVirtualScene();

// Returns the number of rigid objects in the virtual scene
// outInformation contains the information for each rigid object

extern "C" PICK_AND_PLACE_API int GetRigidObjects(double outInformation[]);

// Returns tha haptic position/rotation information
// And the number of interactuable objects in the virtual scene
// outInformation contains the initial information for each interactuable object
extern "C" PICK_AND_PLACE_API int GetInteractObjects(double outInformation[]);

extern "C" PICK_AND_PLACE_API int GetHapticInformation(double outInformation[]);

// Starts the pick transfer place task
// Launch haptic and graphics (if chosen) threads
extern "C" PICK_AND_PLACE_API int StartApp();

extern "C" PICK_AND_PLACE_API int finishApp();

// Change positions/properties
extern "C" PICK_AND_PLACE_API int UpdateGameScenePosition(double x, double y, double z);
extern "C" PICK_AND_PLACE_API int SetHapticForces(bool enable);
extern "C" PICK_AND_PLACE_API int resetObjectsPositions();
extern "C" PICK_AND_PLACE_API int resetObjectPosition(int cube);
extern "C" PICK_AND_PLACE_API int getNumberOfTools();
extern "C" PICK_AND_PLACE_API int updateDynamics(int cube_id, bool enable);

//---------------------------------------------------------------------------
// DECLARED FUNCTIONS
//---------------------------------------------------------------------------

// graphical rendering loops
void updateGraphics(void);
void updateGraphicsGeneral(void);
void updateGraphicsVoid(void);

// main haptics simulation loop
void updateHaptics(void);

int SetUpDisplayAndHapticsContext();
void SetDownDisplayContext();
void SetDownHapticDevices();

void SetUpGameScene(int numDevices, double maxStiffness);

void StartSimulation();
