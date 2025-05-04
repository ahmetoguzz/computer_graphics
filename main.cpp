#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// kendi yazdığımız fonksiyonlar
#include "shader.h"
#include "sphere.h"
#include "cube.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// kamera 1. açı
glm::vec3 cameraPos = glm::vec3(0.0f, 1.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, -0.1f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

/*
// kamera 2. açı
glm::vec3 cameraPos = glm::vec3(0.0f, 6.0f, 0.1f);
glm::vec3 cameraFront = glm::vec3(0.0f, -1.0f, 0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 0.0f, -1.0f);
*/
/*
// 3. açı -  sahneye çapraz bakan açı
glm::vec3 cameraPos = glm::vec3(4.0f, 3.0f, 6.0f);
glm::vec3 cameraFront = glm::normalize(glm::vec3(-1.5f, -1.0f, -2.0f));
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
*/

// zemin + 3 duvar
float wallVertices[] = {
    // zemin
    -2.0f, 0.0f, -2.0f, 0.0f, 1.0f, 0.0f,
    2.0f, 0.0f, -2.0f, 0.0f, 1.0f, 0.0f,
    2.0f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f,
    -2.0f, 0.0f, 2.0f, 0.0f, 1.0f, 0.0f,

    // sol duvar
    -2.0f, 0.0f, 2.0f, 1.0f, 0.0f, 0.0f,
    -2.0f, 0.0f, -2.0f, 1.0f, 0.0f, 0.0f,
    -2.0f, 2.0f, -2.0f, 1.0f, 0.0f, 0.0f,
    -2.0f, 2.0f, 2.0f, 1.0f, 0.0f, 0.0f,

    // arka duvar
    -2.0f, 0.0f, -2.0f, 0.0f, 0.0f, 1.0f,
    2.0f, 0.0f, -2.0f, 0.0f, 0.0f, 1.0f,
    2.0f, 2.0f, -2.0f, 0.0f, 0.0f, 1.0f,
    -2.0f, 2.0f, -2.0f, 0.0f, 0.0f, 1.0f,

    // sağ duvar
    2.0f, 0.0f, -2.0f, -1.0f, 0.0f, 0.0f,
    2.0f, 0.0f, 2.0f, -1.0f, 0.0f, 0.0f,
    2.0f, 2.0f, 2.0f, -1.0f, 0.0f, 0.0f,
    2.0f, 2.0f, -2.0f, -1.0f, 0.0f, 0.0f};

unsigned int wallIndices[] = {
    0, 1, 2, 2, 3, 0,      // zemin
    4, 5, 6, 6, 7, 4,      // sol
    8, 9, 10, 10, 11, 8,   // arka
    12, 13, 14, 14, 15, 12 // sağ
};

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Let it snow", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "GLFW pencere oluşturulamadı\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glEnable(GL_DEPTH_TEST);

    Shader shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");

    // duvar bağlama işlemleri
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(wallVertices), wallVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(wallIndices), wallIndices, GL_STATIC_DRAW);

    // pozisyon
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
    // normal
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // küre ve küp oluştur
    unsigned int sphereVAO, sphereIndexCount;
    sphereVAO = createSphereVAO(sphereIndexCount);
    // sphere.h'deki fonksiyon
    unsigned int cubeVAO = createCubeVAO();

    shader.use();

    while (!glfwWindowShouldClose(window))
    {
        glClearColor(0.1, 0.1, 0.1, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                                                (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);

        // aydınlatma
        shader.setVec3("lightColor", glm::vec3(1.0, 1.0, 1.0));
        shader.setVec3("lightPos", glm::vec3(1.0, 2.0, 2.0));
        shader.setVec3("viewPos", cameraPos);

        // duvarlar + zemin
        glBindVertexArray(VAO);
        glm::mat4 model = glm::mat4(1.0f);
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, 6 * 4, GL_UNSIGNED_INT, 0);

        // küre
        glBindVertexArray(sphereVAO);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 0.3, 0.0));
        model = glm::scale(model, glm::vec3(0.5f));
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

        // orta küre
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 0.8, 0.0));
        model = glm::scale(model, glm::vec3(0.4f));
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

        // en üstteki küre
        model = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0, 1.2, 0.0));
        model = glm::scale(model, glm::vec3(0.3f));
        shader.setMat4("model", model);
        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, 0);

        // küp
        glBindVertexArray(cubeVAO);
        model = glm::translate(glm::mat4(1.0f), glm::vec3(1.0, 0.5, 0.0));
        shader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
