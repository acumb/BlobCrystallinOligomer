// config.cpp

#include <iostream>
#include <memory>
#include <utility>

#include "BlobCrystallinOligomer/config.h"
#include "BlobCrystallinOligomer/file.h"
#include "BlobCrystallinOligomer/monomer.h"
#include "BlobCrystallinOligomer/random_gens.h"
#include "BlobCrystallinOligomer/shared_types.h"

namespace config {

    using file::InputConfigFile;
    using file::MonomerData;
    using file::ParticleData;
    using monomer::Monomer;
    using particle::Particle;
    using random_gens::RandomGens;
    using shared_types::vecT;
    using std::cout;
    using std::pair;
    using std::unique_ptr;

    Config::Config(InputParams params, RandomGens& random_num):
            m_space_store {new CuboidPBC()}, m_space {*m_space_store},
            m_random_num {random_num} {

        InputConfigFile config_file {params.m_config_filename};
        vector<MonomerData> monomers {config_file.get_monomers()};
        create_monomers(monomers);
        m_space.set_len(config_file.get_box_len());

        // Create monomer reference array
        for (auto &m: m_monomers) {
            m_monomer_refs.emplace_back(m);
        }
    }

    Monomer& Config::get_monomer(int monomer_index) {
        return *m_monomers[monomer_index];
    }
    
    Monomer& Config::get_random_monomer() {
        int m_i {m_random_num.uniform_int(0, m_monomers.size())};
        return *m_monomers[m_i];
    }

    monomerArrayT Config::get_monomers() {
        return m_monomer_refs;
    }

    vecT Config::calc_interparticle_vector(Particle& particle1, CoorSet coorset1,
            Particle& particle2, CoorSet coorset2) {

        vecT pos1 {particle1.get_pos(coorset1)};
        vecT pos2 {particle2.get_pos(coorset2)};

        return m_space.calc_diff(pos1, pos2);
    }

    distT Config::calc_dist(Particle& particle1, CoorSet coorset1,
            Particle& particle2, CoorSet coorset2) {

        vecT pos1 {particle1.get_pos(coorset1)};
        vecT pos2 {particle2.get_pos(coorset2)};

        return m_space.calc_dist(pos1, pos2);
    }

    void Config::create_monomers(vector<MonomerData> monomers) {
        for (auto m_data: monomers) {
            m_monomers.emplace_back(m_data, m_space);
        }
    }
}
