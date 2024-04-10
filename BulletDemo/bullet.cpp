#include "bullet.h"
#include <iostream>
#include <glm/gtc/constants.hpp>

namespace game {

Bullet::Bullet(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health)
	: GameObject(position, geom, shader, texture, yScale, xScale, health) {
	lifespan_ = new Timer();
	current_time_ = 0.0;
	last_time_ = 0.0;
	type_ = BulletObj;
	lifespan_->Start(2.0);
}

Bullet::~Bullet() {
	std::cout << "DELTED" << std::endl;
	if (particles_ != nullptr) {
		std::cout << particles_->GetRotation() << std::endl;
		particles_->SetAlive(false);
	}
}

// Check for possible intersections between a ray (P, d) and a circle (C, r)
// Return the two intersection parameters in t1 and t2
bool RayCollision(glm::vec2 P, glm::vec2 d, glm::vec2 C, float r, float &t1, float &t2){

	// Equations explained in the lecture slides
	glm::vec2 CP = P - C;
	float a = glm::dot(d, d);
	float b = glm::dot(2.0f*d, CP);
	float c = glm::dot(CP, CP) - r*r;

	// Solve the quadratic equation
	float delta = b*b - 4*a*c;
	if (delta < 0.0){
		return false;
	}

	float sq = sqrt(delta);
	t1 = (-b + sq)/(2.0*a);
	t2 = (-b - sq)/(2.0*a);
	return true;
}

bool Bullet::CheckCollision(glm::vec3 C, float r){

	// Variables to hold output of collision function
	float t1, t2;

	// Conver input in vec3 format to vec2 format
	glm::vec2 P = glm::vec2(origin_.x, origin_.y);
	glm::vec2 d = glm::vec2(velocity_.x, velocity_.y);
	glm::vec2 C2 = glm::vec2(C.x, C.y);

	// Find possible intersections
	if (RayCollision(P, d, C2, r, t1, t2)){
		// Check if the possible intersections happened

		// Make sure t1 < t2
		if (t2 < t1){
			float temp = t1;
			t1 = t2;
			t2 = temp;
		}

		// Check if current position is between two intersections
		// Works for slow speeds
		/*if ((current_time_ > t1) && (current_time_ < t2)){
			return true;
		}*/

		// Check for an intersection between the last position and current position
		// of the bullet
		if ((t1 > last_time_) && (t1 < current_time_)){
			return true;
		}
		if ((t2 > last_time_) && (t2 < current_time_)) {
			return true;
		}
	}
	return false;
}


// Update function for moving the player object around
void Bullet::Update(double delta_time) {

	// Call the parent's update method to move the object in standard way, if desired
	//GameObject::Update(delta_time);

	// Update current and last time
	last_time_ = current_time_;
	current_time_ += delta_time;

	// Update position based on current time, origin, and bullet direction
	position_ = origin_ + ((float)current_time_)*velocity_;
	if (lifespan_->Finished()) alive_ = false;

	// Add some code to limit the lifespan of the bullet...
}

} // namespace game
