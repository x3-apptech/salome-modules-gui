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

#ifdef WIN32
#include <windows.h>
#endif

#include <GL/gl.h>

#include "OCCViewer_Trihedron.h"


#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include <Graphic3d_AspectFillArea3d.hxx>
#include <Graphic3d_Group.hxx>
#include <Graphic3d_MaterialAspect.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_Root.hxx>
#include <PrsMgr_PresentationManager3d.hxx>


#ifndef WIN32
# include <GL/glx.h>
#endif



#define PI   3.14159265359

OCCT_IMPLEMENT_STANDARD_RTTIEXT( OCCViewer_Trihedron, AIS_InteractiveObject )

/*!
 * Constructor
 */
OCCViewer_Trihedron::OCCViewer_Trihedron( const PrsMgr_TypeOfPresentation3d t )
  : AIS_InteractiveObject( t ),
    myTextList( 0 )
{
}

/*!
 * Destructor
 */
OCCViewer_Trihedron::~OCCViewer_Trihedron()
{
  if( myTextList )
    glDeleteLists( myTextList, 256 );
}

/*!
 * Sets the bounding box (MinMax values).
 */
#if OCC_VERSION_LARGE > 0x06070100 // for OCC-6.7.2 and higher version
void OCCViewer_Trihedron::bounds( Graphic3d_BndBox4f& theMinMax ) const
{
  Graphic3d_Vec4 aMinPt (-1.f, -1.f, -1.f, 1.f);
  Graphic3d_Vec4 aMaxPt (1.f, 1.f, 1.f, 1.f);

  theMinMax.Add (aMinPt);
  theMinMax.Add (aMaxPt);
}
#else
void OCCViewer_Trihedron::bounds( Graphic3d_CBounds& aMinMax ) const
{
  Standard_Real aXMin = -1, aYMin = -1, aZMin = -1;
  Standard_Real aXMax =  1, aYMax =  1, aZMax =  1;

  if( aMinMax.XMin > aXMin ) aMinMax.XMin = aXMin;
  if( aMinMax.YMin > aYMin ) aMinMax.YMin = aYMin;
  if( aMinMax.ZMin > aZMin ) aMinMax.ZMin = aZMin;
  if( aMinMax.XMax < aXMax ) aMinMax.XMax = aXMax;
  if( aMinMax.YMax < aYMax ) aMinMax.YMax = aYMax;
  if( aMinMax.ZMax < aZMax ) aMinMax.ZMax = aZMax;
}
#endif

/*!
 * Redefined method. Calculates the object presentation.
 */
void OCCViewer_Trihedron::Compute( const Handle( PrsMgr_PresentationManager3d )&,
                                   const Handle( Prs3d_Presentation )& aPrs,
                                   const Standard_Integer aMode )
{
  if( aPrs.IsNull() )
    return;

  Handle( Graphic3d_Group ) aGroup = Prs3d_Root::CurrentGroup( aPrs );
  aGroup->UserDraw( this );
}

/*!
 * Redefined method. Does nothing.
 */
void OCCViewer_Trihedron::ComputeSelection( const Handle( SelectMgr_Selection )& theSelection,
                                            const Standard_Integer theMode )
{
}

/*!
 * Generates text list.
 */
GLuint generateTextList()
{
  bool ok = false;
  GLuint aList = glGenLists( 256 );

#ifdef WIN32
  HGLRC hglrc = wglGetCurrentContext();
  if( hglrc )
  {
    HDC hdc = ::wglGetCurrentDC();
    HFONT font = CreateFont( -12, 0, 0, 0, 
                             FW_BOLD,
                             0, 0, 0,
                             ANSI_CHARSET,
                             OUT_TT_PRECIS,
                             CLIP_DEFAULT_PRECIS,
                             ANTIALIASED_QUALITY,
                             FF_DONTCARE | DEFAULT_PITCH,
                             "Courier New" );
    HFONT oldFont = (HFONT)SelectObject( hdc, font );

    ok = ::wglUseFontBitmaps( hdc, 0, 256, aList );
    SelectObject( hdc, oldFont );
    DeleteObject( font );
  }
#else // X Window
  Display* dpy = glXGetCurrentDisplay();
  if( dpy )
  {
    int aFontCount = 0;
    char** aFontList = XListFonts( dpy, "*-courier-*", 1, &aFontCount  );
    if( aFontCount > 0 )
    {
      //XFontStruct* fontInfo = XLoadQueryFont( dpy, "-*-courier-*-r-*-*-14-*-*-*-m-*-*-*" );
      Font aFont = XLoadFont( dpy, aFontList[0] );
      glXUseXFont( aFont, 0, 256, aList );
      XUnloadFont( dpy, aFont );
      ok = true;
    }
    XFreeFontNames( aFontList );
  }
#endif

  if( !ok )
    glDeleteLists( aList, 256 );

  return aList;
}

/*!
 * Draws text string.
 */
void drawText( GLuint theTextList, const char* theText, GLdouble thePosition[3], GLfloat theColor[3] )
{
  glColor3fv( theColor );
  glRasterPos3dv( thePosition );
  glListBase( theTextList );
  glCallLists( strlen( theText ), GL_UNSIGNED_BYTE, (GLubyte*)theText );
}

/*!
 * Displays trihedron.
 */
