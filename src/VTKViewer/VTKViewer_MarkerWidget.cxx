// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include <QtxComboBox.h>

#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include <vtkImageData.h>

#include <QButtonGroup>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QStackedWidget>

#define MARGIN  9
#define SPACING 6

/*!
 * Class       : VTKViewer_MarkerWidget
 * Description : Widget for specifying point marker parameters
 */

/*!
  Constructor
*/
VTKViewer_MarkerWidget::VTKViewer_MarkerWidget( QWidget* theParent )
: QWidget( theParent )
{
  QRadioButton* aStandardTypeRB = new QRadioButton( tr( "STANDARD_MARKER" ), this );
  QRadioButton* aCustomTypeRB   = new QRadioButton( tr( "CUSTOM_MARKER" ), this );
  myTypeGroup = new QButtonGroup( this );
  myTypeGroup->addButton( aStandardTypeRB, 0 );
  myTypeGroup->addButton( aCustomTypeRB,   1 );

  QHBoxLayout* aRadioLayout = new QHBoxLayout;
  aRadioLayout->setMargin( 0 );
  aRadioLayout->setSpacing( SPACING );
  aRadioLayout->addWidget( aStandardTypeRB );
  aRadioLayout->addWidget( aCustomTypeRB );

  // ---

  myWGStack = new QStackedWidget( this );
  myWGStack->setFrameStyle( QFrame::Box | QFrame::Sunken );

  // ---

  QWidget* aStdWidget = new QWidget( myWGStack );

  QLabel* aTypeLab  = new QLabel( tr( "TYPE" ),  aStdWidget );
  QLabel* aScaleLab = new QLabel( tr( "SCALE" ), aStdWidget );

  myStdTypeCombo  = new QtxComboBox( aStdWidget );
  myStdScaleCombo = new QtxComboBox( aStdWidget );

  QGridLayout* aStdLayout = new QGridLayout;
  aStdLayout->setMargin( MARGIN );
  aStdLayout->setSpacing( SPACING );
  aStdLayout->addWidget( aTypeLab,        0, 0 );
  aStdLayout->addWidget( myStdTypeCombo,  0, 1 );
  aStdLayout->addWidget( aScaleLab,       1, 0 );
  aStdLayout->addWidget( myStdScaleCombo, 1, 1 );
  aStdWidget->setLayout( aStdLayout );

  // ---

  QWidget* aCustomWidget = new QWidget( myWGStack );

  QLabel* aCustomLab = new QLabel( tr( "CUSTOM" ), aCustomWidget );
  myCustomTypeCombo = new QtxComboBox( aCustomWidget );
  QPushButton* aBrowseBtn = new QPushButton( tr( "BROWSE" ), aCustomWidget );

  QGridLayout* aCustomLayout = new QGridLayout;
  aCustomLayout->setMargin( MARGIN );
  aCustomLayout->setSpacing( SPACING );
  aCustomLayout->addWidget( aCustomLab,        0, 0 );
  aCustomLayout->addWidget( myCustomTypeCombo, 0, 1 );
  aCustomLayout->addWidget( aBrowseBtn,        0, 2 );
  aCustomLayout->setRowStretch( 1, 5 );
  aCustomWidget->setLayout( aCustomLayout );

  // ---
  
  myWGStack->insertWidget( 0, aStdWidget );
  myWGStack->insertWidget( 1, aCustomWidget );

  // ---

  QVBoxLayout* aTopLayout = new QVBoxLayout;
  aTopLayout->setMargin( MARGIN );
  aTopLayout->setSpacing( SPACING );
  aTopLayout->addLayout( aRadioLayout );
  aTopLayout->addWidget( myWGStack );
  setLayout( aTopLayout );

  // ---

  connect( myTypeGroup, SIGNAL( buttonClicked( int ) ), myWGStack, SLOT( setCurrentIndex( int ) ) );
  connect( myStdTypeCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( onStdMarkerChanged( int ) ) );
  connect( aBrowseBtn,  SIGNAL( clicked() ), this, SLOT( onBrowse() ) );

  // ---

  aStandardTypeRB->setChecked( true );
  init();
}

/*!
  Destructor
*/
VTKViewer_MarkerWidget::~VTKViewer_MarkerWidget()
{
}

void VTKViewer_MarkerWidget::setCustomMarkerMap( VTK::MarkerMap theMarkerMap )
{
  myCustomMarkerMap = theMarkerMap;

  VTK::MarkerMap::const_iterator it = theMarkerMap.begin(), itEnd = theMarkerMap.end();
  for( ; it != itEnd; it++ )
  {
    int anId = it->first;
    VTK::MarkerData aMarkerData = it->second;
    QPixmap aPixmap = markerFromData( aMarkerData );
    if( !aPixmap.isNull() )
    {
      myCustomTypeCombo->addItem( aPixmap, QString::number( anId ) );
      myCustomTypeCombo->setId( myCustomTypeCombo->count()-1, anId );
    }
  }
}

VTK::MarkerMap VTKViewer_MarkerWidget::getCustomMarkerMap()
{
  return myCustomMarkerMap;
}

