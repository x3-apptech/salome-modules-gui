// File:      GLViewer_Object.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_OBJECT_H
#define GLVIEWER_OBJECT_H

#ifdef WNT
#include <windows.h>
#endif

#include "GLViewer.h"
#include "GLViewer_Defs.h"

#include <GL/gl.h>

#include <qfont.h>
#include <qobject.h>
#include <qstring.h>
#include <qcolor.h>
#include <qmime.h>
#include <qrect.h>
#include <qvaluelist.h>

#include <TColStd_SequenceOfInteger.hxx>

class QFile;

#ifdef WNT
#pragma warning( disable:4251 )
#endif

class GLVIEWER_EXPORT GLViewer_Pnt
{
public:
  GLViewer_Pnt() : myX( 0. ), myY( 0. ) {}
  GLViewer_Pnt( GLfloat theX, GLfloat theY ) : myX( theX ), myY( theY ) {}
  
  GLfloat x() const { return myX; }
  GLfloat y() const { return myY; }
  void    setX( GLfloat theX ) { myX = theX; }
  void    setY( GLfloat theY ) { myY = theY; }
  void    setXY( GLfloat theX, GLfloat theY ) { myX = theX; myY = theY; }
  void    move( GLfloat theDX, GLfloat theDY ) { myX += theDX; myY += theDY; }
  
private:
  GLfloat myX;
  GLfloat myY;
};

typedef QValueList<GLViewer_Pnt> GLViewer_PntList;

/***************************************************************************
**  Class:   GLViewer_Rect
**  Descr:   Substitution of QRect for OpenGL
**  Module:  GLViewer
**  Created: UI team, 19.04.04
****************************************************************************/
class GLVIEWER_EXPORT GLViewer_Rect
{
public:
  GLViewer_Rect(): myLeft(0.0), myRight(0.0), myTop(0.0), myBottom(0.0){}
  GLViewer_Rect( float theLeft, float theRight, float theTop, float theBottom )
    : myLeft(theLeft), myRight(theRight), myTop(theTop), myBottom(theBottom) {}
  GLViewer_Rect( QRect theRect ) {
    myLeft = ( float )theRect.left(); myRight = ( float )theRect.right();
    myTop = ( float )theRect.top(); myBottom = ( float )theRect.bottom(); }
  
  
  float       left() const { return myLeft; }
  float       right() const { return myRight; }
  float       top() const { return myTop; }
  float       bottom() const { return myBottom; }
  
  void        setLeft( float theLeft ) { myLeft = theLeft; }
  void        setRight( float theRight ) { myRight = theRight; }
  void        setTop( float theTop ) { myTop = theTop; }
  void        setBottom( float theBottom ) { myBottom = theBottom; }
  
  void        setCoords( float theLeft, float theRight, float theTop, float theBottom )
    { myLeft = theLeft; myRight = theRight; myTop = theTop; myBottom = theBottom; }

  QRect*      toQRect() { return new QRect( ( int )myLeft, ( int )myBottom,
					    ( int )( myRight - myLeft ), ( int )( myTop - myBottom ) ); }

  bool        isNull() const { return myLeft == myRight || myTop == myBottom; }
  bool        isValid() const { return !( myLeft > myRight || myTop < myBottom ); }
  
  bool        contains( GLViewer_Pnt pnt ) { return ( pnt.x() > left() &&
    pnt.x() < right() &&
    pnt.y() > top() &&
    pnt.y() < bottom() ); } 
  
protected:
  float       myLeft;
  float       myRight;
  float       myTop;
  float       myBottom;
};

/***************************************************************************
**  Class:   GLViewer_Text
**  Descr:   Substitution of Prs3d_Text for OpenGL
**  Module:  GLViewer
**  Created: UI team, 10.07.03
****************************************************************************/
class GLVIEWER_EXPORT GLViewer_Text
{
public:
  GLViewer_Text( const QString&, float xPos = 0.0, float yPos = 0.0, const QColor& color = QColor( 0, 255, 0 ) );
  GLViewer_Text( const QString&, float xPos, float yPos, const QColor& , QFont, int );
  ~GLViewer_Text();
  
  void                  setText( const QString& text ) { myText = text; }
  QString               getText() const { return myText; }
  
  void                  setPosition( float xPos, float yPos ) { myXPos = xPos; myYPos = yPos; }
  void                  getPosition( float& xPos, float& yPos ) { xPos = myXPos; yPos = myYPos; }
  
