#26100
Mob del passato (Jerold)~
2 g 100
~
if %actor.level% < 6
wait 1 s
wecho &1&xNon c'e' passato per te.. torna quando sarai piu' grande e consapevole della tua vita.&0
wteleport %actor.name% 26100
elseif %actor.level% < 10
wload mob 2610%actor.level%
wforce passato hit %actor.name%
elseif %actor.level% < 71
wload mob 261%actor.level%
wforce passato hit %actor.name%
else
end
~
#26101
Morte del giovane (Jerold)~
0 f 100
~
'bravo continua il cammino
emote &2&xsi trasforma in polvere che viene spazzata via da un vento tiepido.&0
~
#26102
Teleport dal passato al presente (Jerold)~
2 d 0
bravo continua il cammino~
if %actor.name% == il giovane passato
wdoor 26102 west flags a
wdoor 26102 west room 2314755
wecho %all% &1&xUn vortice ti avvolge e tutto per un secondo sparisce appena apri gli occhi&0
wecho %all% &1&xil perscorso davanti a te e' libero e puoi proseguire.&0
else
end
~
#26103
Chiusura porta dal presente al passato (Jerold)~
2 g 100
~
wdoor 26102 west purge
~
#26104
Mob del Presente (Jerold)~
2 g 100
~
if %actor.level% < 6
wait 1 s
wecho &1&xNon so come sei gunto fino a qui ti ammonisco di non riprovarci.&0
wteleport %actor.name% 26100
elseif %actor.level% < 10
wload mob 2620%actor.level%
wforce presente hit %actor.name%
elseif %actor.level% < 71
wload mob 262%actor.level%
wforce presente hit %actor.name%
else
end
~
#26105
Morte mob del presente (Jerold)~
0 f 100
~
end
emote &2&xcome uno specchio si frantuma polverizzandosi letteralmente.&0
~
#26106
Apertura porta tra il presente e il futuro (Jerold)~
2 d 0
stringi i denti e prosegui~
if %actor.name% == l'adulto
wdoor 26103 south flags a
wdoor 26103 south room 2314962
wecho %all% &1&xUn vortice ti avvolge e tutto per un secondo sparisce appena apri gli occhi&0
wecho %all% &1&xil perscorso davanti a te e' libero e puoi proseguire.&0
else
end
~
#26107
Chiusura porta tra presente e futuro~
2 g 100
~
wdoor 26103 south purge
~
#26108
Mob del futuro (Jerold)~
2 g 100
~
if %actor.level% < 6
wait 1 s
wecho %actor.name% &1&xNon so come sei giunto fino a qui ma la tua punizione sara' l'eterno vagare&0
wteleport %actor.name% 666
elseif %actor.level% < 10
wload mob 2630%actor.level%
wforce futuro hit %actor.name%
elseif %actor.level% < 71
wload mob 263%actor.level%
wforce futuro hit %actor.name%
else
end
~
#26109
Morte mob del Futuro~
0 f 100
~
'complimenti finisci il tuo cammino
emote &2&xsi smaterializza come se non fosse mai esistito.&0
~
#26110
Apertura porta tra futuro e oracolo~
2 d 0
complimenti finisci il tuo cammino~
if %actor.name% == il vecchio
wdoor 26104 east flags a
wdoor 26104 east room 2314155
wecho %all% &1&xUn vortice ti avvolge e tutto per un secondo sparisce appena apri gli occhi&0
wecho %all% &1&xil perscorso davanti a te e' libero e puoi proseguire.&0
else
end
~
#26111
Chiusura porta tra futuro e oracolo~
2 g 100
~
wdoor 26104 east purge
~
#26112
Primo ponte del pentimento (JErold)~
2 g 100
~
if %actor.carry_weight% > 100
wsend %actor.name% Il ponte inizia a cigolare paurosamente...
else
end
~
#26113
Secondo ponte del pentimento (Jerold)~
2 g 100
~
if %actor.carry_weight% > 100
wecho %actor.name% &1&xNon sei stato abbastanza penitente... torna indietro&0
else
end
~
#26114
Terzo ponte del pentimento (JErold)~
2 g 100
~
if %actor.carry_weight% > 100
wecho %actor% &1&xSei troppo attaccato alle cose materiali per vedere l'oracolo&0
wsend %actor% &1&xSei troppo attaccato alle cose materiali per vedere l'oracolo&0
wteleport %actor% 26105
else
endif
~
$~
