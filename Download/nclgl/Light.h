//Made By Alexander Sablin 180575673
#pragma once

#include "Vector4.h"
#include "Vector3.h"

 class Light {
 public:
	 Light(Vector3 position, Vector4 colour, float radius) {
		this -> position = position;
		 this -> colour = colour;
		 this -> radius = radius;
		
	}
	 Light();
	
	 ~Light(void) {};
	
	 Vector3 GetPosition() const { return position; }
	void SetPosition(Vector3 val) { position = val; }
	
	 float GetRadius() const { return radius; }
	 void SetRadius(float val) { radius = val; }
	
	 Vector4 GetColour() const { return colour; }
	 void SetColour(Vector4 val) { colour = val; }
	
 protected:
	 Vector3 position;
	 Vector4 colour;
	 float radius;
	};
