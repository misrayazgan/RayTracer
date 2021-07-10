#include <iostream>
#include "Vec3f.h"
#include "Matrix4f.h"
#include "Scene.h"
#include <thread>
#include <sstream>
#include <iomanip>

int main()
{
	// Get xml file path from user.
	/*std::string filepath;
	std::cout << "Please provide input filepath:" << std::endl;
	std::cin >> filepath;
	
	Scene scene;	
	scene.loadSceneFromXml(filepath);
	scene.renderScene();*/

	Scene scene;
	std::string filepath = "SampleScenes/pathTracing/cornellbox_jaroslav_path_glass.xml";
	//std::string filepath = "SampleScenes/directLighting/cornellbox_jaroslav_diffuse_area.xml";
	//std::string filepath = "SampleScenes/veach_ajar/scene.xml";

	scene.loadSceneFromXml(filepath);

	scene.renderScene();

	/*for (int i = 243; i < 286; i++)
	{
		Scene scene;
		std::stringstream ss;
		ss << std::setw(4) << std::setfill('0') << i;
		std::string imageNumber = ss.str();
		std::string filepath = "water_animation/smooth/tap_" + imageNumber + ".xml";

		scene.loadSceneFromXml(filepath);

		scene.renderScene();
	}*/

	return 0;
}
