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

//  File   : OpenGLUtils_FrameBuffer.cxx
//  Module : SALOME
//
#ifdef VTK_OPENGL2
#define GL_GLEXT_PROTOTYPES
#endif

#include "OpenGLUtils_FrameBuffer.h"

#include <utilities.h>

#include <cstring>

#ifndef WIN32
# ifndef GLX_GLXEXT_LEGACY
#  define GLX_GLXEXT_LEGACY
# endif
# include <GL/glx.h>
# include <dlfcn.h>
#else
# include <wingdi.h>
#endif

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GL_FRAMEBUFFER_EXT
#define GL_FRAMEBUFFER_EXT                0x8D40
#endif

#ifndef GL_RENDERBUFFER_EXT
#define GL_RENDERBUFFER_EXT               0x8D41
#endif

#ifndef GL_COLOR_ATTACHMENT0_EXT
#define GL_COLOR_ATTACHMENT0_EXT          0x8CE0
#endif

#ifndef GL_DEPTH_ATTACHMENT_EXT
#define GL_DEPTH_ATTACHMENT_EXT           0x8D00
#endif

#ifndef GL_FRAMEBUFFER_COMPLETE_EXT
#define GL_FRAMEBUFFER_COMPLETE_EXT       0x8CD5
#endif

typedef void (APIENTRYP PFNGLGENFRAMEBUFFERSEXTPROC) (GLsizei n, GLuint *framebuffers);
typedef void (APIENTRYP PFNGLBINDFRAMEBUFFEREXTPROC) (GLenum target, GLuint framebuffer);
typedef void (APIENTRYP PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) (GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum (APIENTRYP PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) (GLenum target);
typedef void (APIENTRYP PFNGLDELETEFRAMEBUFFERSEXTPROC) (GLsizei n, const GLuint *framebuffers);
typedef void (APIENTRYP PFNGLGENRENDERBUFFERSEXTPROC) (GLsizei n, GLuint *renderbuffers);
typedef void (APIENTRYP PFNGLBINDRENDERBUFFEREXTPROC) (GLenum target, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEEXTPROC) (GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
typedef void (APIENTRYP PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC) (GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
typedef void (APIENTRYP PFNGLDELETERENDERBUFFERSEXTPROC) (GLsizei n, const GLuint *renderbuffers);

static PFNGLGENFRAMEBUFFERSEXTPROC vglGenFramebuffersEXT = NULL;
static PFNGLBINDFRAMEBUFFEREXTPROC vglBindFramebufferEXT = NULL;
static PFNGLFRAMEBUFFERTEXTURE2DEXTPROC vglFramebufferTexture2DEXT = NULL;
static PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC vglCheckFramebufferStatusEXT = NULL;
static PFNGLDELETEFRAMEBUFFERSEXTPROC vglDeleteFramebuffersEXT = NULL;
static PFNGLGENRENDERBUFFERSEXTPROC vglGenRenderbuffersEXT = NULL;
static PFNGLBINDRENDERBUFFEREXTPROC vglBindRenderbufferEXT = NULL;
static PFNGLRENDERBUFFERSTORAGEEXTPROC vglRenderbufferStorageEXT = NULL;
static PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC vglFramebufferRenderbufferEXT = NULL;
static PFNGLDELETERENDERBUFFERSEXTPROC vglDeleteRenderbuffersEXT = NULL;

#ifndef WIN32
#define GL_GetProcAddress( x ) glXGetProcAddressARB( (const GLubyte*)x )
#else
#define GL_GetProcAddress( x ) wglGetProcAddress( (const LPCSTR)x )
#endif

bool InitializeEXT()
{
  vglGenFramebuffersEXT = (PFNGLGENFRAMEBUFFERSEXTPROC)GL_GetProcAddress( "glGenFramebuffersEXT" );
  vglBindFramebufferEXT = (PFNGLBINDFRAMEBUFFEREXTPROC)GL_GetProcAddress( "glBindFramebufferEXT" );
  vglFramebufferTexture2DEXT = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)GL_GetProcAddress( "glFramebufferTexture2DEXT" );
  vglCheckFramebufferStatusEXT = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)GL_GetProcAddress( "glCheckFramebufferStatusEXT" );
  vglDeleteFramebuffersEXT = (PFNGLDELETEFRAMEBUFFERSEXTPROC)GL_GetProcAddress( "glDeleteFramebuffersEXT" );
  vglGenRenderbuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC)GL_GetProcAddress( "glGenRenderbuffersEXT" );
  vglBindRenderbufferEXT = (PFNGLBINDRENDERBUFFEREXTPROC)GL_GetProcAddress( "glBindRenderbufferEXT" );
  vglRenderbufferStorageEXT = (PFNGLRENDERBUFFERSTORAGEEXTPROC)GL_GetProcAddress( "glRenderbufferStorageEXT" );
  vglFramebufferRenderbufferEXT = (PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)GL_GetProcAddress( "glFramebufferRenderbufferEXT" );
  vglDeleteRenderbuffersEXT = (PFNGLDELETERENDERBUFFERSEXTPROC)GL_GetProcAddress( "glDeleteRenderbuffersEXT" );

  bool ok = vglGenFramebuffersEXT && vglBindFramebufferEXT && vglFramebufferTexture2DEXT &&
            vglCheckFramebufferStatusEXT && vglDeleteFramebuffersEXT && vglGenRenderbuffersEXT &&
            vglBindRenderbufferEXT && vglRenderbufferStorageEXT && vglFramebufferRenderbufferEXT &&
            vglDeleteRenderbuffersEXT;

  return ok;
}

