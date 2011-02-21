/*
   Copyright 2005-2009 Last.fm Ltd.
      - Primarily authored by Max Howell, Jono Cole and Doug Mansell

   This file is part of the Last.fm Desktop Application Suite.

   lastfm-desktop is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   lastfm-desktop is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with lastfm-desktop.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QMenu>
#include <QMouseEvent>
#include <QTimer>
#include <QToolButton>

#include "lib/listener/PlayerConnection.h"
#include "lib/unicorn/dialogs/ShareDialog.h"
#include "lib/unicorn/dialogs/TagDialog.h"
#include "lib/unicorn/widgets/GhostWidget.h"
#include "lib/unicorn/widgets/HttpImageWidget.h"

#include "../Application.h"
#include "../StopWatch.h"
#include "../ScrobbleInfoFetcher.h"

#include "WelcomeWidget.h"
#include "ScrobbleInfoWidget.h"
#include "NowPlayingItem.h"

#include <lastfm/ws.h>


NowPlayingItem::NowPlayingItem( const Track& track )
    :TrackItem( track ),
     m_progressColor( 60, 60, 60 ),
     m_progressWidth( 0 )
{
    m_nullInfo = new WelcomeWidget( this );
    m_nullInfo->hide();

    connect( aApp, SIGNAL(paused(bool)), SLOT( onWatchPaused(bool)) );
    connect( aApp, SIGNAL(timeout()), SLOT( onWatchFinished()));
    connect( aApp, SIGNAL(frameChanged(int)), SLOT(onFrameChanged()));

    onWatchPaused( false );
}

void
NowPlayingItem::setTrack( const Track& track )
{
    doSetTrack( track );
    onWatchPaused( false );
}

QWidget*
NowPlayingItem::infoWidget() const
{
    if ( m_track == Track() )
        return m_nullInfo;

    return TrackItem::infoWidget();
}

void
NowPlayingItem::onWatchPaused( bool isPaused )
{
    if ( !m_track.isNull() )
    {
        if ( isPaused )
        {
            m_timestampText =tr( "%1 paused" ).arg( aApp->currentConnection()->name() );
            ui.as->setPixmap( QPixmap() );
        }
        else
        {
            m_timestampText = aApp->currentConnection()->name();
            ui.as->setMovie( movie.scrobbler_as );
            ui.as->movie()->start();
        }
        
        update();
    }
    else
    {
        m_timestampText = "";
        ui.as->clear();
    }
    resizeEvent( 0 );
}


void
NowPlayingItem::onWatchFinished()
{
    connect( ui.as->movie(), SIGNAL(loopFinished()), ui.as->movie(), SLOT(stop()));
    ui.timestamp->setText( tr( "Track Scrobbled" ));
}

void
NowPlayingItem::onFrameChanged()
{
    if( !aApp->stopWatch() )
        return;

    static int prevWidth = 0;

    float percentage = (aApp->stopWatch()->elapsed()/1000.0f) / aApp->stopWatch()->scrobblePoint();

    m_progressWidth = width() * percentage;

    if( m_progressWidth == prevWidth ) {
        return;
    }
    
    QRect r;
    if( m_progressWidth > prevWidth ) {
        r = QRect( prevWidth, 0, m_progressWidth - prevWidth, height());
    } else {
        r = rect();
    }
        
    prevWidth = m_progressWidth;

    update( r );
}

void
NowPlayingItem::paintEvent( QPaintEvent* event )
{
    StylableWidget::paintEvent( event );
    QPainter p( this );
    p.setPen( QColor( Qt::transparent ));
    p.setBrush( m_progressColor );
    QMargins margins = contentsMargins();
    p.drawRect( 0, margins.top(), m_progressWidth, height() - margins.top() - margins.bottom());
}

void NowPlayingItem::updateTimestamp()
{
    // Do nothing for the now playing track
    // its timestamp label is not used as a timestamp
}
