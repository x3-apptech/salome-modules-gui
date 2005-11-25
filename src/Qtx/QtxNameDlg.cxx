//  File   : QtxNameDlg.cxx
//  Author : Vadim SANDLER
//  $Header$

#include "QtxNameDlg.h"

#include <qlayout.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#ifndef WIN32
using namespace std;
#endif

/*!
  Constructor
*/
QtxNameDlg::QtxNameDlg( QWidget* parent )
: QtxDialog( parent ? parent : NULL,//application()->desktop(), 
	     "QtxNameDlg", true, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu )
{
  setCaption( tr("TLT_RENAME") );
  setSizeGripEnabled( TRUE );

  clearButtonFlags( Help );
  setButtonFlags( OKCancel );

  QHBoxLayout* topLayout = new QHBoxLayout( mainFrame(), 0, 5 );

  /***************************************************************/
  QLabel* TextLabel = new QLabel( mainFrame(), "TextLabel1" );
  TextLabel->setText( tr( "NAME_LBL" ) );
  topLayout->addWidget( TextLabel );
  
  myLineEdit = new QLineEdit( mainFrame(), "LineEdit1" );
  myLineEdit->setMinimumSize( 250, 0 );
  topLayout->addWidget( myLineEdit );
 
  /* Move widget on the botton right corner of main widget */
  setAlignment( Qt::AlignCenter );

  QPushButton* btn = dynamic_cast<QPushButton*>( button( OK ) );
  if( btn )
    btn->setAutoDefault( true );

  setFocusProxy( myLineEdit );
}

/*!
  Destructor
*/
QtxNameDlg::~QtxNameDlg()
{
}

/*!
  Sets name
*/
void QtxNameDlg::setName( const QString& name )
{
  myLineEdit->setText( name );
  myLineEdit->end(false);
  myLineEdit->home(true);
}

/*!
  Returns name entered by user
*/
QString QtxNameDlg::name()
{
  return myLineEdit->text();
}

void QtxNameDlg::accept()
{
  if ( name().stripWhiteSpace().isEmpty() )
    return;
  QDialog::accept();
}

/*!
  Creates modal <Rename> dialog and returns name entered [ static ]
*/
QString QtxNameDlg::getName( QWidget* parent, const QString& oldName )
{
  QString n;
  QtxNameDlg* dlg = new QtxNameDlg( parent );
  if ( !oldName.isNull() )
    dlg->setName( oldName );
  if ( dlg->exec() == QDialog::Accepted ) 
    n = dlg->name();
  delete dlg;
  return n;
}
