// Copyright (C) 2007-2019  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include "QtxNotify.h"

#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QTextDocument>
#include <QDesktopWidget>
#include <QSharedPointer>
#include <QPropertyAnimation>
#include <QParallelAnimationGroup>

/*!
    \brief QtxNotify::NotifyWidget
    Class represented notification widget
*/

class QtxNotify::NotifyWidget : public QWidget
{
    typedef QSharedPointer<QTextDocument> TextDocument;

public:
    NotifyWidget(QtxNotify*, const QString& title,
                 const QString& text, const int timeout, QWidget* parent = 0);
    virtual ~NotifyWidget();

    QtxNotify*             notifyMgr() const;

    int                    id() const;
    QString                text() const;
    QString                title() const;
    int                    timeout() const;

    void                   setId(const int);
    void                   setText(const QString&);
    void                   setTitle(const QString&);
    void                   setTimeout(const int);

    virtual QSize          sizeHint() const;
    virtual QSize          minimumSizeHint() const;

protected:
    virtual void           showEvent(QShowEvent*);
    virtual void           paintEvent(QPaintEvent*);
    virtual void           mouseMoveEvent(QMouseEvent*);
    virtual void           mousePressEvent(QMouseEvent*);
    virtual void           mouseReleaseEvent(QMouseEvent*);

private:
    int                    frameWidth() const;
    int                    notificationWidth() const;

    QRect                  textRect() const;
    QRect                  titleRect() const;
    QRect                  closeRect() const;
    TextDocument           textDocument() const;

    void                   onTimeout();

private:
    int                    myId;
    QString                myText;
    QString                myTitle;
    QTimer*                myTimer;
    QtxNotify*             myNotifyMgr;

    bool                   myIsClosePressed;
};

/*!
    \brief Constructor [private]
*/
QtxNotify::NotifyWidget::NotifyWidget(QtxNotify* mgr, const QString& title, const QString& text,
                                      const int timeout, QWidget* parent)
    : QWidget(parent, (parent ? Qt::Widget : Qt::Tool) | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint),
      myTitle(title),
      myText(text),
      myTimer(new QTimer(this)),
      myNotifyMgr(mgr),
      myIsClosePressed(false)
{
    myTimer->setSingleShot(true);

    connect(myTimer, &QTimer::timeout, this, &QtxNotify::NotifyWidget::onTimeout);

    setTimeout(timeout);
    setMouseTracking(true);
}

/*!
    \brief Destructor
*/
QtxNotify::NotifyWidget::~NotifyWidget()
{
}

/*!
    \brief Gets the notification manager.
    \return notification manager instance
*/
QtxNotify* QtxNotify::NotifyWidget::notifyMgr() const
{
    return myNotifyMgr;
}

/*!
    \brief Gets the notification identifier.
    \return notification identifier
*/
int QtxNotify::NotifyWidget::id() const
{
    return myId;
}

/*!
    \brief Gets the notification text.
    \return notification text string
*/
QString QtxNotify::NotifyWidget::text() const
{
    return myText;
}

/*!
    \brief Gets the notification title.
    \return notification title string
*/
QString QtxNotify::NotifyWidget::title() const
{
    return myTitle;
}

/*!
    \brief Gets the notification timeout.
    \return notification timeout
*/
int QtxNotify::NotifyWidget::timeout() const
{
    return myTimer->interval();
}

/*!
    \brief Sets the notification timeout.
    \param id - notification identifier
*/
void QtxNotify::NotifyWidget::setId(const int id)
{
    myId = id;
}

/*!
    \brief Sets the notification text.
    \param text - notification text string
*/
void QtxNotify::NotifyWidget::setText(const QString& text)
{
    myText = text;
    updateGeometry();
    update();
}

/*!
    \brief Sets the notification title.
    \param title - notification title string
*/
void QtxNotify::NotifyWidget::setTitle(const QString& title)
{
    myTitle = title;
    update();
}

