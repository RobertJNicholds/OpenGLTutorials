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

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {		
		
		
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
	
}