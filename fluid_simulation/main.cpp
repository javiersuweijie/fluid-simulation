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
  int x,y;
};

int texture_size = 512;
int width = 768, height = 768;
int movement = 0;
mouse mouse;

GLuint dd, d0, d, fbo_d, fbo_d0, fbo_dd;
GLuint vv, v0, v, fbo_v, fbo_v0, fbo_vv;
GLuint diffuse_shader, display_shader, add_shader, advect_shader, project1_shader, project2_shader;

GLuint vao;

GLfloat* empty, *data, *data2d;


void move_array(int x, int y) {
  
  for (int i=0; i<texture_size; i++) {
    for (int j=0; j<texture_size; j++) {
      data[index(i,j)] = 0.0;
    }
  }
  
  for (int k=0;k<20;k++) {
    for (int j=0;j<20;j++) {
      data[index(x-10+k,y-10+j)] = 10.0;
    }
  }
}

void update_with_input() {
  vec2 point;
  point.x = mouse.x;
  point.y = (height-mouse.y);
  if (mouse.left_click);
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
    case '\r':
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
  if (button == 0) {
    mouse.left_click = state == GLUT_DOWN;
    move_array(x*texture_size/width, (height-y)*texture_size/height);
  }
}

void motion_f(int x, int y) {
  mouse.x = x;
  mouse.y = y;
}

void idle_f(void) {
  update_with_input();
  glutPostRedisplay();
}

void clear_texture(GLuint texture) {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_d);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);
}

void linear_solver(GLuint x0, GLuint x, GLuint xx, float a, float c, GLuint buffer, GLuint save_to) {
  glUseProgram(diffuse_shader);
  glUniform1i(glGetUniformLocation(diffuse_shader, "Texture0"),0);
  glUniform1i(glGetUniformLocation(diffuse_shader, "Texture1"),1);
  glUniform1f(glGetUniformLocation(diffuse_shader, "pixelSize"), 1.0/texture_size);
  glUniform1f(glGetUniformLocation(diffuse_shader, "textureSize"), (float)texture_size);
  glUniform1f(glGetUniformLocation(diffuse_shader, "a"), a);
  glUniform1f(glGetUniformLocation(diffuse_shader, "c"), c);
  
  for (int i=0;i<12;i++) {
    glBindFramebuffer(GL_FRAMEBUFFER, buffer);
    glViewport(0,0, texture_size, texture_size);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, x0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, x);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glBindVertexArrayAPPLE(vao);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
    
    glBindFramebuffer(GL_FRAMEBUFFER, save_to);
    glViewport(0,0, texture_size, texture_size);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, x0);
    
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, xx);
    
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    glBindVertexArrayAPPLE(vao);
    glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
  }
}

void diffuse() {
  
  float a = texture_size * texture_size * 0.000004;
  float c = 1+(4.0*a);
  linear_solver(d0, d, dd, a, c, fbo_dd, fbo_d);

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

void add_source() {
  glBindTexture(GL_TEXTURE_2D, d0);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, texture_size, texture_size, GL_RED, GL_FLOAT, data);
  
  glUseProgram(add_shader);
  glUniform1i(glGetUniformLocation(add_shader, "Texture0"),0);
  glUniform1i(glGetUniformLocation(add_shader, "Texture1"),1);
  
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_dd);
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
  
}

void advect(GLuint density, GLuint velocity, GLuint save_to) {
  
  glBindFramebuffer(GL_FRAMEBUFFER, save_to);
  glUseProgram(advect_shader);
  glUniform1i(glGetUniformLocation(advect_shader, "Texture0"),0);
  glUniform1i(glGetUniformLocation(advect_shader, "Texture1"),1);
  glUniform1f(glGetUniformLocation(advect_shader, "pixelSize"), 1.0/texture_size);
  glViewport(0, 0, texture_size, texture_size);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, density);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, velocity);
  glBindVertexArrayAPPLE(vao);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
}