/*!
    \brief Sets the notification timeout.
    \param timeout - notification timeout
*/
void QtxNotify::NotifyWidget::setTimeout(const int timeout)
{
    if (myTimer->isActive())
        myTimer->stop();
    myTimer->setInterval(timeout);

    if (isVisible() && myTimer->interval() > 0)
        myTimer->start();
}

/*!
    \brief Gets the notification widget size hint.
    \return size hint
*/
QSize QtxNotify::NotifyWidget::sizeHint() const
{
    return minimumSizeHint();
}

/*!
    \brief Gets the notification widget minimum size hint.
    \return minimum size hint
*/
QSize QtxNotify::NotifyWidget::minimumSizeHint() const
{
    int frame = frameWidth();
    int width = notificationWidth();
    TextDocument doc = textDocument();
    int height = (int)doc->size().height() + 2 * frame;

    height += fontMetrics().height() + 2 * frame;

    return QSize(width, height);
}

void QtxNotify::NotifyWidget::showEvent(QShowEvent* e)
{
  QWidget::showEvent(e);

  if (!myTimer->isActive() && myTimer->interval() > 0)
    myTimer->start();
}

/*!
    \brief Reimplemented for notification drawing.
*/
void QtxNotify::NotifyWidget::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    QRect clsRect = closeRect();
    QRect ttlRect = titleRect();
    QRect txtRect = textRect();

    // Fills background and frame
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.fillRect(QRect(QPoint(0,0), size()), Qt::darkGray);
    painter.fillRect(txtRect, Qt::white);

    // Draw title
    painter.save();

    painter.setPen(Qt::white);
    QFont fnt = font();
    fnt.setBold(true);
    painter.setFont(fnt);
    ttlRect.setRight(clsRect.left());

    QString titleText = title().trimmed();
    if (QFontMetrics(painter.font()).width(titleText) > ttlRect.width())
        titleText = QFontMetrics(painter.font()).elidedText(titleText, Qt::ElideRight, ttlRect.width() - 5);
    painter.drawText(ttlRect.adjusted(2, 0, 0, 0), Qt::AlignVCenter, titleText);

    painter.restore();

    // Draw close button
    painter.save();

    if (myIsClosePressed && clsRect.contains(mapFromGlobal(QCursor::pos())))
    {
        painter.fillRect(clsRect.adjusted(0, 0, -1, -1), Qt::gray);
        clsRect = clsRect.adjusted(1, 1, 0, 0);
    }
    else
    {
        painter.fillRect(clsRect.adjusted(1, 1, 0, 0), Qt::lightGray);
        clsRect = clsRect.adjusted(0, 0, -1, -1);
    }
    painter.fillRect(clsRect, Qt::red);

    int m = 2;
    painter.setPen(Qt::white);
    QRect rect = clsRect.adjusted(m, m, -m, -m);
    painter.drawLine(rect.topLeft(), rect.bottomRight());
    painter.drawLine(rect.topRight(), rect.bottomLeft());

    painter.restore();

    // Draw text
    painter.save();

    painter.setPen(Qt::black);
    TextDocument doc = textDocument();
    painter.translate(txtRect.topLeft());
    doc->drawContents(&painter, QRectF(QPointF(0, 0), txtRect.size()));

    painter.restore();

    painter.end();
}

/*!
    \brief Reimplemented for handling close button.
*/
void QtxNotify::NotifyWidget::mouseMoveEvent(QMouseEvent*)
{
    if (myIsClosePressed)
        update();
}

/*!
    \brief Reimplemented for handling close button.
*/
void QtxNotify::NotifyWidget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton && closeRect().contains(e->pos()))
    {
        myIsClosePressed = true;
        update();
    }
}

/*!
    \brief Reimplemented for handling close button.
*/
void QtxNotify::NotifyWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        if (myIsClosePressed && closeRect().contains(e->pos()))
        {
            notifyMgr()->hideNotification(id());
        }
        myIsClosePressed = false;
        update();
    }
}

/*!
    \brief Gets the notification frame with
    \return Frame width in pixels
*/
int QtxNotify::NotifyWidget::frameWidth() const
{
    return 2;
}

