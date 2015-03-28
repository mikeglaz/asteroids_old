#include <iostream>
#include <ctime>
#include <GL/glew.h>

#include "asteroid.h"
#include "mgl.h"

using namespace std;

GLFWwindow* window;

// Window dimensions
const GLuint WIDTH = 800, HEIGHT = 800;

const int NUM_ASTEROIDS = 10;
const int NUM_ASTEROID_VERTICES = 8;

vector<GLfloat> Asteroid::asteroid_vertices;
vector<Asteroid> asteroids;

const GLfloat RADIUS_LARGE = 0.1875f;
GLfloat RADIUS_MEDIUM = 0.09375f;
GLfloat RADIUS_SMALL = 0.046875f;

GLuint VBO, VAO;

GLuint vertex_shader, frag_shader, program;

GLfloat now;

void init_glfw()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  // Init GLFW
  glfwInit();

  // Set all the required options for GLFW
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "Asteroids", nullptr, nullptr);
  glfwMakeContextCurrent(window);

  glewExperimental = GL_TRUE;

  glewInit();

  glViewport(0, 0, WIDTH, HEIGHT);
}

void create_asteroids()
{
  Asteroid::generate_asteroid_vertices(NUM_ASTEROID_VERTICES, RADIUS_LARGE);  

  for(int i=0; i != NUM_ASTEROIDS; i++)
  {
    Asteroid asteroid;
    asteroids.push_back(asteroid);
  }
}

void init_opengl()
{
  program = mgl::load_shaders("vertex.glsl", "fragment.glsl");

  create_asteroids();

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO); 
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, Asteroid::asteroid_vertices.size()*sizeof(GLfloat), Asteroid::asteroid_vertices.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
}

void draw()
{
  static GLfloat last = 0;
  GLfloat delta;
  now = glfwGetTime();
  delta = now - last;

  delta /= 10;

  last = now;
  GLint modelLoc = glGetUniformLocation(program, "model");

  glClear(GL_COLOR_BUFFER_BIT);

  glBindVertexArray(VAO);

  for(int i=0; i != asteroids.size(); i++)
  {
    glm::mat4 model;
    glm::vec3 position;

    asteroids[i].move(delta);
    asteroids[i].check_position();
    position = asteroids[i].get_position();   

    model = glm::translate(model, position);
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glDrawArrays(GL_LINE_LOOP, 0, NUM_ASTEROID_VERTICES);    
  }

  glBindVertexArray(0);
}

int main(int argc, char** argv)
{
  srand(static_cast<unsigned>(time(0)));

  // timer /= 100;

  init_glfw();

  init_opengl();


  // for(int i=0; i != NUM_ASTEROID_VERTICES*2; i += 2)
  //   cout << Asteroid::asteroid_vertices[i] << ", " << Asteroid::asteroid_vertices[i+1] << endl;

  while(!glfwWindowShouldClose(window))
  {
    glfwPollEvents();

    glUseProgram(program);

    draw();

    glfwSwapBuffers(window);
  }

  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &VAO);

  glfwTerminate();
}