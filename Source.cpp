#define _USE_MATH_DEFINES
#include <cmath>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

std::string loadShaderSource(const char* filepath) {
    std::ifstream file(filepath);
    std::stringstream buffer;

    if (!file.is_open()) {
        std::cerr << "Hiba: nem sikerult megnyitni a fajlt: " << filepath << std::endl;
        return "";
    }

    buffer << file.rdbuf();
    return buffer.str();
}


int main() {
    // GLFW inicializálása
    if (!glfwInit()) return -1;

    // Ablak beállítások: 600x600, nem átméretezhető
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* window = glfwCreateWindow(600, 600, "Grafika Beadando", nullptr, nullptr);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glewInit();

    // V-Sync bekapcsolása (hogy ne legyen túl gyors)
    glfwSwapInterval(1);

    // Shaderek fordítása és program összeállítása
    std::string vertexCode = loadShaderSource("vertexShader.glsl");
    const char* vShaderCode = vertexCode.c_str();
    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vShaderCode, nullptr);
    glCompileShader(vs);

    std::string fragmentCode = loadShaderSource("fragmentShader.glsl");
    const char* fShaderCode = fragmentCode.c_str();

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fShaderCode, nullptr);
    glCompileShader(fs);

    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glUseProgram(program);

    // Teljes ablakot lefedő téglalap (két háromszög)
    float vertices[] = {
        -1.0f, -1.0f,   1.0f, -1.0f,  -1.0f,  1.0f,
        -1.0f,  1.0f,   1.0f, -1.0f,   1.0f,  1.0f
    };

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Mozgási változók
    float posX = 300.0f;
    float posY = 300.0f;
    float velX = 0.0f;
    float velY = 0.0f;
    float radius = 50.0f;
    bool isMoving = false;

    while (!glfwWindowShouldClose(window)) {

        // ---INPUT LOGIKA---//
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            posX = 300.0f; posY = 300.0f;
            float angleRads = 25.0f * (static_cast<float>(M_PI) / 180.0f);
            velX = 3.0f * cos(angleRads); velY = 3.0f * sin(angleRads);
            isMoving = true;
        }
        if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
            posX = 300.0f; posY = 300.0f;
            velX = 3.f; velY = 0.0f;
            isMoving = true;
        }
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            isMoving = false;
            posX = 300.0f; posY = 300.0f;
        }if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);


        // --- LOGIKA ---
        if (isMoving) {
            posX += velX;
            posY += velY;

            // Pattanás arányosítva (széleket érintve)
            if (posX + radius > 600.0f) {
                float overshoot = (posX + radius) - 600.0f;
                posX = 600.0f - radius - overshoot;
                velX *= -1.0f;
            }
            else if (posX - radius < 0.0f) {
                float overshoot = 0.0f - (posX - radius);
                posX = radius + overshoot;
                velX *= -1.0f;
            }

            if (posY + radius > 600.0f) {
                float overshoot = (posY + radius) - 600.0f;
                posY = 600.0f - radius - overshoot;
                velY *= -1.0f;
            }
            else if (posY - radius < 0.0f) {
                float overshoot = 0.0f - (posY - radius);
                posY = radius + overshoot;
                velY *= -1.0f;
            }
        }

        // Színfelcserélés feltétele: ha a kör közepe elhagyja a [200, 400] tartományt
        bool inverted = (posX < 200.0f || posX > 400.0f);

        // --- RENDERELÉS ---
        glClear(GL_COLOR_BUFFER_BIT);

        // Uniformok frissítése
        glUniform2f(glGetUniformLocation(program, "u_pos"), posX, posY);
        glUniform1i(glGetUniformLocation(program, "u_inverted"), (int)inverted);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Erőforrások felszabadítása
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glfwTerminate();

    return 0;
}