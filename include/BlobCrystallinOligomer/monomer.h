// monomer.h

#ifndef MONOMER_H
#define MONOMER_H

#include <memory>
#include <vector>

#include "BlobCrystallinOligomer/ifile.h"
#include "BlobCrystallinOligomer/particle.h"
#include "BlobCrystallinOligomer/shared_types.h"
#include "BlobCrystallinOligomer/space.h"

namespace monomer {

    using ifile::MonomerData;
    using ifile::ParticleData;
    using particle::Particle;
    using shared_types::rotMatT;
    using shared_types::vecT;
    using space::CuboidPBC;
    using std::vector;
    using std::reference_wrapper;
    using std::unique_ptr;

    typedef vector<reference_wrapper<Particle>> particleArrayT;

    // Consider making multiple classes for different versions of the alphaB
    // monomer model, or in the distant future, alphaA monomers

    /** alphB cyrstallin coarse grained monomer
      *
      * Contains the particles that make it up and an interface for manipulating
      * the configuration.
      */
    class Monomer {
        public:
            Monomer(MonomerData m_data, CuboidPBC& pbc_space);

            /** Unique index */
            int get_index();

            /** Get all particles */
            particleArrayT get_particles();

            int get_num_particles();

            /** Get geometric center of all particles */
            vecT get_center();

            /** Translate monomer by given vector */
            void translate(vecT disv);

            /** Rotate monomer by given ? around given origin */
            void rotate(vecT rot_c, rotMatT rot_mat);

            /** Make trial configuration current configuration */
            void trial_to_current();

        private:
            int m_index; // Unique monomer index
            vector<unique_ptr<Particle>> m_particles;
            particleArrayT m_particle_refs;
            int m_num_particles;

            void create_particles(vector<ParticleData> p_datas,
                    CuboidPBC& pbc_space);
    };
}

#endif // MONOMER_H
