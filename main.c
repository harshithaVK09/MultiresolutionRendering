#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include <chrono>
#include <cstdlib>

const char* vertexShaderSource = R"(
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 uMVP;
void main() {
    gl_Position = uMVP * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec3 uColor;
void main() {
    FragColor = vec4(uColor, 1.0);
}
)";

GLuint createVAO(const std::vector<float>& vertices) {
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    return VAO;
}

struct Object {
    glm::vec3 position;
    float rotation = 0.0f;
    std::vector<GLuint> vaos;
    std::vector<int> vertexCounts;
    std::vector<float> lodThresholds;

    int render(GLuint prog, GLint loc_uMVP, GLint loc_uColor, const glm::mat4& view, const glm::mat4& proj, const glm::vec3& cameraPos) {
        rotation += 0.005f;
        glUseProgram(prog);

        glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
        model = glm::rotate(model, rotation, glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 mvp = proj * view * model;

        glUniformMatrix4fv(loc_uMVP, 1, GL_FALSE, glm::value_ptr(mvp));

        float distance = glm::length(position - cameraPos);
        int lod = 0;

        if (distance <= lodThresholds[0]) lod = 0;
        else if (distance <= lodThresholds[1]) lod = 1;
        else lod = 2;

        glm::vec3 color = (lod == 0) ? glm::vec3(1.0f,0.0f,0.0f) :
                          (lod == 1) ? glm::vec3(0.0f,1.0f,0.0f) :
                                       glm::vec3(0.0f,0.0f,1.0f);

        glUniform3f(loc_uColor, color.r, color.g, color.b);
        glBindVertexArray(vaos[lod]);
        glDrawArrays(GL_TRIANGLES, 0, vertexCounts[lod]);

        return lod;
    }
};

static GLuint compileShader(GLenum type, const char* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);

    if (!ok) {
        char buf[1024];
        glGetShaderInfoLog(s, 1024, nullptr, buf);
        std::cerr << "Shader compile error: " << buf << "\n";
    }

    return s;
}

int main() {
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800,600,"MultiresVR (Final)", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) return -1;

    glViewport(0,0,800,600);
    glEnable(GL_DEPTH_TEST);

    std::vector<float> lod0 = {
        -0.5f, -0.5f, 0.0f,
         0.5f, -0.5f, 0.0f,
         0.0f,  0.5f, 0.0f
    };

    std::vector<float> hexPts = {
         0.0f, 0.5f,
         0.4f, 0.2f,
         0.4f,-0.2f,
         0.0f,-0.5f,
        -0.4f,-0.2f,
        -0.4f, 0.2f
    };

    std::vector<float> lod1;

    for (int i=0;i<6;++i) {
        float ax = hexPts[(i*2)%12], ay = hexPts[(i*2)%12 + 1];
        float bx = hexPts[((i+1)%6)*2], by = hexPts[((i+1)%6)*2 + 1];

        lod1.insert(lod1.end(), {
            0.0f,0.0f,0.0f,
            ax,ay,0.0f,
            bx,by,0.0f
        });
    }

    const int segments = 36;
    std::vector<float> lod2;
    float radius = 0.5f;

    for (int i=0;i<segments;i++) {
        float a = glm::radians((float)i / segments * 360.0f);
        float b = glm::radians((float)(i+1) / segments * 360.0f);

        lod2.insert(lod2.end(), {
            0.0f, 0.0f, 0.0f,
            radius * cosf(a), radius * sinf(a), 0.0f,
            radius * cosf(b), radius * sinf(b), 0.0f
        });
    }

    GLuint vao0 = createVAO(lod0);
    GLuint vao1 = createVAO(lod1);
    GLuint vao2 = createVAO(lod2);

    GLuint vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glDeleteShader(vs);
    glDeleteShader(fs);

    glUseProgram(program);

    GLint loc_uMVP = glGetUniformLocation(program, "uMVP");
    GLint loc_uColor = glGetUniformLocation(program, "uColor");

    std::vector<Object> scene;

    auto addObjAt = [&](float x) {
        scene.push_back({
            glm::vec3(x, 0.0f, 0.0f),
            0.0f,
            { vao0, vao1, vao2 },
            { (int)(lod0.size()/3), (int)(lod1.size()/3), (int)(lod2.size()/3) },
            { 2.0f, 5.0f }
        });
    };

    addObjAt(-0.8f);
    addObjAt(0.0f);
    addObjAt(0.8f);

    float cameraZ = 2.0f;
    bool prevA = false;

    auto lastFpsTime = std::chrono::high_resolution_clock::now();
    int frames = 0;

    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) cameraZ -= 0.03f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) cameraZ += 0.03f;

        cameraZ = glm::clamp(cameraZ, 0.5f, 50.0f);

        bool aNow = (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS);
        if (aNow && !prevA) {
            float rx = ((rand() % 240) - 120) / 100.0f;
            addObjAt(rx);
            std::cout << "Spawned object at x: " << rx << "\n";
        }
        prevA = aNow;

        glClearColor(0.12f,0.12f,0.12f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 camPos(0.0f, 0.0f, cameraZ);
        glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.0f), glm::vec3(0.0f,1.0f,0.0f));
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.1f, 100.0f);

        int totalVerticesThisFrame = 0;
        int drawCallsThisFrame = 0;

        for (auto &obj : scene) {
            int lod = obj.render(program, loc_uMVP, loc_uColor, view, proj, camPos);
            totalVerticesThisFrame += obj.vertexCounts[lod];
            drawCallsThisFrame++;
        }

        frames++;
        auto now = std::chrono::high_resolution_clock::now();
        float elapsed = std::chrono::duration<float>(now - lastFpsTime).count();

        if (elapsed >= 1.0f) {
            float fps = frames / elapsed;
            float frameTimeMs = 1000.0f / fps;

            std::cout << "FPS: " << fps << "\n";
            std::cout << "Frame Time: " << frameTimeMs << " ms\n\n";

            frames = 0;
            lastFpsTime = now;
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
