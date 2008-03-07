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

QtxResourceMgr::Resources::Resources( const QtxResourceMgr* mgr, const QString& fileName )
: myFileName( fileName ),
  myMgr( const_cast<QtxResourceMgr*>( mgr ) )
{
}

/*!
  Destructor
*/
QtxResourceMgr::Resources::~Resources()
{
}

/*!
  Returns name of resource file
  This file is used to load/save operations
*/
QString QtxResourceMgr::Resources::file() const
{
  return myFileName;
}

/*!
  Sets name of resource file
  \param fn - name of file
*/
void QtxResourceMgr::Resources::setFile( const QString& fn )
{
  myFileName = fn;
}

/*!
  Returns string representation of parameter value
  Returns QString::null if there is no such parameter

  \param sect - name of section
  \param name - name of parameter
  \param subst - if it is true, then the substitution of variables
  will be done with help of makeSubstitution method
  \sa makeSubstitution()
*/
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

/*!
  Sets value by it's string representation

  \param sect - name of section
  \param name - name of parameter
  \param val - string value
*/
void QtxResourceMgr::Resources::setValue( const QString& sect, const QString& name, const QString& val )
{
  Section& s = section( sect );
  s.insert( name, val );
}

/*!
  \return true if section exists
  \param sect - name of section
*/
bool QtxResourceMgr::Resources::hasSection( const QString& sect ) const
{
  return mySections.contains( sect );
}

/*!
  \return true if parameter exists in section
  \param sect - name of section
  \param name - name of parameter
*/
bool QtxResourceMgr::Resources::hasValue( const QString& sect, const QString& name ) const
{
  return hasSection( sect ) && section( sect ).contains( name );
}

/*!
  Removes section from resources
  \param sect - name of section
*/
void QtxResourceMgr::Resources::removeSection( const QString& sect )
{
  mySections.remove( sect );
}

/*!
  Removes parameter from section
  \param sect - name of section
  \param name - name of parameter
*/
void QtxResourceMgr::Resources::removeValue( const QString& sect, const QString& name )
{
  if ( !hasSection( sect ) )
    return;

  Section& s = section( sect );
  s.remove( name );

  if ( s.isEmpty() )
    mySections.remove( sect );
}

/*!
  Removes all sections
*/
void QtxResourceMgr::Resources::clear()
{
  mySections.clear();
}

/*!
  \return list of section names
*/
QStringList QtxResourceMgr::Resources::sections() const
{
  return mySections.keys();
}

/*!
  \return list of parameter names from section
  \param sec - name of section
*/
QStringList QtxResourceMgr::Resources::parameters( const QString& sec ) const
{
  if ( !hasSection( sec ) )
    return QStringList();

  return section( sec ).keys();
}

/*!
  \return path of file from directory built by parameter
  \return QString::null if built path doesn't exist
  \param sec - name of section
  \param prefix - name of parameter containing some path
  \param name - name of file
*/
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

/*!
  \return corresponding resource manager
*/
QtxResourceMgr* QtxResourceMgr::Resources::resMgr() const
{
  return myMgr;
}

/*!
  \return instance of section by it's name. Section will be created if it doesn't exist
*/
QtxResourceMgr::Section& QtxResourceMgr::Resources::section( const QString& sn )
{
  if ( !mySections.contains( sn ) )
    mySections.insert( sn, Section() );

  return mySections[sn];
}

/*!
  \return instance of section by it's name. Section will be created if it doesn't exist
*/
const QtxResourceMgr::Section& QtxResourceMgr::Resources::section( const QString& sn ) const
{
  return mySections[sn];
}

/*!
  \return full path of file
  \param sect - name of section
  \param prefix - name of parameter containing some path
  \param name - name of file
*/
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
  if( !path.isEmpty() )
  {
    QString fname = QDir::convertSeparators( path );
    QFileInfo inf( fname );
    fname = inf.absFilePath();
    return fname;
  }
  return QString();
}

/*!
  \return QPixmap loaded from file
  \param sect - name of section
  \param prefix - name of parameter containing some path
  \param name - name of picture file
*/
QPixmap QtxResourceMgr::Resources::loadPixmap( const QString& sect, const QString& prefix, const QString& name ) const
{
  QString fname = fileName( sect, prefix, name );
  bool toCache = resMgr() ? resMgr()->isPixmapCached() : false;
  QPixmap p;
  if( toCache && myPixmapCache.contains( fname ) )
    p = myPixmapCache[fname];
  else
  {
    p.load( fname );
    if( toCache )
      ( ( QMap<QString,QPixmap>& )myPixmapCache ).insert( fname, p );
  }
  return p;
}

