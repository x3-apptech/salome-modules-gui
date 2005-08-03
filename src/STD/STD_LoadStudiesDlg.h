#ifndef STD_LOADSTUDIESDLG_H
#define STD_LOADSTUDIESDLG_H

#include <qvariant.h>
#include <qdialog.h>
#include <STD.h>
class QVBoxLayout; 
class QHBoxLayout; 
class QGridLayout; 
class QLabel;
class QListBox;
class QListBoxItem;
class QPushButton;

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
