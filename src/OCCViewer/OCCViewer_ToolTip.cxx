// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      OCCViewer_ToolTip.cxx
// Author:    Alexandre SOLOVYOV
//
#include "OCCViewer_ToolTip.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewModel.h"

#include <SUIT_ViewManager.h>

/*!
  Constructor

  \param wnd - window where the tooltip should be assigned
*/
OCCViewer_ToolTip::OCCViewer_ToolTip( OCCViewer_ViewWindow* wnd )
: QtxToolTip( wnd->getViewPort() ),
  myWnd( wnd )
{
  connect( this, SIGNAL( maybeTip( QPoint, QString&, QFont&, QRect&, QRect& ) ),
           this, SLOT( onToolTip( QPoint, QString&, QFont&, QRect&, QRect& ) ) );
}

/*!
  Destructor
 */
OCCViewer_ToolTip::~OCCViewer_ToolTip()
{
}

/*!
  \return font of the tooltip
 */
QFont OCCViewer_ToolTip::font() const
{
  return myFont;
}

/*!
  Change font of the tooltip

  \param f - new font
 */
void OCCViewer_ToolTip::setFont( const QFont& f )
{
  myFont = f;
}

/*!
  Tooltip handler

  \param p - current point
  \param str - returned tooltip text
  \param f - returned tooltip font
  \param txtRect - returned tooltip text rectangle
  \param rect - returned tooltip rectangle
 */
void OCCViewer_ToolTip::onToolTip( QPoint p, QString& str, QFont& f, QRect& txtRect, QRect& rect )
{
  OCCViewer_Viewer* v = dynamic_cast<OCCViewer_Viewer*>( myWnd->getViewManager()->getViewModel() );
  Handle( AIS_InteractiveContext ) aCont = v->getAISContext();
  if( aCont.IsNull() )
    return;

  QString txt;
  Handle( SelectMgr_EntityOwner ) owner = aCont->DetectedOwner();
  if( !owner.IsNull() )
    emit toolTipFor( owner, txt );

  Handle(AIS_InteractiveObject) obj = aCont->DetectedInteractive();
  if( txt.isEmpty() && !obj.IsNull() )
    emit toolTipFor( obj, txt );

  if( txt.isEmpty() )
    return;

  str = txt;
  QFontMetrics m( myFont );
  int w = m.width( str ), h = m.height();

  txtRect = QRect( p.x()+4, p.y()-h, w, h );
  rect = txtRect;
}
