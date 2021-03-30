#include "Light.h"

Light::~Light()
{
	delete model;
	model = nullptr;
}
