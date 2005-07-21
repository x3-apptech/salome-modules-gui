#include "QtxResourceMgr.h"

#include <qdir.h>
#include <qfile.h>
#include <qregexp.h>
#include <qpixmap.h>
#include <qtranslator.h>
#include <qapplication.h>

#ifndef QT_NO_DOM
#include <qdom.h>
#endif

#include <stdlib.h>

/*!
  Class: QtxResourceMgr::Resources
  Level: Internal
*/

QtxResourceMgr::Resources::Resources( const QString& fileName )
: myFileName( fileName )
{
}

QtxResourceMgr::Resources::~Resources()
{
}

QString QtxResourceMgr::Resources::file() const
{
  return myFileName;
}

void QtxResourceMgr::Resources::setFile( const QString& fn )
{
  myFileName = fn;
}

QString QtxResourceMgr::Resources::value( const QString& sect, const QString& name, const bool subst ) const
{
  QString val;

  if ( hasValue( sect, name ) )
  {
    val = section( sect )[name];
    if ( subst )
      val = makeSubstitution( val, sect, name );
  }
  return val;
}

void QtxResourceMgr::Resources::setValue( const QString& sect, const QString& name, const QString& val )
{
  Section& s = section( sect );
  s.insert( name, val );
}

bool QtxResourceMgr::Resources::hasSection( const QString& sect ) const
{
  return mySections.contains( sect );
}

bool QtxResourceMgr::Resources::hasValue( const QString& sect, const QString& name ) const
{
  return hasSection( sect ) && section( sect ).contains( name );
}

void QtxResourceMgr::Resources::removeSection( const QString& sect )
{
  mySections.remove( sect );
}

void QtxResourceMgr::Resources::removeValue( const QString& sect, const QString& name )
{
  if ( !hasSection( sect ) )
    return;

  Section& s = section( sect );
  s.remove( name );

  if ( s.isEmpty() )
    mySections.remove( sect );
}

void QtxResourceMgr::Resources::clear()
{
  mySections.clear();
}

QStringList QtxResourceMgr::Resources::sections() const
{
  return mySections.keys();
}

QStringList QtxResourceMgr::Resources::parameters( const QString& sec ) const
{
  if ( !hasSection( sec ) )
    return QStringList();

  return section( sec ).keys();
}

QString QtxResourceMgr::Resources::path( const QString& sec, const QString& prefix, const QString& name ) const
{
  QString filePath = fileName( sec, prefix, name );
  if ( !filePath.isEmpty() )
  {
    if ( !QFileInfo( filePath ).exists() )
      filePath = QString::null;
  }
  return filePath;
}

QtxResourceMgr::Section& QtxResourceMgr::Resources::section( const QString& sn )
{
  if ( !mySections.contains( sn ) )
    mySections.insert( sn, Section() );

  return mySections[sn];
}

const QtxResourceMgr::Section& QtxResourceMgr::Resources::section( const QString& sn ) const
{
  return mySections[sn];
}

QString QtxResourceMgr::Resources::fileName( const QString& sect, const QString& prefix, const QString& name ) const
{
  QString path;
  if ( hasValue( sect, prefix ) )
  {
    path = value( sect, prefix, true );
    if ( !path.isEmpty() )
    {
      if ( QFileInfo( path ).isRelative() )
        path = Qtx::addSlash( QFileInfo( myFileName ).dirPath( true ) ) + path;

      path = Qtx::addSlash( path ) + name;
    }
  }
  return QDir::convertSeparators( path );
}

QPixmap QtxResourceMgr::Resources::loadPixmap( const QString& sect, const QString& prefix, const QString& name ) const
{
  return QPixmap( fileName( sect, prefix, name ) );
}

QTranslator* QtxResourceMgr::Resources::loadTranslator( const QString& sect, const QString& prefix, const QString& name ) const
{
  QTranslator* trans = new QTranslator( 0 );
  if ( !trans->load( fileName( sect, prefix, name ) ) )
  {
    delete trans;
    trans = 0;
  }
  return trans;
}

