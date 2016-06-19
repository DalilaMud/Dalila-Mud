#1000
Inizio AutoQuest (Offar)~
2 d 100
inizio~
mps %actor.name% 0
wecho Ok, pronto a rischiare la vita? Quando avrai PAURA, potrai scappare dicendo OUT!
wteleport %actor.name% 1001
~
#1010
Dati per Room Speciali (Offar)~
2 g 100
~
set PS %actor.questpoint%
global PS
set pgname %actor.name%
global pgname
~
#1011
Creazione Uscita Post-Evento (Offar)~
2 d 1
porta fine~
Nothing.
~
#1020
receive-greet all~
0 hj 100
~
say My trigger commandlist is not complete!
~
#1050
prova transform offar~
0 d 100
1 vai~
if %speech% == 1
set offar on
global offar
else
mtransform 1000
end
~
#1090
prova~
2 d 1
uno due tre quattro cinque~
set score 1
wecho punti %score%
global score
~
#1091
prova1~
0 e 0
va' verso~
say point!!
tell %actor.name% bastardo
say e' andato verso %arg%
~
#1092
purge uscita~
2 dg 100
porta aiuto~
wecho &x&1okokok&0
~
#1099
crea_uscita~
2 d 1
porta~
wait 1 s
eval num %random.4%
if %num% == 1
set exit north
global exit
elseif %num% == 2
set exit east
global exit
elseif %num% == 3
set exit south
global exit
else
set exit west
global exit
end
wecho  Senti una leggera brezza verso %exit% 
eval destroom %random.5% + 1000
global destroom
wdoor %actor.room% %exit% room %destroom%
wait 10 s
wdoor %actor.room% %exit% purge
~
$~
