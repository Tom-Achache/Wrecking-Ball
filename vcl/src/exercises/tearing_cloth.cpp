//
//  tearing_cloth.cpp
//  pgm
//
//  Created by Tom Achache on 05/03/2019.
//

#include "tearing_cloth.hpp"



#ifdef EXERCISE_TEARING_CLOTH

using namespace vcl;

const size_t N_cloth = 30;

vec3 spring_force(const vec3& pi, const vec3& pj, float L0, float K, float m);


vec3 spring_force(const vec3& pi, const vec3& pj, float L0, float K, float m)
{
    const vec3 p = pi-pj;
    float L = norm(p);
    const vec3 u = normalize(p);
    
    const vec3 F = -K * (L-L0) * u;
    return F;
}





void scene_exercise::compute_forces_sphere()
{
    float m = user_parameters.m_ball;
    collision_shapes.sphere_f = m*vec3(0,-9.81,0);
    
}
// Fill value of force applied on each particle
void scene_exercise::compute_forces( size_t c )
{
    float t = timer.t;
    float omega = 1;
    const size_t NN = force[c].size();      // Total number of particles
    const int N = size_t(std::sqrt(NN)); // Dimension of the grid in u/v direction
    
    // Update simulation parameters
    // Adapt parameter to scale with the size of the mesh
    simulation_parameters.m = user_parameters.m / float(NN);
    simulation_parameters.K = user_parameters.K;
    
    
    // Gravity
    const float m = simulation_parameters.m;   // mass of the particle
    const vec3 g = {0,-9.81f,0};
    for(size_t k=0; k<NN; ++k)
    force[c][k] = m*g;
    
    int n = N/10; //number of neighbors
    // Add other forces ...
    // You have access to the following parameters
    const float K = simulation_parameters.K;   // spring stiffness
    const float L0 = simulation_parameters.L0; // edge length in u and v direction at rest
    const float mu = user_parameters.mu;
    const float wind = user_parameters.wind;
    float thres = user_parameters.robustness;
    
    // Note: It can be convenient to navigate through the 2D (u,v) parameters of the grid
    for(int ku=0; ku<N; ++ku)
    {
        for(int kv=0; kv<N; ++kv)
        {
            
            // the index in the 1D at parameter (ku,kv) is given by kv+N*ku
            // ex. position[kv+N*ku]
            // ...
            vec3 f_up = {0,0,0};
            vec3 f_down = {0,0,0};
            vec3 f_r = {0,0,0};
            vec3 f_l = {0,0,0};
            vec3 f_diag = {0,0,0};
            vec3 f_damp = {0,0,0};
            vec3 f_wind = {0,0,0};
            
            
            for( int i = 1; i<=n ; i++){
                if(ku-i >= 0){
                    if(!removed[c].count(kv+N*(ku-i)) && !removed[c].count(kv-1+N*(ku-i)) ){
                        vec3 df =spring_force(position[c][kv+N*ku], position[c][kv+N*(ku-i)] , L0*i, (1/std::sqrt(i))*K, m);
                        f_up += df;
                        if(i==1){
                            if (std::sqrt(dot(df,df)) > thres*K*L0 ){
                                removed[c].insert(kv+N*(ku-1));
                                removed[c].insert(kv-1+N*(ku-1));
                            }
                    }
                }
            }
                            }
            
            for( int i = 1; i<=n ; i++){
                if(ku+i < N){
                    if(!removed[c].count(kv+N*ku) && !removed[c].count(kv-1+N*ku)){
                        vec3 df =spring_force(position[c][kv+N*ku], position[c][kv+N*(ku+i)] , L0*i, (1/std::sqrt(i))*K, m);
                        f_down += df;
                        if(i==1){
                            if (std::sqrt(dot(df,df)) > thres*K*L0 ){
                                removed[c].insert(kv+N*ku);
                                removed[c].insert(kv-1+N*ku);
                            }
                        }
                }
            }
            }
            
            for( int i = 1; i<=n ; i++){
                if(kv-i >= 0){
                    if(!removed[c].count((kv-1)+N*ku) && !removed[c].count(kv-1+N*(ku-1))){
                        vec3 df =spring_force(position[c][kv+N*ku], position[c][(kv-i)+N*(ku)] , L0*i, (1/std::sqrt(i))*K, m);
                        f_l += df;
                        if(i==1){
                            if (std::sqrt(dot(df,df)) > thres*K*L0 ){
                                removed[c].insert(kv-1+N*ku);
                                removed[c].insert(kv-1+N*(ku-1));
                            }
                        }
                    }
//
                }
            }
            
            for( int i = 1; i<=n ; i++){
                if(kv+i < N){
                    if(!removed[c].count((kv)+N*ku) && !removed[c].count(kv+N*(ku+i))){
                        vec3 df =spring_force(position[c][kv+N*ku], position[c][(kv+i)+N*(ku)] , L0*i, (1/std::sqrt(i))*K, m);
                        f_r += df;
                        if(i==1){
                            if (std::sqrt(dot(df,df)) > thres*K*L0 ){
                                removed[c].insert(kv+N*ku);
                                removed[c].insert(kv+N*(ku+1));
                            }
                        }
                    }
                    
                }
            }
            
            if(kv < N-1 && ku< N-1){
                if(!removed[c].count((kv)+N*(ku)))
                f_diag+= spring_force(position[c][kv+N*ku], position[c][kv+1+N*(ku+1)] , std::sqrt(2)*L0, K/2, m);
            }
            if(kv >0 && ku > 0){
                if(!removed[c].count((kv-1)+N*(ku-1)))
                f_diag+= spring_force(position[c][kv+N*ku], position[c][kv-1+N*(ku-1)] , std::sqrt(2)*L0, K/2, m);
            }
            if(kv < N-1 && ku>0){
                if(!removed[c].count((kv)+N*(ku-1)))
                    f_diag+= spring_force(position[c][kv+N*ku], position[c][kv+1+N*(ku-1)] , std::sqrt(2)*L0, K/2, m);
            }
            if(kv >0 && ku < N-1){
                if(!removed[c].count((kv-1)+N*(ku)))
                    f_diag+= spring_force(position[c][kv+N*ku], position[c][kv-1+N*(ku+1)] , std::sqrt(2)*L0, K/2, m);
            }
            
            f_damp  = -mu*speed[c][kv+N*ku];
            
            f_wind = std::cos(omega*t)*wind*vec3(0,0,1)/NN;
            force[c][kv+N*ku] += f_up + f_down + f_r + f_l + f_diag + f_damp + f_wind;
            
            }
            
        
    }
    
    
}