QString QtxResourceMgr::Resources::environmentVariable( const QString& str, int& start, int& len ) const
{
  QString varName = QString::null;
  len = 0;

  QRegExp rx( "\\$\\{([a-zA-Z]+[a-zA-Z0-9_]*)\\}|\\$\\(([a-zA-Z]+[a-zA-Z0-9_]*)\\)|\\$([a-zA-Z]+[a-zA-Z0-9_]*)|\\%([a-zA-Z]+[a-zA-Z0-9_]*)\\%" );

  int pos = rx.search( str, start );
  if ( pos != -1 )
  {
    start = pos;
    len = rx.matchedLength();
    QStringList caps = rx.capturedTexts();
    for ( uint i = 1; i <= caps.count() && varName.isEmpty(); i++ )
      varName = *caps.at( i );
  }
  return varName;
}

QString QtxResourceMgr::Resources::makeSubstitution( const QString& str, const QString& sect, const QString& name ) const
{
  QString res = str;

  QMap<QString, int> ignoreMap;
  ignoreMap.insert( name, 0 );

  int start( 0 ), len( 0 );
  while ( true )
  {
    QString envName = environmentVariable( res, start, len );
    if ( envName.isNull() )
      break;

    QString newStr = QString::null;
    if ( ::getenv( envName ) )
      newStr = QString( ::getenv( envName ) );

    if ( newStr.isNull() )
    {
      if ( ignoreMap.contains( envName ) )
      {
        start += len;
        continue;
      }

      if ( hasValue( sect, envName ) )
        newStr = value( sect, envName, false );
      ignoreMap.insert( envName, 0 );
    }
    res.replace( start, len, newStr );
  }

  return res;
}

/*!
	Class: QtxResourceMgr::IniFormat
	Level: Internal
*/

class QtxResourceMgr::IniFormat : public Format
{
public:
  IniFormat();
  ~IniFormat();

protected:
  virtual bool load( const QString&, QMap<QString, Section>& );
  virtual bool save( const QString&, const QMap<QString, Section>& );
};

QtxResourceMgr::IniFormat::IniFormat()
: Format( "ini" )
{
}

QtxResourceMgr::IniFormat::~IniFormat()
{
}

bool QtxResourceMgr::IniFormat::load( const QString& fname, QMap<QString, Section>& secMap )
{
  QFile file( fname );
  if ( !file.open( IO_ReadOnly ) )
    return false;

  QTextStream ts( &file );

  QString data;
  int line = 0;
  bool res = true;
  QString section;

  QString separator = option( "separator" );
  if ( separator.isNull() )
    separator = QString( "=" );

  QString comment = option( "comment" );
  if ( comment.isNull() )
    comment = QString( "#" );

  while ( true )
  {
    data = ts.readLine();
    line++;

    if ( data.isNull() )
      break;

    data = data.stripWhiteSpace();
    if ( data.isEmpty() )
      continue;

    if ( data.startsWith( comment ) )
      continue;

    QRegExp rx( "^\\[([\\w\\s]*)\\]$" );
    if ( rx.search( data ) != -1 )
    {
      section = rx.cap( 1 );
      if ( section.isEmpty() )
      {
        res = false;
        qWarning( QString( "Empty section in line %1" ).arg( line ) );
      }
    }
    else if ( data.contains( "=" ) && !section.isEmpty() )
    {
      int pos = data.find( separator );
      QString key = data.left( pos - 1 ).stripWhiteSpace();
      QString val = data.mid( pos + 1 ).stripWhiteSpace();
      secMap[section].insert( key, val );
    }
    else
    {
      res = false;
      section.isEmpty() ? qWarning( "Current section is empty" ) :
                          qWarning( QString( "Error in line: %1" ).arg( line ) );
    }
  }

  file.close();

  return res;
}

bool QtxResourceMgr::IniFormat::save( const QString& fname, const QMap<QString, Section>& secMap )
{
  QFile file( fname );
  if ( !file.open( IO_WriteOnly ) )
    return false;

  bool res = true;
  for ( QMap<QString, Section>::ConstIterator it = secMap.begin(); it != secMap.end() && res; ++it )
  {
    QString data = QString( "[%1]\n" ).arg( it.key() );
    for ( Section::ConstIterator iter = it.data().begin(); iter != it.data().end(); ++iter )
      data += iter.key() + " = " + iter.data() + "\n";
    data += "\n";

    res = file.writeBlock( data.latin1(), data.length() ) == (int)data.length();
  }

  file.close();

  return res;
}

/*!
	Class: QtxResourceMgr::XmlFormat
	Level: Internal
*/

