#pragma comment(lib, "nclgl.lib")
//Made By Alexander Sablin 180575673
#include "../../nclgl/window.h"
#include "Renderer.h"

int main() {
	Window w("Alexander The great", 1920, 1200, true);
	if (!w.HasInitialised()) {
		return -1;
	}

	Renderer renderer(w);
	if (!renderer.HasInitialised()) {
		return -1;
	}

	w.LockMouseToWindow(true);
	w.ShowOSPointer(false);

	PlaySound(TEXT("Disturbed.wav"), NULL, SND_FILENAME | SND_ASYNC);

	while (w.UpdateWindow() && !Window::GetKeyboard()->KeyDown(KEYBOARD_ESCAPE)) {
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		renderer.UpdateScene(w.GetTimer()->GetTimedMS());
		renderer.RenderScene();
	}

	return 0;
}