/*!
    \brief Gets the notification window with
    \return width in pixels
*/
int QtxNotify::NotifyWidget::notificationWidth() const
{
    return notifyMgr()->notificationWidth();
}

/*!
    \brief Gets the text document with actual text and size.
    \return text document instance
*/
QtxNotify::NotifyWidget::TextDocument QtxNotify::NotifyWidget::textDocument() const
{
    TextDocument doc(new QTextDocument());
    if (Qt::mightBeRichText(text()))
        doc->setHtml(text());
    else
        doc->setPlainText(text());
    doc->setTextWidth(notificationWidth() - 2 * frameWidth());
    return doc;
}

/*!
    \brief Gets the notification message text rectangle.
    \return text rectangle
*/
QRect QtxNotify::NotifyWidget::textRect() const
{
    int frame = frameWidth();
    return QRect(frame, 3 * frame + fontMetrics().height(),
                 width() - 2 * frame, height() - 4 * frame - fontMetrics().height());
}

/*!
    \brief Gets the notification title rectangle
    \return title rectangle
*/
QRect QtxNotify::NotifyWidget::titleRect() const
{
    int frame = frameWidth();
    return QRect(frame, frame, width() - 2 * frame, fontMetrics().height() + 2 * frame);
}

/*!
    \brief Gets the notification close button rectangle.
    \return close rectangle
*/
QRect QtxNotify::NotifyWidget::closeRect() const
{
    int frame = frameWidth();
    int size = fontMetrics().height();
    return QRect(width() - 2 * frame - size, 2 * frame, size, size);
}

/*!
    \brief Invoked when notification close timeout and hide the notification.
*/
void QtxNotify::NotifyWidget::onTimeout()
{
    notifyMgr()->hideNotification(id());
}

/*!
    \brief QtxNotify
    Class that manages all notification instances. Performs show, hide, placing, etc
*/

/*!
    \brief Constructor.
*/
QtxNotify::QtxNotify(QObject* parent)
    : QObject(parent),
    mySize(250),
    myWindow(0),
    myPlacement(TopToBottom),
    myAlignment(Qt::AlignRight|Qt::AlignTop),
    myAnimTime(0),
    myAnimBlock(false)
{
    myArrangeTimer = new QTimer(this);
    myArrangeTimer->setInterval(0);
    myArrangeTimer->setSingleShot(true);
    connect(myArrangeTimer, SIGNAL(timeout()), this, SLOT(onArrangeTimeout()));

    QWidget* window = 0;
    QWidgetList windows = QApplication::topLevelWidgets();
    for ( QWidgetList::iterator it = windows.begin(); it != windows.end() && !window; ++it )
    {
        QWidget* win = *it;
        if (win->isVisible() &&
            (win->windowType() == Qt::Widget || win->windowType() == Qt::Window))
            window = win;
    }

    setWindow(window ? window : QApplication::desktop());
}

/*!
    \brief Constructor.
*/
QtxNotify::QtxNotify(QWidget* win, QObject* parent)
    : QObject(parent),
    mySize(250),
    myWindow(0),
    myPlacement(TopToBottom),
    myAlignment(Qt::AlignRight|Qt::AlignTop),
    myAnimTime(0),
    myAnimBlock(false)
{
    myArrangeTimer = new QTimer(this);
    myArrangeTimer->setInterval(0);
    myArrangeTimer->setSingleShot(true);
    connect(myArrangeTimer, SIGNAL(timeout()), this, SLOT(onArrangeTimeout()));

    setWindow(win);
}

/*!
    \brief Destructor.
*/
QtxNotify::~QtxNotify()
{
}

