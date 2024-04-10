#include "item_game_object.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

namespace game {

	ItemGameObject::ItemGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health)
	: GameObject(position, geom, shader, texture, yScale, xScale, health) {

	current_time_ = 0.0;
	type_ = ItemObj;
	item_type_ = InvinciblePower;
}

// Generic mapping function
// Map a number "input" in the range [a, b] to the range [c, d]
//float map(float input, float a, float b, float c, float d){
//
//	return c + (d - c)*(input -a)/(b - a);
//}

// Update function for moving the player object around
void ItemGameObject::Update(double delta_time) {

	// Increment timer
	current_time_ += delta_time;

	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);
}

} // namespace game
