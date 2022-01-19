#include "CommunicationBasics.h"

CommunicationBasics::CommunicationBasics(){}

CommunicationBasics::~CommunicationBasics(){}


char* CommunicationBasics::GetLastErrorMessage()
{
	return lastErrorMsg;
}


void CommunicationBasics::SetAppManager(AppVirtsa * app){
	AppManager = app;
}