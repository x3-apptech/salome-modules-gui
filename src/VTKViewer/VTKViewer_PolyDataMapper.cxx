// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "VTKViewer_PolyDataMapper.h"
#include "VTKViewer_MarkerUtils.h"

#include <utilities.h>

#include <QString>

#include <vtkCellArray.h>
#include <vtkImageData.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTimerLog.h>
#include <vtkWindow.h>

#ifndef WNT
# ifndef GLX_GLXEXT_LEGACY
#  define GLX_GLXEXT_LEGACY
# endif
# include <GL/glx.h>
# include <dlfcn.h>
#else
# include <wingdi.h>
#endif

#ifndef VTK_IMPLEMENT_MESA_CXX
vtkCxxRevisionMacro(VTKViewer_PolyDataMapper, "Revision$");
vtkStandardNewMacro(VTKViewer_PolyDataMapper);
#endif

// some definitions for what the polydata has in it
#define VTK_PDPSM_COLORS             0x0001
#define VTK_PDPSM_CELL_COLORS        0x0002
#define VTK_PDPSM_POINT_TYPE_FLOAT   0x0004
#define VTK_PDPSM_POINT_TYPE_DOUBLE  0x0008
#define VTK_PDPSM_NORMAL_TYPE_FLOAT  0x0010
#define VTK_PDPSM_NORMAL_TYPE_DOUBLE 0x0020
#define VTK_PDPSM_OPAQUE_COLORS      0x0040

#ifndef APIENTRY
#define APIENTRY
#endif
#ifndef APIENTRYP
#define APIENTRYP APIENTRY *
#endif

#ifndef GL_VERTEX_PROGRAM_POINT_SIZE_ARB
#define GL_VERTEX_PROGRAM_POINT_SIZE_ARB  0x8642
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

typedef void (APIENTRYP PFNGLBINDBUFFERARBPROC) (GLenum target, GLuint buffer);
typedef void (APIENTRYP PFNGLDELETEBUFFERSARBPROC) (GLsizei n, const GLuint *buffers);
typedef void (APIENTRYP PFNGLGENBUFFERSARBPROC) (GLsizei n, GLuint *buffers);
typedef void (APIENTRYP PFNGLBUFFERDATAARBPROC) (GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage);

static PFNGLGENBUFFERSARBPROC               vglGenBuffersARB              = NULL;
static PFNGLBINDBUFFERARBPROC               vglBindBufferARB              = NULL;
static PFNGLBUFFERDATAARBPROC               vglBufferDataARB              = NULL;
static PFNGLDELETEBUFFERSARBPROC            vglDeleteBuffersARB           = NULL;

#ifndef WNT
#define GL_GetProcAddress( x )   glXGetProcAddressARB( (const GLubyte*)x )
#else
#define GL_GetProcAddress( x )   wglGetProcAddress( (const LPCSTR)x )
#endif

bool InitializeBufferExtensions()
{
  vglGenBuffersARB = (PFNGLGENBUFFERSARBPROC)GL_GetProcAddress( "glGenBuffersARB" );
  if( !vglGenBuffersARB )
    return false;

  vglBindBufferARB = (PFNGLBINDBUFFERARBPROC)GL_GetProcAddress( "glBindBufferARB" );
  if( !vglBindBufferARB )
    return false;

  vglBufferDataARB = (PFNGLBUFFERDATAARBPROC)GL_GetProcAddress( "glBufferDataARB" );
  if( !vglBufferDataARB )
    return false;

  vglDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)GL_GetProcAddress( "glDeleteBuffersARB" );
  if( !vglDeleteBuffersARB )
    return false;

  return true;
};

static bool IsBufferExtensionsInitialized = InitializeBufferExtensions();

//-----------------------------------------------------------------------------
VTKViewer_PolyDataMapper::VTKViewer_PolyDataMapper()
{
  Q_INIT_RESOURCE( VTKViewer );

  this->ExtensionsInitialized     = ES_None;

  this->PointSpriteTexture        = 0;

  this->MarkerEnabled             = false;
  this->MarkerType                = VTK::MT_NONE;
  this->MarkerScale               = VTK::MS_NONE;
  this->MarkerId                  = 0;
}

