#!/usr/bin/env python
# -*- coding: utf-8 -*-


def getPassword(user, server):
    from getpass import getpass

    assert isinstance(user, str)
    assert isinstance(server, str)

    prompt = 'Type password for %s FTP user on %s server: ' % (user, server)

    return getpass(prompt)

def main():
    from ftplib import FTP

    user = 'web.sir.ugu.pl'
    server = 'web.sir.ugu.pl'

    fileName = 'check_updates.php'

    ftpSession = FTP(server, user, getPassword(user, server))
    ftpSession.storlines('STOR %s' % fileName, open(fileName, 'rb'))
    ftpSession.quit()

    print('File %s was uploaded successfully.' % fileName)


main()
