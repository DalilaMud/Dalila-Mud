#2400000
Apertura Chiusura Portale per Mini (Eliminabile)~
2 d 0
Apriti Portale~
unset lock
if %actor.name% == Feanor
set lock off
global lock
end
If %lock% == off
wait 2
wecho &1&xUn immenso boato poi sotto i tuo piedi si apre un tunnel magico&0
wdoor 1485420 down room 9845
else
end
~
#2400001
Apertura Chiusura Portale per Mini (Eliminabile)~
2 g 100
~
if %actor.clan% == 3 
wait 1 s
wteleport %actor.name% 2405050
wait 1 s
wforce %actor.name% look
else
wait 2 s
wecho &1&xHai una strana sensazione&0
wait 2 s
wteleport %actor.name% 1515480
wforce %actor.name% look
end
~
#2400002
Apertura Chiusura Portale per Mini (Eliminabile)~
2 d 0
Chiudi Portale~
unset lock
if %actor.name% == Feanor
set lock off
global lock
end
If %lock% == off
wait 2
wecho &1&xUn immenso boato poi sotto i tuo piedi piu' nulla&0
wdoor 1485420 down purge
else
end
~
$~
