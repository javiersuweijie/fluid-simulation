//
//  shaderfile.cpp
//  shader_test
//
//  Created by Javiersu on 6/4/15.
//  Copyright (c) 2015 Javiersu. All rights reserved.
//

#include "shaderfile.h"
using namespace std;

GLuint loadProgram(char* vert_filename, char* frag_filename)
{
  const char* vertex_shader = textFileRead(vert_filename);
  const char* fragment_shader = textFileRead(frag_filename);
  GLuint program;
  
  GLuint vs = glCreateShader (GL_VERTEX_SHADER);
  glShaderSource (vs, 1, &vertex_shader, NULL);
  glCompileShader (vs);
  GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
  glShaderSource (fs, 1, &fragment_shader, NULL);
  glCompileShader (fs);
  program = glCreateProgram ();
  
  glAttachShader (program, fs);
  glAttachShader (program, vs);
  glLinkProgram (program);
//  uniform_resolution = glGetUniformLocation(program,"u_resolution");
//  uniform_time = glGetUniformLocation(program,"u_time");
  GLint result;
  glGetProgramiv(program, GL_LINK_STATUS, &result);
  if(result == GL_FALSE) {
    GLint length;
    char *log;
    
    /* get the program info log */
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    log = (char*)malloc(length);
    glGetProgramInfoLog(program, length, &result, log);
    
    /* print an error message and the info log */
    fprintf(stderr, "sceneInit(): Program linking failed: %s\n", log);
    free(log);
    
    /* delete the program */
    glDeleteProgram(program);
    program = 0;
  }
  return program;
}

GLuint getUniformLocation(GLuint program, char* argument) {
  return glGetUniformLocation(program, argument);
}