/*!
    \brief Shows the notifications with spectified text, title and automatic close timeout.
    Notification will be automatically closed after specified timeout in msec. If
    timeout is zero then automatic closing doesn't performed.
    \param text - Notification text
    \param title - Notification title
    \param timeout - Notification close timeout in msec
    \return notification identifier
*/
int QtxNotify::showNotification(const QString& text, const QString& title, int timeout)
{
    QtxNotify::NotifyWidget* notify = notification(text);
    if (!notify)
    {
        notify = new QtxNotify::NotifyWidget(this, title, text, timeout, window());
        notify->setId(generateId());
    }
    else
    {
        myNotifications.removeAll(notify);
        notify->setTimeout(timeout);
        notify->setTitle(title);
        notify->hide();
    }

    myNotifications.append(notify);
    triggerArrange();

    return notify->id();
}

/*!
    \brief Closes the notifications with spectified text.
    \param text - Notification text
*/
void QtxNotify::hideNotification(const QString& text)
{
    removeNotification(notification(text));
}

/*!
    \brief Closes the notifications with spectified identifier.
    \param text - Notification identifier
*/
void QtxNotify::hideNotification(const int id)
{
    removeNotification(notification(id));
}

/*!
    \brief Gets the reference widget for all notifications.
    \return reference widget
*/
QWidget* QtxNotify::window() const
{
    return myWindow;
}

/*!
    \brief Sets the reference widget for all notifications.
    \param win - reference widget
*/
void QtxNotify::setWindow(QWidget* window)
{
    if (myWindow != window)
    {
        if (myWindow)
            myWindow->removeEventFilter(this);
        myWindow = window;
        if (myWindow)
            myWindow->installEventFilter(this);
        updateArrangement();
    }
}

/*!
    \brief Gets the animation time in msec for notification modifications.
    \return animation time
*/
int QtxNotify::animationTime() const
{
    return myAnimTime;
}

/*!
    \brief Sets the animation time in msec for notification modifications.
    If animation time is zero then animation is disabled.
    \param time - animation time
*/
void QtxNotify::setAnimationTime(const int time)
{
    myAnimTime = time;
}

/*!
    \brief Gets the notification placement policy.
    \return notification placement policy
*/
QtxNotify::PlacementPolicy QtxNotify::placementPolicy() const
{
    return myPlacement;
}

/*!
    \brief Sets the notification placement policy.
    \param placement - notification placement policy
*/
void QtxNotify::setPlacementPolicy(const QtxNotify::PlacementPolicy& placement)
{
    if (myPlacement != placement)
    {
        myPlacement = placement;
        updateArrangement();
    }
}

/*!
    \brief Gets the notification placement base point alignment.
    \return alignment flags
*/
int QtxNotify::alignment() const
{
    return myAlignment;
}

/*!
    \brief Sets the notification placement base point alignment.
    \param falgs - alignment flags
*/
void QtxNotify::setAlignment(const int flags)
{
    if (myAlignment != flags)
    {
        myAlignment = flags;
        updateArrangement();
    }
}

/*!
    \brief Reimplemented for tracking reference widget size changing and
    update notification geometries.
    \param o - handled object
    \param e - handled event
*/
bool QtxNotify::eventFilter(QObject* o, QEvent* e)
{
    if (e->type() == QEvent::Resize && o == window())
    {
        updateArrangement();
    }
    return QObject::eventFilter(o, e);
}

/*!
    \brief Gets the notification width size of each notification. If size value more than 1
           then it will be interpreted as absolute width in pixels. If size value in range
           between 0 and 1 then notification width will be calculated as relative part of
           reference window width.
    \return notification size
*/
double QtxNotify::notificationSize() const
{
    return mySize;
}

/*!
    \brief Sets the notification width size of each notification.
    \param size - notification size
*/
void QtxNotify::setNotificationSize(const double size)
{
    if (mySize != size)
    {
        mySize = size;
        updateArrangement();
    }
}

