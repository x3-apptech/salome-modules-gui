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

#include "VTKViewer_MarkerWidget.h"
#include "VTKViewer_MarkerUtils.h"

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include <vtkImageData.h>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QSpinBox>

const int SPACING = 6;
enum { TypeRole = Qt::UserRole, IdRole };

/*!
  \class VTKViewer_MarkerWidget
  \brief Widget for specifying point marker parameters
*/

/*!
  \brief Constructor
  \param parent parent widget
*/
VTKViewer_MarkerWidget::VTKViewer_MarkerWidget( QWidget* parent )
  : QWidget( parent ), myCurrentIdx( -1 )
{
  // create widgets
  myTypeLab  = new QLabel( tr( "TYPE" ),  this );
  myScaleLab = new QLabel( tr( "SCALE" ), this );
  myType     = new QComboBox( this );
  myScale    = new QSpinBox( this );
  // layouting
  QHBoxLayout* ml = new QHBoxLayout( this );
  ml->setMargin( 0 );
  ml->setSpacing( SPACING );
  ml->addWidget( myTypeLab );
  ml->addWidget( myType );
  ml->addWidget( myScaleLab );
  ml->addWidget( myScale );
  myType->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  myScale->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  // connect signals/slots
  connect( myType, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onTypeChanged( int ) ) );
  // initialize
  init();
}

/*!
  \brief Destructor
*/
VTKViewer_MarkerWidget::~VTKViewer_MarkerWidget()
{
}

/*!
  \brief Set custom markers data
  \param markerMap custom marker data (a map {index:texture})
*/
void VTKViewer_MarkerWidget::setCustomMarkers( const VTK::MarkerMap& markerMap )
{
  // store custom markers data
  myCustomMarkers = markerMap;
  // clear current custom markers
  for ( int i = myType->count()-1; i >= 0; i-- ) {
    int type = myType->itemData( i, TypeRole ).toInt();
    if ( type == VTK::MT_USER )
      myType->removeItem( i );
  }
  // add custom markers
  VTK::MarkerMap::const_iterator it;
  for ( it = myCustomMarkers.begin(); it != myCustomMarkers.end(); ++it )
  {
    int id = it->first;
    VTK::MarkerData markerData = it->second;
    QPixmap icon = markerFromData( markerData );
    if( !icon.isNull() )
    {
      int idx = myType->count()-1;
      myType->insertItem( idx, icon, QString() );
      myType->setItemData( idx, VTK::MT_USER, TypeRole );
      myType->setItemData( idx, id, IdRole );
    }
  }
}

/*!
  \brief Get custom markers data
  \return custom marker data
*/
VTK::MarkerMap VTKViewer_MarkerWidget::customMarkers() const
{
  return myCustomMarkers;
}

/*!
  \brief Add standard marker
  The marker type specified with \a type must be > VTK::MT_USER
  \param type marker type
  \param icon marker icon
*/
void VTKViewer_MarkerWidget::addMarker( VTK::MarkerType type, const QPixmap& icon )
{
  if ( type > VTK::MT_USER ) {
    int idx = (int)VTK::MT_USER - 1;
    // find insertion index
    while ( idx < myType->count()-1 ) {
      if ( myType->itemData( idx, TypeRole ) == VTK::MT_USER )
	break;
      ++idx;
    }
    myType->insertItem( idx, icon, QString() );
    myType->setItemData( idx, type, TypeRole );
  }
}

/*!
  \brief Select specified standard marker as current one
  \param type marker type
  \param scale marker scale (optional parameter; can be omitted for extended markers)
*/
void VTKViewer_MarkerWidget::setMarker( VTK::MarkerType type, VTK::MarkerScale scale )
{
  if ( type != VTK::MT_USER ) {
    for ( int i = 0; i < myType->count()-1; i++ ) {
      if ( type == myType->itemData( i, TypeRole ).toInt() ) {
	myType->setCurrentIndex( i );
	break;
      }
    }
  }
  if ( scale != VTK::MS_NONE )
    myScale->setValue( qMax( (int)VTK::MS_10, qMin( (int)VTK::MS_70, (int)scale ) ) );
}

/*!
  \brief Select specified custom marker as current one
  \param id custom marker identifier
*/
void VTKViewer_MarkerWidget::setCustomMarker( int id )
{
  for ( int i = 0; i < myType->count()-1; i++ ) {
    int type = myType->itemData( i, TypeRole ).toInt();
    if ( type == VTK::MT_USER && id == myType->itemData( i, IdRole ).toInt() ) {
      myType->setCurrentIndex( i );
      break;
    }
  }
}

