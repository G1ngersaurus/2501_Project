#include "player_game_object.h"

namespace game {

/*
	PlayerGameObject inherits from GameObject
	It overrides GameObject's update method, so that you can check for input to change the velocity of the player
*/

PlayerGameObject::PlayerGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health)
	: GameObject(position, geom, shader, texture, yScale, xScale, health) {}

// Update function for moving the player object around
void PlayerGameObject::Update(double delta_time, glm::vec3 camera) {

	// Special player updates go here
	if ((position_.x >= camera.x - 5.2f) && (position_.x <= camera.x + 5.2f) && (position_.y >= camera.y - 2.7f) && (position_.y <= camera.y + 3.0f)) {
		this->position_ += (float)delta_time * this->velocity_;
	}
	if (position_.x <= camera.x - 5.2f) this->position_ += glm::vec3(0.02f, 0.0f, 0.0f);
	if (position_.x >= camera.x + 5.2f) this->position_ += glm::vec3(-0.02f, 0.0f, 0.0f);
	if (position_.y <= camera.y - 2.7f) this->position_ += glm::vec3(0.0f, 0.01f, 0.0f);
	//if (position_.y < camera.y + 3.0f) this->position_ += (float)delta_time * this->velocity_;

	
	
	// Call the parent's update method to move the object in standard way, if desired
	//GameObject::Update(delta_time);
}

} // namespace game