//-----------------------------------------------------------------------------
VTKViewer_PolyDataMapper::~VTKViewer_PolyDataMapper()
{
  if( PointSpriteTexture > 0 )
    glDeleteTextures( 1, &PointSpriteTexture );
}

//-----------------------------------------------------------------------------
void VTKViewer_PolyDataMapper::SetMarkerEnabled( bool theMarkerEnabled )
{
  if( this->MarkerEnabled == theMarkerEnabled )
    return;

  this->MarkerEnabled = theMarkerEnabled;
  this->Modified();
}

//-----------------------------------------------------------------------------
void VTKViewer_PolyDataMapper::SetMarkerStd( VTK::MarkerType theMarkerType, VTK::MarkerScale theMarkerScale )
{
  if( this->MarkerType == theMarkerType && this->MarkerScale == theMarkerScale )
    return;

  this->MarkerType = theMarkerType;
  this->MarkerScale = theMarkerScale;

  if( this->MarkerType == VTK::MT_NONE || this->MarkerType == VTK::MT_USER ) {
    this->ImageData = NULL;
    this->Modified();
    return;
  }

  int aMarkerType = (int)this->MarkerType;
  int aMarkerScale = (int)this->MarkerScale;

  int anId = (int)VTK::MS_70 * aMarkerType + aMarkerScale;

  if( this->StandardTextures.find( anId ) == this->StandardTextures.end() )
  {
    QString aFileName = QString( ":/textures/texture%1.dat" ).arg( aMarkerType );
    VTK::MarkerTexture aMarkerTexture;
    if( VTK::LoadTextureData( aFileName, theMarkerScale, aMarkerTexture ) )
      this->StandardTextures[ anId ] = VTK::MakeVTKImage( aMarkerTexture );
  }

  this->ImageData = this->StandardTextures[ anId ];
  this->Modified();
}

//-----------------------------------------------------------------------------
void VTKViewer_PolyDataMapper::SetMarkerTexture( int theMarkerId, VTK::MarkerTexture theMarkerTexture )
{
  if( this->MarkerType == VTK::MT_USER && this->MarkerId == theMarkerId )
    return;

  this->MarkerType = VTK::MT_USER;
  this->MarkerId = theMarkerId;

  if( this->CustomTextures.find( theMarkerId ) == this->CustomTextures.end() )
    this->CustomTextures[ theMarkerId ] = VTK::MakeVTKImage( theMarkerTexture );

  this->ImageData = this->CustomTextures[ theMarkerId ];
  this->Modified();
}

//-----------------------------------------------------------------------------
VTK::MarkerType VTKViewer_PolyDataMapper::GetMarkerType()
{
  return this->MarkerType;
}

//-----------------------------------------------------------------------------
VTK::MarkerScale VTKViewer_PolyDataMapper::GetMarkerScale()
{
  return this->MarkerScale;
}

//-----------------------------------------------------------------------------
int VTKViewer_PolyDataMapper::GetMarkerTexture()
{
  return this->MarkerId;
}

//-----------------------------------------------------------------------------
int VTKViewer_PolyDataMapper::InitExtensions()
{
  char* ext = (char*)glGetString( GL_EXTENSIONS );
  if( !IsBufferExtensionsInitialized ||
      strstr( ext, "GL_ARB_point_sprite" ) == NULL ||
      strstr( ext, "GL_ARB_vertex_buffer_object" ) == NULL )
  {
    MESSAGE("Initializing ARB extensions failed");
    return ES_Error;
  }

  return ES_Ok;
}

//-----------------------------------------------------------------------------
void VTKViewer_PolyDataMapper::InitPointSprites()
{
  glEnable( GL_POINT_SPRITE_ARB );
  glEnable( GL_VERTEX_PROGRAM_POINT_SIZE_ARB );

  glPushAttrib( GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT );

  glDepthFunc( GL_LEQUAL );
  glEnable( GL_DEPTH_TEST );

  glEnable( GL_ALPHA_TEST );
  glAlphaFunc( GL_GREATER, 0.0 );

  glDisable( GL_LIGHTING );

  glDisable( GL_COLOR_MATERIAL );
}