// Handle detection and response to collision with the shape described in "collision_shapes" variable
//void scene_exercise::collision_constraints(size_t c )
//{
//    float alpha = 0.5;
//    float beta = 0.5;
//    const size_t NN = force[c].size();
//    const int N = size_t(std::sqrt(NN));
//    const float m_ball = user_parameters.m;
//    const float m = simulation_parameters.m;
//    for(int ku=0; ku<N; ++ku)
//    {
//        for(int kv=0; kv<N; ++kv)
//        {
//            if(!removed[c].count(kv + N*ku)){
//            float d_to_sphere = std::sqrt(dot(position[c][kv + N*ku] - collision_shapes.sphere_p, position[c][kv + N*ku] - collision_shapes.sphere_p));
//            if ( d_to_sphere  < 1.1*collision_shapes.sphere_r && (position[c][kv + N*ku].y - collision_shapes.sphere_p.y) < -0.00  ){
//                vec3 n = (position[c][kv + N*ku] - collision_shapes.sphere_p)/d_to_sphere;
//                vec3 v = speed[c][kv + N*ku] - collision_shapes.sphere_v;
//                vec3 vT = dot(v,n)*n;
//                vec3 v_new = alpha*v - (beta + alpha)*vT;
////                speed[c][kv + N*ku] = v_new;
//                position[c][kv + N*ku] += ((collision_shapes.sphere_r - d_to_sphere) + 0.1*collision_shapes.sphere_r)*n ;
//                collision_shapes.sphere_v -= 0.01*alpha*collision_shapes.sphere_v;
//                collision_shapes.sphere_p += 0.01*((collision_shapes.sphere_r - d_to_sphere))*n ;
//            }
//            float yp = position[c][kv+ N*ku].y;
//            if(yp<collision_shapes.ground_height){
//                vec3 n = vec3(0,1,0);
//                vec3 v = speed[c][kv + N*ku];
//                vec3 vT = dot(v,n)*n;
//                vec3 v_new = alpha*v - (beta + alpha)*vT;
//                speed[c][kv + N*ku] = v_new;
//                position[c][kv + N*ku] += (collision_shapes.ground_height - yp)*n;
//
//
//            }
//            }
//        }
//    }// To fill ...
//
//
//}




