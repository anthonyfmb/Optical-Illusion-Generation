//#include <GL/glew.h>
//#include <GLFW/glfw3.h>
//#include <iostream>
//#include <cmath>
//
//typedef struct vec2 {
//    float x;
//    float y;
//} vec2;
//
//vec2* vec2_init(float _x, float _y) {
//    vec2* v = (vec2*)malloc(sizeof(vec2));
//    v->x = _x;
//    v->y = _y;
//    return v;
//}
//
//typedef struct vec3 {
//    float x;
//    float y;
//    float z;
//} vec3;
//
//vec3* vec3_init(float _x, float _y, float _z) {
//    vec3* v = (vec3*)malloc(sizeof(vec3));
//    v->x = _x;
//    v->y = _y;
//    v->z = _z;
//    return v;
//}
//
//const int screenWidth = 640;
//const int screenHeight = 480;
//const float PI = 3.14159265;
//
//const char* vertexShaderSource = R"(
//    #version 330 core
//    layout (location = 0) in vec2 aPos;
//
//    void main() {
//        gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
//    }
//)";
//
//const char* fragmentShaderSource = R"(
//    #version 330 core
//    out vec4 FragColor;
//    uniform vec3 color;
//
//    void main() {
//        FragColor = vec4(color, 1.0);
//    }
//)";
//
//void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
//    glViewport(0, 0, width, height);
//}
//
//void processInput(GLFWwindow* window) {
//    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//        glfwSetWindowShouldClose(window, true);
//}
//
//unsigned int createCircleVertexArray(float centerX, float centerY, float radius, int segments) {
//    unsigned int VAO, VBO;
//    glGenVertexArrays(1, &VAO);
//    glGenBuffers(1, &VBO);
//
//    glBindVertexArray(VAO);
//    glBindBuffer(GL_ARRAY_BUFFER, VBO);
//
//    float* vertices = new float[2 * (segments + 1)];
//    for (int i = 0; i <= segments; ++i) {
//        float theta = 2.0f * PI * static_cast<float>(i) / static_cast<float>(segments);
//        vertices[2 * i] = centerX + radius * cosf(theta);
//        vertices[2 * i + 1] = centerY + radius * sinf(theta);
//    }
//
//    glBufferData(GL_ARRAY_BUFFER, 2 * (segments + 1) * sizeof(float), vertices, GL_STATIC_DRAW);
//    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
//    glEnableVertexAttribArray(0);
//
//    delete[] vertices;
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//
//    return VAO;
//}
//
//void renderCircle(unsigned int VAO, vec3 circleColor, int segments, unsigned int shaderProgram) {
//    glUseProgram(shaderProgram);
//    glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, &circleColor.x);
//
//    glBindVertexArray(VAO);
//    glDrawArrays(GL_LINE_LOOP, 0, static_cast<GLsizei>(segments + 1));
//}
//
//int main() {
//    if (!glfwInit()) {
//        std::cerr << "Failed to initialize GLFW" << std::endl;
//        return -1;
//    }
//
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
//    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//
//    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "OpenGL Circle", nullptr, nullptr);
//    if (!window) {
//        std::cerr << "Failed to create GLFW window" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//
//    glfwMakeContextCurrent(window);
//    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//    if (glewInit() != GLEW_OK) {
//        std::cerr << "Failed to initialize GLEW" << std::endl;
//        glfwTerminate();
//        return -1;
//    }
//
//    // Compile shaders
//    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
//    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
//    glCompileShader(vertexShader);
//    // Check for shader compile errors...
//
//    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
//    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
//    glCompileShader(fragmentShader);
//    // Check for shader compile errors...
//
//    unsigned int shaderProgram = glCreateProgram();
//    glAttachShader(shaderProgram, vertexShader);
//    glAttachShader(shaderProgram, fragmentShader);
//    glLinkProgram(shaderProgram);
//    // Check for linking errors...
//
//    glDeleteShader(vertexShader);
//    glDeleteShader(fragmentShader);
//
//    int segments = 100; // Number of segments to approximate the circle
//
//    vec3* circleColor = vec3_init(1.0f, 1.0f, 1.0f); // White color for the circle
//    unsigned int circleVAO = createCircleVertexArray(screenWidth / 2.0f, screenHeight / 2.0f, 100.0f, segments);
//
//    while (!glfwWindowShouldClose(window)) {
//        processInput(window);
//
//        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
//        glClear(GL_COLOR_BUFFER_BIT);
//
//        renderCircle(circleVAO, (*circleColor), segments, shaderProgram);
//
//        glfwSwapBuffers(window);
//        glfwPollEvents();
//    }
//
//    glDeleteVertexArrays(1, &circleVAO);
//    glDeleteProgram(shaderProgram);
//
//    glfwTerminate();
//    return 0;
//}
