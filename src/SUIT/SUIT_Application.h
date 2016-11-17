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

#ifndef SUIT_APPLICATION_H
#define SUIT_APPLICATION_H

#include "SUIT.h"

#include <QObject>
#include <QMap>

class QIcon;
class QLabel;
class QString;
class QAction;
class QWidget;
class QKeySequence;

class SUIT_Desktop;
class SUIT_ViewManager;
class SUIT_ResourceMgr;
class SUIT_ShortcutMgr;
class SUIT_Study;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*! \class QObject
 * \brief For more information see <a href="http://doc.trolltech.com">QT documentation</a>.
 */
/*!
  An <b>Application</b> is a class which defines application configuration and behaviour.
  For example Application object defines what Viewers are used in this application, what auxilliary windows
  are present, how user can dial with them. Also Application object defines an sertain type of data structure by 
  holding of pointer on an instance of SUIT_Study class (which represents Document data structure). In other words
  Application defines type of sata structure, type of used Viewers, type of main GUI widget (Desktop),
  and other auxilliary tools.
*/

class SUIT_EXPORT SUIT_Application : public QObject
{
  Q_OBJECT

public:
  SUIT_Application();
  virtual ~SUIT_Application();

  //! Returns main widget (Desktop) of the application (if it exists)
  virtual SUIT_Desktop* desktop();

  /*! Returns \c false if application can not be closed (because of non saved data for example). 
      This method called by SUIT_Session whin closing of application was requested. */
  virtual bool          isPossibleToClose( bool& );

  /*! Performs some finalization of life cycle of this application.
      For instance, the application can force its documents(s) to close. */
  virtual void          closeApplication();

  //! Returns active Study. If Application supports wirking with several studies this method should be redefined
  virtual SUIT_Study*   activeStudy() const;

  //! Returns Name of application. Using is not defined.
  virtual QString       applicationName() const = 0;

  virtual QString       applicationVersion() const;

  //! Shows the application's main widget. For non GUI application must be redefined.
  virtual void          start();

  //! Opens document <theFileName> into active Study. If Study is empty - creates it.
  virtual bool          useFile( const QString& theFileName);

  //! Creates new empty Study if active Study = 0
  virtual void          createEmptyStudy();

  /*! Returns number of Studies. 
   *  Must be redefined in Applications which support several studies for one Application instance. */
  virtual int           getNbStudies() const;

  SUIT_ResourceMgr*     resourceMgr() const;

  SUIT_ShortcutMgr*     shortcutMgr() const;

  //! Puts the message to the status bar  
  void                  putInfo ( const QString&, const int = 0 );

  //! Invokes application-specific "Open/Save File" dialog and returns the selected file name.
  virtual QString       getFileName( bool open, const QString& initial, const QString& filters, 
                                     const QString& caption, QWidget* parent ) = 0;

  //! Invokes application-specific "Select Directory" dialog and returns the selected directory name.
  virtual QString       getDirectory( const QString& initial, const QString& caption, QWidget* parent ) = 0;


  virtual int           viewManagerId ( const SUIT_ViewManager* ) const = 0;
  virtual void          viewManagers( const QString&, QList<SUIT_ViewManager*>& ) const = 0;
  QAction*              action( const int ) const;

  void                  addPostRoutine( void (*theRoutine)() );

signals:
  void                  applicationClosed( SUIT_Application* );
  void                  activated( SUIT_Application* );
//  void                  moving();
  void                  infoChanged( QString );

public slots:
  virtual void          updateCommandsStatus();
  virtual void          onHelpContextModule( const QString&, const QString&, const QString& = QString() );

private slots:
  void                  onInfoClear();

protected:
  SUIT_Application*     startApplication( int, char** ) const;
  SUIT_Application*     startApplication( const QString&, int, char** ) const;

  virtual void          setDesktop( SUIT_Desktop* );

  //! Creates a new Study instance. Must be redefined in new application according to its Study type.
  virtual SUIT_Study*   createNewStudy();
  virtual void          setActiveStudy( SUIT_Study* );
  
  /** @name Create tool functions*/ //@{
  int                   createTool( const QString&, const QString& = QString() );
  int                   createTool( const int, const int, const int = -1 );
  int                   createTool( const int, const QString&, const int = -1 );
  int                   createTool( QAction*, const int, const int = -1, const int = -1 );
  int                   createTool( QAction*, const QString&, const int = -1, const int = -1 );//@}

  /** @name Create menu functions*/ //@{
  int                   createMenu( const QString&, const int, const int = -1, const int = -1, const int = -1 );
  int                   createMenu( const QString&, const QString&, const int = -1, const int = -1, const int = -1 );
  int                   createMenu( const int, const int, const int = -1, const int = -1 );
  int                   createMenu( const int, const QString&, const int = -1, const int = -1 );
  int                   createMenu( QAction*, const int, const int = -1, const int = -1, const int = -1 );
  int                   createMenu( QAction*, const QString&, const int = -1, const int = -1, const int = -1 );//@}

  /** @name Set menu shown functions*/ //@{
  void                  setMenuShown( QAction*, const bool );
  void                  setMenuShown( const int, const bool );//@}
  /** @name Set tool shown functions*/ //@{
  void                  setToolShown( QAction*, const bool );
  void                  setToolShown( const int, const bool );//@}

  void                  setActionShown( QAction*, const bool );
  void                  setActionShown( const int, const bool );

  static QAction*       separator();
  int                   actionId( const QAction* ) const;

  QList<QAction*>       actions() const;
  QList<int>            actionIds() const;

  int                   registerAction( const int, QAction* );
  QAction*              createAction( const int, const QString&, const QIcon&, const QString&,
                                      const QString&, const int, QObject* = 0,
                                      const bool = false, QObject* = 0, const char* = 0,
				      const QString& = QString() );
  QAction*              createAction( const int, const QString&, const QIcon&, const QString&,
                                      const QString&, const QKeySequence&, QObject* = 0,
                                      const bool = false, QObject* = 0, const char* = 0,
				      const QString& = QString() );

protected slots:
  virtual void          onDesktopActivated();
//  void                  onDesktopMoved();

private:
  SUIT_Study*           myStudy;
  SUIT_Desktop*         myDesktop;
  QMap<int, QAction*>   myActionMap;
  SUIT_ShortcutMgr*     myShortcutMgr;

  QLabel*               myStatusLabel;

  typedef void (*PostRoutine)();
  QList<PostRoutine>    myPostRoutines;
};

//! This function must return a new application instance.
extern "C"
{
  //jfa 22.06.2005:typedef SUIT_Application* (*APP_CREATE_FUNC)( int, char** );
  typedef SUIT_Application* (*APP_CREATE_FUNC)();
}

#define APP_CREATE_NAME "createApplication"

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