void scene_exercise::collision_constraints(size_t c, float h )
{
    float alpha = 0.5;
    float beta = 0.5;
    const size_t NN = force[c].size();
    const int N = size_t(std::sqrt(NN));
    const float m_ball = user_parameters.m;
    const float m_cloth = simulation_parameters.m;
    const float dm = m_cloth/NN;
    const float K = user_parameters.K;   // Attempt to model to force of the clothes on the ball, not used here

    timer.update();
    float t_current = timer.t;
    
    for(int ku=0; ku<N; ++ku)
    {
        for(int kv=0; kv<N; ++kv)
        {
            if(!removed[c].count(kv + N*ku)){
                float d_to_sphere = std::sqrt(dot(position[c][kv + N*ku] - collision_shapes.sphere_p, position[c][kv + N*ku] - collision_shapes.sphere_p));
                if (d_to_sphere  < 1.15*collision_shapes.sphere_r && (position[c][kv + N*ku].y - collision_shapes.sphere_p.y) < -0.001 ){
                    vec3 n = (position[c][kv + N*ku] - collision_shapes.sphere_p)/d_to_sphere;
                    float dist_pen =1.15*collision_shapes.sphere_r - d_to_sphere ;
                    position[c][kv + N*ku] += n*(m_ball/(m_ball + dm))*dist_pen;
                    collision_shapes.sphere_p -= n*(dm/(m_ball + dm))*dist_pen;
                    timer.update();
                    float t_new = timer.t;
                    float dt = t_new - t_current;
                    vec3 tmp = collision_shapes.sphere_v - collision_shapes.sphere_v*(dm/(m_ball + dm))*dist_pen*(1/dt);
                    if (dot(tmp,tmp) > 0){collision_shapes.sphere_v = tmp;}
                    else {collision_shapes.sphere_v = vec3(0,0,0);}
                }
                float yp = position[c][kv+ N*ku].y;
                if(yp<collision_shapes.ground_height){
                    vec3 n = vec3(0,1,0);
                    vec3 v = speed[c][kv + N*ku];
                    vec3 vT = dot(v,n)*n;
                    vec3 v_new = alpha*v - (beta + alpha)*vT;
                    speed[c][kv + N*ku] = v_new;
                    position[c][kv + N*ku] += (collision_shapes.ground_height - yp)*n;
                    
                    
                }
                
                
            }
        }
    }
    
    if (collision_shapes.sphere_p.y - collision_shapes.sphere_r < collision_shapes.ground_height){
        if(collision_shapes.sphere_p.y + collision_shapes.sphere_r > collision_shapes.ground_height){
        collision_shapes.sphere_v.y =  -0.3f * collision_shapes.sphere_v.y;
        collision_shapes.sphere_p.y = collision_shapes.ground_height +collision_shapes.sphere_r;
    }
    }
//    collision_shapes.sphere_v = (collision_shapes.sphere_p - p0)/h;
    
    
}