/*!
    \brief Performs the update of all notification geometries.
    If parameter anim is true then arrangment will be animated otherwise not.
    \param anim - use animation
*/
void QtxNotify::arrangeNotifications(bool anim)
{
    if (myNotifications.isEmpty())
        return;

    QWidget* ref = window() ? window() : QApplication::desktop();
    QPoint refPoint = referencePoint();
    if (myNotifications.first()->isWindow())
        refPoint = ref->mapToGlobal(refPoint);

    int space = 10;
    NotifyList notifications;
    bool reverse = placementPolicy() == QtxNotify::BottomToTop;
    if (alignment() & Qt::AlignBottom)
        reverse = !reverse;

    if (reverse)
    {
        for ( NotifyList::reverse_iterator it = myNotifications.rbegin(); it != myNotifications.rend(); ++it )
            notifications.append(*it);
    }
    else
    {
        for ( NotifyList::iterator it = myNotifications.begin(); it != myNotifications.end(); ++it )
            notifications.append(*it);
    }

    int sign = alignment() & Qt::AlignBottom ? -1 : 1;
    QPoint pos = refPoint;
    QMap<QtxNotify::NotifyWidget*, QRect> geoms;
    for ( NotifyList::iterator it = notifications.begin(); it != notifications.end(); ++it )
    {
        QtxNotify::NotifyWidget* notify = *it;
        QSize size = notify->sizeHint();
        geoms.insert(notify, QRect(QPoint(pos.x(), pos.y() - (sign < 0 ? size.height() : 0)), size));
        pos = QPoint(pos.x(), pos.y() + sign * ( size.height() + space ));
    }

    QParallelAnimationGroup* animGroup = 0;
    if (isAnimated() && anim)
        animGroup = new QParallelAnimationGroup();

    for ( NotifyList::iterator iter = notifications.begin(); iter != notifications.end(); ++iter )
    {
        QtxNotify::NotifyWidget* notify = *iter;
        QRect geom = geoms[notify];
        if (notify->parentWidget())
            geom = QRect(notify->parentWidget()->mapFromGlobal(geom.topLeft()), geom.size());

        bool isvis = notify->isVisible();

        if (animGroup)
        {
            QPropertyAnimation* animation = new QPropertyAnimation(notify, "geometry");
            animation->setDuration(animationTime());
            animGroup->addAnimation(animation);
            if (isvis)
            {
                animation->setStartValue(notify->geometry());
                animation->setEndValue(geom);
            }
            else
            {
                notify->show();
                animation->setStartValue(QRect(QPoint(geom.center().x(), geom.top()), QSize(0, geom.height())));
                animation->setEndValue(geom);
            }
        }
        else
        {
            notify->setGeometry(geom);
            notify->show();
        }
    }
    if (animGroup)
        startAnimation(animGroup);
}

QPoint QtxNotify::referencePoint() const
{
    int margin = 5;
    QWidget* ref = window() ? window() : QApplication::desktop();

    QPoint refPoint;
    int notifywidth = notificationWidth();
    int align = alignment() > 0 ? alignment() : Qt::AlignLeft | Qt::AlignTop;

    if (align & Qt::AlignLeft)
        refPoint.setX(margin);
    else if (align & Qt::AlignRight)
        refPoint.setX(ref->width() - notifywidth - margin);
    else if (align & Qt::AlignHCenter)
        refPoint.setX((ref->width() - notifywidth) / 2);

    if (align & Qt::AlignTop)
        refPoint.setY(margin);
    else if (align & Qt::AlignBottom)
        refPoint.setY(ref->height() - margin);
    else if (align & Qt::AlignVCenter)
        refPoint.setY(ref->height() / 2);

    return ref->mapToGlobal(refPoint);
}

/*!
    \brief Gets the notification with specified identifier.
    \param id - notification identifier
    \return notification instance
*/
QtxNotify::NotifyWidget* QtxNotify::notification(const int id)
{
    QtxNotify::NotifyWidget* notify = 0;
    for (NotifyList::const_iterator it = myNotifications.begin(); it != myNotifications.end() && !notify; ++it)
    {
        if ((*it)->id() == id)
            notify = *it;
    }
    return notify;
}

/*!
    \brief Gets the notification with specified text.
    \param text - notification text
    \return notification instance
*/
QtxNotify::NotifyWidget* QtxNotify::notification(const QString& text)
{
    QtxNotify::NotifyWidget* notify = 0;
    for (NotifyList::const_iterator it = myNotifications.begin(); it != myNotifications.end() && !notify; ++it)
    {
        if ((*it)->text() == text)
            notify = *it;
    }
    return notify;
}

