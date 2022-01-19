#include <thread>
#include <Windows.h>
#include "chai3d.h"
#include <atomic>

using namespace chai3d;

class AppVirtsa{
public:
	AppVirtsa();
	virtual ~AppVirtsa();
	int InitApp(bool debugConsole, bool showGraphics, bool debugGraphics, int arduinoSerialPort, int arduinoBaudRate);
	int InitAppSceneAndStart(bool debugConsole, bool showGraphics, bool debugGraphics, int arduinoSerialPort, int arduinoBaudRate);
	int InitScene();
	int StartMyApp();
	int ShutDownApp();
	int SetForces(bool newState);
	int SetGameScenePosition(double x, double y, double z);
	int ResetGamePositions();
	int ResetCubePosition(int cubeId);
	int getToolsNumber();
	int GetRigidObjectsData(double outInformation[]);
	int GetInteractObjectsData(double outInformation[]);
	int GetHapticsData(double outInformation[]);
	int UpdateCubeDynamics(int cube_id, bool enable);

private:
	void WaitThereAlive();

private:
	std::thread* virtsaApp;
	cThread *myThread;
	std::atomic<bool> virtsaStarted;
};
