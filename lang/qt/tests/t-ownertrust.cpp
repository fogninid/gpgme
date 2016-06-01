/* t-ownertrust.cpp

    This file is part of qgpgme, the Qt API binding for gpgme
    Copyright (c) 2016 Intevation GmbH

    QGpgME is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.

    QGpgME is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

    In addition, as a special exception, the copyright holders give
    permission to link the code of this program with any edition of
    the Qt library by Trolltech AS, Norway (or with modified versions
    of Qt that use the same license as Qt), and distribute linked
    combinations including the two.  You must obey the GNU General
    Public License in all respects for all of the code used other than
    Qt.  If you modify this file, you may extend this exception to
    your version of the file, but you are not obligated to do so.  If
    you do not wish to do so, delete this exception statement from
    your version.
*/

#include <QDebug>
#include <QTest>
#include <QSignalSpy>
#include "keylistjob.h"
#include "protocol.h"
#include "keylistresult.h"
#include "changeownertrustjob.h"

using namespace QGpgME;
using namespace GpgME;

class ChangeOwnerTrustTest: public QObject
{
    Q_OBJECT

Q_SIGNALS:
    void asyncDone();

private Q_SLOTS:

    void testChangeOwnerTrust()
    {
        KeyListJob *job = openpgp()->keyListJob(false, true, true);
        std::vector<GpgME::Key> keys;
        GpgME::KeyListResult result = job->exec(QStringList() << QStringLiteral("alfa@example.net"),
                                                false, keys);
        Q_ASSERT (!result.error());
        Q_ASSERT (keys.size() == 1);
        Key key = keys.front();
        Q_ASSERT (key.ownerTrust() == Key::Unknown);

        ChangeOwnerTrustJob *job2 = openpgp()->changeOwnerTrustJob();
        connect(job2, &ChangeOwnerTrustJob::result, this, [this](Error e)
        {
            if (e) {
                qDebug() <<  "Error in result: " << e.asString();
            }
            Q_ASSERT(!e);
            Q_EMIT asyncDone();
        });
        job2->start(key, Key::Ultimate);
        QSignalSpy spy (this, &ChangeOwnerTrustTest::asyncDone);
        Q_ASSERT(spy.wait());

        job = openpgp()->keyListJob(false, true, true);
        result = job->exec(QStringList() << QStringLiteral("alfa@example.net"),
                           false, keys);
        key = keys.front();
        Q_ASSERT (key.ownerTrust() == Key::Ultimate);

        ChangeOwnerTrustJob *job3 = openpgp()->changeOwnerTrustJob();
        connect(job3, &ChangeOwnerTrustJob::result, this, [this](Error e)
        {
            Q_ASSERT(!e);
            Q_EMIT asyncDone();
        });
        job3->start(key, Key::Unknown);
        Q_ASSERT(spy.wait());

        job = openpgp()->keyListJob(false, true, true);
        result = job->exec(QStringList() << QStringLiteral("alfa@example.net"),
                           false, keys);
        key = keys.front();
        Q_ASSERT (key.ownerTrust() == Key::Unknown);
    }

    void initTestCase()
    {
        const QString gpgHome = qgetenv("GNUPGHOME");
        QVERIFY2(!gpgHome.isEmpty(), "GNUPGHOME environment variable is not set.");
    }
};

QTEST_MAIN(ChangeOwnerTrustTest)

#include "t-ownertrust.moc"
