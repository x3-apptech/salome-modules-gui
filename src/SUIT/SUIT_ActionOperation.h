#ifndef SUIT_ACTIONOPERATION_H
#define SUIT_ACTIONOPERATION_H

#include "SUIT_Operation.h"

class QtxAction;

class SUIT_EXPORT SUIT_ActionOperation : public SUIT_Operation
{
	Q_OBJECT

public:
	SUIT_ActionOperation( SUIT_Application* theApp );
	virtual ~SUIT_ActionOperation();

	QtxAction*   action() const;

	/** @name GUI management*/ //@{
  virtual void setAction( const QString& text, const QIconSet& icon, const QString& menuText,
                          QKeySequence accel, QObject* parent, const char* name = 0, bool toggle = false );

  virtual void setAction( const QString& text, const QString& menuText, QKeySequence accel,
                          QObject* parent, const char* name = 0, bool toggle = false );
	virtual void setAction( QtxAction* theAction );
	
  bool         addTo( QWidget* theWidget );
  bool         addTo( QWidget* theWidget, int thePos );

  void         setStatusTip( const QString& theTip );//@}

private:
	QtxAction*   myAction;

	friend class SUIT_Study;
};

#endif
