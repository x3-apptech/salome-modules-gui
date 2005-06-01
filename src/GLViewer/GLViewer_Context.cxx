// File:      GLViewer_Context.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

/****************************************************************************
**  Class:   GLViewer_Context 
**  Descr:   OpenGL Context
**  Module:  GLViewer
**  Created: UI team, 20.09.02
*****************************************************************************/

#include "GLViewer_Context.h"

#include "GLViewer_Object.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_ViewPort2d.h"

#include <TColStd_SequenceOfInteger.hxx>

#define TOLERANCE  12

GLViewer_Context::GLViewer_Context( GLViewer_Viewer2d* v ) :
       myGLViewer2d( v ),
       myHighlightColor( Quantity_NOC_CYAN1 ),
       mySelectionColor( Quantity_NOC_RED ),
       myTolerance( TOLERANCE ),
       myNumber( 0 )
{
  myLastPicked = NULL;
  isLastPickedChanged = false;
  myHFlag = GL_TRUE;
  mySFlag = GL_TRUE;
  mySelCurIndex = 0;
}

GLViewer_Context::~GLViewer_Context()
{
}

int GLViewer_Context::MoveTo( int xi, int yi, bool byCircle )
{
  GLfloat x = (GLfloat)xi;
  GLfloat y = (GLfloat)yi;
  myGLViewer2d->transPoint( x, y );
  //cout << "GLViewer_Context::MoveTo " << x << " " << y << endl;

  QValueList<GLViewer_Object*> anUpdatedObjects;
  
  ObjectMap::Iterator it;
  
  myXhigh = x;
  myYhigh = y;  
  
  //if( !myHFlag )
  //  return 1;

  GLfloat aXScale;
  GLfloat aYScale;
  GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )myGLViewer2d->getActiveView()->getViewPort();
  vp->getScale( aXScale, aYScale );

  //bool onlyUpdate = true;
  GLboolean update = GL_FALSE;
  GLboolean isHigh = GL_FALSE;
  GLboolean onObject = GL_FALSE;
  
  GLViewer_Object* aPrevLastPicked = myLastPicked;
  GLViewer_Object* lastPicked = NULL;
  QRect* rect;
  QRegion obj;
  QRegion intersection;
  QRect region;

  region.setLeft( (int)(x - myTolerance) );
  region.setRight( (int)(x + myTolerance) );
  region.setTop( (int)(y - myTolerance) );
  region.setBottom( (int)(y + myTolerance) );

  for( it = myObjects.begin(); it != myObjects.end(); ++it )
  {
    it.key()->setScale( aXScale, aYScale );
    rect = it.key()->getUpdateRect()->toQRect();
    obj = QRegion( *rect );

    if( !byCircle && rect->intersects( region ) )
    {
      //      cout << "object : " << it.data() << endl;
      update = it.key()->highlight( x, y, myTolerance, GL_FALSE );
      isHigh = it.key()->isHighlighted();
      onObject = GL_TRUE;
      //if( update )
      //    cout << "update" << endl;
      //if( isHigh )
      //    cout << "highlight" << endl;
    }

    if( byCircle )
    {
      QRegion circle( (int)(x - myTolerance), (int)(y - myTolerance),
		      (int)(2 * myTolerance), (int)(2 * myTolerance), QRegion::Ellipse );
      intersection = obj.intersect( circle );
      if( !intersection.isEmpty() )
      {
	update = it.key()->highlight( x, y, myTolerance, GL_TRUE );
	isHigh = it.key()->isHighlighted();
	onObject = GL_TRUE;
      }
    }

    if( isHigh )
    {
      lastPicked = it.key();
      break;
    }
  }

  if( !myHFlag )
  {
    myLastPicked = lastPicked; //we need this information everytime
    return -1;
  }
  
  if ( !onObject )
  {
    for( it = myObjects.begin(); it != myObjects.end(); ++it )
      if( it.key()->unhighlight() )
	anUpdatedObjects.append( it.key() );
    
    myLastPicked = NULL;
    isLastPickedChanged = aPrevLastPicked != myLastPicked;
    
    if( isLastPickedChanged )
      myGLViewer2d->updateAll();  
    
    return 0;
  }
  
  if( !myLastPicked && isHigh )
  {
    //cout << "1" << endl;
    myLastPicked = lastPicked;
    myHNumber = myObjects[ lastPicked ];
    anUpdatedObjects.append( myLastPicked );
  }
  else if( myLastPicked && !isHigh )
  {
    //cout << "2" << endl;
    //onlyUpdate = 
    myLastPicked->unhighlight();
    anUpdatedObjects.append( myLastPicked );
    //eraseObject( myLastPicked, true );
    myLastPicked = NULL;
    myHNumber = -1;
  }
  else if( myLastPicked && isHigh )
  {
    //cout << "3" << endl;
    //cout << "HNumber" << myHNumber << endl;

    myLastPicked->highlight( x, y, myTolerance, byCircle );
    if( !myLastPicked->isHighlighted() )
    {
      myLastPicked->unhighlight();
      anUpdatedObjects.append( myLastPicked );
      myLastPicked = lastPicked;
      myHNumber = myObjects[ lastPicked ];
    }
    anUpdatedObjects.append( myLastPicked );
  }

  isLastPickedChanged = ( aPrevLastPicked != myLastPicked );

  if( isLastPickedChanged/*!onlyUpdate*/ )
    {
      myGLViewer2d->updateAll();
      //myGLViewer2d->activateAllDrawers( true );
    }
  else
    myGLViewer2d->activateDrawers( anUpdatedObjects, TRUE, TRUE );

  return 0;
}

