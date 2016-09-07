// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef VTKVIEWER_OPENGLHELPER_H
#define VTKVIEWER_OPENGLHELPER_H

#include <vtkObject.h>
#include <vtkMatrix4x4.h>
#include "VTKViewer.h"

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>


#ifndef GL_ARB_shader_objects
typedef GLuint GLhandleARB;
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GL_ARB_shader_objects
typedef char GLcharARB;
#endif

#ifndef GL_VERTEX_PROGRAM_POINT_SIZE_ARB
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB  0x8642
#endif

#ifndef GL_VERTEX_SHADER_ARB
#define GL_VERTEX_SHADER_ARB              0x8B31
#endif

#ifndef GL_FRAGMENT_SHADER_ARB
#define GL_FRAGMENT_SHADER_ARB            0x8B30
#endif

#ifndef GL_ARB_point_sprite
#define GL_POINT_SPRITE_ARB               0x8861
#define GL_COORD_REPLACE_ARB              0x8862
#endif

#ifndef GL_ARB_vertex_buffer_object
typedef ptrdiff_t GLsizeiptrARB;

#define GL_ARRAY_BUFFER_ARB               0x8892
#define GL_STATIC_DRAW_ARB                0x88E4
#endif


namespace GUI_OPENGL {
char* readFromFile( std::string fileName );
}

class VTKVIEWER_EXPORT VTKViewer_OpenGLHelper
{
public:
  VTKViewer_OpenGLHelper();
  ~VTKViewer_OpenGLHelper();

  void Init();

  bool IsInitialized() const { return mIsInitialized; }

public:
  bool CreateShaderProgram (const std::string& theFilePath,
                            GLhandleARB&       theProgram,
                            GLhandleARB&       theVertexShader,
                            GLhandleARB&       theFragmentShader) const;

