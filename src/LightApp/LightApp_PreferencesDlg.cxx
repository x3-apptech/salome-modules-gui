// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// File:      LightApp_PreferencesDlg.cxx
// Author:    Sergey TELKOV
//
#include "LightApp_PreferencesDlg.h"
#include "LightApp_Preferences.h"

#include "QtxResourceMgr.h"

#include <SUIT_MessageBox.h>

#include <QAbstractButton>
#include <QVBoxLayout>
#include <QFileDialog>

/*!
  Constructor.
*/
LightApp_PreferencesDlg::LightApp_PreferencesDlg( LightApp_Preferences* prefs, QWidget* parent )
: QtxDialog( parent, true, true, OK | Close | Apply ),
myPrefs( prefs ), mySaved ( false )
{
  setWindowTitle( tr( "CAPTION" ) );

  QVBoxLayout* main = new QVBoxLayout( mainFrame() );
  main->setMargin( 0 );
  main->setSpacing( 5 );
  main->addWidget( myPrefs );

  setFocusProxy( myPrefs );
  myPrefs->setFrameStyle( QFrame::Box | QFrame::Sunken );
  myPrefs->show();

  setButtonPosition( Right, Close );

  setDialogFlags( AlignOnce );

  connect( this, SIGNAL( dlgHelp() ),  this, SLOT( onHelp() ) );
  connect( this, SIGNAL( dlgApply() ), this, SLOT( onApply() ) );

  QAbstractButton* defBtn = userButton( insertButton( tr( "DEFAULT_BTN_TEXT" ) ) );
  if ( defBtn )
    connect( defBtn, SIGNAL( clicked() ), this, SLOT( onDefault() ) );
  QAbstractButton* impBtn = userButton( insertButton( tr( "IMPORT_BTN_TEXT" ) ) );
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

  mainFrame()->layout()->removeWidget( myPrefs );
  myPrefs->setParent( 0 );
  myPrefs->hide();
  myPrefs = 0;
}

/*!Show/hide dialog.*/
void LightApp_PreferencesDlg::setVisible(bool visible)
{
  if ( visible ) {
    myPrefs->retrieve();
    myPrefs->toBackup();
  }

  QtxDialog::setVisible(visible);
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
  if( SUIT_MessageBox::Ok == SUIT_MessageBox::question( this, tr( "WARNING" ), tr( "DEFAULT_QUESTION" ),
                                                        SUIT_MessageBox::Ok | SUIT_MessageBox::Cancel,
                                                        SUIT_MessageBox::Ok ) )
    {
      if ( myPrefs && myPrefs->resourceMgr() )
        {
          QtxResourceMgr::WorkingMode prev = myPrefs->resourceMgr()->workingMode();
          myPrefs->resourceMgr()->setWorkingMode( QtxResourceMgr::IgnoreUserValues );
          myPrefs->retrieve();
          myPrefs->resourceMgr()->setWorkingMode( prev );
        }
    }
}

/*! Import preferences from some file */
void LightApp_PreferencesDlg::onImportPref()
{
  QtxResourceMgr* mgr = myPrefs->resourceMgr();
  if( !mgr )
    return;

  QFileDialog dlg( this, tr("IMPORT_PREFERENCES"), ".", "*" );
  dlg.setObjectName( "" );
  //dlg.setShowHiddenFiles( true );
  dlg.exec();

  QStringList files = dlg.selectedFiles();
  if ( files.isEmpty() )
    return;

  QString fname = files[0];
  if( mgr->import( fname ) )
  {
    myPrefs->retrieve();
    myPrefs->toBackup();
  }
}
