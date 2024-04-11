#ifndef PLAYER_GAME_OBJECT_H_
#define PLAYER_GAME_OBJECT_H_

#include "game_object.h"
namespace game {

    // Inherits from GameObject
    class PlayerGameObject : public GameObject {

        public:
            PlayerGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health, int maxHealth);

            // Update function for moving the player object around
            void Update(double delta_time, glm::vec3 camera);

            void RepairHealth(int value);

            inline int GetMaxHealth(void) const { return max_health_; }

        protected:

            int max_health_;

    }; // class PlayerGameObject

} // namespace game

#endif // PLAYER_GAME_OBJECT_H_
