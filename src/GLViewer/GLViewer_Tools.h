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
// File:      GLViewer_Tools.h
// Created:   April, 2005
//
#ifndef GLVIEWER_TOOLS_H
#define GLVIEWER_TOOLS_H

#ifdef WIN32
#include "windows.h"
#endif

#include "GLViewer.h"

/*!
  \class GLViewer_Tools 
  Tools for Viewer
*/
class GLVIEWER_API GLViewer_Tools
{
public:
        //GLViewer_Tools();
        //virtual ~GLViewer_Tools();
  //static 

};

//! Dimension of line
enum FieldDim
{
  FD_X = 0, /*along x axis*/
  FD_Y      /*along y axis*/
};

/*!
  \class GLViewer_LineList 
  Tools for distinct line
  This class implmented interface for segment operations:
  add, cut, remove and etc.
  Memory does not changed and allocated only one time
*/
class GLViewer_LineList  
{
public:
  GLViewer_LineList( int  );
  virtual ~GLViewer_LineList();

  //! Returns number of segments
  int         count() const { return mySegmentNumber; }
  //! Returns real size
  int         size() const { return myRealSize; }
  
  bool        addSegment( double coord1, double coord2 );
  bool        removeSegment( int index );
  bool        removeSegment( double coord1, double coord2 );

  bool        readSegment( int index, double& coord1, double& coord2 );

  //! Returns index of segment, else -1
  int         contains( double thePoint ) const;

  //! Sets level of segments
  void        setMainCoord( double theVal ) { myMainCoord = theVal; }
  double      mainCoord() const { return myMainCoord; }

  void        clear();
  void        print();

  void        show( FieldDim );
  
  GLViewer_LineList& operator = ( GLViewer_LineList );

private:
  double*     myArray;
  int         myRealSize;
  int         mySegmentNumber;

  double      myMainCoord;
};

/*! struct GraphNode describe node in algorithm on rare grid*/
struct GraphNode
{
  int       myCount;
  FieldDim  myDim;
  int       myLineIndex;
  int       mySegmentindex;
  int       prevNodeIndex; //feedback for searching for solution
};

/*! struct SearchPoint describe node for solving algorithm*/
struct SearchPoint
{
  int       myXLineIndex;
  int       myXSegmentIndex;
  int       myYLineIndex;
  int       myYSegmentIndex;
  int       mySolveIndex;
};

/*! 
  \class  GLViewer_LineField 
  Tools for solving algorithm of finding shortest path on rare grid with minimum of 
  line turns number
*/
class GLViewer_LineField
{
public:
  //!Searched point
  enum  FieldPoint
  {
    FP_Start = 0,
    FP_End = 1
  };

  //! Status of interation
  enum IterationStatus
  {
    IS_ERROR = 0,
    IS_LOOP,
    IS_NOT_SOLVED,
    IS_SOLVED
  };

  //! Final status of solving
  enum EndStatus
  {
    ES_ERROR = 0,
    ES_LOOP,
    ES_SOLVED
  };

  GLViewer_LineField();
  GLViewer_LineField( const int theMAXSize, const int xn, const int yn );
  virtual ~GLViewer_LineField();

  //! Adds new line
  /*!best way, if line is already sorted*/
  void                addLine( FieldDim, GLViewer_LineList* );
  //! Calls previous
  void                addLine( FieldDim theDim, double theMC, double theBegin, double theEnd );
  
  //! Adds new line and sorted field
  /*! Returns position*/
  int                 insertLine( FieldDim theDim, GLViewer_LineList*, int thePosition );
  //! Calls previous
  int                 insertLine( FieldDim theDim, double theMC, double theBegin, double theEnd, int thePosition );

  //! Returns other dimension
  static FieldDim     invertDim( FieldDim );

  //! Returns line by index and dimension
  GLViewer_LineList*  getLine( int index, FieldDim );

  //! Nullifys field and sets same continued segments
  void                setBorders( double X1, double X2, double Y1, double Y2 );
  //! Cut rectangle in grid
  void                addRectangle( double top, double right, double bottom, double left );

  //! returns arrey of intersects indexes with \param theLL
  int*                intersectIndexes( FieldDim theDim, int theIndex, const GLViewer_LineList* theLL , int& theSize );

  void                print();

  void                show();  

  int                 getDimSize( FieldDim );
  //! Returns number of segment
  int                 segmentNumber();

  //! Sets start/end search point
  bool                setPoint( FieldPoint, double x, double y );

  //! Optimize field
  /*! Removes all multiple segments*/
  void                optimize();
  //! Some prepare actions
  /*! Needs call setPoint before*/
  void                initialize();
  //! Main method
  EndStatus           startAlgorithm();

  //! Returns solution and size of solution
  double*             solution( int& size );

protected:
  //! One iteration of algorithm
  void                iteration();
  //! Checks for complete status
  IterationStatus     checkComplete();  

  //! Finds LineList by counts and returns indexes
  int*                findByCount( int& theParam );
  //! Finds LineList by segment and dimension
  int                 findBySegment( FieldDim, int coord1, int coord2, bool inCurArray = true );

  //! Returns current solution array
  GraphNode*          getCurArray();
  //! Returns 
  GraphNode*          getSecArray();

  //! Returns maximum segment number
  int                 maxSegmentNum();

  //! Returns list of LileList by dimension
  GLViewer_LineList** getLLArray( FieldDim );

private:
  GLViewer_LineList**    myXLineArray,
                   **    myYLineArray;

  int           myXSize,
                myYSize;

  GraphNode*    myGraphArray1,
           *    myGraphArray2;
  int           myCurArrayIndex;

  SearchPoint   myStartPoint,
                myEndPoint;
  int           myCurCount;
};

#endif //GLVIEWER_TOOLS_H
