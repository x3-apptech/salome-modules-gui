// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
#include "VTKViewer_Texture.h"

#include "vtkHomogeneousTransform.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkObjectFactory.h"
#include "vtkOpenGLRenderer.h"
#include "vtkPointData.h"
#include "vtkRenderWindow.h"
#ifndef VTK_OPENGL2
#include "vtkOpenGLExtensionManager.h"
#include "vtkgl.h" // vtkgl namespace
#else
#include "vtkTextureObject.h"
#include "vtkOpenGLError.h"
#endif
#include "vtkOpenGLRenderWindow.h"
#include "vtkTransform.h"
#include "vtkPixelBufferObject.h"
#include "vtkOpenGL.h"
#include <vtkObjectFactory.h>

vtkStandardNewMacro(VTKViewer_Texture);


// ----------------------------------------------------------------------------
VTKViewer_Texture::VTKViewer_Texture()
{
  myWidth = 0;
  myHeight = 0;
  myPosition = VTKViewer_Texture::Centered;
}

// ----------------------------------------------------------------------------
VTKViewer_Texture::~VTKViewer_Texture()
{
}

// ----------------------------------------------------------------------------
// Implement base class method.
void VTKViewer_Texture::Load(vtkRenderer *ren)
{
#ifndef VTK_OPENGL2  
  GLenum format = GL_LUMINANCE;
  vtkImageData *input = this->GetInput();

  this->Initialize(ren);

  // Need to reload the texture.
  // There used to be a check on the render window's mtime, but
  // this is too broad of a check (e.g. it would cause all textures
  // to load when only the desired update rate changed).
  // If a better check is required, check something more specific,
  // like the graphics context.
  vtkOpenGLRenderWindow* renWin = 
    static_cast<vtkOpenGLRenderWindow*>(ren->GetRenderWindow());

  if(this->BlendingMode != VTK_TEXTURE_BLENDING_MODE_NONE
     && vtkgl::ActiveTexture)
    {
    glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, vtkgl::COMBINE);

    switch(this->BlendingMode)
      {
      case VTK_TEXTURE_BLENDING_MODE_REPLACE:
        {
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, GL_REPLACE);
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, GL_REPLACE);
        break;
        }
      case VTK_TEXTURE_BLENDING_MODE_MODULATE:
        {
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, GL_MODULATE);
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, GL_MODULATE);
        break;
        }
      case VTK_TEXTURE_BLENDING_MODE_ADD:
        {
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, GL_ADD);
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, GL_ADD);
        break;
        }
      case VTK_TEXTURE_BLENDING_MODE_ADD_SIGNED:
        {
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, vtkgl::ADD_SIGNED);
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, vtkgl::ADD_SIGNED);
        break;
        }
      case VTK_TEXTURE_BLENDING_MODE_INTERPOLATE:
        {
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, vtkgl::INTERPOLATE);
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, vtkgl::INTERPOLATE);
        break;
        }
      case VTK_TEXTURE_BLENDING_MODE_SUBTRACT:
        {
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, vtkgl::SUBTRACT);
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, vtkgl::SUBTRACT);
        break;
        }
      default:
        {
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_RGB, GL_ADD);
        glTexEnvf (GL_TEXTURE_ENV, vtkgl::COMBINE_ALPHA, GL_ADD);
        }
      }
    }

  if (this->GetMTime() > this->LoadTime.GetMTime() ||
      input->GetMTime() > this->LoadTime.GetMTime() ||
      (this->GetLookupTable() && this->GetLookupTable()->GetMTime () >  
       this->LoadTime.GetMTime()) || 
       renWin != this->RenderWindow.GetPointer() ||
       renWin->GetContextCreationTime() > this->LoadTime)
    {
    int bytesPerPixel;
    int size[3];
    vtkDataArray *scalars;
    unsigned char *dataPtr;
    unsigned char *resultData=NULL;
    int xsize, ysize;
    unsigned int xs,ys;
    GLuint tempIndex=0;

    // Get the scalars the user choose to color with.
    scalars = this->GetInputArrayToProcess(0, input);

    // make sure scalars are non null
    if (!scalars) 
      {
      vtkErrorMacro(<< "No scalar values found for texture input!");
      return;
      }

    // get some info
    input->GetDimensions(size);

    if (input->GetNumberOfCells() == scalars->GetNumberOfTuples())
      {
      // we are using cell scalars. Adjust image size for cells.
      for (int kk=0; kk < 3; kk++)
        {
        if (size[kk]>1)
          {
          size[kk]--;
          }
        }
      }

    bytesPerPixel = scalars->GetNumberOfComponents();

    // make sure using unsigned char data of color scalars type
    if (this->MapColorScalarsThroughLookupTable ||
       scalars->GetDataType() != VTK_UNSIGNED_CHAR )
      {
      dataPtr = this->MapScalarsToColors (scalars);
      bytesPerPixel = 4;
      }
    else
      {
      dataPtr = static_cast<vtkUnsignedCharArray *>(scalars)->GetPointer(0);
      }

    // we only support 2d texture maps right now
    // so one of the three sizes must be 1, but it 
    // could be any of them, so lets find it
    if (size[0] == 1)
      {
      xsize = size[1]; ysize = size[2];
      }
    else
      {
      xsize = size[0];
      if (size[1] == 1)
        {
        ysize = size[2];
        }
      else
        {
        ysize = size[1];
        if (size[2] != 1)
          {
          vtkErrorMacro(<< "3D texture maps currently are not supported!");
          return;
          }
        }
      }
    
    
    if(!this->CheckedHardwareSupport)
      {
      vtkOpenGLExtensionManager *m=renWin->GetExtensionManager();
      this->CheckedHardwareSupport=true;
      this->SupportsNonPowerOfTwoTextures=
        m->ExtensionSupported("GL_VERSION_2_0")
        || m->ExtensionSupported("GL_ARB_texture_non_power_of_two");
      this->SupportsPBO=vtkPixelBufferObject::IsSupported(renWin);
      }
    
    // -- decide whether the texture needs to be resampled --
    
    GLint maxDimGL;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxDimGL);
    // if larger than permitted by the graphics library then must resample
    bool resampleNeeded=xsize > maxDimGL || ysize > maxDimGL;
    if(resampleNeeded)
      {
      vtkDebugMacro( "Texture too big for gl, maximum is " << maxDimGL);
      }
    
    if(!resampleNeeded && !this->SupportsNonPowerOfTwoTextures)
      {
      // xsize and ysize must be a power of 2 in OpenGL
      xs = static_cast<unsigned int>(xsize);
      ys = static_cast<unsigned int>(ysize);
      while (!(xs & 0x01))
        {
        xs = xs >> 1;
        }
      while (!(ys & 0x01))
        {
        ys = ys >> 1;
        }
      // if not a power of two then resampling is required
      resampleNeeded= (xs>1) || (ys>1);
      }

    if(resampleNeeded)
      {
      vtkDebugMacro(<< "Resampling texture to power of two for OpenGL");
      resultData = this->ResampleToPowerOfTwo(xsize, ysize, dataPtr, 
                                              bytesPerPixel);
      }

    if ( resultData == NULL )
        {
        resultData = dataPtr;
        }

    // free any old display lists (from the old context)
    if (this->RenderWindow)
      {
      this->ReleaseGraphicsResources(this->RenderWindow);
      }
    
     this->RenderWindow = ren->GetRenderWindow();
     
    // make the new context current before we mess with opengl
    this->RenderWindow->MakeCurrent();
 
    // define a display list for this texture
    // get a unique display list id

