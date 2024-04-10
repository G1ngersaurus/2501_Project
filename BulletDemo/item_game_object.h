#ifndef ITEM_GAME_OBJECT_H_
#define ITEM_GAME_OBJECT_H_

#include "game_object.h"

namespace game {

    enum ItemType { RepairKit, DamageUpgrade, InvinciblePower};
    // Inherits from GameObject
    class ItemGameObject : public GameObject {

        public:
            ItemGameObject(const glm::vec3 &position, Geometry *geom, Shader *shader, GLuint texture, float yScale, float xScale, int health);

            // Update function for moving the player object around
            void Update(double delta_time) override;

            inline ItemType GetItemType(void) const { return item_type_; }
            inline void SetItemType(ItemType tp) { item_type_ = tp; }

        protected:

            double current_time_;
            ItemType item_type_;

    }; // class ItemGameObject

} // namespace game

#endif // ITEM_GAME_OBJECT_H_
