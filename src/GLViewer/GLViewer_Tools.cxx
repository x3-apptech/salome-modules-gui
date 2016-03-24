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
// File:      GLViewer_Tools.cxx
// Created:   April, 2005
//
#include "GLViewer_Tools.h"

#include <GL/gl.h>

#include <iostream>
#include <string.h>

/*!
  Constructor
  \param size - size of internal array
*/
GLViewer_LineList::GLViewer_LineList( int size )
{
  myRealSize = 2*size;
  mySegmentNumber = 0;
  myMainCoord = 0.0;

  myArray = new double[myRealSize];

  if( !myArray )
  {
    std::cout << "Can't allocate memory: " << size << std::endl;
    myRealSize = 0;
  }
  else
    memset( myArray, 0, myRealSize*sizeof(double) );
}

/*!
  Destructor
*/
GLViewer_LineList::~GLViewer_LineList()
{
  delete myArray;
}

/*!
  Adds new line segment to list
  \param coord1, coord2 - co-ordinates of points
*/
bool GLViewer_LineList::addSegment( double coord1, double coord2 )
{
  if( coord1 > coord2 )
  {
    double temp = coord1;
    coord1 = coord2;
    coord2 = temp;
  }

  if( 2*mySegmentNumber == myRealSize || !myArray )
    return false;

  int index = 0;
  double c1, c2;
  while( index < mySegmentNumber)
  {
    readSegment( index, c1, c2 );
    if( coord1 < c1 && coord2 < c1 )
    {
      for( int i = mySegmentNumber; i > index - 1; i--)
      {
        myArray[2*i] = myArray[2*i-2]; //2*(i-1)
        myArray[2*i+1] = myArray[2*i-1];//2*(i-1)+1
      }
      myArray[0] = coord1;
      myArray[1] = coord2;
      // mySegmentNumber; what is means ?
      return true;
    }
    else if( coord1 < c1 && coord2 < c2 )
    {
      myArray[index*2] = coord1;
      return true;
    }
    else if( c1 < coord1 && coord2 < c2 )
    {
      return true;
    }
    else if( coord1 < c2 && c2 < coord2 )
    {
      if( c1 > coord1 )
        myArray[2*index] = coord1;

      if( index != mySegmentNumber - 1 )
      {
        for( int i = index+1; i < mySegmentNumber; i++ )
        {
          if( coord2 < myArray[2*i] )
          {
            myArray[2*index+1] = coord2;
            if( index+1 != i )
            {
              for( int j = 0; i+j < mySegmentNumber;j++ )
              {
                myArray[2*(index+1+j)] = myArray[2*(i+j)];
                myArray[2*(index+1+j)+1] = myArray[2*(i+j)+1];
              }
              for( int k = 0; k < mySegmentNumber - i; k++ )
              {
                myArray[2*(mySegmentNumber - 1- k)] = 0.0;
                myArray[2*(mySegmentNumber - 1- k)+1] = 0.0;
              }
              mySegmentNumber -= i - index-1;
            }            
            return true;
          }
          else if( coord2 < myArray[2*i+1] )
          {
            myArray[2*index+1] = myArray[2*i+1];
            
            for( int j = index+1; j < mySegmentNumber-1;j++ )
            {
              myArray[2*j] = myArray[2*(i+j-index)];
              myArray[2*j+1] = myArray[2*(i+j-index)+1];
            }
            for( int k = 0; k < mySegmentNumber - i-1; k++ )
            {
              myArray[2*(mySegmentNumber - 1- k)] = 0.0;
              myArray[2*(mySegmentNumber - 1- k)+1] = 0.0;
            }
            mySegmentNumber -= i - index;
            return true;
          }
        }
      }
      else
      {
        myArray[2*index+1] = coord2;
        return true;
      }
    }    
    index++;
  }

  myArray[mySegmentNumber*2] = coord1;
  myArray[mySegmentNumber*2+1] = coord2;
  mySegmentNumber++;

  return true;
}

/*!
  Gets info about segment
  \param theIndex - index of segment
  \param coord1, coord2 - co-ordinates of points
*/
bool GLViewer_LineList::readSegment( int theIndex, double& coord1, double& coord2 )
{
  if( theIndex > mySegmentNumber || !myArray)
    return false;

  coord1 = myArray[theIndex*2];
  coord2 = myArray[theIndex*2+1];

  return true;
}