/*!
  \return just created and loaded translator
  \param sect - name of section
  \param prefix - name of parameter containing some path
  \param name - name of file
*/
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

/*!
  Finds in string variables by patterns: ${name} or $(name) or %name%
  \return first found name or QString::null if there is no ones
  \param str - string where the search is processed
  \param start - integer value for returning start position of variable
  \param len - integer value for returning length of variable
*/
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

/*!
  Substitutes variables by its' values. If variable is from enviroment,
  it will be replaced by environment value. If it isn't, method tries to
  find it's value among resources
  \return new variant of string 'str'
  \param str - string to process substitution
  \param sect - section, in which the variables will be finding
  \param name - name of variable which must be ignored during substitution
*/
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

/*!
  Default constructor
*/
QtxResourceMgr::IniFormat::IniFormat()
: Format( "ini" )
{
}

/*!
  Destructor
*/
QtxResourceMgr::IniFormat::~IniFormat()
{
}

/*!
  Loads resources from ini-file to map of sections
  \param fname - name of resource file
  \param secMap - map of sections
*/
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

    QRegExp rx( "^\\[([\\w\\s\\._]*)\\]$" );
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
      QString key = data.left( pos ).stripWhiteSpace();
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

/*!
  Saves map of sections to resource ini-file
  \param fname - name of resource file
  \param secMap - map of sections
*/
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

/*!
  Default constructor
*/
QtxResourceMgr::XmlFormat::XmlFormat()
: Format( "xml" )
{
}

/*!
  Destructor
*/
QtxResourceMgr::XmlFormat::~XmlFormat()
{
}

/*!
  Loads resources from xml-file to map of sections
  \param fname - name of resource file
  \param secMap - map of sections
*/
bool QtxResourceMgr::XmlFormat::load( const QString& fname, QMap<QString, Section>& secMap )
{
  bool res = false;

#ifndef QT_NO_DOM

  QFile file( fname );
  if ( !file.open( IO_ReadOnly ) )
  {
    qDebug( "File cannot be opened" );
    return false;
  }

  QDomDocument doc;

  res = doc.setContent( &file );
  file.close();

  if ( !res )
  {
    qDebug( "File is empty" );
    return false;
  }

  QDomElement root = doc.documentElement();
  if ( root.isNull() || root.tagName() != docTag() )
  {
    qDebug( "Invalid root" );
    return false;
  }

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
	    {
	      qDebug( "Invalid parameter element" );
              res = false;
	    }
          }
	  else
	  {
	    res = paramNode.isComment();
	    if( !res )
	      qDebug( "Node isn't element nor comment" );
	  }

          paramNode = paramNode.nextSibling();
        }
      }
      else
      {
	qDebug( "Invalid section" );
        res = false;
      }
    }
    else
    {
      res = sectNode.isComment(); // if it's a comment -- let it be, pass it..
      if( !res )
	qDebug( "Node isn't element nor comment" );
    }

    sectNode = sectNode.nextSibling();
  }

#endif

  if( res )
    qDebug( QString( "File '%1' is loaded successfully" ).arg( fname ) );
  return res;
}

/*!
  Saves map of sections to resource xml-file
  \param fname - name of resource file
  \param secMap - map of sections
*/
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
    for ( Section::ConstIterator iter = it.data().begin(); iter != it.data().end(); ++iter )
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

/*!
  \return XML tag of document
*/
QString QtxResourceMgr::XmlFormat::docTag() const
{
  QString tag = option( "doc_tag" );
  if ( tag.isEmpty() )
    tag = QString( "document" );
  return tag;
}

/*!
  \return XML tag of section
*/
QString QtxResourceMgr::XmlFormat::sectionTag() const
{
  QString tag = option( "section_tag" );
  if ( tag.isEmpty() )
    tag = QString( "section" );
  return tag;
}

/*!
  \return XML tag of parameter
*/
QString QtxResourceMgr::XmlFormat::parameterTag() const
{
  QString tag = option( "parameter_tag" );
  if ( tag.isEmpty() )
    tag = QString( "parameter" );
  return tag;
}

