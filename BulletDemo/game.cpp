#include <stdexcept>
#include <string>
#define GLM_FORCE_RADIANS
#include <glm/gtc/matrix_transform.hpp> 
#include <SOIL/SOIL.h>
#include <iostream>

#include <path_config.h>

#include "sprite.h"
#include "particles.h"
#include "shader.h"
#include "player_game_object.h"
#include "particle_system.h"
#include "game.h"
#include "enemy_game_object.h"
#include "bullet.h"

namespace game {

// Some configuration constants
// They are written here as global variables, but ideally they should be loaded from a configuration file

// Globals that define the OpenGL window and viewport
const char *window_title_g = "2501 Project";
const unsigned int window_width_g = 800;
const unsigned int window_height_g = 600;
const glm::vec3 viewport_background_color_g(0.0, 0.0, 1.0);

// Directory with game resources such as textures
const std::string resources_directory_g = RESOURCES_DIRECTORY;


Game::Game(void)
{
    // Don't do work in the constructor, leave it for the Init() function
}


void Game::Init(void)
{

    // Initialize the window management library (GLFW)
    if (!glfwInit()) {
        throw(std::runtime_error(std::string("Could not initialize the GLFW library")));
    }

    // Set whether window can be resized
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE); 

    // Create a window and its OpenGL context
    window_ = glfwCreateWindow(window_width_g, window_height_g, window_title_g, NULL, NULL);
    if (!window_) {
        glfwTerminate();
        throw(std::runtime_error(std::string("Could not create window")));
    }

    // Make the window's OpenGL context the current one
    glfwMakeContextCurrent(window_);

    // Initialize the GLEW library to access OpenGL extensions
    // Need to do it after initializing an OpenGL context
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        throw(std::runtime_error(std::string("Could not initialize the GLEW library: ") + std::string((const char *)glewGetErrorString(err))));
    }

    // Set event callbacks
    glfwSetFramebufferSizeCallback(window_, ResizeCallback);

    // Initialize sprite geometry
    sprite_ = new Sprite();
    sprite_->CreateGeometry();

    // Initialize particle geometry
    particles_ = new Particles();
    particles_->CreateGeometry();

    // Initialize particle shader
    particle_shader_.Init((resources_directory_g+std::string("/particle_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/particle_fragment_shader.glsl")).c_str());

    // Initialize sprite shader
    sprite_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/sprite_fragment_shader.glsl")).c_str());

    // Initialize time
    current_time_ = 0.0;

    // Shooting cool down
    next_shot_ = 0.0;
}


Game::~Game()
{
    // Free memory for all objects
    // Only need to delete objects that are not automatically freed
    delete sprite_;
    delete particles_;
    for (int i = 0; i < game_objects_.size(); i++){
        delete game_objects_[i];
    }

    // Close window
    glfwDestroyWindow(window_);
    glfwTerminate();
}


void Game::Setup(void)
{

    // Setup the game world

    // Load textures
    SetAllTextures();

    // Setup the player object (position, texture, vertex count)
    // Note that, in this specific implementation, the player object should always be the first object in the game object vector 
    PlayerGameObject *player = new PlayerGameObject(glm::vec3(1.0f, -1.0f, 0.0f), sprite_, &sprite_shader_, tex_[6]);
    float pi_over_two = glm::pi<float>() / 2.0f;
    player->SetRotation(pi_over_two);
    //player->SetScale(1.0);
    game_objects_.push_back(player);

    // Setup other objects
    EnemyGameObject *enemy1 = new EnemyGameObject(glm::vec3(-2.0f, -2.0f, 0.0f), sprite_, &sprite_shader_, tex_[1]);
    enemy1->SetRotation(pi_over_two);
    enemy1->SetTarget(player);
    enemy1->SetVelocity(glm::vec3(0.0, 0.5, 0.0));
    game_objects_.push_back(enemy1);
    
    //game_objects_.push_back(new GameObject(glm::vec3(1.0f, -0.5f, 0.0f), sprite_, &sprite_shader_, tex_[2]));
    //game_objects_[2]->SetRotation(pi_over_two);

    // Setup background
    // In this specific implementation, the background is always the
    // last object
    GameObject *background = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), sprite_, &sprite_shader_, tex_[8]);
    background->SetScale(20.0);
    game_objects_.push_back(background);

    // Setup particle system
    GameObject *particles = new ParticleSystem(glm::vec3(-0.5f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], game_objects_[0]);
    particles->SetScale(0.2);
    particles->SetRotation(-pi_over_two);
    game_objects_.push_back(particles);
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height)
{

    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);
}