//-----------------------------------------------------------------------------
void VTKViewer_PolyDataMapper::CleanupPointSprites()
{
  glPopAttrib();

  glDisable( GL_VERTEX_PROGRAM_POINT_SIZE_ARB );
  glDisable( GL_POINT_SPRITE_ARB );
}

//-----------------------------------------------------------------------------
void VTKViewer_PolyDataMapper::InitTextures()
{
  if( !this->ImageData.GetPointer() )
    return;

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

  int* aSize = this->ImageData->GetDimensions();
  unsigned char* dataPtr = (unsigned char*)this->ImageData->GetScalarPointer();
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, aSize[0], aSize[1], 0,
                GL_RGBA, GL_UNSIGNED_BYTE, dataPtr );

  //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
  glEnable( GL_TEXTURE_2D );
  glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
  glBindTexture( GL_TEXTURE_2D, this->PointSpriteTexture );
}

//-----------------------------------------------------------------------------
void VTKViewer_PolyDataMapper::RenderPiece( vtkRenderer* ren, vtkActor* act )
{
  bool isUsePointSprites = this->MarkerEnabled && this->MarkerType != VTK::MT_NONE;
  if( isUsePointSprites )
  {
    if( this->ExtensionsInitialized == ES_None )
      this->ExtensionsInitialized = this->InitExtensions();
    this->InitPointSprites();
    this->InitTextures();
  }

  MAPPER_SUPERCLASS::RenderPiece( ren, act );

  if( isUsePointSprites )
    this->CleanupPointSprites();
}

//-----------------------------------------------------------------------------
// Definition of structures and fuctions used in Draw() method
namespace VTK
{
  //-----------------------------------------------------------------------------
  struct TVertex
  {
    GLfloat r, g, b, a;
    GLfloat vx, vy, vz;
  };

  //-----------------------------------------------------------------------------
  struct TColorFunctorBase
  {
    vtkFloatingPointType myAlpha;

    TColorFunctorBase( vtkProperty* theProperty )
    {
      myAlpha = theProperty->GetOpacity();
    }

    virtual
    void
    get( TVertex& theVertex, vtkIdType thePointId, vtkIdType theCellId ) = 0;
  };

  //-----------------------------------------------------------------------------
  struct TPropertyColor : TColorFunctorBase
  {
    vtkFloatingPointType myColor[3];

    TPropertyColor( vtkProperty* theProperty ):
      TColorFunctorBase( theProperty )
    {
      theProperty->GetColor( myColor );
    }

    virtual
    void
    get( TVertex& theVertex, vtkIdType thePointId, vtkIdType theCellId )
    {
      theVertex.r = myColor[0];
      theVertex.g = myColor[1];
      theVertex.b = myColor[2];
      theVertex.a = myAlpha;
    }
  };

  //-----------------------------------------------------------------------------
  struct TColors2Color : TColorFunctorBase
  {
    vtkUnsignedCharArray* myColors;

    TColors2Color( vtkProperty* theProperty,
                   vtkUnsignedCharArray* theColors ):
      TColorFunctorBase( theProperty ),
      myColors( theColors )
    {}

    virtual
    void
    get( TVertex& theVertex, vtkIdType thePointId, vtkIdType theCellId )
    {
      vtkIdType aTupleId = GetTupleId( thePointId, theCellId );
      unsigned char* aColor = myColors->GetPointer( aTupleId << 2 );

      theVertex.r = int( aColor[0] ) / 255.0;
      theVertex.g = int( aColor[1] ) / 255.0;
      theVertex.b = int( aColor[2] ) / 255.0;
      theVertex.a = myAlpha;
    }

    virtual
    vtkIdType
    GetTupleId( vtkIdType thePointId, vtkIdType theCellId ) = 0;
  };

  //-----------------------------------------------------------------------------
  struct TPointColors2Color : TColors2Color
  {
    TPointColors2Color( vtkProperty* theProperty,
                        vtkUnsignedCharArray* theColors ):
      TColors2Color( theProperty, theColors )
    {}

