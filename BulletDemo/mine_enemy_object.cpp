#include "mine_enemy_object.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

namespace game {

MineEnemyObject::MineEnemyObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health, glm::vec3 orbit)
	: GameObject(position, geom, shader, texture, yScale, xScale, health) {

	current_time_ = 0.0;
	type_ = EnemyObj;
	orbit_ = orbit;
}

// Generic mapping function
// Map a number "input" in the range [a, b] to the range [c, d]
//float map(float input, float a, float b, float c, float d){
//
//	return c + (d - c)*(input -a)/(b - a);
//}

// Update function for moving the player object around
void MineEnemyObject::Update(double delta_time) {

	// Increment timer
	current_time_ += delta_time;

	SetRotation(GetRotation() - delta_time * 2);
	position_.x = (0.2f * cos(current_time_ * 5)) + orbit_.x;
	position_.y = (0.2f * sin(current_time_ * 5)) + orbit_.y;

	if (health_ <= 0) alive_ = false;
	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);
}

} // namespace game