  void                  setColor( const QColor& color ) { myColor = color; }
  QColor                getColor() const { return myColor; }
  
  void                  setFont( const QFont theQFont) { myQFont = theQFont; }
  QFont                 getFont() const { return myQFont; }
  
  int                   getSeparator(){ return mySeparator; }
  void                  setSeparator( int theSep ){ mySeparator = theSep; }
  
  int                   getWidth();
  int                   getHeight();
  
  QByteArray            getByteCopy() const;
  
  static GLViewer_Text* fromByteCopy( QByteArray );
  
  DisplayTextFormat     getDisplayTextFormat() const { return myDTF; }
  void                  setTextDisplayFormat( DisplayTextFormat theDTF ) { myDTF = theDTF; }
  
protected:
  QString            myText;
  float              myXPos;
  float              myYPos;
  QColor             myColor;
  QFont              myQFont;
  int                mySeparator;
  DisplayTextFormat  myDTF;
};

/***************************************************************************
**  Class:   GLViewer_CoordSystem
**  Descr:   
**  Module:  GLViewer
**  Created: UI team, 02.09.02
****************************************************************************/
class GLVIEWER_EXPORT GLViewer_CoordSystem
{
public:
  enum CSType { Cartesian, Polar };
  
private:
  double myX0, myY0;       //The coordinates of origin in the reference CS
  double myXUnit, myYUnit; //The lengths of axis units in the reference unit
  double myRotation;       //The rotation in radians relative to reference CS
  
  //!!! In the polar CS myYUnit is ignored, but myXUnit is the unit of polar radius
  
  CSType myType;
  
public:
  GLViewer_CoordSystem( CSType aType, double X0 = 0.0, double Y0 = 0.0, 
			double XUnit = 1.0, double YUnit = 1.0, 
			double Rotation = 0.0 );
  
  void getOrigin( double& x, double& y ) const;
  void setOrigin( double x, double y );
  
  void getUnits( double& x, double& y ) const;
  void setUnits( double x, double y );
  
  double getRotation() const;
  void   setRotation( double rotation );
  
  CSType getType() const;
  void setType( CSType type );
  
  void transform( GLViewer_CoordSystem& aSystem, double& x, double& y );
  //Transform the coordinates x, y from current CS to aSystem
  
  virtual void getStretching( GLViewer_CoordSystem& aSystem, double& theX, double& theY );
  //Return how many times line width in aSystem system bigger than in current
  
protected:
  virtual void toReference( double& x, double& y );
  virtual void fromReference( double& x, double& y );
};

/***************************************************************************
**  Class:   GLViewer_Object
**  Descr:   OpenGL Object
**  Module:  GLViewer
**  Created: UI team, 02.09.02
****************************************************************************/
#ifdef WNT
#include <windows.h>
#endif

class GLViewer_Drawer;
class GLViewer_AspectLine;
class GLViewer_Owner;
class GLViewer_Group;

class GLVIEWER_EXPORT GLViewer_Object : public QObject
{
    Q_OBJECT

public:
    GLViewer_Object();
    virtual ~GLViewer_Object();
  
    virtual void              compute() = 0;
    virtual GLViewer_Drawer*  createDrawer() = 0;
    GLViewer_Drawer*          getDrawer(){ return myDrawer; }
    virtual void              setDrawer( GLViewer_Drawer* theDrawer ) { myDrawer = theDrawer; }
    