class QtxResourceMgr::XmlFormat : public Format
{
public:
  XmlFormat();
  ~XmlFormat();

protected:
  virtual bool load( const QString&, QMap<QString, Section>& );
  virtual bool save( const QString&, const QMap<QString, Section>& );

private:
  QString      docTag() const;
  QString      sectionTag() const;
  QString      parameterTag() const;
  QString      nameAttribute() const;
  QString      valueAttribute() const;
};

QtxResourceMgr::XmlFormat::XmlFormat()
: Format( "xml" )
{
}

QtxResourceMgr::XmlFormat::~XmlFormat()
{
}

bool QtxResourceMgr::XmlFormat::load( const QString& fname, QMap<QString, Section>& secMap )
{
  bool res = false;

#ifndef QT_NO_DOM

  QFile file( fname );
  if ( !file.open( IO_ReadOnly ) )
    return false;

  QDomDocument doc;

  res = doc.setContent( &file );
  file.close();

  if ( !res )
    return false;

  QDomElement root = doc.documentElement();
  if ( root.isNull() || root.tagName() != docTag() )
    return false;

  QDomNode sectNode = root.firstChild();
  while ( res && !sectNode.isNull() )
  {
    res = sectNode.isElement();
    if ( res )
    {
      QDomElement sectElem = sectNode.toElement();
      if ( sectElem.tagName() == sectionTag() && sectElem.hasAttribute( nameAttribute() ) )
      {
        QString section = sectElem.attribute( nameAttribute() );
        QDomNode paramNode = sectNode.firstChild();
        while ( res && !paramNode.isNull() )
        {
          res = paramNode.isElement();
          if ( res )
          {
            QDomElement paramElem = paramNode.toElement();
            if ( paramElem.tagName() == parameterTag() &&
                 paramElem.hasAttribute( nameAttribute() ) && paramElem.hasAttribute( valueAttribute() ) )
            {
              QString paramName = paramElem.attribute( nameAttribute() );
              QString paramValue = paramElem.attribute( valueAttribute() );

              secMap[section].insert( paramName, paramValue );
            }
            else
              res = false;
          }
	  else
	    res = paramNode.isComment();

          paramNode = paramNode.nextSibling();
        }
      }
      else
        res = false;
    }
    else
      res = sectNode.isComment(); // if it's a comment -- let it be, pass it..

    sectNode = sectNode.nextSibling();
  }

#endif

  return res;
}

bool QtxResourceMgr::XmlFormat::save( const QString& fname, const QMap<QString, Section>& secMap )
{
  bool res = false;

#ifndef QT_NO_DOM

  QFile file( fname );
  if ( !file.open( IO_WriteOnly ) )
    return false;

  QDomDocument doc( docTag() );
  QDomElement root = doc.createElement( docTag() );
  doc.appendChild( root );

  for ( QMap<QString, Section>::ConstIterator it = secMap.begin(); it != secMap.end(); ++it )
  {
    QDomElement sect = doc.createElement( sectionTag() );
    sect.setAttribute( nameAttribute(), it.key() );
    root.appendChild( sect );
    for ( QMap<QString, QString>::ConstIterator iter = it.data().begin(); iter != it.data().end(); ++iter )
    {
      QDomElement val = doc.createElement( parameterTag() );
      val.setAttribute( nameAttribute(), iter.key() );
      val.setAttribute( valueAttribute(), iter.data() );
      sect.appendChild( val );
    }
  }

  QString docStr = doc.toString();
  res = file.writeBlock( docStr.latin1(), docStr.length() ) == (int)docStr.length();
  file.close();

#endif

  return res;
}

QString QtxResourceMgr::XmlFormat::docTag() const
{
  QString tag = option( "doc_tag" );
  if ( tag.isEmpty() )
    tag = QString( "document" );
  return tag;
}

QString QtxResourceMgr::XmlFormat::sectionTag() const
{
  QString tag = option( "section_tag" );
  if ( tag.isEmpty() )
    tag = QString( "section" );
  return tag;
}

QString QtxResourceMgr::XmlFormat::parameterTag() const
{
  QString tag = option( "parameter_tag" );
  if ( tag.isEmpty() )
    tag = QString( "parameter" );
  return tag;
}

QString QtxResourceMgr::XmlFormat::nameAttribute() const
{
  QString str = option( "name_attribute" );
  if ( str.isEmpty() )
    str = QString( "name" );
  return str;
}

