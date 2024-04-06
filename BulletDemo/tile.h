#ifndef TILE_H_
#define TILE_H_

#include "geometry.h"

namespace game {

    // A tile
    class Tile : public Geometry {

        public:
            // Constructor and destructor
            Tile(void);

            // Create the geometry (called once)
            void CreateGeometry(void);

            // Use the geometry
            void SetGeometry(GLuint shader_program);

    }; // class Tile
} // namespace game

#endif // SPRITE_H_
