#ifndef STD_TABDESKTOP_H
#define STD_TABDESKTOP_H

#include "STD.h"

#include <SUIT_Desktop.h>

class QtxAction;
class QPopupMenu;
class QWorkspace;
class QtxWorkstack;
class QtxWorkstackAction;

#if defined WNT
#pragma warning( disable: 4251 )
#endif

class STD_EXPORT STD_TabDesktop: public SUIT_Desktop 
{
  Q_OBJECT

public:
  enum { VSplit, HSplit };

public:
  STD_TabDesktop();
  virtual ~STD_TabDesktop();

  virtual SUIT_ViewWindow* activeWindow() const;
  virtual QPtrList<SUIT_ViewWindow> windows() const;

  void                     windowOperation( const int );

  void                     setWindowOperations( const int, ... );
  void                     setWindowOperations( const QValueList<int>& );

  QtxWorkstack*            workstack() const;

private slots:
  void                     onWindowActivated( QWidget* );

protected:
  void                     createActions();
  virtual QWidget*         parentArea() const;

private:
  int                      operationFlag( const int ) const;

private:
  QtxWorkstack*            myWorkstack;
  QtxWorkstackAction*      myWorkstackAction;
};

#if defined WNT
#pragma warning( default: 4251 )
#endif

#endif
