// File:      QtxWorkstack.h
// Author:    Sergey TELKOV

#ifndef QTXWORKSTACK_H
#define QTXWORKSTACK_H

#include "Qtx.h"

#include <qhbox.h>
#include <qwidget.h>
#include <qtabbar.h>
#include <qwidgetlist.h>

class QAction;
class QTabBar;
class QPainter;
class QSplitter;
class QPushButton;
class QWidgetStack;

class QtxWorkstackArea;
class QtxWorkstackDrag;
class QtxWorkstackChild;
class QtxWorkstackTabBar;

class QTX_EXPORT QtxWorkstack : public QWidget
{
  Q_OBJECT
 
public:

  QtxWorkstack( QWidget* = 0 );
  virtual ~QtxWorkstack();

  QWidgetList         windowList() const;
  QWidgetList         splitWindowList() const;

  QWidget*            activeWindow() const;

  void                split( const int );

  enum { SplitVertical, SplitHorizontal, Close };

  // begin: jfa 06.07.2005
  enum SplitType {
    SPLIT_STAY, //!< given widget stays in its workarea, others are moved into a new one
    SPLIT_AT,   //!< widgets before a given widget stays in they workarea, others are moved into a new one
    SPLIT_MOVE  //!< given widget is moved into a new workarea, others stay in an old one
  };

  /*!
   * \brief Split workarea of the given widget on two parts.
   * \param wid  - widget, belonging to this workstack
   * \param o    - orientation of splitting (Qt::Horizontal or Qt::Vertical)
   * \param type - type of splitting, see <VAR>SplitType</VAR> enumeration
   */
  void Split( QWidget* wid, const Qt::Orientation o, const SplitType type);

  /*!
   * \brief Put given widget on top of its workarea
   * \param wid - widget, belonging to this workstack
   */
  void OnTop( QWidget* wid);

  /*!
   * \brief Move widget(s) from source workarea into target workarea
   *        or just reorder widgets inside one workarea.
   * \param wid1 - widget from target workarea
   * \param wid2 - widget from source workarea
   * \param all  - if this parameter is TRUE, all widgets from source workarea will
   *               be moved into the target one, else only the \a wid2 will be moved
   *
   * Move \a wid2 in target workarea. Put it right after \a wid1.
   * If value of boolean argument is TRUE, all widgets from source workarea
   * will be moved together with \a wid2, source workarea will be deleted.
   * If \a wid1 and \a wid2 belongs to one workarea, simple reordering will take place.
   */
  void Attract( QWidget* wid1, QWidget* wid2, const bool all );

  /*!
   * \brief Set position of the widget relatively its splitter.
   * \param wid - widget to set position of
   * \param pos - position relatively splitter. Value in range [0..1].
   *
   * Orientation of positioning will correspond to the splitter orientation.
   */
  void SetRelativePositionInSplitter( QWidget* wid, const double pos );

  /*!
   * \brief Set position of the widget relatively the entire workstack.
   * \param wid - widget to set position of
   * \param o   - orientation of positioning (Qt::Horizontal or Qt::Vertical).
   *              If o = Qt::Horizontal, horizontal position of \a wid will be changed.
   *              If o = Qt::Vertical, vertical position of \a wid will be changed.
   * \param pos - position relatively workstack. Value in range [0..1].
   */
  void SetRelativePosition( QWidget* wid, const Qt::Orientation o, const double pos );
  // end: jfa 06.07.2005

  /*!
   * \brief Sets the action's accelerator key to accel. 
   * \param id - the key of the action in the actions map.
   * \param accel - action's accelerator key.
   */
  void setAccel( const int id, const int accel );

  /*!
   * \brief Returns the action's accelerator key.
   * \param id - the key of the action in the actions map.
   * \retval int  - action's accelerator key.
   */
  int accel (const int id) const;

signals:
  void                windowActivated( QWidget* );

public slots:
  void                splitVertical();
  void                splitHorizontal();
  
private slots:
  void                onDestroyed( QObject* );
  void                onWindowActivated( QWidget* );
  void                onContextMenuRequested( QPoint );
  void                onDeactivated( QtxWorkstackArea* );
  /*!
   * \brief Closes the active window.
   */
  void                onCloseWindow();

protected:
  virtual void        childEvent( QChildEvent* );
  virtual void        customEvent( QCustomEvent* );

private:
  QSplitter*          splitter( QtxWorkstackArea* ) const;
  void                splitters( QSplitter*, QPtrList<QSplitter>&, const bool = false ) const;
  void                areas( QSplitter*, QPtrList<QtxWorkstackArea>&, const bool = false ) const;

  QSplitter*          wrapSplitter( QtxWorkstackArea* );
  void                insertWidget( QWidget*, QWidget*, QWidget* );

  QtxWorkstackArea*   areaAt( const QPoint& ) const;

  QtxWorkstackArea*   targetArea();
  QtxWorkstackArea*   activeArea() const;
  QtxWorkstackArea*   currentArea() const;

  void                setActiveArea( QtxWorkstackArea* );
  QtxWorkstackArea*   neighbourArea( QtxWorkstackArea* ) const;

  QtxWorkstackArea*   createArea( QWidget* ) const;

  void                updateState();
  void                updateState( QSplitter* );

  void                distributeSpace( QSplitter* ) const;

