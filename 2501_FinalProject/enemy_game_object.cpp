#include "enemy_game_object.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

namespace game {

EnemyGameObject::EnemyGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health)
	: GameObject(position, geom, shader, texture, yScale, xScale, health) {

	target_ = nullptr;
	current_time_ = 0.0;
	wander_cool_down_ = 0.0;
	type_ = EnemyObj;
}

// Generic mapping function
// Map a number "input" in the range [a, b] to the range [c, d]
//float map(float input, float a, float b, float c, float d){
//
//	return c + (d - c)*(input -a)/(b - a);
//}

// Update function for moving the player object around
void EnemyGameObject::Update(double delta_time) {

	// Increment timer
	current_time_ += delta_time;

	// Set orientation based on current velocity vector
	angle_ = glm::atan(velocity_.y, velocity_.x);

	// Wandering behavior
	double activation_time = 5.0;
	if (current_time_ < activation_time){
		if (current_time_ > wander_cool_down_){

			// Get a random angle in the interval [-opening, opening]
			float opening = glm::pi<float>() / 10.0; // Radians
			float r = (float)std::rand() / (float)RAND_MAX;
			float r_angle = -opening + r * opening * 2.0;

			// Add random angle to current orientation,
			// to align the opening with the bearing direction
			// and get the target angle
			float t_angle = angle_ + r_angle;

			// Get point to seek based on target angle
			float d = 1.0; // Radius of circle
			glm::vec3 w = d * glm::vec3(cos(t_angle), sin(t_angle), 0.0);

			// Displace point based on bearing direction (optional)
			//glm::vec3 b = GetBearing();
			//w = b + w;

			// Get desired vector for steering force
			glm::vec3 desired = w;
			glm::vec3 acc = desired - velocity_;
			velocity_ += acc; // Impulse rather than force, so we ignore delta time

			// Keep the velocity controlled
			float speed = 1.0f;
			velocity_ = speed * glm::normalize(velocity_);

			// Update orientation since we modified the velocity
			angle_ = glm::atan(velocity_.y, velocity_.x);

			// Update only after cool down interval
			wander_cool_down_ = current_time_ + 0.25;
		}
	} else {
		// Chase steering behavior

		// Compute steering force (acceleration)
		glm::vec3 desired = target_->GetPosition() - position_;
		glm::vec3 acc = desired - velocity_;

		// Add steering to velocity
		velocity_ += acc*((float) delta_time);

		// Limit maximum velocity
		float s = 1.0;
		velocity_ = s*glm::normalize(velocity_);

		// Perform arrival
		float r = 0.5; // Distance threshold when arrival kicks in
		float d = glm::length(target_->GetPosition() - position_);
		if (d < r){
			//float z = map(d, 0, r, 0, s);
			//velocity_ = z * glm::normalize(velocity_);
		}
	}

	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);
}

} // namespace game
