#pragma once

#include "../base_exercise/base_exercise.hpp"

#ifdef EXERCISE_PARTICLES

// Structure of a particle
struct particle_structure
{
    vcl::vec3 p0; // Initial position
    vcl::vec3 v0; // Initial speed
    float t0;
    float r;// Time of creation
    float radius;
    vcl::vec3 color;
};

struct scene_exercise : base_scene_exercise
{

    void setup_data(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);
    void frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui);


    std::list<particle_structure> particles; // Storage of all currently active particles

    vcl::mesh_drawable sphere; // Visual representation of a particle - a sphere
    vcl::mesh_drawable ground;
    vcl::mesh_drawable cauldron;
    // Visual representation of the ground - a disc
    vcl::timer_event timer;    // Timer allowing to indicate periodic events

};

#endif