/*!
  \return true if line list covers point
  \param thePoint - point co-ordinate
*/
int GLViewer_LineList::contains( double thePoint ) const
{
  if( !myArray || mySegmentNumber == 0 )
    return -1;

  for( int i = 0; i < mySegmentNumber; i++ )
    if( myArray[2*i] <= thePoint && thePoint <= myArray[2*i+1] )
      return i;

  return -1;

}

/*!
  Removes segment
  \param theIndex - segment index
*/
bool GLViewer_LineList::removeSegment( int theIndex )
{
  if( theIndex > mySegmentNumber || !myArray)
    return false;

  for( int i = theIndex; i < mySegmentNumber; i++ )
  {
    myArray[i*2] = myArray[(i+1)*2];
    myArray[i*2+1] = myArray[(i+1)*2+1];
  }
  mySegmentNumber--;
  myArray[mySegmentNumber*2] = 0.0;
  myArray[mySegmentNumber*2+1] = 0.0;
  return true;
}

/*!
  Removes segment from line list
  \param coord1, coord2 - co-ordinates of points
*/
bool GLViewer_LineList::removeSegment( double coord1, double coord2 )
{
  if( coord1 > coord2 )
  {
    double temp = coord1;
    coord1 = coord2;
    coord2 = temp;
  }

  if( 2*mySegmentNumber == myRealSize || !myArray )
    return false;

  int index = 0;
  double c1, c2;
  while( index < mySegmentNumber)
  {
    readSegment( index, c1, c2 );
    if( coord1 < c1 && coord2 < c1 )
    {
      //nothing
      return true;
    }
    else if( coord1 < c1 && coord2 < c2 )
    {
      myArray[index*2] = coord2;
      return true;
    }
    else if( c1 < coord1 && coord2 < c2 )
    {
      if( 2*mySegmentNumber == myRealSize )
        return false;
      for( int i = mySegmentNumber; i > index + 1; i-- )
      {
        myArray[2*i] = myArray[2*(i-1)];
        myArray[2*i+1] = myArray[2*(i-1)+1];
      }
      myArray[2*(index+1)+1] = myArray[2*index+1];
      myArray[2*(index+1)] = coord2;
      myArray[2*index+1] = coord1;
      mySegmentNumber++;
      return true;
    }
    else if( coord1 < c2 && c2 < coord2 )
    {
      if( c1 < coord1 )
      {
        myArray[2*index+1] = coord1;
      }

      if( index != mySegmentNumber - 1 )
      {
        for( int i = index+1; i < mySegmentNumber; i++ )
        {
          if( coord2 < myArray[2*i] )
          {
            if( index+1 != i )
            {
              for( int j = 1; i+j-1 < mySegmentNumber;j++ )
              {
                myArray[2*(index+j)] = myArray[2*(i+j-1)];
                myArray[2*(index+j)+1] = myArray[2*(i+j-1)+1];
              }
              for( int k = 0; k < mySegmentNumber - i; k++ )
              {
                myArray[2*(mySegmentNumber - 1- k)] = 0.0;
                myArray[2*(mySegmentNumber - 1- k)+1] = 0.0;
              }
              mySegmentNumber -= i - index -1;
            }
            else
            {
              if( !(c1 < coord1) )
              {
                for( int j = 0; index + j + 1 < mySegmentNumber;j++ )
                {
                  myArray[2*(index+j)] = myArray[2*(index+j+1)];
                  myArray[2*(index+j)+1] = myArray[2*(index+j+1)+1];
                }
                  
                myArray[2*(mySegmentNumber - 1)] = 0.0;
                myArray[2*(mySegmentNumber - 1)+1] = 0.0;
                
                mySegmentNumber --;
              }

            }

            return true;

          }
          else if( coord2 < myArray[2*i+1] )
          {
            if( index+1 != i )
            {
              if( c1 < coord1 )
                index++;

              myArray[2*index] = coord2;
              myArray[2*index+1] = myArray[2*i+1];
            
              for( int j = 1; i+j < mySegmentNumber;j++ )
              {
                myArray[2*(index+j)] = myArray[2*(i+j)];
                myArray[2*(index+j)+1] = myArray[2*(i+j)+1];
              }
              for( int k = 0; k < mySegmentNumber - i - 1; k++ )
              {
                myArray[2*(mySegmentNumber - 1- k)] = 0.0;
                myArray[2*(mySegmentNumber - 1- k)+1] = 0.0;
              }
              mySegmentNumber -= i - index;
            }
            else
            {
              if( c1 < coord1 )
              {
                myArray[2*(index+1)] = coord2;
                return true;
              }
              else
              {
                myArray[2*(index)] = coord2;
                myArray[2*(index)+1] = myArray[2*(index+1)+1];
                for( int j = index+1; j < mySegmentNumber-1; j++ )
                {
                  myArray[2*j] = myArray[2*(j+1)];
                  myArray[2*j+1] = myArray[2*(j+1)+1];
                }
                mySegmentNumber--;
                myArray[2*mySegmentNumber] = 0.0;
                myArray[2*mySegmentNumber+1] = 0.0;
              }
            }
            return true;
          }
        }
      }
      else
      {
        if( !(c1 < coord1) )
        {
          mySegmentNumber--;
          myArray[2*index] = 0.0;
          myArray[2*index+1] = 0.0;
        }
      }
    }    
    index++;
  }
  return true;
}