void VTKViewer_MarkerWidget::setStandardMarker( VTK::MarkerType theMarkerType, VTK::MarkerScale theMarkerScale )
{
  if ( ( theMarkerType > VTK::MT_NONE && theMarkerType < VTK::MT_USER ) ||
       myExtraMarkerList.contains( theMarkerType ) ) {
    myTypeGroup->button( 0 )->setChecked( true );
    myWGStack->setCurrentIndex( 0 );
    myStdTypeCombo->setCurrentId( theMarkerType );
    int aMarkerScale = std::max( (int)VTK::MS_10, std::min( (int)VTK::MS_70, (int)theMarkerScale ) );
    myStdScaleCombo->setCurrentId( aMarkerScale );
  }
}

void VTKViewer_MarkerWidget::setCustomMarker( int theId )
{
  if ( theId > 0 ) {
    myTypeGroup->button( 1 )->setChecked( true );
    myWGStack->setCurrentIndex( 1 );
    addTexture( theId );
    myCustomTypeCombo->setCurrentId( theId );
  }
}

VTK::MarkerType VTKViewer_MarkerWidget::getMarkerType() const
{
  return myWGStack->currentIndex() == 0 ? (VTK::MarkerType)myStdTypeCombo->currentId() : VTK::MT_USER;
}

VTK::MarkerScale VTKViewer_MarkerWidget::getStandardMarkerScale() const
{
  return myWGStack->currentIndex() == 0 ? (VTK::MarkerScale)myStdScaleCombo->currentId() : VTK::MS_NONE;
}

int VTKViewer_MarkerWidget::getCustomMarkerID() const
{
  return myWGStack->currentIndex() == 1 ? myCustomTypeCombo->currentId() : 0;
}

void VTKViewer_MarkerWidget::addExtraStdMarker( VTK::MarkerType theMarkerType, const QPixmap& thePixmap )
{
  if( myExtraMarkerList.isEmpty() )
    myStdTypeCombo->insertSeparator( myStdTypeCombo->count() );
  myStdTypeCombo->addItem( thePixmap, QString() );
  myStdTypeCombo->setId( myStdTypeCombo->count()-1, theMarkerType );

  myExtraMarkerList.append( theMarkerType );
}

void VTKViewer_MarkerWidget::init()
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  for ( int i = VTK::MT_POINT; i < VTK::MT_USER; i++ ) {
    QString icoFile = QString( "ICON_VERTEX_MARKER_%1" ).arg( i );
    QPixmap pixmap = resMgr->loadPixmap( "VTKViewer", tr( qPrintable( icoFile ) ) );
    myStdTypeCombo->addItem( pixmap, QString() );
    myStdTypeCombo->setId( myStdTypeCombo->count()-1, i );
  }

  for ( int i = VTK::MS_10; i <= VTK::MS_70; i++ ) {
    myStdScaleCombo->addItem( QString::number( (i-1)*0.5 + 1.0 ) );
    myStdScaleCombo->setId( myStdScaleCombo->count()-1, i );
  }
}

void VTKViewer_MarkerWidget::addTexture( int id, bool select )
{
  if ( id > 0 && myCustomTypeCombo->index( id ) == -1 &&
       myCustomMarkerMap.find( id ) != myCustomMarkerMap.end() ) {
    VTK::MarkerData aMarkerData = myCustomMarkerMap[ id ];
    QPixmap pixmap = markerFromData( aMarkerData );
    if( !pixmap.isNull() ) {
      myCustomTypeCombo->addItem( pixmap, QString::number( id ) );
      myCustomTypeCombo->setId( myCustomTypeCombo->count()-1, id );
      if ( select ) myCustomTypeCombo->setCurrentId( id );
    }
  }
}

QPixmap VTKViewer_MarkerWidget::markerFromData( const VTK::MarkerData& theMarkerData )
{
  const VTK::MarkerTexture& aMarkerTexture = theMarkerData.second;
  vtkSmartPointer<vtkImageData> anImageData = VTK::MakeVTKImage( aMarkerTexture, false );

  QImage anImage = VTK::ConvertToQImage( anImageData.GetPointer() );
  if( anImage.isNull() )
    return QPixmap();

  return QPixmap::fromImage( anImage );
}

void VTKViewer_MarkerWidget::onStdMarkerChanged( int index )
{
  VTK::MarkerType aMarkerType = (VTK::MarkerType)myStdTypeCombo->id( index );
  bool anIsExtraMarker = myExtraMarkerList.contains( aMarkerType );
  myStdScaleCombo->setEnabled( !anIsExtraMarker );
}

void VTKViewer_MarkerWidget::onBrowse()
{
  QStringList filters;
  filters << tr( "Texture files (*.dat)" ) << tr( "All files (*)" );
  QString aFileName = SUIT_Session::session()->activeApplication()->getFileName( true, QString(), filters.join( ";;" ), tr( "LOAD_TEXTURE_TLT" ), this );
  if ( !aFileName.isEmpty() ) {
    VTK::MarkerTexture aMarkerTexture;
    if ( VTK::LoadTextureData( aFileName, VTK::MS_NONE, aMarkerTexture ) ) {
      int anId = VTK::GetUniqueId( myCustomMarkerMap );
      VTK::MarkerData& aMarkerData = myCustomMarkerMap[ anId ];
      aMarkerData.first = aFileName.toStdString();
      aMarkerData.second = aMarkerTexture;
      addTexture( anId, true );
    }
  }
}
