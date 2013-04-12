// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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
//  Author : Vadim SANDLER (Open CASCADE S.A.S), Adrien Bruneton (CEA/DEN)

#ifndef PYCONSOLE_EVENT_H
#define PYCONSOLE_EVENT_H

#include "PyConsole.h"

#include <QEvent>
#include <QString>

/*!
  \class PrintEvent
  \brief Python command output backend event.
  \internal
*/
class PrintEvent : public QEvent
{
public:
  static const int EVENT_ID = 65432;

  /*!
    \brief Constructor
    \param c message text (python trace)
    \param isError default to false - if true indicates that an error is being printed.
  */
  PrintEvent( const char* c, bool isError = false) :
    QEvent( (QEvent::Type)EVENT_ID ), myText( c ), errorFlag(isError)
  {}

  /*!
    \brief Get message
    \return message text (python trace)
  */
  QString text() const { return myText; }

  /**
   * @return true if this is an error message
   */
  bool isError() const { return errorFlag; }

protected:
  QString myText; //!< Event message (python trace)

  /** Set to true if an error msg is to be displayed */
  bool errorFlag;
};

#endif // PYCONSOLE_EVENT_H