// Initialize the geometrical model
// (this function can be called several times by the user)
void scene_exercise::initialize()
{
    
    // Number of samples of the model (total number of particles is N_cloth x N_cloth)
    
    
    // Rest length (length of an edge)
    simulation_parameters.L0 = 1.0f/float(N_cloth-1);
    
    // Create cloth mesh in its initial position
    // Horizontal grid of length 1 x 1
    collision_shapes.sphere_p = {-0.0f,5.0f,-0.5};
    collision_shapes.sphere_v = {-0.0f,-5.0f,-0.0f};
    collision_shapes.sphere_f = {-0.0f,0.0f,-0.0f};
    float y = 0.0f;
    
    
    
    for (size_t i = 0; i<clothes.size(); i++){
        vcl::mesh_drawable& cloth = clothes[i];
        
    
        const mesh base_cloth = mesh_primitive_grid(N_cloth,N_cloth,1.0f,1.0f,{-0.5,y,0},{0,1,0});
        y+=0.5;
        
        // Set particle position from cloth geometry
        
        position[i] = base_cloth.position;
        
        
        // Set hard positional constraints
        positional_constraints[i][0] = position[i][0];
        positional_constraints[i][N_cloth*(N_cloth-1)] = position[i][N_cloth*(N_cloth-1)];
        positional_constraints[i][(N_cloth-1)] = position[i][(N_cloth-1)];
        positional_constraints[i][(N_cloth)*(N_cloth)-1] = position[i][(N_cloth)*(N_cloth)-1];

        
        // Init particles data (speed, force)
        speed[i].resize(0);
        speed[i].resize(position[i].size());
        force[i].resize(0);
        force[i].resize(position[i].size());
        
        
        // Store connectivity and normals
        connectivity[i] = base_cloth.connectivity;
        normals[i] = normal(position[i],connectivity[i]);
        removed[i].clear();
        
        // Send data to GPU
        cloth.data_gpu.clear();
        cloth = mesh_drawable(base_cloth);
        cloth.uniform_parameter.shading.specular = 0.0f;
    }
    simulation_diverged = false;
    force_simulation = false;
    
    timer.update();
}

void scene_exercise::setup_data(std::map<std::string,GLuint>& shaders, scene_structure& , gui_structure& gui)
{
    gui.show_frame_camera = false;
    
    // Load shaders
    shaders["open_surface"] = create_shader_program("../../shaders/mesh_back_illumination/mesh.vert.glsl","../../shaders/mesh_back_illumination/mesh.frag.glsl");
    shaders["wireframe_quads"] = create_shader_program("../../shaders/wireframe_quads/shader.vert.glsl","../../shaders/wireframe_quads/shader.geom.glsl","../../shaders/wireframe_quads/shader.frag.glsl");
    
    // Load textures
    texture_cloth = texture_gpu(image_load_png("../../data/cloth/cloth.png"));
    texture_wood = texture_gpu(image_load_png("../../data/cloth/wood.png"));
    
    
//    texture_cloths[0] = texture_gpu(image_load_png("data/cloth/cloth.png"));

    
    // Initialize cloth geometry and particles
    initialize();
    
    // Default value for simulation parameters
    user_parameters.K = 20.0f;
    user_parameters.m = 1.0f;
    user_parameters.m_ball = 200.0f;
    user_parameters.wind = 0.0f;
    user_parameters.mu = 0.1f;
    user_parameters.robustness = 0.3f;
    
    // Set collision shapes
    collision_shapes.sphere_p = {-0.0f,5.0f,-0.5};
    collision_shapes.sphere_v = {-0.0f,-5.0f,-0.0f};
    collision_shapes.sphere_f = {-0.0f,0.0f,-0.0f};
    collision_shapes.sphere_r = 0.10f;
    collision_shapes.ground_height = -1.0f;
    
    // Init visual models
    sphere = mesh_drawable(mesh_primitive_sphere(1.0f,{0,0,0},60,60));
    ground = mesh_drawable(mesh_primitive_quad({-1,collision_shapes.ground_height-1e-3f,-1}, {1,collision_shapes.ground_height-1e-3f,-1},{-1,collision_shapes.ground_height-1e-3f,1}));
    
    timer.update();
    const float t_current = timer.t;
    
    boule.t0 = t_current;
    boule.p0 = vec3(0,2.0f,0);
    boule.v0 = vec3(0,0,0);
    
    
    // If cone
//    sphere = mesh_drawable(mesh_primitive_cone(0.3f, boule.p0, boule.p0 - vec3(0,1.0f,0)));
//    sphere.uniform_parameter.color = vec3(0.6f, 0.6f, 1.0f);
    
     /*
     // If sphere
     sphere = mesh_drawable(mesh_primitive_sphere(0.3f));
     sphere.uniform_parameter.color = vec3(0.6f, 0.6f, 1.0f);
     */

    
    gui_display_texture = true;
    gui_display_wireframe = false;
}

