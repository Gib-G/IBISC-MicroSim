#include "cLevelHandler.h"

cLevelHandler::cLevelHandler(cOVRDevice* o){
	mainLevel = NULL;
	oculusVR = o;
}

void cLevelHandler::setLevel(cGenericLevel* level) {
	mainLevel = level;
	resetCamera();
	mainLevel->init();
	oculusVR->recenterPose();
}

void cLevelHandler::resetCamera() {
	mainLevel->m_camera->setLocalPos(mainLevel->defaultPos);
	mainLevel->m_camera->setLocalRot(mainLevel->defaultRot);
}