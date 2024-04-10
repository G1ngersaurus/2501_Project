#ifndef EXPLOSION_PARTICLES_H_
#define EXPLOSION_PARTICLES_H_

#include "geometry.h"

#define NUM_PARTICLES 4000

namespace game {

    // A set of particles that can be rendered
    class ExplosionParticles : public Geometry {

        public:
            ExplosionParticles(void);

            // Create the geometry (called once)
            void CreateGeometry(void);

            // Use the geometry
            void SetGeometry(GLuint shader_program);

    }; // class Particles
} // namespace game

#endif // EXPLOSION_PARTICLES_H_
