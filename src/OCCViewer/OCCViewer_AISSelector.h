#if !defined( OCCVIEWER_AISSELECTOR_H )
#define OCCVIEWER_AISSELECTOR_H

#include "OCCViewer.h"
#include <qobject.h>

#include <Quantity_NameOfColor.hxx>
#include <AIS_InteractiveContext.hxx>

class OCCVIEWER_EXPORT OCCViewer_AISSelector : public QObject
{
  Q_OBJECT
    
public:
  // constructor
  OCCViewer_AISSelector( QObject* parent, const Handle (AIS_InteractiveContext)& );
  // destructor
  ~OCCViewer_AISSelector();
  
  // enables/disables selection
  void enableSelection( bool );
  // enables/disables multiple selection
  void enableMultipleSelection( bool );
  
  // detects the interactive objects at position (x,y).
  bool moveTo ( int, int, const Handle (V3d_View)& );
  // selects the objects covered by the rectangle.
  bool select ( int, int, int, int, const Handle (V3d_View)& );
  // adds new selected objects covered by the rectangle to the objects
  // previously selected.
  bool shiftSelect ( int, int, int, int, const Handle (V3d_View)& );
  // selects the detected interactive objects.
  bool select ();
  // adds new selected objects to the objects previously selected.
  bool shiftSelect ();
  
  // sets the interactive context for this selector
  void setAISContext ( const Handle (AIS_InteractiveContext)& );
  // sets the color to hilight the detected objects
  void setHilightColor ( Quantity_NameOfColor color );
  // sets the color to display the selected objects
  void setSelectColor ( Quantity_NameOfColor color );
  
protected:
  // checks the status of pick and emits 'selSelectionDone' or 'selSelectionCancel'.
  bool checkSelection ( AIS_StatusOfPick status, bool hadSelection, bool addTo );
  
signals:
  // 'selection done' signal
  void selSelectionDone( bool bAdded );
  // 'selection cancelled' signal
  void selSelectionCancel( bool bAdded );
  
protected:
  Handle (AIS_InteractiveContext) myAISContext;    // graphic context
  Quantity_NameOfColor            myHilightColor;  // color for hilight object
  Quantity_NameOfColor            mySelectColor;   // color for selected object
  
  int  myNumSelected;                              // nymber of selected objects
  bool myEnableSelection;                          // enable selection flag
  bool myEnableMultipleSelection;                  // enable multiple selection flag
};

#endif // OCCVIEWER_AISSELECTOR_H