void scene_exercise::frame_draw(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& gui)
{
    const float dt = timer.update();
    
    set_gui();
    
    // Force constant simulation time step
    float h = dt<=1e-6f? 0.0f : timer.scale*0.0003f;
    
    
    for (size_t c = 0; c<clothes.size(); c++){
        vcl::mesh_drawable& cloth = clothes[c];
        
        if( (!simulation_diverged || force_simulation) && h>0)
        {
            compute_forces(c);
            numerical_integration(h, c);
            
            collision_constraints(c,h);                 // Detect and solve collision with other shapes
            
            hard_constraints();                      // Enforce hard positional constraints
            
            normal(position[c], connectivity[c], normals[c]); // Update normals of the cloth
            detect_simulation_divergence(c);          // Check if the simulation seems to diverge
        }
        
        
        cloth.data_gpu.update_position(position[c]);
        cloth.data_gpu.update_normal(normals[c]);
        connectivity[c] = connectivity_grid_w_holes(N_cloth, N_cloth, removed[c]);
        cloth.data_gpu.update_connectivity(connectivity[c]);
        
    }
    
    compute_forces_sphere();
    numerical_integration_sphere(h);
    
    
    display_elements(shaders, scene, gui);
    
}

void scene_exercise::numerical_integration(float h, size_t i)
{
    const size_t NN = position[i].size();
    const float m = user_parameters.m/NN;
    for(size_t k=0; k<NN; ++k)
    {
        vec3& p = position[i][k];
        vec3& v = speed[i][k];
        const vec3& f = force[i][k];
        
        v = v + h*f/m;
        p = p + h*v;
    }
}

void scene_exercise::numerical_integration_sphere(float h)
{
    
    const float m = user_parameters.m_ball;
        vec3& p = collision_shapes.sphere_p;
        vec3& v = collision_shapes.sphere_v;
        const vec3& f = collision_shapes.sphere_f;
        
        v = v + h*f/m;
        p = p + h*v;
    
}



void scene_exercise::display_elements(std::map<std::string,GLuint>& shaders, scene_structure& scene, gui_structure& )
{
    glEnable( GL_POLYGON_OFFSET_FILL );
    
    // Display cloth
    if(gui_display_texture)
    glBindTexture(GL_TEXTURE_2D, texture_cloth);
    glPolygonOffset( 1.0, 1.0 );
    for (size_t i = 0; i<clothes.size(); i++){
        vcl::mesh_drawable &cloth = clothes[i];
    cloth.draw(shaders["open_surface"],scene.camera);
    }
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);

    
    if(gui_display_wireframe)
    {
        for (size_t i = 0; i<clothes.size(); i++){
            vcl::mesh_drawable &cloth = clothes[i];
        glPolygonOffset( 1.0, 1.0 );
        cloth.draw(shaders["wireframe_quads"],scene.camera);
        }
    }
    
    
    // Display positional constraint using spheres
    sphere.uniform_parameter.scaling = 0.02f;
    for (size_t i = 0; i<clothes.size(); i++){
    
        for(const auto& constraints : positional_constraints[i])
        {
            
            sphere.uniform_parameter.translation = constraints.second;
            sphere.draw(shaders["mesh"],scene.camera);
        }
    }
    
    
    // Display sphere collision
    sphere.uniform_parameter.scaling =  collision_shapes.sphere_r;
    sphere.uniform_parameter.translation = collision_shapes.sphere_p;
    sphere.uniform_parameter.color = {1,0,0};
    sphere.draw(shaders["mesh"],scene.camera);
    
    
    // Display ground
    glBindTexture(GL_TEXTURE_2D, texture_wood);
    ground.draw(shaders["open_surface"],scene.camera);
    glBindTexture(GL_TEXTURE_2D, scene.texture_white);
}