#ifdef GL_VERSION_1_1
    glGenTextures(1, &tempIndex);
    this->Index = static_cast<long>(tempIndex);
    glBindTexture(GL_TEXTURE_2D, this->Index);
#else
    this->Index = glGenLists(1);
    glDeleteLists (static_cast<GLuint>(this->Index), static_cast<GLsizei>(0));
    glNewList (static_cast<GLuint>(this->Index), GL_COMPILE);
#endif
    //seg fault protection for those wackos that don't use an
    //opengl render window
    if(this->RenderWindow->IsA("vtkOpenGLRenderWindow"))
      {
      static_cast<vtkOpenGLRenderWindow *>(ren->GetRenderWindow())->
        RegisterTextureResource( this->Index );
      }

    if (this->Interpolate)
      {
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                       GL_LINEAR);
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                       GL_LINEAR );
      }
    else
      {
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
      }
    if (this->Repeat)
      {
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT );
      glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT );
      }
    else
      {
      vtkOpenGLExtensionManager* manager = renWin->GetExtensionManager();
      if (this->EdgeClamp &&
           (manager->ExtensionSupported("GL_VERSION_1_2") ||
            manager->ExtensionSupported("GL_EXT_texture_edge_clamp")))
        {
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                         vtkgl::CLAMP_TO_EDGE );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                         vtkgl::CLAMP_TO_EDGE );
        }
      else
        {
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP );
        glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP );
        }
      }
    int internalFormat = bytesPerPixel;
    switch (bytesPerPixel)
      {
      case 1: format = GL_LUMINANCE; break;
      case 2: format = GL_LUMINANCE_ALPHA; break;
      case 3: format = GL_RGB; break;
      case 4: format = GL_RGBA; break;
      }
    // if we are using OpenGL 1.1, you can force 32 or16 bit textures
