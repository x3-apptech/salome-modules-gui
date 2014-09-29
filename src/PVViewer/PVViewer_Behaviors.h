// Copyright (C) 2010-2014  CEA/DEN, EDF R&D
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
// Author: Adrien Bruneton (CEA)


#ifndef PVGUIBEHAVIORS_H_
#define PVGUIBEHAVIORS_H_

#include <QObject>

class SalomeApp_Module;
class SUIT_Desktop;
class pqPropertiesPanel;

/**!
 * PARAVIS behaviors - mimic what is done in
 *    Qt/ApplicationComponents/pqParaViewBehaviors.cxx
 * Except a few ones, behaviors are destroyed when the module is destroyed.
 */
class PVViewer_Behaviors: public QObject
{
  Q_OBJECT

public:
  PVViewer_Behaviors(SUIT_Desktop * parent);

  void instanciateMinimalBehaviors(SUIT_Desktop * desk);
  void instanciateAllBehaviors(SUIT_Desktop * desk);

  virtual ~PVViewer_Behaviors() {}

//public slots:
//  void onEmulateApply();

private:
  static int BehaviorLoadingLevel;
};

#endif /* PVGUIBEHAVIORS_H_ */
