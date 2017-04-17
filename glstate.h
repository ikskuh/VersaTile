#ifndef GLSTATE_H
#define GLSTATE_H

#include <GL/gl3w.h>

struct GLState
{
    GLint last_program;
    GLint last_texture;
    GLint last_active_texture;
    GLint last_array_buffer;
    GLint last_element_array_buffer;
    GLint last_vertex_array;
    GLint last_blend_src;
    GLint last_blend_dst;
    GLint last_blend_equation_rgb;
    GLint last_blend_equation_alpha;
    GLint last_viewport[4];
    GLint last_scissor_box[4];
    GLboolean last_enable_blend;
    GLboolean last_enable_cull_face;
    GLboolean last_enable_depth_test;
    GLboolean last_enable_scissor_test;

    void restore();

    static GLState backup();
};


#endif // GLSTATE_H