    virtual
    vtkIdType
    GetTupleId( vtkIdType thePointId, vtkIdType theCellId )
    {
      return thePointId;
    }
  };

  //-----------------------------------------------------------------------------
  struct TCellColors2Color : TColors2Color
  {
    TCellColors2Color( vtkProperty* theProperty,
                       vtkUnsignedCharArray* theColors ):
      TColors2Color( theProperty, theColors )
    {}

    virtual
    vtkIdType
    GetTupleId( vtkIdType thePointId, vtkIdType theCellId )
    {
      return theCellId;
    }
  };

  //-----------------------------------------------------------------------------
  template < class TCoordinates >
  void DrawPoints( TCoordinates* theStartPoints,
                   vtkCellArray* theCells,
                   TColorFunctorBase* theColorFunctor,
                   TVertex* theVertexArr,
                   vtkIdType &theCellId,
                   vtkIdType &theVertexId )
  {
    vtkIdType* ptIds = theCells->GetPointer();
    vtkIdType* endPtIds = ptIds + theCells->GetNumberOfConnectivityEntries();

    while ( ptIds < endPtIds ) {
      vtkIdType nPts = *ptIds;
      ++ptIds;

      while ( nPts > 0 ) {
        TVertex& aVertex = theVertexArr[ theVertexId ];
        vtkIdType aPointId = *ptIds;

        TCoordinates* anOffsetPoints = theStartPoints + 3 * aPointId;
        aVertex.vx = anOffsetPoints[0];
        aVertex.vy = anOffsetPoints[1];
        aVertex.vz = anOffsetPoints[2];

        theColorFunctor->get( aVertex, aPointId, theCellId );

        ++theVertexId;
        ++ptIds; 
        --nPts; 
      }

      ++theCellId;
    }
  }

  //-----------------------------------------------------------------------------
  template < class TCoordinates >
  void DrawCellsPoints( vtkPolyData* theInput,
                        vtkPoints* thePoints,
                        TColorFunctorBase* theColorFunctor,
                        TVertex* theVertexArr )
  {
    vtkIdType aCellId = 0, aVertexId = 0;

    TCoordinates* aStartPoints = (TCoordinates*)thePoints->GetVoidPointer(0);

    if ( vtkCellArray* aCellArray = theInput->GetVerts() )
      DrawPoints( aStartPoints, aCellArray, theColorFunctor, theVertexArr, aCellId, aVertexId );
  
    if ( vtkCellArray* aCellArray = theInput->GetLines() )
      DrawPoints( aStartPoints, aCellArray, theColorFunctor, theVertexArr, aCellId, aVertexId );
  
    if ( vtkCellArray* aCellArray = theInput->GetPolys() )
      DrawPoints( aStartPoints, aCellArray, theColorFunctor, theVertexArr, aCellId, aVertexId );
  
    if ( vtkCellArray* aCellArray = theInput->GetStrips() )
      DrawPoints( aStartPoints, aCellArray, theColorFunctor, theVertexArr, aCellId, aVertexId ); 
  }
} // namespace VTK

