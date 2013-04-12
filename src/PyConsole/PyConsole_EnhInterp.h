// Copyright (C) 2007-2013  CEA/DEN, EDF R&D
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
// Author : Adrien Bruneton (CEA/DEN)
// Created on: 4 avr. 2013


#ifndef PYCONSOLE_ENHINTERP_H_
#define PYCONSOLE_ENHINTERP_H_

#include "PyConsole.h"

#include <Python.h>
#include "PyConsole_Interp.h"

#include <vector>
#include <QString>

/**
 * Enhanced Python interpreter used for auto-completion.
 * This extends PyConsole_Interp with an API wrapping the Python dir() command nicely.
 */
class PYCONSOLE_EXPORT PyConsole_EnhInterp: public PyConsole_Interp
{
public:
  PyConsole_EnhInterp()
    : PyConsole_Interp(), _last_matches(0), _doc_str("")
    {}

  virtual ~PyConsole_EnhInterp() {}

  const std::vector<QString>& getLastMatches() const { return _last_matches; }
  const QString & getDocStr() const                  { return _doc_str; }

  virtual int runDirCommand(const QString& dirArgument, const QString& startMatch);
  virtual void clearCompletion();

protected:
  /** Hard coded list of Python keywords */
  static const std::vector<QString> PYTHON_KEYWORDS;

  /** Last computed matches */
  std::vector<QString> _last_matches;
  /** Doc string of the first match - when only one match it will be displayed by the Editor*/
  QString _doc_str;

  virtual int runDirAndExtract(const QString& dirArgument, const QString & startMatch,
      std::vector<QString> & result, bool discardSwig=true) const;

};

#endif /* PYCONSOLE_ENHINTERP_H_ */
