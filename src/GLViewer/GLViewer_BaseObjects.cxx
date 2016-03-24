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

#include "GLViewer_BaseObjects.h"
#include "GLViewer_BaseDrawers.h"
#include "GLViewer_AspectLine.h"
#include "GLViewer_CoordSystem.h"
#include "GLViewer_Text.h"
#include "GLViewer_Group.h"
#include "GLViewer_Drawer.h"

#include <QFile>

/*!
  Constructor
*/
GLViewer_MarkerSet::GLViewer_MarkerSet( int number, float size, const QString& toolTip ) :
  GLViewer_Object(),
  myNumber( 0 ),
  myXCoord( 0 ),
  myYCoord( 0 )       
{
    
    myMarkerSize = size;
    myHNumbers.clear();
    myUHNumbers.clear();
    mySelNumbers.clear();
    myUSelNumbers.clear();
    myCurSelNumbers.clear();
    myPrevHNumbers.clear();    

    myType = "GLViewer_MarkerSet";
    myToolTipText = toolTip;
    
    setNumMarkers( number );    
}

/*!
  Destructor
*/
GLViewer_MarkerSet::~GLViewer_MarkerSet()
{
    if ( myXCoord )
        delete[] myXCoord;
    if ( myYCoord )
        delete[] myYCoord;
}

/*!
  Adds coords to text buffer in HPGL format
  \param buffer - text buffer
  \param command - command to be added with coords
  \param aViewerCS - viewer co-ordinates system
  \param aPaperCS - paper co-ordinates system
  \param x - x co-ordinate
  \param y - y co-ordinate
  \param NewLine - adds new line to buffer
*/
void AddCoordsToHPGL( QString& buffer, QString command, GLViewer_CoordSystem* aViewerCS, 
                      GLViewer_CoordSystem* aPaperCS, double x, double y, bool NewLine = true )
{
    if( aViewerCS && aPaperCS )
        aViewerCS->transform( *aPaperCS, x, y );

    QString temp = command + "%1, %2;";
    buffer += temp.arg( x ).arg( y );
    if( NewLine )
        buffer += ";\n";
}

/*!
  Adds coords to text buffer in PostScript format
  \param buffer - text buffer
  \param command - command to be added with coords
  \param aViewerCS - viewer co-ordinates system
  \param aPaperCS - paper co-ordinates system
  \param x - x co-ordinate
  \param y - y co-ordinate
  \param NewLine - adds new line to buffer
*/
void AddCoordsToPS( QString& buffer, QString command, GLViewer_CoordSystem* aViewerCS, 
                    GLViewer_CoordSystem* aPaperCS, double x, double y, bool NewLine = true )
{
    if( aViewerCS && aPaperCS )
        aViewerCS->transform( *aPaperCS, x, y );

    QString temp = "%1 %2 "+command;    
    buffer += temp.arg( x ).arg( y );
    if( NewLine )
        buffer += "\n";
}

/*!
  Adds line aspect description to text buffer in PostScript format
  \param buffer - text buffer
  \param anAspect - line aspect
  \param aViewerCS - viewer co-ordinates system
  \param aPaperCS - paper co-ordinates system
*/
void AddLineAspectToPS( QString& buffer, GLViewer_AspectLine* anAspect, 
                        GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPaperCS )
{
    if( anAspect )
    {
        QColor col1, col2, col3;
        anAspect->getLineColors( col1, col2, col3 );

        float aWidth = anAspect->getLineWidth();
        int aLineType = anAspect->getLineType();

        QString temp = "%1 %2 %3 setrgbcolor\n";
        double rr = 1 - double( col1.red() ) / 255.0, //color inverting
               gg = 1 - double( col1.green() ) / 255.0,
               bb = 1 - double( col1.blue() ) / 255.0;

        buffer += temp.arg( rr ).arg( gg ).arg( bb );

        double x_stretch, y_stretch;
        aViewerCS->getStretching( *aPaperCS, x_stretch, y_stretch );
        buffer += temp.arg( x_stretch * aWidth )+" setlinewidth\n";

        if( aLineType==0 ) //solid
            buffer += "[] 0 setdash\n";
        else if( aLineType==1 ) //strip
            buffer += "[2] 0 setdash\n";
    }
}

#ifdef WIN32
/*!
  Adds line aspect description EMF image
  \param hDC - descriptor of EMF
  \param anAspect - line aspect
  \param aViewerCS - viewer co-ordinates system
  \param aPaperCS - paper co-ordinates system
*/
HPEN AddLineAspectToEMF( HDC hDC, GLViewer_AspectLine* anAspect, 
                         GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPaperCS )
{
    if( anAspect )
    {
        QColor col1, col2, col3;
        anAspect->getLineColors( col1, col2, col3 );

        double x_stretch, y_stretch;
        aViewerCS->getStretching( *aPaperCS, x_stretch, y_stretch );

        double aWidth = anAspect->getLineWidth()*x_stretch;
        int aLineType = anAspect->getLineType();

        return CreatePen( PS_SOLID, aWidth, RGB( 255-col1.red(), 255-col1.green(), 255-col1.blue() ) );
    }
    else
        return NULL;
}
#endif

/*!
  Saves to file PostScript set of markers
  \param hFile - file instance
  \param aViewerCS - viewer co-ordinates system
  \param aPSCS - paper co-ordinates system
*/
bool GLViewer_MarkerSet::translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS )
{   
    int noPoints = 20;

    QString aBuffer = "newpath\n";

    AddLineAspectToPS( aBuffer, getAspectLine(), aViewerCS, aPSCS );

    for( int i=0; i<myNumber; i++ )
    {       
        aBuffer += "\n";

        double x_stretch, y_stretch;
        aViewerCS->getStretching( *aPSCS, x_stretch, y_stretch );

        double x0 = myXCoord[i],
               y0 = myYCoord[i],
               r  = myMarkerSize,
               x, y;

        for( int j=0; j<=noPoints; j++ )
        {
            x = x0 + r*cos( double(j)*2*PI/double(noPoints) );
            y = y0 + r*sin( double(j)*2*PI/double(noPoints) );          
            if( j==0 )
                AddCoordsToPS( aBuffer, "moveto", aViewerCS, aPSCS, x, y, true );               
            else
                AddCoordsToPS( aBuffer, "lineto", aViewerCS, aPSCS, x, y, true );
        }
    }
    aBuffer+="closepath\nstroke\n";

    hFile.write( aBuffer.toLatin1() );

    return true;
}

