#ifndef STD_MDIDESKTOP_H
#define STD_MDIDESKTOP_H

#include "STD.h"

#include <SUIT_Desktop.h>

class QtxAction;
class QPopupMenu;
class QWorkspace;
class QtxWorkstack;
//class QtxWorkspaceAction;

#if defined WNT
#pragma warning( disable: 4251 )
#endif

class STD_EXPORT STD_MDIDesktop: public SUIT_Desktop 
{
  Q_OBJECT

public:
  enum { Cascade, Tile, HTile, VTile };

public:
  STD_MDIDesktop();
  virtual ~STD_MDIDesktop();

  virtual SUIT_ViewWindow* activeWindow() const;
  virtual QPtrList<SUIT_ViewWindow> windows() const;

  void                     windowOperation( const int );

  void                     setWindowOperations( const int, ... );
  void                     setWindowOperations( const QValueList<int>& );

  QWorkspace*              workspace() const;

private slots:
  void                     onWindowActivated( QWidget* );

protected:
  void                     createActions();
  virtual QWidget*         parentArea() const;

private:
  int                      operationFlag( const int ) const;

private:
  QtxWorkstack*            myWorkstack;
//  QtxWorkspaceAction*      myWorkspaceAction;
};

#if defined WNT
#pragma warning( default: 4251 )
#endif

#endif
