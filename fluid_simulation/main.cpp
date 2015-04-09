//
//  main.cpp
//  fluid_simulation
//
//  Created by Javiersu on 8/4/15.
//  Copyright (c) 2015 Local Food. All rights reserved.
//
#include <OpenGL/OpenGL.h>
#include <GLUT/GLUT.h>
#include <iostream>
#include <vector>

#include "bmp.h"
#include "shaderfile.h"
#include "textfile.h"

using namespace std;

#define index(i,j) ((i)+height*(j))

struct mouse {
  float x;
  float y;
  int x_index;
  int y_index;
  bool left_click;
  bool right_click;
};

struct vec2 {
  float x,y;
};

int width = 512, height = 512;
bool left_mouse_down;
mouse mouse;
vector<vec2> points = {};
GLuint d0, d, d_fbo_A, d_fbo_B;
GLuint diffuse_shader, display_shader;

GLuint vao;

void update_with_input() {
  vec2 point;
  point.x = (mouse.x/(float)width)*2-1;
  point.y = ((height-mouse.y)/(float)height)*2-1;
  if (mouse.left_click) points.push_back(point);
}

/*
 ----------------------------------------------------------------------
 GLUT callback routines
 ----------------------------------------------------------------------
 */

void reshape_f(int w, int h) {
//  if (w > h) {
//    glViewport((w - h) / 2, 0, h, h);
//  } else {
//    glViewport(0, (h - w) / 2, w, w);
//  }
  width = w;
  height = h;
}

void keyboard_f(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
      
    default:
      cout << "Unhandled key press " << key << "." << endl;
  }
}

void mouse_f(int button, int state, int x, int y) {
  mouse.x = x;
  mouse.y = height - y; // bottom left is 0,0
//  cout << "Draw point:" << x << " " << height - y << endl;
  if (button == 0) mouse.left_click = state == GLUT_DOWN;
}

void motion_f(int x, int y) {
  mouse.x = x;
  mouse.y = y;
}

void idle_f(void) {
  update_with_input();
  glutPostRedisplay();
}

void update_diffuse() {
  glUseProgram(diffuse_shader);
  glUniform1i(glGetUniformLocation(diffuse_shader, "Texture0"),0);
  glUniform1i(glGetUniformLocation(diffuse_shader, "Texture1"),1);
  glUniform1f(glGetUniformLocation(diffuse_shader, "pixelSize"), 1.0/width);
  
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_A);
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, d0);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, d);
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  glBindVertexArrayAPPLE(vao);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
  glDisable(GL_TEXTURE_2D);
}

void swap_texture() {
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_A);
  glUseProgram(display_shader);
  glUniform1i(glGetUniformLocation(display_shader, "Texture0") ,0);
  
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, d0);
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  glBindVertexArrayAPPLE(vao);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
  glDisable(GL_TEXTURE_2D);
}

void draw_texture() {
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glUseProgram(display_shader);
  glUniform1i(glGetUniformLocation(display_shader, "Texture0") ,0);
  
  glEnable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, d);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  glBindVertexArrayAPPLE(vao);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
  glDisable(GL_TEXTURE_2D);
}


void draw_f(void) {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  swap_texture();
//  for (int i = 0; i<20; i++)
  update_diffuse();
  draw_texture();
  
  glutSwapBuffers();
}

/*
 ----------------------------------------------------------------------
 Texture / Framebuffer / Shader initiation
 ----------------------------------------------------------------------
 */

void init_textures() {
  // Create density textures
  
  GLubyte* data = (GLubyte *) malloc(width*height*sizeof(GLubyte));
  int i = width/2;
  for (int k=0;k<5;k++) {
    data[index(i-k,i-k)] = 255;
    data[index(i-k,i)] = 255;
    data[index(i,i-k)] = 255;
    data[index(i,i)] = 255;
    data[index(i+k,i+k)] = 255;
    data[index(i+k,i)] = 255;
    data[index(i,i+k)] = 255;
  }
  
  glGenTextures(1, &d0);
  glBindTexture(GL_TEXTURE_2D, d0);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  GLubyte* empty = (GLubyte *) malloc(width*height*sizeof(GLubyte));
  glGenTextures(1, &d);
  glBindTexture(GL_TEXTURE_2D, d);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, 512, 512, 0, GL_RED, GL_UNSIGNED_BYTE, empty);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
}

void init_framebuffer() {
  // Create density framebuffers;
  glGenFramebuffers(1, &d_fbo_A);
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_A);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d, 0);
  
  glGenFramebuffers(1, &d_fbo_B);
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_B);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d0, 0);
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) exit(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void init_shaders() {
  diffuse_shader = loadProgram("shaders/basic.vert", "shaders/diffuse.frag");
  display_shader = loadProgram("shaders/basic.vert", "shaders/basic.frag");
}

/*
 ----------------------------------------------------------------------
 Vertex Array Objects
 ----------------------------------------------------------------------
 */

void init_vao() {
  float vertex[] = {
    -1.0f, 1.0f, .0f,
    -1.0f, -1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
    1.0f, 1.0f, 0.0f
  };
  float texture[] = {
    0.0, 1.0,
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0
  };
  
  unsigned int index[] = { 0, 1, 2, 3 };
  glGenVertexArraysAPPLE(1, &vao);
  glBindVertexArrayAPPLE(vao);
  
  GLuint buffer;
  glGenBuffers(1, &buffer);
  
  GLuint vertexLoc = glGetAttribLocation(diffuse_shader,"position");
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);
  glEnableVertexAttribArray(vertexLoc );
  glVertexAttribPointer(vertexLoc , 3, GL_FLOAT, 0, 0, 0);
  
  GLuint tex_buffer;
  glGenBuffers(1, &tex_buffer);
  GLuint texLoc = glGetAttribLocation(diffuse_shader,"tex");
  glBindBuffer(GL_ARRAY_BUFFER, tex_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(texture), texture, GL_STATIC_DRAW);
  glEnableVertexAttribArray( texLoc );
  glVertexAttribPointer(texLoc , 2, GL_FLOAT, 0, 0, 0);
  
  GLuint index_buffer;
  glGenBuffers(1, &index_buffer);
  
  // bind buffer for positions and copy data into buffer
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

  glBindVertexArrayAPPLE(0);
}

/*
 ----------------------------------------------------------------------
 GLUT initiation
 ----------------------------------------------------------------------
 */

void init_glut() {
  glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
  glutInitWindowPosition( 60, 60 );
  glutInitWindowSize( width, height );
  glutCreateWindow("Fluid simulation");
  init_textures();
  init_shaders();
  init_framebuffer();
  init_vao();
  glutReshapeFunc(reshape_f);
  glutKeyboardFunc(keyboard_f);
  glutMouseFunc(mouse_f);
  glutMotionFunc(motion_f);
  glutIdleFunc(idle_f);
  glutDisplayFunc(draw_f);
}

int main(int argc, char * argv[]) {
  glutInit(&argc, argv);
  init_glut();
  //  std::cout <<  glGetString(GL_VERSION) << endl;
  glutMainLoop();
}
