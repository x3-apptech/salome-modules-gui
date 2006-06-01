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
#ifndef STD_LOADSTUDIESDLG_H
#define STD_LOADSTUDIESDLG_H

#include <STD.h>

#include <qdialog.h>
#include <qvariant.h>

class QLabel;
class QListBox;
class QPushButton;
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QListBoxItem;

/*!\class STD_LoadStudiesDlg
 * \brief Describes a dialog box that gives a list of opened studies.
 * 
 */
class STD_EXPORT STD_LoadStudiesDlg : public QDialog
{ 
   Q_OBJECT

public:
   STD_LoadStudiesDlg( QWidget* parent = 0, bool modal = FALSE, WFlags fl = 0 );
   ~STD_LoadStudiesDlg() {}

  /*!\var TextLabel1
   * \brief stores a dialog text label
   */
  QLabel* TextLabel1;
  
  /*!\var buttonOk
   * \brief stores a dialog button OK
   */
  QPushButton* buttonOk;

  /*!\var buttonCancel
   * \brief stores a dialog button Cancel
   */  
  QPushButton* buttonCancel;

  /*!\var ListComponent
   * \brief stores a dialog list compoent
   */ 
   QListBox* ListComponent;

};

#endif // STD_LOADSTUDIESDLG_H
