//
//  shaderfile.h
//  shader_test
//
//  Created by Javiersu on 6/4/15.
//  Copyright (c) 2015 Javiersu. All rights reserved.
//

#ifndef __shader_test__shaderfile__
#define __shader_test__shaderfile__

#include <stdio.h>
#include <stdlib.h>
#include <OpenGL/gl.h>
#include "textfile.h"

GLuint loadProgram(char* vert_filename, char* frag_filename);
#endif /* defined(__shader_test__shaderfile__) */
