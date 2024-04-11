#include "shark_enemy_object.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

namespace game {

	SharkEnemyObject::SharkEnemyObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health)
	: GameObject(position, geom, shader, texture, yScale, xScale, health) {

	current_time_ = 0.0;
	type_ = EnemyObj;
	target_ = nullptr;
	next_shot_ = 1.0;
}

// Generic mapping function
// Map a number "input" in the range [a, b] to the range [c, d]
float map(float input, float a, float b, float c, float d){

	return c + (d - c)*(input -a)/(b - a);
}

// Update function for moving the player object around
void SharkEnemyObject::Update(double delta_time) {

	// Increment timer
	current_time_ += delta_time;

	// Set orientation based on current velocity vector
	if (alive_) {
		angle_ = glm::atan(velocity_.y, velocity_.x);

		// Chase steering behavior

			// Compute steering force (acceleration)
		glm::vec3 desired = target_->GetPosition() - position_;
		glm::vec3 acc = desired - velocity_;

		// Add steering to velocity
		velocity_ += acc * ((float)delta_time);

		// Limit maximum velocity
		float s = 0.6;
		velocity_ = s * glm::normalize(velocity_);

		// Perform arrival
		float r = 0.5; // Distance threshold when arrival kicks in
		float d = glm::length(target_->GetPosition() - position_);
		if (d < r) {
			float z = map(d, 0, r, 0, s);
			velocity_ = z * glm::normalize(velocity_);
		}
	}
	
	if (health_ <= 0) alive_ = false;
	GameObject::Update(delta_time);
}

} // namespace game
