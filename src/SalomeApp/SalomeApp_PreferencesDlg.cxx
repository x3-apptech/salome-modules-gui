// File:      SalomeApp_PreferencesDlg.cxx
// Author:    Sergey TELKOV

#include "SalomeApp_PreferencesDlg.h"

#include <QtxListResourceEdit.h>

#include <qlayout.h>

SalomeApp_PreferencesDlg::SalomeApp_PreferencesDlg( QtxResourceMgr* resMgr, QWidget* parent )
: QtxDialog( parent, 0, true, false, Standard | Apply )
{
  setCaption( tr( "CAPTION" ) );

  QVBoxLayout* main = new QVBoxLayout( mainFrame(), 5 );

  myEdit = new QtxListResourceEdit( resMgr, mainFrame() );
  main->addWidget( myEdit );

  setFocusProxy( myEdit );

  setDialogFlags( AlignOnce );

  connect( this, SIGNAL( dlgHelp() ),  this, SLOT( onHelp() ) );
  connect( this, SIGNAL( dlgApply() ), this, SLOT( onApply() ) );
  connect( myEdit, SIGNAL( resourceChanged( QString&, QString& ) ),
           this, SIGNAL( resourceChanged( QString&, QString& ) ) );
}

SalomeApp_PreferencesDlg::~SalomeApp_PreferencesDlg()
{
}

void SalomeApp_PreferencesDlg::store()
{
  myEdit->store();
}

void SalomeApp_PreferencesDlg::retrieve()
{
  myEdit->retrieve();
}

void SalomeApp_PreferencesDlg::show()
{
  retrieve();
  myEdit->toBackup();

  QtxDialog::show();
}

void SalomeApp_PreferencesDlg::accept()
{
  QtxDialog::accept();

  store();
}

void SalomeApp_PreferencesDlg::reject()
{
  QtxDialog::reject();

  myEdit->fromBackup();
}

int SalomeApp_PreferencesDlg::addPreference( const QString& label, const int pId, const int type,
					     const QString& section, const QString& param, SalomeApp_Module* mod )
{
  int id = myEdit->addItem( label, pId, type, section, param );
  if ( id != -1 && mod )
    myPrefMod.insert( id, mod );
  return id;
}

QVariant SalomeApp_PreferencesDlg::property( const int id, const QString& param ) const
{
  return myEdit->QtxResourceEdit::property( id, param );
}

void SalomeApp_PreferencesDlg::setProperty( const int id, const QString& param, const QVariant& prop )
{
  myEdit->QtxResourceEdit::setProperty( id, param, prop );
}

void SalomeApp_PreferencesDlg::onHelp()
{
}

void SalomeApp_PreferencesDlg::onApply()
{
  myEdit->store();
}

SalomeApp_Module* SalomeApp_PreferencesDlg::module( const int id ) const
{
  SalomeApp_Module* mod = 0;
  if ( myPrefMod.contains( id ) )
    mod = myPrefMod[id];
  return mod;
}
