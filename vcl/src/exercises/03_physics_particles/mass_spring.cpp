
#include "mass_spring.hpp"


#ifdef EXERCISE_MASS_SPRING

using namespace vcl;


static void set_gui(timer_basic& timer);
static void verletIntegration(std::vector<particle_element>& pi, float h, float mu, vec3 g, float K , float L0, int n);


/** Compute spring force applied on pi with respect to pj */
vec3 spring_force(const vec3& pi, const vec3& pj, float L0, float K, float m)
{
    const vec3 p = pi-pj;
    float L = norm(p);
    const vec3 u = normalize(p);

    const vec3 F = -K/m * (L-L0) * u;
    return F;
}


void scene_exercise::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& , gui_structure& gui)
{
    gui.show_frame_camera = false;
    shaders["segment_immediate_mode"] = create_shader_program("../../shaders/segment_immediate_mode/segment_immediate_mode.vert.glsl","../../shaders/segment_immediate_mode/segment_immediate_mode.frag.glsl");
    segment_drawer.init();
    sphere = mesh_drawable( mesh_primitive_sphere(1.0f));


    std::vector<vec3> borders_segments = {{-1,-1,-1},{1,-1,-1}, {1,-1,-1},{1,1,-1}, {1,1,-1},{-1,1,-1}, {-1,1,-1},{-1,-1,-1},
                                          {-1,-1,1} ,{1,-1,1},  {1,-1,1}, {1,1,1},  {1,1,1}, {-1,1,1},  {-1,1,1}, {-1,-1,1},
                                          {-1,-1,-1},{-1,-1,1}, {1,-1,-1},{1,-1,1}, {1,1,-1},{1,1,1},   {-1,1,-1},{-1,1,1}};
    borders = segments_gpu(borders_segments);
    borders.uniform_parameter.color = {0,0,0};


    pa.p = {0,0,0};
    pa.v = {0,0,0};
    

    int N = 4;  //number of masses
    for(int i=0; i<N; i++){
        pi.push_back(particle_element());
    }
    
    
    
    pi[0].p = {0,0,0};
    pi[0].v = {0,0,0};
    
    pi[1].p = {0.4,0,0};
    pi[1].v = {0,0,0};
    
    pi[2].p ={0.6,0,0};
    pi[2].v = {0,0,0};
    
    pi[3].p ={1,0,0};
    pi[3].v = {0,0,0};
    
    
    
    
    
//    float step = 0.5;
//    float z0 = 0;
//    for (particle_element p : pi){
//        p.p = {z0,0,0};
//        p.v = {0,0,0};
//        z0 += step;
//    }
 
}





void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    const float dt = timer.update();
    set_gui(timer);

    // Force constant time step
    float h = dt<=1e-6f? 0.0f : timer.scale*0.005f;


    const float K = 1;        // spring stiffness
    
    const float L0 = 0.33;

    float mu       = 0.0f;       // damping coefficient
    const vec3 g  = {0,0,0}; // gravity

    // Forces

    // Numerical Integration (explicit Euler)
//    pb.p = pb.p + h*pb.v;
//    pb.v = pb.v + h*(fg+f_ba-mu*pb.v);
    
    
    //Verlet
  
    int n = 1; // neighbors linked
    
    verletIntegration(pi, h, mu , g, K, L0, n);


    pi[pi.size()-1].p = {1.5,0,0};

    // Display

//    // particle pa
//    sphere.uniform_parameter.translation = pa.p;
//    sphere.uniform_parameter.scaling = 0.05f;
//    sphere.uniform_parameter.color = {0,0,0};
//    sphere.draw(shaders["mesh"], scene.camera);

    float col =0;
    for (particle_element p : pi ){
        if ( col == pi.size()-1){
            col = 0;
        }
        sphere.uniform_parameter.translation = p.p;
        sphere.uniform_parameter.scaling = 0.05f;
        sphere.uniform_parameter.color = {0,0,col};
        sphere.draw(shaders["mesh"], scene.camera);
        col++;
    }
    
    for (unsigned k=0; k<pi.size()-1; k++){
        segment_drawer.uniform_parameter.p1 = pi[k].p;
        segment_drawer.uniform_parameter.p2 = pi[k+1].p;
        segment_drawer.draw(shaders["segment_immediate_mode"],scene.camera);
    }





    borders.draw(shaders["curve"], scene.camera);
}


/** Part specific GUI drawing */
static void set_gui(timer_basic& timer)
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

static void verletIntegration(std::vector<particle_element>& p, float h, float mu, vec3 g, float K, float L0, int n ){
    float m = 1.0f;
    float mk = 0;
    vec3 fg = {0,0,0};
    vec3 f_pp_pos = {0,0,0};
    vec3 f_pp_neg = {0,0,0};
    for(size_t k = 1; k<p.size()-1; k++){
        mk = m;
        fg = m*g;
        f_pp_pos = {0,0,0};
        f_pp_neg = {0,0,0};
        for(unsigned i =1 ; i<=n; i++){
           
            if(k-i>=0){
           f_pp_pos += spring_force(p[k].p, p[k-i].p, L0, K, m);
            }
        }
        
        for(unsigned i =1 ; i<=n; i++){
            
            if(k+i<p.size()){
                f_pp_neg += spring_force(p[k].p, p[k+i].p, L0, K, m);
            }
        }
        
        
        p[k].v = p[k].v + h*(fg+ f_pp_pos + f_pp_neg - mu*p[k].v);
    }
    
    for(size_t k = 1; k<p.size()-1; k++){
        p[k].p = p[k].p + h*p[k].v;
        
//    pb.v = pb.v + h*(fg+f_ba-mu*pb.v);
//    pb.p = pb.p + h*pb.v;
}
}


#endif
