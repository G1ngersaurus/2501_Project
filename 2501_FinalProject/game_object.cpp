#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "game_object.h"

namespace game {

GameObject::GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health) 
{

    // Initialize all attributes
    position_ = position;
    scale_ = 1.0;
    angle_ = 0.0;
    geometry_ = geom;
    shader_ = shader;
    texture_ = texture;
    velocity_ = glm::vec3(0.0f, 0.0f, 0.0f);
    type_ = GenericObj;
    yScale_ = yScale;
    xScale_ = xScale;
    health_ = health;
    alive_ = true;
    particles_ = nullptr;
    death_timer_ = new Timer();
    invincible_ = false;
    powerup_timer_ = new Timer();
}

GameObject::~GameObject() {

    if (particles_ != nullptr) particles_->SetAlive(false);
}

void GameObject::PowerUp(void) {
    invincible_ = true;
    powerup_timer_->Start(10);
}

void GameObject::SetAlive(bool value) {
    alive_ = false;
    //if (particles_ != nullptr) particles_->SetAlive(false);
}

bool GameObject::TakeDamage(int value) {
    health_ -= value;
    if (health_ <= 0) return true;
    return false;
}

glm::vec3 GameObject::GetBearing(void) const {

    glm::vec3 dir(cos(angle_), sin(angle_), 0.0);
    return dir;
}

glm::vec3 GameObject::GetBottomLeft(void) const {
    float pi_over_four = glm::pi<float>() / 4.0f;
    glm::vec3 dir(cos(angle_ - pi_over_four), sin(angle_ - pi_over_four), 0.0);
    return dir;
}


glm::vec3 GameObject::GetBottomRight(void) const {
    float pi_over_four = glm::pi<float>() / 4.0f;
    glm::vec3 dir(cos(angle_ + pi_over_four), sin(angle_ + pi_over_four), 0.0);
    return dir;
}

glm::vec3 GameObject::GetRight(void) const {

    float pi_over_two = glm::pi<float>() / 2.0f;
    glm::vec3 dir(cos(angle_ - pi_over_two), sin(angle_ - pi_over_two), 0.0);
    return dir;
}


void GameObject::SetRotation(float angle){ 

    // Set rotation angle of the game object
    // Make sure angle is in the range [0, 2*pi]
    float two_pi = 2.0f*glm::pi<float>();
    angle = fmod(angle, two_pi);
    if (angle < 0.0){
        angle += two_pi;
    }
    angle_ = angle;
}


void GameObject::Update(double delta_time) {
    if (powerup_timer_->Finished()) invincible_ = false;
    position_ += velocity_*((float) delta_time);
}


void GameObject::Render(glm::mat4 view_matrix, double current_time){

    // Set up the shader
    shader_->Enable();

    // Set up the view matrix
    shader_->SetUniformMat4("view_matrix", view_matrix);

    // Setup the scaling matrix for the shader
    glm::mat4 scaling_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale_ * xScale_, scale_ * yScale_, 1.0));

    // Setup the rotation matrix for the shader
    glm::mat4 rotation_matrix = glm::rotate(glm::mat4(1.0f), angle_, glm::vec3(0.0, 0.0, 1.0));

    // Set up the translation matrix for the shader
    glm::mat4 translation_matrix = glm::translate(glm::mat4(1.0f), position_);

    // Setup the transformation matrix for the shader
    glm::mat4 transformation_matrix = translation_matrix * rotation_matrix * scaling_matrix;

    // Set the transformation matrix in the shader
    shader_->SetUniformMat4("transformation_matrix", transformation_matrix);

    // Set up the geometry
    geometry_->SetGeometry(shader_->GetShaderProgram());

    // Bind the entity's texture
    glBindTexture(GL_TEXTURE_2D, texture_);

    // Draw the entity
    glDrawElements(GL_TRIANGLES, geometry_->GetSize(), GL_UNSIGNED_INT, 0);
}

} // namespace game
