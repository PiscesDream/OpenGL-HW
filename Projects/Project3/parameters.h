#pragma once

#ifndef _PARAMETERS_H_
#define _PARAMETERS_H_

#include <glm/glm.hpp>

// parameters:
namespace Params {
	// map related
	const float cell_width = 2.0f;
	const float cell_height = 5.0f;
	const float ground_max_height = 0.5f;
	const int hill_count = 10;
	const float hill_height = 5.0;
	const float hill_noisy = 1.0;
	const float hill_norm = 10.0;
	const int smooth_times = 10;

	// skybox
	const float skybox_height = 400.0f;
	const float skybox_margin = 800.0f;
	const float sun_margin = 150.0f;

	// camera related
	const float cam_height = 3.0f;
	const float initialFoV = 20.0f;
	const float mouseSpeed = 0.001f;
	
	const float accelerate = 0.3f;
	const float fiction = 0.95f;

	const float boundMargin = 1.0;
	//const float boundMargin = 0.0;
	const float lightMargin = 2.0;

	// particle
	const float p_margin = 20;
	const float p_speed = 1.0;
	const float p_noise = 0.5;
	const float p_rate = 0.5;
	const int p_count = 500;

	// jump
	const float jump_height = 0.3;
	const float gravity = 0.04;
}

#endif
