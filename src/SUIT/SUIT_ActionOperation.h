// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#ifndef SUIT_ACTIONOPERATION_H
#define SUIT_ACTIONOPERATION_H

#include "SUIT_Operation.h"

class QtxAction;
class QString;
class QIconSet;
class QKeySequence;
class QWidget;

/*!Provide support QtxAction*/
class SUIT_EXPORT SUIT_ActionOperation : public SUIT_Operation
{
	Q_OBJECT

public:
	SUIT_ActionOperation( SUIT_Application* theApp );
	virtual ~SUIT_ActionOperation();

	QtxAction*   action() const;

	/** @name GUI management*/
	//@{
	virtual void setAction( const QString& text, const QIconSet& icon, const QString& menuText,
				QKeySequence accel, QObject* parent, const char* name = 0, bool toggle = false );

	virtual void setAction( const QString& text, const QString& menuText, QKeySequence accel,
				QObject* parent, const char* name = 0, bool toggle = false );
	virtual void setAction( QtxAction* theAction );
	
	bool         addTo( QWidget* theWidget );
	bool         addTo( QWidget* theWidget, int thePos );
	
	void         setStatusTip( const QString& theTip );
	//@}
	
private:
	QtxAction*   myAction;

	friend class SUIT_Study;
};

#endif