/*!
  Clears line list
*/
void GLViewer_LineList::clear()
{
  if( myArray )
    memset( myArray, 0, myRealSize*sizeof(double) );
}

/*!
  Prints debug info about line list
*/
void GLViewer_LineList::print()
{
  std::cout << "MainCoord: " << myMainCoord <<" SIZE: " << myRealSize << " ENum: " << mySegmentNumber << " :::";
  for( int i = 0; i < mySegmentNumber; i++ )
    std::cout << "  " << myArray[2*i] << " " << myArray[2*i+1] << " | ";

  std::cout << std::endl;
}

/*!
  Draws line list with help of OpenGL 
  \param theDim - dimension
*/
void GLViewer_LineList::show( FieldDim theDim )
{
  if( !myArray )
    return;

  glColor3f( 1.0, 0.0, 1.0 );
  if( theDim == FD_X )
  {
    glBegin( GL_LINES );
      for( int i = 0; i < mySegmentNumber; i++ )
      {
        glVertex2d( myArray[2*i], myMainCoord );
        glVertex2d( myArray[2*i+1], myMainCoord );
      }
    glEnd();
  }
  else if( theDim == FD_Y )
  {
    glBegin( GL_LINES );
      for( int i = 0; i < mySegmentNumber; i++ )
      {
        glVertex2d( myMainCoord, myArray[2*i]  );
        glVertex2d( myMainCoord, myArray[2*i+1] );
      }
    glEnd();
  }
}


/*!
  Constructor
*/
GLViewer_LineField::GLViewer_LineField()
{
  myCurArrayIndex = 0;
  myGraphArray1 = NULL;
  myGraphArray2 = NULL;

  myCurCount = 0;

  myXSize = 0;    
  myYSize = 0;
  myXLineArray = NULL;
  myYLineArray = NULL;
}

/*!
  Constructor
*/
GLViewer_LineField::GLViewer_LineField( const int theMAXSize, const int theXN, const int theYN )
{
  myCurArrayIndex = 0;
  myGraphArray1 = NULL;
  myGraphArray2 = NULL;

  myCurCount = 0;

  if( theXN <= 0 || theYN <= 0 )
  {
    myXSize = 0;    
    myYSize = 0;
    myXLineArray = NULL;
    myYLineArray = NULL;
  }
  else
  {
    myXLineArray = new GLViewer_LineList*[theXN];
    myYLineArray = new GLViewer_LineList*[theYN];

    for( int i = 0; i < theXN; i++ )
      myXLineArray[i] = new GLViewer_LineList( theMAXSize );

    for( int j = 0; j < theYN; j++ )
      myYLineArray[j] = new GLViewer_LineList( theMAXSize );

    myXSize = theXN;    
    myYSize = theYN;
  }
}