void OCCViewer_Trihedron::display()
{
  GLdouble TriedronOrigin[3] = { 0.0, 0.0, 0.0 };

  GLdouble TriedronAxeX[3] = { 1.0, 0.0, 0.0 };
  GLdouble TriedronAxeY[3] = { 0.0, 1.0, 0.0 };
  GLdouble TriedronAxeZ[3] = { 0.0, 0.0, 1.0 };

  GLfloat TriedronColorX[3] = { 1.0, 0.0, 0.0 };
  GLfloat TriedronColorY[3] = { 0.0, 1.0, 0.0 };
  GLfloat TriedronColorZ[3] = { 0.0, 0.0, 1.0 };

  GLfloat TriedronLetterColorX[3] = { 1.0, 1.0, 1.0 };
  GLfloat TriedronLetterColorY[3] = { 1.0, 1.0, 1.0 };
  GLfloat TriedronLetterColorZ[3] = { 1.0, 1.0, 1.0 };

  GLfloat TriedronLineWidth = 2.0;
  GLdouble TriedronScale = 0.15;

  GLdouble U, V, minUV;
  GLint aViewPort[4];
  glGetIntegerv(GL_VIEWPORT, aViewPort);
  U = aViewPort[2];
  V = aViewPort[3];

  if( U < V )
    minUV = U;
  else
    minUV = V;

  GLdouble L = minUV * TriedronScale;

  TriedronOrigin[0]= 0.0; 
  TriedronOrigin[1]= 0.0;
  TriedronOrigin[2]= 0.0; 

  TriedronAxeX[0] = TriedronOrigin[0] + L;
  TriedronAxeX[1] = TriedronOrigin[1] + 0.0;
  TriedronAxeX[2] = TriedronOrigin[2] + 0.0;

  TriedronAxeY[0] = TriedronOrigin[0] + 0.0;
  TriedronAxeY[1] = TriedronOrigin[1] + L;
  TriedronAxeY[2] = TriedronOrigin[2] + 0.0;

  TriedronAxeZ[0] = TriedronOrigin[0] + 0.0;
  TriedronAxeZ[1] = TriedronOrigin[1] + 0.0;
  TriedronAxeZ[2] = TriedronOrigin[2] + L;

  glPushAttrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT | GL_LINE_BIT | GL_VIEWPORT_BIT );

  glDepthRange( 0, 0 );
  glDisable( GL_LIGHTING );
  glDisable( GL_COLOR_MATERIAL );
  for( int i = 0; i < GL_MAX_CLIP_PLANES; i++ )
    glDisable( GL_CLIP_PLANE0 + i  );

  glLineWidth( TriedronLineWidth );

  glColor3fv( TriedronColorX );
  glBegin( GL_LINES );
  glVertex3dv( TriedronOrigin );
  glVertex3dv( TriedronAxeX );
  glEnd();

  glColor3fv( TriedronColorY );
  glBegin( GL_LINES );
  glVertex3dv( TriedronOrigin );
  glVertex3dv( TriedronAxeY );
  glEnd();

  glColor3fv( TriedronColorZ );
  glBegin( GL_LINES );
  glVertex3dv( TriedronOrigin );
  glVertex3dv( TriedronAxeZ );
  glEnd();

  GLdouble l = L - L/4.;
  GLdouble rayon = L/20.;
  GLint ii, NbFacettes = 12;
  GLdouble Angle = 2. * PI / NbFacettes;
  GLdouble TriedronCoord[3] = { 1.0, 0.0, 0.0 };

  if( myTextList == 0 )
    myTextList = generateTextList();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glColor3fv( TriedronColorX );
  glBegin( GL_TRIANGLE_FAN );
  glVertex3dv( TriedronAxeX );
  TriedronCoord[0] = TriedronOrigin[0] + l ;
  ii = NbFacettes;
  while (ii >= 0 ) {
    TriedronCoord[1] = TriedronOrigin[1] + ( rayon * sin(ii * Angle) );
    TriedronCoord[2] = TriedronOrigin[2] + ( rayon * cos(ii * Angle) );
    glVertex3dv( TriedronCoord );
    ii--;
  }
  glEnd();

  drawText( myTextList, "X", TriedronAxeX, TriedronLetterColorX );

  glColor3fv( TriedronColorY );
  glBegin( GL_TRIANGLE_FAN );
  glVertex3dv( TriedronAxeY );
  TriedronCoord[1] = TriedronOrigin[1] + l ;
  ii = NbFacettes;
  while (ii >= 0 ) {
    TriedronCoord[0] = TriedronOrigin[0] + (rayon * cos(ii * Angle) );
    TriedronCoord[2] = TriedronOrigin[2] + (rayon * sin(ii * Angle) );
    glVertex3dv( TriedronCoord );
    ii--;
  }
  glEnd();

  drawText( myTextList, "Y", TriedronAxeY, TriedronLetterColorY );

  glColor3fv( TriedronColorZ );
  glBegin( GL_TRIANGLE_FAN );
  glVertex3dv( TriedronAxeZ );
  TriedronCoord[2] = TriedronOrigin[2] + l ;
  ii = NbFacettes;
  while (ii >= 0 ) {
    TriedronCoord[0] = TriedronOrigin[0] + ( rayon * sin(ii * Angle) );
    TriedronCoord[1] = TriedronOrigin[1] + ( rayon * cos(ii * Angle) );
    glVertex3dv( TriedronCoord );
    ii--;
  }
  glEnd();

  drawText( myTextList, "Z", TriedronAxeZ, TriedronLetterColorZ );

  glPopAttrib();
}
