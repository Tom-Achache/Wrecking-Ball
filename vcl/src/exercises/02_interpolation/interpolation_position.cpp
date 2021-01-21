
#include "interpolation_position.hpp"


#ifdef EXERCISE_INTERPOLATION_POSITION

using namespace vcl;


static vec3 spline_cardinal(float t,
                            const std::array<vec3,4> P,
                            const std::array<float,4> T,
                            float tension);

/** Function returning the index i such that t \in [vt[i],vt[i+1]] */
static size_t index_at_value(float t, const std::vector<float>& vt);

/** Set elements of the gui */
static void set_gui(gui_structure& gui, timer_interval& timer);

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& )
{
    // Set keyframes
    keyframe_position = {{0,0,0},{1,3,0}, {1,1,0}, {2,1,0}, {3,1,0}, {3,0,0}, {1,0,0}};
    keyframe_time = {0,5,6,7,10,12,19};

    // Set timer bounds
    timer.t_min = 0;
    timer.t_max = 19;

    // Prepare the visual sphere
    sphere = mesh_drawable(mesh_primitive_sphere(1.0f));

    // Visual representation of the segments between keyframes
    polygon_segments = curve_drawable(keyframe_position);
    
}


/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui)
{
    timer.update();
    set_gui(gui,timer);


    // ********************************************* //
    // Compute interpolated position at time t
    // ********************************************* //
    const float t = timer.t;
    const size_t idx = index_at_value(t, keyframe_time);

    // Linear interpolation
    const float t1 = keyframe_time[idx];
    const float t2 = keyframe_time[idx+1];

    const vec3& p1 = keyframe_position[idx];
    const vec3& p2 = keyframe_position[idx+1];

    vec3 p;
    
    float tension = 1.2;
    if(idx == 0){
        std::array<vec3,4> P = {p1, p1, p2, keyframe_position[idx+2]};
        const std::array<float,4> T = {t1, t1, t2, keyframe_time[idx+2]};
        p = spline_cardinal(t, P, T, tension);
    }
    else if(idx == keyframe_position.size()-2){
        std::array<vec3,4> P = {keyframe_position[idx-1], p1, p2, p2};
        const std::array<float,4> T = {keyframe_time[idx], t1, t2, t2};
        p = spline_cardinal(t, P, T, tension);
    }
    else{
        if(idx < keyframe_position.size()-2){
            std::array<vec3,4> P = {keyframe_position[idx-1], p1, p2, keyframe_position[idx+2]};
            const std::array<float,4> T = {keyframe_time[idx], t1, t2, keyframe_time[idx+2]};
            p = spline_cardinal(t, P, T, tension);
        }
    }
    
    
    
    curve.uniform_parameter.color = {0,1,1};
    curve.add_point(p);
    curve.draw(shaders["curve"],scene.camera);


    // Draw current position
    sphere.uniform_parameter.color = vec3(0,0,1);
    sphere.uniform_parameter.scaling = 0.08f;
    sphere.uniform_parameter.translation = p;
    sphere.draw(shaders["mesh"],scene.camera);


    // Draw sphere at each keyframe position
    size_t N = keyframe_position.size();
    for(size_t k=0; k<N; ++k)
    {
        const vec3& p_keyframe = keyframe_position[k];
        sphere.uniform_parameter.color = vec3(1,1,1);
        sphere.uniform_parameter.scaling = 0.05f;
        sphere.uniform_parameter.translation = p_keyframe;
        sphere.draw(shaders["mesh"],scene.camera);
    }

    // Draw segments between each keyframe position
    glLineWidth(1.0f);
    polygon_segments.uniform_parameter.color = vec3(0,0,0);
    polygon_segments.draw(shaders["curve"],scene.camera);
}


/** Part specific GUI drawing */
static void set_gui(gui_structure& , timer_interval& timer)
{
    ImGui::SliderScalar("Time", ImGuiDataType_Float, &timer.t, &timer.t_min, &timer.t_max, "%.2f s");

    float scale_min = 0.05f;
    float scale_max = 2.0f;
    ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer.scale, &scale_min, &scale_max, "%.2f s");

    if (ImGui::Button("Stop"))
        timer.stop();
    if (ImGui::Button("Start"))
        timer.start();
}

static size_t index_at_value(float t, const std::vector<float>& vt)
{
    const size_t N = vt.size();
    assert(vt.size()>=2);
    assert(t>=vt[0]);
    assert(t<vt[N-1]);

    size_t k=0;
    while( vt[k+1]<t )
        ++k;
    return k;
}

static vec3 spline_cardinal(float t,
                            const std::array<vec3,4> P,
                            const std::array<float,4> T,
                            float tension)
{
    
    vec3 d1 = (T[2]-T[1])*tension*(P[2]-P[0])/(T[2]-T[0]);
    vec3 d2 = (T[2]-T[1])*tension*(P[3]-P[1])/(T[3]-T[1]);
    
    float s = (t - T[1])/(T[2]-T[1]);
    
    vec3 ps = (2*s*s*s - 3*s*s + 1)*P[1];
    ps += (s*s*s - 2*s*s + s)*d1;
    ps += (-2*s*s*s + 3*s*s)*P[2];
    ps += (s*s*s - s*s)*d2;
    return ps;
}

#endif