/*!
  Destructor
*/
GLViewer_LineField::~GLViewer_LineField()
{
  if( myXLineArray )
  {
    for( int i = 0; i < myXSize; i++ )
      delete myXLineArray[i];

    delete myXLineArray;
  }

  if( myYLineArray )
  {
    for( int j = 0; j < myYSize; j++ )
      delete myYLineArray[j];

    delete myYLineArray;
  }

  if( myGraphArray1 )
    delete myGraphArray1;

  if( myGraphArray2 )
    delete myGraphArray2;
}

/*!
  Adds line
*/
void GLViewer_LineField::addLine( FieldDim theDim, GLViewer_LineList* )
{
  //not implemented
}

/*!
  Adds line
  \param theDim - dimension
  \param theMC - main co-ordinate
  \param theBegin - start co-ordinate
  \param theEnd - end co-ordinate
*/
void GLViewer_LineField:: addLine( FieldDim theDim, double theMC, double theBegin, double theEnd )
{
  GLViewer_LineList* aLL = new GLViewer_LineList( 1 );
  aLL->addSegment( theBegin, theEnd );
  aLL->setMainCoord( theMC );
  addLine( theDim, aLL );
}

/*!
  Adds line
  \param theDim - dimension
  \param theLL - main co-ordinate
  \param thePosition - index in list
*/
int GLViewer_LineField::insertLine( FieldDim theDim, GLViewer_LineList* theLL, int thePosition )
{
  if( !myXLineArray || !myYLineArray )
    return -1;

  GLViewer_LineList** anArray = getLLArray( theDim );
  if( !anArray )
    return -1;

  int size = getDimSize( theDim ); 

  if( thePosition >= size )
    return -1;
  else if( thePosition < 0 )
  {    
    if( anArray[size-1]->count() != 0 ) // no more space
      return -1;

    for( int i = 0; i < size; i++ )
    {
      if( anArray[i]->count() == 0 )
      {
        delete anArray[i];
        anArray[i] = theLL;
        return i;
      }

      double cur_mc = anArray[i]->mainCoord();
      if( theLL->mainCoord() < cur_mc )
      {        
        for( int j = 0; j+i+1 < size; j++ )
        {
          delete anArray[size-j-1];
          anArray[size-j-1] = anArray[size-j-2];
        }
        delete anArray[i];
        anArray[i] = theLL;
        return i;
      }          
    }
  }
  else
  {
    delete anArray[thePosition];
    anArray[thePosition] = theLL;
    return thePosition;
  }

  return -1;
}

/*!
  Adds line
  \param theDim - dimension
  \param theMainCoord - main co-ordinate
  \param theBegin - start co-ordinate
  \param theEnd - end co-ordinate
  \param thePosition - index in list
*/
int GLViewer_LineField::insertLine( FieldDim theDim, double theMainCoord, double theBegin, double theEnd, int thePosition )
{
  GLViewer_LineList* aLL = new GLViewer_LineList( 1 );
  aLL->addSegment( theBegin, theEnd );
  aLL->setMainCoord( theMainCoord );
  return insertLine( theDim, aLL, thePosition );
}

/*!
  \return other dimension
*/
FieldDim GLViewer_LineField::invertDim( FieldDim theFD )
{
  if( theFD == FD_X )
    return FD_Y;
  else
    return FD_X;
}

/*!
  \return line list
  \param theIndex - index in list
  \param tehFD - dimension
*/
GLViewer_LineList* GLViewer_LineField::getLine( int theIndex, FieldDim theFD )
{
  if( !myXLineArray || !myYLineArray )
    return NULL;

  if( theFD == FD_X )
  {
    if( theIndex > myXSize )
      return NULL;
    
    return myXLineArray[theIndex];
  }
  else if( theFD == FD_Y )
  {
    if( theIndex > myYSize )
      return NULL;
    
    return myYLineArray[theIndex];
  }

  return NULL;
}

/*!
  Sets borders of field
  \param X1, X2 - minimal and maximal abscisses
  \param Y1, Y2 - minimal and maximal ordinates
*/
void GLViewer_LineField::setBorders( double X1, double X2, double Y1, double Y2 )
{
  if( !myXLineArray || !myYLineArray )
    return;
    
  for( int i = 0; i < myXSize; i++ )
  {
    myXLineArray[i]->clear();
    myXLineArray[i]->addSegment( X1, X2 );
    myXLineArray[i]->setMainCoord( Y1 + (Y2-Y1)*(double(i)/(myXSize-1)) );
  }

  for( int j = 0; j < myYSize; j++ )
  {
    myYLineArray[j]->clear();
    myYLineArray[j]->addSegment( Y1, Y2 );
    myYLineArray[j]->setMainCoord( X1 + (X2-X1)*(double(j)/(myYSize-1)) );
  }
} 

