#ifndef GL_CORE_H
#define GL_CORE_H

#include <SDL2/SDL.h>
#include <stddef.h>

#define GL_VERTEX_SHADER   0x8B31
#define GL_FRAGMENT_SHADER 0x8B20
#define GL_COMPILE_STATUS  0x8B81
#define GL_LINK_STATUS     0x8B82
#define GL_ARRAY_BUFFER    0x8892
#define GL_STATIC_DRAW     0x88E4
#define GL_TEXTURE_2D      0x0DE1
#define GL_TEXTURE0        0x84C0
#define GL_RED             0x1903
#define GL_R8              0x8229
#define GL_UNSIGNED_BYTE   0x1401
#define GL_NEAREST         0x2600
#define GL_CLAMP_TO_EDGE   0x812F
#define GL_TEXTURE_MIN_FILTER 0x2801
#define GL_TEXTURE_MAG_FILTER 0x2800
#define GL_TEXTURE_WRAP_S  0x2802
#define GL_TEXTURE_WRAP_T  0x2803
#define GL_TRIANGLES       0x0004
#define GL_COLOR_BUFFER_BIT 0x0000
#define GL_FLOAT           0x1406
#define GL_FALSE           0

typedef int GLsizei;
typedef ptrdiff_t GLsizeiptr;
typedef unsigned int GLuint;
typedef int GLint;
typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef float GLfloat;
typedef ptrdiff_t GLintptr;
typedef unsigned int GLbitfield;

int gl_core_load(void);

extern void (*glDeleteShader)(GLuint);
extern void (*glDeleteProgram)(GLuint);
extern void (*glDeleteBuffers)(GLsizei, const GLuint*);
extern void (*glDeleteVertexArrays)(GLsizei, const GLuint*);
extern void (*glDeleteTextures)(GLsizei, const GLuint*);
extern GLuint (*glCreateShader)(GLenum);
extern void (*glShaderSource)(GLuint, GLsizei, const char* const*, const GLint*);
extern void (*glCompileShader)(GLuint);
extern void (*glGetShaderiv)(GLuint, GLenum, GLint*);
extern void (*glGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, char*);
extern GLuint (*glCreateProgram)(void);
extern void (*glAttachShader)(GLuint, GLuint);
extern void (*glLinkProgram)(GLuint);
extern void (*glGetProgramiv)(GLuint, GLenum, GLint*);
extern void (*glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, char*);
extern void (*glUseProgram)(GLuint);
extern GLint (*glGetUniformLocation)(GLuint, const char*);
extern void (*glUniform1f)(GLint, GLfloat);
extern void (*glUniform2f)(GLint, GLfloat, GLfloat);
extern void (*glUniform3f)(GLint, GLfloat, GLfloat, GLfloat);
extern void (*glUniform1i)(GLint, GLint);
extern void (*glGenVertexArrays)(GLsizei, GLuint*);
extern void (*glBindVertexArray)(GLuint);
extern void (*glGenBuffers)(GLsizei, GLuint*);
extern void (*glBindBuffer)(GLenum, GLuint);
extern void (*glBufferData)(GLenum, GLsizeiptr, const void*, GLenum);
extern void (*glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
extern void (*glEnableVertexAttribArray)(GLuint);
extern void (*glDrawArrays)(GLenum, GLint, GLsizei);
extern void (*glGenTextures)(GLsizei, GLuint*);
extern void (*glBindTexture)(GLenum, GLuint);
extern void (*glActiveTexture)(GLenum);
extern void (*glTexParameteri)(GLenum, GLenum, GLint);
extern void (*glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
extern void (*glClear)(GLbitfield);
extern void (*glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat);
extern void (*glViewport)(GLint, GLint, GLsizei, GLsizei);

#endif
