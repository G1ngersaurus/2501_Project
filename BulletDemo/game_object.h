#ifndef GAME_OBJECT_H_
#define GAME_OBJECT_H_

#include <glm/glm.hpp>
#define GLEW_STATIC
#include <GL/glew.h>

#include "shader.h"
#include "geometry.h"
#include "timer.h"

namespace game {

    enum ObjectType { GenericObj, PlayerObj, EnemyObj, MineObj, 
        SharkObj, SubObj, BulletObj, SharkBulletObj, PSystemObj, 
        BackgroundObj, TimerObj, HealthObj, ScoreObj, TorpedoObj, SubTorpedoObj, BossObj, TurretObj };

    /*
        GameObject is responsible for handling the rendering and updating of one object in the game world
        The update and render methods are virtual, so you can inherit them from GameObject and override the update or render functionality (see PlayerGameObject for reference)
    */
    class GameObject {

        public:
            // Constructor
            GameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health);
            ~GameObject();
            // Update the GameObject's state. Can be overriden in children
            virtual void Update(double delta_time);

            // Renders the GameObject 
            virtual void Render(glm::mat4 view_matrix, double current_time);

            // Getters
            inline glm::vec3 GetPosition(void) const { return position_; }
            inline float GetScale(void) const { return scale_; }
            inline float GetRotation(void) const { return angle_; }
            inline int GetHealth(void) const { return health_; }

            // Get bearing direction (direction in which the game object
            // is facing)
            glm::vec3 GetBearing(void) const;

            // Get vector pointing to the right side of the game object
            glm::vec3 GetRight(void) const;
            ObjectType GetType(void) const { return type_; }

            glm::vec3 GetBottomLeft(void) const;
            glm::vec3 GetBottomRight(void) const;

            inline void SetParticles(GameObject* particle) { particles_ = particle; }

            inline bool GetAlive(void) const { return alive_; }
            void SetAlive(bool value);

            // returns true if thing dies taking this damage
            bool TakeDamage(int value);

            // Setters
            inline void SetPosition(const glm::vec3& position) { position_ = position; }
            inline void SetScale(float scale) { scale_ = scale; }
            void SetRotation(float angle);
            void SetType(ObjectType tp) { type_ = tp; }
            // Velocity
            glm::vec3 GetVelocity(void) const { return velocity_; }
            void SetVelocity(glm::vec3& vel) { velocity_ = vel; }

        protected:
            // Object's Transform Variables
            glm::vec3 position_;
            float scale_;
            float angle_;
            float yScale_;
            float xScale_;
            glm::vec3 velocity_;

            Timer* death_timer_;

            bool alive_;

            GameObject* particles_;

            int health_;

            // Geometry
            Geometry *geometry_;
 
            // Shader
            Shader *shader_;

            // Object's texture reference
            GLuint texture_;

            // Object type
            ObjectType type_;
    }; // class GameObject

} // namespace game

#endif // GAME_OBJECT_H_
