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
#include "tile.h"
#include "text_game_object.h"
#include "mine_enemy_object.h"
#include "shark_enemy_object.h"
#include "sub_enemy_object.h"
#include "boss_sub_object.h"
#include "boss_turret_object.h"

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

    tile_ = new Tile();
    tile_->CreateGeometry();

    // Initialize particle shader
    particle_shader_.Init((resources_directory_g+std::string("/particle_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/particle_fragment_shader.glsl")).c_str());

    // Initialize sprite shader
    sprite_shader_.Init((resources_directory_g+std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g+std::string("/sprite_fragment_shader.glsl")).c_str());

    text_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/text_fragment_shader.glsl")).c_str());

    // Initialize drawing shader
    drawing_shader_.Init((resources_directory_g + std::string("/sprite_vertex_shader.glsl")).c_str(), (resources_directory_g + std::string("/drawing_fragment_shader.glsl")).c_str());

    // Initialize time
    current_time_ = 0.0;

    // Shooting cool down
    next_shot_ = 0.0;

    lastSecond_ = -1;
    camera_position_ = glm::vec3(0.0f, 0.0f, 0.0f);
    score_ = 0;
    killcount_ = 0;
    player_dead_ = false;
    turret_killcount_ = 0;
    boss_dead_ = false;
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
    PlayerGameObject *player = new PlayerGameObject(glm::vec3(1.0f, -1.0f, 0.0f), sprite_, &sprite_shader_, tex_[6], 0.8f, 2.0f, 10);
    float pi_over_two = glm::pi<float>() / 2.0f;
    player->SetRotation(pi_over_two);
    //player->SetScale(1.0);
    player->SetType(PlayerObj);
    game_objects_.push_back(player);

    //// Setup other objects
    //MineEnemyObject *enemy1 = new MineEnemyObject(glm::vec3(2.0f, 4.0f, 0.0f), sprite_, &sprite_shader_, tex_[11], 1.0f, 1.0f, 1, glm::vec3(2.0f, 6.0f, 0.0f));
    ////enemy1->SetVelocity(glm::vec3(0.0, 1.0, 0.0));
    //enemy1->SetType(MineObj);
    //game_objects_.push_back(enemy1);


    //SharkEnemyObject* enemy2 = new SharkEnemyObject(glm::vec3(-2.0f, 6.0f, 0.0f), sprite_, &sprite_shader_, tex_[12], 1.0f, 1.0f, 5);
    //enemy2->SetTarget(player);
    //enemy2->SetScale(2.0f);
    //enemy2->SetType(SharkObj);
    ////enemy1->SetVelocity(glm::vec3(0.0, 1.0, 0.0));
    //game_objects_.push_back(enemy2);

    //SubEnemyObject* enemy3 = new SubEnemyObject(glm::vec3(-3.0f, 3.0f, 0.0f), sprite_, &sprite_shader_, tex_[13], 0.6f, 1.8f, 1);
    //enemy3->SetTarget(player);
    //enemy3->SetRotation(3* pi_over_two);
    //enemy3->SetType(SubObj);
    //game_objects_.push_back(enemy3);
    BossSubObject* boss = new BossSubObject(glm::vec3(0.0f, 5.0f, 0.0f), sprite_, &sprite_shader_, tex_[15], 1.0f, 1.25f, 20);
    boss->SetScale(8.0f);
    boss->SetTarget(player);
    //boss->SetRotation(3* pi_over_two);
    boss->SetType(BossObj);
    game_objects_.push_back(boss);

    BossTurretObject* turret1 = new BossTurretObject(glm::vec3(-3.55f, 1.4f, -0.5f), sprite_, &sprite_shader_, tex_[16], boss, glm::vec3(-3.55f, 1.4f, -0.5f), 1.0f, 1.0f, 10);
    turret1->SetTarget(player);
    turret1->SetScale(3.0f);
    turret1->SetRotation(-pi_over_two);
    turret1->SetType(TurretObj);
    BossTurretObject* turret2 = new BossTurretObject(glm::vec3(-1.95f, 1.4f, -0.5f), sprite_, &sprite_shader_, tex_[16], boss, glm::vec3(-3.55f, 1.4f, -0.5f), 1.0f, 1.0f, 10);
    turret2->SetTarget(player);
    turret2->SetScale(3.0f);
    turret2->SetRotation(-pi_over_two);
    turret2->SetType(TurretObj);
    BossTurretObject* turret3 = new BossTurretObject(glm::vec3(1.95f, 1.4f, -0.5f), sprite_, &sprite_shader_, tex_[16], boss, glm::vec3(-3.55f, 1.4f, -0.5f), 1.0f, 1.0f, 10);
    turret3->SetTarget(player);
    turret3->SetScale(3.0f);
    turret3->SetRotation(-pi_over_two);
    turret3->SetType(TurretObj);
    BossTurretObject* turret4 = new BossTurretObject(glm::vec3(3.55f, 1.4f, -0.5f), sprite_, &sprite_shader_, tex_[16], boss, glm::vec3(-3.55f, 1.4f, -0.5f), 1.0f, 1.0f, 10);
    turret4->SetTarget(player);
    turret4->SetScale(3.0f);
    turret4->SetRotation(-pi_over_two);
    turret4->SetType(TurretObj);

    game_objects_.push_back(turret1);
    game_objects_.push_back(turret2);
    game_objects_.push_back(turret3);
    game_objects_.push_back(turret4);

    //game_objects_.push_back(new GameObject(glm::vec3(1.0f, -0.5f, 0.0f), sprite_, &sprite_shader_, tex_[2]));
    //game_objects_[2]->SetRotation(pi_over_two);

    TextGameObject* text = new TextGameObject(camera_position_ + glm::vec3(-4.1f, -3.75f, -1.0f), sprite_, &text_shader_, tex_[10], 1.0f, 5.0f, 1);
    text->SetScale(0.5f);
    text->SetType(TimerObj);
    text->SetVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    std::string time = std::to_string(lastSecond_);
    text->SetText("Time: " + time);
    game_objects_.push_back(text);

    TextGameObject* text2 = new TextGameObject(camera_position_ + glm::vec3(-0.35f, -3.75f, -1.0f), sprite_, &text_shader_, tex_[10], 0.5f, 5.0f, 1);
    text2->SetScale(1.0f);
    text2->SetType(HealthObj);
    text2->SetVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    std::string health = std::to_string((player->GetHealth() / 10) * 100);
    text2->SetText("|Hull Integrity: " + health + "%");
    game_objects_.push_back(text2);

    TextGameObject* text3 = new TextGameObject(camera_position_ + glm::vec3(3.7f, -3.75f, -1.0f), sprite_, &text_shader_, tex_[10], 0.5f, 3.35f, 1);
    text3->SetScale(1.0f);
    text3->SetType(ScoreObj);
    text3->SetVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    std::string score = std::to_string(score_); 
    text3->SetText("|Score: " + score); 
    game_objects_.push_back(text3); 
    // Setup background
    // In this specific implementation, the background is always the
    // last object
    GameObject *background = new GameObject(glm::vec3(0.0f, 0.0f, 0.0f), tile_, &sprite_shader_, tex_[8], 1.0f, 1.0f, 1);
    background->SetScale(500.0);
    game_objects_.push_back(background);

    // Setup particle system
    GameObject *particles = new ParticleSystem(glm::vec3(-1.0f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], game_objects_[0], 1.0f, 1.0f, 1);
    particles->SetScale(0.2);
    particles->SetRotation(-pi_over_two);
    game_objects_.push_back(particles);
}