QString QtxResourceMgr::XmlFormat::valueAttribute() const
{
  QString str = option( "value_attribute" );
  if ( str.isEmpty() )
    str = QString( "value" );
  return str;
}

/*!
	Class: QtxResourceMgr::Format
	Level: Public
*/

QtxResourceMgr::Format::Format( const QString& fmt )
: myFmt( fmt )
{
}

QtxResourceMgr::Format::~Format()
{
}

QString QtxResourceMgr::Format::format() const
{
  return myFmt;
}

QStringList QtxResourceMgr::Format::options() const
{
  return myOpt.keys();
}

QString QtxResourceMgr::Format::option( const QString& opt ) const
{
  QString val;
  if ( myOpt.contains( opt ) )
    val = myOpt[opt];
  return val;
}
void QtxResourceMgr::Format::setOption( const QString& opt, const QString& val )
{
  myOpt.insert( opt, val );
}

bool QtxResourceMgr::Format::load( Resources* res )
{
  if ( !res )
    return false;

  QMap<QString, Section> sections;
  bool status = load( res->myFileName, sections );
  if ( status )
    res->mySections = sections;
  else
    qDebug( "QtxResourceMgr: Could not load resource file \"%s\"", res->myFileName.latin1() );

  return status;
}

bool QtxResourceMgr::Format::save( Resources* res )
{
  if ( !res )
    return false;

  Qtx::mkDir( Qtx::dir( res->myFileName ) );

  return save( res->myFileName, res->mySections );
}

/*!
	Class: QtxResourceMgr
	Level: Public
*/

QtxResourceMgr::QtxResourceMgr( const QString& appName, const QString& resVarTemplate )
: myAppName( appName )
{
  QString envVar = !resVarTemplate.isEmpty() ? resVarTemplate : QString( "%1Resources" );
  if ( envVar.contains( "%1" ) )
    envVar = envVar.arg( appName );

  QString dirs;
  if ( ::getenv( envVar ) )
    dirs = ::getenv( envVar );

  setDirList( QStringList::split( ";", dirs ) );

  installFormat( new XmlFormat() );
  installFormat( new IniFormat() );

  setOption( "translators", QString( "%P_msg_%L.qm|%P_images.qm" ) );
}

QtxResourceMgr::~QtxResourceMgr()
{
  QStringList prefList = myTranslator.keys();
  for ( QStringList::const_iterator it = prefList.begin(); it != prefList.end(); ++it )
    removeTranslators( *it );
}

QString QtxResourceMgr::appName() const
{
  return myAppName;
}

QStringList QtxResourceMgr::dirList() const
{
  return myDirList;
}

void QtxResourceMgr::setDirList( const QStringList& dl )
{
  myDirList = dl;
  for ( ResListIterator it( myResources ); it.current(); ++it )
    delete it.current();

  myResources.clear();
}

void QtxResourceMgr::initialize( const bool autoLoad ) const
{
  if ( !myResources.isEmpty() )
    return;

  QtxResourceMgr* that = (QtxResourceMgr*)this;

  that->myResources.append( new Resources( userFileName( appName() ) ) );
  for ( QStringList::const_iterator it = myDirList.begin(); it != myDirList.end(); ++it )
  {
    QString path = Qtx::addSlash( *it ) + globalFileName( appName() );
    that->myResources.append( new Resources( path ) );
  }

  if ( autoLoad )
    that->load();
}

void QtxResourceMgr::clear()
{
  for ( ResListIterator it( myResources ); it.current(); ++it )
    it.current()->clear();
}

QString QtxResourceMgr::currentSection() const
{
  return myCurSection;
}

void QtxResourceMgr::setCurrentSection( const QString& str )
{
  myCurSection = str;
}

bool QtxResourceMgr::value( const QString& name, int& val ) const
{
  return value( currentSection(), name, val );
}

bool QtxResourceMgr::value( const QString& name, double& val ) const
{
  return value( currentSection(), name, val );
}

bool QtxResourceMgr::value( const QString& name, bool& val ) const
{
  return value( currentSection(), name, val );
}

bool QtxResourceMgr::value( const QString& name, QColor& val ) const
{
  return value( currentSection(), name, val );
}

bool QtxResourceMgr::value( const QString& name, QFont& val ) const
{
  return value( currentSection(), name, val );
}

bool QtxResourceMgr::value( const QString& name, QString& val, const bool subst ) const
{
  return value( currentSection(), name, val, subst );
}