/*!
  Saves to file HPGL set of markers
  \param hFile - file instance
  \param aViewerCS - viewer co-ordinates system
  \param aHPGLCS - paper co-ordinates system
*/
bool GLViewer_MarkerSet::translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS,
                                       GLViewer_CoordSystem* aHPGLCS )
{
    int noPoints = 20;
    QString aBuffer;
    for( int i=0; i<myNumber; i++ )
    {
        aBuffer = "";

        double x_stretch, y_stretch;
        aViewerCS->getStretching( *aHPGLCS, x_stretch, y_stretch );

        double x0 = myXCoord[i],
               y0 = myYCoord[i],
               r  = myMarkerSize,
               x, y;

        AddCoordsToHPGL( aBuffer, "PA", aViewerCS, aHPGLCS, x0+r, y0 );
        aBuffer+="PD;\n";
        for( int j=1; j<=noPoints; j++ )
        {
            x = x0 + r*cos( double(j)*2*PI/double(noPoints) );
            y = y0 + r*sin( double(j)*2*PI/double(noPoints) );
            AddCoordsToHPGL( aBuffer, "PD", aViewerCS, aHPGLCS, x, y );
        }
        aBuffer+="PU;\n";

        hFile.write( aBuffer.toLatin1() );
    }

    return true;
}

#ifdef WIN32
/*!
  Saves to EMF image set of markers
  \param dc - EMF image descriptor
  \param aViewerCS - viewer co-ordinates system
  \param aEMFCS - paper co-ordinates system
*/
bool GLViewer_MarkerSet::translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS )
{
    int noPoints = 20;
    if( !aViewerCS || !aEMFCS )
        return false;
    
    HPEN pen = AddLineAspectToEMF( dc, getAspectLine(), aViewerCS, aEMFCS );
    HGDIOBJ old = SelectObject( dc, pen );

    for( int i=0; i<myNumber; i++ )
    {
        double x0 = myXCoord[i],
               y0 = myYCoord[i],
               r  = myMarkerSize,
               x, y;

        for( int j=0; j<=noPoints; j++ )
        {
            x = x0 + r*cos( double(j)*2*PI/double(noPoints) );
            y = y0 + r*sin( double(j)*2*PI/double(noPoints) );
            aViewerCS->transform( *aEMFCS, x, y );
            if( j==0 )
                MoveToEx( dc, x, y, NULL );
            else
                LineTo( dc, x, y );
        }
    }

    SelectObject( dc, old );
    if( pen )
        DeleteObject( pen );
    return true;
}
#endif

/*! 
  Computes all necessary information about object for presentation in drawer
*/
void GLViewer_MarkerSet::compute()
{
//  cout << "GLViewer_MarkerSet::compute" << endl;
  GLfloat xa = myXCoord[0]; 
  GLfloat xb = myXCoord[0]; 
  GLfloat ya = myYCoord[0]; 
  GLfloat yb = myYCoord[0]; 

  for ( int i = 0; i < myNumber; i++ )  
  {
    xa = qMin( xa, myXCoord[i] );
    xb = qMax( xb, myXCoord[i] );
    ya = qMin( ya, myYCoord[i] );
    yb = qMax( yb, myYCoord[i] );
  }
  
  myXGap = ( xb - xa ) / 10;
  myYGap = ( yb - ya ) / 10;

  myRect->setLeft( xa - myXGap );
  myRect->setTop( yb + myYGap ); 
  myRect->setRight( xb + myXGap );
  myRect->setBottom( ya - myYGap );
}

/*!
  Creates corresponding drawer
*/
GLViewer_Drawer* GLViewer_MarkerSet::createDrawer()
{
//  cout << "GLViewer_MarkerSet::createDrawer" << endl;
  return myDrawer = new GLViewer_MarkerDrawer();
}

/*!
  Computes highlight presentation
  \param x        - x coord
  \param y        - y coord
  \param tol      - tolerance of detecting
  \param isCircle - true if sensitive area of detection is round
  \return true if highlight status is changed
*/
GLboolean GLViewer_MarkerSet::highlight( GLfloat x, GLfloat y, GLfloat tol, GLboolean isCircle )
{
    if( !myIsVisible )
        return false;
//  cout << "GLViewer_MarkerSet::highlight " << x <<" " << y << " " << tol << endl;
  int count = 0;
  GLfloat xdist, ydist, radius;
  QList<int>::Iterator it;
  QList<int> curHNumbers;
  bool isFound;
  GLboolean update;
  int cnt = 0;

  radius = tol - myMarkerSize / 2.;
  
  myUHNumbers += myHNumbers;
  myHNumbers.clear();

  for ( int i = 0; i < myNumber; i++ ) 
  {
    xdist = ( myXCoord[i] - x ) * myXScale;
    ydist = ( myYCoord[i] - y ) * myYScale;

//    if ( isCircle && ( xdist * xdist + ydist * ydist <= radius * radius ) ||
    if ( ( isCircle && ( xdist * xdist + ydist * ydist <= myMarkerSize * myMarkerSize ) ) ||
         ( !isCircle && ( fabs( xdist ) <= radius && fabs( ydist ) <= radius ) ) )
    {
      isFound = false;
      count++;
      for ( it = myCurSelNumbers.begin(); it != myCurSelNumbers.end(); ++it )
        if( i == *it )
        {
          isFound = true;
          curHNumbers.append( i );
        }
      
      if( !isFound )
          myHNumbers.append( i );
      else
        cnt++;
    }
  }
  myCurSelNumbers = curHNumbers;

  myIsHigh = ( GLboolean )count;
  update = ( GLboolean )( myHNumbers != myPrevHNumbers );

  myPrevHNumbers = myHNumbers;

  //cout << "GLViewer_MarkerSet::highlight complete with " << (int)myIsHigh << endl;
  return update;
}

/*!
  Unhilights object
*/
GLboolean GLViewer_MarkerSet::unhighlight()
{
  if( !myHNumbers.isEmpty() )
  {
    myUHNumbers += myHNumbers;
    myPrevHNumbers.clear();
    myHNumbers.clear();
    //??? myCurSelNumbers.clear();
    return GL_TRUE;
  }
  
  return GL_FALSE;
}

