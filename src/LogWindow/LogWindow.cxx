//  KERNEL SALOME_Event : Define event posting mechanism
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 

#include "LogWindow.h"

#include <qfile.h>
#include <qlayout.h>
#include <qaction.h>
#include <qpopupmenu.h>
#include <qtextbrowser.h>
#include <qapplication.h>

#include <SUIT_Tools.h>
#include <SUIT_FileDlg.h>
#include <SUIT_Session.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>

#define DEFAULT_SEPARATOR "***"

//****************************************************************
static QString plainText( const QString& richText )
{
  QString aText = richText;
  int startTag = aText.find('<');
  while ( 1 ) {
    if ( startTag < 0 )
      break;
    int finishTag = aText.find('>',startTag);
    if (finishTag < 0)
      break;
    aText = aText.remove(startTag, finishTag-startTag+1);
    startTag = aText.find('<');
  }
  return aText;
}

//****************************************************************

LogWindow::LogWindow( QWidget* parent )
: QFrame( parent ),
SUIT_PopupClient()
{
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QString fntSet = resMgr ? resMgr->stringValue( "Log Window", "font", QString::null ) : QString::null;

  setFont( SUIT_Tools::stringToFont( fntSet ) );

  myView = new QTextBrowser(this,"myView");
#if QT_VERSION>0x030007
  myView->setTextFormat( Qt::LogText );
#endif
  myView->viewport()->installEventFilter( this );

  QVBoxLayout* main = new QVBoxLayout( this );
  main->addWidget( myView );

  myBannerSize = 0;
  myBanner = "<b>Message Log</b>\n********************";
  mySeparator = DEFAULT_SEPARATOR;

  clear();

  createActions();
}

LogWindow::~LogWindow()
{
}

bool LogWindow::eventFilter( QObject* o, QEvent* e )
{
  if ( o == myView->viewport() && e->type() == QEvent::ContextMenu )
  {
    contextMenuRequest( (QContextMenuEvent*)e );
    return true;
  }
  return QFrame::eventFilter( o, e );
}

void LogWindow::setBanner( const QString& banner )
{
  myBanner = banner;

  clear( false );
}

void LogWindow::setSeparator( const QString& separator )
{
  mySeparator = separator;

  clear( false );
}

void LogWindow::putMessage( const QString& message, bool addSeparator )
{
  myView->append( message );
  myHistory.append( plainText( message ) );

  if ( addSeparator && !mySeparator.isNull() )
  {
    myView->append( mySeparator );   // add separator
    myHistory.append( plainText( mySeparator ) );
  }
  myView->scrollToBottom();
}

void LogWindow::clear( bool clearHistory )
{
  myView->clear();
  if ( clearHistory )
    myHistory.clear();

  if ( !myBanner.isEmpty() )
  {
    myView->append( myBanner );
    myBannerSize = myView->paragraphs();
  }
  else
    myBannerSize = 0;
}

bool LogWindow::saveLog( const QString& fileName )
{
  QFile file( fileName );
  if ( !file.open( IO_WriteOnly ) )
    return false;

  QTextStream stream( &file );

  stream << "*****************************************"   << endl;
  stream << "Message Log"                                 << endl;
  stream << QDate::currentDate().toString( "dd.MM:yyyy" ) << "  ";
  stream << QTime::currentTime().toString( "hh:mm:ss" )   << endl;
  stream << "*****************************************"   << endl;

  for ( uint i = 0; i < myHistory.count(); i++ )
    stream << myHistory[ i ] << endl;

  file.close();
  return true;
}

void LogWindow::createActions()
{
  QAction* a = new QAction( "", tr( "&Copy" ), 0, this );
  a->setStatusTip( tr( "&Copy" ) );
  connect( a, SIGNAL( activated() ), SLOT( onCopy()));
  myActions.insert( CopyId, a );

  a = new QAction( "", tr( "Clea&r" ), 0, this );
  a->setStatusTip( tr( "Clea&r" ) );
  connect( a, SIGNAL( activated() ), SLOT( onClear()));
  myActions.insert( ClearId, a );

  a = new QAction( "", tr( "Select &All" ), 0, this );
  a->setStatusTip( tr( "Select &All" ) );
  connect( a, SIGNAL( activated() ), SLOT( onSelectAll()));
  myActions.insert( SelectAllId, a );

  a = new QAction( "", tr( "&Save log to file..." ), 0, this );
  a->setStatusTip( tr( "&Save log to file..." ) );
  connect( a, SIGNAL( activated() ), SLOT( onSaveToFile()));
  myActions.insert( SaveToFileId, a );
}

void LogWindow::contextMenuPopup( QPopupMenu* popup )
{
  myActions[ CopyId ]->addTo( popup );
  myActions[ ClearId ]->addTo( popup );
  
  popup->insertSeparator();
  
  myActions[ SelectAllId ]->addTo( popup );
  
  popup->insertSeparator();
  
  myActions[ SaveToFileId ]->addTo( popup );

  updateActions();
}

void LogWindow::updateActions()
{
  int paraFrom, paraTo, indexFrom, indexTo;
  myView->getSelection( &paraFrom, &indexFrom, &paraTo, &indexTo );
  bool allSelected = myView->hasSelectedText() &&
                     !paraFrom && paraTo == myView->paragraphs() - 1 && 
                     !indexFrom && indexTo == myView->paragraphLength( paraTo );
  myActions[ CopyId ]->setEnabled( myView->hasSelectedText() );
  myActions[ ClearId ]->setEnabled( myView->paragraphs() > myBannerSize );
  myActions[ SelectAllId ]->setEnabled( !allSelected );
  myActions[ SaveToFileId ]->setEnabled( myHistory.count() > 0 );
}

void LogWindow::onSaveToFile()
{
  QString aUsedFilter;
  QString aName = QFileDialog::getSaveFileName( QString::null, QString( "*.log" ), this,
                                                0, QString::null, &aUsedFilter );
  if ( aName.isNull() ) 
    return;

  int aStart = aUsedFilter.find('*');
  int aEnd = aUsedFilter.find(')', aStart + 1);
  QString aExt = aUsedFilter.mid(aStart + 1, aEnd - aStart - 1);
  if ( aExt.contains('*') == 0 )
  {
    if ( aName.contains( aExt, false ) == 0 )
      aName += aExt;
  }


  if ( !aName.isNull() )
  {
    QApplication::setOverrideCursor( Qt::waitCursor );
    
    bool bOk = saveLog( aName );

    QApplication::restoreOverrideCursor();

    if ( !bOk )
      SUIT_MessageBox::error1( this, tr( "Error" ), tr( "Can't save file" ), tr( "OK" ) );
  }
}

void LogWindow::onSelectAll()
{
  if ( myView )
    myView->selectAll();
}

void LogWindow::onClear()
{
  clear( false );
}

void LogWindow::onCopy()
{
  if ( myView )
    myView->copy();
}