void Game::ResizeCallback(GLFWwindow* window, int width, int height)
{

    // Set OpenGL viewport based on framebuffer width and height
    glViewport(0, 0, width, height);
}


void Game::SetTexture(GLuint w, const char *fname, bool background)
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
    if (background) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    

    // Texture Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}


void Game::SetAllTextures(void)
{
    // Load all textures that we will need
    // Declare all the textures here
    const char *texture[] = {"/textures/destroyer_red.png", "/textures/destroyer_green.png", "/textures/destroyer_blue.png", 
        "/textures/stars2.png", "/textures/orb.png", "/textures/bullet.png", "/textures/player_sub.png", 
        "/textures/water1.png", "/textures/water2.png", "/textures/sonic_javelin.png", "/textures/font.png", 
        "/textures/mine_enemy.png", "/textures/shark_enemy.png", "/textures/enemy_sub.png", "/textures/torpedo.png", "/textures/red_white_blue_october.png",
        "/textures/rwb_turret.png"
    };
    // Get number of declared textures
    int num_textures = sizeof(texture) / sizeof(char *);
    // Allocate a buffer for all texture references
    tex_ = new GLuint[num_textures];
    glGenTextures(num_textures, tex_);
    // Load each texture
    for (int i = 0; i < num_textures; i++){
        if (i == 8) {
            SetTexture(tex_[i], (resources_directory_g + std::string(texture[i])).c_str(), true);
        }
        else SetTexture(tex_[i], (resources_directory_g+std::string(texture[i])).c_str(), false);
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
        Render(delta_time);

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
        //player->SetPosition(curpos + motion_increment*dir);
        player->SetVelocity(glm::vec3(0.0f, 5.0f, 0.0f));
    }
    else if (glfwGetKey(window_, GLFW_KEY_S) == GLFW_PRESS) {
        //player->SetPosition(curpos - motion_increment*dir);
        player->SetVelocity(glm::vec3(0.0f, -3.0f, 0.0f));
    }
    //if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
    //    player->SetRotation(angle - angle_increment);
    //}
    //if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
    //    player->SetRotation(angle + angle_increment);
    //}
    else if (glfwGetKey(window_, GLFW_KEY_A) == GLFW_PRESS) {
        //player->SetPosition(curpos - motion_increment*player->GetRight());
        player->SetVelocity(glm::vec3(-4.0f, 1.0f, 0.0f));
    }
    else if (glfwGetKey(window_, GLFW_KEY_D) == GLFW_PRESS) {
        //player->SetPosition(curpos + motion_increment*player->GetRight());
        player->SetVelocity(glm::vec3(4.0f, 1.0f, 0.0f));
    }
    else {
        player->SetVelocity(glm::vec3(0.0f, 1.0f, 0.0f));
    }
    if (glfwGetKey(window_, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window_, true);
    }
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_1) == GLFW_PRESS) {
        if (current_time_ > next_shot_){
            GameObject *player = game_objects_[0];
            Bullet *bullet = new Bullet(player->GetPosition(), sprite_, &sprite_shader_, tex_[9], 1.0f, 1.0f, 1);
            bullet->SetRotation(player->GetRotation()-glm::pi<float>()/2.0);
            bullet->SetVelocity(18.0f*player->GetBearing());
            //bullet->SetScale(0.5f);
            bullet->SetOrigin(player->GetPosition());
            game_objects_.insert(game_objects_.begin()+1, bullet);
            next_shot_ = current_time_ + 0.25;
        }
    }
    if (glfwGetMouseButton(window_, GLFW_MOUSE_BUTTON_2) == GLFW_PRESS) {
        if (current_time_ > next_torpedo_) {
            GameObject* player = game_objects_[0];
            Bullet* torpedo = new Bullet(player->GetPosition(), sprite_, &sprite_shader_, tex_[14], 1.0f, 1.0f, 1);
            torpedo->SetRotation(glm::pi<float>() / 2.0);
            torpedo->SetVelocity(8.0f * player->GetBearing());
            //bullet->SetScale(0.5f);
            torpedo->SetOrigin(player->GetPosition());
            torpedo->SetType(TorpedoObj);
            game_objects_.insert(game_objects_.begin() + 1, torpedo);

            GameObject* particles = new ParticleSystem(glm::vec3(-0.6f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], torpedo, 1.0f, 1.0f, 1);
            particles->SetScale(0.1);
            particles->SetRotation(-glm::pi<float>() / 2.0);
            torpedo->SetParticles(particles);
            

            
            game_objects_.push_back(particles);
            next_torpedo_ = current_time_ + 5.0;
        }
    }
    
}


