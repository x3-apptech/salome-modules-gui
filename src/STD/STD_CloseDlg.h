#ifndef STD_CloseDlg_H
#define STD_CloseDlg_H

#include <qdialog.h> 

/*!
 * \brief 
 *
 * 
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

