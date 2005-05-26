#ifndef SUITAPP_APPLICATION_H
#define SUITAPP_APPLICATION_H

#include <qapplication.h>

class SUIT_ExceptionHandler;

class SUITApp_Application : public QApplication
{
  Q_OBJECT

public:
  SUITApp_Application( int& argc, char** argv, SUIT_ExceptionHandler* = 0 );
  SUITApp_Application( int& argc, char** argv, Type type, SUIT_ExceptionHandler* = 0 );

  virtual bool notify( QObject* receiver, QEvent* e );

	SUIT_ExceptionHandler* handler() const;
	void                   setHandler( SUIT_ExceptionHandler* );

private:
  SUIT_ExceptionHandler* myExceptHandler;
};

#endif
