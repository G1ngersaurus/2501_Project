#ifndef SHARK_ENEMY_OBJECT_H_
#define SHARK_ENEMY_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class SharkEnemyObject : public GameObject {

        public:
            SharkEnemyObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health);

            // Update function for moving the player object around
            void Update(double delta_time) override;

            // Update target
            GameObject *GetTarget(void) { return target_; }
            void SetTarget(GameObject *t) { target_ = t; }

            inline double GetNextShot(void) const { return next_shot_; }
            inline void SetNextShot(double value) { next_shot_ = value; }
        protected:
            GameObject *target_;
            //double wander_cool_down_;
            double current_time_;
            double next_shot_;

    }; // class EnemyGameObject

} // namespace game

#endif // SHARK_ENEMY_OBJECT_H_