bool QtxResourceMgr::value( const QString& sect, const QString& name, int& iVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  bool ok;
  iVal = val.toInt( &ok );

  return ok;
}

bool QtxResourceMgr::value( const QString& sect, const QString& name, double& dVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  bool ok;
  dVal = val.toDouble( &ok );

  return ok;
}

bool QtxResourceMgr::value( const QString& sect, const QString& name, bool& bVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  static QMap<QString, bool> boolMap;
  if ( boolMap.isEmpty() )
  {
    boolMap["true"]  = boolMap["yes"] = boolMap["on"]  = true;
    boolMap["false"] = boolMap["no"]  = boolMap["off"] = false;
  }

  val = val.lower();
  bool res = boolMap.contains( val );
  if ( res )
    bVal = boolMap[val];
  else
  {
    double num = val.toDouble( &res );
    if ( res )
      bVal = num != 0;
  }

  return res;
}

bool QtxResourceMgr::value( const QString& sect, const QString& name, QColor& cVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  bool res = true;
  QStringList vals = QStringList::split( ",", val, true );

  QIntList nums;
  for ( QStringList::const_iterator it = vals.begin(); it != vals.end() && res; ++it )
    nums.append( (*it).toInt( &res ) );

  if ( res && nums.count() >= 3 )
    cVal.setRgb( nums[0], nums[1], nums[2] );
  else
  {
    int pack = val.toInt( &res );
    if ( res )
      Qtx::rgbSet( pack, cVal );
  }

  return res;
}

bool QtxResourceMgr::value( const QString& sect, const QString& name, QFont& fVal ) const
{
  QString val = stringValue( sect, name, "" );
  QStringList font_values = QStringList::split( ",", val );
  if( font_values.count()<2 || font_values.count()>5 )
    return false;
  
  QString family = font_values[0];
  bool isBold = false, isItalic = false, isUnderline = false, isOk = false;
  int pSize = -1;
  for( int i=1, n=font_values.count(); i<n; i++ )
  {
    QString curval = font_values[i].stripWhiteSpace().lower();
    if( !isBold && curval=="bold" )
      isBold = true;
    else if( !isItalic && curval=="italic" )
      isItalic = true;
    else if( !isUnderline && curval=="underline" )
      isUnderline = true;
    else if( pSize<0 )
    {
      pSize = curval.toInt( &isOk );
      if( !isOk )
        pSize = -1;
    }
  }

  if( pSize>0 && !family.isEmpty() )
  {
    fVal = QFont( family, pSize );
    fVal.setBold( isBold );
    fVal.setItalic( isItalic );
    fVal.setUnderline( isUnderline );
    return true;
  }
  else
    return false;
}

bool QtxResourceMgr::value( const QString& sect, const QString& name, QString& val, const bool subst ) const
{
  initialize();

  bool ok = false;
  for ( ResListIterator it( myResources ); it.current() && !ok; ++it )
  {
    ok = it.current()->hasValue( sect, name );
    if ( ok )
      val = it.current()->value( sect, name, subst );
  }

  return ok;
}

int QtxResourceMgr::integerValue( const QString& name, const int def ) const
{
  return integerValue( currentSection(), name, def );
}

double QtxResourceMgr::doubleValue( const QString& name, const double def ) const
{
  return doubleValue( currentSection(), name, def );
}

bool QtxResourceMgr::booleanValue( const QString& name, const bool def ) const
{
  return booleanValue( currentSection(), name, def );
}

QFont QtxResourceMgr::fontValue( const QString& name, const QFont& def ) const
{
  return fontValue( currentSection(), name, def );
}
  
QColor QtxResourceMgr::colorValue( const QString& name, const QColor& def ) const
{
  return colorValue( currentSection(), name, def );
}

QString QtxResourceMgr::stringValue( const QString& name, const char* def ) const
{
  return stringValue( currentSection(), name, def );
}

