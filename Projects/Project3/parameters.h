#pragma once

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <glm/glm.hpp>

// parameters:
namespace Params {
	// map related
	const float cell_width = 2.0f;
	const float cell_height = 5.0f;
	const double ground_max_height = 0.2f;

	// camera related
	const double cam_height = 3.0f;
	const float initialFoV = 20.0f;
	const float mouseSpeed = 0.001f;
	
	const float accelerate = 0.3f;
	const float fiction = 0.95f;
}

#endif
