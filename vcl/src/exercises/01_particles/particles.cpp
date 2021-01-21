
#include "particles.hpp"

#include <random>

#ifdef EXERCISE_PARTICLES

using namespace vcl;

// Generator for uniform random number
std::default_random_engine generator;
std::uniform_real_distribution<float> distrib(0.0,1.0);


static void set_gui(timer_event& timer);

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& gui)
{
    // Create mesh for particles represented as spheres
    sphere = mesh_drawable(mesh_primitive_sphere(0.07f));
    sphere.uniform_parameter.color = vec3(0.6f, 0.6f, 1.0f);

    // Create mesh for the ground displayed as a disc
    ground = mesh_drawable(mesh_primitive_disc(2.0f,{0,0.7f,0},{0,1,0},80));
    ground.uniform_parameter.color = vec3(1,1,1);
    
    cauldron = mesh_drawable(mesh_load_file_obj("../../data/cauldron/cauldron.obj"));

    // Initial camera position
    scene.camera.scale = 15.0f;
    const mat3 Ry = rotation_from_axis_angle_mat3({0,1,0},float(M_PI)/4.0f);
    const mat3 Rx = rotation_from_axis_angle_mat3({1,0,0},-float(M_PI)/8.0f);
    scene.camera.orientation = Ry*Rx;

    // Delay between emission of a new particles
    timer.periodic_event_time_step = 0.05f;

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

    // Draw ground
    ground.draw(shaders["mesh"],scene.camera);
    
    cauldron.draw(shaders["mesh"],scene.camera);


    // Emission of new particle if needed
    const bool is_new_particle = timer.event;
    if( is_new_particle )
    {
        particle_structure new_particle;
        new_particle.t0 = t_current;
        float x0 = 1.3*distrib(generator);
        float z0 = 1.3*distrib(generator);
        new_particle.p0 = vec3(x0,0.5,z0);

        // Initial speed is random. (x,z) components are uniformly distributed along a circle.
        const float theta = 0;
        new_particle.r = 0.5*distrib(generator);
        new_particle.v0 = vec3( 0,0,0);
        new_particle.color = vec3(0.7,0.7, distrib(generator)*0.5f + 0.5f);
        new_particle.radius = distrib(generator)*2 + 1;

        particles.push_back(new_particle);
    }

    // Compute position of particles at current time
    const vec3 g = {0,-9.81f,0};
    
    for(particle_structure& particle : particles)
    {
        
        const float t = t_current-particle.t0; // local time of the current particle since its creation
        float theta = 5*t;
        float y = particle.p0.y + t;
        float x = particle.p0.x+particle.r*std::cos(theta);
        float z = particle.p0.z+particle.r*std::sin(theta);
        const vec3 p = vec3(x,y,z);
        
//        if(t>( 2 * particle.v0.y /9.81)){
//            particle.t0 = t_current ;
//            particle.v0 = particle.v0 * 0.9;
//            particle.p0 = p;
////            std::cout << t << std::endl;
//        }
        
        sphere.uniform_parameter.scaling = particle.radius;
        sphere.uniform_parameter.translation = p;
        sphere.uniform_parameter.color = particle.color;
        sphere.draw(shaders["mesh"], scene.camera);
    }

    // Remove particles that have exceeded they lifetime
    for(auto it = particles.begin(); it!=particles.end(); ++it)
        if( t_current-it->t0 > 30)
            it = particles.erase(it);

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
