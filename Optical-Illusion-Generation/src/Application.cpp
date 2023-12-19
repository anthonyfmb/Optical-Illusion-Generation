#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/color_space.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

enum Type {
    FIRST = 1,
    SECOND = 2,
    THIRD = 3
};

typedef struct line {
    Shader* shader;
    VertexArray* vao;
    VertexBuffer* vbo;
    float* vertices;
    glm::vec2 startPoint;
    glm::vec2 endPoint;
    glm::vec3 lineColor;
} line;

typedef struct circle {
    Shader* shader;
    VertexArray* vao;
    VertexBuffer* vbo;
    IndexBuffer* ibo;
    float* vertices;
    glm::vec2 center;
    float radius;
    glm::vec3 circleColor;
} circle;

typedef struct rect {
    Shader* shader;
    VertexBuffer* vbo;
    VertexArray* vao;
    IndexBuffer* ibo;
    float* vertices;
    glm::vec2 center;
    float radius;
    glm::vec3 circleColor;
} rect;

struct Illusion {
    Type t;
    int iterations;
    float size;
    float l_size; // for first illusion
    double intensity;
    float angle;
    int layers;
    float* layer_offsets;
};

line* line_init(glm::vec2 start, glm::vec2 end) {

    line* l = (line*)malloc(sizeof(line));

    float x1 = start.x;
    float y1 = start.y;
    float x2 = end.x;
    float y2 = end.y;

    x1 = 2 * x1 - 1;
    y1 = 2 * y1 - 1;

    x2 = 2 * x2 - 1;
    y2 = 2 * y2 - 1;

    l->startPoint = start;
    l->endPoint = end;
    l->lineColor = glm::vec3(0.5f, 0.5f, 0.5f);

    l->vertices = (float*)(malloc(sizeof(float) * 6));
    l->vertices[0] = x1;
    l->vertices[1] = y1;
    l->vertices[2] = x2;
    l->vertices[3] = y2;

    VertexArray* vao = new VertexArray();
    VertexBuffer* vbo = new VertexBuffer(l->vertices, sizeof(float) * 4);

    VertexBufferLayout layout;
    layout.Push<float>(2);
    (*vao).AddBuffer((*vbo), layout);

    Shader* shader = new Shader("res/shaders/FirstIllusion.shader");
    (*shader).Bind();
    (*shader).SetUniform4f("u_Color", 0.5f, 0.5f, 0.5f, 1.0f);

    (*vao).Unbind();
    (*shader).Unbind();
    (*vbo).Unbind();

    l->vao = vao;
    l->vbo = vbo;
    l->shader = shader;

    return l;
}

circle* circle_init(glm::vec2 center, float radius, float intensity) 
{
    circle* c = (circle*)malloc(sizeof(circle));

    float x = center.x;
    float y = center.y;

    x = 2 * x - 1;
    y = 2 * y - 1;

    center.x = x;
    center.y = y;

    c->center = center;
    c->radius = radius;
    c->circleColor = glm::vec3(1.0f, 1.0f, 1.0f);

    int segments = 50;

    c->vertices = new float[2 * (segments + 1)];
    for (int i = 0; i <= segments; ++i) {
        float theta = 2.0f * 3.14159 * static_cast<float>(i) / static_cast<float>(segments);
        c->vertices[2 * i] = c->center.x + radius * cosf(theta);
        c->vertices[2 * i + 1] = c->center.y + radius * sinf(theta);
    }

    VertexArray* vao = new VertexArray();
    VertexBuffer* vbo = new VertexBuffer(c->vertices, 2 * (segments + 1) * sizeof(float));
    

    VertexBufferLayout layout;
    layout.Push<float>(2);
    (*vao).AddBuffer((*vbo), layout);

    unsigned int* indices = new unsigned int[3 * segments];
    for (int i = 0; i < segments; ++i) {
        indices[3 * i] = 0;
        indices[3 * i + 1] = i + 1;
        indices[3 * i + 2] = i + 2;
    }
    indices[3 * segments - 1] = 1;

    IndexBuffer* ibo = new IndexBuffer(indices, 3 * segments);

    Shader* shader = new Shader("res/shaders/FirstIllusion.shader");
    (*shader).Bind();
    (*shader).SetUniform4f("u_Color", 1.0f, 1.0f, 1.0f, 1.0f);

    (*vao).Unbind();
    (*shader).Unbind();
    (*vbo).Unbind();
    (*ibo).Unbind();
    
    c->vao = vao;
    c->vbo = vbo;
    c->ibo = ibo;
    c->shader = shader;

    return c;
}