/*!
  Adds rectangle
  \param top, right - a corner of rectangle
  \param bottom, left - other corner of rectangle
*/
void GLViewer_LineField::addRectangle( double top, double right, double bottom, double left )
{
  if( !myXLineArray || !myYLineArray )
    return;
  for( int i = 0; i < myXSize; i++ )
  {
    double mainCoord = myXLineArray[i]->mainCoord();
    if( mainCoord < top && mainCoord > bottom )
      myXLineArray[i]->removeSegment( left, right );
  }

  for( int j = 0; j < myYSize; j++ )
  {
    double mainCoord = myYLineArray[j]->mainCoord();
    if( mainCoord < right && mainCoord > left )
      myYLineArray[j]->removeSegment( bottom, top );
  }
}

/*!
  Prints debug info about line field
*/
void GLViewer_LineField::print()
{
  std::cout << "My X matrix Number: " << myXSize << std::endl;
  for( int i = 0; i < myXSize; i++ )
    myXLineArray[i]->print();

  std::cout << "My Y matrix Number: " << myYSize << std::endl;
  for( int j = 0; j < myYSize; j++ )
    myYLineArray[j]->print();
}

/*!
  Draws field with help of OpenGL 
*/
void GLViewer_LineField::show()
{
  for( int i = 0; i < myXSize; i++ )
    getLine( i, FD_X )->show( FD_X );

  for( int j = 0; j < myYSize; j++ )
    getLine( j, FD_Y )->show( FD_Y );
  int count = 0;
  double* anArray = solution( count );
  glColor3f( 1.0, 0.0, 0.0 );
  glBegin( GL_LINES );
  for( int k = 0; k < count; k++ )
  {
     glVertex2d( anArray[4*k], anArray[4*k+1] );
     glVertex2d( anArray[4*k+2], anArray[4*k+3] );
  }
  glEnd();
  delete[] anArray;
  std::cout << "Show function" << std::endl;
}

/*!
  \return size
  \param theDim - dimension
*/
int GLViewer_LineField::getDimSize( FieldDim theDim )
{
  if( theDim == FD_X )
    return myXSize;
  else if( theDim == FD_Y )
    return myYSize;

  return -1;
}

/*!
  \return array of intersected indexes
  \param theDim - dimension
  \param theIndex - index
  \param theLL - line with that intersection is checked
  \param theSize - to return value of array size
*/
int* GLViewer_LineField::intersectIndexes( FieldDim theDim, int theIndex, const GLViewer_LineList* theLL, int& theSize )
{
  theSize = 0;
  if( !myXLineArray || !myYLineArray )
    return NULL;

  int aDimSize = getDimSize( theDim );
  int* anArray = new int[aDimSize*2 ];

  for( int i = 0; i < aDimSize; i++ )
  {
    GLViewer_LineList* aLL = getLine( i, theDim );      
    int index = aLL->contains( theLL->mainCoord() );       
    if( index != -1 && theLL->contains( aLL->mainCoord() ) == theIndex )
    {
      anArray[theSize*2] = i;
      anArray[theSize*2+1] = index;
      theSize++;
    }
  }
  
  return anArray;
}