/*!
    \brief Removes the specified notification.
    \param notify - notification instance
*/
void QtxNotify::removeNotification(QtxNotify::NotifyWidget* notify)
{
    if (!notify)
        return;

    myNotifications.removeAll(notify);
    notify->setTimeout(0);

    if (isAnimated()) {
        QPropertyAnimation* animation = new QPropertyAnimation(notify, "geometry");
        animation->setDuration(animationTime());
        animation->setStartValue(notify->geometry());
        animation->setEndValue(QRect(QPoint(notify->geometry().center().x(), notify->geometry().top()),
                                     QSize(0, notify->geometry().height())));

        connect(animation, SIGNAL(finished()), notify, SLOT(hide()));
        connect(animation, SIGNAL(finished()), notify, SLOT(deleteLater()));
        connect(animation, SIGNAL(finished()), this, SLOT(onArrangeTimeout()));
        startAnimation(animation);
    }
    else
    {
        notify->hide();
        notify->deleteLater();
        arrangeNotifications();
    }
}

/*!
    \brief Generates the new free notification identifier.
    \return generated identifier
*/
int QtxNotify::generateId() const
{
    QMap<int, bool> map;
    for (NotifyList::const_iterator it = myNotifications.begin(); it != myNotifications.end(); ++it)
        map.insert((*it)->id(), true);

    int id = 0;
    while (map.contains(id))
        id++;
    return id;
}

/*!
    \brief Gets the notification window with
    \return width in pixels
*/
int QtxNotify::notificationWidth() const
{
    QWidget* refWin = window();
    double size = notificationSize();
    int width = (int)( size > 1 ? size : size * refWin->width() );
    width = qMin(width, refWin->width() - 10);
    return width;
}

/*!
    \brief Schedule delayed notification arrangement.
*/
void QtxNotify::triggerArrange()
{
    if (myArrangeTimer->isActive())
        myArrangeTimer->stop();
    myArrangeTimer->start();
}

/*!
    \brief Gets the animation using state.
    If it's true then animation will be used during notification modifcations.
    \return animation using state
*/
bool QtxNotify::isAnimated() const
{
    return myAnimTime > 0;
}

/*!
    \brief Gets the active animation state. If there is exist active animation then return true otherwise false.
    \return active animation state
*/
bool QtxNotify::hasAcitveAnimation() const
{
    bool has = false;
    for (AnimationList::const_iterator it = myAnimations.begin(); it != myAnimations.end() && !has; ++it)
        has = (*it)->state() == QAbstractAnimation::Running;
    return has;
}

/*!
    \brief Starts the given animation.
    \param animation - animation instance
*/
void QtxNotify::startAnimation(QAbstractAnimation* animation)
{
    myAnimations.append(animation);
    connect(animation, SIGNAL(destroyed(QObject*)), this, SLOT(onAnimationDestroyed(QObject*)));

    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

/*!
    \brief Stops the given animation.
    \param animation - animation instance
*/
void QtxNotify::stopAnimation(QAbstractAnimation* animation)
{
    animation->stop();
    animation->deleteLater();
}

/*!
    \brief Invoked when animation is destroyed. Removes the destroyed animation reference from list.
    \param obj - destroyed animation reference
*/
void QtxNotify::onAnimationDestroyed(QObject* obj)
{
    myAnimations.removeAll((QAbstractAnimation*)obj);
}

/*!
    \brief Performs the scheduled delayed notification arrangment.
*/
void QtxNotify::onArrangeTimeout()
{
    if (hasAcitveAnimation())
        triggerArrange();
    else
    {
        arrangeNotifications(!myAnimBlock);
        myAnimBlock = false;
    }
}

/*!
    \brief Performs the notification arrangment with disabled animation.
*/
void QtxNotify::updateArrangement()
{
    myAnimBlock = true;
    triggerArrange();
}
