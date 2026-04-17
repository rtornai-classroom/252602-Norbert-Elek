#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cmath>
#include <iostream>

// Ablak méretei
const int WIDTH = 800;
const int HEIGHT = 600;

struct Point {
    float x, y;
};


std::vector<Point> controlPoints;
int draggedPointIdx = -1;
const float POINT_RADIUS = 0.04f;




long long binomial(int n, int k) {
    if (k < 0 || k > n) return 0;
    if (k == 0 || k == n) return 1;
    if (k > n / 2) k = n - k;
    long long res = 1;
    for (int i = 1; i <= k; ++i) res = res * (n - i + 1) / i;
    return res;
}


Point calculateBezier(float t) {
    Point p = { 0, 0 };
    int n = (int)controlPoints.size() - 1;
    for (int i = 0; i <= n; i++) {
        float b = (float)binomial(n, i) * powf(1 - t, (float)(n - i)) * powf(t, (float)i);
        p.x += b * controlPoints[i].x;
        p.y += b * controlPoints[i].y;
    }
    return p;
}



void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    // Pixel koordináták átváltása NDC-be (-1 és 1 közé)
    float ndcX = (float)(xpos / WIDTH) * 2.0f - 1.0f;
    float ndcY = 1.0f - (float)(ypos / HEIGHT) * 2.0f; // Y tengely tükrözése
    Point mouseP = { ndcX, ndcY };

    // A kattintási távolság (küszöbérték) is sokkal kisebb kell legyen!
    float threshold = 0.05f; 

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        for (int i = 0; i < controlPoints.size(); i++) {
            if (hypotf(controlPoints[i].x - mouseP.x, controlPoints[i].y - mouseP.y) < threshold) {
                draggedPointIdx = i;
                return;
            }
        }
        controlPoints.push_back(mouseP);
    }
    
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    // Kilépés ESC gombra
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (draggedPointIdx != -1) {
        controlPoints[draggedPointIdx] = { (float)xpos, (float)ypos };
    }
}



void drawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_TRIANGLE_FAN);
    for (int i = 0; i <= num_segments; i++) {
        float theta = 2.0f * 3.1415926f * float(i) / float(num_segments);
        glVertex2f(cx + r * cosf(theta), cy + r * sinf(theta));
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    if (controlPoints.empty()) return;

    // 1. Kontrollpoligon rajzolása (Szürke)
    glColor3f(0.5f, 0.8f, 0.5f);
    glBegin(GL_LINE_STRIP);
    for (const auto& p : controlPoints) glVertex2f(p.x, p.y);
    glEnd();

    // 2. Bézier-görbe rajzolása (Sárga) - Csak ha van legalább 2 pont
    if (controlPoints.size() >= 2) {
        glColor3f(1.0f, 1.0f, 0.0f);
        glBegin(GL_LINE_STRIP);
        int segments = 100;
        for (int i = 0; i <= segments; i++) {
            Point bp = calculateBezier((float)i / segments);
            glVertex2f(bp.x, bp.y);
        }
        glEnd();
    }

    // 3. Kontrollpontok rajzolása (Piros körök)
    glColor3f(1.0f, 0.2f, 0.2f);
    for (const auto& p : controlPoints) {
        drawCircle(p.x, p.y, POINT_RADIUS / 2.0f, 20);
    }
}

int main() {
    if (!glfwInit()) return -1;


    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Grafika Beadando2 - Final", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glewInit();


    controlPoints = { {-0.5f, -0.5f}, {-0.2f, 0.5f}, {0.2f, 0.5f}, {0.5f, -0.5f} };
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);


    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetKeyCallback(window, keyCallback);

    while (!glfwWindowShouldClose(window)) {
        display();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}