/*!
  \return XML attribute of parameter name
*/
QString QtxResourceMgr::XmlFormat::nameAttribute() const
{
  QString str = option( "name_attribute" );
  if ( str.isEmpty() )
    str = QString( "name" );
  return str;
}

/*!
  \return XML attribute of parameter value
*/
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

/*!
  \brief Constructs the format object with specified name.
  \param fmt - name of the format
*/
QtxResourceMgr::Format::Format( const QString& fmt )
: myFmt( fmt )
{
}

/*!
  \brief Destructs the format object.
*/
QtxResourceMgr::Format::~Format()
{
}

/*!
  \brief Returns the format name.
*/
QString QtxResourceMgr::Format::format() const
{
  return myFmt;
}

/*!
  \brief Returns the string list of the format options.
*/
QStringList QtxResourceMgr::Format::options() const
{
  return myOpt.keys();
}

/*!
  \brief Returns the value of the option with specified name.
         If option doesn't exist then empty string returned.
  \param opt - name of the option
*/
QString QtxResourceMgr::Format::option( const QString& opt ) const
{
  QString val;
  if ( myOpt.contains( opt ) )
    val = myOpt[opt];
  return val;
}

/*!
  \brief Sets the value of the option with specified name.
  \param opt - name of the option
  \param opt - value of the option
*/
void QtxResourceMgr::Format::setOption( const QString& opt, const QString& val )
{
  myOpt.insert( opt, val );
}

/*!
  \brief Perform the loading of the resources from resource file.
  \param res - resources object which will be loaded
*/
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

/*!
  \brief Perform the saving of the resources into resource file.
  \param res - resources object which will be saved
*/
bool QtxResourceMgr::Format::save( Resources* res )
{
  if ( !res )
    return false;

  Qtx::mkDir( Qtx::dir( res->myFileName ) );

  QtxResourceMgr* mgr = res->resMgr();
  QString name = mgr ? mgr->userFileName( mgr->appName(), false ) : res->myFileName;
  return save( name, res->mySections );
}

/*!
	Class: QtxResourceMgr
	Level: Public
*/

/*!
  \brief Constructs the resource manager object for application.
  \param appName - name of the application which resources will be used.
  \param resVarTemplate - template for the resource environment variable name which
                          should point to the resource directory list.
                          Default value is "%1Resources". Its mean that for application
                          with name "MyApp" environment variable "MyAppResources" will
                          be used. Template may not have the parameter '%1' substituted
                          by application name. In this case this string will be used as
                          is without substitution.
  Resource environment variable should contains one or several resource directories
  separated by symbol ';'. Resource directories list transfered into the setDirList().
  These directories and the user home directory used for the loading application resources.
  Each of the resource directories can contains resource file. The name of this file defined
  by the function globalFileName(). Resource file name in the user home defined by the
  function userFileName(). Any resource looking firstly in the user home resources then
  resource directories used in the specified order. All setted resources always stored into
  the resource file at the user home. Only user home resource file is saved.
  If you want to ignore of loading of Local User Preferences, you needs setup setIngoreUserValues()
  as true.
*/
QtxResourceMgr::QtxResourceMgr( const QString& appName, const QString& resVarTemplate )
: myAppName( appName ),
  myCheckExist( true ),
  myIsPixmapCached( true ),
  myIsIgnoreUserValues( false )
{
  QString envVar = !resVarTemplate.isEmpty() ? resVarTemplate : QString( "%1Resources" );
  if ( envVar.contains( "%1" ) )
    envVar = envVar.arg( appName );

  QString dirs;
  if ( ::getenv( envVar ) )
    dirs = ::getenv( envVar );
#ifdef WIN32
  QString dirsep = ";";      // for Windows: ";" is used as directories separator
#else
  QString dirsep = "[:|;]";  // for Linux: both ":" and ";" can be used
#endif
  setDirList( QStringList::split( QRegExp(dirsep), dirs ) );

  installFormat( new XmlFormat() );
  installFormat( new IniFormat() );

  setOption( "translators", QString( "%P_msg_%L.qm|%P_images.qm" ) );
}

