// File:      QtxToolBar.h
// Author:    Sergey TELKOV

#include "Qtx.h"

#include <qtoolbar.h>

class QTX_EXPORT QtxToolBar : public QToolBar
{
  Q_OBJECT

  class Watcher;

public:
  QtxToolBar( const bool, const QString&, QMainWindow*, QWidget*, bool = false, const char* = 0, WFlags = 0 );
  QtxToolBar( const QString&, QMainWindow*, QWidget*, bool = false, const char* = 0, WFlags = 0 );
  QtxToolBar( const bool, QMainWindow* = 0, const char* = 0 );
  QtxToolBar( QMainWindow* = 0, const char* = 0 );
  virtual ~QtxToolBar();

  virtual void  setWidget( QWidget* );

  bool          isStretchable() const;
  virtual void  setStretchable( const bool );

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

public slots:
  virtual void  show();
  virtual void  hide();

private:
  Watcher*      myWatcher;
  bool          myStretch;
};
