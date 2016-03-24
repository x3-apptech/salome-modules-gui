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
//  File   : VTKViewer_MarkerDlg.cxx
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "VTKViewer_MarkerDlg.h"
#include "VTKViewer_MarkerWidget.h"

#include <SUIT_Application.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include <QFrame>
#include <QVBoxLayout>
#include <QKeyEvent>

/*!
  \class VTKViewer_MarkerDlg
  \brief Dialog for specifying of point marker parameters
*/

/*!
  \brief Constructor
  \param parent parent widget
*/
VTKViewer_MarkerDlg::VTKViewer_MarkerDlg( QWidget* parent )
: QtxDialog( parent, true, true )
{
  // set title
  setWindowTitle( tr( "SET_MARKER_TLT" ) );

  // create widgets
  QFrame* frame = new QFrame( mainFrame() );
  frame->setFrameStyle( QFrame::Sunken | QFrame::Box );
  myMarkerWidget = new VTKViewer_MarkerWidget( frame );

  // layoting
  QBoxLayout* vl = new QVBoxLayout( frame );
  vl->setSpacing( 6 ) ;
  vl->setMargin( 11 );
  vl->addWidget( myMarkerWidget );

  QBoxLayout* topLayout = new QVBoxLayout( mainFrame() );
  topLayout->setSpacing( 0 ) ;
  topLayout->setMargin( 0 );
  topLayout->addWidget( frame );

  // connect signals / slots
  connect( this, SIGNAL( dlgHelp() ), this, SLOT( onHelp() ) );
}

/*!
  \brief Destructor
*/
VTKViewer_MarkerDlg::~VTKViewer_MarkerDlg()
{
}

/*!
  \brief Associate documentation page with the dialog box
  \param module module name
  \param helpFile reference help file
*/
void VTKViewer_MarkerDlg::setHelpData( const QString& module,
                                       const QString& helpFile )
{
  myModule   = module;
  myHelpFile = helpFile;
}

/*!
  \brief Process key press event
  \param e key press event
*/
void VTKViewer_MarkerDlg::keyPressEvent( QKeyEvent* e )
{
  QtxDialog::keyPressEvent( e );
  if ( e->isAccepted() )
    return;

  // invoke Help on <F1> key presss
  if ( e->key() == Qt::Key_F1 ) {
    e->accept();
    onHelp();
  }
}

/*!
  \brief Activate help for the dialog box
*/
void VTKViewer_MarkerDlg::onHelp()
{
  if ( !myModule.isEmpty() && !myHelpFile.isEmpty() ) {
    SUIT_Session::session()->activeApplication()->onHelpContextModule( myModule, myHelpFile );
  }
}

/*!
  \brief Set custom markers data
  \param markerMap custom marker data (a map {index:texture})
*/
void VTKViewer_MarkerDlg::setCustomMarkers( const VTK::MarkerMap& markerMap )
{
  myMarkerWidget->setCustomMarkers( markerMap );
}

/*!
  \brief Get custom markers data
  \return custom marker data
*/
VTK::MarkerMap VTKViewer_MarkerDlg::customMarkers() const
{
  return myMarkerWidget->customMarkers();
}

/*!
  \brief Add standard marker
  The marker type specified with \a type must be > VTK::MT_USER
  \param type marker type
  \param icon marker icon
*/
void VTKViewer_MarkerDlg::addMarker( VTK::MarkerType type, const QPixmap& icon )
{
  myMarkerWidget->addMarker( type, icon );
}

/*!
  \brief Select specified standard marker as current one
  \param type marker type
  \param scale marker scale (optional parameter; can be omitted for extended markers)
*/
void VTKViewer_MarkerDlg::setMarker( VTK::MarkerType type, VTK::MarkerScale scale )
{
  myMarkerWidget->setMarker( type, scale );
}

/*!
  \brief Select specified custom marker as current one
  \param id custom marker identifier
*/
void VTKViewer_MarkerDlg::setCustomMarker( int id )
{
  myMarkerWidget->setCustomMarker( id );
}

/*!
  \brief Get current marker's type.
  For custom marker, VTK::MT_USER is returned and markerId() function 
  then returns its identifier.
  \return currently selected marker type
*/
VTK::MarkerType VTKViewer_MarkerDlg::markerType() const
{
  return myMarkerWidget->markerType();
}

/*!
  \brief Get current marker's scale size.
  For custom marker return value is undefined.
  \return currently selected marker scale size
*/
VTK::MarkerScale VTKViewer_MarkerDlg::markerScale() const
{
  return myMarkerWidget->markerScale();
}

/*!
  \bried Get currently selected custom marker's identifier.
  For standard markers return value is VTK::MT_NONE.
*/
int VTKViewer_MarkerDlg::markerId() const
{
  return myMarkerWidget->markerId();
}
