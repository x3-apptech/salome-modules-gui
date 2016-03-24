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

// File:      QtxLogoMgr.cxx
// Author:    Sergey TELKOV
//
#include "QtxLogoMgr.h"

#include <QLabel>
#include <QImage>
#include <QBitmap>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QPointer>
#include <QApplication>
#include <QMovie>

/*!
  \class QtxLogoMgr::LogoBox
  \internal
  \brief Logo images container.
*/

class QtxLogoMgr::LogoBox : public QWidget
{
public:
  LogoBox( QMenuBar* );

  QMenuBar*      menuBar() const;
  virtual bool   eventFilter( QObject*, QEvent* );
  void           setLabels( const QList<QLabel*>& );

protected:
  virtual void   customEvent( QEvent* );

private:
  void           updateCorner();
  void           updateContents();

private:
  typedef QPointer<QWidget> WidgetPtr;

private:
  QMenuBar*      myMB;       //!< parent menu bar
  QList<QLabel*> myLabels;   //!< list of labels containing logo images
  WidgetPtr      myCornWid;  //!< corner widget
};

/*!
  \brief Constructor.
  \param mb menu bar
*/
QtxLogoMgr::LogoBox::LogoBox( QMenuBar* mb )
: QWidget( mb ),
  myMB( mb ),
  myCornWid( 0 )
{
  myMB->installEventFilter( this );
  updateCorner();
}

/*!
  \brief Get menu bar.
  \return menu bar
*/
QMenuBar* QtxLogoMgr::LogoBox::menuBar() const
{
  return myMB;
}

/*!
  \brief Custom event filter.
  \param o event receiver object
  \param e event sent to object
  \return \c true if further event processing should be stopped
*/
bool QtxLogoMgr::LogoBox::eventFilter( QObject* o, QEvent* e )
{
  if ( o != menuBar() )
    return false;

  if ( e->type() == QEvent::Resize )
    updateCorner();

  if ( e->type() == QEvent::ChildAdded || e->type() == QEvent::ChildRemoved )
  {
    updateCorner();
    QApplication::postEvent( this, new QEvent( QEvent::User ) );
  }

  return false;
}

/*!
  \brief Set label widgets (logo containers).
  \param labs list of labels
*/
void QtxLogoMgr::LogoBox::setLabels( const QList<QLabel*>& labs )
{
  for ( QList<QLabel*>::iterator it = myLabels.begin(); it != myLabels.end(); ++it )
  {
    if ( !labs.contains( *it ) )
      delete *it;
  }

  myLabels = labs;
  updateContents();
}

/*!
  \brief Custom event processing (update logo widget).
  \param e event (not used)
*/
void QtxLogoMgr::LogoBox::customEvent( QEvent* /*e*/ )
{
  updateCorner();
}

/*!
  \brief Update menu bar's corner widget.
*/
void QtxLogoMgr::LogoBox::updateCorner()
{
  if ( menuBar()->cornerWidget() == this )
    return;

  myCornWid = menuBar()->cornerWidget();
  myMB->setCornerWidget( this );
  updateContents();
}

/*!
  \brief Update logo manager contents.
*/
void QtxLogoMgr::LogoBox::updateContents()
{
  if ( layout() )
    delete layout();

  QHBoxLayout* base = new QHBoxLayout( this );
  base->setMargin( 0 );
  base->setSpacing( 3 );

  for ( QList<QLabel*>::const_iterator it = myLabels.begin(); it != myLabels.end(); ++it )
    base->addWidget( *it );

  if ( myCornWid )
    base->addWidget( myCornWid );

  QApplication::sendPostedEvents();
}

/*!
  \class QtxLogoMgr
  \brief Provides a way to install logo pictures to the application main window.

  The class includes the following functionality:
  - add the logo image
  - remove logo image
  - support static images and animated images (QMovie)
  - start/stop and pause/resume the animated logos
*/

/*!
  \brief Constructor.
  \param mb parent menu bar
*/
QtxLogoMgr::QtxLogoMgr( QMenuBar* mb )
: QObject( mb )
{
  myBox = new LogoBox( mb );
}

/*!
  \brief Destructor.
*/
QtxLogoMgr::~QtxLogoMgr()
{
}

/*!
  \brief Get menu bar.
  \return parent menu bar
*/
QMenuBar* QtxLogoMgr::menuBar() const
{
  return myBox->menuBar();
}

/*!
  \brief Get number of logo images.
  \return current number of logo images
*/
int QtxLogoMgr::count() const
{
  return myLogos.count();
}

/*!
  \brief Insert new logo pixmap to the menu bar area.
  \param id unique string identifier of the logo
  \param pix logo pixmap
  \param index logo position (if < 0, logo is added to the end)
*/
void QtxLogoMgr::insert( const QString& id, const QPixmap& pix, const int index )
{
  if ( pix.isNull() )
    return;

  LogoInfo& inf = insert( id, index );

  inf.pix = pix;

  generate();
}