    virtual GLboolean         highlight( GLfloat x, GLfloat y, GLfloat tol, GLboolean isCircle = GL_FALSE ) = 0;
    virtual GLboolean         unhighlight() = 0;
    virtual GLboolean         select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect, GLboolean isFull = GL_FALSE,
				      GLboolean isCircle = GL_FALSE, GLboolean isShift = GL_FALSE ) = 0;
    virtual GLboolean         unselect() = 0;
    
    virtual GLboolean         isInside( GLViewer_Rect );
    
    virtual bool              portContains( GLViewer_Pnt ) { return false; }
    virtual bool              startPulling( GLViewer_Pnt ) { return false; }
    virtual void              pull( GLViewer_Pnt, GLViewer_Object* ) {}
    virtual void              finishPulling() {}
    virtual bool              isPulling() { return false; }
    
    virtual void              setRect( GLViewer_Rect* rect) { myRect = rect; }
    virtual GLViewer_Rect*    getRect() const { return myRect; }
    virtual GLViewer_Rect*    getUpdateRect() = 0;
    
    virtual void              setScale( GLfloat xScale, GLfloat yScale ) { myXScale = xScale; myYScale = yScale; }
    virtual void              getScale( GLfloat& xScale, GLfloat& yScale ) const { xScale = myXScale; yScale = myYScale;}
    
    virtual GLboolean         setZoom( GLfloat zoom, GLboolean = GL_FALSE );
    virtual GLfloat           getZoom() const { return myZoom; }
    virtual GLboolean         updateZoom( bool zoomIn );
    
    virtual GLboolean         isHighlighted() const { return myIsHigh; }
    virtual GLboolean         isSelected() const { return myIsSel; }  
    virtual void              setSelected( GLboolean state ) { myIsSel = state; }
    
    void                      setGLText( GLViewer_Text* glText ) { myGLText = glText; }
    GLViewer_Text*            getGLText() const { return myGLText; }
    
    virtual void              setAspectLine ( GLViewer_AspectLine* aspect ) { myAspectLine = aspect; }
    virtual GLViewer_AspectLine* getAspectLine() const { return myAspectLine; }
    
    QString                   getObjectType() const { return myType; } 
    
    void                      setName( QString name ) { myName = name; } 
    QString                   getName() const { return myName; } 
    
    virtual void              moveObject( float, float, bool fromGroup = false ) = 0;
    virtual void              finishMove() {}
    
    bool                      getVisible(){ return myIsVisible; }
    void                      setVisible( bool theStatus ){ myIsVisible = theStatus; }
    
    void                      setToolTipText( QString str ){ myToolTipText = str; }
    virtual QString           getToolTipText(){ return myToolTipText; }
    
    bool                      isTooTipHTML() const { return isToolTipHTML; }
    void                      setToolTipFormat( bool isHTML ) { isToolTipHTML = isHTML; }
  
    virtual QByteArray        getByteCopy();
    virtual bool              initializeFromByteCopy( QByteArray );
    
    virtual bool              translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS ) = 0;
    virtual bool              translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS ) = 0;  
    
#ifdef WIN32
    virtual bool              translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS ) = 0;
#endif

    GLViewer_Owner*           owner() const;
    void                      setOwner( GLViewer_Owner* );
    
    void                      setGroup( GLViewer_Group* );
    GLViewer_Group*           getGroup() const;
    
protected:
    QString                   myName;
    QString                   myType;
    
    GLViewer_Rect*            myRect;
    GLfloat                   myXScale;
    GLfloat                   myYScale;
    GLfloat                   myXGap;
    GLfloat                   myYGap;
    
    GLfloat                   myZoom;

    GLboolean                 myIsHigh;
    GLboolean                 myIsSel;
    GLViewer_Text*            myGLText;

    GLViewer_Drawer*          myDrawer;
    GLViewer_AspectLine*      myAspectLine;
    QString                   myToolTipText;  
    bool                      isToolTipHTML;
    
    bool                      myIsVisible;
    GLViewer_Owner*           myOwner;
    GLViewer_Group*           myGroup;
};

/***************************************************************************
**  Class:   GLViewer_Owner
**  Descr:   
**  Module:  GLViewer
**  Created: UI team, 15.12.04
****************************************************************************/
#include <SUIT_DataOwner.h>

class GLVIEWER_EXPORT GLViewer_Owner: public SUIT_DataOwner
{
public:
  GLViewer_Owner():SUIT_DataOwner() {};
  ~GLViewer_Owner() {};
  
public:
  int                     getIndex() const { return myIndex; }
  
protected:
  int                     myIndex;
};

/***************************************************************************
**  Class:   GLViewer_MarkerSet
**  Descr:   OpenGL MarkerSet
**  Module:  GLViewer
**  Created: UI team, 03.09.02
****************************************************************************/
class GLVIEWER_EXPORT GLViewer_MarkerSet: public GLViewer_Object
{
  Q_OBJECT
    
public:
  GLViewer_MarkerSet( int number = 1, float size = 5.0, const QString& toolTip = "GLMarker" );
  ~GLViewer_MarkerSet();
  
  virtual void            compute();
  virtual GLViewer_Drawer* createDrawer();
  
