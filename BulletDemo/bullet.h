#ifndef BULLET_H_
#define BULLET_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class Bullet : public GameObject {

        public:
            Bullet(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health);
            ~Bullet();
            // Update function for moving the player object around
            void Update(double delta_time) override;

            // Bullet variables
            void SetOrigin(glm::vec3 origin) { origin_ = origin; }

            bool CheckCollision(glm::vec3 C, float r);
        protected:
            double current_time_;
            double last_time_;
            glm::vec3 origin_;
            Timer* lifespan_;
    }; // class Bullet

} // namespace game

#endif // BULLET_H_
