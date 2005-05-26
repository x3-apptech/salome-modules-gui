#ifndef SUIT_POPUPCLIENT_H
#define SUIT_POPUPCLIENT_H

#include "SUIT.h"

#include <qobject.h>

class QPopupMenu;
class QContextMenuEvent;

/*
  Class: SUIT_PopupClient [public]
  Descr: Base class for instances which creates popup menu on QContextMenuEvent
*/

class SUIT_EXPORT SUIT_PopupClient
{
public:

  class Signal;

  SUIT_PopupClient();
  ~SUIT_PopupClient();

  bool            connectPopupRequest( QObject* reciever, const char* slot );
  bool            disconnectPopupRequest( QObject* reciever, const char* slot );

  virtual QString popupClientType() const = 0;
  virtual void    contextMenuPopup( QPopupMenu* ) {}

protected:
  void    contextMenuRequest( QContextMenuEvent* e );

private:
  Signal* mySignal;
};

/*
  Class: SUIT_PopupClient::Signal [internal]
  Descr: invoke signal which is connected to reciever in SUIT_PopupClient
*/

class SUIT_PopupClient::Signal : public QObject
{
  Q_OBJECT

public:
  Signal();
  virtual ~Signal();

  void    sendSignal( SUIT_PopupClient*, QContextMenuEvent* );

signals:
  void contextMenuRequest( SUIT_PopupClient*, QContextMenuEvent* );
};

#endif
