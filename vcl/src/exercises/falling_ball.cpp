//
//  falling_ball.cpp
//  pgm
//
//  Created by Tom Achache on 05/03/2019.
//

#include "falling_ball.hpp"

#include <random>

#ifdef EXERCISE_FALLING_BALL

using namespace vcl;

// Generator for uniform random number
std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0,1.0);


static void set_gui(timer_event& timer);

/** This function is called before the beginning of the animation loop
 It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& gui)
{
    // Initial camera position
    scene.camera.scale = 5.0f;
    const mat3 Ry = rotation_from_axis_angle_mat3({0,1,0},float(M_PI)/4.0f);
    const mat3 Rx = rotation_from_axis_angle_mat3({1,0,0},-float(M_PI)/8.0f);
    scene.camera.orientation = Ry*Rx;
    
    const mat3 R = rotation_from_axis_angle_mat3({1,0,0},float(M_PI)/2.0f);
    
    // If cone
    sphere = mesh_drawable(mesh_primitive_cone(0.3f));
    sphere.uniform_parameter.color = vec3(0.6f, 0.6f, 1.0f);
    sphere.uniform_parameter.rotation = R;
    
    /*
     // If sphere
     sphere = mesh_drawable(mesh_primitive_sphere(0.3f));
     sphere.uniform_parameter.color = vec3(0.6f, 0.6f, 1.0f);
     */
    
    timer.update();
    const float t_current = timer.t;
    
    boule.t0 = t_current;
    boule.p0 = vec3(0,2.0f,0);
    boule.v0 = vec3(0,0,0);
    
    // Default gui display
    gui.show_frame_worldspace = true;
    gui.show_frame_camera = false;
}



/** This function is called at each frame of the animation loop.
 It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    
    timer.update();
    const float t_current = timer.t;
    set_gui(timer);
    
    // Compute position of particles at current time
    const vec3 g = {0,-9.81f,0};
    const float t = t_current - boule.t0; // local time of the current particle since its creation
    const vec3 p = (g*t*t/2.0f + boule.v0*t + boule.p0)/100;
    
    sphere.uniform_parameter.translation = p;
    sphere.draw(shaders["mesh"], scene.camera);
    
}


/** Part specific GUI drawing */
static void set_gui(timer_event& timer)
{
    // Can set the speed of the animation
    float scale_min = 0.05f;
    float scale_max = 2.0f;
    ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer.scale, &scale_min, &scale_max, "%.2f s");
    
    // Start and stop animation
    if (ImGui::Button("Stop"))
    timer.stop();
    if (ImGui::Button("Start"))
    timer.start();
}


#endif
