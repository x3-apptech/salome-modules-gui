// Copyright (C) 2007-2018  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef QTXNOTIFY_H
#define QTXNOTIFY_H

#include "Qtx.h"

#include <QWidget>

#include <QList>
#include <QAbstractAnimation>

class QTimer;

class QTX_EXPORT QtxNotify : public QObject
{
    Q_OBJECT

    class NotifyWidget;

public:
    typedef enum {
        TopToBottom,
        BottomToTop
    } PlacementPolicy;

    Q_ENUM(PlacementPolicy)

public:
    QtxNotify(QObject* = 0);
    QtxNotify(QWidget*, QObject* = 0);
    virtual ~QtxNotify();

    int             showNotification(const QString&, const QString&, int timeout = -1);
    void            hideNotification(const QString& = QString());
    void            hideNotification(const int);

    QWidget*        window() const;
    void            setWindow(QWidget* window);

    double          notificationSize() const;
    void            setNotificationSize(const double);

    int             animationTime() const;
    void            setAnimationTime(const int);

    PlacementPolicy placementPolicy() const;
    void            setPlacementPolicy(const PlacementPolicy&);

    int             alignment() const;
    void            setAlignment(const int);

    virtual bool    eventFilter(QObject*, QEvent*);

private Q_SLOTS:
    void            onArrangeTimeout();
    void            onAnimationDestroyed(QObject*);

private:
    void            triggerArrange();
    void            updateArrangement();
    void            arrangeNotifications(bool = true);

    NotifyWidget*   notification(const int);
    NotifyWidget*   notification(const QString&);
    void            removeNotification(NotifyWidget*);

    int             generateId() const;

    bool            isAnimated() const;

    bool            hasAcitveAnimation() const;
    void            startAnimation(QAbstractAnimation*);
    void            stopAnimation(QAbstractAnimation*);

    QPoint          referencePoint() const;
    int             notificationWidth() const;

private:
    typedef QList<QtxNotify::NotifyWidget*> NotifyList;
    typedef QList<QAbstractAnimation*>      AnimationList;

private:
    double          mySize;
    QWidget*        myWindow;
    PlacementPolicy myPlacement;
    int             myAlignment;
    QTimer*         myArrangeTimer;

    int             myAnimTime;
    bool            myAnimBlock;
    AnimationList   myAnimations;

    NotifyList      myNotifications;

    friend class QtxNotify::NotifyWidget;
};

#endif // QTXNOTIFY_H
