#include <iostream>
#include <stdlib.h>
#include <math.h>
#define GLFW_DLL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "stb_image.h"
#include "MyShader.h"

// 注意 glad的引用一定要在GLFW之前
using namespace std;

#define JPG 1
#define PNG 2

float mixValue = 0.0f ;

void initializeGLFW();
int initializeGLAD();
void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void makeTriangle(unsigned int VAO);
void makeRectangle(unsigned int VAO);
void processInput(GLFWwindow *window);
void readTex(unsigned int texture, const char* texturePath, unsigned int type);

int main()
{
    // 初始化GLFW
    initializeGLFW();
    // 创建一个窗口，将窗口的上下文应用到当前的主上下文
    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL){
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }   
    glfwMakeContextCurrent(window);
    // 初始化GLAD
    if(!initializeGLAD())
        return -1;

    // 初始化渲染窗口，及视口ViewPort
    glViewport(0, 0, 800, 600);
    // 绑定回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // 创建和编译我的着色器
    MyShader myShader("../src/shader/shader.vs", "../src/shader/shader.fs");

    // 创建一个三角形的Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    makeTriangle(VAO);
    // 创建一个矩形
    unsigned int VAORect;
    glGenVertexArrays(1, &VAORect);
    makeRectangle(VAORect);

    // 创建纹理
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glGenTextures(1, &texture2);
    readTex(texture1, "../src/tex/container.jpg", JPG);
    readTex(texture2, "../src/tex/awesomeface.png", PNG);
    myShader.use(); // 别忘记在激活着色器前先设置uniform！
    glUniform1i(glGetUniformLocation(myShader.ID, "Texture1"), 0); // 手动设置
    myShader.setInt("Texture2", 1); // 或者使用着色器类设置

    // 准备引擎
    // 循环不停的渲染，及渲染循环
    while(!glfwWindowShouldClose(window)){
        // 检查用户的输入
        processInput(window);
        // 检查触发事件
        glfwPollEvents();
        // 清空屏幕
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // 应用着色器程序
        myShader.use();
        // 创建转换矩阵，一般按照缩放，旋转，位移的顺序来来进行变换，一定要想好转换的顺序。
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));
        trans = glm::translate(trans, glm::vec3(0.5, -0.5, 0.0));
        trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));
        // 传递转换矩阵
        glUniformMatrix4fv(glGetUniformLocation(myShader.ID, "transform"), 1, GL_FALSE, glm::value_ptr(trans));
        // 绑定纹理
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        // 监控键盘设置
        myShader.setFloat("mixValue", mixValue);
        // 通过EBO来绘制矩形
        glBindVertexArray(VAORect);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // 应用程序采用着双缓冲模式，一张在前面显示，一张在后面慢慢加载
        // Swap交换缓冲，完成立刻刷新
        glfwSwapBuffers(window);
    }

    // 释放glfw的资源
    glfwTerminate();

    return 0;
}

void initializeGLFW(){
    // 初始GLFW，设置版本号为3.3，使用核心模式
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

int initializeGLAD(){
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return 0;
    }
    return 1;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewport(0, 0, width, height);
}

void makeTriangle(unsigned int VAO){
    // 绑定VAO来存储下面的顶点解释和可见性设置
    glBindVertexArray(VAO);

    // 生成VBO对象（顶点缓冲对象）
    float vertices[]{
         // 位置              // 颜色
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   // 右下
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
         0.0f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f    // 顶部
    };
    unsigned int VBO;
    // 1是需要创建的缓存数量，&VBO是存储申请的缓存ID的地址
    glGenBuffers(1, &VBO);
    // 将VBO对应的缓冲对象绑定到GL_ARRAY_BUFFER目标上
    // 从这一条语句执行结束后，所有对GL_ARRAY_BUFFER的调用
    // 都会用于配置刚刚绑定的缓冲对象
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // 向GL_ARRAY_BUFFER拷贝数据
    // 第四个参数有三种，表示数据的类型
    // GL_STATIC_DRAW: 数据基本不会改变
    // GL_DYNAMIC_DRAW：数据会被改变很多（可能被保存到高速缓冲区）
    // GL_STREAM_DRAW：数据每次绘制都会改变（可能被保存到高速缓冲区）
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 设置如何解释定点数据
    // 第一个参数0和VertexShader中的layout(location=0)对应
    // 第二个参数代表一个顶点有几个属性，vec3
    // 第三个参数代表属性的类型
    // 第四个参数代表数据是否标准化（映射到0，1）
    // 第五个参数代表每个顶点之间的距离，步长stide
    // 第六个参数代表在缓冲中起始位置的偏移量
    // 该函数规定从最近与GL_ARRAY_BUFFER绑定VBO中获取数据
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
    // 设置VAO的第0个数据组可用
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    // 解绑
    glBindVertexArray(0);
}

void makeRectangle(unsigned int VAO){
    // 绑定VAO
    glBindVertexArray(VAO);
    // 绑定VBO
    float vertices[] = {
    //     ---- 位置 ----       ---- 颜色 ----     - 纹理坐标 -
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   2.0f, 2.0f,   // 右上
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   2.0f, 0.0f,   // 右下
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // 左下
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 2.0f    // 左上
    };
    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 绑定EBO
    unsigned int indices[] = { // 注意索引从0开始
        0, 1, 3,  // 第一个三角形
        1, 2, 3   // 第二个三角形
    };
    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // 设置定点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    // 解绑
    glBindVertexArray(0);
}

void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
        mixValue += 0.01f;
        if(mixValue > 1.0f)
            mixValue = 1.0f;
    }
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
        mixValue -= 0.01f;
        if(mixValue < 0.0f)
            mixValue = 0.0f;
    }
}

void readTex(unsigned int texture, const char* texturePath, unsigned int type){
    glBindTexture(GL_TEXTURE_2D, texture);
    // 为当前绑定的纹理对象设置环绕、过滤方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // 加载并生成纹理
    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);
    if (data){
        switch(type){
            case JPG:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
                break;
            case PNG:
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        }
        glGenerateMipmap(GL_TEXTURE_2D);
    }else{
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
