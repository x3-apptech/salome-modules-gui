// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
//  File   : ToolsGUI_IntervalWindow.cxx
//  Author : Oksana TCHEBANOVA
//  Module : SALOME

#include "ToolsGUI_IntervalWindow.h"

#include <qbuttongroup.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
# include "utilities.h"

#define MARGIN_SIZE      11
#define SPACING_SIZE      6
#define MIN_SPIN_WIDTH  100 

/*!
  Constructor
*/
ToolsGUI_IntervalWindow::ToolsGUI_IntervalWindow ( QWidget* parent )
: QDialog( parent, "ToolsGUI_IntervalWindow" , true, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu | WDestructiveClose )
{
  BEGIN_OF("ToolsGUI_IntervalWindow constructor")
  setCaption( tr( "Refresh Interval"  ) );
  setSizeGripEnabled( true );

  QGridLayout* topLayout = new QGridLayout( this );
  topLayout->setSpacing( SPACING_SIZE );
  topLayout->setMargin( MARGIN_SIZE );

  QGroupBox* intervalGrp = new QGroupBox( this, "intervalGrp" );
  intervalGrp->setColumnLayout( 0, Qt::Horizontal );//Vertical
  intervalGrp->layout()->setSpacing( 0 );
  intervalGrp->layout()->setMargin( 0 );
  QGridLayout* intervalGrpLayout = new QGridLayout( intervalGrp->layout() );
  intervalGrpLayout->setAlignment( Qt::AlignTop );
  intervalGrpLayout->setSpacing( SPACING_SIZE );
  intervalGrpLayout->setMargin( MARGIN_SIZE  );  

  QHBoxLayout* aBtnLayout = new QHBoxLayout;
  aBtnLayout->setSpacing( SPACING_SIZE );
  aBtnLayout->setMargin( 0 );

  myButtonOk = new QPushButton( this, "buttonOk" );
  myButtonOk->setText( tr( "BUT_OK"  ) );
  myButtonOk->setAutoDefault( TRUE );
  myButtonOk->setDefault( TRUE );
  
  myButtonCancel = new QPushButton( this, "buttonCancel" );
  myButtonCancel->setText( tr( "BUT_CANCEL"  ) );
  myButtonCancel->setAutoDefault( TRUE );

  QLabel* TextLabel = new QLabel( intervalGrp, "TextLabel" );
  TextLabel->setText( tr( "Please, enter a number of seconds:"  ) );

  mySpinBox = new QSpinBox( 1, 999999999, 1, intervalGrp, "SpinBox" );
  mySpinBox->setValue( 100 );
  mySpinBox->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
  mySpinBox->setMinimumWidth(MIN_SPIN_WIDTH);

  intervalGrpLayout->addWidget(TextLabel, 0, 0);
  intervalGrpLayout->addWidget(mySpinBox, 0, 1);

  aBtnLayout->addWidget( myButtonOk );
  aBtnLayout->addItem( new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum ) );
  aBtnLayout->addWidget( myButtonCancel );

  topLayout->addWidget( intervalGrp, 0, 0 );
  topLayout->addLayout( aBtnLayout, 1, 0 ); 

  END_OF("ToolsGUI_IntervalWindow constructor")
}

/*!
  Destructor
*/
ToolsGUI_IntervalWindow::~ToolsGUI_IntervalWindow() {}

/*!
   Sets start interval size
*/
void ToolsGUI_IntervalWindow::setValue(const int size)
{
  mySpinBox->setValue(size);
}

/*!
   \return interval size
*/
int ToolsGUI_IntervalWindow::getValue()
{
  return mySpinBox->value();
}

/*!
   \return a pointer to myButtonOk
*/
QPushButton* ToolsGUI_IntervalWindow::Ok()
{
  return myButtonOk;
}

/*!
   \return a pointer to myButtonCancel
*/
QPushButton* ToolsGUI_IntervalWindow::Cancel()
{
  return myButtonCancel;
}
