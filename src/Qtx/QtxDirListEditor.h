#ifndef QTX_DIRLISTEDITOR_H
#define QTX_DIRLISTEDITOR_H

#include "Qtx.h"

#include <qwidget.h>
#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qlistbox.h>
#include <qcombobox.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!
 * \brief The GUI implementation of the directory list
 */
class QTX_EXPORT QtxDirListEditor : public QWidget {

  Q_OBJECT

public:

  /*!
   * \brief Constructor
   * \param parent - the parent of the widget
   */
  QtxDirListEditor(QWidget* parent);

  /*!
   * \brief Destructor
   */
  ~QtxDirListEditor();

  /*!
   * \brief Gets list of paths
   * \param list - the returned reference to the list of paths
   */
  void          getPathList(QStringList& list);

  /*!
   * \brief Sets list of paths
   * \param list - the list of paths to set
   */
  void          setPathList(const QStringList& list);

  /*!
   * \brief Event filter, redefined from QObject class
   */
  bool          eventFilter(QObject* object, QEvent* event);

protected:

  /*!
   * \brief Validates entered path
   * \retval bool - returns status (true if OK)
   */
  bool          validate();

  /*!
   * \brief Appends/changes path
   * \param item - the item in QListBox
   * \param dir - the path
   */
  void          appendDir(QListBoxItem* item, const QString& dir);

  /*!
   * \brief Removes directory from list
   * \param item - the item in QListBox
   */
  void          removeDir(QListBoxItem* item);

  /*!
   * \brief Resize event handler, reimplemented from QWidget
   * \param event - the resize event
   */
  void          resizeEvent(QResizeEvent* event);

protected slots:

  /*!
   * \brief Called when user clicks inside directories list box
   */
  void          onMouseButtonClicked(int, QListBoxItem*, const QPoint&);

  /*!
   * \brief Called when user double-clicks on any item
   */
  void          onDblClicked(QListBoxItem*);

  /*!
   * \brief <...> (Browse dir) button slot
   */
  void          onBtnClicked();

  /*!
   * \brief Ccalled when user finises editing of path by pressing <Enter>
   */
  void          onEditFinished();

  /*!
   * \brief <Insert> button slot
   */
  void          onInsert();

  /*!
   * \brief  <Delete> button slot
   */
  void          onDelete();

  /*!
   * \brief  <Move up> button slot
   */
  void          onUp();

  /*!
   * \brief  <Move down> button slot
   */
  void          onDown();

private:
  QListBox*     myDirList;          //!< directory list
  QLineEdit*    myEdit;             //!< path edit box
  QToolButton*  myBtn;              //!<  browse pah button
  bool          myEdited;           //!<  edit mode flag
  QListBoxItem* myLastSelected;     //!< last selected row

};

#endif
