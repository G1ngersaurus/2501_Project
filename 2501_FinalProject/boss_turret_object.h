#ifndef BOSS_TURRET_OBJECT_H_
#define BOSS_TURRET_OBJECT_H_

#include "game_object.h"

namespace game {

    // Inherits from GameObject
    class BossTurretObject : public GameObject {

        public:
            BossTurretObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, GameObject* parent, glm::vec3 fake_position, float yScale, float xScale, int health);

            // Update function for moving the player object around
            void Update(double delta_time) override;

            void Render(glm::mat4 view_matrix, double current_time);

            inline void SetNextShot(double value) { next_shot_ = value; }
            inline double GetNextShot(void) { return next_shot_; }

            inline glm::vec3 GetFakePosition(void) const { return fake_position_; }

            // Update target
            GameObject *GetTarget(void) { return target_; }
            void SetTarget(GameObject *t) { target_ = t; }
        protected:
            GameObject *target_;
            GameObject* parent_;
            double wander_cool_down_;
            double current_time_;
            double next_shot_;


            glm::vec3 fake_position_;


    }; // class EnemyGameObject

} // namespace game

#endif // BOSS_TURRET_OBJECT_H_
