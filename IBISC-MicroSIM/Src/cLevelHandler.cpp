#include "cLevelHandler.h"

cLevelHandler::cLevelHandler(cOVRDevice* o){
	mainLevel = NULL;
	oculusVR = o;
}

void cLevelHandler::setLevel(cGenericLevel* level) {
	mainLevel = level;
	mainLevel->m_camera->setLocalPos(mainLevel->defaultPos);
	oculusVR->recenterPose();
}