static bool IsEXTInitialized = InitializeEXT();

OpenGLUtils_FrameBuffer::OpenGLUtils_FrameBuffer()
  : textureId( 0 ),
    fboId( 0 ),
    rboId( 0 )
{
}

OpenGLUtils_FrameBuffer::~OpenGLUtils_FrameBuffer()
{
  release();
}

bool OpenGLUtils_FrameBuffer::init( const GLsizei& xSize, const GLsizei& ySize )
{
#ifdef VTK_OPENGL2
  int n = 0;
  std::ostringstream strm;
  glGetIntegerv(GL_NUM_EXTENSIONS, &n);
  for (int i = 0; i < n; i++)
    {
      const char *exti = (const char *)glGetStringi(GL_EXTENSIONS, i);
      strm<< exti <<" ";
    }
  std::string s = strm.str();
  const char* ext = s.c_str();
#else  
  char* ext = (char*)glGetString( GL_EXTENSIONS );
#endif  
  if( !IsEXTInitialized || !ext ||
      strstr( ext, "GL_EXT_framebuffer_object" ) == NULL )
  {
    MESSAGE( "Initializing OpenGL FrameBuffer extension failed" );
    return false;
  }

  // create a texture object
  glEnable( GL_TEXTURE_2D );
  glGenTextures( 1, &textureId );
  glBindTexture( GL_TEXTURE_2D, textureId );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, xSize, ySize, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
  glBindTexture( GL_TEXTURE_2D, 0 );

  // create a renderbuffer object to store depth info
  vglGenRenderbuffersEXT( 1, &rboId );
  vglBindRenderbufferEXT( GL_RENDERBUFFER_EXT, rboId );
  vglRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, xSize, ySize );
  vglBindRenderbufferEXT( GL_RENDERBUFFER_EXT, 0 );

  // create a framebuffer object
  vglGenFramebuffersEXT( 1, &fboId );
  vglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fboId );

  // attach the texture to FBO color attachment point
  vglFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, textureId, 0 );

  // attach the renderbuffer to depth attachment point
  vglFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, rboId );

  // check FBO status
  GLenum status = vglCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );

  // Unbind FBO
  vglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

  return status == GL_FRAMEBUFFER_COMPLETE_EXT;
}

void OpenGLUtils_FrameBuffer::release()
{
  if( !IsEXTInitialized )
    return;

  glDeleteTextures( 1, &textureId );
  textureId = 0;

  vglDeleteFramebuffersEXT( 1, &fboId );
  fboId = 0;

  vglDeleteRenderbuffersEXT( 1, &rboId );
  rboId = 0;
}

void OpenGLUtils_FrameBuffer::bind()
{
  if( !IsEXTInitialized )
    return;

  vglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, fboId );
}

void OpenGLUtils_FrameBuffer::unbind()
{
  if( !IsEXTInitialized )
    return;

  vglBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}
