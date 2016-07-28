// Copyright (C) 2015-2016  OPEN CASCADE
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
// File   : PyEditor_StdSettings.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

#ifndef PYEDITOR_STDSETTINGS_H
#define PYEDITOR_STDSETTINGS_H

#include "PyEditor.h"
#include "PyEditor_Settings.h"

#include <QSettings>

class PYEDITOR_EXPORT PyEditor_StdSettings : public PyEditor_Settings
{
public:
  PyEditor_StdSettings();
  PyEditor_StdSettings( const QString& );
  PyEditor_StdSettings( const QString&, const QString&, QSettings::Format = QSettings::IniFormat );

  void setLanguage( const QString& );
  QString language() const;

  void load();
  void save();

private:
  QSettings mySettings;
  QString myGroup;
  QString myLanguage;
};

#endif // PYEDITOR_STDSETTINGS_H
