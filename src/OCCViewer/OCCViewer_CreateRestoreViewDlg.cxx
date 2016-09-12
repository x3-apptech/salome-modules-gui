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

#include "OCCViewer_CreateRestoreViewDlg.h"
#include "OCCViewer_ViewPort3d.h"

#include <QPushButton>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QListWidgetItem>
#include <QEvent>
#include <QKeyEvent>

/*!
  Constructor
*/
OCCViewer_CreateRestoreViewDlg::OCCViewer_CreateRestoreViewDlg( QWidget* aWin, OCCViewer_ViewWindow* theViewWindow )
: QDialog( aWin )
{
  setWindowTitle( tr( "CAPTION" ) );

  myParametersMap = theViewWindow->getViewAspects();

  int aQuantityOfItems = myParametersMap.count();
        
  setFixedSize( 400, 300 );

  QGridLayout* aGrid = new QGridLayout( this );
  aGrid->setMargin( 5 );
  aGrid->setSpacing( 10 );

  QWidget* aWidget1 = new QWidget( this );
  QWidget* aWidget2 = new QWidget( this );
        
  QHBoxLayout* aLayout = new QHBoxLayout( aWidget1 );
        
  myListBox = new QListWidget( aWidget1 );
  myListBox->installEventFilter( this );

  myCurViewPort = new OCCViewer_ViewPort3d( aWidget1, theViewWindow->getViewPort()->getViewer(), V3d_ORTHOGRAPHIC );
  myCurViewPort->getView()->SetBackgroundColor( Quantity_NOC_BLACK );

  myListBox->setEditTriggers( QAbstractItemView::DoubleClicked );
        
  if ( aQuantityOfItems )
  {
    myListBox->clear();
    for( int i = 0; i < aQuantityOfItems; i++ ) {
      myListBox->insertItem( i, myParametersMap[ i ].name );
      myListBox->item( i )->setFlags( myListBox->item( i )->flags() | Qt::ItemIsEditable );
    }
    myListBox->item( 0 )->setSelected( true );
    changeImage( myListBox->item( 0 ) );
  }
  else
  {
    myListBox->clear();
    myListBox->insertItem( 0, "No Items" );
  }
  myListBox->setSelectionMode( QAbstractItemView::ExtendedSelection );
  connect( myListBox, SIGNAL( currentItemChanged(QListWidgetItem*, QListWidgetItem *)), this, SLOT( changeImage( QListWidgetItem* ) ) );
  connect( myListBox, SIGNAL( itemChanged( QListWidgetItem* ) ), this, SLOT( editItemText( QListWidgetItem* ) ) );
        
  aLayout->addWidget( myListBox );
  aLayout->addWidget( myCurViewPort, 30 );

  QHBoxLayout* aButtonLayout = new QHBoxLayout( aWidget2 );
  aButtonLayout->setMargin( 0 );
  aButtonLayout->setSpacing( 5 );

  QPushButton* theOk     = new QPushButton( tr( "Ok" ), aWidget2 );            theOk->setAutoDefault( false );
  QPushButton* theCancel = new QPushButton( tr( "Cancel" ), aWidget2 );          theCancel->setAutoDefault( false );
  QPushButton* theDelete = new QPushButton( tr( "Delete" ), aWidget2 );          theDelete->setAutoDefault( false );
  QPushButton* theClearAll = new QPushButton( tr( "Clear List" ), aWidget2 );  theClearAll->setAutoDefault( false );

  aButtonLayout->addWidget( theOk );
  aButtonLayout->addWidget( theCancel );
  aButtonLayout->addWidget( theDelete );
  aButtonLayout->addWidget( theClearAll );

  aGrid->addWidget( aWidget1, 0, 0 );
  aGrid->addWidget( aWidget2, 1, 0 );
        
  connect( theOk, SIGNAL( clicked() ), this, SLOT( OKpressed() ) );
  connect( theCancel, SIGNAL( clicked() ), this, SLOT( reject() ) );
  connect( theDelete, SIGNAL( clicked() ), this, SLOT( deleteSelectedItems() ) );
  connect( theClearAll, SIGNAL( clicked() ), this, SLOT( clearList() ) );
}

