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

//  Author : OPEN CASCADE
// File:      GLViewer_Drawer.h
// Created:   November, 2004
//
#ifndef GLVIEWER_DRAWER_H
#define GLVIEWER_DRAWER_H

#ifdef WIN32
#include "windows.h"
#endif

#include <QColor>
#include <QFont>

class QFile;

#include <GL/gl.h>

#include "GLViewer.h"
#include "GLViewer_Defs.h"
#include "GLViewer_Geom.h"

class GLViewer_Object;
class GLViewer_Rect;
class GLViewer_CoordSystem;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif
/*! 
 * Struct GLViewer_TexIdStored
 * Structure for store information about texture
 */
struct GLVIEWER_API GLViewer_TexIdStored
{
  //! Texture ID
  GLuint      myTexFontId;
  //! Texture width
  int         myTexFontWidth;
  //! texture height
  int         myTexFontHeight;
};

/*! 
 * Struct GLViewer_TexFindId
 * Structure for srorage information about texture font
 */
struct GLVIEWER_API GLViewer_TexFindId
{
  //! Font family description
  QString     myFontFamily;
  //! Bold parameter
  bool        myIsBold;
  //! Italic parameter
  bool        myIsItal;
  //! Underline parameter
  bool        myIsUndl;
  //! Font Size
  int         myPointSize;
  //! View POrt ID
  int         myViewPortId;
  //! Overloaded operator for using struct as MAP key
  bool operator < (const GLViewer_TexFindId theStruct) const 
  { 
    if ( myViewPortId != theStruct.myViewPortId )
      return myViewPortId < theStruct.myViewPortId;
    else if ( myPointSize != theStruct.myPointSize )
      return myPointSize < theStruct.myPointSize;
    else if ( myIsBold != theStruct.myIsBold )
      return myIsBold < theStruct.myIsBold;
    else if ( myIsItal != theStruct.myIsItal )
      return myIsItal < theStruct.myIsItal;
    else if ( myIsUndl != theStruct.myIsUndl )
      return myIsUndl < theStruct.myIsUndl;
    else
      return myFontFamily < theStruct.myFontFamily;
  }
};

/*!
  \class GLViewer_TexFont
  Font for GLViewer_Drawer, Drawing bitmap and texture fonts in GLViewer
*/

class GLVIEWER_API GLViewer_TexFont
{
public:
  //! A default constructor
  GLViewer_TexFont();
  //! A constructor
  /*
  * \param theFont         - a base font
  * \param theSeparator    - separator between letters
  * \param theIsResizeable - specifies whether text drawn by this object can be scaled along with the scene
  * \param theMinMagFilter - min/mag filter, affects text sharpness
  */
  GLViewer_TexFont( QFont* theFont, 
                    int theSeparator = 2, 
                    bool theIsResizeable = false, 
                    GLuint theMinMagFilter = GL_LINEAR/*_ATTENUATION*/ );
  //! A destructor
  ~GLViewer_TexFont();
  
  //! Generating font texture
  bool            generateTexture();
  //! Drawing string theStr in point with coords theX and theY
  void            drawString( QString  theStr,
                              GLdouble theX = 0.0,
                              GLdouble theY = 0.0,
                              GLfloat  theScale = 1.0 );
  
  //! Returns separator between letters
  int             getSeparator(){ return mySeparator; }
  //! Installing separator between letters
  void            setSeparator( int theSeparator ){ mySeparator = theSeparator; }
  
  //! Returns width of string in pixels
  int             getStringWidth( QString theString );
  //! Returns height of string in pixels
  int             getStringHeight();
  
  //! Clears all generated fonts
  static void     clearTextBases();

  //! Map for strorage generated texture fonts
  static QMap<GLViewer_TexFindId,GLViewer_TexIdStored> TexFontBase;
  //! Map for strorage generated bitmaps fonts
  static QMap<GLViewer_TexFindId,GLuint>               BitmapFontCache;

private:
  //! Initializes font parameters
  void            init();
  
private:
  //! Number of characters in the font texture
  int             myNbSymbols;
  //! Array of letter width
  int*            myWidths;
  //! Array of letter positions in texture
  int*            myPositions;
  //! Pointer to base font
  QFont           myQFont;
  //! Font texture ID
  GLuint          myTexFont;
  //! Font texture width
  int             myTexFontWidth;
  //! Font texture height
  int             myTexFontHeight;
  //! Separator between letters
  int             mySeparator;
  //! Flag controlling scalability of this texmapped font
  bool            myIsResizeable;
  //! Min/mag filter
  GLuint          myMinMagFilter;
  //! Font height
  int             myFontHeight;
  //! Diagnostic information
  int             myMaxRowWidth;
};

