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
#include "SalomeApp_CheckFileDlg.h"

#include <qcheckbox.h>
#include <qlabel.h>
#include <qpushbutton.h>

/*!
Constructor
*/
SalomeApp_CheckFileDlg::SalomeApp_CheckFileDlg( QWidget* parent, bool open, const QString& theCheckBoxName, bool showQuickDir, bool modal) :
SUIT_FileDlg( parent, open, showQuickDir, modal )
{    
  myCheckBox = new QCheckBox( theCheckBoxName, this );
  QLabel* label = new QLabel("", this);
  QPushButton* pb = new QPushButton(this);               
  addWidgets( label, myCheckBox, pb );
  pb->hide();
}

/*!
Destructor
*/
SalomeApp_CheckFileDlg::~SalomeApp_CheckFileDlg() 
{

}

/*!Sets checked.*/
void SalomeApp_CheckFileDlg::SetChecked( bool check )
{
  myCheckBox->setChecked(check);
}

/*!Is checked?
 *\retval boolean - true, check box is checked, else false.
 */
bool SalomeApp_CheckFileDlg::IsChecked() const
{
  return myCheckBox->isChecked();
}
