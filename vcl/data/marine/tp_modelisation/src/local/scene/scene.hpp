
/** TP 4ETI - CPE Lyon - 2015/2016 */

#pragma once

#ifndef SCENE_HPP
#define SCENE_HPP

#include <GL/gl.h>
#include <GL/glew.h>

#include "../../lib/3d/mat3.hpp"
#include "../../lib/3d/vec3.hpp"
#include "../../lib/mesh/mesh.hpp"
#include "../../lib/opengl/mesh_opengl.hpp"
#include "../../lib/interface/camera_matrices.hpp"

#include <vector>


class myWidgetGL;

class scene
{
public:

    scene();



    /** \brief Method called only once at the beginning (load off files ...) */
    void load_scene();

    /** \brief Method called at every frame */
    void draw_scene();

    /** Set the pointer to the parent Widget */
    void set_widget(myWidgetGL* widget_param);


private:

    /** Load a texture from a given file and returns its id */
    GLuint load_texture_file(std::string const& filename);

    /** Access to the parent object */
    myWidgetGL* pwidget;

    /** Default id for the texture (white texture) */
    GLuint texture_default;

    /** The id of the shader do draw meshes */
    GLuint shader_program_id;


    // Data of the scene
    cpe::mesh mesh_dinosaur;
    cpe::mesh_opengl mesh_dinosaur_opengl;
    GLuint texture_dinosaur;

    cpe::mesh mesh_camel;
    cpe::mesh_opengl mesh_camel_opengl;

    cpe::mesh mesh_ground;
    cpe::mesh_opengl mesh_ground_opengl;




};

#endif