/*!
  \brief Destructs the resource manager object and free allocated memory.
*/
QtxResourceMgr::~QtxResourceMgr()
{
  QStringList prefList = myTranslator.keys();
  for ( QStringList::const_iterator it = prefList.begin(); it != prefList.end(); ++it )
    removeTranslators( *it );
  for ( ResListIterator resIt( myResources ); resIt.current(); ++resIt )
    delete resIt.current();
  myResources.clear();
  for ( FormatListIterator formIt( myFormats ); formIt.current(); ++formIt )
    delete formIt.current();
}

/*!
  \brief Returns the application name.
*/
QString QtxResourceMgr::appName() const
{
  return myAppName;
}

/*!
  \brief Returns the checking of the existance flag. If its 'true' then resource
         will be setted into the manager only if it doesn't exist or has different
         value that existing value.
*/
bool QtxResourceMgr::checkExisting() const
{
  return myCheckExist;
}

/*!
  \brief Sets the checking of the existance flag.
  \param on - boolean value of the flag.
*/
void QtxResourceMgr::setCheckExisting( const bool on )
{
  myCheckExist = on;
}

/*!
  \brief Returns the resource directories list except user home directory.
*/
QStringList QtxResourceMgr::dirList() const
{
  return myDirList;
}

/*!
  \brief Initialise the manager. Prepare the resource containers and load resources.
  \param autoLoad - if 'true' then all resources will be loaded.
*/
void QtxResourceMgr::initialize( const bool autoLoad ) const
{
  if ( !myResources.isEmpty() )
    return;

  QtxResourceMgr* that = (QtxResourceMgr*)this;

  if ( !userFileName( appName() ).isEmpty() )
    that->myResources.append( new Resources( this, userFileName( appName() ) ) );

  for ( QStringList::const_iterator it = myDirList.begin(); it != myDirList.end(); ++it )
  {
    QString path = Qtx::addSlash( *it ) + globalFileName( appName() );
    that->myResources.append( new Resources( this, path ) );
  }

  if ( autoLoad )
    that->load();
}

/*!
  \brief Return true if all loaded pixmaps are stored in internal map; by default: true
*/
bool QtxResourceMgr::isPixmapCached() const
{
  return myIsPixmapCached;
}

/*!
  \brief Set true, if it is necessary to store all loaded pixmap in internal map
  (it accelerates following calls of loadPixmap)
*/
void QtxResourceMgr::setIsPixmapCached( const bool on )
{
  myIsPixmapCached = on;
}

/*!
  \brief Removes all resources from the manager.
*/
void QtxResourceMgr::clear()
{
  for ( ResListIterator it( myResources ); it.current(); ++it )
    it.current()->clear();
}

/*!
  Set state 'ignore user values'.
  If it is true, then all resources loaded from user home directory is ignored
*/
void QtxResourceMgr::setIgnoreUserValues( const bool val )
{
  myIsIgnoreUserValues = val;
}

/*!
  \return state 'ignore user values'
*/
bool QtxResourceMgr::ignoreUserValues() const
{
  return myIsIgnoreUserValues;
}

/*!
  \brief Get the resource value as integer. Returns 'true' if it successfull otherwise
         returns 'false'.
  \param sect - Resource section name which contains resource.
  \param name - Name of the resource.
  \param iVal - Reference on the variable which should contains the resource output.
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, int& iVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  bool ok;
  iVal = val.toInt( &ok );

  return ok;
}

/*!
  \brief Get the resource value as double. Returns 'true' if it successfull otherwise
         returns 'false'.
  \param sect - Resource section name which contains resource.
  \param name - Name of the resource.
  \param dVal - Reference on the variable which should contains the resource output.
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, double& dVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  bool ok;
  dVal = val.toDouble( &ok );

  return ok;
}

/*!
  \brief Get the resource value as boolean. Returns 'true' if it successfull otherwise
         returns 'false'.
  \param sect - Resource section name which contains resource.
  \param name - Name of the resource.
  \param bVal - Reference on the variable which should contains the resource output.
*/
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

/*!
  \brief Get the resource value as color. Returns 'true' if it successfull otherwise
         returns 'false'.
  \param sect - Resource section name which contains resource.
  \param name - Name of the resource.
  \param cVal - Reference on the variable which should contains the resource output.
*/
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

