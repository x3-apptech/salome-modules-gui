#ifndef LIGHTAPP_WIDGETCONTAINER_H
#define LIGHTAPP_WIDGETCONTAINER_H

#include "LightApp.h"

#include <qdockwindow.h>

class QWidget;
class QWidgetStack;

/*!
  Class which privade widget container.
*/
class LIGHTAPP_EXPORT LightApp_WidgetContainer : public QDockWindow
{
    Q_OBJECT

public:
  LightApp_WidgetContainer( const int, QWidget* = 0 );
  virtual ~LightApp_WidgetContainer();

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
