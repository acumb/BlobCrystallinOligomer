// energy.cpp

#include <memory>
#include <vector>

#include "BlobCrystallinOligomer/config.h"
#include "BlobCrystallinOligomer/energy.h"
#include "BlobCrystallinOligomer/param.h"
#include "BlobCrystallinOligomer/potential.h"
#include "BlobCrystallinOligomer/shared_types.h"

namespace energy {

    using config::Config;
    using file::InputEnergyFile;
    using file::InteractionData;
    using file::PotentialData;
    using monomer::Monomer;
    using monomer::particleArrayT;
    using param::InputParams;
    using potential::HardSpherePotential;
    using potential::ShiftedLJPotential;
    using potential::PatchyPotential;
    using potential::OrientedPatchyPotential;
    using shared_types::CoorSet;
    using shared_types::distT;
    using shared_types::vecT;
    using std::shared_ptr;
    using std::vector;

    Energy::Energy(Config& conf, InputParams params):
            m_config {conf} {

        InputEnergyFile energy_file {params.m_energy_filename};
        vector<PotentialData> potentials {energy_file.get_potentials()};
        vector<InteractionData> interactions {energy_file.get_interactions()};
        create_potentials(potentials, interactions);
    }

    eneT Energy::calc_monomer_pair_energy(Monomer& monomer1, CoorSet coorset1,
            Monomer& monomer2, CoorSet coorset2) {

        eneT pair_ene {0};
        particleArrayT particles1 {monomer1.get_particles()};
        particleArrayT particles2 {monomer2.get_particles()};
        for (size_t p1_i {0}; p1_i != particles1.size(); p1_i++) {
            for (size_t p2_i {p1_i}; p2_i != particles2.size(); p2_i++) {
                Particle& p1 = particles1[p1_i]; // Doesn't allow {} intialization
                Particle& p2 = particles1[p2_i];
                pair_ene += calc_particle_pair_energy(p1, coorset1, p2, coorset2);
            }
        }

        return pair_ene;
    }

    bool Energy::monomers_interacting(Monomer& monomer1, CoorSet coorset1,
            Monomer& monomer2, CoorSet coorset2) {

        bool m_interacting {false};
        particleArrayT particles1 {monomer1.get_particles()};
        particleArrayT particles2 {monomer2.get_particles()};
        for (size_t p1_i {0}; p1_i != particles1.size(); p1_i++) {
            for (size_t p2_i {p1_i}; p2_i != particles2.size(); p2_i++) {
                Particle& p1 = particles1[p1_i]; // Doesn't allow {} intialization
                Particle& p2 = particles1[p2_i];
                bool p_interacting {particles_interacting(p1, coorset1, p2,
                        coorset2)};
                if (p_interacting) {
                    m_interacting = true;
                    break;
                }
            }
        }

        return m_interacting;
    }

    bool Energy::particles_interacting(Particle& particle1, CoorSet coorset1,
            Particle& particle2, CoorSet coorset2) {

        distT dist {m_config.calc_dist(particle1, coorset1, particle2, coorset2)};
        pair<int, int> key {particle1.get_type(), particle2.get_type()};

        PairPotential& pot {m_pair_to_pot[key].get()};
        bool interacting {pot.particles_interacting(dist)};

        return interacting;
    }

    eneT Energy::calc_particle_pair_energy(Particle& particle1, CoorSet coorset1,
            Particle& particle2, CoorSet coorset2) {

        vecT diff {m_config.calc_interparticle_vector(particle1, coorset1,
                particle2, coorset2)};
        distT dist {diff.norm()};
        auto p1_ore {particle1.get_ore(coorset1)};
        auto p2_ore {particle2.get_ore(coorset1)};
        pair<int, int> key {particle1.get_type(), particle2.get_type()};

        PairPotential& pot {m_pair_to_pot[key].get()};
        eneT ene {pot.calc_energy(dist, diff, p1_ore, p2_ore)};

        return ene;
    }

    void Energy::create_potentials(vector<PotentialData> potentials,
            vector<InteractionData> interactions) {

        for (auto p_data: potentials) {
            PairPotential* pot;
            if (p_data.form == "HardSphere") {
                pot = new  HardSpherePotential {p_data.sigh};
            }
            else if (p_data.form == "ShiftedLJ") {
                pot = new ShiftedLJPotential {p_data.sigl, p_data.eps,
                        p_data.rcut};
            }
            else if (p_data.form == "Patchy") {
                pot = new PatchyPotential {p_data.sigl, p_data.eps, p_data.rcut,
                        p_data.siga1, p_data.siga2};
            }
            else if (p_data.form == "OrientedPatchy") {
                pot = new OrientedPatchyPotential {p_data.sigl, p_data.eps,
                        p_data.rcut, p_data.siga1, p_data.siga2,
                        p_data.sigt};
            }
            m_potentials.emplace_back(pot);
        }

        for (auto i_data: interactions) {
            for (auto p_pair: i_data.particle_pairs) {
                PairPotential& pot {*m_potentials[i_data.potential_index]};
                m_pair_to_pot[p_pair] = pot;
                pair<int, int> reversed_p_pair {p_pair.second, p_pair.first};
                m_pair_to_pot[reversed_p_pair] = pot;
            }
        }
    }

}