void Game::Update(double delta_time)
{
    // hold player for certain purposes
    PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(game_objects_[0]);
    // Update time
    current_time_ += delta_time;

    score_ = (killcount_ * 100);
    //if ((fmod(current_time_, 1) <= 0.01) && (fmod(current_time_, 1) >= 0.0)) std::cout << current_time_ << std::endl;
    //std::cout << fmod(current_time_, 1) << std::endl;
    seconds_ = (int)(current_time_ + 0.5);
    if (seconds_ > lastSecond_) {
        lastSecond_ = seconds_;
        if (fmod(lastSecond_, 10) == 0 && lastSecond_ > 0) {
            /*MineEnemyObject* newMine = new MineEnemyObject(camera_position_ + glm::vec3(0.0f, 4.0f, 0.0f), sprite_, &sprite_shader_, tex_[11], 1.0f, 1.0f, 1, camera_position_ + glm::vec3(0.0f, 5.0f, 0.0f));
            newMine->SetType(MineObj);
            game_objects_.insert(game_objects_.end() - 2, newMine);
            SharkEnemyObject* newShark = new SharkEnemyObject(camera_position_ + glm::vec3(-3.0f, 6.0f, 0.0f), sprite_, &sprite_shader_, tex_[12], 1.0f, 1.0f, 1);
            newShark->SetTarget(player);
            newShark->SetScale(2.0f);
            newShark->SetType(SharkObj);
            newShark->SetNextShot(current_time_ + 1.0);
            game_objects_.insert(game_objects_.end() - 2, newShark);*/


        }
    }

    

    // List of objects to be removed at the end of the update
    std::vector<GameObject*> to_erase;
    
    // Update all game objects
    for (int i = 0; i < game_objects_.size(); i++) {
        // Get the current game object
        GameObject* current_game_object = game_objects_[i];
        

        // Update the current game object
        if (current_game_object->GetType() == PlayerObj) {
            PlayerGameObject* player = dynamic_cast<PlayerGameObject*>(current_game_object);
            player->Update(delta_time, camera_position_);
        }
        else if (current_game_object->GetType() == TimerObj) {
            TextGameObject* timer = dynamic_cast<TextGameObject*>(current_game_object);
            timer->Update(delta_time);
            std::string time = std::to_string(lastSecond_);
            timer->SetText("Time: " + time);
        }
        else if (current_game_object->GetType() == HealthObj) {
            TextGameObject* health = dynamic_cast<TextGameObject*>(current_game_object);
            health->Update(delta_time);
            float percent = player->GetHealth();
            percent /= 10;
            std::string healthVal = std::to_string((int)(percent * 100));
            health->SetText("|Hull Integrity: " + healthVal + "%");
        }
        else if (current_game_object->GetType() == ScoreObj) {
            TextGameObject* score = dynamic_cast<TextGameObject*>(current_game_object);
            score->Update(delta_time);
            std::string scoreVal = std::to_string(score_);
            score->SetText("|Score: " + scoreVal);
        }
        else if (current_game_object->GetType() == SubObj){
            SubEnemyObject* sub = dynamic_cast<SubEnemyObject*>(current_game_object);
            sub->Update(delta_time, camera_position_);

            if (current_time_ > sub->GetNextShot()) {
                Bullet* torpedo1 = new Bullet(sub->GetPosition(), sprite_, &sprite_shader_, tex_[14], 1.0f, 1.0f, 1);
                torpedo1->SetRotation(sub->GetRotation() + glm::pi<float>() / 4.0);
                torpedo1->SetVelocity(3.0f * sub->GetBottomRight());
                torpedo1->SetScale(0.75f);
                torpedo1->SetOrigin(sub->GetPosition() - glm::vec3(0.0f, 0.55f, 0.0f));
                torpedo1->SetType(SubTorpedoObj);
                game_objects_.insert(game_objects_.begin() + 1, torpedo1);

                GameObject* particles1 = new ParticleSystem(glm::vec3(-0.45f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], torpedo1, 1.0f, 1.0f, 1);
                particles1->SetScale(0.1);
                particles1->SetRotation(sub->GetRotation());
                torpedo1->SetParticles(particles1);
                game_objects_.push_back(particles1);
                
                Bullet* torpedo2 = new Bullet(sub->GetPosition(), sprite_, &sprite_shader_, tex_[14], 1.0f, 1.0f, 1);
                torpedo2->SetRotation(sub->GetRotation() - glm::pi<float>() / 4.0);
                torpedo2->SetVelocity(3.0f * sub->GetBottomLeft());
                torpedo2->SetScale(0.75f);
                torpedo2->SetOrigin(sub->GetPosition() - glm::vec3(0.0f, 0.55f, 0.0f));
                torpedo2->SetType(SubTorpedoObj);
                game_objects_.insert(game_objects_.begin() + 1, torpedo2);
                sub->SetNextShot(current_time_ + 3.0);

                GameObject* particles2 = new ParticleSystem(glm::vec3(-0.45f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], torpedo2, 1.0f, 1.0f, 1);
                particles2->SetScale(0.1);
                particles2->SetRotation(sub->GetRotation());
                torpedo2->SetParticles(particles2);
                game_objects_.push_back(particles2);
            }
        }
        else if (current_game_object->GetType() == BossObj) {
            BossSubObject* boss = dynamic_cast<BossSubObject*>(current_game_object);
            boss->Update(delta_time, camera_position_);

            if (current_time_ > boss->GetNextShot()) {
                Bullet* torpedo1 = new Bullet(boss->GetPosition(), sprite_, &sprite_shader_, tex_[14], 1.0f, 1.0f, 1);
                torpedo1->SetRotation(boss->GetRotation() - (3 * glm::pi<float>()) / 4.0);
                torpedo1->SetVelocity(3.0f * -boss->GetBottomRight());
                torpedo1->SetScale(0.75f);
                torpedo1->SetOrigin(boss->GetPosition() + glm::vec3(0.0f, 0.8f, 0.0f));
                torpedo1->SetType(SubTorpedoObj);
                game_objects_.insert(game_objects_.begin() + 1, torpedo1);

                GameObject* particles1 = new ParticleSystem(glm::vec3(-0.45f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], torpedo1, 1.0f, 1.0f, 1);
                particles1->SetScale(0.1);
                particles1->SetRotation(torpedo1->GetRotation() + glm::pi<float>() / 4.0);
                torpedo1->SetParticles(particles1);
                game_objects_.push_back(particles1);

                Bullet* torpedo2 = new Bullet(boss->GetPosition(), sprite_, &sprite_shader_, tex_[14], 1.0f, 1.0f, 1);
                torpedo2->SetRotation(boss->GetRotation() - glm::pi<float>() / 4.0);
                torpedo2->SetVelocity(3.0f * boss->GetBottomLeft());
                torpedo2->SetScale(0.75f);
                torpedo2->SetOrigin(boss->GetPosition() + glm::vec3(0.0f, 0.8f, 0.0f));
                torpedo2->SetType(SubTorpedoObj);
                game_objects_.insert(game_objects_.begin() + 1, torpedo2);

                GameObject* particles2 = new ParticleSystem(glm::vec3(-0.45f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], torpedo2, 1.0f, 1.0f, 1);
                particles2->SetScale(0.1);
                particles2->SetRotation(torpedo2->GetRotation() - glm::pi<float>() / 4.0);
                torpedo2->SetParticles(particles2);
                game_objects_.push_back(particles2);

                Bullet* torpedo3 = new Bullet(boss->GetPosition(), sprite_, &sprite_shader_, tex_[14], 1.0f, 1.0f, 1);
                torpedo3->SetRotation(boss->GetRotation() - glm::pi<float>() / 2.0);
                torpedo3->SetVelocity(3.0f * boss->GetRight());
                torpedo3->SetScale(0.75f);
                torpedo3->SetOrigin(boss->GetPosition() + glm::vec3(0.0f, 0.8f, 0.0f));
                torpedo3->SetType(SubTorpedoObj);
                game_objects_.insert(game_objects_.begin() + 1, torpedo3);
                boss->SetNextShot(current_time_ + 3.0);

                GameObject* particles3 = new ParticleSystem(glm::vec3(-0.45f, 0.0f, 0.0f), particles_, &particle_shader_, tex_[4], torpedo3, 1.0f, 1.0f, 1);
                particles3->SetScale(0.1);
                particles3->SetRotation(torpedo3->GetRotation());
                torpedo3->SetParticles(particles3);
                game_objects_.push_back(particles3);
            }
        }
        else current_game_object->Update(delta_time);

        // Shark firing
        if (current_game_object->GetType() == SharkObj) {
            SharkEnemyObject* shark = dynamic_cast<SharkEnemyObject*>(current_game_object);
            
            if (current_time_ > shark->GetNextShot()) {
                Bullet* bullet = new Bullet(shark->GetPosition(), sprite_, &sprite_shader_, tex_[9], 1.0f, 1.0f, 1);
                bullet->SetRotation(shark->GetRotation() - glm::pi<float>() / 2.0);
                bullet->SetVelocity(4.0f * shark->GetBearing());
                bullet->SetOrigin(shark->GetPosition());
                bullet->SetType(SharkBulletObj);
                game_objects_.insert(game_objects_.begin() + 1, bullet);
                shark->SetNextShot(current_time_ + 2.0);
            }
        } // Turret firing
        else if (current_game_object->GetType() == TurretObj) {
            BossTurretObject* turret = dynamic_cast<BossTurretObject*>(current_game_object);

            if (current_time_ > turret->GetNextShot()) {
                Bullet* bullet = new Bullet(turret->GetFakePosition(), sprite_, &sprite_shader_, tex_[9], 1.0f, 1.0f, 1);
                bullet->SetRotation(turret->GetRotation() - glm::pi<float>());
                bullet->SetVelocity(5.0f * turret->GetRight());
                bullet->SetOrigin(turret->GetFakePosition());
                bullet->SetType(SharkBulletObj);
                game_objects_.insert(game_objects_.begin() + 1, bullet);
                turret->SetNextShot(current_time_ + 2.0);
            }

        }

        // Check for collision with other game objects
        if (current_game_object->GetType() == BulletObj){
            // If the object is a bullet, use its special collision function
            // Cast the object into a bullet
            Bullet *bullet = dynamic_cast<Bullet*>(current_game_object);
            // Check for ray-circle collision with enemy game objects
            for (int j = 0; j < (game_objects_.size() - 1); j++) {
                GameObject* other_game_object = game_objects_[j];
                if (other_game_object->GetType() == EnemyObj || other_game_object->GetType() == MineObj || 
                    other_game_object->GetType() == SharkObj || other_game_object->GetType() == SubObj){
                    if (bullet->CheckCollision(other_game_object->GetPosition(), other_game_object->GetScale()*0.45)){
                        // Don't remove objects now
                        // Add them to a list to remove them later (code down below)
                        //to_erase.push_back(othwer_game_object);
                        if (other_game_object->TakeDamage(1) == true) killcount_++;
                        //to_erase.push_back(bullet);
                        bullet->SetAlive(false);
                        // If an intersection happened and objects will be removed,
                        // don't check for more intersections
                        break;
                    }
                }
                else if (other_game_object->GetType() == TurretObj) {
                    BossTurretObject* turret = dynamic_cast<BossTurretObject*>(other_game_object);
                    if (bullet->CheckCollision(turret->GetFakePosition(), turret->GetScale() * 0.1)) {
                        // Don't remove objects now
                        // Add them to a list to remove them later (code down below)
                        //to_erase.push_back(other_game_object);
                        if (other_game_object->TakeDamage(1) == true) {
                            killcount_++;
                            turret_killcount_++;
                        }
                        //to_erase.push_back(bullet);
                        bullet->SetAlive(false);
                        // If an intersection happened and objects will be removed,
                        // don't check for more intersections
                        break;
                    }
                }
                else if (other_game_object->GetType() == BossObj && turret_killcount_ == 4) {
                    BossSubObject* boss = dynamic_cast<BossSubObject*>(other_game_object);

                    if (bullet->CheckCollision(boss->GetPosition(), boss->GetScale() * 0.1)) {
                        // Don't remove objects now
                        // Add them to a list to remove them later (code down below)
                        if (boss->TakeDamage(1) == true) boss_dead_ = true;
                        //to_erase.push_back(bullet);
                        bullet->SetAlive(false);
                        // If an intersection happened and objects will be removed,
                        // don't check for more intersections

                    }
                }
            }
        }
        else if (current_game_object->GetType() == TorpedoObj) {
            Bullet* bullet = dynamic_cast<Bullet*>(current_game_object);
            // Check for ray-circle collision with enemy game objects
            for (int j = 0; j < (game_objects_.size() - 1); j++) {
                GameObject* other_game_object = game_objects_[j];
                if (other_game_object->GetType() == EnemyObj || other_game_object->GetType() == MineObj ||
                    other_game_object->GetType() == SharkObj || other_game_object->GetType() == SubObj) {
                    if (bullet->CheckCollision(other_game_object->GetPosition(), other_game_object->GetScale() * 0.45)) {
                        // Don't remove objects now
                        // Add them to a list to remove them later (code down below)
                        //to_erase.push_back(other_game_object);
                        if (other_game_object->TakeDamage(3) == true) killcount_++;
                        //to_erase.push_back(bullet);
                        bullet->SetAlive(false);
                        // If an intersection happened and objects will be removed,
                        // don't check for more intersections
                        break;
                    }
                }
                else if (other_game_object->GetType() == TurretObj) {
                    BossTurretObject* turret = dynamic_cast<BossTurretObject*>(other_game_object);
                    if (bullet->CheckCollision(turret->GetFakePosition(), turret->GetScale() * 0.2)) {
                        // Don't remove objects now
                        // Add them to a list to remove them later (code down below)
                        //to_erase.push_back(other_game_object);
                        if (other_game_object->TakeDamage(3) == true) {
                            killcount_++;
                            turret_killcount_++;
                        }
                        //to_erase.push_back(bullet);
                        bullet->SetAlive(false);
                        // If an intersection happened and objects will be removed,
                        // don't check for more intersections
                        break;
                    }
                }
                else if (other_game_object->GetType() == BossObj && turret_killcount_ == 4) {
                    BossSubObject* boss = dynamic_cast<BossSubObject*>(other_game_object);

                    if (bullet->CheckCollision(boss->GetPosition(), boss->GetScale() * 0.2)) {
                        // Don't remove objects now
                        // Add them to a list to remove them later (code down below)
                        if (boss->TakeDamage(3) == true) boss_dead_ = true;
                        //to_erase.push_back(bullet);
                        bullet->SetAlive(false);
                        // If an intersection happened and objects will be removed,
                        // don't check for more intersections

                    }
                }
            }
        }
        else if (current_game_object->GetType() == SharkBulletObj) {
            Bullet* bullet = dynamic_cast<Bullet*>(current_game_object);
 
            if (bullet->CheckCollision(player->GetPosition(), player->GetScale() * 0.6)) {
                // Don't remove objects now
                // Add them to a list to remove them later (code down below)
                if (player->TakeDamage(1) == true) player_dead_ = true;
                //to_erase.push_back(bullet);
                bullet->SetAlive(false);
               // If an intersection happened and objects will be removed,
               // don't check for more intersections
                
            }

        } 
        else if (current_game_object->GetType() == SubTorpedoObj) {
           Bullet* bullet = dynamic_cast<Bullet*>(current_game_object);

           if (bullet->CheckCollision(player->GetPosition(), player->GetScale() * 0.6)) {
               // Don't remove objects now
               // Add them to a list to remove them later (code down below)
               if (player->TakeDamage(3) == true) player_dead_ = true;
               //to_erase.push_back(bullet);
               bullet->SetAlive(false);
               // If an intersection happened and objects will be removed,
               // don't check for more intersections

           }
        }
        else {
            // Note the loop bounds: we avoid testing the last object since
            // it's the background covering the whole game world
            for (int j = i + 1; j < (game_objects_.size()-1); j++) {
                GameObject* other_game_object = game_objects_[j];

                // Compute distance between object i and object j
                float distance = glm::length(current_game_object->GetPosition() - other_game_object->GetPosition());
                // If distance is below a threshold, we have a collision
                if (distance < 0.9f) {
                    if (other_game_object->GetType() == MineObj && current_game_object->GetType() == PlayerObj) {
                        current_game_object->TakeDamage(5);
                        //to_erase.push_back(other_game_object);
                        other_game_object->SetAlive(false);
                    }
                    else if ((other_game_object->GetType() == SharkObj || other_game_object->GetType() == SubObj) && current_game_object->GetType() == PlayerObj) {
                        if (current_game_object->TakeDamage(2) == true) player_dead_ = true;
                        //to_erase.push_back(other_game_object);
                        other_game_object->SetAlive(false);
                        killcount_++;
                    }
                    // This is where you would perform collision response between objects
                }
            }
        }
        if (!current_game_object->GetAlive()) {
            to_erase.push_back(current_game_object);
        }
    }

    // Remove game objects save in the to_erase vector
    for (int i = 0; i < to_erase.size(); i++){
        for (int j = 0; j < game_objects_.size(); j++){
            if (game_objects_[j] == to_erase[i]){
                delete game_objects_[j];
                game_objects_.erase(game_objects_.begin() + j);
                std::cout << "SOMETHING WAS DELETED" << std::endl;
                break;
            }
        }
    }

    if (player_dead_) {
        std::cout << "Mission failed, comrade." << std::endl;
        glfwSetWindowShouldClose(window_, true);
    }
    else {
        if (boss_dead_) {
            std::cout << "Mission accomplished, comrade." << std::endl;
            glfwSetWindowShouldClose(window_, true);
        }
    }
}


void Game::Render(double delta_time){

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
    camera_position_ += (float)delta_time * glm::vec3(0.0f, 1.0f, 0.0f);
    //glm::mat4 view_matrix = window_scale_matrix * camera_zoom_matrix;
    glm::mat4 view_matrix = window_scale_matrix * camera_zoom_matrix * glm::translate(glm::mat4(1.0f), -camera_position_);
    // Render all game objects
    for (int i = 0; i < game_objects_.size(); i++) {
        game_objects_[i]->Render(view_matrix, current_time_);
    }
}
      
} // namespace game
