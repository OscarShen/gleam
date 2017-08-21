

#include <base/window.h>
#include <input/input_engine.h>
#include <rendering/camera.h>
#include <rendering/camera_controller.h>
using namespace gleam;
int main() {
	InputEngine ie;
	Camera camera;
	camera.ViewParams(glm::vec3(0), glm::vec3(0, 0, -1));
	camera.ProjParams(glm::radians(45.0f), 16.0f / 9, 0.1f, 100.0f);
	FirstPersonCameraController cc;
	cc.AttachCamera(camera);

	WindowPtr window = std::make_shared<GLFWWnd>("Hello World!", 1600, 900);

	auto a = std::bind(&FirstPersonCameraController::InputHandler, &cc, std::placeholders::_1, std::placeholders::_2);
	ie.Register(window, a);

	while (true)
	{
		std::cout << camera.ForwardVec().x << ", " << camera.ForwardVec().y << ", " << camera.ForwardVec().z << std::endl;
		window->Update();
		ie.Update();
	}
	std::cout << "ni" << std::endl;
}