/*!
  \brief Get current marker's type.
  For custom marker, VTK::MT_USER is returned and markerId() function 
  then returns its identifier.
  \return currently selected marker type
*/
VTK::MarkerType VTKViewer_MarkerWidget::markerType() const
{
  return myType->itemData( myType->currentIndex(), TypeRole ).toInt();
}

/*!
  \brief Get current marker's scale size.
  For custom marker return value is undefined.
  \return currently selected marker scale size
*/
VTK::MarkerScale VTKViewer_MarkerWidget::markerScale() const
{
  return myScale->value();
}

/*!
  \bried Get currently selected custom marker's identifier.
  For standard markers return value is VTK::MT_NONE.
*/
int VTKViewer_MarkerWidget::markerId() const
{
  int type = myType->itemData( myType->currentIndex(), TypeRole ).toInt();
  return type == VTK::MT_USER ? myType->itemData( myType->currentIndex(), IdRole ).toInt() : VTK::MT_NONE;
}

/*!
  \brief Get access to the internal marker type label
  \return marker type label widget
*/
QLabel* VTKViewer_MarkerWidget::typeLabel()
{
  return myTypeLab;
}

/*!
  \brief Get access to the internal marker scale label
  \return marker scale label widget
*/
QLabel* VTKViewer_MarkerWidget::scaleLabel()
{
  return myScaleLab;
}

/*!
  \brief Internal initialization
*/
void VTKViewer_MarkerWidget::init()
{
  myType->blockSignals( true );

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();
  // standard marker types
  for ( int type = VTK::MT_POINT; type < VTK::MT_USER; type++ ) {
    QString icoFile = QString( "ICON_VERTEX_MARKER_%1" ).arg( type );
    QPixmap pixmap = resMgr->loadPixmap( "VTKViewer", tr( qPrintable( icoFile ) ) );
    myType->addItem( pixmap, QString() );
    myType->setItemData( myType->count()-1, type, TypeRole );
  }
  // standard marker sizes
  myScale->setMinimum( (int)VTK::MS_10 );
  myScale->setMaximum( (int)VTK::MS_70 );
  // add item for loading custom textures
  myType->addItem( "..." );
  myType->setItemData( myType->count()-1, VTK::MT_NONE, TypeRole );

  myType->blockSignals( false );

  // set current item to first type in the list
  myType->setCurrentIndex( 0 );
}

/*!
  \brief Create icon from the custom marker data (texture)
  \param markerData custom marker data
  \return icon generated from texture specified with marker data
*/
QPixmap VTKViewer_MarkerWidget::markerFromData( const VTK::MarkerData& markerData )
{
  // get texture data
  const VTK::MarkerTexture& texture = markerData.second;
  // generate VTK image
  vtkSmartPointer<vtkImageData> image = VTK::MakeVTKImage( texture, false );
  // convert VTK image to icon
  QImage qimage = VTK::ConvertToQImage( image.GetPointer() );
  return qimage.isNull() ? QPixmap() : QPixmap::fromImage( qimage );
}

/*!
  \brief Called when marker type is changed (by the user or programmatically)
  \param index index of item being selected
*/
void VTKViewer_MarkerWidget::onTypeChanged( int index )
{
  if ( index == myType->count()-1 ) {
    // browse new custom texture file item is selected
    QStringList filters;
    filters << tr( "Texture files (*.dat)" ) << tr( "All files (*)" );
    QString fileName = SUIT_Session::session()->activeApplication()->getFileName( true, 
										  QString(), 
										  filters.join( ";;" ), 
										  tr( "LOAD_TEXTURE_TLT" ), 
										  parentWidget() );
    if ( !fileName.isEmpty() ) {
      // load texture and add new marker
      VTK::MarkerTexture texture;
      if ( VTK::LoadTextureData( fileName, VTK::MS_NONE, texture ) ) {
	int id = VTK::GetUniqueId( myCustomMarkers );
	VTK::MarkerData& markerData = myCustomMarkers[ id ];
	markerData.first  = fileName.toStdString();
	markerData.second = texture;
	QPixmap icon = markerFromData( markerData );
	if( !icon.isNull() ) {
	  int idx = myType->count()-1;
	  myType->blockSignals( true );
	  myType->insertItem( idx, icon, QString() );
	  myType->blockSignals( false );
	  myType->setItemData( idx, VTK::MT_USER, TypeRole );
	  myType->setItemData( idx, id, IdRole );
	  myType->setCurrentIndex( idx );
	  return;
	}
      }
    }
    // if user cancelled texture loading or there was an error when loading texture
    // reset to the previous item
    myType->setCurrentIndex( myCurrentIdx );
    return;
  }
  else {
    myCurrentIdx = index;
  }
  int type = myType->itemData( index, TypeRole ).toInt();
  myScale->setEnabled( type < VTK::MT_USER );
  myScaleLab->setEnabled( type < VTK::MT_USER );
}
