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

#define index(i,j) ((i)+texture_size*(j))

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

int texture_size = 128;
int width = 768, height = 768;
int movement = 0;
mouse mouse;
vector<vec2> points = {};
GLuint dd, d0, d, d_fbo_d, d_fbo_d0, d_fbo_dd;
GLuint diffuse_shader, display_shader, add_shader;

GLuint vao;

GLubyte* empty, *data;

void update_with_input() {
  vec2 point;
  point.x = (mouse.x/(float)width)*2-1;
  point.y = ((height-mouse.y)/(float)height)*2-1;
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

void move_array() {
  int i = texture_size/2;
  for (int k=0;k<20;k++) {
    for (int j=0;j<20;j++) {
      data[index(i+k+movement,i+j)] = 255;
    }
  }
}

void keyboard_f(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case '\r':
      move_array();
      movement++;
      glutPostRedisplay();
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
//  glutPostRedisplay();
}

void update_diffuse() {
  glUseProgram(diffuse_shader);
  glUniform1i(glGetUniformLocation(diffuse_shader, "Texture0"),0);
  glUniform1i(glGetUniformLocation(diffuse_shader, "Texture1"),1);
  glUniform1f(glGetUniformLocation(diffuse_shader, "pixelSize"), 1.0/texture_size);
  glUniform1f(glGetUniformLocation(diffuse_shader, "textureSize"), (float)texture_size);
  
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_dd);
  glViewport(0,0, texture_size, texture_size);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, d0);
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, d);
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  glBindVertexArrayAPPLE(vao);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
  
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_d);
  glViewport(0,0, texture_size, texture_size);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, d0);
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, dd);
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  glBindVertexArrayAPPLE(vao);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);

}

void swap_texture(GLuint framebuffer, GLuint texture) {
  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
  glViewport(0,0, texture_size, texture_size);
  
  glUseProgram(display_shader);
  glUniform1i(glGetUniformLocation(display_shader, "Texture0") ,0);
  glUniform1f(glGetUniformLocation(diffuse_shader, "pixelSize"), 1.0/texture_size);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  glBindVertexArrayAPPLE(vao);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
  glBindTexture(GL_FRAMEBUFFER, 0);
}

void swap_texture_2(GLuint *A, GLuint *B) {
  GLuint *temp = A;
  A = B;
  B = temp;
}

void add_source() {
  glBindTexture(GL_TEXTURE_2D, d0);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture_size, texture_size, GL_RED, GL_UNSIGNED_BYTE, data);
  
  glUseProgram(add_shader);
  glUniform1i(glGetUniformLocation(add_shader, "Texture0"),0);
  glUniform1i(glGetUniformLocation(add_shader, "Texture1"),1);
  
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_dd);
  glViewport(0,0, texture_size, texture_size);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, d0);
  
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, d);
  
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  glBindVertexArrayAPPLE(vao);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
}

void set_boundary() {
}

void draw_texture(GLuint texture) {
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear ( GL_COLOR_BUFFER_BIT );
  glViewport(0,0, width, height);
  
  glUseProgram(display_shader);
  glUniform1i(glGetUniformLocation(display_shader, "Texture0") ,0);
  glUniform1f(glGetUniformLocation(diffuse_shader, "pixelSize"), 1.0/texture_size);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  
  glBindVertexArrayAPPLE(vao);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
  
  glutSwapBuffers();
}

void clear_texture(GLuint texture) {
//  glBindTexture(GL_TEXTURE_2D, texture);
//  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture_size, texture_size, GL_RED, GL_UNSIGNED_BYTE, empty);
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_d);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void draw_f(void) {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  if (mouse.left_click) {
    add_source();
    swap_texture(d_fbo_d0,dd);
  }
  clear_texture(d);
  clear_texture(dd);

  for (int i = 0; i<12; i++) {
    update_diffuse();
  }
  swap_texture(d_fbo_d0,d);
  draw_texture(d0);
}

/*
 ----------------------------------------------------------------------
 Texture / Framebuffer / Shader initiation
 ----------------------------------------------------------------------
 */

void init_textures() {
  // Create density textures
  
  data = (GLubyte *) malloc(texture_size*texture_size*sizeof(GLubyte));
  int i = texture_size/2;
  for (int k=0;k<20;k++) {
    for (int j=0;j<20;j++) {
      data[index(i+k+movement,i+j)] = 255;
    }
  }

  glGenTextures(1, &d0);
  glBindTexture(GL_TEXTURE_2D, d0);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RED, texture_size, texture_size, 0, GL_RED, GL_UNSIGNED_BYTE, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  empty = (GLubyte *) malloc(texture_size*texture_size*sizeof(GLubyte));
  glGenTextures(1, &d);
  glBindTexture(GL_TEXTURE_2D, d);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RED, texture_size, texture_size, 0, GL_RED, GL_UNSIGNED_BYTE, empty);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glGenTextures(1, &dd);
  glBindTexture(GL_TEXTURE_2D, dd);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RED, texture_size, texture_size, 0, GL_RED, GL_UNSIGNED_BYTE, empty);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
}

void init_framebuffer() {
  // Create density framebuffers;
  glGenFramebuffers(1, &d_fbo_d);
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_d);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d, 0);
  
  glGenFramebuffers(1, &d_fbo_d0);
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_d0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d0, 0);
  
  glGenFramebuffers(1, &d_fbo_dd);
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_dd);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dd, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void init_shaders() {
  diffuse_shader = loadProgram("shaders/basic.vert", "shaders/diffuse.frag");
  display_shader = loadProgram("shaders/basic.vert", "shaders/basic.frag");
  add_shader     = loadProgram("shaders/basic.vert", "shaders/add.frag");
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