  // begin: jfa 06.07.2005
  /*!
   * \brief Set position of given widget.
   * \param wid          - widget to be moved
   * \param split        - currently processed splitter (goes from more common
   *                       to more particular splitter in recursion calls)
   * \param o            - orientation of positioning
   * \param need_pos     - required position of the given widget in pixels
   *                       (from top/left side of workstack area)
   * \param splitter_pos - position of the splitter \a split
   *                       (from top/left side of workstack area)
   * \retval int - returns difference between a required and a distinguished position.
   * 
   * Internal method. Recursively calls itself.
   * Is called from <VAR>SetRelativePosition</VAR> public method.
   */
  int                 setPosition( QWidget* wid, QSplitter* split, const Qt::Orientation o,
				   const int need_pos, const int splitter_pos);
  // end: jfa 06.07.2005

private:
  QWidget*            myWin;
  QtxWorkstackArea*   myArea;
  QSplitter*          mySplit;

  QMap<int, QAction*> myActionsMap; //!< The map of the actions. Allows to get the QAction object by the key.

  friend class QtxWorkstackArea;
  friend class QtxWorkstackDrag;
};

class QtxWorkstackArea : public QWidget
{
  Q_OBJECT

public:
  QtxWorkstackArea( QWidget* );
  virtual ~QtxWorkstackArea();

  bool                isEmpty() const;

  void                insertWidget( QWidget*, const int = -1 );
  void                removeWidget( QWidget* );

  QWidget*            activeWidget() const;
  void                setActiveWidget( QWidget* );

  bool                contains( QWidget* ) const;

  QWidgetList         widgetList() const;

  bool                isActive() const;
  void                updateActiveState();

  QtxWorkstack*       workstack() const;

  virtual bool        eventFilter( QObject*, QEvent* );

  QRect               floatRect() const;
  QRect               floatTab( const int ) const;

  int                 tabAt( const QPoint& ) const;

signals:
  void                activated( QWidget* );
  void                contextMenuRequested( QPoint );
  void                deactivated( QtxWorkstackArea* );

public slots:
  virtual void        show();
  virtual void        hide();

private slots:
  void                onClose();
  void                onSelected( int );

  void                onDragActiveTab();
  void                onChildDestroyed( QObject* );
  void                onChildShown( QtxWorkstackChild* );
  void                onChildHided( QtxWorkstackChild* );
  void                onChildActivated( QtxWorkstackChild* );
  void                onChildCaptionChanged( QtxWorkstackChild* );

protected:
  virtual void        customEvent( QCustomEvent* );
  virtual void        focusInEvent( QFocusEvent* );
  virtual void        mousePressEvent( QMouseEvent* );

private:
  enum { ActivateWidget = QEvent::User, FocusWidget, RemoveWidget };

private:
  void                updateState();
  void                updateCurrent();
  void                updateTab( QWidget* );

  QWidget*            widget( const int ) const;
  int                 widgetId( QWidget* ) const;
  bool                widgetVisibility( QWidget* ) const;

  void                setWidgetActive( QWidget* );
  void                setWidgetShown( QWidget*, const bool );

  int                 generateId() const;

  bool                isBlocked( QWidget* ) const;
  void                setBlocked( QWidget*, const bool );

  QtxWorkstackChild*  child( QWidget* ) const;

private:
  typedef QMap<QWidget*, bool>               BlockMap;
  typedef QMap<QWidget*, QtxWorkstackChild*> ChildMap;
  typedef struct { int id; bool vis; }       WidgetInfo;
  typedef QMap<QWidget*, WidgetInfo>         WidgetInfoMap;

private:
  QtxWorkstackTabBar* myBar;
  QPushButton*        myClose;
  QWidgetStack*       myStack;

  QWidgetList         myList;
  WidgetInfoMap       myInfo;
  ChildMap            myChild;
  BlockMap            myBlock;
};

class QtxWorkstackChild : public QHBox
{
  Q_OBJECT

public:
  QtxWorkstackChild( QWidget*, QWidget* = 0 );
  virtual ~QtxWorkstackChild();

  QWidget*            widget() const;

  virtual bool        eventFilter( QObject*, QEvent* );

signals:
  void                shown( QtxWorkstackChild* );
  void                hided( QtxWorkstackChild* );
  void                activated( QtxWorkstackChild* );
  void                captionChanged( QtxWorkstackChild* );

private slots:
  void                onDestroyed( QObject* );

protected:
  virtual void        childEvent( QChildEvent* );

private:
  QWidget*            myWidget;
};

class QtxWorkstackTabBar : public QTabBar
{
  Q_OBJECT

public:
  QtxWorkstackTabBar( QWidget* = 0 );
  virtual ~QtxWorkstackTabBar();

  QRect               tabRect( const int ) const;

  void                setActive( const bool );

signals:
  void                dragActiveTab();
  void                contextMenuRequested( QPoint );

protected:
  virtual void        mouseMoveEvent( QMouseEvent* );
  virtual void        mousePressEvent( QMouseEvent* );
  virtual void        mouseReleaseEvent( QMouseEvent* );
  virtual void        contextMenuEvent( QContextMenuEvent* );

  virtual void        paintLabel( QPainter*, const QRect&, QTab*, bool ) const;

private:
  int                 myId;
};

class QtxWorkstackDrag : public QObject
{
  Q_OBJECT

public:
  QtxWorkstackDrag( QtxWorkstack*, QtxWorkstackChild* );
  virtual ~QtxWorkstackDrag();

  virtual bool        eventFilter( QObject*, QEvent* );

private:
  void                dropWidget();

  void                updateTarget( const QPoint& );
  QtxWorkstackArea*   detectTarget( const QPoint&, int& ) const;
  void                setTarget( QtxWorkstackArea*, const int );

  void                drawRect();
  void                endDrawRect();
  void                startDrawRect();

private:
  QtxWorkstack*       myWS;
  QtxWorkstackChild*  myChild;

  int                 myTab;
  QtxWorkstackArea*   myArea;
  QPainter*           myPainter;
  
};

#endif
