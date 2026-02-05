#include "gl_core.h"

static void* glProc(const char* name) {
    return (void*)SDL_GL_GetProcAddress(name);
}

void (*glDeleteShader)(GLuint) = nullptr;
void (*glDeleteProgram)(GLuint) = nullptr;
void (*glDeleteBuffers)(GLsizei, const GLuint*) = nullptr;
void (*glDeleteVertexArrays)(GLsizei, const GLuint*) = nullptr;
void (*glDeleteTextures)(GLsizei, const GLuint*) = nullptr;
GLuint (*glCreateShader)(GLenum) = nullptr;
void (*glShaderSource)(GLuint, GLsizei, const char* const*, const GLint*) = nullptr;
void (*glCompileShader)(GLuint) = nullptr;
void (*glGetShaderiv)(GLuint, GLenum, GLint*) = nullptr;
void (*glGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, char*) = nullptr;
GLuint (*glCreateProgram)(void) = nullptr;
void (*glAttachShader)(GLuint, GLuint) = nullptr;
void (*glLinkProgram)(GLuint) = nullptr;
void (*glGetProgramiv)(GLuint, GLenum, GLint*) = nullptr;
void (*glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, char*) = nullptr;
void (*glUseProgram)(GLuint) = nullptr;
GLint (*glGetUniformLocation)(GLuint, const char*) = nullptr;
void (*glUniform1f)(GLint, GLfloat) = nullptr;
void (*glUniform2f)(GLint, GLfloat, GLfloat) = nullptr;
void (*glUniform3f)(GLint, GLfloat, GLfloat, GLfloat) = nullptr;
void (*glUniform1i)(GLint, GLint) = nullptr;
void (*glGenVertexArrays)(GLsizei, GLuint*) = nullptr;
void (*glBindVertexArray)(GLuint) = nullptr;
void (*glGenBuffers)(GLsizei, GLuint*) = nullptr;
void (*glBindBuffer)(GLenum, GLuint) = nullptr;
void (*glBufferData)(GLenum, GLsizeiptr, const void*, GLenum) = nullptr;
void (*glVertexAttribPointer)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*) = nullptr;
void (*glEnableVertexAttribArray)(GLuint) = nullptr;
void (*glDrawArrays)(GLenum, GLint, GLsizei) = nullptr;
void (*glGenTextures)(GLsizei, GLuint*) = nullptr;
void (*glBindTexture)(GLenum, GLuint) = nullptr;
void (*glActiveTexture)(GLenum) = nullptr;
void (*glTexParameteri)(GLenum, GLenum, GLint) = nullptr;
void (*glTexImage2D)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*) = nullptr;
void (*glClear)(GLbitfield) = nullptr;
void (*glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat) = nullptr;
void (*glViewport)(GLint, GLint, GLsizei, GLsizei) = nullptr;

int gl_core_load(void) {
#define L(n) do { *(void**)&n = glProc(#n); if (!(n)) return -1; } while(0)
    L(glDeleteShader);
    L(glDeleteProgram);
    L(glDeleteBuffers);
    L(glDeleteVertexArrays);
    L(glDeleteTextures);
    L(glCreateShader);
    L(glShaderSource);
    L(glCompileShader);
    L(glGetShaderiv);
    L(glGetShaderInfoLog);
    L(glCreateProgram);
    L(glAttachShader);
    L(glLinkProgram);
    L(glGetProgramiv);
    L(glGetProgramInfoLog);
    L(glUseProgram);
    L(glGetUniformLocation);
    L(glUniform1f);
    L(glUniform2f);
    L(glUniform3f);
    L(glUniform1i);
    L(glGenVertexArrays);
    L(glBindVertexArray);
    L(glGenBuffers);
    L(glBindBuffer);
    L(glBufferData);
    L(glVertexAttribPointer);
    L(glEnableVertexAttribArray);
    L(glDrawArrays);
    L(glGenTextures);
    L(glBindTexture);
    L(glActiveTexture);
    L(glTexParameteri);
    L(glTexImage2D);
    L(glClear);
    L(glClearColor);
    L(glViewport);
#undef L
    return 0;
}
