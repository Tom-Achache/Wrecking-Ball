//
//  tearing_cloth.hpp
//  pgm
//
//  Created by Tom Achache on 05/03/2019.
//
#pragma once

#include "../exercises/base_exercise/base_exercise.hpp"
#include <set>

#ifdef EXERCISE_TEARING_CLOTH
#define N_CLOTH 5

struct user_parameters_structure
{
    float m;    // Global mass (to be divided by the number of particles)
    float m_ball;
    float K;    // Global stiffness (to be divided by the number of particles)
    float mu;   // Damping
    float wind; // Wind magnitude;
    float robustness;
};

struct simulation_parameters_structure
{
    float m;  // mass
    float K;  // spring stiffness
    float L0; // spring rest length
};

// Sphere and ground used for collision
struct collision_shapes_structure
{
    vcl::vec3 sphere_p;
    vcl::vec3 sphere_v;
    vcl::vec3 sphere_f;
    float sphere_r;
    float ground_height;
};

// Structure of a particle
struct particle_structure
{
    vcl::vec3 p0; // Initial position
    vcl::vec3 v0; // Initial speed
    float t0;     // Time of creation
};


struct scene_exercise : base_scene_exercise
{
    // Particles parameters
    std::array<std::vector<vcl::vec3>, N_CLOTH > position;
    std::array<std::vector<vcl::vec3>, N_CLOTH > speed;
    std::array<std::vector<vcl::vec3>, N_CLOTH > force;
    std::array<std::set<int>, N_CLOTH> removed;
    
    // Simulation parameters
    simulation_parameters_structure simulation_parameters; // parameters that user can control directly
    user_parameters_structure user_parameters;             // parameters adjusted with respect to mesh size (not controled directly by the user)
    
    // Cloth meshes elements
    std::array<vcl::mesh_drawable, N_CLOTH> clothes;
    std::array<std::vector<vcl::vec3>, N_CLOTH > normals;        // Normal of the cloth used for rendering and wind force computation
    std::array<std::vector<vcl::index3> , N_CLOTH> connectivity; // Connectivity of the triangular model
    
    
    
    // Parameters of the shape used for collision
    collision_shapes_structure collision_shapes;
    
    // Store index and position of vertices constrained to have a fixed 3D position
    std::array<std::map<int,vcl::vec3> , N_CLOTH> positional_constraints;
    
    // Textures
    GLuint texture_cloth;
    GLuint texture_wood;
    
    // Visual elements of the scene
    vcl::mesh_drawable sphere;
    vcl::mesh_drawable ground;
    
    // Gui parameters
    bool gui_display_wireframe;
    bool gui_display_texture;
    
    // Parameters used to control if the simulation runs when a numerical divergence is detected
    bool simulation_diverged; // Active when divergence is detected
    bool force_simulation;    // Force to run simulation even if divergence is detected
    
    particle_structure boule;
    float m; // mass of the boule
    
    vcl::timer_event timer;
    
    
    void numerical_integration_sphere(float h);
    void initialize();
    void collision_constraints(size_t i, float h);
    void compute_forces(size_t i);
    void compute_forces_sphere();
    void numerical_integration(float h, size_t i);
    void detect_simulation_divergence(size_t i);
    void hard_constraints();
    void set_gui();
    
    
    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void display_elements(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
};






#endif
