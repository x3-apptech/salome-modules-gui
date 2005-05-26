#ifndef SALOMEAPP_WIDGETCONTAINER_H
#define SALOMEAPP_WIDGETCONTAINER_H

#include "SalomeApp.h"

#include <qdockwindow.h>

class QWidget;
class QWidgetStack;

class SALOMEAPP_EXPORT SalomeApp_WidgetContainer : public QDockWindow
{
    Q_OBJECT

public:
  SalomeApp_WidgetContainer( const int, QWidget* = 0 );
  virtual ~SalomeApp_WidgetContainer();

  bool         isEmpty() const;

  int          type() const;

  int          insert( const int, QWidget* );
  void         remove( QWidget* );
  void         remove( const int );
  bool         contains( const int ) const;

  void         activate( QWidget* );
  void         activate( const int );

  QWidget*     active() const;
  QWidget*     widget( const int ) const;

private:
  int            myType;
  QWidgetStack*  myStack;
};

#endif