#ifdef GL_VERSION_1_1
    if (this->Quality == VTK_TEXTURE_QUALITY_32BIT)
      {
      switch (bytesPerPixel)
        {
        case 1: internalFormat = GL_LUMINANCE8; break;
        case 2: internalFormat = GL_LUMINANCE8_ALPHA8; break;
        case 3: internalFormat = GL_RGB8; break;
        case 4: internalFormat = GL_RGBA8; break;
        }
      }
    else if (this->Quality == VTK_TEXTURE_QUALITY_16BIT)
      {
      switch (bytesPerPixel)
        {
        case 1: internalFormat = GL_LUMINANCE4; break;
        case 2: internalFormat = GL_LUMINANCE4_ALPHA4; break;
        case 3: internalFormat = GL_RGB4; break;
        case 4: internalFormat = GL_RGBA4; break;
        }
      }
#endif
    if(this->SupportsPBO)
      {
      if(this->PBO==0)
        {
        this->PBO=vtkPixelBufferObject::New();
        this->PBO->SetContext(renWin);
        }
      unsigned int dims[2];
      vtkIdType increments[2];
      dims[0]=static_cast<unsigned int>(xsize);
      dims[1]=static_cast<unsigned int>(ysize);
      increments[0]=0;
      increments[1]=0;
      this->PBO->Upload2D(VTK_UNSIGNED_CHAR,resultData,dims,bytesPerPixel,
        increments);
      // non-blocking call
      this->PBO->Bind(vtkPixelBufferObject::UNPACKED_BUFFER);
      glTexImage2D( GL_TEXTURE_2D, 0 , internalFormat,
                    xsize, ysize, 0, format, 
                    GL_UNSIGNED_BYTE,0);
      myWidth = xsize;
      myHeight = ysize;
      this->PBO->UnBind();
      }
    else
      {
      // blocking call
      glTexImage2D( GL_TEXTURE_2D, 0 , internalFormat,
                    xsize, ysize, 0, format, 
                    GL_UNSIGNED_BYTE,
                    static_cast<const GLvoid *>(resultData) );
        myWidth = xsize;
        myHeight = ysize;
      }
#ifndef GL_VERSION_1_1
    glEndList ();
#endif
    // modify the load time to the current time
    this->LoadTime.Modified();
    
    // free memory
    if (resultData != dataPtr)
      {
      delete [] resultData;
      }
    }

  // execute the display list that uses creates the texture
#ifdef GL_VERSION_1_1
  glBindTexture(GL_TEXTURE_2D, this->Index);
#else
  glCallList(this->Index);
