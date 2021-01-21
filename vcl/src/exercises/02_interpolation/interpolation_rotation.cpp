
#include "interpolation_rotation.hpp"

#ifdef EXERCISE_INTERPOLATION_ROTATION

using namespace vcl;


static mat3 quaternion_interpolate(vec3 axis_1, vec3 axis_2, float angle1, float angle2, float t);

/** Set elements of the gui */
static void set_gui(gui_structure& gui, timer_interval_reversing& timer);

/** This function is called before the beginning of the animation loop
    It is used to initialize all part-specific data */
void scene_exercise::setup_data(std::map<std::string,GLuint>& , scene_structure& , gui_structure& gui)
{
    translation_1 = {0,0,-0.5};
    axis_1  = {0,0,1};
    angle_1 = 0.0f;

    translation_2 = {0,0,0.5f};
    axis_2  = normalize(vec3{1,1,0});
    angle_2 = float(M_PI);

    frame = mesh_drawable(mesh_primitive_frame());
    frame.uniform_parameter.scaling = 0.35f;

    timer.t_max = 1;
    gui.show_frame_camera = false;

}


/** This function is called at each frame of the animation loop.
    It is used to compute time-varying argument and perform data data drawing */
void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui)
{
    timer.update();
    set_gui(gui,timer);

    const vec3 tr1 = translation_1;
    const vec3 tr2 = translation_2;

    const mat3 R1 = rotation_from_axis_angle_mat3(axis_1,angle_1);
    const mat3 R2 = rotation_from_axis_angle_mat3(axis_2,angle_2);

    const float alpha = timer.t;

    // Linear interpolation of translation
    const vec3 tr = (1-alpha)*tr1 + alpha*tr2;

    // Linear interpolation of matrix
    const mat3 R = quaternion_interpolate( axis_1, axis_2, angle_1, angle_2, alpha);

    frame.uniform_parameter.translation = tr1;
    frame.uniform_parameter.rotation = R1;
    frame.draw(shaders["mesh"],scene.camera);

    frame.uniform_parameter.translation = tr2;
    frame.uniform_parameter.rotation = R2;
    frame.draw(shaders["mesh"],scene.camera);

    frame.uniform_parameter.translation = tr;
    frame.uniform_parameter.rotation = R;
    frame.draw(shaders["mesh"],scene.camera);
}


/** Part specific GUI drawing */
static void set_gui(gui_structure& , timer_interval_reversing& timer)
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

static mat3 quaternion_interpolate(vec3 axis_1, vec3 axis_2, float angle1, float angle2, float t){
    vec4 q1 = {axis_1.x*std::sin(angle1/2),axis_1.y*std::sin(angle1/2),axis_1.z*std::sin(angle1/2), std::cos(angle1/2)};
    vec4 q2 = {axis_2.x*std::sin(angle2/2),axis_2.y*std::sin(angle2/2),axis_2.z*std::sin(angle2/2), std::cos(angle2/2)};
    
    
    if(dot(q1,q2)<0){
        q2 = -q2;
    }
    float omega = std::acos(dot(q1, q2));
    
    vec4 q = std::sin((1-t)*omega)*q1/std::sin(omega) + std::sin(t*omega)*q2/std::sin(omega);
    
//    std::cout << dot(q1,q2) << std::endl;
    mat3 R;
    R.xx = 1 - 2*(q.y*q.y + q.z*q.z);
    R.xy = 2*(q.x*q.y - q.w*q.z);
    R.xz = 2*(q.x*q.z + q.w*q.y);
    
    R.yx = 2*(q.x*q.y + q.w*q.y);
    R.yy = 1 - 2*(q.x*q.x + q.z*q.z);
    R.yz = 2*(q.y*q.z - q.w*q.x);
    
    R.zx = 2*(q.x*q.z - q.w*q.y);
    R.zy = 2*(q.y*q.z + q.w*q.x);
    R.zz = 1 - 2*(q.x*q.x + q.y*q.y);
    
    
    return R;
    
    
    
}


#endif
