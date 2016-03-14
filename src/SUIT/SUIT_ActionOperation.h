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

#ifndef SUIT_ACTIONOPERATION_H
#define SUIT_ACTIONOPERATION_H

#include "SUIT_Operation.h"

#include <QKeySequence>

class QIcon;
class QString;
class QWidget;

class QtxAction;

/*!Provide support QtxAction*/
class SUIT_EXPORT SUIT_ActionOperation : public SUIT_Operation
{
        Q_OBJECT

public:
        SUIT_ActionOperation( SUIT_Application* );
        virtual ~SUIT_ActionOperation();

        QtxAction*   action() const;

        /** @name GUI management*/
        //@{
        virtual void setAction( const QString& text, const QIcon& icon, const QString& menuText,
                                                  QKeySequence accel, QObject* parent, bool toggle = false );

        virtual void setAction( const QString& text, const QString& menuText,
                          QKeySequence accel, QObject* parent, bool toggle = false );
        virtual void setAction( QtxAction* theAction );
        
        bool         addTo( QWidget* theWidget );

        void         setStatusTip( const QString& theTip );
        //@}

private:
        QtxAction*   myAction;

        friend class SUIT_Study;
};

#endif