/*!
  \brief Get the resource value as font. Returns 'true' if it successfull otherwise
         returns 'false'.
  \param sect - Resource section name which contains resource.
  \param name - Name of the resource.
  \param fVal - Reference on the variable which should contains the resource output.
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, QFont& fVal ) const
{
  QString val;
  if ( !value( sect, name, val, true ) )
    return false;

  QStringList fontDescr = QStringList::split( ",", val );

  if ( fontDescr.count() < 2 )
    return false;

  QString family = fontDescr[0];
  if ( family.isEmpty() )
    return false;

  fVal = QFont( family );

  for ( int i = 1; i < (int)fontDescr.count(); i++ )
  {
    QString curval = fontDescr[i].stripWhiteSpace().lower();
    if ( curval == QString( "bold" ) )
      fVal.setBold( true );
    else if ( curval == QString( "italic" ) )
      fVal.setItalic( true );
    else if ( curval == QString( "underline" ) )
      fVal.setUnderline( true );
    else
    {
      bool isOk = false;
      int ps = curval.toInt( &isOk );
      if ( isOk )
        fVal.setPointSize( ps );
    }
  }

  return true;
}

/*!
  \brief Get the resource value as string (native format). Returns 'true' if it
         successfull otherwise returns 'false'.
  \param sect  - Resource section name which contains resource.
  \param name  - Name of the resource.
  \param val   - Reference on the variable which should contains the resource output.
  \param subst - If 'true' then manager substitute reference on environment variables
                 and other resources by thier values. Default value of this parameter
                 is 'true'
*/
bool QtxResourceMgr::value( const QString& sect, const QString& name, QString& val, const bool subst ) const
{
  initialize();

  bool ok = false;
 
  ResListIterator it( myResources );
  if ( ignoreUserValues() )
    ++it;

  for ( ; it.current() && !ok; ++it )
  {
    ok = it.current()->hasValue( sect, name );
    if ( ok )
      val = it.current()->value( sect, name, subst );
  }

  return ok;
}

