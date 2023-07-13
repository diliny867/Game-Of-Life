#include <glad/glad.h>//should always be first
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>//OpenGL Mathematics
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/useful_includes.hpp>

#include <iostream>
#include <functional>

#include "../include/GL/Time.h"
#include "../include/GL/VBO.h"
#include "../include/GL/VAO.h"
#include "../include/GL/Shader.h"

#include "GameOfLifeView.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window,int key,int scancode,int action,int mods);
void processInput(GLFWwindow* window);
void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
std::function<void()> shaders_callback;
void printFPS();

float SCR_WIDTH = 800;
float SCR_HEIGHT = 600;

class Mouse {
public:
    Mouse(const float x,const float y):pos({x,y}){}
    Mouse():Mouse(0,0){}
    glm::vec2 pos;
    bool leftPressed = false;
    bool rightPressed = false;
    bool wheelScrolled = false;
    bool firstInput = true;
};
Mouse mouse(SCR_WIDTH/2.0f,SCR_HEIGHT/2.0f);

GameOfLifeView gameOfLifeView;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);//opengl versions
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//set to core profile

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL application", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window,key_callback);
    glfwSetCursorPosCallback(window, mouse_cursor_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, mouse_scroll_callback);
    glfwSwapInterval(0);//VSync
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);//capture mouse
    //glfwWindowHint(GLFW_SAMPLES,4); //enable anti aliasing

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    //glEnable(GL_DEPTH_TEST);
    //glDepthFunc(GL_LESS);
	glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT,GL_NICEST);
    //glEnable(GL_MULTISAMPLE);

    Time::Init();

    Shader cellShader("resources/shaders/cellShader_vs.glsl","resources/shaders/cellShader_fs.glsl");
    //Shader canvasShader("resources/shaders/canvasShader_vs.glsl","resources/shaders/canvasShader_fs.glsl");
    Shader canvasLineShader("resources/shaders/canvasLineShader_vs.glsl","resources/shaders/canvasLineShader_fs.glsl");

    glm::mat4 projection = glm::ortho(0.0f,SCR_WIDTH,SCR_HEIGHT,0.0f,-1000.0f,1000.0f);
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 model = glm::mat4(1.0f);


    cellShader.use();
    cellShader.setMat4("projection",projection);
    cellShader.setMat4("view",view);
    cellShader.setMat4("model",model);
    canvasLineShader.use();
    canvasLineShader.setMat4("projection",projection);
    canvasLineShader.setMat4("view",view);
    canvasLineShader.setMat4("model",model);

    shaders_callback = [&]() {
	    projection = glm::ortho(0.0f,SCR_WIDTH,SCR_HEIGHT,0.0f,-1000.0f,1000.0f);

        view = glm::mat4(1.0f);
        view = glm::translate(view,glm::vec3(SCR_WIDTH/2,SCR_HEIGHT/2,0.0f));
        view = glm::scale(view,glm::vec3(gameOfLifeView.GetZoomFactor(),gameOfLifeView.GetZoomFactor(),1.0f));
        view = glm::translate(view,-glm::vec3(SCR_WIDTH/2,SCR_HEIGHT/2,0.0f));

        cellShader.use();
        cellShader.setMat4("projection",projection);
        cellShader.setMat4("view",view);
        cellShader.setMat4("model",model);
        canvasLineShader.use();
        canvasLineShader.setMat4("projection",projection);
        canvasLineShader.setMat4("view",view);
        canvasLineShader.setMat4("model",model);
    };

	gameOfLifeView.offset -= glm::vec2(-SCR_WIDTH/2,-SCR_HEIGHT/2); //center everything
    gameOfLifeView.Init(SCR_WIDTH,SCR_HEIGHT);
    std::vector<GameOfLife::Cell> startingCells = {{0,0},{1,1},{2,1},{2,0},{2,-1}};
    gameOfLifeView.SetCells(startingCells);

    std::cout<<"CONTROLS:\nHold and drag RIGHT MOUSE BUTTON to move canvas\nPress or drag LEFT MOUSE BUTTON to toggle a cell\nPress SPACE to pause\nPress UP to speed up ticks\nPress DOWN to speed down ticks\n";

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT /*| GL_STENCIL_BUFFER_BIT*/);

        Time::Update();
        processInput(window);

        gameOfLifeView.Draw(cellShader,canvasLineShader);
        //gameOfLifeView.Draw(cellShader,canvasShader);
        gameOfLifeView.Update();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void printFPS() {
    std::cout << 1/Time::deltaTime << std::endl;
}