//-----------------------------------------------------------------------------
int VTKViewer_PolyDataMapper::Draw( vtkRenderer* ren, vtkActor* act )
{
  if( !this->MarkerEnabled || this->MarkerType == VTK::MT_NONE || !this->ImageData.GetPointer() )
    return MAPPER_SUPERCLASS::Draw( ren, act );

  vtkUnsignedCharArray* colors = NULL;
  vtkPolyData* input  = this->GetInput();
  vtkPoints* points;
  int noAbort = 1;
  int cellScalars = 0;
  vtkProperty* prop = act->GetProperty();

  points = input->GetPoints();

  if ( this->Colors )
  {
    colors = this->Colors;
    if ( (this->ScalarMode == VTK_SCALAR_MODE_USE_CELL_DATA ||
          this->ScalarMode == VTK_SCALAR_MODE_USE_CELL_FIELD_DATA ||
          !input->GetPointData()->GetScalars() )
         && this->ScalarMode != VTK_SCALAR_MODE_USE_POINT_FIELD_DATA)
      cellScalars = 1;
  }

  {
    vtkIdType aTotalConnectivitySize = 0;

    if ( vtkCellArray* aCellArray = input->GetVerts() )
      aTotalConnectivitySize += aCellArray->GetNumberOfConnectivityEntries() - aCellArray->GetNumberOfCells();

    if ( vtkCellArray* aCellArray = input->GetLines() )
      aTotalConnectivitySize += aCellArray->GetNumberOfConnectivityEntries() - aCellArray->GetNumberOfCells();

    if ( vtkCellArray* aCellArray = input->GetPolys() )
      aTotalConnectivitySize += aCellArray->GetNumberOfConnectivityEntries() - aCellArray->GetNumberOfCells();

    if ( vtkCellArray* aCellArray = input->GetStrips() )
      aTotalConnectivitySize += aCellArray->GetNumberOfConnectivityEntries() - aCellArray->GetNumberOfCells();

    if ( aTotalConnectivitySize > 0 ) {
      VTK::TVertex* aVertexArr = new VTK::TVertex[ aTotalConnectivitySize ];

      int* aSize = this->ImageData->GetDimensions();
      glPointSize( std::max( aSize[0], aSize[1] ) );

      int aMode = 0; // to remove
      {
        VTK::TColorFunctorBase* aColorFunctor = NULL;
        if( colors && aMode != 1 ) {
          if ( cellScalars )
            aColorFunctor = new VTK::TCellColors2Color( prop, colors );
          else
            aColorFunctor = new VTK::TPointColors2Color( prop, colors );
        } else {
          aColorFunctor = new VTK::TPropertyColor( prop );
        }
        if ( points->GetDataType() == VTK_FLOAT )
          VTK::DrawCellsPoints< float >( input, points, aColorFunctor, aVertexArr );
        else
          VTK::DrawCellsPoints< double >( input, points, aColorFunctor, aVertexArr );

        delete aColorFunctor;
      }

      if( this->ExtensionsInitialized == ES_Ok ) {
        GLuint aBufferObjectID = 0;
        vglGenBuffersARB( 1, &aBufferObjectID );
        vglBindBufferARB( GL_ARRAY_BUFFER_ARB, aBufferObjectID );
        
        int anArrayObjectSize = sizeof( VTK::TVertex ) * aTotalConnectivitySize;
        vglBufferDataARB( GL_ARRAY_BUFFER_ARB, anArrayObjectSize, aVertexArr, GL_STATIC_DRAW_ARB );
        
        delete [] aVertexArr;
        
        vglBindBufferARB( GL_ARRAY_BUFFER_ARB, 0 );
        vglBindBufferARB( GL_ARRAY_BUFFER_ARB, aBufferObjectID );
        
        glColorPointer( 4, GL_FLOAT, sizeof(VTK::TVertex), (void*)0 );
        glVertexPointer( 3, GL_FLOAT, sizeof(VTK::TVertex), (void*)(4*sizeof(GLfloat)) );
        
        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_COLOR_ARRAY );
        
        glDrawArrays( GL_POINTS, 0, aTotalConnectivitySize );
        
        glDisableClientState( GL_COLOR_ARRAY );
        glDisableClientState( GL_VERTEX_ARRAY );
        
        vglDeleteBuffersARB( 1, &aBufferObjectID );
      } else { // there are no extensions
        glColorPointer( 4, GL_FLOAT, sizeof(VTK::TVertex), aVertexArr );
        glVertexPointer( 3, GL_FLOAT, sizeof(VTK::TVertex), 
                         (void*)((GLfloat*)((void*)(aVertexArr)) + 4));

        glEnableClientState( GL_VERTEX_ARRAY );
        glEnableClientState( GL_COLOR_ARRAY );
        
        glDrawArrays( GL_POINTS, 0, aTotalConnectivitySize );
        
        glDisableClientState( GL_COLOR_ARRAY );
        glDisableClientState( GL_VERTEX_ARRAY );

        delete [] aVertexArr;
      }
    }
  }

  this->UpdateProgress(1.0);
  return noAbort;
}
