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
  mouse.y = height - y;
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

void draw_f(void) {
  glViewport(0, 0, width, height);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glutSwapBuffers();
}

/*
 ----------------------------------------------------------------------
 Texture & Framebuffer initiation
 ----------------------------------------------------------------------
 */

void init_textures() {
  // Create density textures
  glGenTextures(1, &d0);
  glBindTexture(GL_TEXTURE_2D, d0);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glGenTextures(1, &d);
  glBindTexture(GL_TEXTURE_2D, d);
  glTexImage2D(GL_TEXTURE_2D, 0,GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glBindTexture(GL_TEXTURE_2D, 0);
}

void init_framebuffer() {
  // Create density framebuffers;
  glGenFramebuffers(1, &d_fbo_A);
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_A);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,d, 0);
  
  glGenFramebuffers(1, &d_fbo_B);
  glBindFramebuffer(GL_FRAMEBUFFER, d_fbo_B);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,d0, 0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
//  initShaders();
//  initRendering();
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
