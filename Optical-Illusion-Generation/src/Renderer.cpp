#include "Renderer.h"

#include <iostream>

void GLClearError()
{
    while (glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        std::cout << "[OpenGL Error] (" << error << ")" << function <<
            " " << file << ": " << line << std::endl;
        return false;
    }

    return true;
}

void Renderer::Clear() const
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Renderer::Draw(const VertexArray& vao, const IndexBuffer& ibo, const Shader& shader) const
{

    shader.Bind();
    vao.Bind();
    ibo.Bind();
    GLCall(glDrawElements(GL_TRIANGLES, ibo.GetCount(), GL_UNSIGNED_INT, nullptr));
}

void Renderer::DrawLineArrays(const VertexArray& vao, const Shader& shader) const
{
    shader.Bind();
    vao.Bind();
    GLCall(glDrawArrays(GL_LINES, 0, 2));
}

