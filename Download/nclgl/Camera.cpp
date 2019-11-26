#include "Camera.h"
//Made By Alexander Sablin 180575673


void Camera::UpdateCamera(float msec)	{
	//Update the mouse by how much
	if (Window::GetKeyboard()->KeyDown(KEYBOARD_L))
	{
		if (mode == 0) mode = 1;
		else if (mode == 1) mode = 0;
	}



	if (mode == 1) {
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);

		//Bounds check the pitch, to be between straight up and straight down ;)
		pitch = min(pitch, 90.0f);
		pitch = max(pitch, -90.0f);

		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}

		msec *= 5.0f;

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(0, 0, -1) * msec;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
			position += Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
			position -= Matrix4::Rotation(yaw, Vector3(0, 1, 0)) * Vector3(-1, 0, 0) * msec;
		}

		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
			position.y += msec/2;
		}
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
			position.y -= msec/2;
		}
	}
}

void Camera::AutoCamera(float msec) {
	
	Vector3 checkpoints[4] = {
		Vector3(1000,15000,1000),
		Vector3(80000,15000,80000),
		Vector3(160000,15000,1000),
		Vector3(1000,15000,80000)
	};
	
		pitch -= (Window::GetMouse()->GetRelativePosition().y);
		yaw -= (Window::GetMouse()->GetRelativePosition().x);

		//Bounds check the pitch, to be between straight up and straight down ;)
		pitch = min(pitch, 90.0f);
		pitch = max(pitch, -90.0f);

		if (yaw < 0) {
			yaw += 360.0f;
		}
		if (yaw > 360.0f) {
			yaw -= 360.0f;
		}

		msec *= 5.0f;

		if (position != checkpoints[currentCheckpoint])
		{
			position = Vector3::MoveTowards(position, checkpoints[currentCheckpoint], 100.0f);
		}
		else currentCheckpoint++;
		
		if (currentCheckpoint == 4) {
			currentCheckpoint = 0;
		}

	
}
/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix()	{
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix4::Rotation(-pitch, Vector3(1,0,0)) * 
			Matrix4::Rotation(-yaw, Vector3(0,1,0)) * 
			Matrix4::Translation(-position);
};
