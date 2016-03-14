// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

// File:      LightApp_AboutDlg.cxx
// Created:   03.06.2005 13:52:45
// Author:    Sergey TELKOV
//
#include "LightApp_AboutDlg.h"
#include "LightApp_Application.h"

#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <QtxGridBox.h>

#include <QLabel>
#include <QVBoxLayout>
#include <QPixmap>
#include <QIcon>
#include <QGroupBox>
#include <QTabWidget>
#include <QPushButton>

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

  QTabWidget* tw = new QTabWidget( mainFrame() );
  
  QGridLayout* main = new QGridLayout( mainFrame() );
  main->addWidget( tw, 0, 0, 1, 3 );
  
  QtxGridBox* base = new QtxGridBox( 1, Qt::Horizontal, tw, 0, 0 );
  base->setInsideMargin( 0 );

  tw->addTab(base, tr("ABOUT_BASE") );

  tw->addTab(getModulesInfoWidget(tw), tr("ABOUT_MODULE_INFOS") );

  QPushButton * btn = new QPushButton( tr("ABOUT_CLOSE"), mainFrame() );

  main->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 0, 1, 1);

  main->addWidget( btn, 1, 1, 1, 1);

  main->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), 1, 2, 1, 1);

  connect(btn, SIGNAL( clicked() ), this, SLOT( accept()) );


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

/*! Return widget with info about versions of modules */
QWidget* LightApp_AboutDlg::getModulesInfoWidget(QWidget* parent) const {

  QWidget* modulesInfo = new QWidget(parent);
  QGridLayout* gridLayout = new QGridLayout(modulesInfo);

  LightApp_Application* app = dynamic_cast<LightApp_Application*>(SUIT_Session::session()->activeApplication());
  if(app) {

    CAM_Application::ModuleShortInfoList info = app->getVersionInfo();
    
    CAM_Application::ModuleShortInfoList::const_iterator it = info.constBegin();
    int i = 0;
    
    QString unknownVersion = tr("ABOUT_UNKNOWN_VERSION");

    while (it != info.constEnd()) {
      QLabel * name = new QLabel( "<h4>" + (*it).name + ":</h4>", modulesInfo);
      QString v = (*it).version.isEmpty() ?  unknownVersion : (*it).version;
      QLabel * version = new QLabel("<h4>" + v + "</h4>",modulesInfo);
      gridLayout->addWidget(name , i, 0);
      gridLayout->addWidget(version , i, 1);
      gridLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum), i, 2);
      it++;
      i++;
    }
    gridLayout->addItem(new QSpacerItem(0, 0,  QSizePolicy::Minimum, QSizePolicy::Expanding), i, 0);
    gridLayout->addItem(new QSpacerItem(0, 0,  QSizePolicy::Minimum, QSizePolicy::Expanding), i, 1);
  }  
  return modulesInfo;
}
