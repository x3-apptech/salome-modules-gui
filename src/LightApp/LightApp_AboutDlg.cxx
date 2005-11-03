// File:      LightApp_AboutDlg.cxx
// Created:   03.06.2005 13:52:45
// Author:    Sergey TELKOV
// Copyright (C) CEA 2005

#include "LightApp_AboutDlg.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <qlabel.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qgroupbox.h>

/*!Constructor.*/
LightApp_AboutDlg::LightApp_AboutDlg( const QString& defName, const QString& defVer, QWidget* parent )
: QtxDialog( parent, "salome_about_dialog", true, false, None )
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QPixmap ico = resMgr->loadPixmap( "LightApp", tr( "ICO_ABOUT" ), false );
  if ( !ico.isNull() )
    setIcon( ico );

  QPalette pal = palette();
  QColorGroup cg = pal.active();
  cg.setColor( QColorGroup::Foreground, Qt::darkBlue ); 
  cg.setColor( QColorGroup::Background, Qt::white );
  pal.setActive( cg ); pal.setInactive( cg ); pal.setDisabled( cg );
  setPalette(pal);

  QVBoxLayout* main = new QVBoxLayout( mainFrame() );
  QGroupBox* base = new QGroupBox( 1, Qt::Horizontal, "", mainFrame() );
  base->setFrameStyle( QFrame::NoFrame );
  base->setInsideMargin( 0 );
  main->addWidget( base );

  QLabel* screen = new QLabel( base );
  screen->setScaledContents( true );
  screen->setAlignment( Qt::AlignCenter );
  screen->setFrameStyle( QFrame::Box | QFrame::Plain );

  QLabel* title = new QLabel( base );
  title->setAlignment( Qt::AlignCenter );
  changeFont( title, true, false, false, 5 );

  QLabel* version = new QLabel( base );
  version->setAlignment( Qt::AlignCenter );
  changeFont( version, false, true, false, 2 );
  
  QLabel* copyright = new QLabel( base );
  copyright->setAlignment( Qt::AlignCenter );
  changeFont( copyright, false, false, false, 1 );

  QLabel* license = new QLabel( base );
  license->setAlignment( Qt::AlignCenter );
  changeFont( license, false, false, false, 1 );

  screen->setPixmap( resMgr->loadPixmap( "LightApp", tr( "ABOUT" ), false ) );
  checkLabel( screen );

  QString titleText = tr( "ABOUT_TITLE" );
  if ( titleText == "ABOUT_TITLE" )
    titleText = defName;
  title->setText( titleText );
  checkLabel( title );

  QString verText = tr( "ABOUT_VERSION" );
  if ( verText.contains( "%1" ) )
    verText = verText.arg( defVer );
  version->setText( verText );
  checkLabel( version );

  copyright->setText( tr( "ABOUT_COPYRIGHT" ) );
  checkLabel( copyright );

  license->setText( tr( "ABOUT_LICENSE" ) );
  checkLabel( license );

  QString capText = tr( "ABOUT_CAPTION" );
  if ( capText.contains( "%1" ) )
    capText = capText.arg( defName );
  setCaption( capText );

  setSizeGripEnabled( false );
}

/*!Destructor.*/
LightApp_AboutDlg::~LightApp_AboutDlg()
{
  //! Do nothing.
}

/*!On mouse press event.*/
void LightApp_AboutDlg::mousePressEvent( QMouseEvent* )
{
  accept();
}

/*!Change font of widget \a wid.
 *\param wid - QWidget
 *\param bold - boolean value
 *\param italic - boolean value
 *\param underline - boolean value
 *\param inc - integer increment for font point size.
 */
void LightApp_AboutDlg::changeFont( QWidget* wid, const bool bold, const bool italic,
                                     const bool underline, const int inc ) const
{
  if ( !wid )
    return;

  QFont widFont = wid->font();
  widFont.setBold( bold );
  widFont.setItalic( italic );
  widFont.setUnderline( underline );
  widFont.setPointSize( widFont.pointSize() + inc );
}

/*!Check lable \a lab.*/
void LightApp_AboutDlg::checkLabel( QLabel* lab ) const
{
  if ( !lab )
    return;

  bool vis = !lab->text().stripWhiteSpace().isEmpty() ||
             ( lab->pixmap() && !lab->pixmap()->isNull() );
  vis ? lab->show() : lab->hide();
}
