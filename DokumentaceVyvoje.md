# Externi Knihovny #
## QJson ##
Naprosto esencialni je precist si:
http://qjson.sourceforge.net/usage/ Jako doporuceny zpusob vyvoje je vytvaret v GameShared tridy a ty pak ObjectHelperem prevadet do QVariant a ten pouzit v knihovne komunikace.

# ProtocolLib #
Hlavni objekt teto knihovny je QJsonCommunication. Existuji dva zpusoby pouziti: Requesty a Notifikace
### Request ###
Ocekava se odpoved. Vola se metodou sendRequest, argumenty jsou pak identifikator requestu, QVariant data a callback pro odpoved. V handleru na druhe strane dostanete objekt QJsonRequest na kterem se da zavolat metoda sendResponse, to potom vede k vyvolani callbacku ze sendRequest. Je doporucene registrovat si callback na konrektni request pres metodu registerRequest, popr. ale lze i chytat signal request.
### Notifikace ###
Neocekava se odpoved, pouze jednosmerne poslana data. Vola se metodou sendNotification, lze registrovat callback pro urcitou notifikaci pres registerNotification, popr chytnou signal notification.
### Signaly ###
pro klienta je dulezity signal connected a socketError aka povedlo se pripojit nebo ne. Pro obe strany pak signal disconnected.