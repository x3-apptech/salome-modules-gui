#include "SUIT_ResourceMgr.h"

/*!
    Constructor
*/
SUIT_ResourceMgr::SUIT_ResourceMgr( const QString& app, const QString& resVarTemplate )
: QtxResourceMgr( app, resVarTemplate )
{
}

/*!
    Destructor
*/
SUIT_ResourceMgr::~SUIT_ResourceMgr()
{
}

/*!
    Returns the version of application
*/
QString SUIT_ResourceMgr::version() const
{
  return myVersion;
}

/*!
    Sets the version of application
*/
void SUIT_ResourceMgr::setVersion( const QString& ver )
{
  myVersion = ver;
}

/*!
    Loads a doc page from 'prefix' resources and indetified by 'id'
*/
QString SUIT_ResourceMgr::loadDoc( const QString& prefix, const QString& id ) const
{
  QString docSection = option( "doc_section_name" );
  if ( docSection.isEmpty() )
    docSection = QString( "docs" );

  return path( docSection, prefix, id );
}

/*!
    Returns the user file name for specified application
*/
QString SUIT_ResourceMgr::userFileName( const QString& appName ) const
{
  QString pathName = QtxResourceMgr::userFileName( appName );

  if ( !version().isEmpty() )
    pathName += QString( "." ) + version();

  return pathName;
}
