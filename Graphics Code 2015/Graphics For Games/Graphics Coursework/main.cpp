#include "../../nclgl/Window.h"
#include "Renderer.h"

#pragma comment(lib, "nclgl.lib")

int main()
{
	Window w("Graphics Coursework", 1280, 720, false);
	if (!w.HasInitialised())
		return -1;

	Renderer renderer(w);
	if (!renderer.HasInitialised())
		return -1;

	w.LockMouseToWindow(false);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {		
		
		
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_UP))
		{
			Vector3 forward = Vector3(10.0f, 0.0f, 0.0f);
			renderer.UpdateLight(forward);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_DOWN))
		{
			Vector3 backward = Vector3(-10.0f, 0.0f, 0.0f);
			renderer.UpdateLight(backward);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_LEFT))
		{
			Vector3 left = Vector3(0.0f, 0.0f, -10.0f);
			renderer.UpdateLight(left);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_RIGHT))
		{
			Vector3 right = Vector3(0.0f, 0.0f, 10.0f);
			renderer.UpdateLight(right);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_Y))
		{
			Vector3 up = Vector3(0.0f, 10.0f, 0.0f);
			renderer.UpdateLight(up);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_H))
		{
			Vector3 down = Vector3(0.0f, -10.0f, 0.0f);
			renderer.UpdateLight(down);
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
		{
			renderer.SwitchToLightView();
		}
	}

	return 0;
	
}