/*!
  Destructor
*/
OCCViewer_CreateRestoreViewDlg::~OCCViewer_CreateRestoreViewDlg()
{
}

/*!
  Changes image in accordance with item
  \param curItem - item contains info about view parameters
*/
void OCCViewer_CreateRestoreViewDlg::changeImage( QListWidgetItem* curItem )
{
  if( curItem && ( curItem->flags() & Qt::ItemIsEditable ) )
  {
    int lowLevel  = -1;
    int highLevel = -1;
    int index = curItem->listWidget()->row( curItem );  
    Handle(V3d_View) aView3d = myCurViewPort->getView();
    myCurrentItem = myParametersMap[ index ];
    
    Standard_Boolean prev = aView3d->SetImmediateUpdate( Standard_False );
    aView3d->SetScale( myCurrentItem.scale );
#if OCC_VERSION_LARGE <= 0x06070100
    aView3d->SetCenter( myCurrentItem.centerX, myCurrentItem.centerY );
#endif
    aView3d->SetProj( myCurrentItem.projX, myCurrentItem.projY, myCurrentItem.projZ );
    aView3d->SetTwist( myCurrentItem.twist );
    aView3d->SetAt( myCurrentItem.atX, myCurrentItem.atY, myCurrentItem.atZ );
    aView3d->SetImmediateUpdate( prev );
    aView3d->SetEye( myCurrentItem.eyeX, myCurrentItem.eyeY, myCurrentItem.eyeZ );
    aView3d->SetAxialScale( myCurrentItem.scaleX, myCurrentItem.scaleY, myCurrentItem.scaleZ );
  }
}

/*!
  \return current view parameters (corresponding to current item)
*/
viewAspect OCCViewer_CreateRestoreViewDlg::currentItem() const
{
  return myCurrentItem;
}

/*!
  Deletes selected items from list view
*/
void OCCViewer_CreateRestoreViewDlg::deleteSelectedItems()
{
  QList<QListWidgetItem*> selectedItems = myListBox->selectedItems();
  if( myListBox->count() && selectedItems.count())
  {
    int curIndex = -1;
    // Iterate by all selected items
    for(int i = 0; i < selectedItems.count(); i++) 
    {
      QListWidgetItem* item =  selectedItems.at(i);
      // get position of the selected item in the list
      int position = myListBox->row(item);

      //Calculate current index in case if "item" is last selected item.
      if(i == selectedItems.count() - 1)
      {
        if(position != myListBox->count() - 1)
          curIndex = position;
        else 
          curIndex = position - 1;
      }

      //Delete item
      delete item;

      //Shift parameters in the map
      for( int j = position; j < (int)myParametersMap.count(); j++ )
      {
        if( j != myParametersMap.count() - 1 )
          myParametersMap[ j ] = myParametersMap[ j + 1 ];
        else
          myParametersMap.removeAt( j );
      }
    }
    if( curIndex >= 0 )
    {
      myListBox->setItemSelected( myListBox->item( curIndex ), true );
    }
  }
  if( !myListBox->count() )
  {
    clearList();
  }
}

/*!
  Clears list of view aspects
*/
void OCCViewer_CreateRestoreViewDlg::clearList()
{
  myListBox->clear();
  myListBox->insertItem( 0, "No Items" );
  myParametersMap.clear();
	
  //Clear view
  myCurViewPort->reset();
}

/*!
  \return const reference to all view aspects
*/
const viewAspectList& OCCViewer_CreateRestoreViewDlg::parameters() const
{
  return myParametersMap;
}

/*!
  Renames key of view aspect map in accordance with item name
  \param anItem - item
*/
void OCCViewer_CreateRestoreViewDlg::editItemText( QListWidgetItem* anItem )
{
  int index = anItem->listWidget()->row( anItem );
  myParametersMap[ index ].name = anItem->text().toLatin1();
}

/*!
  SLOT: called on OK click, emits dlgOk() and closes dialog
*/
void OCCViewer_CreateRestoreViewDlg::OKpressed()
{
  emit dlgOk();
  accept();
}

