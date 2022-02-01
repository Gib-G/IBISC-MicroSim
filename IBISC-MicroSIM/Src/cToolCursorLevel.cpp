#include "cToolCursorLevel.h"

using namespace std;

cToolCursorLevel::cToolCursorLevel(const std::string a_resourceRoot,
	const int a_numDevices,
	std::shared_ptr<cGenericHapticDevice> a_hapticDevice0,
	std::shared_ptr<cGenericHapticDevice> a_hapticDevice1) : cGenericLevel(a_resourceRoot, a_numDevices, a_hapticDevice0, a_hapticDevice1) {

	// create tools
	m_tools[0] = NULL;
	m_tools[1] = NULL;
	m_tool0 = NULL;
	m_tool1 = NULL;

	m_numTools = cMin(a_numDevices, 2);

	toolRadius = 0.01;

	if (m_numTools > 0)
	{
		m_tool0 = new cToolCursor(m_world);
		m_world->addChild(m_tool0);
		m_tool0->setHapticDevice(shared_ptr<cGenericHapticDevice>(a_hapticDevice0));
		m_tool0->setRadius(toolRadius);
		m_tool0->enableDynamicObjects(true);
		m_tool0->setWaitForSmallForce(true);
		m_tool0->start();
		m_tools[0] = m_tool0;
		// if the haptic device has a gripper, enable it as a user switch
		shared_ptr<cGenericHapticDevice>(a_hapticDevice0)->setEnableGripperUserSwitch(true);
		m_tool0->setShowContactPoints(true, false);
		// map the physical workspace of the haptic device to a larger virtual workspace.
		m_tool0->setWorkspaceRadius(1);
		// start the haptic tool
		m_tool0->start();
	}

	if (m_numTools > 1)
	{
		m_tool1 = new cToolCursor(m_world);
		m_world->addChild(m_tool1);
		m_tool1->setHapticDevice(shared_ptr<cGenericHapticDevice>(a_hapticDevice1));
		m_tool1->setRadius(toolRadius);
		m_tool1->enableDynamicObjects(true);
		m_tool1->setWaitForSmallForce(true);
		m_tool1->start();
		m_tools[1] = m_tool1;
		// if the haptic device has a gripper, enable it as a user switch
		shared_ptr<cGenericHapticDevice>(a_hapticDevice1)->setEnableGripperUserSwitch(true);
		m_tool1->setShowContactPoints(true, false);
		// map the physical workspace of the haptic device to a larger virtual workspace.
		m_tool1->setWorkspaceRadius(1);
		// start the haptic tool
		m_tool1->start();
	}

}

void cToolCursorLevel::close() {

	// shutdown haptic devices
	if (m_tool0 != NULL)
	{
		m_tool0->stop();
	}
	if (m_tool1 != NULL)
	{
		m_tool1->stop();
	}

}