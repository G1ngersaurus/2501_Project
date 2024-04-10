#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "shader.h"
#include "game_object.h"

namespace game {

    // A class for holding the main game objects
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();

            // Call Init() before calling any other method
            // Initialize graphics libraries and main window
            void Init(void); 

            // Set up the game (scene, game objects, etc.)
            void Setup(void);

            // Run the game (keep the game active)
            void MainLoop(void); 

        private:
            // Main window: pointer to the GLFW window structure
            GLFWwindow *window_;

            // Sprite geometry
            Geometry *sprite_;

            // Particle geometry
            Geometry *particles_;

            Geometry* explosion_particles_;

            // Tile geometry
            Geometry* tile_;

            // Shader for rendering sprites in the scene
            Shader sprite_shader_;

            // Shader for rendering particles
            Shader particle_shader_;

            Shader text_shader_;

            Shader drawing_shader_;

            // References to textures
            // This needs to be a pointer
            GLuint *tex_;

            // List of game objects
            std::vector<GameObject*> game_objects_;

            // Keep track of time
            double current_time_;

            int seconds_;
            int lastSecond_;

            int killcount_;
            bool player_dead_;
            int turret_killcount_;
            bool boss_dead_;
            bool boss_vuln_;

            int damage_factor_;

            int score_;

            glm::vec3 camera_position_;
            glm::vec3 cam_velocity_;

            // Shooting cool down
            double next_shot_;
            double next_torpedo_;

            // Callback for when the window is resized
            static void ResizeCallback(GLFWwindow* window, int width, int height);

            // Set a specific texture
            void SetTexture(GLuint w, const char *fname, bool background);

            // Load all textures
            void SetAllTextures();

            // Handle user input
            void HandleControls(double delta_time);

            // Update all the game objects
            void Update(double delta_time);
 
            // Render the game world
            void Render(double delta_time);

    }; // class Game

} // namespace game

#endif // GAME_H_