/*! 
  \class GLViewer_Drawer
  Drawer for GLViewer_Objects.
  Drawer creates only one times per one type of object
*/
class GLVIEWER_API GLViewer_Drawer
{
public:
  //! Text position relatively object
  enum
  {
    GLText_Center = 0,
    GLText_Left,
    GLText_Right,
    GLText_Top,
    GLText_Bottom
  };

  // Objects status ( needs for change colors )
  //enum ObjectStatus
  //{
  //  OS_Normal = 0,
  //  OS_Highlighted,
  //  OS_Selected
  //};
  
  // 
  //enum ClosedStatus
  //{
  //  CS_CLOSED = 0,
  //  CS_OPEN = 1
  //};  

  //! A constructor
  GLViewer_Drawer();
  //! A destructor
  virtual ~GLViewer_Drawer();
  
  //! Main method which drawing object in GLViewer
  /*
  *\param xScale - current scale along X-direction
  *\param yScale - current scale along Y-direction
  *\param onlyUpdate - = true if only update highlight-select information
  */
  virtual void                    create( float xScale, float yScale, bool onlyUpdate ) = 0;  
  
  //! Adds object to drawer display list
  virtual void                    addObject( GLViewer_Object* theObject ){ myObjects.append( theObject ); }
  //! Clears drawer display list
  virtual void                    clear(){ myObjects.clear(); }
  
  //! Returns object type (needs for dynamic search of right drawer ) 
  QString                         getObjectType() const { return myObjectType; }

  //! Returns object priority
  int                             getPriority() const { return myPriority; }

        //! The function enables and disables antialiasing in Open GL (for points, lines and polygons).
        void                            setAntialiasing(const bool on);
  
  //! Clears all generated textures
  static void                     destroyAllTextures();
  
  //! A function translate object in to HPGL file on disk
  /*!
   *\param hFile     the name of PostScript file chosen by user
   *\param aViewerCS the GLViewer_CoordSystem of window
   *\param aHPGLCS   the GLViewer_CoordSystem of PostScript page
  */
  virtual bool                    translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );
  
  //! A function translate object in to PostScript file on disk
  /*!
   *\param hFile     the name of PostScript file chosen by user
   *\param aViewerCS the GLViewer_CoordSystem of window
   *\param aPSCS     the GLViewer_CoordSystem of PostScript page
  */
  virtual bool                    translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS ); 
  
