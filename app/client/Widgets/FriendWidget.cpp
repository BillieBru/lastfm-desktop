
#include <QHBoxLayout>

#include <lastfm/RadioStation.h>

#include "lib/unicorn/widgets/AvatarWidget.h"
#include "lib/unicorn/widgets/Label.h"

#include "../Application.h"
#include "PlayableItemWidget.h"

#include "FriendWidget.h"



FriendWidget::FriendWidget( const lastfm::XmlQuery& user, QWidget* parent)
    :StylableWidget( parent ), m_user( user ), m_order( 0 - 1 )
{   
    m_recentTrack.setTitle( user["recenttrack"]["name"].text() );
    m_recentTrack.setAlbum( user["recenttrack"]["album"]["name"].text() );
    m_recentTrack.setArtist( user["recenttrack"]["artist"]["name"].text() );
    m_recentTrack.setExtra( "playerName", user["scrobblesource"]["name"].text() );
    m_recentTrack.setExtra( "playerURL", user["scrobblesource"]["url"].text() );

    QDateTime timestamp = QDateTime::fromString( user["recenttrack"].attribute( "date" ), "d MMM yyyy, hh:mm" );
    qDebug() << timestamp;
    m_recentTrack.setTimeStamp( timestamp );


    QHBoxLayout* layout = new QHBoxLayout( this );
    layout->setContentsMargins( 0, 0, 0, 0 );
    layout->setSpacing( 0 );

    layout->addWidget( ui.avatar = new AvatarWidget( this ) );
    ui.avatar->setObjectName( "avatar" );

    QRegExp re( "/serve/(\\d*)s?/" );

    ui.avatar->loadUrl( user["image size=medium"].text().replace( re, "/serve/\\1s/" ), HttpImageWidget::ScaleNone );
    ui.avatar->setHref( user["url"].text() );

    QVBoxLayout* vl = new QVBoxLayout();
    vl->setContentsMargins( 0, 0, 0, 0 );
    vl->setSpacing( 0 );
    layout->addLayout( vl, 1 );

    vl->addWidget( ui.name = new Label( "", this) );
    ui.name->setObjectName( "name" );
    vl->addWidget( ui.lastTrack = new Label( "", this) );
    ui.lastTrack->setObjectName( "lastTrack" );
    ui.lastTrack->setTextFormat( Qt::RichText );
    setDetails();

    vl->addWidget( ui.radio = new PlayableItemWidget( RadioStation::library( User( user["name"].text() ) ), tr( "%2%1s Library Radio" ).arg( QChar( 0x2019 ), user["name"].text() ) ) );
    ui.radio->setObjectName( "radio" );

    ui.avatar->setUser( m_user );
}


void
FriendWidget::update( const lastfm::XmlQuery& user, unsigned int order )
{
    m_order = order;

    m_recentTrack.setTitle( user["recenttrack"]["name"].text() );
    m_recentTrack.setAlbum( user["recenttrack"]["album"]["name"].text() );
    m_recentTrack.setArtist( user["recenttrack"]["artist"]["name"].text() );
    m_recentTrack.setExtra( "playerName", user["scrobblesource"]["name"].text() );
    m_recentTrack.setExtra( "playerURL", user["scrobblesource"]["url"].text() );

    QDateTime timestamp = QDateTime::fromString( user["recenttrack"].attribute( "date" ), "d MMM yyyy, hh:mm" );
    qWarning() << timestamp;
    m_recentTrack.setTimeStamp( timestamp );

    setDetails();
}

void
FriendWidget::setOrder( int order )
{
    m_order = order;
}

bool
FriendWidget::operator<( const FriendWidget& that ) const
{
    // sort by most recently listened and then by name

    if ( !this->m_recentTrack.timestamp().isNull() && that.m_recentTrack.timestamp().isNull() )
        return true;

    if ( this->m_recentTrack.timestamp().isNull() && !that.m_recentTrack.timestamp().isNull() )
        return false;

    if ( this->m_recentTrack.timestamp().isNull() && that.m_recentTrack.timestamp().isNull() )
        return this->name().toLower() < that.name().toLower();

    // both timestamps are valid!

    if ( this->m_recentTrack.timestamp() == that.m_recentTrack.timestamp() )
    {
        if ( this->m_order == that.m_order )
            return this->name().toLower() < that.name().toLower();

        return this->m_order < that.m_order;
    }

    // this is the other way around because a higher time means it's lower in the list
    return this->m_recentTrack.timestamp() > that.m_recentTrack.timestamp();
}


void
FriendWidget::setDetails()
{
    QString nameString = name();
    QString realnameString = realname();

    if ( !realnameString.isEmpty() )
        nameString += QString( " - %1" ).arg( realnameString );

    QString player = Label::anchor( m_recentTrack.extra( "playerURL" ), m_recentTrack.extra( "playerName" ) );
    QString track = Label::anchor( m_recentTrack.artist().www().toString(), m_recentTrack.artist().name() );
    track.append( QString( " " ) + QChar(8211) + " " );
    track.append( Label::anchor( m_recentTrack.www().toString(), m_recentTrack.title() ) );

    ui.name->setText( nameString );
    ui.lastTrack->setText( tr( "<p>Last track on %1</p><p>%2</p><p>%3</p>" ).arg( player, track, m_recentTrack.timestamp().toString( "d MMM yyyy, hh:mm" ) ) );
}

QString
FriendWidget::name() const
{
    return m_user.name();
}

QString
FriendWidget::realname() const
{
    return m_user.realName();
}