void scene_exercise::hard_constraints()
{
    // Fixed positions of the cloth
    for (size_t i = 0; i<clothes.size(); i++){
        
    for(const auto& constraints : positional_constraints[i])
    position[i][constraints.first] = constraints.second;
    }
}




void scene_exercise::detect_simulation_divergence( size_t i )
{
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
    
    const size_t NN = position[i].size();
    for(size_t k=0; simulation_diverged==false && k<NN; ++k)
    {
        const float f = norm(force[i][k]);
        
        if( f!=f) // detect NaN in force
        {
            std::cout<<"NaN detected in forces"<<std::endl;
            simulation_diverged = true;
        }
        
        if( f>20.0f ) // detect strong force magnitude
        {
            std::cout<<" **** Warning : Strong force magnitude detected "<<f<<" at vertex "<<k<<" ****"<<std::endl;
            simulation_diverged = true;
        }
        
        if( position[i][k].x != position[i][k].x ) // detect NaN in position
        {
            std::cout<<"NaN detected in positions"<<std::endl;
            simulation_diverged = true;
        }
        
        if(simulation_diverged==true)
        {
            std::cerr<<" **** Simulation has diverged **** "<<std::endl;
            std::cerr<<" > Stop simulation iterations"<<std::endl;
            timer.stop();
        }
    }
#pragma GCC diagnostic pop
}


void scene_exercise::set_gui()
{
    // Can set the speed of the animation
    float scale_min = 0.05f;
    float scale_max = 2.0f;
    ImGui::SliderScalar("Time scale", ImGuiDataType_Float, &timer.scale, &scale_min, &scale_max, "%.2f s");
    
    float stiffness_min = 0.1f;
    float stiffness_max = 50.0f;
    ImGui::SliderScalar("Stiffness",ImGuiDataType_Float, &user_parameters.K, &stiffness_min, &stiffness_max, "%.2f s");
    
    float mu_min = 0.0f;
    float mu_max = 1.0f;
    ImGui::SliderScalar("Damping",ImGuiDataType_Float, &user_parameters.mu, &mu_min, &mu_max, "%.2f s");
    
    
    float mass_min = 0.1f;
    float mass_max = 3.0f;
    ImGui::SliderScalar("Mass",ImGuiDataType_Float, &user_parameters.m, &mass_min, &mass_max, "%.2f s");
    
    float mass_ball_min = 1.0f;
    float mass_ball_max = 1000.0f;
    ImGui::SliderScalar("Mass Ball",ImGuiDataType_Float, &user_parameters.m_ball, &mass_ball_min, &mass_ball_max, "%.2f s");
    
    float wind_min = 0.0f;
    float wind_max = 100.0f;
    ImGui::SliderScalar("Wind",ImGuiDataType_Float, &user_parameters.wind, &wind_min, &wind_max, "%.2f s");
    
    float robustness_min = 0.3f;
    float robustness_max = 3.0f;
    ImGui::SliderScalar("Robustness",ImGuiDataType_Float, &user_parameters.robustness, &robustness_min, &robustness_max, "%.2f s");
    
    ImGui::Checkbox("Wireframe",&gui_display_wireframe);
    ImGui::Checkbox("Texture",&gui_display_texture);
    
    
    // Start and stop animation
    if (ImGui::Button("Stop anim"))
    timer.stop();
    if (ImGui::Button("Start anim"))
    {
        if( simulation_diverged )
        force_simulation=true;
        
        timer.start();
    }
    
    if (ImGui::Button("Initialize Geometry"))
    initialize();
    
}








#endif