#endif
  
  // don't accept fragments if they have zero opacity. this will stop the
  // zbuffer from be blocked by totally transparent texture fragments.
  glAlphaFunc (GL_GREATER, static_cast<GLclampf>(0));
  glEnable (GL_ALPHA_TEST);

  if (this->PremultipliedAlpha)
    {
    // save the blend function.
    glPushAttrib(GL_COLOR_BUFFER_BIT);

    // make the blend function correct for textures premultiplied by alpha.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }

  // now bind it
  glEnable(GL_TEXTURE_2D);

  // clear any texture transform
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();

  // build transformation 
  if (this->Transform)
    {
    double *mat = this->Transform->GetMatrix()->Element[0];
    double mat2[16];
    mat2[0] = mat[0];
    mat2[1] = mat[4];
    mat2[2] = mat[8];
    mat2[3] = mat[12];
    mat2[4] = mat[1];
    mat2[5] = mat[5];
    mat2[6] = mat[9];
    mat2[7] = mat[13];
    mat2[8] = mat[2];
    mat2[9] = mat[6];
    mat2[10] = mat[10];
    mat2[11] = mat[14];
    mat2[12] = mat[3];
    mat2[13] = mat[7];
    mat2[14] = mat[11];
    mat2[15] = mat[15];
    
    // insert texture transformation 
    glMultMatrixd(mat2);
    }
  glMatrixMode(GL_MODELVIEW);
  
  GLint uUseTexture=-1;
  GLint uTexture=-1;
  
  vtkOpenGLRenderer *oRenderer=static_cast<vtkOpenGLRenderer *>(ren);
 
/*  if(oRenderer->GetDepthPeelingHigherLayer())
    {
    uUseTexture=oRenderer->GetUseTextureUniformVariable();
    uTexture=oRenderer->GetTextureUniformVariable();
    vtkgl::Uniform1i(uUseTexture,1);
    vtkgl::Uniform1i(uTexture,0); // active texture 0
    }
    */
#else
    if (!this->ExternalTextureObject)
    {
    vtkImageData *input = this->GetInput();

    // Need to reload the texture.
    // There used to be a check on the render window's mtime, but
    // this is too broad of a check (e.g. it would cause all textures
    // to load when only the desired update rate changed).
    // If a better check is required, check something more specific,
    // like the graphics context.
    vtkOpenGLRenderWindow* renWin =
      static_cast<vtkOpenGLRenderWindow*>(ren->GetRenderWindow());

    // has something changed so that we need to rebuild the texture?
    if (this->GetMTime() > this->LoadTime.GetMTime() ||
        input->GetMTime() > this->LoadTime.GetMTime() ||
        (this->GetLookupTable() && this->GetLookupTable()->GetMTime () >
         this->LoadTime.GetMTime()) ||
         renWin != this->RenderWindow.GetPointer() ||
         renWin->GetContextCreationTime() > this->LoadTime)
      {
      int size[3];
      unsigned char *dataPtr;
      unsigned char *resultData = 0;
      int xsize, ysize;

      this->RenderWindow = renWin;
      if (this->TextureObject == 0)
        {
        this->TextureObject = vtkTextureObject::New();
        }
      this->TextureObject->ResetFormatAndType();
      this->TextureObject->SetContext(renWin);

      // Get the scalars the user choose to color with.
      vtkDataArray* scalars = this->GetInputArrayToProcess(0, input);

      // make sure scalars are non null
      if (!scalars)
        {
        vtkErrorMacro(<< "No scalar values found for texture input!");
        return;
        }

      // get some info
      input->GetDimensions(size);

      if (input->GetNumberOfCells() == scalars->GetNumberOfTuples())
        {
        // we are using cell scalars. Adjust image size for cells.
        for (int kk = 0; kk < 3; kk++)
          {
          if (size[kk]>1)
            {
            size[kk]--;
            }
          }
        }

      int bytesPerPixel = scalars->GetNumberOfComponents();

      // make sure using unsigned char data of color scalars type
      if (this->IsDepthTexture != 1 &&
        (this->MapColorScalarsThroughLookupTable ||
         scalars->GetDataType() != VTK_UNSIGNED_CHAR ))
        {
        dataPtr = this->MapScalarsToColors (scalars);
        bytesPerPixel = 4;
        }
      else
        {
        dataPtr = static_cast<vtkUnsignedCharArray *>(scalars)->GetPointer(0);
        }

      // we only support 2d texture maps right now
      // so one of the three sizes must be 1, but it
      // could be any of them, so lets find it
      if (size[0] == 1)
        {
        xsize = size[1]; ysize = size[2];
        }
      else
        {
        xsize = size[0];
        if (size[1] == 1)
          {
          ysize = size[2];
          }
        else
          {
          ysize = size[1];
          if (size[2] != 1)
            {
            vtkErrorMacro(<< "3D texture maps currently are not supported!");
            return;
            }
          }
        }

      // -- decide whether the texture needs to be resampled --
      GLint maxDimGL;
      glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxDimGL);
      vtkOpenGLCheckErrorMacro("failed at glGetIntegerv");
      // if larger than permitted by the graphics library then must resample
      bool resampleNeeded = xsize > maxDimGL || ysize > maxDimGL;
      if(resampleNeeded)
        {
        vtkDebugMacro( "Texture too big for gl, maximum is " << maxDimGL);
        }

      if (resampleNeeded)
        {
        vtkDebugMacro(<< "Resampling texture to power of two for OpenGL");
        resultData = this->ResampleToPowerOfTwo(xsize, ysize, dataPtr,
                                                bytesPerPixel);
        }

      if (!resultData)
        {
        resultData = dataPtr;
        }

      // create the texture
      if (this->IsDepthTexture)
        {
        this->TextureObject->CreateDepthFromRaw(
          xsize, ysize, vtkTextureObject::Float32, scalars->GetDataType(), resultData);
        }
      else
        {
        this->TextureObject->Create2DFromRaw(
          xsize, ysize, bytesPerPixel, VTK_UNSIGNED_CHAR, resultData);
        }
      myWidth = xsize;
      myHeight = ysize;
      // activate a free texture unit for this texture
      this->TextureObject->Activate();

      // update parameters
      if (this->Interpolate)
        {
        this->TextureObject->SetMinificationFilter(vtkTextureObject::Linear);
        this->TextureObject->SetMagnificationFilter(vtkTextureObject::Linear);
        }
      else
        {
        this->TextureObject->SetMinificationFilter(vtkTextureObject::Nearest);
        this->TextureObject->SetMagnificationFilter(vtkTextureObject::Nearest);
        }
      if (this->Repeat)
        {
        this->TextureObject->SetWrapS(vtkTextureObject::Repeat);
        this->TextureObject->SetWrapT(vtkTextureObject::Repeat);
        this->TextureObject->SetWrapR(vtkTextureObject::Repeat);
        }
      else
        {
        this->TextureObject->SetWrapS(vtkTextureObject::ClampToEdge);
        this->TextureObject->SetWrapT(vtkTextureObject::ClampToEdge);
        this->TextureObject->SetWrapR(vtkTextureObject::ClampToEdge);
        }

      // modify the load time to the current time
      this->LoadTime.Modified();

      // free memory
      if (resultData != dataPtr)
        {
        delete [] resultData;
        resultData = 0;
        }
      }
    }
  else
    {
    vtkOpenGLRenderWindow* renWin =
      static_cast<vtkOpenGLRenderWindow*>(ren->GetRenderWindow());

      // has something changed so that we need to rebuild the texture?
      if (this->GetMTime() > this->LoadTime.GetMTime() ||
         renWin != this->RenderWindow.GetPointer() ||
         renWin->GetContextCreationTime() > this->LoadTime)
        {
        this->RenderWindow = renWin;
        this->TextureObject->SetContext(renWin);
        }
    }        
  // activate a free texture unit for this texture
  this->TextureObject->Activate();

  if (this->PremultipliedAlpha)
    {
    // make the blend function correct for textures premultiplied by alpha.
    glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
    }
  vtkOpenGLCheckErrorMacro("failed after Load");
#endif  
}

void VTKViewer_Texture::Initialize(vtkRenderer * vtkNotUsed(ren))
{
}

int VTKViewer_Texture::GetWidth() const
{
  return myWidth;
}

int VTKViewer_Texture::GetHeight() const
{
  return myHeight;
}

void VTKViewer_Texture::SetPosition(int pos)
{
  myPosition = pos;
}

int VTKViewer_Texture::GetPosition() const
{
  return myPosition;
}