  virtual GLboolean       highlight( GLfloat x, GLfloat y, GLfloat tol = 15.0, GLboolean isCircle = GL_FALSE );
  virtual GLboolean       unhighlight();
  virtual GLboolean       select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect, GLboolean isFull = GL_FALSE,
				  GLboolean isCircle = GL_FALSE, GLboolean isShift = GL_FALSE );
  virtual GLboolean       unselect();
  
  virtual GLViewer_Rect*  getUpdateRect();
  
  void                    setXCoord( GLfloat* xCoord, int size );
  void                    setYCoord( GLfloat* yCoord, int size );
  GLfloat*                getXCoord() const { return myXCoord; }
  GLfloat*                getYCoord() const { return myYCoord; }
  void                    setNumMarkers( GLint );
  GLint                   getNumMarkers() const { return myNumber; };
  
  void                    setMarkerSize( const float size ) { myMarkerSize = size; }
  float                   getMarkerSize() const { return myMarkerSize; }
  
  void                    exportNumbers( QValueList<int>&, QValueList<int>& , QValueList<int>&, QValueList<int>& );
  
  QValueList<int>         getSelectedElements() { return mySelNumbers; }
  bool                    addOrRemoveSelected( int index );
  void                    addSelected( const TColStd_SequenceOfInteger& );
  void                    setSelected( const TColStd_SequenceOfInteger& );
  
  virtual void            moveObject( float, float, bool fromGroup = false );
  
  virtual QByteArray      getByteCopy();
  virtual bool            initializeFromByteCopy( QByteArray );
  
  virtual bool            translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS );
  virtual bool            translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );    
  
#ifdef WIN32
  virtual bool            translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif
  
protected slots:
  void                    onSelectionDone( bool );
  void                    onSelectionCancel();
  
signals:
  void                    dvMarkersSelected( const TColStd_SequenceOfInteger& );
  
protected:
  GLint                   myNumber;
  GLfloat*                myXCoord;
  GLfloat*                myYCoord;    
  GLfloat                 myMarkerSize;
  QValueList<int>         myHNumbers;
  QValueList<int>         myUHNumbers;
  QValueList<int>         mySelNumbers;
  QValueList<int>         myCurSelNumbers;
  QValueList<int>         myUSelNumbers;
  QValueList<int>         myPrevHNumbers;
  TColStd_SequenceOfInteger mySelectedIndexes;
};

/***************************************************************************
**  Class:   GLViewer_Polyline
**  Descr:   OpenGL Polyline
**  Module:  GLViewer
**  Created: UI team, 03.09.02
****************************************************************************/
class GLVIEWER_EXPORT GLViewer_Polyline: public GLViewer_Object
{
  Q_OBJECT
    
public:
  GLViewer_Polyline( int number = 1, float size = 5.0, const QString& toolTip = "GLPolyline" );
  ~GLViewer_Polyline();
  
  virtual void            compute();
  virtual GLViewer_Drawer* createDrawer();
  
  virtual GLboolean       highlight( GLfloat x, GLfloat y, GLfloat tol = 15.0, GLboolean isCircle = GL_FALSE );
  virtual GLboolean       unhighlight();
  virtual GLboolean       select( GLfloat x, GLfloat y, GLfloat tol,  GLViewer_Rect rect, GLboolean isFull = GL_FALSE,
				  GLboolean isCircle = GL_FALSE, GLboolean isShift = GL_FALSE );
  virtual GLboolean       unselect();
  
  virtual GLViewer_Rect*  getUpdateRect();
  
  void                    setXCoord( GLfloat* xCoord, int size );
  void                    setYCoord( GLfloat* yCoord, int size );
  GLfloat*                getXCoord() const { return myXCoord; }
  GLfloat*                getYCoord() const { return myYCoord; }
  void                    setNumber( GLint );
  GLint                   getNumber() const { return myNumber; };
  
  void                    setClosed( GLboolean closed ) { myIsClosed = closed; }
  GLboolean               isClosed() const { return myIsClosed; }
  
  void                    setHighSelAll( GLboolean highSelAll ) { myHighSelAll = highSelAll; }
  GLboolean               isHighSelAll() const { return myHighSelAll; }
  
  void                    exportNumbers( QValueList<int>&, QValueList<int>& , QValueList<int>&, QValueList<int>& );
  
  QValueList<int>         getSelectedElements() { return mySelNumbers; }
  
  virtual void            moveObject( float, float, bool fromGroup = false );
  
  virtual QByteArray      getByteCopy();
  virtual bool            initializeFromByteCopy( QByteArray );
  
  virtual bool            translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS );
  virtual bool            translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );    
  
#ifdef WIN32
  virtual bool            translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif
  
protected slots:
  void                    onSelectionDone( bool );
  void                    onSelectionCancel();
  