int QtxResourceMgr::integerValue( const QString& sect, const QString& name, const int def ) const
{
  int val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

double QtxResourceMgr::doubleValue( const QString& sect, const QString& name, const double def ) const
{
  double val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

bool QtxResourceMgr::booleanValue( const QString& sect, const QString& name, const bool def ) const
{
  bool val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

QFont QtxResourceMgr::fontValue( const QString& sect, const QString& name, const QFont& def ) const
{
  QFont font;
  if( !value( sect, name, font ) )
    font = def;
  return font;
}

QColor QtxResourceMgr::colorValue( const QString& sect, const QString& name, const QColor& def ) const
{
  QColor val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

QString QtxResourceMgr::stringValue( const QString& sect, const QString& name, const char* def ) const
{
  QString val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

bool QtxResourceMgr::hasValue( const QString& name ) const
{
  return hasValue( currentSection(), name );
}

bool QtxResourceMgr::hasValue( const QString& sect, const QString& name ) const
{
  initialize();

  bool ok = false;
  for ( ResListIterator it( myResources ); it.current() && !ok; ++it )
    ok = it.current()->hasValue( sect, name );

  return ok;
}

bool QtxResourceMgr::hasSection( const QString& sect ) const
{
  initialize();

  bool ok = false;
  for ( ResListIterator it( myResources ); it.current() && !ok; ++it )
    ok = it.current()->hasSection( sect );

  return ok;
}

void QtxResourceMgr::setValue( const QString& name, int val )
{
  setValue( currentSection(), name, val );
}

void QtxResourceMgr::setValue( const QString& name, double val )
{
  setValue( currentSection(), name, val );
}

void QtxResourceMgr::setValue( const QString& name, bool val )
{
  setValue( currentSection(), name, val );
}

void QtxResourceMgr::setValue( const QString& name, const QColor& val )
{
  setValue( currentSection(), name, val );
}

void QtxResourceMgr::setValue( const QString& name, const QFont& val )
{
  setValue( currentSection(), name, val );
}

void QtxResourceMgr::setValue( const QString& name, const QString& val )
{
  setValue( currentSection(), name, val );
}

void QtxResourceMgr::setValue( const QString& sect, const QString& name, int val )
{
  setValue( sect, name, QString::number( val ) );
}

void QtxResourceMgr::setValue( const QString& sect, const QString& name, double val )
{
  setValue( sect, name, QString::number( val, 'g', 12 ) );
}

void QtxResourceMgr::setValue( const QString& sect, const QString& name, bool val )
{
  setValue( sect, name, QString( val ? "true" : "false" ) );
}

void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QColor& val )
{
  setValue( sect, name, QString( "%1, %2, %3").arg( val.red() ).arg( val.green() ).arg( val.blue() ) );
}

void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QFont& f )
{
  QStringList val;
  val.append( f.family() );
  if( f.bold() )
    val.append( "Bold" );
  if( f.italic() )
    val.append( "Italic" );
  if( f.underline() )
    val.append( "Underline" );
  val.append( QString( "%1" ).arg( f.pointSize() ) );
  
  setValue( sect, name, val.join( "," ) );
}

void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QString& val )
{
  initialize();

  if ( !myResources.isEmpty() )
    myResources.first()->setValue( sect, name, val );
}

void QtxResourceMgr::remove( const QString& name )
{
  remove( currentSection(), name );
}

void QtxResourceMgr::remove( const QString& sect, const QString& name )
{
  initialize();

  for ( ResListIterator it( myResources ); it.current(); ++it )
    it.current()->removeValue( sect, name );
}

void QtxResourceMgr::removeSection( const QString& sect )
{
  initialize();

  for ( ResListIterator it( myResources ); it.current(); ++it )
    it.current()->removeSection( sect );
}

QString QtxResourceMgr::currentFormat() const
{
  QString fmt;
  if ( !myFormats.isEmpty() )
    fmt = myFormats.getFirst()->format();
  return fmt;
}

void QtxResourceMgr::setCurrentFormat( const QString& fmt )
{
  Format* form = format( fmt );
  if ( !form )
    return;

  myFormats.remove( form );
  myFormats.prepend( form );

  if ( myResources.isEmpty() )
    return;

  ResListIterator resIt( myResources );
  if ( resIt.current() )
    resIt.current()->setFile( userFileName( appName() ) );
  ++resIt;

  for ( QStringList::const_iterator it = myDirList.begin(); it != myDirList.end() && resIt.current(); ++it, ++resIt )
    resIt.current()->setFile( Qtx::addSlash( *it ) + globalFileName( appName() ) );
}

QtxResourceMgr::Format* QtxResourceMgr::format( const QString& fmt ) const
{
  Format* form = 0;
  for ( FormatListIterator it( myFormats ); it.current() && !form; ++it )
  {
    if ( it.current()->format() == fmt )
      form = it.current();
  }

  return form;
}

void QtxResourceMgr::installFormat( QtxResourceMgr::Format* form )
{
  if ( !myFormats.contains( form ) )
    myFormats.prepend( form );
}

void QtxResourceMgr::removeFormat( QtxResourceMgr::Format* form )
{
  myFormats.remove( form );
}

QStringList QtxResourceMgr::options() const
{
  return myOptions.keys();
}

QString QtxResourceMgr::option( const QString& opt ) const
{
  QString val;
  if ( myOptions.contains( opt ) )
    val = myOptions[opt];
  return val;
}

void QtxResourceMgr::setOption( const QString& opt, const QString& val )
{
  myOptions.insert( opt, val );
}

bool QtxResourceMgr::load()
{
  initialize( false );

  Format* fmt = format( currentFormat() );
  if ( !fmt )
    return false;

  bool res = true;
  for ( ResListIterator it( myResources ); it.current(); ++it )
    res = fmt->load( it.current() ) && res;

  return res;
}

bool QtxResourceMgr::save()
{
  initialize( false );

  Format* fmt = format( currentFormat() );
  if ( !fmt )
    return false;

  if ( myResources.isEmpty() )
    return true;

  return fmt->save( myResources.getFirst() );
}

QStringList QtxResourceMgr::sections() const
{
  initialize();

  QMap<QString, int> map;
  for ( ResListIterator it( myResources ); it.current(); ++it )
  {
    QStringList lst = it.current()->sections();
    for ( QStringList::const_iterator itr = lst.begin(); itr != lst.end(); ++itr )
      map.insert( *itr, 0 );
  }

  QStringList res;
  for ( QMap<QString, int>::ConstIterator iter = map.begin(); iter != map.end(); ++iter )
    res.append( iter.key() );

  return res;
}

QStringList QtxResourceMgr::parameters( const QString& sec ) const
{
  initialize();

  QMap<QString, int> map;
  for ( ResListIterator it( myResources ); it.current(); ++it )
  {
    QStringList lst = it.current()->parameters( sec );
    for ( QStringList::const_iterator itr = lst.begin(); itr != lst.end(); ++itr )
      map.insert( *itr, 0 );
  }

  QStringList res;
  for ( QMap<QString, int>::ConstIterator iter = map.begin(); iter != map.end(); ++iter )
    res.append( iter.key() );

  return res;
}

QString QtxResourceMgr::path( const QString& sect, const QString& prefix, const QString& name ) const
{
  QString res;
  for ( ResListIterator it( myResources ); it.current() && res.isEmpty(); ++it )
    res = it.current()->path( sect, prefix, name );
  return res;
}

QString QtxResourceMgr::resSection() const
{
  QString res = option( "res_section_name" );
  if ( res.isEmpty() )
    res = QString( "resources" );
  return res;
}

QString QtxResourceMgr::langSection() const
{
  QString res = option( "lang_section_name" );
  if ( res.isEmpty() )
    res = QString( "language" );
  return res;
}

QPixmap QtxResourceMgr::defaultPixmap() const
{
  return myDefaultPix;
}

void QtxResourceMgr::setDefaultPixmap( const QPixmap& pix )
{
  myDefaultPix = pix;
}

QPixmap QtxResourceMgr::loadPixmap( const QString& prefix, const QString& name ) const
{
  return loadPixmap( prefix, name, true );
}

QPixmap QtxResourceMgr::loadPixmap( const QString& prefix, const QString& name, const bool useDef ) const
{
  return loadPixmap( prefix, name, useDef ? defaultPixmap() : QPixmap() );
}

QPixmap QtxResourceMgr::loadPixmap( const QString& prefix, const QString& name, const QPixmap& defPix ) const
{
  initialize();

  QPixmap pix;
  for ( ResListIterator it( myResources ); it.current() && pix.isNull(); ++it )
    pix = it.current()->loadPixmap( resSection(), prefix, name );
  if ( pix.isNull() )
    pix = defPix;
  return pix;
}

void QtxResourceMgr::loadLanguage( const QString& pref, const QString& l )
{
  initialize();

  QMap<QChar, QString> substMap;
  substMap.insert( 'A', appName() );

  QString lang = l;
  if ( lang.isEmpty() )
    value( langSection(), "language", lang );

  if ( lang.isEmpty() )
  {
    lang = QString( "en" );
    qWarning( QString( "Language not specified. Assumed: %1" ).arg( lang ) );
  }

  substMap.insert( 'L', lang );

  QString trs;
  if ( value( langSection(), "translators", trs, false ) && !trs.isEmpty() )
  {
    QStringList translators    = QStringList::split( "|", option( "translators" ) );
    QStringList newTranslators = QStringList::split( "|", trs );
    for ( uint i = 0; i < newTranslators.count(); i++ )
      if ( translators.find( newTranslators[i] ) == translators.end() )
        translators += newTranslators[i];
    setOption( "translators", translators.join( "|" ) );
  }

  QStringList trList = QStringList::split( "|", option( "translators" ) );
  if ( trList.isEmpty() )
  {
    trList.append( "%P_msg_%L.qm" );
    qWarning( QString( "Translators not defined. Assumed: %1" ).arg( trList.first() ) );
  }

  QStringList prefixList;
  if ( !pref.isEmpty() )
    prefixList.append( pref );
  else
    prefixList = parameters( resSection() );

  for ( QStringList::const_iterator iter = prefixList.begin(); iter != prefixList.end(); ++iter )
  {
    QString prefix = *iter;
    substMap.insert( 'P', prefix );

    QStringList trs;
    for ( QStringList::const_iterator it = trList.begin(); it != trList.end(); ++it )
      trs.append( substMacro( *it, substMap ).stripWhiteSpace() );

    for ( QStringList::const_iterator itr = trs.begin(); itr != trs.end(); ++itr )
      loadTranslator( prefix, *itr );
  }
}

void QtxResourceMgr::loadTranslator( const QString& prefix, const QString& name )
{
  initialize();

  QTranslator* trans = 0;
  for ( ResListIterator it( myResources ); it.current() && !trans; ++it )
    trans = it.current()->loadTranslator( resSection(), prefix, name );

  if ( !trans )
    return;

  if ( !myTranslator[prefix].contains( trans ) )
    myTranslator[prefix].append( trans );
  qApp->installTranslator( trans );
}

void QtxResourceMgr::removeTranslators( const QString& prefix )
{
  if ( !myTranslator.contains( prefix ) )
    return;

  for ( TransListIterator it( myTranslator[prefix] ); it.current(); ++it )
  {
    qApp->removeTranslator( it.current() );
    delete it.current();
  }

  myTranslator.remove( prefix );
}

void QtxResourceMgr::raiseTranslators( const QString& prefix )
{
  if ( !myTranslator.contains( prefix ) )
    return;

  for ( TransListIterator it( myTranslator[prefix] ); it.current(); ++it )
  {
    qApp->removeTranslator( it.current() );
    qApp->installTranslator( it.current() );
  }
}

void QtxResourceMgr::refresh()
{
  QStringList sl = sections();
  for ( QStringList::const_iterator it = sl.begin(); it != sl.end(); ++it )
  {
    QStringList pl = parameters( *it );
    for ( QStringList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr )
      setValue( *it, *itr, stringValue( *it, *itr ) );
  }
}

QString QtxResourceMgr::userFileName( const QString& appName ) const
{
  QString fileName;
  QString pathName = QDir::homeDirPath();

#ifdef WIN32
  fileName = QString( "%1.%2" ).arg( appName ).arg( currentFormat() );
#else
  fileName = QString( ".%1rc" ).arg( appName );
#endif

  if ( !fileName.isEmpty() )
    pathName = Qtx::addSlash( pathName ) + fileName;

  return pathName;
}

QString QtxResourceMgr::globalFileName( const QString& appName ) const
{
  return QString( "%1.%2" ).arg( appName ).arg( currentFormat() );
}

QString QtxResourceMgr::substMacro( const QString& src, const QMap<QChar, QString>& substMap ) const
{
  QString trg = src;

  QRegExp rx( "%[A-Za-z%]" );

  int idx = 0;
  while ( ( idx = rx.search( trg, idx ) ) >= 0 )
  {
    QChar spec = trg.at( idx + 1 );
    QString subst;
    if ( spec == '%' )
      subst = "%";
    else if ( substMap.contains( spec ) )
      subst = substMap[spec];

    if ( !subst.isEmpty() )
    {
      trg.replace( idx, rx.matchedLength(), subst );
      idx += subst.length();
    }
    else
      idx += rx.matchedLength();
  }

  return trg;
}
