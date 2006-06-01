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
// File:      LightApp_PreferencesDlg.cxx
// Author:    Sergey TELKOV

#include "LightApp_PreferencesDlg.h"
#include "LightApp_Preferences.h"

#include "QtxResourceMgr.h"

#include <qbutton.h>
#include <qlayout.h>
#include <qmessagebox.h>
#include <qvbox.h>
#include <qfiledialog.h>

/*!
  Constructor.
*/
LightApp_PreferencesDlg::LightApp_PreferencesDlg( LightApp_Preferences* prefs, QWidget* parent )
: QtxDialog( parent, 0, true, true, OK | Close | Apply ),
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

  QButton* defBtn = userButton( insertButton( tr( "DEFAULT_BTN_TEXT" ) ) );
  if ( defBtn )
    connect( defBtn, SIGNAL( clicked() ), this, SLOT( onDefault() ) );
  QButton* impBtn = userButton( insertButton( tr( "IMPORT_BTN_TEXT" ) ) );
  if( impBtn )
    connect( impBtn, SIGNAL( clicked() ), this, SLOT( onImportPref() ) );
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
  
  // Fix for Bug PAL11197: Restoring the corrected values from resource manager.
  // (Correcting in VisuGUI.cxx and SMESHGUI.cxx in methods
  // ::preferencesChanged( const QString& sect, const QString& name ))
  myPrefs->retrieve();
  //
  
  myPrefs->toBackup();
  mySaved = true;
}

/*! Restore default preferences*/
void LightApp_PreferencesDlg::onDefault()
{
  if( QMessageBox::Ok == QMessageBox::information( this, tr( "WARNING" ), tr( "DEFAULT_QUESTION" ), QMessageBox::Ok, QMessageBox::Cancel ) )
    {
      if ( myPrefs && myPrefs->resourceMgr() )
	{
          bool prev = myPrefs->resourceMgr()->ignoreUserValues();
	  myPrefs->resourceMgr()->setIgnoreUserValues( true ); 
	  myPrefs->retrieve();
          myPrefs->resourceMgr()->setIgnoreUserValues( prev );
	}      
    }
}

/*! Import preferences from some file */
void LightApp_PreferencesDlg::onImportPref()
{
  QtxResourceMgr* mgr = myPrefs->resourceMgr();
  if( !mgr )
    return;

  QFileDialog dlg( ".", "*", this, "", tr( "IMPORT_PREFERENCES" ) );
  dlg.setShowHiddenFiles( true );
  dlg.exec();
  QString fname = dlg.selectedFile();
  if( fname.isEmpty() )
    return;

  if( mgr->import( fname ) )
  {
    myPrefs->retrieve();
    myPrefs->toBackup();
  }
}