void Game::SetTexture(GLuint w, const char *fname)
{
    // Bind texture buffer
    glBindTexture(GL_TEXTURE_2D, w);

    // Load texture from a file to the buffer
    int width, height;
    unsigned char* image = SOIL_load_image(fname, &width, &height, 0, SOIL_LOAD_RGBA);
    if (!image){
        std::cout << "Cannot load texture " << fname << std::endl;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    SOIL_free_image_data(image);

    // Texture Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Game::SetAllTextures(void)
{
    // Load all textures that we will need
    // Declare all the textures here
    const char *texture[] = {"/textures/destroyer_red.png", "/textures/destroyer_green.png", "/textures/destroyer_blue.png", "/textures/stars2.png", "/textures/orb.png", "/textures/bullet.png", "/textures/player_sub.png", "/textures/water1.png", "/textures/water2.png"};
    // Get number of declared textures
    int num_textures = sizeof(texture) / sizeof(char *);
    // Allocate a buffer for all texture references
    tex_ = new GLuint[num_textures];
    glGenTextures(num_textures, tex_);
    // Load each texture
    for (int i = 0; i < num_textures; i++){
        SetTexture(tex_[i], (resources_directory_g+std::string(texture[i])).c_str());
    }
    // Set first texture in the array as default
    glBindTexture(GL_TEXTURE_2D, tex_[0]);
}


void Game::MainLoop(void)
{
    // Loop while the user did not close the window
    double last_time = glfwGetTime();
    while (!glfwWindowShouldClose(window_)){

        // Calculate delta time
        double current_time = glfwGetTime();
        double delta_time = current_time - last_time;
        last_time = current_time;

        // Update window events like input handling
        glfwPollEvents();

        // Handle user input
        HandleControls(delta_time);

        // Update all the game objects
        Update(delta_time);

        // Render all the game objects
        Render();

        // Push buffer drawn in the background onto the display
        glfwSwapBuffers(window_);
    }
}


void Game::HandleControls(double delta_time)
{
    // Get player game object
    GameObject *player = game_objects_[0];
    // Get current position and angle
    glm::vec3 curpos = player->GetPosition();
    float angle = player->GetRotation();
    // Compute current bearing direction
    glm::vec3 dir = player->GetBearing();
    // Adjust motion increment and angle increment 
    // if translation or rotation is too slow
    float speed = delta_time*2000.0;
    float motion_increment = 0.001*speed;
    float angle_increment = (glm::pi<float>() / 1800.0f)*speed;

    // Check for player input and make changes accordingly
    if (glfwGetKey(window_, GLFW_KEY_W) == GLFW_PRESS) {
        player->SetPosition(curpos + motion_increment*dir);
    }
    if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        player->SetPosition(curpos - motion_increment*dir);
    }
    //if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    //    player->SetRotation(angle - angle_increment);
    //}
    //if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    //    player->SetRotation(angle + angle_increment);
    //}
    if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        player->SetPosition(curpos - motion_increment*player->GetRight());
    }
    if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        player->SetPosition(curpos + motion_increment*player->GetRight());
    }
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        if (current_time_ > next_shot_){
            GameObject *player = game_objects_[0];
            Bullet *bullet = new Bullet(player->GetPosition(), sprite_, &sprite_shader_, tex_[5]);
            bullet->SetRotation(player->GetRotation()-glm::pi<float>()/2.0);
            bullet->SetVelocity(10.0f*player->GetBearing());
            bullet->SetScale(0.5f);
            bullet->SetOrigin(player->GetPosition());
            game_objects_.insert(game_objects_.end()-2, bullet);
            next_shot_ = current_time_ + 0.5;
        }
    }
}


void Game::Update(double delta_time)
{

    // Update time
    current_time_ += delta_time;

    // List of objects to be removed at the end of the update
    std::vector<GameObject*> to_erase;
    
    // Update all game objects
    for (int i = 0; i < game_objects_.size(); i++) {
        // Get the current game object
        GameObject* current_game_object = game_objects_[i];

        // Update the current game object
        current_game_object->Update(delta_time);

        // Check for collision with other game objects
        if (current_game_object->GetType() == BulletObj){
            // If the object is a bullet, use its special collision function
            // Cast the object into a bullet
            Bullet *bullet = dynamic_cast<Bullet*>(current_game_object);
            // Check for ray-circle collision with enemy game objects
            for (int j = 0; j < (game_objects_.size() - 1); j++) {
                GameObject* other_game_object = game_objects_[j];
                if (other_game_object->GetType() == EnemyObj){
                    if (bullet->CheckCollision(other_game_object->GetPosition(),
                            other_game_object->GetScale()*0.6)){
                        // Don't remove objects now
                        // Add them to a list to remove them later (code down below)
                        to_erase.push_back(other_game_object);
                        to_erase.push_back(bullet);
                        // If an intersection happened and objects will be removed,
                        // don't check for more intersections
                        break;
                    }
                }
            }
        } else {
            // Note the loop bounds: we avoid testing the last object since
            // it's the background covering the whole game world
            for (int j = i + 1; j < (game_objects_.size()-1); j++) {
                GameObject* other_game_object = game_objects_[j];

                // Compute distance between object i and object j
                float distance = glm::length(current_game_object->GetPosition() - other_game_object->GetPosition());
                // If distance is below a threshold, we have a collision
                if (distance < 0.8f) {
                    // This is where you would perform collision response between objects
                }
            }
        }
    }

    // Remove game objects save in the to_erase vector
    for (int i = 0; i < to_erase.size(); i++){
        for (int j = 0; j < game_objects_.size(); j++){
            if (game_objects_[j] == to_erase[i]){
                game_objects_.erase(game_objects_.begin() + j);
                break;
            }
        }
    }
}


void Game::Render(void){

    // Clear background
    glClearColor(viewport_background_color_g.r,
                 viewport_background_color_g.g,
                 viewport_background_color_g.b, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use aspect ratio to properly scale the window
    int width, height;
    glfwGetWindowSize(window_, &width, &height);
    glm::mat4 window_scale_matrix;
    if (width > height){
        float aspect_ratio = ((float) width)/((float) height);
        window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f/aspect_ratio, 1.0f, 1.0f));
    } else {
        float aspect_ratio = ((float) height)/((float) width);
        window_scale_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f/aspect_ratio, 1.0f));
    }

    // Set view to zoom out, centered by default at 0,0
    float camera_zoom = 0.25f;
    glm::mat4 camera_zoom_matrix = glm::scale(glm::mat4(1.0f), glm::vec3(camera_zoom, camera_zoom, camera_zoom));
    glm::mat4 view_matrix = window_scale_matrix * camera_zoom_matrix;

    // Render all game objects
    for (int i = 0; i < game_objects_.size(); i++) {
        game_objects_[i]->Render(view_matrix, current_time_);
    }
}
      
} // namespace game