/*!
  Selects marker set
  /param x, y - co-ordinates of mouse
  /param tol - tolerance
  /param rect - rectangle (in case of rectangular selection)
  /param isFull - if it is true, then object may selected only if it lays whole in selection zone
  \param isCircle - true if sensitive area of detection is round
  \param isShift  - true if selection exec with append option
*/
GLboolean GLViewer_MarkerSet::select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect, GLboolean isFull,
                                      GLboolean isCircle, GLboolean isShift )
{
    if( !myIsVisible )
        return false;
//  cout << "GLViewer_MarkerSet::select " << x << " " << y << endl;
  int count = 0;
  GLfloat xdist, ydist, radius;
  QList<int>::Iterator it;
  QList<int>::Iterator it1;
  QList<int>::Iterator remIt;
  QList<int>::Iterator curIt;

  radius = tol - myMarkerSize / 2.;

  if( radius < myMarkerSize / 2.)
    radius = myMarkerSize / 2.;

  count = isShift ? mySelNumbers.count() : 0;

  myUSelNumbers = mySelNumbers;

  if ( !isShift )
  {
    mySelNumbers.clear();
    myCurSelNumbers.clear();
  }

  for ( int i = 0; i < myNumber; i++ ) 
  {
    xdist = ( myXCoord[i] - x ) * myXScale;
    ydist = ( myYCoord[i] - y ) * myYScale;

    //if ( isCircle && ( xdist * xdist + ydist * ydist <= radius * radius ) ||
    if ( ( isCircle && ( xdist * xdist + ydist * ydist <= myMarkerSize * myMarkerSize ) ) ||
         ( !isCircle && ( fabs( xdist ) <= radius && fabs( ydist ) <= radius ) ) )
    {
      count++;
      if ( isShift )
      {
        bool isFound = false;
          for( it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
            if ( *it == i )
            {
              myUSelNumbers.append( *it );
            remIt = it;
              isFound = true;
              break;
            }

          if ( !isFound )
        {
          mySelNumbers.append( i );
            myCurSelNumbers.append( i );
            for ( it1 = myHNumbers.begin(); it1 != myHNumbers.end(); ++it1 )
              if( i == *it1 )
              {
                myHNumbers.erase( it1 );
                break;
              }
      for ( it1 = myUHNumbers.begin(); it1 != myUHNumbers.end(); ++it1 )
        if( i == *it1 )
        {
          myUHNumbers.erase( it1 );
          break;
        }
        }
    else
        {
      mySelNumbers.erase( remIt );
      for ( curIt = myCurSelNumbers.begin(); curIt != myCurSelNumbers.end(); ++curIt )
        if( *curIt == *remIt)
        {
          myCurSelNumbers.erase( curIt );
          break;
        }
      for ( it1 = myHNumbers.begin(); it1 != myHNumbers.end(); ++it1 )
        if( i == *it1 )
        {
          myHNumbers.erase( it1 );
          break;
        }
      for ( it1 = myUHNumbers.begin(); it1 != myUHNumbers.end(); ++it1 )
        if( i == *it1 )
        {
          myUHNumbers.erase( it1 );
          break;
        }
        }
      }
      else
      {
    mySelNumbers.append( i );
    myCurSelNumbers.append( i );
    for ( it1 = myHNumbers.begin(); it1 != myHNumbers.end(); ++it1 )
      if( i == *it1 )
      {
        myHNumbers.erase( it1 );
        break;
      }
    for ( it1 = myUHNumbers.begin(); it1 != myUHNumbers.end(); ++it1 )
      if( i == *it1 )
          {
        myUHNumbers.erase( it1 );
        break;
      }        
      }     
    }
  }

  for( it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
    for( it1 = myUSelNumbers.begin(); it1 != myUSelNumbers.end(); ++it1 )
      if( *it == *it1 )
      {
        it1 = myUSelNumbers.erase( it1 );
        it1--;
      }
  
  myIsSel = (GLboolean)count;

//  cout << "GLViewer_MarkerSet::select complete with " << (int)myIsSel << endl;
  return myIsSel;
}

/*!
  Unselects marker set
*/
GLboolean GLViewer_MarkerSet::unselect()
{
  if( !mySelNumbers.isEmpty() )
  {
    myUSelNumbers = mySelNumbers;
    mySelNumbers.clear();
    myCurSelNumbers.clear();
    return GL_TRUE;
  }

  return GL_FALSE;
}

/*!
  \return update object rectangle
  Does not equal getRect() if object have a persistence to some viewer transformations
*/
GLViewer_Rect* GLViewer_MarkerSet::getUpdateRect()
{
  GLViewer_Rect* rect = new GLViewer_Rect();
  
  rect->setLeft( myRect->left() + myXGap - myMarkerSize / myXScale );
  rect->setTop( myRect->top() + myYGap + myMarkerSize / myYScale ); 
  rect->setRight( myRect->right() - myXGap + myMarkerSize / myXScale );
  rect->setBottom( myRect->bottom() - myYGap - myMarkerSize / myYScale );
  //cout << " Additional tolerance " << myMarkerSize / myYScale << endl;
  //rect->setLeft( myRect->left() - myMarkerSize / myXScale );
  //rect->setTop( myRect->top() - myMarkerSize / myYScale ); 
  //rect->setRight( myRect->right() + myMarkerSize / myXScale );
  //rect->setBottom( myRect->bottom() + myMarkerSize / myYScale );
  
  return rect;
}

/*!
  Sets array of x coords of points
  \param xCoord - array of co-ordinates
  \param size - array size
*/
void GLViewer_MarkerSet::setXCoord( GLfloat* xCoord, int size )
{
  myXCoord = new GLfloat[ size ];
  for( int i = 0; i < size; i++ )
     myXCoord[i] = xCoord[i];
}

/*!
  Sets array of y coords of points
  \param yCoord - array of co-ordinates
  \param size - array size
*/
void GLViewer_MarkerSet::setYCoord( GLfloat* yCoord, int size )
{
  myYCoord = new GLfloat[ size ];
  for( int i = 0; i < size; i++ )
     myYCoord[i] = yCoord[i];
}

/*!
  Sets number of markers
  \param number - new number of markers
*/
void GLViewer_MarkerSet::setNumMarkers( GLint number )
{
  if ( myNumber == number )
    return;
    
  if ( myXCoord && myYCoord )
  {
    delete[] myXCoord;
    delete[] myYCoord;
  }

  myNumber = number;
  myXCoord = new GLfloat[ myNumber ];
  myYCoord = new GLfloat[ myNumber ];
}


/*!
  Export numbers of highlighted/selected lines
*/
void GLViewer_MarkerSet::exportNumbers( QList<int>& highlight,
                     QList<int>& unhighlight,
                     QList<int>& select,
                     QList<int>& unselect )
{
    highlight = myHNumbers;
    unhighlight = myUHNumbers;
    select = mySelNumbers;
    unselect = myUSelNumbers;

    myUHNumbers = myHNumbers;
}

/*!
  Adds or remove selected number
  \param index - selected index
*/
bool GLViewer_MarkerSet::addOrRemoveSelected( int index )
{
  if( index < 0 || index > myNumber )
    return false;

  int n = mySelNumbers.indexOf( index );
  if( n == -1 )
    mySelNumbers.append( index );
  else
  {
    mySelNumbers.removeAt(n);
    myUSelNumbers.append( index );
  }
  return true;
}

/*!
  Adds some selected numbers
  \param seq - sequence of indices
*/
void GLViewer_MarkerSet::addSelected( const TColStd_SequenceOfInteger& seq )
{
  for ( int i = 1; i <= seq.Length(); i++ )
    if( mySelNumbers.indexOf( seq.Value( i ) ) == -1 )
      mySelNumbers.append( seq.Value( i ) - 1 );
}

/*!
  Sets some numbers as selected
  \param seq - sequence of indices
*/
void GLViewer_MarkerSet::setSelected( const TColStd_SequenceOfInteger& seq )
{
//   for( QList<int>::Iterator it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
//     if( myUSelNumbers.findIndex( *it ) == -1 )
//       myUSelNumbers.append( *it );

  myUSelNumbers = mySelNumbers;
  mySelNumbers.clear();
    
  for ( int i = 1; i <= seq.Length(); i++ )
    mySelNumbers.append( seq.Value( i ) - 1 );
}

/*! Moves object by recomputing
  \param dx        - moving along X coord
  \param dy        - moving along Y coord
  \param fromGroup - is true if this method called from group
*/
void GLViewer_MarkerSet::moveObject( float theX, float theY, bool fromGroup )
{
    if( !fromGroup && myGroup)
    {
      myGroup->dragingObjects( theX, theY );
      return;
    }
    for( int i = 0; i < myNumber;  i++ )
    {
        myXCoord[i] = myXCoord[i] + theX;
        myYCoord[i] = myYCoord[i] + theY;
    }
    compute();    
}

/*!
  Codes marker set as byte copy
  \return byte array
*/
QByteArray GLViewer_MarkerSet::getByteCopy()
{
    int i = 0;
    int anISize = sizeof( GLint );
    int aFSize = sizeof( GLfloat );
    
    QByteArray aObject = GLViewer_Object::getByteCopy();

    QByteArray aResult;
    aResult.resize( anISize + 2*aFSize*myNumber + aFSize + aObject.size());

    char* aPointer = (char*)&myNumber;
    for( i = 0; i < anISize; i++, aPointer++ )
        aResult[i] = *aPointer;

    aPointer = (char*)myXCoord;
    for( ; i < anISize + aFSize*myNumber; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)myYCoord;
    for( ; i < anISize + 2*aFSize*myNumber; i++, aPointer++ )
        aResult[i] = *aPointer;
    
    aPointer = (char*)&myMarkerSize;
    for( ; i < anISize + 2*aFSize*myNumber + aFSize; i++, aPointer++ )
        aResult[i] = *aPointer;
        
    
    for ( ; i < (int)aResult.size(); i++ )
        aResult[i] = aObject[i - anISize - 2*aFSize*myNumber - aFSize];

    return aResult;
}

/*!
  Initialize marker set by byte array
  \param theArray - byte array
*/
bool GLViewer_MarkerSet::initializeFromByteCopy( QByteArray theArray )
{
    int i = 0;
    int anISize = sizeof( GLint );
    int aFSize = sizeof( GLfloat );

    char* aPointer = (char*)&myNumber;
    for( i = 0; i < anISize; i++, aPointer++ )
        *aPointer = theArray[i];

    int aSize = theArray.size();
    if( aSize < anISize + 2*aFSize*myNumber + aFSize)
        return false;

    myXCoord = new GLfloat[myNumber];
    myYCoord = new GLfloat[myNumber];
    aPointer = (char*)myXCoord;
    for( ; i < anISize + aFSize*myNumber; i++, aPointer++ )
        *aPointer = theArray[i];
    aPointer = (char*)myYCoord;
    for( ; i < anISize + 2*aFSize*myNumber; i++, aPointer++ )
        *aPointer = theArray[i];

    aPointer = (char*)&myMarkerSize;
    for( ; i < anISize + 2*aFSize*myNumber + aFSize; i++, aPointer++ )
         *aPointer = theArray[i];
         
    int aCurIndex = anISize + 2*aFSize*myNumber + aFSize;
    QByteArray aObject;
    aObject.resize( aSize - aCurIndex );
    for( ; i < aSize; i++ )
        aObject[i - aCurIndex] = theArray[i];
        

    if( !GLViewer_Object::initializeFromByteCopy( aObject ) || myType != "GLViewer_MarkerSet" )
        return false;

    myHNumbers.clear();
    myUHNumbers.clear();
    mySelNumbers.clear();
    myUSelNumbers.clear();
    myCurSelNumbers.clear();
    myPrevHNumbers.clear();

    return true;        
}

/*!
  \class GLViewer_Polyline
  OpenGL Polyline
*/

#define SECTIONS 100
#define DISTANTION 5

/*!
  Constructor
  \param number - number of segments
  \param size - size of polyline
  \param toolTip - tool tip of polyline
*/
GLViewer_Polyline::GLViewer_Polyline( int number, float size, const QString& toolTip ):
  GLViewer_Object(),
  myNumber( 0 ),
  myXCoord( 0 ),
  myYCoord( 0 )       
{
  myHighFlag = GL_TRUE;

  myHNumbers.clear();
  myUHNumbers.clear();
  mySelNumbers.clear();
  myUSelNumbers.clear();
  myCurSelNumbers.clear();
  myPrevHNumbers.clear();

  setNumber( number );

  myType = "GLViewer_Polyline";
  myToolTipText = toolTip;
}

/*!
  Destructor, destroys internal arrays of co-ordinates
*/
GLViewer_Polyline::~GLViewer_Polyline()
{
  if ( myXCoord )
    delete[] myXCoord;
  if ( myYCoord )
    delete[] myYCoord;
}

/*!
  Saves polyline to file PostScript
  \param hFile - file instance
  \param aViewerCS - viewer co-ordinates system
  \param aPSCS - paper co-ordinates system
*/
bool GLViewer_Polyline::translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS )
{
    QString aBuffer = "newpath\n";

    AddLineAspectToPS( aBuffer, getAspectLine(), aViewerCS, aPSCS );

    for( int i=0; i<myNumber; i++ )
        if( i==0 )
            AddCoordsToPS( aBuffer, "moveto", aViewerCS, aPSCS, myXCoord[i], myYCoord[i] );
        else
            AddCoordsToPS( aBuffer, "lineto", aViewerCS, aPSCS, myXCoord[i], myYCoord[i] );

    if( myIsClosed )
        AddCoordsToPS( aBuffer, "lineto", aViewerCS, aPSCS, myXCoord[0], myYCoord[0] );

    aBuffer+="closepath\nstroke\n";
    
    hFile.write( aBuffer.toLatin1() );

    return true;
}

/*!
  Saves polyline to file HPGL
  \param hFile - file instance
  \param aViewerCS - viewer co-ordinates system
  \param aHPGLCS - paper co-ordinates system
*/
bool GLViewer_Polyline::translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS )
{
    QString aBuffer = "";
    for( int i=0; i<myNumber; i++ )
    {
        AddCoordsToHPGL( aBuffer, "PA", aViewerCS, aHPGLCS, myXCoord[i], myYCoord[i] );
        if( i==0 )
            aBuffer+="PD;\n";
    }

    if( myIsClosed )
        AddCoordsToHPGL( aBuffer, "PA", aViewerCS, aHPGLCS, myXCoord[0], myYCoord[0] );

    aBuffer+="PU;\n";
    
    hFile.write( aBuffer.toLatin1() );

    return true;
}