/*!
  Sets start/end search point
  \param thePoint - type of point (start: FP_Start; end: FP_End )
  \param theX, theY - point co-ordinates
*/
bool GLViewer_LineField::setPoint( FieldPoint thePoint, double theX, double theY )
{
  if( !myXLineArray || !myYLineArray )
    return false;

  int i = -1, j = -1;
  int xSeg = -1, ySeg = -1;
  for( i = 0; i < myXSize; i++ )
  {
    GLViewer_LineList* aLL = getLine( i, FD_X );
    if( aLL->mainCoord() == theY )
    {
      xSeg = aLL->contains( theX );
      break;
    }
  }

  for( j = 0; j < myYSize; j++ )
  {
    GLViewer_LineList* aLL = getLine( j, FD_Y );
    if( aLL->mainCoord() == theX )
    {
      ySeg = aLL->contains( theY );
      break;
    }
  }

  if( xSeg != -1 && ySeg != -1 )
  {
    if( thePoint == FP_Start )
    {
      myStartPoint.myXLineIndex = i;
      myStartPoint.myXSegmentIndex = xSeg;
      myStartPoint.myYLineIndex = j;
      myStartPoint.myYSegmentIndex = ySeg;
      myStartPoint.mySolveIndex = -1;
    }
    else
    {
      myEndPoint.myXLineIndex = i;
      myEndPoint.myXSegmentIndex = xSeg;
      myEndPoint.myYLineIndex = j;
      myEndPoint.myYSegmentIndex = ySeg;
      myEndPoint.mySolveIndex = -1;
    }
    return true;
  }
  else
    return false;
}

/*!
  \return number of segments
*/
int GLViewer_LineField::segmentNumber()
{
  if( !(myXLineArray || myYLineArray) )
    return -1;

  int aNumber = 0;
  for( int aDim = 0; aDim < 2; aDim++ )
    for( int i = 0, n = getDimSize( (FieldDim)aDim ); i < n; i++ )
      aNumber += getLine( i, (FieldDim)aDim  )->count();

  return aNumber;
}

/*!
  Removes all multiple segments
*/
void GLViewer_LineField::optimize()
{
  if( !myXLineArray || !myYLineArray )
    return;

  for( int aDim = 0; aDim < 2; aDim++ )
  {
    for( int i = 0, n = getDimSize( (FieldDim)aDim ); i < n; i++ )
    {
      GLViewer_LineList* aLL = getLine( i, (FieldDim)aDim  );
      for( int k =0, aSegNum = aLL->count(); k < aSegNum; k++ )
      {
        // int index = i; unused
        double a1, a2;
        aLL->readSegment( k, a1, a2 );
        for( int l = i+1, m = getDimSize( (FieldDim)aDim ); l < m; l++ )
        {
          int end = -1;
          GLViewer_LineList* aCurLL = getLine( l, (FieldDim)aDim );
          for( int j = 0, count = aCurLL->count(); j < count; j++  )
          {
            double c1, c2;
            aCurLL->readSegment( j, c1, c2 );
            if( a1 == c1 && a2 == c2 )
            {
              if( !(aDim == 0 && myStartPoint.myXLineIndex == l && myStartPoint.myXSegmentIndex == j) &&
                  !(aDim == 0 && myEndPoint.myXLineIndex == l && myEndPoint.myXSegmentIndex == j) &&
                  !(aDim == 1 && myStartPoint.myYLineIndex == l && myStartPoint.myYSegmentIndex == j) &&
                  !(aDim == 1 && myEndPoint.myYLineIndex == l && myEndPoint.myYSegmentIndex == j) )
                aCurLL->removeSegment( j );
              end = 0;
              break;
            }
            if( a1 < c1 )
            {
              end = 1;
              break;
            }            
          }
          if( end == -1 || end == 1)
              break;
        }
      }
    }
  }
}

/*!
  Some prepare actions
  Needs call setPoint before
*/
void GLViewer_LineField::initialize()
{
  if( !myXLineArray || !myYLineArray )
    return;

  int size = segmentNumber();

  myCurArrayIndex = 0;
  myCurCount = 0;

  myGraphArray1 = new GraphNode[size];
  myGraphArray2 = new GraphNode[size];

  int index = 0;
  bool isXSet = false,
       isYSet = false;
  for( int aDim = 0; aDim < 2; aDim++ )
  {
    for( int i = 0, n = getDimSize( (FieldDim)aDim ); i < n; i++ )
    {
      GLViewer_LineList* aLL = getLine( i, (FieldDim)aDim  );
      for( int k =0, aSegNum = aLL->count(); k < aSegNum; k++ )
      {
        myGraphArray1[index].myCount = size;
        myGraphArray1[index].myDim = (FieldDim)aDim;
        myGraphArray1[index].myLineIndex = i;
        myGraphArray1[index].mySegmentindex = k;
        myGraphArray1[index].prevNodeIndex = -1;

        myGraphArray2[index].myCount = size;
        myGraphArray2[index].myDim = (FieldDim)aDim;
        myGraphArray2[index].myLineIndex = i;
        myGraphArray2[index].mySegmentindex = k;
        myGraphArray2[index].prevNodeIndex = -1;

        if( !isXSet && aDim == FD_X && myStartPoint.myXLineIndex == i && myStartPoint.myXSegmentIndex == k )
        {
          myGraphArray1[index].myCount = 0;
          isXSet = true;
        }

        if( aDim == FD_Y && !isYSet && myStartPoint.myYLineIndex == i && myStartPoint.myYSegmentIndex == k )
        {
          myGraphArray1[index].myCount = 0;
          isYSet = true;
        }

        index++;
      }
    }
  }
}

