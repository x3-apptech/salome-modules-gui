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

#include "OCCViewer_AISSelector.h"

#include <Basics_OCCTVersion.hxx>

/*!
  Constructor
*/
OCCViewer_AISSelector::OCCViewer_AISSelector( QObject* parent, 
                                             const Handle (AIS_InteractiveContext)& aisContext) :
  QObject( parent ), 
  myNumSelected( 0 ), 
  myEnableSelection( true ),
  myEnableMultipleSelection( true )
{
  myHilightColor = Quantity_NOC_CYAN1;
  mySelectColor  = Quantity_NOC_GRAY80;

  setAISContext( aisContext );
}

/*!
  Destructor
*/
OCCViewer_AISSelector::~OCCViewer_AISSelector()
{
}

/*!
  Enables/disables selection
*/
void OCCViewer_AISSelector::enableSelection( bool bEnable )
{
  myEnableSelection = bEnable;
}

/*!
  Enables/disables multiple selection i.e
  selection of several objects at the same time.
*/
void OCCViewer_AISSelector::enableMultipleSelection( bool bEnable )
{
  myEnableMultipleSelection = bEnable;
  if ( bEnable ) myEnableSelection = bEnable;
}

/*!
  Sets the color to hilight the detected objects
*/
void OCCViewer_AISSelector::setHilightColor ( Quantity_NameOfColor color )
{
  myHilightColor = color;
  if ( !myAISContext.IsNull() ) {
#if OCC_VERSION_LARGE > 0x07000000    
    const Handle(Graphic3d_HighlightStyle)& hStyle = myAISContext->HighlightStyle();
    hStyle->SetColor( myHilightColor );
#else
  myAISContext->SetHilightColor( myHilightColor );
#endif  
  }
}

/*!
  Sets the color to display the selected objects
*/
void OCCViewer_AISSelector::setSelectColor ( Quantity_NameOfColor color )
{
  mySelectColor = color;
  if ( !myAISContext.IsNull() ) {
#if OCC_VERSION_LARGE > 0x07000000
    const Handle(Graphic3d_HighlightStyle)& sStyle = myAISContext->SelectionStyle();
    sStyle->SetColor( mySelectColor );
#else    
    myAISContext->SelectionColor( mySelectColor );
#endif
  }
}

/*!
  Sets the interactive context for this selector
*/
void OCCViewer_AISSelector::setAISContext ( const Handle (AIS_InteractiveContext)& aisContext )
{
  myAISContext = aisContext;
  if ( ! myAISContext.IsNull() ) { 
#if OCC_VERSION_LARGE > 0x07000000
    const Handle(Graphic3d_HighlightStyle)& hStyle = myAISContext->HighlightStyle();
    const Handle(Graphic3d_HighlightStyle)& sStyle = myAISContext->SelectionStyle();
    hStyle->SetColor( myHilightColor );
    sStyle->SetColor( mySelectColor );
#else
    myAISContext->SetHilightColor( myHilightColor );
    myAISContext->SelectionColor( mySelectColor );
#endif    
    myAISContext->SetSubIntensityColor( Quantity_NOC_CYAN1 );
  }
}

/*!
  Checks the status of pick and emits 'selSelectionDone' or
  'selSelectionCancel'.
  Returns 'true' if no error, 'false' otherwise.
*/
bool OCCViewer_AISSelector::checkSelection ( AIS_StatusOfPick status, 
                                             bool hadSelection, 
                                             bool addTo )
{
  if ( myAISContext.IsNull() )
    return false;

  myNumSelected = myAISContext->NbCurrents(); /* update after the last selection */
  
  if ( status == AIS_SOP_NothingSelected && !hadSelection ) {
    emit selSelectionCancel( addTo );
  }
  else if ( status == AIS_SOP_NothingSelected && hadSelection ) {
    emit selSelectionCancel( addTo ); /* unselected now */
  }
  else if ( status == AIS_SOP_OneSelected || status == AIS_SOP_SeveralSelected )
  {
    emit selSelectionDone( addTo ); /* selected ( the same object, may be ) */
  }
  return ( status != AIS_SOP_Error && status != AIS_SOP_NothingSelected );
}

/*!
  Detects the interactive objects at position (x,y).
  Returns 'true' if no error, 'false' otherwise.
*/
bool OCCViewer_AISSelector::moveTo ( int x, int y, const Handle (V3d_View)& view )
{
  if ( myAISContext.IsNull() )
    return false;

  if ( !myEnableSelection )
    return false;
  
  AIS_StatusOfDetection status = AIS_SOD_Error;
  status = myAISContext->MoveTo (x, y, view);
  
  return ( status != AIS_SOD_Error && status != AIS_SOD_AllBad );
}

/*!
  Selects the detected interactive objects.
  Calls checkSelection() for checking the status.
*/
bool OCCViewer_AISSelector::select ()
{
  if ( myAISContext.IsNull() )
    return false;

  if ( !myEnableSelection )
    return false;
  
  bool hadSelection = ( myNumSelected > 0 );
  
  /* select and send notifications */
  return checkSelection ( myAISContext->Select(), hadSelection, false );
}

/*!
  Selects the objects covered by the rectangle.
  Multiple selection must be enabled to get use of this function.
  Calls checkSelection() for checking the status.
*/
bool OCCViewer_AISSelector::select ( int left, int top, int right, int bottom,
                                     const Handle (V3d_View)& view )
{
  if ( myAISContext.IsNull() )
    return false;

  if ( !myEnableSelection || !myEnableMultipleSelection )
    return false;  /* selection with rectangle is considered as multiple selection */
  
  bool hadSelection = ( myNumSelected > 0 );
  
  /* select and send notifications */
  return checkSelection ( myAISContext->Select(left, top, right, bottom, view),
                          hadSelection, false );
}

/*!
  Adds new selected objects to the objects previously selected.
  Multiple selection must be enabled to get use of this function.
  Calls checkSelection() for checking the status.
*/
bool OCCViewer_AISSelector::shiftSelect ()
{
  if ( myAISContext.IsNull() )
    return false;

  if ( !myEnableSelection )
    return false;
  
  bool hadSelection = ( myNumSelected > 0 ); /* something was selected */
  if ( hadSelection && !myEnableMultipleSelection)
    return false;
  
  /* select and send notifications */
  return checkSelection ( myAISContext->ShiftSelect(), hadSelection, true );
}

/*!
  Adds new selected objects covered by the rectangle to the objects
  previously selected.
  Multiple selection must be enabled to get use of this function.
  Calls checkSelection() for checking the status.
*/
bool OCCViewer_AISSelector::shiftSelect ( int left, int top, int right, int bottom,
                                         const Handle (V3d_View)& view )
                                         
{
  if ( myAISContext.IsNull() )
    return false;

  if ( !myEnableSelection || !myEnableMultipleSelection )
    return false;  /* selection with rectangle is considered as multiple selection */
  
  bool hadSelection = ( myNumSelected > 0 );      /* something was selected */
  if ( hadSelection && !myEnableMultipleSelection)
    return false;
  
  /* select and send notifications */
  return checkSelection ( myAISContext->ShiftSelect(left,top,right,bottom, view),
    hadSelection, true );
}
