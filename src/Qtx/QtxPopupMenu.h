#ifndef QTXPOPUPMENU_H
#define QTXPOPUPMENU_H

#include "Qtx.h"

#include <qstring.h>
#include <qiconset.h>
#include <qpopupmenu.h>

class QTX_EXPORT QtxPopupMenu : public QPopupMenu
{
  Q_OBJECT

protected:
  class TitleMenuItem;

public:
  enum { TitleAuto, TitleOn, TitleOff };

public:
  QtxPopupMenu( QWidget* = 0, const char* = 0 );
  virtual ~QtxPopupMenu();

  QString                titleText() const;
  QIconSet               titleIcon() const;

  int                    titlePolicy() const;
  int                    titleAlignment() const;

  virtual void           setTitleText( const QString& );
  virtual void           setTitleIcon( const QIconSet& );

  virtual void           setTitlePolicy( const int );
  virtual void           setTitleAlignment( const int );

public slots:
  virtual void           show();
  virtual void           hide();

protected:
  virtual TitleMenuItem* createTitleItem( const QString&, const QIconSet&, const int ) const;

private:
  void                   updateTitle();
  void                   insertTitle();
  void                   removeTitle();

private:
  int                    myId;
  QString                myText;
  QIconSet               myIcon;
  int                    myAlign;
  int                    myPolicy;
};

#endif