/*!
  One iteration of algorithm
*/
void GLViewer_LineField::iteration()
{
  int aParam = myCurCount;
  myCurCount++;

  int* aNodes = findByCount( aParam );
  GraphNode* aCurArray = getCurArray();

  for( int i = 0; i < aParam; i++ )
  {
    GraphNode aCurNode = aCurArray[aNodes[i]];
    int aSize = 0;
    int* aInterNodes = intersectIndexes( invertDim( aCurNode.myDim ), aCurNode.mySegmentindex,
                                         getLine( aCurNode.myLineIndex, aCurNode.myDim ), aSize );
    for( int j = 0; j < aSize; j++ )
    {
      int index = findBySegment( invertDim( aCurNode.myDim ), aInterNodes[2*j], aInterNodes[2*j+1], false );
      if( index != -1 )
        if( aCurArray[index].myCount > myCurCount )
        {
          aCurArray[index].myCount = myCurCount;
          aCurArray[index].prevNodeIndex = aNodes[i];
        }
    }

    delete[] aInterNodes;
  }

  delete[] aNodes;
}

/*!
  Checks for complete status
*/
GLViewer_LineField::IterationStatus GLViewer_LineField::checkComplete()
{
  if( !myXLineArray || !myYLineArray || !myGraphArray1 || !myGraphArray2 )
    return IS_ERROR; 
   
  int count = 0;
  GraphNode* aCurArray = getCurArray(),
           * aSecArray = getSecArray();
  
  for( int i = 0, n = segmentNumber(); i < n; i++ )
  {
    if( aCurArray[i].myCount != aSecArray[i].myCount )
    {
      if( aCurArray[i].myDim == FD_X && 
          aCurArray[i].myLineIndex == myEndPoint.myXLineIndex && 
          aCurArray[i].mySegmentindex == myEndPoint.myXSegmentIndex )
      {
        std::cout << "Algorithm complete X!!!!!!!" << std::endl;
        myEndPoint.mySolveIndex = i;
        return IS_SOLVED;
      }
      else if( aCurArray[i].myDim == FD_Y && 
               aCurArray[i].myLineIndex == myEndPoint.myYLineIndex && 
               aCurArray[i].mySegmentindex == myEndPoint.myYSegmentIndex )
      {
        std::cout << "Algorithm complete Y!!!!!!!" << std::endl;
        myEndPoint.mySolveIndex = i;  
        return IS_SOLVED;
      }
      else
      {
        count++;
        aSecArray[i].myCount = aCurArray[i].myCount;
        aSecArray[i].prevNodeIndex = aCurArray[i].prevNodeIndex;
      }
    }
  }  
  
  if( myCurArrayIndex == 0)
    myCurArrayIndex = 1;
  else
    myCurArrayIndex = 0;

  std::cout << "Number of ways: " << count << std::endl;
  if( count == 0 )
    return IS_LOOP;

  return IS_NOT_SOLVED;
}

/*!
  Finds LineList by counts and returns indexes
*/
int* GLViewer_LineField::findByCount( int& theParam )
{
  if( !myXLineArray || !myYLineArray || !myGraphArray1 || !myGraphArray2 )
    return NULL;

  int count = segmentNumber();
  int* anArray = new int[count];
  int aSize = 0;

  GraphNode* aCurArray = getCurArray();  
  for( int i = 0; i < count; i++ )
  {
    GraphNode aCurNode = aCurArray[i];
    if( aCurNode.myCount == theParam )
    {
      anArray[aSize] = i;
      aSize++;
    }
  }

  theParam = aSize;
  return anArray;
}

