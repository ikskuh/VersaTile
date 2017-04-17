#include "glstate.h"

GLState GLState::backup()
{
    GLState state;
    glGetIntegerv(GL_CURRENT_PROGRAM, &state.last_program);
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &state.last_texture);
    glGetIntegerv(GL_ACTIVE_TEXTURE, &state.last_active_texture);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &state.last_array_buffer);
    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &state.last_element_array_buffer);
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &state.last_vertex_array);
    glGetIntegerv(GL_BLEND_SRC, &state.last_blend_src);
    glGetIntegerv(GL_BLEND_DST, &state.last_blend_dst);
    glGetIntegerv(GL_BLEND_EQUATION_RGB, &state.last_blend_equation_rgb);
    glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &state.last_blend_equation_alpha);
    glGetIntegerv(GL_VIEWPORT, state.last_viewport);
    glGetIntegerv(GL_SCISSOR_BOX, state.last_scissor_box);
    state.last_enable_blend = glIsEnabled(GL_BLEND);
    state.last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
    state.last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
    state.last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
    return state;
}

void GLState::restore()
{
    // Restore modified GL state
    glUseProgram(last_program);
    glActiveTexture(last_active_texture);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glBindVertexArray(last_vertex_array);
    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
    glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
    glBlendFunc(last_blend_src, last_blend_dst);
    if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
    if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
    if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
    if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
    glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
    glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
}
