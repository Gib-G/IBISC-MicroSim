#include "AppVirtsa.h"
#include "Pick-and-Place.h"

void virtsaGoThreadFunction(){
	SetVirtualScene();
}


AppVirtsa::AppVirtsa(){
	virtsaStarted = false;
	virtsaApp = NULL;
}


AppVirtsa::~AppVirtsa(){}


int AppVirtsa::InitApp(bool debugConsole, bool showGraphics, bool debugGraphics, int arduinoSerialPort, int arduinoBaudRate){
	return InitializeApp(debugConsole, showGraphics, debugGraphics, arduinoSerialPort, arduinoBaudRate);
}


int AppVirtsa::InitAppSceneAndStart(bool debugConsole, bool showGraphics, bool debugGraphics, int arduinoSerialPort, int arduinoBaudRate){
	InitializeApp(debugConsole, showGraphics, debugGraphics, arduinoSerialPort, arduinoBaudRate);
	//virtsaApp = new std::thread(&AppVirtsa::WaitThereAlive, this);
	myThread = new cThread();
	myThread->start(virtsaGoThreadFunction, CTHREAD_PRIORITY_GRAPHICS);
	Sleep(500);
	StartApp();
	Sleep(4000);
	virtsaStarted = true;
	//SetHapticForces(false);
	return getToolsNumber();
}

int AppVirtsa::InitScene(){
	//virtsaApp = new std::thread(&AppVirtsa::WaitThereAlive, this);
	//Sleep(2000);
	return 0;
}


int AppVirtsa::StartMyApp(){
	return StartApp();
}


int AppVirtsa::ShutDownApp(){
	virtsaStarted = false;
	finishApp();

	// clean thread
	if (virtsaApp != NULL){
		if (virtsaApp->joinable()){
			virtsaApp->join();
		}
		delete virtsaApp;
		virtsaApp = NULL;
	}
	return 0;
}

int AppVirtsa::SetForces(bool newState){
	return SetHapticForces(newState);
}

int AppVirtsa::SetGameScenePosition(double x, double y, double z){
	return UpdateGameScenePosition(x, y, z);
}

int AppVirtsa::ResetGamePositions(){
	return resetObjectsPositions();
}

int AppVirtsa::ResetCubePosition(int cubeId) {
	return resetObjectPosition(cubeId);
}

int AppVirtsa::getToolsNumber(){
	return getNumberOfTools();
}

int AppVirtsa::GetRigidObjectsData(double outInformation[]){
	return GetRigidObjects(outInformation);
}

int AppVirtsa::GetInteractObjectsData(double outInformation[]){
	if (!virtsaStarted) return -1;
	return GetInteractObjects(outInformation);
}

int AppVirtsa::GetHapticsData(double outInformation[]){
	return GetHapticInformation(outInformation);
}

int AppVirtsa::UpdateCubeDynamics(int cube_id, bool enable)
{
	return updateDynamics(cube_id, enable);
}

void AppVirtsa::WaitThereAlive(){
	//SetVirtualScene();
	printf("Stop thread...");
}
