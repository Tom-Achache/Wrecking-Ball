
#include "blend_shape.hpp"

#ifdef EXERCISE_BLEND_SHAPE

using namespace vcl;

static vec3 spline_cardinal(float t,
                            const std::array<vec3,4> P,
                            const std::array<float,4> T,
                            float tension);


/** Function returning the index i such that t \in [vt[i],vt[i+1]] */
static size_t index_at_value(float t, const std::vector<float>& vt);

/** Set elements of the gui */
static void set_gui(gui_structure& , timer_interval& timer,bool& display_wireframe,bool& display_body);

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& scene, gui_structure& gui)
{
    std::list<size_t> index_order = {};
    for (size_t n =0; n<48 ; n++){
        index_order.push_back(n);
    }
    

    size_t N = index_order.size();
    
    for(size_t k=0; k<N; ++k)
    {
        const std::string filename = "../../data/horse/horse-gallop-"+zero_fill(std::to_string(k+1),2)+".obj";
        keyframe_position.push_back(mesh_load_file_obj_read_vertices(filename));
        keyframe_time.push_back(float(k)/5);
        
    }
    
    keyframe_position.resize(N);
    keyframe_time.resize(N);

    const size_t N_vertex = keyframe_position[0].size();
    current_position.resize(N_vertex);

    // Set timer bounds
    timer.t_min = 0;
    timer.t_max = (N-1-1.0f/100)/5;
//    timer.t_max = 5;

    // Prepare the visual representation of the face
//    face = mesh_drawable(mesh_load_file_obj("../../data/horse/horse-gallop-01"));
//    body = mesh_drawable(mesh_load_file_obj("../../data/face/body.obj"));
     horse = mesh_drawable(mesh_load_file_obj("../../data/horse/horse-gallop-01.obj"));

    scene.camera.translation = {0,0,0};
    scene.camera.scale = 2;

    gui.show_frame_camera = false;

}


/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui)
{
    timer.update();
    set_gui(gui,timer, display_wireframe, display_body);

    // ********************************************* //
    // Compute interpolated position at time t
    // ********************************************* //
    const float t = timer.t;
    const size_t idx = index_at_value(t, keyframe_time);

    // Linear interpolation (over all vertices)
    const std::vector<vec3>& p1 = keyframe_position[idx];
    float tension = 1;
    const size_t N = p1.size();
    for(size_t k=0; k<N; ++k){
        const float t1 = keyframe_time[idx];
        const float t2 = keyframe_time[idx+1];
        
        const vec3& p1 = keyframe_position[idx][k];
        const vec3& p2 = keyframe_position[idx+1][k];
        
        vec3 p;
        
        if(idx == 0){
            std::array<vec3,4> P = {p1, p1, p2, keyframe_position[idx+2][k]};
            const std::array<float,4> T = {t1, t1, t2, keyframe_time[idx+2]};
            p = spline_cardinal(t, P, T, tension);
        }
        else if(idx == keyframe_position.size()-2){
            std::array<vec3,4> P = {keyframe_position[idx-1][k], p1, p2, p2};
            const std::array<float,4> T = {keyframe_time[idx], t1, t2, t2};
            p = spline_cardinal(t, P, T, tension);
        }
        else{
            if(idx < keyframe_position.size()-2){
                std::array<vec3,4> P = {keyframe_position[idx-1][k], p1, p2, keyframe_position[idx+2][k]};
                const std::array<float,4> T = {keyframe_time[idx], t1, t2, keyframe_time[idx+2]};
                p = spline_cardinal(t, P, T, tension);
            }
        }
        
        current_position[k] = p;
    }
//    face.data_gpu.update_position(current_position);
    horse.data_gpu.update_position(current_position);

    // ********************************************* //
    // Draw results
    // ********************************************* //

//    face.draw(shaders["mesh"],scene.camera);
//    if(display_body)
//        body.draw(shaders["mesh"],scene.camera);
//
//    if(display_wireframe)
//    {
//        face.draw(shaders["wireframe"],scene.camera);
//        if(display_body)
//            body.draw(shaders["wireframe"],scene.camera);
//    }
    horse.draw(shaders["mesh"], scene.camera);

}


/** Part specific GUI drawing */
static void set_gui(gui_structure& , timer_interval& timer,bool& display_wireframe,bool& display_body)
{
    ImGui::SliderScalar("Time", ImGuiDataType_Float, &timer.t, &timer.t_min, &timer.t_max, "%.2f s");

    float scale_min = 0.05f;
    float scale_max = 2.0f;
    ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer.scale, &scale_min, &scale_max, "%.2f s");

    if (ImGui::Button("Stop"))
        timer.stop();
    if (ImGui::Button("Start"))
        timer.start();

    if( ImGui::Button("Wireframe") )
        display_wireframe = !display_wireframe;
    if( ImGui::Button("Body") )
        display_body = !display_body;

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
