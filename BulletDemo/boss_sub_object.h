#ifndef BOSS_SUB_OBJECT_H_
#define BOSS_SUB_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class BossSubObject : public GameObject {

        public:
            BossSubObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health);

            // Update function for moving the player object around
            void Update(double delta_time, glm::vec3 camera);

            inline void SetNextShot(double value) { next_shot_ = value; }
            inline double GetNextShot(void) { return next_shot_; }

            // Update target
            GameObject *GetTarget(void) { return target_; }
            void SetTarget(GameObject *t) { target_ = t; }
        protected:
            GameObject *target_;
            double wander_cool_down_;
            double current_time_;
            bool docile_;
            int lastSecond_;
            double next_shot_;

    }; // class BossSubObject

} // namespace game

#endif // BOSS_SUB_OBJECT_H_