#ifdef WIN32
/*!
  Saves polyline to EMF image
  \param dc - EMF image descriptor
  \param aViewerCS - viewer co-ordinates system
  \param aEMFCS - paper co-ordinates system
*/
bool GLViewer_Polyline::translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS )
{
    if( !aViewerCS || !aEMFCS )
        return false;
    
    HPEN pen = AddLineAspectToEMF( dc, getAspectLine(), aViewerCS, aEMFCS );
    HGDIOBJ old = SelectObject( dc, pen );

    double x, y;
    for( int i=0; i<myNumber; i++ )
    {
        x = myXCoord[i];
        y = myYCoord[i];
        aViewerCS->transform( *aEMFCS, x, y );
        if( i==0 )
            MoveToEx( dc, x, y, NULL );
        else
            LineTo( dc, x, y );
    }

    if( myIsClosed )
    {
        x = myXCoord[0];
        y = myYCoord[0];
        aViewerCS->transform( *aEMFCS, x, y );
        LineTo( dc, x, y );
    }

    SelectObject( dc, old );
    if( pen )
        DeleteObject( pen );

    return true;
}
#endif

/*! 
  Computes all necessary information about object for presentation in drawer
*/
void GLViewer_Polyline::compute()
{
//  cout << "GLViewer_MarkerSet::compute" << endl;
  GLfloat xa = myXCoord[0]; 
  GLfloat xb = myXCoord[0]; 
  GLfloat ya = myYCoord[0]; 
  GLfloat yb = myYCoord[0]; 

  for ( int i = 0; i < myNumber; i++ )  
  {
    xa = qMin( xa, myXCoord[i] );
    xb = qMax( xb, myXCoord[i] );
    ya = qMin( ya, myYCoord[i] );
    yb = qMax( yb, myYCoord[i] );
  }

  GLfloat xGap = ( xb - xa ) / 10;
  GLfloat yGap = ( yb - ya ) / 10;

  myRect->setLeft( xa - xGap );
  myRect->setTop( yb + yGap ); 
  myRect->setRight( xb + xGap );
  myRect->setBottom( ya - yGap );
}