int GLViewer_Context::Select( bool Append, bool byCircle )
{
  //cout << "GLViewer_Context::Select " << (int)Append << endl;
  QValueList<int>::Iterator it;
  ObjectMap::Iterator oit;
  SelectionStatus status = SS_Invalid;

  bool onlyUpdate = false;

  QValueList<GLViewer_Object*> aList;

  if ( !mySFlag )
    return status;//invalid
  
  if( myHFlag && myLastPicked )
  {
    if( mySelNumbers.count() == 1 && mySelNumbers.first() == myHNumber )
      status = SS_LocalChanged;
    
    if ( !Append )
    {
      for( it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
        for( oit = myObjects.begin(); oit != myObjects.end(); ++oit )
          if( *it == myObjects[ oit.key() ] )
            if ( myLastPicked != oit.key() )
            {
                onlyUpdate = oit.key()->unselect();
                aList.append( oit.key() );
            }

      if( onlyUpdate )
        myGLViewer2d->updateAll();
      else
        myGLViewer2d->activateDrawers( aList, TRUE, TRUE );

      if( mySelNumbers.count() != 0 && status == SS_Invalid )
	status = SS_GlobalChanged;
      mySelNumbers.clear();
    } 

    if ( myLastPicked->select( myXhigh, myYhigh, myTolerance, GLViewer_Rect(), false, byCircle, Append )
      && mySelNumbers.findIndex( myHNumber ) == -1 )
      /*if ( myLastPicked->select( myXhigh, myYhigh, myTolerance, byCircle, Append ) )
      //&&( mySelNumbers.findIndex( myHNumber ) == -1 ) )*/
    {
      mySelNumbers.append( myHNumber );
      //cout << "context::select object #" << myHNumber << endl;
      myGLViewer2d->activateDrawer( myLastPicked, TRUE, TRUE );
      
      if( status == SS_Invalid )
	status = SS_GlobalChanged;
//      else
//        status = SS_GlobalChanged;
    }
    else if( status == SS_LocalChanged )
      status = SS_GlobalChanged;
    
    return status;
  }
  
  if( myHFlag && !myLastPicked )
  {
    if ( !Append )
    {
      for( it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
        for( oit = myObjects.begin(); oit != myObjects.end(); ++oit )
          if( *it == myObjects[ oit.key() ] )
          {
            onlyUpdate = oit.key()->unselect();
            //list.Append( *it );
            aList.append( oit.key() );
          } 
     
      if( onlyUpdate )
          myGLViewer2d->updateAll();
      else
	myGLViewer2d->activateDrawers( aList, TRUE, TRUE );

      if( mySelNumbers.count() != 0 )
	status = SS_GlobalChanged;
      
      mySelNumbers.clear();
      //cout << " myHFlag && !myLastPicked  " << endl;
    }
    return status;
  }

//   if( !myHFlag )
//   {
//     GLViewer_Object* lastPicked = NULL;
//     GLboolean update = FALSE;
//     GLboolean isSel = GL_FALSE;
//     list = new int[2];
//     number = 0;
//     float xa, xb, ya, yb;
//     QRect* rect;

//     ObjectMap::Iterator it;
//     for( it = myObjects.begin(); it != myObjects.end(); ++it )
//     {
//       rect = it.key()->getRect();
//       xa = rect->left();
//       xb = rect->right();
//       ya = rect->top();
//       yb = rect->bottom();      
      
//       if( myXhigh >= xa && myXhigh <= xb && myYhigh >= ya && myYhigh <= yb )
//       {
//  update = it.key()->select( myXhigh, myYhigh, myTolerance, byCircle, Append );
//  isSel = it.key()->isSelected();
//  if( isSel )
//  {
//    myLastPicked = it.key();
//    number = myObjects[ lastPicked ];
//  }
//       }
//     }

//     if ( !Append )
//     {
//       for( it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
//  for( it1 = myObjects.begin(); it1 != myObjects.end(); ++it1 )
//    if( *it == myObjects[ it1.key() ] )
//    {
//      it1.key()->unselect();
//      myGLViewer2d->initDrawer( it1.key(), it.data() );
//      list.Append( *it );
//    }      
//       myGLViewer2d->activateDrawers( list, TRUE );
//       mySelNumbers.clear();
//     }
//     if ( mySelNumbers.findIndex( myObjects[lastPicked] ) == -1)
//       mySelNumbers.append( myObjects[lastPicked] );

//     number = mySelNumbers.count();
//     list = new int[number + 1];
//     list[0] = number;
    
//     for( it = mySelNumbers.begin(), i = 1; it != mySelNumbers.end(); ++it, i++ )
//       list[i] = *it;

//     myGLViewer2d->initDrawer( myLastPicked, myHNumber );
//     myGLViewer2d->activateDrawers( list, TRUE );
//   }

  if( !myHFlag )
  {
    bool isSel = false;
    GLfloat aXScale;
    GLfloat aYScale;
    GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )myGLViewer2d->getActiveView()->getViewPort();
    vp->getScale( aXScale, aYScale );
    
    QRect* rect;
    QRect region;
    QRegion obj;
    
    region.setLeft( ( int )( myXhigh - myTolerance ) );
    region.setRight( ( int )( myXhigh + myTolerance ) );
    region.setTop( ( int )( myYhigh - myTolerance ) );
    region.setBottom( ( int )( myYhigh + myTolerance ) );
    
    QRegion circle( ( int )( myXhigh - myTolerance ), ( int )( myYhigh - myTolerance ),
		    2 * myTolerance, 2 * myTolerance, QRegion::Ellipse );
    
    if ( !Append )
    {
      for( it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
	for( oit = myObjects.begin(); oit != myObjects.end(); ++oit )
	  if( *it == myObjects[ oit.key() ] )
	  {
	    onlyUpdate |= (bool)oit.key()->unselect();
	    aList.append( oit.key() );
	  }
      
      if( onlyUpdate )
	myGLViewer2d->updateAll();
      else
	myGLViewer2d->activateDrawers( aList, TRUE, TRUE );
      
      if( mySelNumbers.count() != 0 )
	status = SS_GlobalChanged;

      mySelNumbers.clear();
    }        

    for( oit = myObjects.begin(); oit != myObjects.end(); ++oit )
    {
      oit.key()->setScale( aXScale, aYScale );
      rect = oit.key()->getUpdateRect()->toQRect();
      obj = QRegion( *rect );
      
      if( !byCircle && rect->intersects( region ) )
      {
	oit.key()->select( myXhigh, myYhigh, myTolerance, GLViewer_Rect(), false, byCircle, Append );
	isSel = oit.key()->isSelected();
      }
      
      if( byCircle && !obj.intersect( circle ).isEmpty() )
      {
	oit.key()->select( myXhigh, myYhigh, myTolerance, GLViewer_Rect(), false, byCircle, Append );
	isSel = oit.key()->isSelected();
      }
      if( isSel )
      {
	myLastPicked = oit.key();
	mySelNumbers.append( myObjects[ myLastPicked ] );
	myGLViewer2d->activateDrawer( myLastPicked, TRUE, TRUE );
	status = SS_GlobalChanged;
	return status;
      }
    }
  }
        
  return SS_NoChanged;
}

int GLViewer_Context::SelectByRect( const QRect& theRect, bool Append )
{
    GLfloat aXScale;
    GLfloat aYScale;
    GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )myGLViewer2d->getActiveView()->getViewPort();
    vp->getScale( aXScale, aYScale );

    SelectionStatus status = SS_NoChanged;

    QValueList<int>::Iterator it;
    ObjectMap::Iterator oit;

    QValueList<GLViewer_Object*> aList;

    if ( !mySFlag || myObjList.empty() )
        return SS_Invalid;

    bool updateAll = false;
    if( !Append )
    {
        if( mySelNumbers.count() != 0 )
            status = SS_GlobalChanged;

        for( it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
            for( oit = myObjects.begin(); oit != myObjects.end(); ++oit )
                if( *it == myObjects[ oit.key() ] )
                {
                    updateAll |= (bool)oit.key()->unselect();
                    aList.append( oit.key() );
                }
        mySelNumbers.clear();
    }

    for( oit = myObjects.begin(); oit != myObjects.end(); ++oit )
    {
        bool isSel = false;
        oit.key()->setScale( aXScale, aYScale );
        QRect rect = //myGLViewer2d->getWinObjectRect( oit.key() ); //->getUpdateRect()->toQRect();
          myGLViewer2d->getQRect( *(oit.key()->getRect()) );

        if( rect.intersects( theRect ) )
        {
            GLViewer_Rect aRect = myGLViewer2d->getGLVRect( theRect );
            oit.key()->select( myXhigh, myYhigh, myTolerance, aRect, false, false, Append );
            isSel = oit.key()->isSelected();
        }

        if( isSel && mySelNumbers.findIndex( oit.data() ) == -1 )
        {
            aList.append( oit.key() );
            mySelNumbers.append( oit.data() );
            status = SS_GlobalChanged;
        }
    }

    if( updateAll ) //i.e only update
    {
        //cout << "Unhilight.ALL" << endl;
        myGLViewer2d->updateAll();
    }
    else
        myGLViewer2d->activateDrawers( aList, TRUE, TRUE );

    return status;
}

void GLViewer_Context::SetHighlightColor( Quantity_NameOfColor aCol )
{
  myHighlightColor = aCol;
  
  Quantity_Color colorH( aCol );
  int redH = 255 * (int)colorH.Red();
  int greenH = 255 * (int)colorH.Green();
  int blueH = 255 * (int)colorH.Blue();
  QColor colH = QColor( redH, greenH, blueH );

  Quantity_Color colorS( mySelectionColor );
  int redS = 255 * (int)colorS.Red();
  int greenS = 255 * (int)colorS.Green();
  int blueS = 255 * (int)colorS.Blue();
  QColor colS = QColor( redS, greenS, blueS );

  myGLViewer2d->updateColors( colH, colS);
}

void GLViewer_Context::SetSelectionColor( Quantity_NameOfColor aCol )
{
  mySelectionColor = aCol;
  
  Quantity_Color colorH( myHighlightColor );
  int redH = 255 * (int)colorH.Red();
  int greenH = 255 * (int)colorH.Green();
  int blueH = 255 * (int)colorH.Blue();
  QColor colH = QColor( redH, greenH, blueH );

  Quantity_Color colorS( aCol );
  int redS = 255 * (int)colorS.Red();
  int greenS = 255 * (int)colorS.Green();
  int blueS = 255 * (int)colorS.Blue();
  QColor colS = QColor( redS, greenS, blueS );

  myGLViewer2d->updateColors( colH, colS);
}

int GLViewer_Context::NbSelected()
{
  return mySelNumbers.count();
}

void GLViewer_Context::InitSelected()
{
  mySelCurIndex = 0;
}

bool GLViewer_Context::MoreSelected()
{
  return ( mySelCurIndex < NbSelected() );
}

bool GLViewer_Context::NextSelected()
{
  if ( mySelCurIndex >= 0 && mySelCurIndex < NbSelected() )
  {
    mySelCurIndex++;
    return TRUE;
  }

  return FALSE;
}

GLViewer_Object* GLViewer_Context::SelectedObject()
{
    ObjectMap::Iterator it;
    for( it = myObjects.begin(); it != myObjects.end(); ++it )
        if( mySelNumbers[mySelCurIndex] == it.data() )
            return it.key();

    return NULL;
}

bool  GLViewer_Context::isSelected( GLViewer_Object* theObj )
{
  for( InitSelected(); MoreSelected(); NextSelected() )
    if( SelectedObject() == theObj )
      return true;

  return false;
}

int GLViewer_Context::insertObject( GLViewer_Object* object, bool display )
{
//  cout << "GLViewer_Context::insertObject" << endl;
  
    myObjects.insert( object, myNumber++ );
    myObjList.append( object );

    if( display )
    {
        //QRect* rect = object->getRect()->toQRect();
        //myGLViewer2d->updateBorders( *rect );
        myGLViewer2d->activateDrawer( object, FALSE );
    }

    return myNumber - 1;
}

bool GLViewer_Context::replaceObject( GLViewer_Object* oldObject, GLViewer_Object* newObject )
{
  if( myObjects.contains( oldObject ) )
  {
    int index = myObjects[ oldObject ];
    myObjects.remove( oldObject );
    myObjects.insert( newObject, index );
    return GL_TRUE;
  }

  return GL_FALSE;
}

void GLViewer_Context::updateScales( GLfloat scX, GLfloat scY )
{
  if( scX && scY )
    for ( ObjectMap::Iterator it = myObjects.begin(); it != myObjects.end(); ++it )
      it.key()->setScale( scX, scY );
}

void GLViewer_Context::clearSelected( bool updateViewer )
{
  QValueList<int>::Iterator it;
  ObjectMap::Iterator oit;

  TColStd_SequenceOfInteger list;

  if( !mySFlag )
    return ;

  for( it = mySelNumbers.begin(); it != mySelNumbers.end(); ++it )
    for( oit = myObjects.begin(); oit != myObjects.end(); ++oit )
      if( *it == myObjects[ oit.key() ] )
      {         
        oit.key()->unselect();
        //myGLViewer2d->initDrawer( oit.key(), oit.data() );
        ///myGLViewer2d->initDrawer( oit.key() );
        list.Append( *it );
      }          
        
  if( updateViewer )
    myGLViewer2d->activateDrawers( list, TRUE );
  mySelNumbers.clear();    
}

void GLViewer_Context::setSelected( GLViewer_Object* object, bool updateViewer )
{
  int index = -1;
  if ( myObjects.contains( object ) )
    index = myObjects[object];

  if( index == -1 || mySelNumbers.findIndex( index ) != -1 )
    return;

  mySelNumbers.append( index );
  object->setSelected( TRUE );

  if( updateViewer )
    myGLViewer2d->activateDrawer( object, TRUE, TRUE );
}

void GLViewer_Context::remSelected( GLViewer_Object* object, bool updateViewer )
{
  int index = -1;
  if ( myObjects.contains( object ) )
    index = myObjects[object];

  if( index == -1 || mySelNumbers.findIndex( index ) == -1 )
    return;
  
  mySelNumbers.remove( index );
  object->unselect();

  if( updateViewer )
    myGLViewer2d->activateDrawer( object, TRUE, TRUE );
}

void GLViewer_Context::eraseObject( GLViewer_Object* theObject, bool theUpdateViewer )
{
    if( !theObject )
        return;

    theObject->unhighlight();
    theObject->unselect();
    theObject->setVisible( false );

    if( theUpdateViewer )
        myGLViewer2d->updateAll();
        //myGLViewer2d->activateAllDrawers( true );
}

void GLViewer_Context::deleteObject( GLViewer_Object* theObject, bool updateViewer )
{
    if( !theObject || !myObjects.contains(theObject) )
        return;

    int anIndex = myObjects[theObject];
    myObjects.remove( theObject );
    myObjList.remove( theObject );

    if( myLastPicked == theObject )
    {
        myLastPicked = NULL;
        myHNumber = -1;
    }

    QValueList<int>::Iterator anIt= mySelNumbers.find( anIndex );
    if( anIt != mySelNumbers.end() )
        mySelNumbers.remove( anIt );

    if( mySelCurIndex == anIndex )
        mySelCurIndex = 0;

    if ( updateViewer )
      myGLViewer2d->updateAll();
}
