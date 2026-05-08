#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include <cmath>

using namespace glm;
using namespace std;


const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;


float radiusCamera = 9.0f;
float angleCamera = 0.0f;
float cameraHeight = 3.0f;
bool lightOn = true;
bool lKeyWasPressed = false;


const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 lightPos;
uniform bool lightOn;
uniform bool isLightSource;

void main() {
    vec3 lightColor = vec3(1.0, 0.7, 0.2); // ITT ÁLLÍTHATÓ A FÉNY SZÍNE (NARANCSOS)
    vec3 objectColor = vec3(1.0, 1.0, 1.0); // FEHÉR KOCKÁK

    if(isLightSource) {
        FragColor = vec4(lightColor, 1.0);
        return;
    }

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;
  	
    if(lightOn) {
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        vec3 result = (ambient + diffuse) * objectColor;
        FragColor = vec4(result, 1.0);
    } else {
        FragColor = vec4(ambient * objectColor, 1.0);
    }
}
)";



float cubeVertices[] = {
    // Pozíciók           // Normálisok
    -0.5f,-0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  0.5f,-0.5f,-0.5f,  0.0f, 0.0f,-1.0f,  0.5f, 0.5f,-0.5f,  0.0f, 0.0f,-1.0f,
     0.5f, 0.5f,-0.5f,  0.0f, 0.0f,-1.0f, -0.5f, 0.5f,-0.5f,  0.0f, 0.0f,-1.0f, -0.5f,-0.5f,-0.5f,  0.0f, 0.0f,-1.0f,
    -0.5f,-0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.5f,-0.5f, 0.5f,  0.0f, 0.0f, 1.0f,  0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
     0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f, -0.5f, 0.5f, 0.5f,  0.0f, 0.0f, 1.0f, -0.5f,-0.5f, 0.5f,  0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f,-0.5f, -1.0f, 0.0f, 0.0f, -0.5f,-0.5f,-0.5f, -1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f,-0.5f, -1.0f, 0.0f, 0.0f, -0.5f,-0.5f, 0.5f, -1.0f, 0.0f, 0.0f, -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
     0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.5f, 0.5f,-0.5f,  1.0f, 0.0f, 0.0f,  0.5f,-0.5f,-0.5f,  1.0f, 0.0f, 0.0f,
     0.5f,-0.5f,-0.5f,  1.0f, 0.0f, 0.0f,  0.5f,-0.5f, 0.5f,  1.0f, 0.0f, 0.0f,  0.5f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f,
    -0.5f,-0.5f,-0.5f,  0.0f,-1.0f, 0.0f,  0.5f,-0.5f,-0.5f,  0.0f,-1.0f, 0.0f,  0.5f,-0.5f, 0.5f,  0.0f,-1.0f, 0.0f,
     0.5f,-0.5f, 0.5f,  0.0f,-1.0f, 0.0f, -0.5f,-0.5f, 0.5f,  0.0f,-1.0f, 0.0f, -0.5f,-0.5f,-0.5f,  0.0f,-1.0f, 0.0f,
    -0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f,  0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f,
     0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f, -0.5f, 0.5f, 0.5f,  0.0f, 1.0f, 0.0f, -0.5f, 0.5f,-0.5f,  0.0f, 1.0f, 0.0f
};

// Szilárd gömb generálása indexekkel
void generateSolidSphere(float radius, int segments, vector<float>& vertices, vector<unsigned int>& indices) {
    for (int y = 0; y <= segments; ++y) {
        for (int x = 0; x <= segments; ++x) {
            float xSegment = (float)x / (float)segments;
            float ySegment = (float)y / (float)segments;
            float xPos = radius * cos(xSegment * 2.0f * glm::pi<float>()) * sin(ySegment * glm::pi<float>());
            float yPos = radius * cos(ySegment * glm::pi<float>());
            float zPos = radius * sin(xSegment * 2.0f * glm::pi<float>()) * sin(ySegment * glm::pi<float>());
            vertices.push_back(xPos); vertices.push_back(yPos); vertices.push_back(zPos);
            vertices.push_back(xPos / radius); vertices.push_back(yPos / radius); vertices.push_back(zPos / radius);
        }
    }
    for (int y = 0; y < segments; ++y) {
        for (int x = 0; x < segments; ++x) {
            indices.push_back((y + 1) * (segments + 1) + x);
            indices.push_back(y * (segments + 1) + x);
            indices.push_back(y * (segments + 1) + (x + 1));
            indices.push_back((y + 1) * (segments + 1) + x);
            indices.push_back(y * (segments + 1) + (x + 1));
            indices.push_back((y + 1) * (segments + 1) + (x + 1));
        }
    }
}



int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Solid Sphere & Diffuse Light", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    glewInit();
    glEnable(GL_DEPTH_TEST);

    // Shader program összeállítása
    GLuint shaderProgram = glCreateProgram();
    GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &vertexShaderSource, NULL); glCompileShader(vShader);
    GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &fragmentShaderSource, NULL); glCompileShader(fShader);
    glAttachShader(shaderProgram, vShader); glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);

    // Kocka inicializálás
    GLuint cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO); glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);

    
    vector<float> sVerts; vector<unsigned int> sInds;
    generateSolidSphere(0.25f, 24, sVerts, sInds);
    GLuint sVAO, sVBO, sEBO;
    glGenVertexArrays(1, &sVAO); glGenBuffers(1, &sVBO); glGenBuffers(1, &sEBO);
    glBindVertexArray(sVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sVBO); glBufferData(GL_ARRAY_BUFFER, sVerts.size() * sizeof(float), sVerts.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sEBO); glBufferData(GL_ELEMENT_ARRAY_BUFFER, sInds.size() * sizeof(unsigned int), sInds.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window)) {
        
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) angleCamera += 0.002f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) angleCamera -= 0.002f;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) cameraHeight += 0.002f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) cameraHeight -= 0.002f;
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS && !lKeyWasPressed) { lightOn = !lightOn; lKeyWasPressed = true; }
        if (glfwGetKey(window, GLFW_KEY_L) == GLFW_RELEASE) lKeyWasPressed = false;

        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        
        mat4 view = lookAt(vec3(radiusCamera * cos(angleCamera), radiusCamera * sin(angleCamera), cameraHeight), vec3(0, 0, 0), vec3(0, 0, 1));
        mat4 projection = perspective(radians(45.0f), (float)WIDTH / HEIGHT, 0.1f, 100.0f);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));

      
        float t = (float)glfwGetTime();
        vec3 lightPos = vec3(sin(t * 1.2f) * 5.0f, cos(t * 1.2f) * 5.0f, 0.0f);
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, value_ptr(lightPos));
        glUniform1i(glGetUniformLocation(shaderProgram, "lightOn"), lightOn);

        // KOCKÁK RAJZOLÁSA
        glUniform1i(glGetUniformLocation(shaderProgram, "isLightSource"), false);
        glBindVertexArray(cubeVAO);
        for (int i = -1; i <= 1; i++) {
            mat4 model = translate(mat4(1.0f), vec3(0.0f, 0.0f, i * 2.5f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(model));
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // FÉNY-GÖMB RAJZOLÁSA
        if (lightOn) {
            glUniform1i(glGetUniformLocation(shaderProgram, "isLightSource"), true);
            mat4 sModel = translate(mat4(1.0f), lightPos);
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, value_ptr(sModel));
            glBindVertexArray(sVAO);
            glDrawElements(GL_TRIANGLES, (GLsizei)sInds.size(), GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}