void project(GLuint v0, GLuint v, GLuint vv, GLuint p, GLuint div, GLuint buffer, GLuint save_to, GLuint fbo_div, GLuint fbo_p) {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_div);
  glUseProgram(project1_shader);
  glUniform1i(glGetUniformLocation(project1_shader, "Texture0"),0);
  glUniform1f(glGetUniformLocation(advect_shader, "pixelSize"), 1.0/texture_size);
  glViewport(0, 0, texture_size, texture_size);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, v0);
  glBindVertexArrayAPPLE(vao);
  glDrawElements(GL_QUADS, 4, GL_UNSIGNED_INT, NULL);
  
  clear_texture(p);
  linear_solver(div, p, vv, 1.0, 4.0, buffer, fbo_p);
  
  draw_texture(p);
  GLbyte sum[1];
  GLbyte pixel2[1];
  GLbyte pixel3[1];
  for (int i=0; i<texture_size; ++i) {
    for (int j=0; j<texture_size; ++j) {
//      glReadPixels(i, j, 1, 1, GL_RG, GL_UNSIGNED_BYTE, pixel1);
//      glTexSubImage2D(GL_TEXTURE_2D, 0, i, j, 1, 1, GL_RG, GL_UNSIGNED_BYTE, <#const GLvoid *pixels#>)
    }
  }
}

void draw_f(void) {
  glClearColor(0.0, 0.0, 0.0, 1.0);
  if (mouse.left_click) {
    add_source();
    swap_texture(fbo_d0,dd);
  }
  clear_texture(d);
  
  diffuse();
  advect(d, v0, fbo_d0);
//  swap_texture(fbo_d0,d);
  
  draw_texture(d0);
  glutSwapBuffers();
}

/*
 ----------------------------------------------------------------------
 Texture / Framebuffer / Shader initiation
 ----------------------------------------------------------------------
 */

void init_textures() {
  // Create density textures
  
  data = (GLfloat *) malloc(texture_size*texture_size*sizeof(GLfloat));
  int i = texture_size/2;
  for (int k=0;k<20;k++) {
    for (int j=0;j<20;j++) {
      data[index(i+k+movement,i+j)] = 10.0;
    }
  }

  glGenTextures(1, &d0);
  glBindTexture(GL_TEXTURE_2D, d0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texture_size, texture_size, 0, GL_RED, GL_FLOAT, data);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  empty = (GLfloat *) malloc(texture_size*texture_size*sizeof(GLfloat));
  glGenTextures(1, &d);
  glBindTexture(GL_TEXTURE_2D, d);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_R32F, texture_size, texture_size, 0, GL_RED, GL_FLOAT, empty);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glGenTextures(1, &dd);
  glBindTexture(GL_TEXTURE_2D, dd);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_R32F, texture_size, texture_size, 0, GL_RED, GL_FLOAT, empty);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  data2d = (GLfloat *) malloc(texture_size*texture_size*2*sizeof(GLfloat));
  for (int k=0; k<texture_size*texture_size*2; k++) {
    data2d[k] = 0.0;
  }
  glGenTextures(1, &v0);
  glBindTexture(GL_TEXTURE_2D, v0);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RG16F, texture_size, texture_size, 0, GL_RG, GL_FLOAT, data2d);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glGenTextures(1, &v);
  glBindTexture(GL_TEXTURE_2D, v);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RG16F, texture_size, texture_size, 0, GL_RED, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glGenTextures(1, &vv);
  glBindTexture(GL_TEXTURE_2D, vv);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RG16F, texture_size, texture_size, 0, GL_RED, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
}

void init_framebuffer() {
  // Create density framebuffers;
  glGenFramebuffers(1, &fbo_d);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_d);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d, 0);
  
  glGenFramebuffers(1, &fbo_d0);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_d0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d0, 0);
  
  glGenFramebuffers(1, &fbo_dd);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_dd);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dd, 0);
  
  // Create velocity framebuffers;
  glGenFramebuffers(1, &fbo_v);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_v);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d, 0);
  
  glGenFramebuffers(1, &fbo_v0);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_v0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, d0, 0);
  
  glGenFramebuffers(1, &fbo_vv);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_vv);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, dd, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void init_shaders() {
  diffuse_shader  = loadProgram("shaders/basic.vert", "shaders/diffuse.frag");
  display_shader  = loadProgram("shaders/basic.vert", "shaders/basic.frag");
  add_shader      = loadProgram("shaders/basic.vert", "shaders/add.frag");
  advect_shader   = loadProgram("shaders/basic.vert", "shaders/advect.frag");
  project1_shader = loadProgram("shaders/basic.vert", "shaders/project1.frag");
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
