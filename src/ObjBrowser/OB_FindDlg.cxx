
#include <OB_FindDlg.h>
#include <OB_ObjSearch.h>
#include <OB_Browser.h>
#include <OB_ListItem.h>

#include <SUIT_DataObject.h>
#include <SUIT_MessageBox.h>

#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>

OB_FindDlg::OB_FindDlg( QWidget* parent )
: QGroupBox( 1, Qt::Horizontal, tr( "FIND" ), parent ),
  mySearch( 0 )
{
  QFrame *btns = new QFrame( this ), *checks = new QFrame( this );
  
  myData = new QLineEdit( btns );
  myToFirst = new QPushButton( "|<<", btns );
  myToLast = new QPushButton( ">>|", btns );
  myNext = new QPushButton( ">>", btns );
  myPrev = new QPushButton( "<<", btns );
  myClose = new QPushButton( tr( "CLOSE" ), checks );
  myIsCaseSens = new QCheckBox( tr( "CASE_SENSITIVE" ), checks );
  myIsRegExp = new QCheckBox( tr( "IS_REG_EXP" ), checks );
  int w = 30, h = myToFirst->height();
  myToFirst->setMaximumSize( w, h );
  myToLast->setMaximumSize( w, h );
  myNext->setMaximumSize( w, h );
  myPrev->setMaximumSize( w, h );

  QHBoxLayout* l = new QHBoxLayout( btns, 5, 5 );
  l->addWidget( myToFirst, 0 );
  l->addWidget( myPrev, 0 );
  l->addWidget( myData, 1 );
  l->addWidget( myNext, 0 );
  l->addWidget( myToLast, 0 );

  QHBoxLayout* c = new QHBoxLayout( checks, 5, 5 );
  c->addWidget( myIsCaseSens, 0 );
  c->addWidget( myIsRegExp, 0 );
  c->addWidget( myClose, 0 );

  connect( myToFirst, SIGNAL( clicked() ), this, SLOT( onFind() ) );
  connect( myToLast, SIGNAL( clicked() ), this, SLOT( onFind() ) );
  connect( myNext, SIGNAL( clicked() ), this, SLOT( onFind() ) );
  connect( myPrev, SIGNAL( clicked() ), this, SLOT( onFind() ) );
  connect( myClose, SIGNAL( clicked() ), this, SLOT( onClose() ) );
}

OB_FindDlg::~OB_FindDlg()
{
}

void OB_FindDlg::onClose()
{
  hide();
}

void OB_FindDlg::onFind()
{
  OB_ListItem* it = 0;
  mySearch->setPattern( myData->text(), myIsRegExp->isChecked(), myIsCaseSens->isChecked() );
  if( sender()==myToFirst )
    it = mySearch->findFirst();
  else if( sender()==myNext )
    it = mySearch->findNext();
  else if( sender()==myPrev )
    it = mySearch->findPrev();
  else if( sender()==myToLast )
    it = mySearch->findLast();
  if( it )
  {
    mySearch->browser()->setSelected( it->dataObject(), false );
    mySearch->browser()->listView()->ensureItemVisible( it );
  }
  else
    SUIT_MessageBox::info1( this, tr( "FIND" ), tr( "NOT_FOUND" ), tr( "OK" ) );
}

OB_ObjSearch* OB_FindDlg::getSearch() const
{
  return mySearch;
}

void OB_FindDlg::setSearch( OB_ObjSearch* s )
{
  mySearch = s;
}
