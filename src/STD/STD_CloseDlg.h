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
#ifndef STD_CloseDlg_H
#define STD_CloseDlg_H

#include <qdialog.h> 

/*! \class QDialog
 * \brief For more information see <a href="http://doc.trolltech.com">QT documentation</a>.
 */
/*!\class STD_CloseDlg
 * \brief Describes a dialog box shown on closing the active study
 */
class STD_CloseDlg: public QDialog
{
  Q_OBJECT

public:
  STD_CloseDlg ( QWidget * parent = 0, bool modal = FALSE, WFlags f = 0 ) ;
  ~STD_CloseDlg ( ) { };

private slots:
  void onButtonClicked();

private:
  /*!\var m_pb1
   * \brief Private, stores a dialog button 1
   */
  QPushButton* m_pb1; 
  /*!\var m_pb2
   * \brief Private, stores a dialog button 2
   */
  QPushButton* m_pb2;
  /*!\var m_pb3
   * \brief Private, stores a dialog button 3
   */
  QPushButton* m_pb3;

  /*!\var m_pb4
   * \brief Private, stores a dialog button 4
   */
  QPushButton* m_pb4;
};

#endif