/*!
  \return update object rectangle
  Does not equal getRect() if object have a persistence to some viewer transformations
*/
GLViewer_Rect* GLViewer_Polyline::getUpdateRect()
{
    GLViewer_Rect* rect = new GLViewer_Rect();

    rect->setLeft( myRect->left() - myXGap );
    rect->setTop( myRect->top() + myYGap ); 
    rect->setRight( myRect->right() + myXGap );
    rect->setBottom( myRect->bottom() - myYGap );

    return rect;
}

/*!
  Creates corresponding drawer
*/
GLViewer_Drawer* GLViewer_Polyline::createDrawer()
{
//  cout << "GLViewer_MarkerSet::createDrawer" << endl;
    return myDrawer = new GLViewer_PolylineDrawer();
}

/*!
  Computes highlight presentation
  \param x        - x coord
  \param y        - y coord
  \param tol      - tolerance of detecting
  \param isCircle - true if sensitive area of detection is round
  \return true if highlight status is changed
*/
GLboolean GLViewer_Polyline::highlight( GLfloat x, GLfloat y, GLfloat tol, GLboolean isCircle )
{
    if( !myIsVisible )
        return false;
    GLfloat xa, xb, ya, yb, l;
    GLfloat rsin, rcos, r, ra, rb;
    GLboolean update;
    GLboolean highlighted = myIsHigh;

    myIsHigh = GL_FALSE;

    int c = 0;
    if( myIsClosed )
        c = 1;

    for( int i = 0; i < myNumber-1+c; i++ ) 
    {
        xa = myXCoord[i];
        ya = myYCoord[i];
        if( i != myNumber-1 )
        {
              xb = myXCoord[i+1];
              yb = myYCoord[i+1];
        }
        else
        {    
              xb = myXCoord[0];      
              yb = myYCoord[0];
        }

        l = sqrt( (xb-xa)*(xb-xa) + (yb-ya)*(yb-ya) );
        rsin = (yb-ya) / l;
        rcos = (xb-xa) / l;
        r = ( (x-xa)*(y-yb) - (x-xb)*(y-ya) ) / ( rsin*(ya-yb) + rcos*(xa-xb) );
        ra = sqrt( (x-xa)*(x-xa) + (y-ya)*(y-ya) );
        rb = sqrt( (x-xb)*(x-xb) + (y-yb)*(y-yb) );
        if( fabs( r ) * myXScale <= DISTANTION && ra <= l + DISTANTION && rb <= l + DISTANTION )
        {
            myIsHigh = GL_TRUE;
            break;
        }
    }

    if( !myHighFlag && myIsHigh )
        myIsHigh = GL_FALSE;
    else
        myHighFlag = GL_TRUE;

    update = ( GLboolean )( myIsHigh != highlighted );

//  cout << "GLViewer_Polyline::highlight complete with " << (int)myIsHigh << endl;
    return update;
}

/*!
  Unhilights object
*/
GLboolean GLViewer_Polyline::unhighlight()
{
//   if( !myHNumbers.isEmpty() )
//   {
//     myUHNumbers = myHNumbers;
//     myHNumbers.clear();
//     return GL_TRUE;
//   }

  if( myIsHigh )
  {
    myIsHigh = GL_FALSE;
    return GL_TRUE;
  }

  return GL_FALSE;
}

