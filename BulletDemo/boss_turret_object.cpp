#include "boss_turret_object.h"
#include <iostream>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace game {

BossTurretObject::BossTurretObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, GameObject* parent, glm::vec3 fake_position, float yScale, float xScale, int health)
	: GameObject(position, geom, shader, texture, yScale, xScale, health) {
	parent_ = parent;
	fake_position_ = fake_position;
	target_ = nullptr;
	current_time_ = 0.0;
	wander_cool_down_ = 0.0;
	type_ = EnemyObj;
	next_shot_ = 1.0;
}

// Generic mapping function
// Map a number "input" in the range [a, b] to the range [c, d]
//float map(float input, float a, float b, float c, float d){
//
//	return c + (d - c)*(input -a)/(b - a);
//}

// Update function for moving the player object around
void BossTurretObject::Update(double delta_time) {

	// Increment timer
	current_time_ += delta_time;
	fake_position_ = parent_->GetPosition() + position_;

	// Set orientation based on current velocity vector
	glm::vec3 targetDir = glm::vec3(glm::distance(fake_position_.y, target_->GetPosition().y), glm::distance(fake_position_.x, target_->GetPosition().x), 0.0f);
	angle_ = glm::atan(targetDir.y, targetDir.x);


	// Compute steering force (acceleration)
	//velocity_ = parent_->GetVelocity();
	if (health_ <= 0) alive_ = false;
	// Call the parent's update method to move the object in standard way, if desired
	GameObject::Update(delta_time);
}

void BossTurretObject::Render(glm::mat4 view_matrix, double current_time) {

	// Set up the shader
	shader_->Enable();

	// Set up the view matrix
	shader_->SetUniformMat4("view_matrix", view_matrix);

	// Setup the scaling matrix for the shader
	glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_, scale_, 1.0));

	// Setup the rotation matrix for the shader
	glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), angle_, glm::vec3(0.0, 0.0, 1.0));

	// Set up the translation matrix for the shader
	glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

	// Set up the parent transformation matrix
	//glm::mat4 parent_rotation_matrix = glm::rotate(glm::mat4(1.0f), parent_->GetRotation(), glm::vec3(0.0, 0.0, 1.0));
	glm::mat4 parent_translation_matrix = glm::translate(glm::mat4(1.0f), parent_->GetPosition());
	glm::mat4 parent_transformation_matrix = parent_translation_matrix;

	// Setup the transformation matrix for the shader
	glm::mat4 transformation_matrix = parent_transformation_matrix * translation_matrix * rotation_matrix * scaling_matrix;

	// Set the transformation matrix in the shader
	shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

	// Set up the geometry
	geometry_->SetGeometry(shader_->GetShaderProgram());

	// Bind the particle texture
	glBindTexture(GL_TEXTURE_2D, texture_);

	// Draw the entity
	glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
}
} // namespace game
