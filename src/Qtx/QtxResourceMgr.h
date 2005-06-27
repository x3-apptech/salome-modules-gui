#ifndef QTX_RESOURCEMGR_H
#define QTX_RESOURCEMGR_H

#include "Qtx.h"

#include <qmap.h>
#include <qcolor.h>
#include <qpixmap.h>
#include <qstringlist.h>
#include <qtranslator.h>

class QPixmap;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxResourceMgr
{
  class IniFormat;
  class XmlFormat;
  class Resources;

public:

  class Format;

  typedef QMap<QString, QString> Section;

public:
  QtxResourceMgr( const QString&, const QString& = QString::null );
  virtual ~QtxResourceMgr();

  QString         appName() const;
  QStringList     dirList() const;

  void            clear();

  QString         currentSection() const;
  void            setCurrentSection( const QString& );

  bool            value( const QString&, int& ) const;
  bool            value( const QString&, double& ) const;
  bool            value( const QString&, bool& ) const;
  bool            value( const QString&, QColor& ) const;
  bool            value( const QString&, QString&, const bool = true ) const;

  bool            value( const QString&, const QString&, int& ) const;
  bool            value( const QString&, const QString&, double& ) const;
  bool            value( const QString&, const QString&, bool& ) const;
  bool            value( const QString&, const QString&, QColor& ) const;
  bool            value( const QString&, const QString&, QString&, const bool = true ) const;

  int             integerValue( const QString&, const int = 0 ) const;
  double          doubleValue( const QString&, const double = 0 ) const;
  bool            booleanValue( const QString&, const bool = false ) const;
  QColor          colorValue( const QString&, const QColor& = QColor() ) const;
  QString         stringValue( const QString&, const char* = 0 ) const;

  int             integerValue( const QString&, const QString&, const int = 0 ) const;
  double          doubleValue( const QString&, const QString&, const double = 0 ) const;
  bool            booleanValue( const QString&, const QString&, const bool = false ) const;
  QColor          colorValue( const QString&, const QString&, const QColor& = QColor() ) const;
  QString         stringValue( const QString&, const QString&, const char* = 0 ) const;

  bool            hasSection( const QString& ) const;
  bool            hasValue( const QString& ) const;
  bool            hasValue( const QString&, const QString& ) const;

  void            setValue( const QString&, const int );
  void            setValue( const QString&, const double );
  void            setValue( const QString&, const bool );
  void            setValue( const QString&, const QColor& );
  void            setValue( const QString&, const QString& );

  void            setValue( const QString&, const QString&, const int );
  void            setValue( const QString&, const QString&, const double );
  void            setValue( const QString&, const QString&, const bool );
  void            setValue( const QString&, const QString&, const QColor& );
  void            setValue( const QString&, const QString&, const QString& );

  void            remove( const QString& );
  void            remove( const QString&, const QString& );
  void            removeSection( const QString& );

  QString         currentFormat() const;
  void            setCurrentFormat( const QString& );

  Format*         format( const QString& ) const;
  void            installFormat( Format* );
  void            removeFormat( Format* );

  QStringList     options() const;
  QString         option( const QString& ) const;
  void            setOption( const QString&, const QString& );

  QPixmap         defaultPixmap() const;
  virtual void    setDefaultPixmap( const QPixmap& );

  QString         resSection() const;
  QString         langSection() const;

  QPixmap         loadPixmap( const QString&, const QString& ) const;
  QPixmap         loadPixmap( const QString&, const QString&, const bool ) const;
  QPixmap         loadPixmap( const QString&, const QString&, const QPixmap& ) const;
  void            loadLanguage( const QString& = QString::null, const QString& = QString::null );

  void            raiseTranslators( const QString& );
  void            removeTranslators( const QString& );
  void            loadTranslator( const QString&, const QString& );

  QString         path( const QString&, const QString&, const QString& ) const;

  bool            load();
  bool            save();

  QStringList     sections() const;
  QStringList     parameters( const QString& ) const;

  void            refresh();

protected:
  void            setDirList( const QStringList& );

  virtual QString userFileName( const QString& ) const;
  virtual QString globalFileName( const QString& ) const;

private:
  void            initialize( const bool = true ) const;
  QString         substMacro( const QString&, const QMap<QChar, QString>& ) const;

private:
  typedef QPtrList<Resources>           ResList;
  typedef QPtrList<Format>              FormatList;
  typedef QMap<QString, QString>        OptionsMap;
  typedef QPtrListIterator<Resources>   ResListIterator;
  typedef QPtrListIterator<Format>      FormatListIterator;

  typedef QPtrList<QTranslator>         TransList;
  typedef QMap<QString, TransList>      TransListMap;
  typedef QPtrListIterator<QTranslator> TransListIterator;

private:
  QString         myAppName;
  QStringList     myDirList;
  FormatList      myFormats;
  OptionsMap      myOptions;
  ResList         myResources;
  TransListMap    myTranslator;
  QString         myCurSection;
  QPixmap         myDefaultPix;
};

class QTX_EXPORT QtxResourceMgr::Format
{
public:
  Format( const QString& );
  ~Format();

  QString                format() const;

  QStringList            options() const;
  QString                option( const QString& ) const;
  void                   setOption( const QString&, const QString& );

  bool                   load( Resources* );
  bool                   save( Resources* );

protected:
  virtual bool           load( const QString&, QMap<QString, Section>& ) = 0;
  virtual bool           save( const QString&, const QMap<QString, Section>& ) = 0;

private:
  QString                myFmt;
  QMap<QString, QString> myOpt;
};

class QtxResourceMgr::Resources
{
public:
  Resources( const QString& );
  virtual ~Resources();

  QString                file() const;
  void                   setFile( const QString& );

  QString                value( const QString&, const QString&, const bool ) const;
  void                   setValue( const QString&, const QString&, const QString& );

  bool                   hasSection( const QString& ) const;
  bool                   hasValue( const QString&, const QString& ) const;

  void                   removeSection( const QString& );
  void                   removeValue( const QString&, const QString& );

  QPixmap                loadPixmap( const QString&, const QString&, const QString& ) const;
  QTranslator*           loadTranslator( const QString&, const QString&, const QString& ) const;

  QString                environmentVariable( const QString&, int&, int& ) const;
  QString                makeSubstitution( const QString&, const QString&, const QString& ) const;

  void                   clear();

  QStringList            sections() const;
  QStringList            parameters( const QString& ) const;

  QString                path( const QString&, const QString&, const QString& ) const;

private:
  Section&               section( const QString& );
  const Section&         section( const QString& ) const;

  QString                fileName( const QString&, const QString&, const QString& ) const;

private:
  QMap<QString, Section> mySections;
  QString                myFileName;

  friend class QtxResourceMgr::Format;
};

#endif