protected:
  GLfloat*                myXCoord;
  GLfloat*                myYCoord;
  GLint                   myNumber;
  GLboolean               myIsClosed;
  GLboolean               myHighSelAll;
  
  QValueList<int>         myHNumbers;
  QValueList<int>         myUHNumbers;
  QValueList<int>         mySelNumbers;
  QValueList<int>         myUSelNumbers;
  QValueList<int>         myCurSelNumbers;
  QValueList<int>         myPrevHNumbers;
  TColStd_SequenceOfInteger mySelectedIndexes;
  
  GLboolean               myHighFlag;
};


/***************************************************************************
**  Class:   GLViewer_TextObject
**  Descr:   Text as Object for OpenGL
**  Module:  GLViewer
**  Created: UI team, 27.02.04
****************************************************************************/
class GLVIEWER_EXPORT GLViewer_TextObject : public GLViewer_Object
{
  Q_OBJECT
    
public:  
  GLViewer_TextObject( const QString&, float xPos = 0, float yPos = 0, 
		       const QColor& color = QColor( 0, 255, 0 ), const QString& toolTip = "GLText" );
  ~GLViewer_TextObject();
  
  virtual void              compute();
  virtual GLViewer_Drawer*  createDrawer();
  
  virtual void              setDrawer( GLViewer_Drawer* theDrawer );
  
  virtual GLboolean         highlight( GLfloat x, GLfloat y, GLfloat tol, GLboolean isCircle = GL_FALSE );
  virtual GLboolean         unhighlight();
  virtual GLboolean         select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect, GLboolean isFull = GL_FALSE,
				    GLboolean isCircle = GL_FALSE, GLboolean isShift = GL_FALSE );
  virtual GLboolean         unselect();
  
  virtual GLViewer_Rect*    getUpdateRect();
  
  virtual void              moveObject( float, float, bool fromGroup = false );
  
  virtual QByteArray        getByteCopy();
  virtual bool              initializeFromByteCopy( QByteArray );
  
  virtual bool              translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS );
  virtual bool              translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS );  
  
#ifdef WIN32
  virtual bool              translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS );
#endif
  
  int                       getWidth(){ return myWidth; }
  int                       getHeight(){ return myWidth; }
  void                      setWidth( int w ){ myWidth=w; }
  void                      setHeight( int h ){ myHeight=h; }
  
protected:
  bool                      myHighFlag;
  int                       myWidth;
  int                       myHeight;
};


/***************************************************************************
**  Class:   GLViewer_AspectLine
**  Descr:   Substitution of Prs2d_AspectLine for OpenGL
**  Module:  GLViewer
**  Created: UI team, 05.11.02
****************************************************************************/
class GLVIEWER_EXPORT GLViewer_AspectLine 
{
public:
  GLViewer_AspectLine();
  GLViewer_AspectLine( int, float );
  ~GLViewer_AspectLine();
  
  void                  setLineColors( QColor nc = QColor( 0, 0, 0 ),
				       QColor hc = QColor( 0, 255, 255 ),
				       QColor sc = QColor( 255, 0, 0 ) );
  int                   setLineWidth( const float );
  int                   setLineType( const int );
  
  void                  getLineColors( QColor&, QColor&, QColor& ) const;
  float                 getLineWidth() const { return myLineWidth; };
  int                   getLineType() const { return myLineType; };
  
  QByteArray            getByteCopy() const;
  
  static GLViewer_AspectLine* fromByteCopy( QByteArray );
  
protected:
  QColor                myNColor;
  QColor                myHColor;
  QColor                mySColor;
  float                 myLineWidth;
  int                   myLineType;  // 0 - normal, 1 - strip (not support in markers)
};

/***************************************************************************
**  Class:   GLViewer_MimeSource
**  Descr:   Needs for a work with QClipboard
**  Module:  GLViewer
**  Created: UI team, 22.03.04
****************************************************************************/
class GLVIEWER_EXPORT GLViewer_MimeSource: public QMimeSource
{
public:
  GLViewer_MimeSource():QMimeSource(){};
  ~GLViewer_MimeSource();
  
  bool                                setObjects( QValueList<GLViewer_Object*> );
  static QValueList<GLViewer_Object*> getObjects( QByteArray, QString );
  static GLViewer_Object*             getObject( QByteArray, QString );
  
  virtual const char*                 format( int theIndex = 0 ) const;
  virtual QByteArray                  encodedData( const char* ) const;
  
private:
  QByteArray                          myByteArray;
};

#ifdef WNT
#pragma warning ( default:4251 )
#endif

#endif // GLVIEWER_OBJECT_H