/*!
  \brief Returns the integer resource value. If resource can not be found or converted
         then specified default value will be returned.
  \param sect  - Resource section name which contains resource.
  \param name  - Name of the resource.
  \param def   - Default resource value which will be used when resource not found.
*/
int QtxResourceMgr::integerValue( const QString& sect, const QString& name, const int def ) const
{
  int val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Returns the double resource value. If resource can not be found or converted
         then specified default value will be returned.
  \param sect  - Resource section name which contains resource.
  \param name  - Name of the resource.
  \param def   - Default resource value which will be used when resource not found.
*/
double QtxResourceMgr::doubleValue( const QString& sect, const QString& name, const double def ) const
{
  double val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Returns the boolean resource value. If resource can not be found or converted
         then specified default value will be returned.
  \param sect  - Resource section name which contains resource.
  \param name  - Name of the resource.
  \param def   - Default resource value which will be used when resource not found.
*/
bool QtxResourceMgr::booleanValue( const QString& sect, const QString& name, const bool def ) const
{
  bool val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Returns the font resource value. If resource can not be found or converted
         then specified default value will be returned.
  \param sect  - Resource section name which contains resource.
  \param name  - Name of the resource.
  \param def   - Default resource value which will be used when resource not found.
*/
QFont QtxResourceMgr::fontValue( const QString& sect, const QString& name, const QFont& def ) const
{
  QFont font;
  if( !value( sect, name, font ) )
    font = def;
  return font;
}

/*!
  \brief Returns the color resource value. If resource can not be found or converted
         then specified default value will be returned.
  \param sect  - Resource section name which contains resource.
  \param name  - Name of the resource.
  \param def   - Default resource value which will be used when resource not found.
*/
QColor QtxResourceMgr::colorValue( const QString& sect, const QString& name, const QColor& def ) const
{
  QColor val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Returns the string resource value. If resource can not be found or converted
         then specified default value will be returned.
  \param sect  - Resource section name which contains resource.
  \param name  - Name of the resource.
  \param def   - Default resource value which will be used when resource not found.
*/
QString QtxResourceMgr::stringValue( const QString& sect, const QString& name, const QString& def ) const
{
  QString val;
  if ( !value( sect, name, val ) )
    val = def;
  return val;
}

/*!
  \brief Checks existance of the specified resource.
  \param sect  - Resource section name which contains resource.
  \param name  - Name of the resource.
*/
bool QtxResourceMgr::hasValue( const QString& sect, const QString& name ) const
{
  initialize();

  bool ok = false;
  for ( ResListIterator it( myResources ); it.current() && !ok; ++it )
    ok = it.current()->hasValue( sect, name );

  return ok;
}

/*!
  \brief Checks existance of the specified resource section.
  \param sect  - Resource section name which contains resource.
*/
bool QtxResourceMgr::hasSection( const QString& sect ) const
{
  initialize();

  bool ok = false;
  for ( ResListIterator it( myResources ); it.current() && !ok; ++it )
    ok = it.current()->hasSection( sect );

  return ok;
}

/*!
  \brief Sets the integer resource value.
  \param sect  - Resource section name.
  \param name  - Name of the resource.
  \param val   - Resource value.
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, int val )
{
  int res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, QString::number( val ) );
}

/*!
  \brief Sets the double resource value.
  \param sect  - Resource section name.
  \param name  - Name of the resource.
  \param val   - Resource value.
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, double val )
{
  double res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, QString::number( val, 'g', 12 ) );
}

/*!
  \brief Sets the boolean resource value.
  \param sect  - Resource section name.
  \param name  - Name of the resource.
  \param val   - Resource value.
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, bool val )
{
  bool res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, QString( val ? "true" : "false" ) );
}

/*!
  \brief Sets the color resource value.
  \param sect  - Resource section name.
  \param name  - Name of the resource.
  \param val   - Resource value.
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QColor& val )
{
  QColor res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, QString( "%1, %2, %3" ).arg( val.red() ).arg( val.green() ).arg( val.blue() ) );
}

/*!
  \brief Sets the font resource value.
  \param sect  - Resource section name.
  \param name  - Name of the resource.
  \param val   - Resource value.
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QFont& val )
{
  QFont res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  QStringList fontDescr;
  fontDescr.append( val.family() );
  if ( val.bold() )
    fontDescr.append( "Bold" );
  if ( val.italic() )
    fontDescr.append( "Italic" );
  if ( val.underline() )
    fontDescr.append( "Underline" );
  fontDescr.append( QString( "%1" ).arg( val.pointSize() ) );

  setResource( sect, name, fontDescr.join( "," ) );
}

/*!
  \brief Sets the string resource value.
  \param sect  - Resource section name.
  \param name  - Name of the resource.
  \param val   - Resource value.
*/
void QtxResourceMgr::setValue( const QString& sect, const QString& name, const QString& val )
{
  QString res;
  if ( checkExisting() && value( sect, name, res ) && res == val )
    return;

  setResource( sect, name, val );
}

/*!
  \brief Remove the all specified resource section.
  \param sect  - Resource section name.
*/
void QtxResourceMgr::remove( const QString& sect )
{
  initialize();

  for ( ResListIterator it( myResources ); it.current(); ++it )
    it.current()->removeSection( sect );
}

/*!
  \brief Remove the specified resource.
  \param sect  - Resource section name.
  \param name  - Name of the resource.
*/
void QtxResourceMgr::remove( const QString& sect, const QString& name )
{
  initialize();

  for ( ResListIterator it( myResources ); it.current(); ++it )
    it.current()->removeValue( sect, name );
}

/*!
  \brief Returns the current format which operates with resource files.
*/
QString QtxResourceMgr::currentFormat() const
{
  QString fmt;
  if ( !myFormats.isEmpty() )
    fmt = myFormats.getFirst()->format();
  return fmt;
}

/*!
  \brief Sets the current format which operates with resource files.
  \param fmt - Resource format name.
*/
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
  if ( myResources.count() > myDirList.count() && resIt.current() ) {
    resIt.current()->setFile( userFileName( appName() ) );
    ++resIt;
  }

  for ( QStringList::const_iterator it = myDirList.begin(); it != myDirList.end() && resIt.current(); ++it, ++resIt )
    resIt.current()->setFile( Qtx::addSlash( *it ) + globalFileName( appName() ) );
}

/*!
  \brief Returns the resource format object by it name.
  \param fmt - Resource format name.
*/
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

/*!
  \brief Add the resource format to the manager. Newly added become current.
  \param form - Resource format object.
*/
void QtxResourceMgr::installFormat( QtxResourceMgr::Format* form )
{
  if ( !myFormats.contains( form ) )
    myFormats.prepend( form );
}

/*!
  \brief Remove the resource format from the manager.
  \param form - Resource format object.
*/
void QtxResourceMgr::removeFormat( QtxResourceMgr::Format* form )
{
  myFormats.remove( form );
}

/*!
  \brief Returns the string list of the resource format options names.
*/
QStringList QtxResourceMgr::options() const
{
  return myOptions.keys();
}

/*!
  \brief Returns the string value for the specified option. If option doesn't exist
         then empty string will be returned.
  \param opt - Option name.
*/
QString QtxResourceMgr::option( const QString& opt ) const
{
  QString val;
  if ( myOptions.contains( opt ) )
    val = myOptions[opt];
  return val;
}

/*!
  \brief Sets the string value for the specified option.
  \param opt - Option name.
  \param val - Option value.
*/
void QtxResourceMgr::setOption( const QString& opt, const QString& val )
{
  myOptions.insert( opt, val );
}

/*!
  \brief Load the all resources from the resource files.
*/
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

/*!
  \brief Import some file with resources
*/
bool QtxResourceMgr::import( const QString& fname )
{
  Format* fmt = format( currentFormat() );
  if ( !fmt )
    return false;

  Resources* r = myResources.getFirst();
  if( !r )
    return false;

  QString old = r->file();
  r->setFile( fname );
  bool res = fmt->load( r );
  r->setFile( old );
  return res;
}

/*!
  \brief Save the changed resources in to the user resource file.
*/
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

/*!
  \brief Returns the string list of the existing section names..
*/
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

/*!
  \brief Returns the string list of the existing resource names in the specified section.
  \param sec - Resource section name.
*/
QStringList QtxResourceMgr::parameters( const QString& sec ) const
{
  initialize();

#if defined(QTX_NO_INDEXED_MAP)
  typedef QMap<QString, int> PMap;
#else
  typedef IMap<QString, int> PMap;
#endif
  PMap pmap;
  ResListIterator it( myResources );
  it.toLast();
  for ( ; it.current(); --it ) {
    QStringList lst = it.current()->parameters( sec );
    for ( QStringList::const_iterator itr = lst.begin(); itr != lst.end(); ++itr )
      pmap.insert( *itr, 0, false );
  }

  QStringList res;
  for ( PMap::ConstIterator iter = pmap.begin(); iter != pmap.end(); ++iter )
    res.append( iter.key() );

  return res;
}

/*!
  \return path of file from directory built by parameter
  \return QString::null if built path doesn't exist
  \param sec - name of section
  \param prefix - name of parameter containing some path
  \param name - name of file
*/
QString QtxResourceMgr::path( const QString& sect, const QString& prefix, const QString& name ) const
{
  QString res;
  for ( ResListIterator it( myResources ); it.current() && res.isEmpty(); ++it )
    res = it.current()->path( sect, prefix, name );
  return res;
}

/*!
  \return section corresponding to resources paths
*/
QString QtxResourceMgr::resSection() const
{
  QString res = option( "res_section_name" );
  if ( res.isEmpty() )
    res = QString( "resources" );
  return res;
}

/*!
  \return section corresponding to language settings
*/
QString QtxResourceMgr::langSection() const
{
  QString res = option( "lang_section_name" );
  if ( res.isEmpty() )
    res = QString( "language" );
  return res;
}

/*!
  \return default image used when during loading the image file doesn't exist
*/
QPixmap QtxResourceMgr::defaultPixmap() const
{
  return myDefaultPix;
}

/*!
  Set image as default image used when during loading the image file doesn't exist
  \param pix - image
*/
void QtxResourceMgr::setDefaultPixmap( const QPixmap& pix )
{
  myDefaultPix = pix;
}

/*!
  \return image loaded from file 
  \param prefix - name of parameter containing some path
  \param name - name of file
*/
QPixmap QtxResourceMgr::loadPixmap( const QString& prefix, const QString& name ) const
{
  return loadPixmap( prefix, name, true );
}

/*!
  \return image loaded from file 
  \param prefix - name of parameter containing some path
  \param name - name of file
  \param useDef - indicates if it is possible to use default image returning by defaultPixmap() method.
  If it is false, the empty pixmap will be used as default
  \sa defaultPixmap()
*/
QPixmap QtxResourceMgr::loadPixmap( const QString& prefix, const QString& name, const bool useDef ) const
{
  return loadPixmap( prefix, name, useDef ? defaultPixmap() : QPixmap() );
}

/*!
  Finds in all sections an existing path corresponding to 'prefix' parameter
  and load image with name 'name' from this folder

  \return image loaded from file 
  \param prefix - name of parameter containing some path
  \param name - name of file
  \param defPix - default image used when file doesn't exist
*/
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

/*!
  Loads translator for language
  Name of translator file is constructed by list returning by option "translators" or,
  if it is empty, by predefined pattern "%P_msg_%L.qm". It is recommended to used in translators
  name the strings %A, %P, %L whose will be replaced by application name, prefix and language name correspondingly

  \param pref - name of parameter containing path to translator's file.
  If it is empty, the list of parameters from resource section ( resSection() )
  is used.

  \param l - name of language. If it is empty, then value of parameter "language"
  from language section ( langSection() ) is used. If it is also empty, then
  predefined name "en" is used

  \sa resSection(), langSection()
*/
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

    loadTranslators( prefix, trs );
  }
}

/*!
  Loads translators by path and list of files

  \param prefix - value of this parameter must contain path
  \param translators - list of translators' files 
*/
void QtxResourceMgr::loadTranslators( const QString& prefix, const QStringList& translators )
{
  initialize();

  QTranslator* trans = 0;
  ResListIterator it( myResources );
  it.toLast();
  for ( ; it.current(); --it )
  {
    for ( QStringList::const_iterator itr = translators.begin(); itr != translators.end(); ++itr )
    {
      trans = it.current()->loadTranslator( resSection(), prefix, *itr );
      if ( trans )
      {
        if ( !myTranslator[prefix].contains( trans ) )
          myTranslator[prefix].append( trans );
        qApp->installTranslator( trans );
      }
    }
  }
}

/*!
  Loads translator by path and file name

  \param prefix - value of this parameter must contain path
  \param name - name of translator file
*/
void QtxResourceMgr::loadTranslator( const QString& prefix, const QString& name )
{
  initialize();

  QTranslator* trans = 0;
  ResListIterator it( myResources );
  it.toLast();
  for ( ; it.current(); --it )
  {
    trans = it.current()->loadTranslator( resSection(), prefix, name );
    if ( trans )
    {
      if ( !myTranslator[prefix].contains( trans ) )
        myTranslator[prefix].append( trans );
      qApp->installTranslator( trans );
    }
  }
}

/*!
  Remove all translators corresponding to prefix

  \param prefix - parameter containing path
*/
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

/*!
  Moves translators corresponding to prefix to the top of translator stack 

  \param prefix - parameter containing path
*/
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

/*!
  Copies all resources to user resources, so that they will be saved in user home folder
*/
void QtxResourceMgr::refresh()
{
  QStringList sl = sections();
  for ( QStringList::const_iterator it = sl.begin(); it != sl.end(); ++it )
  {
    QStringList pl = parameters( *it );
    for ( QStringList::const_iterator itr = pl.begin(); itr != pl.end(); ++itr )
      setResource( *it, *itr, stringValue( *it, *itr ) );
  }
}

/*!
  \brief Sets the resource directories list except user home directory and clear resources
*/
void QtxResourceMgr::setDirList( const QStringList& dl )
{
  myDirList = dl;
  for ( ResListIterator it( myResources ); it.current(); ++it )
    delete it.current();

  myResources.clear();
}

/*!
  Sets resource value
  \param sect - name of section
  \param name - name of parameter
  \param val - string representation of value
*/
void QtxResourceMgr::setResource( const QString& sect, const QString& name, const QString& val )
{
  initialize();

  if ( !myResources.isEmpty() )
    myResources.first()->setValue( sect, name, val );
}

/*!
  \return name of resource file, which is being found in user home directory
  \param appName - name of application
  \param for_load - flag indicating that file will be used for loading (true) or for saving(false)
  It makes possible to use different resource files for loading and saving
*/
QString QtxResourceMgr::userFileName( const QString& appName, const bool /*for_load*/ ) const
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

/*!
  \return name of resource file, which is being found in all resource directories, except user home
*/
QString QtxResourceMgr::globalFileName( const QString& appName ) const
{
  return QString( "%1.%2" ).arg( appName ).arg( currentFormat() );
}

/*!
  Replaced substrings by pattern %A, %B, etc by values from map

  \param src - string to be processed
  \param substMap - map of values for replacing
*/
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