/*!
  Selects polyline
  /param x, y - co-ordinates of mouse
  /param tol - tolerance
  /param rect - rectangle (in case of rectangular selection)
  /param isFull - if it is true, then object may selected only if it lays whole in selection zone
  \param isCircle - true if sensitive area of detection is round
  \param isShift  - true if selection exec with append option
*/
GLboolean GLViewer_Polyline::select( GLfloat x, GLfloat y, GLfloat tol, GLViewer_Rect rect, GLboolean isFull,
                                     GLboolean isCircle, GLboolean isShift )
{
    if( !myIsVisible )
        return false;
    GLfloat xa, xb, ya, yb, l;
    GLfloat rsin, rcos, r, ra, rb;
    // GLboolean update;
    GLboolean selected = myIsSel;

    myIsSel = GL_FALSE;

    int c = 0;
    if( myIsClosed )
        c = 1;

    for( int i = 0; i < myNumber-1+c; i++ ) 
    {
        xa = myXCoord[i];
        ya = myYCoord[i];
        if( i != myNumber-1 )
        {
            xb = myXCoord[i+1];
            yb = myYCoord[i+1];
        }
        else
        {
            xb = myXCoord[0];
            yb = myYCoord[0];
        }

        l = sqrt( (xb-xa)*(xb-xa) + (yb-ya)*(yb-ya) );
        rsin = (yb-ya) / l;
        rcos = (xb-xa) / l;
        r = ( (x-xa)*(y-yb) - (x-xb)*(y-ya) ) / ( rsin*(ya-yb) + rcos*(xa-xb) );
        ra = sqrt( (x-xa)*(x-xa) + (y-ya)*(y-ya) );
        rb = sqrt( (x-xb)*(x-xb) + (y-yb)*(y-yb) );
        if( fabs( r ) * myXScale <= DISTANTION && ra <= l + DISTANTION && rb <= l + DISTANTION )
        {
            myIsSel = GL_TRUE;
            break;
        }
    }

    if ( myIsSel )
    {
        myHighFlag = GL_FALSE;
        myIsHigh = GL_FALSE;
    }
    else
        myHighFlag = GL_TRUE;

    // update = ( GLboolean )( myIsSel != selected );

    //  cout << "GLViewer_Polyline::select complete with " << (int)myIsSel << endl;

    //  return update;  !!!!!!!!!!!!!!!!!!!!!!!!!!! no here
    return myIsSel;
}

/*!
  Unselects polyline
*/
GLboolean GLViewer_Polyline::unselect()
{
//   if( !mySelNumbers.isEmpty() )
//   {
//     myUSelNumbers = mySelNumbers;
//     mySelNumbers.clear();
//     myCurSelNumbers.clear();
//     return GL_TRUE;
//   }

  if( myIsSel )
  {
    myIsSel = GL_FALSE;
    return GL_TRUE;
  }

  return GL_FALSE;
}

/*!
  Sets array of abscisses for points of polyline
  \param xCoord - array of of abscisses
  \param size - size of array
*/
void GLViewer_Polyline::setXCoord( GLfloat* xCoord, int size )
{
  myXCoord = new GLfloat[ size ];
  for( int i = 0; i < size; i++ )
     myXCoord[i] = xCoord[i];
}

/*!
  Sets array of ordinates for points of polyline
  \param xCoord - array of of ordinates
  \param size - size of array
*/
void GLViewer_Polyline::setYCoord( GLfloat* yCoord, int size )
{
  myYCoord = new GLfloat[ size ];
  for( int i = 0; i < size; i++ )
     myYCoord[i] = yCoord[i];
}

/*!
  Sets number of points
  \param number - new number of points
*/
void GLViewer_Polyline::setNumber( GLint number )
{
  if ( myNumber == number )
    return;
    
  if ( myXCoord && myYCoord )
  {
    delete[] myXCoord;
    delete[] myYCoord;
  }

  myNumber = number;
  myXCoord = new GLfloat[ myNumber ];
  myYCoord = new GLfloat[ myNumber ];
}

/*!
  Export numbers of highlighted/selected lines
*/
void GLViewer_Polyline::exportNumbers( QList<int>& highlight,
                     QList<int>& unhighlight,
                     QList<int>& select,
                     QList<int>& unselect )
{
  highlight = myHNumbers;
  unhighlight = myUHNumbers;
  select = mySelNumbers;
  unselect = myUSelNumbers;
}

/*!
  Moves object by recomputing
  \param dx        - moving along X coord
  \param dy        - moving along Y coord
  \param fromGroup - is true if this method called from group
*/
void GLViewer_Polyline::moveObject( float theX, float theY, bool fromGroup )
{
  if( !fromGroup && myGroup)
  {
    myGroup->dragingObjects( theX, theY );
    return;
  }
  for( int i = 0; i < myNumber;  i++ )
  {
      myXCoord[i] = myXCoord[i] + theX;
      myYCoord[i] = myYCoord[i] + theY;
  }
  compute();    
}

/*!
  Codes polyline as byte copy
  \return byte array
*/
QByteArray GLViewer_Polyline::getByteCopy()
{
    int i = 0;
    int anISize = sizeof( GLint );
    int aFSize = sizeof( GLfloat );
    int aBSize = sizeof( GLboolean );

    QByteArray aObject = GLViewer_Object::getByteCopy();

    QByteArray aResult;
    aResult.resize( aFSize*myNumber*2 + anISize + 2*aBSize + aObject.size());

    char* aPointer = (char*)&myNumber;
    for( i = 0; i < anISize; i++, aPointer++ )
        aResult[i] = *aPointer;

    aPointer = (char*)myXCoord;
    for( ; i < anISize + aFSize*myNumber; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)myYCoord;
    for( ; i < anISize + 2*aFSize*myNumber; i++, aPointer++ )
        aResult[i] = *aPointer;
    
    aPointer = (char*)&myIsClosed;
    for( ; i < anISize + 2*aFSize*myNumber + aBSize; i++, aPointer++ )
        aResult[i] = *aPointer;
    aPointer = (char*)&myHighSelAll;
    for( ; i < anISize + 2*aFSize*myNumber + 2*aBSize; i++, aPointer++ )
        aResult[i] = *aPointer;

    for ( ; i < (int)aResult.size(); i++ )
        aResult[i] = aObject[i - anISize - 2*aFSize*myNumber - 2*aBSize];

    return aResult;
}


