/***************************************************************************
*   Copyright 2005-2008 Last.fm Ltd.                                      *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.         *
***************************************************************************/

#ifndef RQL_QUERY_THREAD_H
#define RQL_QUERY_THREAD_H

#include "TRequestThread.h"
#include "SimilarArtists.h"


class QueryRunnable
{
public:
    virtual ~QueryRunnable() {}
    virtual void doRequest(class RqlQueryThread*) = 0;
    virtual void abort() = 0;
};


class RqlQueryThread : 
    public TRequestThread<QueryRunnable>, 
    public TCreateThreadMixin<RqlQueryThread>
{
    class ParseRunnable : public QueryRunnable
    {
        QString m_rql;
        class ILocalRqlParseCallback *m_cb;

    public:
        ParseRunnable(QString, ILocalRqlParseCallback *);
        virtual void doRequest(class RqlQueryThread*);
        virtual void abort();
    };

    class NextTrackRunnable : public QueryRunnable
    {
        class ILocalRqlTrackCallback *m_cb;
        class RqlQuery *m_src;

    public:
        NextTrackRunnable(class RqlQuery*, ILocalRqlTrackCallback *);
        virtual void doRequest(class RqlQueryThread*);
        virtual void abort();
    };


    class LocalCollection* m_pCollection;
    SimilarArtists m_sa;

    void parse(QString, ILocalRqlParseCallback *);
    void nextTrack(class RqlQuery*, ILocalRqlTrackCallback*);

protected:
    virtual void run();
    virtual void doRequest(QueryRunnable *);

public:
    void enqueueParse(const char* sRql, ILocalRqlParseCallback *);
    void enqueueGetNextTrack(class RqlQuery*, ILocalRqlTrackCallback*);
};


#endif