/*!
  \brief Insert new animated logo to the menu bar area.
  \param id unique string identifier of the logo
  \param pix logo movie
  \param index logo position (if < 0, logo is added to the end)
*/
void QtxLogoMgr::insert( const QString& id, QMovie* movie, const int index )
{
  if ( !movie )
    return;

  LogoInfo& inf = insert( id, index );

  inf.mov = movie;
  movie->setParent( this );
  movie->setCacheMode( QMovie::CacheAll );
  movie->jumpToFrame( 0 );

  generate();
}

/*!
  \brief Insert new logo information structure into the logos list.
  \param id unique string identifier of the logo
  \param index logo position (if < 0, logo is added to the end)
  \return logo information object
*/
QtxLogoMgr::LogoInfo& QtxLogoMgr::insert( const QString& id, const int index )
{
  LogoInfo empty;
  empty.id = id;
  empty.mov = 0;

  int idx = find( id );
  if ( idx < 0 )
  {
    idx = index < (int)myLogos.count() ? index : -1;
    if ( idx < 0 )
    {
      myLogos.append( empty );
      idx = myLogos.count() - 1;
    }
    else
      myLogos.insert( idx, empty );
  }

  LogoInfo& inf = myLogos[idx];

  return inf;
}

/*!
  \brief Remove a logo.
  \param id logo identifier
*/
void QtxLogoMgr::remove( const QString& id )
{
  int idx = find( id );
  if ( idx < 0 )
    return;

  myLogos.removeAt( idx );

  generate();
}

/*!
  \brief Removes all logos.
*/
void QtxLogoMgr::clear()
{
  myLogos.clear();
  generate();
}

/*!
  \brief Start the animation of movie logo.

  If \a id is empty, all movie logos animation are started.

  \param id logo identifier
*/
void QtxLogoMgr::startAnimation( const QString& id )
{
  QList<QMovie*> movList;
  movies( id, movList );

  for ( QList<QMovie*>::iterator it = movList.begin(); it != movList.end(); ++it )
    (*it)->start();
}

/*!
  \brief Stop the animation of movie logo.

  If \a id is empty, all movie logos animation are stopped.

  \param id logo identifier
*/
void QtxLogoMgr::stopAnimation( const QString& id )
{
  QList<QMovie*> movList;
  movies( id, movList );

  for ( QList<QMovie*>::iterator it = movList.begin(); it != movList.end(); ++it )
    (*it)->stop();
}

/*!
  \brief Pause/resume the animation of movie logo.

  If \a pause is \c true, the animation is paused; otherwise
  it is resumed.
  
  If \a id is empty, the operation is performed for all movis logos.

  \param pause if \c true, pause animation, otherwise resume it
  \param id logo identifier
*/
void QtxLogoMgr::pauseAnimation( const bool pause, const QString& id )
{
  QList<QMovie*> movList;
  movies( id, movList );

  for ( QList<QMovie*>::iterator it = movList.begin(); it != movList.end(); ++it )
    (*it)->setPaused( pause );
}

/*!
  \brief Regenerate logo manager widget contents.
  
  Insert logo to menu bar if it not yet done, layout the widget.
*/
void QtxLogoMgr::generate()
{
  if ( !menuBar() )
    return;

  QList<QLabel*> labels;
  for ( LogoList::const_iterator it = myLogos.begin(); it != myLogos.end(); ++it )
  {
    QPixmap pix = (*it).pix;
    QMovie* mov = (*it).mov;
    if ( !pix.isNull() && !pix.mask() )
    {
      QBitmap bm;
      QImage img = pix.toImage();
      if ( img.hasAlphaChannel() )
        bm = QPixmap::fromImage( img.createAlphaMask() );
      else
        bm = QPixmap::fromImage( img.createHeuristicMask() );
      pix.setMask( bm );
    }

    QLabel* logoLab = new QLabel( myBox );
    if ( mov )
      logoLab->setMovie( mov );
    else
    {
      logoLab->setPixmap( (*it).pix );
//      if ( !pix.mask().isNull() )
//          logoLab->setMask( pix.mask() );
    }

    logoLab->setScaledContents( false );
    logoLab->setAlignment( Qt::AlignCenter );

    labels.append( logoLab );
  }

  myBox->setLabels( labels );
}

/*!
  \brief Search the logo by the specified \a id.
  \param id logo identifier
  \return index of logo or -1 if not found
*/
int QtxLogoMgr::find( const QString& id ) const
{
  int idx = -1;
  for ( int i = 0; i < myLogos.count() && idx < 0; i++ )
  {
    if ( myLogos.at( i ).id == id )
      idx = i;
  }
  return idx;
}

/*!
  \brief Get movie logos by specified \a id.

  If \a id is empty, all movie logos are returned.

  \param id logo identifier
  \param lst list of movies, which satisfy the \a id
*/
void QtxLogoMgr::movies( const QString& id, QList<QMovie*>& lst ) const
{
  lst.clear();
  for ( LogoList::const_iterator it = myLogos.begin(); it != myLogos.end(); ++it )
  {
    if ( (*it).mov && ( id.isEmpty() || id == (*it).id ) )
      lst.append( (*it).mov );
  }
}