/*!
  Initialize polyline by byte array
  \param theArray - byte array
*/
bool GLViewer_Polyline::initializeFromByteCopy( QByteArray theArray )
{
    int i = 0;
    int anISize = sizeof( GLint );
    int aFSize = sizeof( GLfloat );
    int aBSize = sizeof( GLboolean );

    char* aPointer = (char*)&myNumber;
    for( i = 0; i < anISize; i++, aPointer++ )
        *aPointer = theArray[i];

    int aSize = theArray.size();
    if( aSize < aFSize*myNumber*2 + anISize + 2*aBSize )
        return false;

    myXCoord = new GLfloat[myNumber];
    myYCoord = new GLfloat[myNumber];
    aPointer = (char*)myXCoord;
    for( ; i < anISize + aFSize*myNumber; i++, aPointer++ )
        *aPointer = theArray[i];
    aPointer = (char*)myYCoord;
    for( ; i < anISize + 2*aFSize*myNumber; i++, aPointer++ )
        *aPointer = theArray[i];

    aPointer = (char*)&myIsClosed;
    for( ; i < anISize + 2*aFSize*myNumber + aBSize; i++, aPointer++ )
         *aPointer = theArray[i];
    aPointer = (char*)&myHighSelAll;
    for( ; i < anISize + 2*aFSize*myNumber + 2*aBSize; i++, aPointer++ )
         *aPointer = theArray[i];

    int aCurIndex = anISize + 2*aFSize*myNumber + 2*aBSize;
    QByteArray aObject;
    aObject.resize( aSize - aCurIndex );
    for( ; i < aSize; i++ )
        aObject[i - aCurIndex] = theArray[i];

    if( !GLViewer_Object::initializeFromByteCopy( aObject ) || myType != "GLViewer_Polyline" )
        return false;

    myHNumbers.clear();
    myUHNumbers.clear();
    mySelNumbers.clear();
    myUSelNumbers.clear();
    myCurSelNumbers.clear();
    myPrevHNumbers.clear();

    return true;        
}



/*!
  Constructor
  \param theStr - text string
  \param xPos - x position
  \param yPos - y position
  \param color - color of text
  \param toolTip - tooltip of text object
*/
GLViewer_TextObject::GLViewer_TextObject( const QString& theStr, float xPos, float yPos, 
                                    const QColor& color, const QString& toolTip )
                                    : GLViewer_Object()
{
    myGLText = new GLViewer_Text( theStr, xPos, yPos, color );
    myWidth = 0;
    myHeight = 0;

    myHighFlag = GL_TRUE;

    myToolTipText = toolTip;
}

/*!
  Destructor
*/
GLViewer_TextObject::~GLViewer_TextObject()
{
  if ( myGLText )
    delete myGLText;
}

/*!
  Saves text object to file PostScript
  \param hFile - file instance
  \param aViewerCS - viewer co-ordinates system
  \param aPSCS - paper co-ordinates system
*/
bool GLViewer_TextObject::translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS )
{
    QString aText = myGLText->getText();    
    float xPos, yPos;
    myGLText->getPosition( xPos, yPos );

    QString aBuffer = "/Times-Roman findfont\n";
    aBuffer += "12 scalefont setfont\n";

    AddCoordsToPS( aBuffer, "moveto", aViewerCS, aPSCS, double(xPos), double(yPos) );
    aBuffer += "(" + aText + ") show\n";

    hFile.write( aBuffer.toLatin1() );

    return true;
}

/*!
  Saves text object to file HPGL
  \param hFile - file instance
  \param aViewerCS - viewer co-ordinates system
  \param aHPGLCS - paper co-ordinates system
*/
bool GLViewer_TextObject::translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS )
{
    QString aText = myGLText->getText();    
    float xPos, yPos;
    myGLText->getPosition( xPos, yPos );

    QString aBuffer = "";
    AddCoordsToHPGL( aBuffer, "PA", aViewerCS, aHPGLCS, double(xPos), double(yPos) );
    
    aBuffer = "LB" + aText + "#;";
    
    hFile.write( aBuffer.toLatin1() );

    return true;
}

#ifdef WIN32
/*!
  Saves text object to EMF image
  \param dc - EMF image descriptor
  \param aViewerCS - viewer co-ordinates system
  \param aEMFCS - paper co-ordinates system
*/
bool GLViewer_TextObject::translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS )
{
    QString aText = myGLText->getText();    
    float xPos, yPos;
    myGLText->getPosition( xPos, yPos );

    double x = double( xPos ), 
           y = double( yPos );

    aViewerCS->transform( *aEMFCS, x, y );

    int nHeight = 35*14;       // height of font
    int nWidth = 35*12;        // average character width
    int nEscapement = 0;       // angle of escapement
    int nOrientation = 0;      // base-line orientation angle
    int fnWeight = FW_NORMAL;  // font weight
    DWORD fdwItalic = false;    // italic attribute option
    DWORD fdwUnderline = false; // underline attribute option
    DWORD fdwStrikeOut = false; // strikeout attribute option
    DWORD fdwCharSet = ANSI_CHARSET; // character set identifier
    DWORD fdwOutputPrecision = OUT_DEFAULT_PRECIS;  // output precision
    DWORD fdwClipPrecision = CLIP_DEFAULT_PRECIS;    // clipping precision
    DWORD fdwQuality = PROOF_QUALITY;          // output quality
    DWORD fdwPitchAndFamily = FIXED_PITCH | FF_DONTCARE;   // pitch and family
    LPCTSTR lpszFace = NULL;         // typeface name


    HFONT aFont = CreateFont( nHeight, nWidth, nEscapement, nOrientation, fnWeight, fdwItalic,
                              fdwUnderline, fdwStrikeOut, fdwCharSet, fdwOutputPrecision, 
                              fdwClipPrecision, fdwQuality, fdwPitchAndFamily, lpszFace );
    LOGBRUSH aBrushData;
    aBrushData.lbStyle = BS_HOLLOW;

    HBRUSH aBrush = CreateBrushIndirect( &aBrushData );

    HGDIOBJ old1 = SelectObject( dc, aFont );
    HGDIOBJ old2 = SelectObject( dc, aBrush );

    TextOut( dc, x, y, aText.toLatin1().constData(), aText.length() );

    SelectObject ( dc, old1 );
    SelectObject ( dc, old2 );

    DeleteObject( aFont );

    return true;
}
#endif

