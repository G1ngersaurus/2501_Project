#include "boss_sub_object.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

namespace game {

BossSubObject::BossSubObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health)
	: GameObject(position, geom, shader, texture, yScale, xScale, health) {

	target_ = nullptr;
	current_time_ = 0.0;
	wander_cool_down_ = 0.0;
	type_ = EnemyObj;
	lastSecond_ = 0;
	docile_ = true;
	next_shot_ = 3.0;
}

// Generic mapping function
// Map a number "input" in the range [a, b] to the range [c, d]
//float map(float input, float a, float b, float c, float d){
//
//	return c + (d - c)*(input -a)/(b - a);
//}

// Update function for moving the player object around
void BossSubObject::Update(double delta_time, glm::vec3 camera) {

	// Increment timer
	// Increment timer
	current_time_ += delta_time;


	// Set orientation based on current velocity vector
	//angle_ = glm::atan(-velocity_.y, velocity_.x);

	if (docile_) {
		velocity_ = glm::vec3(0.0f, 0.5f, 0.0f);

		if (position_.y < camera.y + 1.55) {
			docile_ = false;
			velocity_ = glm::vec3(2.0f, 1.0f, 0.0f);
		}
	}
	else {
		if ((position_.x <= camera.x - 3) || (position_.x >= camera.x + 3)) {
			velocity_.x *= -1.0f;
		}

	}
	if (health_ <= 0) alive_ = false;
	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);
}

} // namespace game