  void DestroyShaderProgram (GLhandleARB theProgram,
                             GLhandleARB theVertexShader,
                             GLhandleARB theFragmentShader) const;
#ifdef VTK_OPENGL2
  void SetUniformMatrix (const GLint         theLocation,
                         const vtkMatrix4x4* theMatrix) const;
#endif  

protected:
  typedef void        (APIENTRYP PFNGLSHADERSOURCEARBPROC)             (GLhandleARB shaderObj, GLsizei count, const GLcharARB* *string, const GLint *length);
  typedef GLhandleARB (APIENTRYP PFNGLCREATESHADEROBJECTARBPROC)       (GLenum shaderType);
  typedef void        (APIENTRYP PFNGLBINDBUFFERARBPROC)               (GLenum target, GLuint buffer);
  typedef void        (APIENTRYP PFNGLDELETEBUFFERSARBPROC)            (GLsizei n, const GLuint *buffers);
  typedef void        (APIENTRYP PFNGLGENBUFFERSARBPROC)               (GLsizei n, GLuint *buffers);
  typedef void        (APIENTRYP PFNGLBUFFERDATAARBPROC)               (GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);
  typedef void        (APIENTRYP PFNGLCOMPILESHADERARBPROC)            (GLhandleARB shaderObj);
  typedef GLhandleARB (APIENTRYP PFNGLCREATEPROGRAMOBJECTARBPROC)      (void);
  typedef void        (APIENTRYP PFNGLATTACHOBJECTARBPROC)             (GLhandleARB containerObj, GLhandleARB obj);
  typedef void        (APIENTRYP PFNGLLINKPROGRAMARBPROC)              (GLhandleARB programObj);
  typedef void        (APIENTRYP PFNGLUSEPROGRAMOBJECTARBPROC)         (GLhandleARB programObj);
  typedef GLint       (APIENTRYP PFNGLGETATTRIBLOCATIONARBPROC)        (GLhandleARB programObj, const GLcharARB *name);
  typedef void        (APIENTRYP PFNGLVERTEXATTRIBPOINTERARBPROC)      (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid *pointer);
  typedef void        (APIENTRYP PFNGLENABLEVERTEXATTRIBARRAYARBPROC)  (GLuint index);
  typedef void        (APIENTRYP PFNGLDISABLEVERTEXATTRIBARRAYARBPROC) (GLuint index);

#ifdef VTK_OPENGL2
  typedef void        (APIENTRYP PFNGLDETACHOBJECTARBPROC)             (GLhandleARB containerObj, GLhandleARB obj);
  typedef void        (APIENTRYP PFNGLDELETEOBJECTARBPROC)             (GLhandleARB obj);
  typedef void        (APIENTRYP PFNGLVALIDATEPROGRAMARBPROC)          (GLhandleARB program);
  typedef GLint       (APIENTRYP PFNGLGETUNIFORMLOCATIONARBPROC)       (GLhandleARB program, const GLcharARB *name );
  typedef void        (APIENTRYP PFNGLGETSHADERIVARBPROC)              (GLuint shader, GLenum pname, GLint *params);
  typedef void        (APIENTRYP PFNGLGETPROGRAMIVARBPROC)             (GLuint program, GLenum pname, GLint *params);
  typedef void        (APIENTRYP PFNGLGETSHADERINFOLOGARBPROC)         (GLuint shader, GLsizei maxLength, GLsizei *length, GLchar *infoLog);
  typedef void        (APIENTRYP PFNGLUNIFORMMATRIX4FVARBPROC)         (GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  typedef void        (APIENTRYP PFNGLGENVERTEXARRAYSARBPROC)          (GLsizei n, GLuint *arrays);
  typedef void        (APIENTRYP PFNGLBINDVERTEXARRAYARBPROC)          (GLuint array);
  typedef void        (APIENTRYP PFNGLUNIFORM1IARBPROC)                (GLint location, GLint v0);
#endif

public:
  PFNGLSHADERSOURCEARBPROC             vglShaderSourceARB;
  PFNGLCREATESHADEROBJECTARBPROC       vglCreateShaderObjectARB;
  PFNGLCOMPILESHADERARBPROC            vglCompileShaderARB;
  PFNGLCREATEPROGRAMOBJECTARBPROC      vglCreateProgramObjectARB;
  PFNGLATTACHOBJECTARBPROC             vglAttachObjectARB;
  PFNGLLINKPROGRAMARBPROC              vglLinkProgramARB;
  PFNGLUSEPROGRAMOBJECTARBPROC         vglUseProgramObjectARB;

  PFNGLGENBUFFERSARBPROC               vglGenBuffersARB;
  PFNGLBINDBUFFERARBPROC               vglBindBufferARB;
  PFNGLBUFFERDATAARBPROC               vglBufferDataARB;
  PFNGLDELETEBUFFERSARBPROC            vglDeleteBuffersARB;
  PFNGLGETATTRIBLOCATIONARBPROC        vglGetAttribLocationARB;
  PFNGLVERTEXATTRIBPOINTERARBPROC      vglVertexAttribPointerARB;
  PFNGLENABLEVERTEXATTRIBARRAYARBPROC  vglEnableVertexAttribArrayARB;
  PFNGLDISABLEVERTEXATTRIBARRAYARBPROC vglDisableVertexAttribArrayARB;

#ifdef VTK_OPENGL2
  PFNGLDETACHOBJECTARBPROC             vglDetachObjectARB;
  PFNGLDELETEOBJECTARBPROC             vglDeleteObjectARB;
  PFNGLVALIDATEPROGRAMARBPROC          vglValidateProgramARB;
  PFNGLGETSHADERIVARBPROC              vglGetShaderivARB;
  PFNGLGETPROGRAMIVARBPROC             vglGetProgramivARB;
  PFNGLGETSHADERINFOLOGARBPROC         vglGetShaderInfoLogARB;

  PFNGLUNIFORMMATRIX4FVARBPROC         vglUniformMatrix4fvARB;
  PFNGLGENVERTEXARRAYSARBPROC          vglGenVertexArraysARB;
  PFNGLBINDVERTEXARRAYARBPROC          vglBindVertexArrayARB;
  PFNGLUNIFORM1IARBPROC                vglUniform1iARB;
  PFNGLGETUNIFORMLOCATIONARBPROC       vglGetUniformLocationARB;
#endif

protected:
  bool mIsInitialized;
};

#endif
