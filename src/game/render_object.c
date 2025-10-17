#include "render_object.h"

#include <common/types.h>
#include <common/log.h>
#include "vertex.h"

void setup_vertex_attribs(void) {
    // vec3 pos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, pos));
    glEnableVertexAttribArray(0);

    // vec3 normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(1);

    // vec2 uv
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glEnableVertexAttribArray(2);

    // vec4 color
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(3);
}

RenderObject create_render_object(MeshData* mesh) {
    RenderObject render_obj = { .mesh = mesh };

    // generate objects
    glGenVertexArrays(1, &render_obj.VAO);
    glGenBuffers(1, &render_obj.VBO);
    glGenBuffers(1, &render_obj.EBO);

    // bind 
    glBindVertexArray(render_obj.VAO);

    // pass data
    glBindBuffer(GL_ARRAY_BUFFER, render_obj.VBO);
    glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count * sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_obj.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count * sizeof(u32), mesh->indices, GL_STATIC_DRAW);

    // vertex attribs
    setup_vertex_attribs();

    // unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 

    LOG("created render object");
    return render_obj;
}

void delete_render_object(RenderObject render_obj) {
    glDeleteVertexArrays(1, &render_obj.VAO);
    glDeleteBuffers(1, &render_obj.VBO);
    glDeleteBuffers(1, &render_obj.EBO);
}