#ifdef WIN32
  //! A function translate object in to EMF file on disk
  /*!
   *\warning WIN32 only
   *
   *\param dc        the name of HDC associated with file chosen by user
   *\param aViewerCS the GLViewer_CoordSystem of window
   *\param aEMFCS    the GLViewer_CoordSystem of EMF page
  */
  virtual bool                    translateToEMF( HDC hDC, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif
  
  //! Loads texture from file
  /*!
   *\param fileName - the name of texture file
   *\param x_size   - the horizontal size of picture ( less or equal texture horizontal size )
   *\param y_size   - the vertical size of picture ( less or equal texture vertical size )
   *\param t_size   - the size of texture ( texture vertical size equals texture horizontal size )
  */
  static GLuint                   loadTexture( const QString& fileName,
                                               GLint* x_size = 0,
                                               GLint* y_size = 0,
                                               GLint* t_size = 0);

  //! Draw square texture
  /*!
   *\param texture - the texture ID
   *\param size    - the size of square texture
   *\param x       - x coord
   *\param y       - y coord
  */
  void                            drawTexture( GLuint texture,
                                               GLint size,
                                               GLfloat x,
                                               GLfloat y );

  //! Draw texture
  /*!
   *\param texture - the texture ID
   *\param x_size  - the horizontal size of texture
   *\param y_size  - the vertical size of texture
   *\param x       - x coord
   *\param y       - y coord
  */
  void                            drawTexture( GLuint texture,
                                               GLint x_size,
                                               GLint y_size,
                                               GLfloat x,
                                               GLfloat y );

  //! Draw texture part
  /*!
   *\param texture - the texture ID
   *\param x_ratio - the horizontal ratio of texture part
   *\param y_ratio - the vertical ratio of texture part
   *\param x_size  - the horizontal size of texture
   *\param y_size  - the vertical size of texture
   *\param x       - x coord
   *\param y       - y coord
   *\param scale   - common scale factor ( if = 0, use drawer scales )
  */
  void                            drawTexturePart( GLuint texture,
                                                   GLfloat x_ratio,
                                                   GLfloat y_ratio,
                                                   GLfloat x_size,
                                                   GLfloat y_size,
                                                   GLfloat x,
                                                   GLfloat y,
                                                   GLfloat scale = 0 );



  //! Draw text string
  /*!
   *\param text              - the text string
   *\param xPos              - x coord
   *\param yPos              - y coord
   *\param color             - text color
   *\param aFont             - base font of text
   *\param theSeparator      - letter separator
   *\param DisplayTextFormat - text format
  */
  void                            drawText( const QString& text,
                                            GLfloat xPos,
                                                                          GLfloat yPos,
                                            const QColor& color,
                                            QFont* aFont,
                                            int theSeparator,
                                            DisplayTextFormat = DTF_BITMAP );

  //! Draw text string
  /*!
   *\param text      - the text string
   *\param x         - x coord
   *\param y         - y coord
   *\param hPosition - horizontal alignment
   *\param vPosition - vertical alignment
   *\param color     - text color
   *\param smallFont - font format
  */
  void                            drawGLText( QString text,
                                                                            float x,
                                                                            float y,
                                              int hPosition = GLText_Center,
                                              int vPosition = GLText_Center,
                                              QColor color = Qt::black,
                                              bool smallFont = false );

  //! Sets a default font to be used by drawGLText method
  /*!
   *\param font      - the default font
  */
  inline void                     setFont( const QFont& font ) { myFont = font; }

  //! Returns a default font used by drawGLText method
  inline QFont                    font() const { return myFont; }

  //! Sets a default text displaying format to be used by drawGLText method
  /*!
   *\param format    - the default text displaying format
  */
  inline void                     setTextFormat( const DisplayTextFormat format ) { myTextFormat = format; }

  //! Returns a default text displaying format used by drawGLText method
  inline DisplayTextFormat        textFormat() const { return myTextFormat; }

  //! Sets a text string displaying scale factor (used only with text format DTF_TEXTURE_SCALABLE)
  /*!
   *\param factor    - scale factor
  */
  inline void                     setTextScale( const GLfloat factor ) { myTextScale = factor; }

  //! Returns a text string displaying scale factor
  inline GLfloat                  textScale() const { return myTextScale; }

  //! Returns a rectangle of text (without viewer scale)
  GLViewer_Rect                   textRect( const QString& ) const;


  //! Draw rectangle with predefined color
  static void                     drawRectangle( GLViewer_Rect* theRect, QColor = Qt::black );

protected:
  //! Draw basic primitives: rectangle, contour, polygon, vertex, cross, arrow
  //* with predefined color
  static void                     drawRectangle( GLViewer_Rect*, GLfloat, GLfloat = 0, QColor = Qt::black, 
                                                 bool = false, QColor = Qt::white );
  static void                     drawContour( GLViewer_Rect*, QColor, GLfloat, GLushort, bool );
  static void                     drawContour( const GLViewer_PntList&, QColor, GLfloat );
  static void                     drawPolygon( GLViewer_Rect*, QColor, GLushort, bool );
  static void                     drawPolygon( const GLViewer_PntList&, QColor );
  static void                     drawVertex( GLfloat, GLfloat, QColor );
  static void                     drawCross( GLfloat, GLfloat, QColor );
  static void                     drawArrow( const GLfloat red, const GLfloat green, const GLfloat blue,
                                             GLfloat, GLfloat, GLfloat, GLfloat,
                                             GLfloat, GLfloat, GLfloat, GLboolean = GL_FALSE );

  //! Draw object text
  virtual void                    drawText( GLViewer_Object* theObject );

  //! X Scale factor
  float                           myXScale;
  //! Y scale factor
  float                           myYScale;
  
  //! List of objects
  QList<GLViewer_Object*>    myObjects;
  //! List generated textures
  GLuint                          myTextList;
  
  //! Type of supporting object
  QString                         myObjectType;
  //! Dislay priority
  int                             myPriority;

  //! Default font for drawGLText() method
  QFont                           myFont;
  //! Default text displaying format for drawGLText() method
  DisplayTextFormat               myTextFormat;

  //! Scale factor for text string draw, by default 0.125
  //! (used only with text format DTF_TEXTURE_SCALABLE)
  GLfloat                         myTextScale;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif // GLVIEWER_DRAWER_H
