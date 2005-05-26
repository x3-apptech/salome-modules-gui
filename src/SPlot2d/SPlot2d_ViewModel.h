//  File   : SPlot2d_ViewModel.h
//  Author : Sergey RUIN
//  Module : SUIT

#ifndef SPlot2d_ViewModel_H
#define SPlot2d_ViewModel_H

#include "SPlot2d.h"  

#include "SALOME_Prs.h"
#include "Plot2d_ViewModel.h"
#include "SALOME_InteractiveObject.hxx"
#include "Plot2d_ViewFrame.h"
#include "Plot2d_ViewWindow.h"
#include "SPlot2d_Curve.h"

class SPLOT2D_EXPORT SPlot2d_Viewer : public Plot2d_Viewer, public SALOME_View
{ 
  Q_OBJECT

public:
  /* Construction/destruction */
  SPlot2d_Viewer( bool theAutoDel = false );
  virtual ~SPlot2d_Viewer();

public:

  /*  interactive object management */
  void highlight( const Handle(SALOME_InteractiveObject)& IObject, bool highlight, bool update = true ) {}
  void unHighlightAll() {}
  void rename( const Handle(SALOME_InteractiveObject)& IObject, QString newName );
  bool isInViewer( const Handle(SALOME_InteractiveObject)& IObject );
  
  /* display */		
  void Display( const Handle(SALOME_InteractiveObject)& IObject, bool update = true );
  void DisplayOnly( const Handle(SALOME_InteractiveObject)& IObject );
  void Erase( const Handle(SALOME_InteractiveObject)& IObject, bool update = true );

  /* Reimplemented from SALOME_View */
  void                 Display( const SALOME_Prs2d* );
  void                 Erase( const SALOME_Prs2d*, const bool = false );
  virtual void         EraseAll(const bool = false);
  virtual void         Repaint();
  virtual SALOME_Prs*  CreatePrs( const char* entry = 0 );
  virtual void         BeforeDisplay( SALOME_Displayer* d );
  virtual void         AfterDisplay ( SALOME_Displayer* d );
  virtual bool         isVisible( const Handle(SALOME_InteractiveObject)& IObject );


  /* operations */
  SPlot2d_Curve*                   getCurveByIO( const Handle(SALOME_InteractiveObject)& theIObject );
  Plot2d_ViewFrame*                getActiveViewFrame();
  Handle(SALOME_InteractiveObject) FindIObject( const char* Entry );

};


#endif // SPlot2d_ViewModel_H




