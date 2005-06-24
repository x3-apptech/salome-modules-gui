// File:      GLViewer_Tools.h
// Created:   April, 2005
// Author:    OCC team

#ifndef GLVIEWER_TOOLS_H
#define GLVIEWER_TOOLS_H

#ifdef WNT
#include "windows.h"
#endif

#include "GLViewer.h"
#include "GLViewer_Defs.h"

class GLViewer_LineField;

/****************************************************************************
**  Class:   GLViewer_Tools 
**  Descr:   Tools for Viewer
**  Module:  GLViewer
**  Created: UI team, 27.10.05
*****************************************************************************/
class GLVIEWER_API GLViewer_Tools
{
public:
	//GLViewer_Tools();
	//virtual ~GLViewer_Tools();
  //static 

};

enum FieldDim
{
  FD_X = 0,
  FD_Y = 1
};

/****************************************************************************
**  Class:   GLViewer_LineList 
**  Descr:   Tools for distinct line
**  Module:  GLViewer
**  Created: UI team, 27.10.05
*****************************************************************************/
class GLViewer_LineList  
{
public:
	GLViewer_LineList( int  );
	virtual ~GLViewer_LineList();

  int         count() const { return mySegmentNumber; }
  int         size() const { return myRealSize; }

  bool        addSegment( double coord1, double coord2 );
  bool        removeSegment( int index );
  bool        removeSegment( double coord1, double coord2 );

  bool        readSegment( int index, double&, double& );

  int         contains( double thePoint ) const;

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

struct GraphNode
{
  int       myCount;
  FieldDim  myDim;
  int       myLineIndex;
  int       mySegmentindex;
  int       prevNodeIndex; //feedback for searching for solution
};

struct SearchPoint
{
  int       myXLineIndex;
  int       myXSegmentIndex;
  int       myYLineIndex;
  int       myYSegmentIndex;
  int       mySolveIndex;
};

/****************************************************************************
**  Class:   GLViewer_LineField 
**  Descr:   Tools for solving 
**  Module:  GLViewer
**  Created: UI team, 27.10.05
*****************************************************************************/
class GLViewer_LineField
{
public:
  enum  FieldPoint
  {
    FP_Start = 0,
    FP_End = 1
  };

  enum IterationStatus
  {
    IS_ERROR = 0,
    IS_LOOP,
    IS_NOT_SOLVED,
    IS_SOLVED
  };

  enum EndStatus
  {
    ES_ERROR = 0,
    ES_LOOP,
    ES_SOLVED
  };

  GLViewer_LineField();
  GLViewer_LineField( const int theMAXSize, const int xn, const int yn );
  virtual ~GLViewer_LineField();

  //best way, if line is already sorted
  void                addLine( FieldDim, GLViewer_LineList* );
  void                addLine( FieldDim theDim, double theMC, double theBegin, double theEnd );
//  void                addLine( FieldDim, double theMainCoord, double theBegin, double theEnd ):
  
  int                 insertLine( FieldDim theDim, GLViewer_LineList*, int thePosition ); // return position
  int                 insertLine( FieldDim theDim, double theMC, double theBegin, double theEnd, int thePosition ); // return position

  static FieldDim     invertDim( FieldDim );

  GLViewer_LineList*  getLine( int index, FieldDim );

  void                setBorders( double X1, double X2, double Y1, double Y2 );
  void                addRectangle( double top, double right, double bottom, double left );

  int*                intersectIndexes( FieldDim theDim, int theIndex, const GLViewer_LineList* theLL , int& theSize );

  void                print();

  void                show();  

  int                 getDimSize( FieldDim );
  int                 segmentNumber();

  bool                setPoint( FieldPoint, double, double );

  void                optimize();
  void                initialize();//needs call setPoint before
  EndStatus           startAlgorithm();//main method

  double*             solution( int& );

protected:
  void                iteration();
  IterationStatus     checkComplete();  

  int*                findByCount( int& theParam );
  int                 findBySegment( FieldDim, int, int, bool inCurArray = true );

  GraphNode*          getCurArray();
  GraphNode*          getSecArray();

  int                 maxSegmentNum();

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
