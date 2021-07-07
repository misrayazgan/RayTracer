#include <iostream>
#include "Vec3f.h"
#include "Scene.h"
#include <thread>

int main()
{
	// Get xml file path from user.
	std::string filepath;
	std::cout << "Please provide input filepath:" << std::endl;
	std::cin >> filepath;

    Scene scene;

    scene.loadSceneFromXml(filepath);
	std::cout << "Scene file is parsed successfully" << std::endl;
	scene.renderScene();

	return 0;
}