bool changingAliveCells = false;
std::unordered_set<GameOfLife::Cell,GameOfLife::Cell::Hash,GameOfLife::Cell::Equal> changedCells;
void processInput(GLFWwindow* window) {
    if(mouse.leftPressed) {

	    const glm::vec2 screenSize = glm::vec2(SCR_WIDTH,SCR_HEIGHT);
	    const glm::vec2 zoomOffset = glm::vec2(glm::vec2(SCR_WIDTH,SCR_HEIGHT)/gameOfLifeView.GetZoomFactor()-screenSize)/2.0f;
        glm::ivec2 cellPos = glm::floor((-gameOfLifeView.offset-zoomOffset)/gameOfLifeView.GetCellSize()+mouse.pos/gameOfLifeView.GetCellSize()/gameOfLifeView.GetZoomFactor());
        GameOfLife::Cell cell = GameOfLife::Cell(cellPos.x,cellPos.y);

        if(changedCells.count(cell)==0){
            if(changingAliveCells == gameOfLifeView.ContainsCell(cell)) {
                gameOfLifeView.ToggleCell(cell);
            }
        }
        changedCells.insert(cell);
    }else {
        changedCells.clear();
    }
}

void key_callback(GLFWwindow* window,int key,int scancode,int action,int mods) {
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window,true);
    }
    if(key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        gameOfLifeView.Pause(!gameOfLifeView.IsPaused());
    }
    if(key == GLFW_KEY_UP && action == GLFW_PRESS) {
        gameOfLifeView.SpeedUpTicks();
    }
    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
        gameOfLifeView.SpeedDownTicks();
    }
    if(key == GLFW_KEY_EQUAL && action == GLFW_PRESS) {
        gameOfLifeView.SetZoomFactor(1.0f);
        shaders_callback();
    }
}

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos) {
    float xposIn = static_cast<float>(xpos);
    float yposIn = static_cast<float>(ypos);

    if (mouse.firstInput) {
        mouse.pos.x = xposIn;
        mouse.pos.y = yposIn;
        mouse.firstInput = false;
    }

    glm::vec2 mouseDelta = glm::vec2(xposIn,yposIn)-mouse.pos;

    if(mouse.rightPressed){
        gameOfLifeView.offset+= mouseDelta/gameOfLifeView.GetZoomFactor();
    }

    mouse.pos.x = xposIn;
    mouse.pos.y = yposIn;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouse.leftPressed = true;

        glm::ivec2 cellPos = glm::floor((mouse.pos-gameOfLifeView.offset)/gameOfLifeView.GetCellSize()/gameOfLifeView.GetZoomFactor());
        GameOfLife::Cell cell = GameOfLife::Cell(cellPos.x,cellPos.y);
        changingAliveCells = gameOfLifeView.ContainsCell(cell);
    }
    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouse.leftPressed = false;
    }
	if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        mouse.rightPressed = true;
    }
    if(button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE) {
        mouse.rightPressed = false;
    }
}

void mouse_scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	gameOfLifeView.SetZoomFactor(gameOfLifeView.GetZoomFactor()+yoffset/25.0f);
	shaders_callback();
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height) {
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    gameOfLifeView.SetViewpointSize(SCR_WIDTH,SCR_HEIGHT);
    glViewport(0,0,width,height); //0,0 - left bottom
    shaders_callback();
}