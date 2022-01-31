#include "cToolGripperLevel.h"

using namespace std;

cToolGripperLevel::cToolGripperLevel(const std::string a_resourceRoot,
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
		m_tool0 = new cToolGripper(m_world);
		m_world->addChild(m_tool0);
		m_tool0->setHapticDevice(shared_ptr<cGenericHapticDevice>(a_hapticDevice0));
		// if the haptic device has a gripper, enable it as a user switch
		a_hapticDevice0->setEnableGripperUserSwitch(true);
		// define a radius for the tool
		m_tool0->setRadius(toolRadius);
		m_tool0->setShowContactPoints(true, false);
		// enable if objects in the scene are going to rotate of translate
		// or possibly collide against the tool. If the environment
		// is entirely static, you can set this parameter to "false"
		m_tool0->enableDynamicObjects(true);

		// haptic forces are enabled only if small forces are first sent to the device;
		// this mode avoids the force spike that occurs when the application starts when 
		// the tool is located inside an object for instance. 
		m_tool0->setWaitForSmallForce(true);
		m_tool0->start();
		m_tools[0] = m_tool0;
	}

	if (m_numTools > 1)
	{
		m_tool1 = new cToolGripper(m_world);
		m_world->addChild(m_tool1);
		m_tool1->setHapticDevice(shared_ptr<cGenericHapticDevice>(a_hapticDevice1));
		// if the haptic device has a gripper, enable it as a user switch
		a_hapticDevice1->setEnableGripperUserSwitch(true);
		// define a radius for the tool
		m_tool1->setRadius(toolRadius);
		m_tool1->setShowContactPoints(true, false);
		// enable if objects in the scene are going to rotate of translate
		// or possibly collide against the tool. If the environment
		// is entirely static, you can set this parameter to "false"
		m_tool1->enableDynamicObjects(true);

		// map the physical workspace of the haptic device to a larger virtual workspace.

		// haptic forces are enabled only if small forces are first sent to the device;
		// this mode avoids the force spike that occurs when the application starts when 
		// the tool is located inside an object for instance. 
		m_tool1->setWaitForSmallForce(true);
		m_tool1->start();
		m_tools[1] = m_tool1;
	}

}

void cToolGripperLevel::close() {

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