/*!
  Finds LineList by segment and dimension
*/
int GLViewer_LineField::findBySegment( FieldDim theDim, int theLineIndex, int theSegment, bool inCurArray )
{
  if( !myXLineArray || !myYLineArray || !myGraphArray1 || !myGraphArray2 || getDimSize( theDim ) <= theLineIndex )
    return -1;

  GraphNode* aCurArray;
  if( inCurArray )
    aCurArray = getCurArray();
  else
    aCurArray = getSecArray();

  for( int i = 0, n = segmentNumber(); i < n; i++ )
  {
    GraphNode aCurNode = aCurArray[i];
    if( aCurNode.myDim == theDim && aCurNode.myLineIndex == theLineIndex && aCurNode.mySegmentindex == theSegment )
      return i;
  }

  return -1;
}

/*!
  Main method, performs algorithm execution
*/
GLViewer_LineField::EndStatus GLViewer_LineField::startAlgorithm()
{
  if( !myXLineArray || !myYLineArray || !myGraphArray1 || !myGraphArray2 )
    return ES_ERROR;

  while( true )
  {
    std::cout << "-----------Iteration #" << myCurCount << "-------------" << std::endl;
    iteration();

    IterationStatus is = checkComplete();
    if( is == IS_ERROR )
      return ES_ERROR;
    else if( is == IS_LOOP )
      return ES_LOOP;
    else if( is == IS_SOLVED )
      return ES_SOLVED;
  }
  return ES_SOLVED;
}

/*!
  \return solution and size of solution
*/
double* GLViewer_LineField::solution( int& theSize )
{
  if( !myXLineArray || !myYLineArray || !myGraphArray1 || !myGraphArray2 )
    return NULL;

  if( myEndPoint.mySolveIndex == -1 )
    return NULL;

  theSize = myCurCount+1;
  double* anArray = new double[theSize*4];

  GraphNode* aCurArray = getCurArray();
  
  int index = myEndPoint.mySolveIndex;
  for( int i = 0; i <= myCurCount; i++  )
  {
    if( index == -1 )
      break;
    double c1, c2;
    GLViewer_LineList* aLL = getLine( aCurArray[index].myLineIndex, aCurArray[index].myDim );
    aLL->readSegment( aCurArray[index].mySegmentindex, c1, c2 );

    if( aCurArray[index].myDim == FD_X )
    {
      anArray[i*4] = c1;
      anArray[i*4+1] = aLL->mainCoord();
      anArray[i*4+2] = c2;
      anArray[i*4+3] = aLL->mainCoord();
    }
    else
    {
      anArray[i*4] = aLL->mainCoord();
      anArray[i*4+1] = c1;
      anArray[i*4+2] = aLL->mainCoord();
      anArray[i*4+3] = c2;
    }

    index = aCurArray[index].prevNodeIndex;    
  }

  return anArray;
}

/*!
  \return current solution array
*/
GraphNode* GLViewer_LineField::getCurArray()
{
  if( !myGraphArray1 || !myGraphArray2 )
    return NULL;

  if( myCurArrayIndex == 0)
    return myGraphArray1;
  else
    return myGraphArray2;
}

/*!
  \return other solution array
*/
GraphNode* GLViewer_LineField::getSecArray()
{
  if( !myGraphArray1 || !myGraphArray2 )
    return NULL;

  if( myCurArrayIndex == 0)
    return myGraphArray2;
  else
    return myGraphArray1;
}

/*!
  \return maximum segment number
*/
int GLViewer_LineField::maxSegmentNum()
{
  if( !myXLineArray || !myYLineArray )
    return -1;

  int max_num = -1;
  for( int aDim = 0; aDim < 2; aDim++ )
  {
    for( int i = 0, n = getDimSize( (FieldDim)aDim ); i < n; i++ )
    {
      int count = getLine( i, (FieldDim)aDim  )->count();
      if( count > max_num )
        max_num = count;
    }
  }

  return max_num;
}

/*!
  \return list of LileList by dimension
  \param theDim - dimension
*/
GLViewer_LineList** GLViewer_LineField::getLLArray( FieldDim theDim )
{
  if( theDim == FD_X )
    return myXLineArray;
  else if( theDim == FD_Y )
    return myYLineArray;
  else
    return NULL;
}