/*!
  Creates corresponding drawer
*/
GLViewer_Drawer* GLViewer_TextObject::createDrawer()
{
    myDrawer = new GLViewer_TextDrawer();
    compute();
    return myDrawer;
}

/*! 
  Computes all necessary information about object for presentation in drawer
*/
void GLViewer_TextObject::compute()
{
    float xPos, yPos;
    QString aStr = myGLText->getText();
    myGLText->getPosition( xPos, yPos );

    myWidth = myGLText->getWidth();
    myHeight = myGLText->getHeight();
    myRect->setLeft( xPos );
    myRect->setTop( yPos + myHeight  ); 
    myRect->setRight( xPos + myWidth );
    myRect->setBottom( yPos );
}

/*!
  Installing already exist drawer with same type
  \param theDrawer - new drawer
*/
void GLViewer_TextObject::setDrawer( GLViewer_Drawer* theDrawer )
{
    myDrawer = theDrawer;
    //compute();
}

/*!
  \return update object rectangle
  Does not equal getRect() if object have a persistence to some viewer transformations
*/
GLViewer_Rect* GLViewer_TextObject::getUpdateRect()
{    
    GLViewer_Rect* rect = new GLViewer_Rect();

    float xPos, yPos;
    QString aStr = myGLText->getText();
    myGLText->getPosition( xPos, yPos );

    rect->setLeft( myRect->left() + myXGap - myWidth / myXScale );
    rect->setTop( myRect->top() + myYGap + myHeight / myYScale );
    rect->setRight( myRect->right() - myXGap + myWidth / myXScale );
    rect->setBottom( myRect->bottom() - myYGap - myHeight / myYScale );

    return rect;
}

/*!
  Computes highlight presentation
  \param x        - x coord
  \param y        - y coord
  \param tol      - tolerance of detecting
  \param isCircle - true if sensitive area of detection is round
  \return true if highlight status is changed
*/
GLboolean GLViewer_TextObject::highlight( GLfloat theX, GLfloat theY, GLfloat theTol, GLboolean isCircle )
{
    if( !myIsVisible )
        return false;

    float xPos, yPos;
    myGLText->getPosition( xPos, yPos );

    QRect aRect;
    aRect.setLeft( (int)xPos );
    aRect.setRight( (int)(xPos + myWidth / myXScale) );
    aRect.setTop( (int)yPos );// - myHeight / myYScale );
    aRect.setBottom( (int)(yPos + myHeight / myYScale) );

    //cout << "theX: " << theX << "  theY: " << theY << endl;
    //cout << "aRect.left(): " << aRect.left() << "  aRect.right(): " << aRect.right() << endl;
    //cout << "aRect.top(): " << aRect.top() << "  aRect.bottom(): " << aRect.bottom() << endl;

    QRegion obj( aRect );
    QRegion intersection;
    QRect region;

    region.setLeft( (int)(theX - theTol) );
    region.setRight( (int)(theX + theTol) );
    region.setTop( (int)(theY - theTol) );
    region.setBottom( (int)(theY + theTol) );

    QRegion circle( (int)(theX - theTol), (int)(theY - theTol),
                      (int)(2 * theTol), (int)(2 * theTol), QRegion::Ellipse );
    if( isCircle )
        intersection = obj.intersected( circle );
    else
        intersection = obj.intersected( region );
    
    if( intersection.isEmpty() )
        myIsHigh = false;
    else
        myIsHigh = true;
    
    if( !myHighFlag && myIsHigh )
        myIsHigh = GL_FALSE;
    else
        myHighFlag = GL_TRUE;

    return myIsHigh;
}

/*!
  Unhilights object
*/
GLboolean GLViewer_TextObject::unhighlight()
{
    if( myIsHigh )
    {
        myIsHigh = GL_FALSE;
        return GL_TRUE;
    }

    return GL_FALSE;
}

/*!
  Selects text object
  /param x, y - co-ordinates of mouse
  /param tol - tolerance
  /param rect - rectangle (in case of rectangular selection)
  /param isFull - if it is true, then object may selected only if it lays whole in selection zone
  \param isCircle - true if sensitive area of detection is round
  \param isShift  - true if selection exec with append option
*/
GLboolean GLViewer_TextObject::select( GLfloat theX, GLfloat theY, GLfloat theTol, GLViewer_Rect rect,
                                       GLboolean isFull, GLboolean isCircle, GLboolean isShift )
{ 
    if( !myIsVisible )
        return false;

    QRegion obj( myRect->toQRect() );
    QRegion intersection;
    QRect region;

    region.setLeft( (int)(theX - theTol) );
    region.setRight( (int)(theX + theTol) );
    region.setTop( (int)(theY - theTol) );
    region.setBottom( (int)(theY + theTol) );

    QRegion circle( (int)(theX - theTol), (int)(theY - theTol),
                      (int)(2 * theTol), (int)(2 * theTol), QRegion::Ellipse );
    if( isCircle )
        intersection = obj.intersected( circle );
    else
        intersection = obj.intersected( region );
    
    if( intersection.isEmpty() )
        myIsSel = false;
    else
        myIsSel = true;

    if ( myIsSel )
    {
        myHighFlag = GL_FALSE;
        myIsHigh = GL_FALSE;
    }
    else
        myHighFlag = GL_TRUE;

    return myIsSel;
}

/*!
  Unselects text object
*/
GLboolean GLViewer_TextObject::unselect()
{
    if( myIsSel )
    {
        myIsSel = GL_FALSE;
        return GL_TRUE;
    }

    return GL_FALSE;
}

/*!
  Moves object by recomputing
  \param dx        - moving along X coord
  \param dy        - moving along Y coord
  \param fromGroup - is true if this method called from group
*/
void GLViewer_TextObject::moveObject( float theX, float theY, bool fromGroup )
{
  if( !fromGroup && myGroup)
  {
    myGroup->dragingObjects( theX, theY );
    return;
  }
  float aX, anY;
  myGLText->getPosition( aX, anY );
  aX += theX;
  anY += theY;
  myGLText->setPosition( aX, anY );
  compute();
}

/*!
  Codes text object as byte copy
  \return byte array
*/
QByteArray GLViewer_TextObject::getByteCopy()
{
    QByteArray aObject = GLViewer_Object::getByteCopy();

    return aObject;
}

/*!
  Initialize text object by byte array
  \param theArray - byte array
*/
bool GLViewer_TextObject::initializeFromByteCopy( QByteArray theArray )
{
    if( !GLViewer_Object::initializeFromByteCopy( theArray ) || myType != "GLViewer_TextObject" )
        return false;

    myHighFlag = true;
    return true;        
}
