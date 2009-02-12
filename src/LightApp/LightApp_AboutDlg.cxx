//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      LightApp_AboutDlg.cxx
// Created:   03.06.2005 13:52:45
// Author:    Sergey TELKOV
//
#include "LightApp_AboutDlg.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <QtxGridBox.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <QGroupBox>

/*!Constructor.*/
LightApp_AboutDlg::LightApp_AboutDlg( const QString& defName, const QString& defVer, QWidget* parent )
: QtxDialog( parent, true, false, None )
{
  setObjectName( "salome_about_dialog" );

  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QPixmap ico = resMgr->loadPixmap( "LightApp", tr( "ICO_ABOUT" ), false );
  if ( !ico.isNull() )
    setWindowIcon( ico );

  QPalette pal = palette();

  pal.setBrush( QPalette::Active, QPalette::WindowText, QBrush( Qt::darkBlue ) );
  pal.setBrush( QPalette::Active, QPalette::Window,     QBrush( Qt::white ) );

  pal.setBrush( QPalette::Inactive, QPalette::WindowText, QBrush( Qt::darkBlue ) );
  pal.setBrush( QPalette::Inactive, QPalette::Window,     QBrush( Qt::white ) );

  pal.setBrush( QPalette::Disabled, QPalette::WindowText, QBrush( Qt::darkBlue ) );
  pal.setBrush( QPalette::Disabled, QPalette::Window,     QBrush( Qt::white ) );

  setPalette(pal);

  QVBoxLayout* main = new QVBoxLayout( mainFrame() );
  QtxGridBox* base = new QtxGridBox( 1, Qt::Horizontal, mainFrame(), 0, 0 );
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
  setWindowTitle( capText );

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

  bool vis = !lab->text().trimmed().isEmpty() ||
             ( lab->pixmap() && !lab->pixmap()->isNull() );
  vis ? lab->show() : lab->hide();
}
