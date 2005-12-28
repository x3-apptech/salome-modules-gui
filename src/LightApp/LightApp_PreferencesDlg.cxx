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
// See http://www.salome-platform.org/
//
// File:      LightApp_PreferencesDlg.cxx
// Author:    Sergey TELKOV

#include "LightApp_PreferencesDlg.h"

#include "LightApp_Preferences.h"

#include <qvbox.h>
#include <qlayout.h>

/*!
  Constructor.
*/
LightApp_PreferencesDlg::LightApp_PreferencesDlg( LightApp_Preferences* prefs, QWidget* parent )
: QtxDialog( parent, 0, true, false, OK | Close | Apply ),
myPrefs( prefs ), mySaved ( false )
{
  setCaption( tr( "CAPTION" ) );

  QVBoxLayout* main = new QVBoxLayout( mainFrame(), 5 );

  QVBox* base = new QVBox( mainFrame() );
  main->addWidget( base );

  myPrefs->reparent( base, QPoint( 0, 0 ), true );

  setFocusProxy( myPrefs );

  setButtonPosition( Right, Close );

  setDialogFlags( AlignOnce );

  connect( this, SIGNAL( dlgHelp() ),  this, SLOT( onHelp() ) );
  connect( this, SIGNAL( dlgApply() ), this, SLOT( onApply() ) );
}

/*!
  Destructor.
*/
LightApp_PreferencesDlg::~LightApp_PreferencesDlg()
{
  if ( !myPrefs )
    return;

  myPrefs->reparent( 0, QPoint( 0, 0 ), false );
  myPrefs = 0;
}

/*!Show dialog.*/
void LightApp_PreferencesDlg::show()
{
  myPrefs->retrieve();
  myPrefs->toBackup();

  QtxDialog::show();
}

/*!Store preferences on accept.*/
void LightApp_PreferencesDlg::accept()
{
  QtxDialog::accept();

  myPrefs->store();
  mySaved = true;
}

/*!Reject. Restore preferences from backup.*/
void LightApp_PreferencesDlg::reject()
{
  QtxDialog::reject();

  myPrefs->fromBackup();
}

/*!Do nothing.*/
void LightApp_PreferencesDlg::onHelp()
{
}

/*!Store preferences on apply.*/
void LightApp_PreferencesDlg::onApply()
{
  myPrefs->store();
  myPrefs->toBackup();
  mySaved = true;
}
