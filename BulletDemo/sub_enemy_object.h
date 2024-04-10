#ifndef SUB_ENEMY_OBJECT_H_
#define SUB_ENEMY_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class SubEnemyObject : public GameObject {

        public:
            SubEnemyObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health);

            // Update function for moving the player object around
            void Update(double delta_time, glm::vec3 camera);

            // Update target
            GameObject *GetTarget(void) { return target_; }
            void SetTarget(GameObject *t) { target_ = t; }

            double GetNextShot(void) const { return next_shot_; }
            void SetNextShot(double value) { next_shot_ = value; }

        protected:
            GameObject *target_;
            double wander_cool_down_;
            double current_time_;
            bool docile_;
            int lastSecond_;
            double next_shot_;
    }; // class EnemyGameObject

} // namespace game

#endif // SUB_ENEMY_OBJECT_H_
