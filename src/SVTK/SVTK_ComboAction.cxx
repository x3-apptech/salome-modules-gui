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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : 
//  Author : 

#include "SVTK_ComboAction.h"

#include <QComboBox>
#include <QHBoxLayout>

SVTK_ComboAction::SVTK_ComboAction( QObject* parent )
  : QWidgetAction( parent )
{
  myCurId = -1;
}

SVTK_ComboAction::SVTK_ComboAction( const QString& text, QObject* parent )
  : QWidgetAction( parent )
{
  setToolTip( text );
  myCurId = -1;
}

SVTK_ComboAction::~SVTK_ComboAction()
{
  myIcons.clear();
}

void SVTK_ComboAction::insertItem( const QIcon& icon, const int index )
{
  if ( index < 0 || index > myIcons.size() )
    myIcons.append( icon );
  else
    myIcons.insert( index, icon );

  update();
}

void SVTK_ComboAction::clear()
{
  myIcons.clear();
  update();
}

void SVTK_ComboAction::setCurrentIndex( const int id )
{
  if ( myCurId != id ) 
  {
    myCurId = id;
    update();
  }
}

int SVTK_ComboAction::currentIndex() const
{
  return myCurId;
}

QWidget* SVTK_ComboAction::createWidget( QWidget* parent )
{
  QWidget* w = 0;
  if ( parent->inherits("QToolBar") )
  {
    w = new QWidget( parent );
    QHBoxLayout* l = new QHBoxLayout( w );
    l->setSpacing(0); l->setMargin(0);
    QComboBox* combo = new QComboBox( w );
    combo->setFocusPolicy( Qt::NoFocus );
    combo->setSizeAdjustPolicy( QComboBox::AdjustToContents );
    l->addSpacing( 3 );
    l->addWidget( combo );
    l->addSpacing( 3 );

    updateCombo( combo );
    connect( combo, SIGNAL( activated( int ) ), this, SIGNAL( triggered( int ) ) );
  }
  return w;
}

void SVTK_ComboAction::update()
{
  QList<QWidget*> aList = createdWidgets();
  for ( QList<QWidget*>::const_iterator it = aList.begin(); it != aList.end(); ++it )
    updateCombo( (*it)->findChild<QComboBox*>() );
}

void SVTK_ComboAction::updateCombo( QComboBox* combo )
{
  if ( !combo ) return;

  combo->clear();

  for ( QList<QIcon>::const_iterator it = myIcons.begin(); it != myIcons.end(); ++it )
    combo->addItem( *it, "" );

  if ( combo->count() > 0 ) {
    if ( myCurId < 0 ) myCurId = 0;
    combo->setCurrentIndex( myCurId );
  }
}