rect* rect_init(glm::vec2 center, float length, const std::string& texturePath, float angle)
{
    rect* r = (rect*)malloc(sizeof(rect));

    center.x = 2 * center.x - 1;
    center.y = 2 * center.y - 1;

    float positions[] = {
        center.x - length, center.y - length, 0.0f, 0.0f,
        center.x + length, center.y - length, 1.0f, 0.0f,
        center.x + length, center.y + length, 1.0f, 1.0f,
        center.x - length, center.y + length, 0.0f, 1.0f 
    };

    r->center = center;

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    VertexArray* vao = new VertexArray();
    VertexBuffer* vbo = new VertexBuffer(positions, 4 * 4 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    (*vao).AddBuffer((*vbo), layout);

    IndexBuffer* ibo = new IndexBuffer(indices, 6);

    glm::mat4 proj = glm::mat4(1.0f);
    proj = glm::rotate(proj, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 rot = glm::mat4(1.0f);
    rot = glm::rotate(rot, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(-center.x, -center.y, 0.0f));
    glm::mat4 f = glm::inverse(translate) * rot * translate;

    //unsigned int rendererID = (*r->shader).GetRendererID();

    //unsigned int modelLoc = glGetUniformLocation(rendererID, "u_MVP");
    //glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(f));

    Shader* shader = new Shader("res/shaders/Texture.shader");
    (*shader).Bind();
    (*shader).SetUniformMat4f("u_MVP", proj);

    Texture* texture = new Texture(texturePath);
    (*texture).Bind();
    (*shader).SetUniform1i("u_Texture", 0);

    (*vao).Unbind();
    (*shader).Unbind();
    (*vbo).Unbind();
    (*ibo).Unbind();

    r->vao = vao;
    r->vbo = vbo;
    r->ibo = ibo;
    r->shader = shader;


    return r;
}

glm::vec3 GenerateContrastHue(const glm::vec3 color, float distance)
{
    distance = distance * 140;

    glm::vec3 hsv = glm::hsvColor(color);

    hsv[0] = glm::mod(hsv[0] + distance, 360.0f);

    return glm::rgbColor(hsv);
}

glm::vec3 GenerateContrastLight(const glm::vec3 color, double distance)
{
    distance = distance * 100;
    
    glm::vec3 hsv = glm::hsvColor(color);

    hsv[2] = distance;

    return glm::rgbColor(hsv);
}


Illusion* parse(std::istream& istr) {
    Illusion* illusion = (Illusion*)malloc(sizeof(Illusion));
    Type t;
    int iterations;
    float size;
    float l_size;
    double intensity;
    float angle;
    int layers;
    float* layer_offsets;

    std::string l;
    std::getline(istr, l);
    t = (Type) std::stoi(l);

    if (t == FIRST)
    {
        std::getline(istr, l);
        iterations = std::stoi(l);
        std::getline(istr, l);
        size = std::stof(l);
        std::getline(istr, l);
        l_size = std::stof(l);
        std::getline(istr, l);
        intensity = std::stof(l);

        illusion->t = t;
        illusion->iterations = iterations;
        illusion->size = size;
        illusion->l_size = l_size;
        illusion->intensity = intensity;
    }
    else if (t == SECOND)
    {
        std::getline(istr, l);
        iterations = std::stoi(l);
        std::getline(istr, l);
        size = std::stof(l);
        std::getline(istr, l);
        intensity = std::stof(l);
        std::getline(istr, l);
        angle = std::stof(l);

        illusion->t = t;
        illusion->iterations = iterations;
        illusion->size = size;
        illusion->intensity = intensity;
        illusion->angle = angle;
    }
    else if (t == THIRD)
    {
        std::getline(istr, l);
        iterations = std::stoi(l);
        std::getline(istr, l);
        size = std::stof(l);
        std::getline(istr, l);
        angle = std::stof(l);
        std::getline(istr, l);
        layers = std::stoi(l);
        layer_offsets = (float*) malloc(sizeof(float) * layers * 2);
        for (int i = 0; i < 2 * layers; i += 2) 
        {
            std::getline(istr, l);
            layer_offsets[i] = std::stof(l);
            std::getline(istr, l);
            layer_offsets[i + 1] = std::stof(l);
        }

        illusion->t = t;
        illusion->iterations = iterations;
        illusion->size = size;
        illusion->angle = angle;
        illusion->layers = layers;
        illusion->layer_offsets = layer_offsets;
    }
    else
    {
        std::cout << "Error reading file" << std::endl;
    }

    return illusion;
}

std::string getNextLine(std::istream& istr) {
    const std::string comment = "#";
    std::string line = "";
    while (line == "") {
        std::getline(istr, line);
        // Skip comments and empty lines
        auto found = line.find(comment);
        if (found != std::string::npos)
            line = line.substr(0, found);
        //line = trim(line);
    }
    return line;
}


// Remove empty lines, comments, and trim leading and trailing whitespace
std::stringstream preprocessStream(std::istream& istr) {
    istr.exceptions(istr.badbit | istr.failbit);
    std::stringstream ss;

    try {
        while (true) {
            std::string line = getNextLine(istr);
            ss << line << std::endl;	// Add newline after each line
        }								// Stream always ends with a newline

    }
    catch (const std::exception& e) {
        if (!istr.eof()) throw e;
    }

    return ss;
}

// Parse a file
Illusion* parseFile(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open())
        throw std::runtime_error("failed to open " + filename);

    // Preprocess to remove comments & whitespace
    std::stringstream ss = preprocessStream(file);
    return parse(ss);
}


int main(int argc, char** argv)
{
    std::string configFile = std::string("models/third.txt");
    //std::string configFile = argv[1];
    Illusion* il = parseFile(configFile);
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1000, 700, "Hello World", NULL, NULL); // 640 480
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) {
        std::cout << "Error!" << std::endl;
        return -1;
    }

    Renderer renderer;

    int iter = il->iterations + 1;
    int t = il->t;

    //int iter = pow(2, iterations - 1);

    std::vector<line*> lines;
    std::vector<circle*> circles;
    std::vector<rect*> rects;
    std::vector<rect*> almonds;

    if (t == FIRST)
    {
        // These numbers came out of thin air
        float rad = il->size * (float)iter / pow(iter, 2.5);
        //glLineWidth(rad * 2 * 200);

        // 450 is to scale it, no idea what system of measurement glLineWidth uses
        glLineWidth(450 * il->l_size * (float)iter / pow(iter, 2.5));

        for (int i = 1; i < iter; i++) 
        {
            line* l = line_init(glm::vec2((float)i / iter, 0), glm::vec2((float)i / iter, 1));
            lines.push_back(l);
        }

        for (int i = 1; i < iter; i++) 
        {
            line* l = line_init(glm::vec2(0, (float)i/iter), glm::vec2(1, (float)i / iter));
            lines.push_back(l);
        }

        for (int i = 1; i < iter; i++) 
        {
            for (int j = 1; j < iter; j++) 
            {
                circle* c = circle_init(glm::vec2((float)i/iter, (float)j/iter), rad, 0.0);
                circles.push_back(c);
            }
        }
    }
    else if (t == SECOND)
    {
        float size = il->size * (float)1 / (1.5 * iter);
        for (int i = 1; i < iter; i++) 
        {
            float a = 20 * i;
            for (int j = 1; j < iter; j++) 
            {
                rect* r1 = rect_init(glm::vec2((float)i / (iter), (float)(iter - j) / iter), size, "res/textures/almond.png", a * j);
                almonds.push_back(r1);
            }
        }
    }
    else if (t == THIRD)
    {
        iter--;
        float size_factor = 1.0f;
        float size = ((float)1 / (iter)) * size_factor;

        for (int i = 0; i <= iter; i++) 
        {
            for (int j = 0; j <= iter; j++) 
            {
                rect* r1 = rect_init(glm::vec2((float)i / iter, (float)j / iter), size, "res/textures/circle.png", 0.0f);
                rects.push_back(r1);
            }
        }

        float* layer_offsets = il->layer_offsets;

        for (int i = 0; i < 2 * il->layers; i += 2) 
        {
            size = ((float)1 / (iter)) * layer_offsets[i + 1];

            int inner_iter = 2 * iter;

            if (layer_offsets[i] == -1) 
            {
                for (int i = 1; i < inner_iter; i += 2) 
                {
                    for (int j = 1; j < inner_iter; j += 2) 
                    {
                        rect* r1 = rect_init(glm::vec2((float)i / inner_iter, (float)j / inner_iter), size, "res/textures/circle.png", 0.0f);
                        rects.push_back(r1);
                    }
                }
            } 
            else
            {
                int inner_iter = layer_offsets[i] + 1;

                for (int i = 1; i < inner_iter; i++) 
                {
                    for (int j = 1; j < inner_iter; j++)
                    {
                        rect* r1 = rect_init(glm::vec2((float)i / inner_iter, (float)j / inner_iter), size, "res/textures/circle.png", 0.0f);
                        rects.push_back(r1);
                    }
                }
            }
        }
    }
    else
    {
        std::cout << "Error reading type" << std::endl;
        return -1;
    }

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        // 158, 77, 32
        // 0.62, 0.30, 0.13

        //glm::vec3 test = glm::vec3(0.62f, 0.3f, 0.13f);

        //glClearColor(1.0, 1.0, 1.0, 1.0);
        //glClearColor(0.20, 0.80, 0.20, 1.0); // rgb(50,205,50)
        
        glClear(GL_COLOR_BUFFER_BIT);

        if (t == FIRST)
        {
            glm::vec3 test = glm::vec3(0.0f, 0.0f, 0.0f);
            glm::vec3 res = GenerateContrastLight(test, 1 - il->intensity);
            //glClearColor(0.0, 0.0, 0.0, 1.0);
            glClearColor(res[0] / (float) 255, res[1] / (float)255, res[2] / (float)255, 1.0);

            for (line* l : lines) 
            {
                renderer.DrawLineArrays((*l->vao), (*l->shader));
            }

            for (circle* c : circles) 
            {
                //circle_draw(circles.at(i));
                (*c->shader).Bind();

                renderer.Draw((*c->vao), (*c->ibo), (*c->shader));
            }
        }

        else if (t == SECOND)
        {

            glm::vec3 test = glm::vec3(0.62f, 0.3f, 0.13f);
            glm::vec3 res = GenerateContrastHue(test, il->intensity);
            //glClearColor(0.0, 0.0, 0.0, 1.0);
            glClearColor(res[0], res[1], res[2], 1.0);

            float angle = 0.0f;
            float change_in_angle = il->angle;
            int row = 1;
            int col = 1;

            for (rect* r : almonds) 
            {
                if (row > (iter - 1)) 
                {
                    angle = (col)*change_in_angle;
                    row = 1;
                    col++;
                }
                (*r->shader).Bind();
                glm::vec2 center = r->center;

                glm::mat4 rot = glm::mat4(1.0f);
                rot = glm::rotate(rot, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
                glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(-center.x, -center.y, 0.0f));
                glm::mat4 f = glm::inverse(translate) * rot * translate;

                unsigned int rendererID = (*r->shader).GetRendererID();

                unsigned int modelLoc = glGetUniformLocation(rendererID, "u_MVP");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(f));

                renderer.Draw((*r->vao), (*r->ibo), (*r->shader));

                angle = angle + change_in_angle;
                row++;
            }
        }

        //GLCall(circle_draw(c));

        /*renderCircle(circleVAO, (*circleColor), segments, shaderProgram);*/

        else if (t == THIRD)
        {
            glClearColor(1.0, 1.0, 1.0, 1.0);

            float angle = 0.0f;
            float change_in_angle = il->angle;
            int row = 1;
            int col = 1;

            for (rect* r : rects) 
            {
                (*r->shader).Bind();

                glm::vec2 center = r->center;

                glm::mat4 rot = glm::mat4(1.0f);
                rot = glm::rotate(rot, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
                glm::mat4 translate = glm::translate(glm::mat4(1.0f), glm::vec3(-center.x, -center.y, 0.0f));
                glm::mat4 f = glm::inverse(translate) * rot * translate;

                unsigned int rendererID = (*r->shader).GetRendererID();

                unsigned int modelLoc = glGetUniformLocation(rendererID, "u_MVP");
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(f));


                renderer.Draw((*r->vao), (*r->ibo), (*r->shader));
                angle += change_in_angle;
                row++;
            }
        }

        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}