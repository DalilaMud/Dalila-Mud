#12100
Sentinella covo Shade (Jerold)~
0 g 100
~
if %actor.vnum% == -1
wait 1 s
set target %actor.name%
global target
set allerta on
global allerta
set conta 0
global conta
emote sbuca dal nulla
Say &1&xNon muoverti o sei una persona morta! Parola d'ordine!&0
else
end
~
#12101
Sentinella Covo Shade (Jerold)~
0 ab 100
~
if %allerta% == on
if %conta% > 0
Say &1&xTe la sei cercata tu!&0
wait 1 s
set allerta off
global allerta
unset conta
mkill %target%
unset target
else
eval conta %conta%+1
global conta
end
else
end
~
#12102
Covo Shade 3 (Jerold)~
0 d 1
dictatum~
if %speech% == dictatum
if %actor.name% == %target%
set allerta off
global allerta
wait 2 s
Say &xBene! Puoi accedere alla roccaforte&0
unlock porticina e
open porticina e
wait 3 s
close porticina e
lock porticina e
unset conta
unset target
end
else
Say  &1&xNon l'ho chiesto a te!&0
end
~
#12103
Covo Shade 4 (Jerold)~
0 d 1
grandshade~
if %speech% == grandshade
if %actor.name% == %target%
set allerta off
global allerta
wait 2 s
Say &xBene! Puoi accedere alla cattedrale&0
unlock cancellata n
open cancellata n
wait 3 s
close cancellata n
lock cancellata n
unset conta
unset target
end
else
Say  &1&xNon l'ho chiesto a te!&0
end
~
#12104
Trappola 3~
2 g 100
~
wait 1 s
if %actor.vnum% == -1
eval num %random.5%
if %num% == 1
wecho &xCLICK!&0
elseif %num% == 3
wecho &xTik Tik Tik!&0
elseif %num% == 5
wecho Una tarantole cade dal soffitto
wload mob 12101
else end
end
else end
~
#12105
trappola4~
2 g 100
~
if %actor.vnum% == -1
set target %actor.name%
eval num %random.5%
if %num% == 1
wait 2
wsend %target% &xOH !OHHHHHHHHHHHHHHHH !!!&0
wteleport %target% 12128
wait 3
wdamage %target% 25
wsend %target% Sotto i tuoi piedi si apre una botola!
wecho %target% cade quando si spalanca una botola nel marmo
wait 3
wecho Lo vedi sparire mentre cade in una botola...
wforce %target% look
else end
end
else end
~
#12106
Porta Segreta (Jerold)~
2 d 1
Sael~
   if %actor.name% == Salomon
wait 1 s
wecho Un varco si apre nel muro di fronte a te
wteleport %actor.name% 12181
wait 1 s
wecho Il varco si chiude.
else
end
~
#12107
Cattedrale Shade 1 (Jerold)~
2 g 100
~
set target %actor.name%
global target
set allerta on
global allerta
set conta 0
global conta
end
~
#12108
Cattedrale Shade 2 (Jerold)~
2 ab 100
~
if %allerta% == on
if %conta% > 0
set allerta off
global allerta
unset conta
wteleport %target% 12111
wpurge %target%
unset target
else
eval conta %conta%+1
global conta
end
else
end
~
#12109
Cattedrale Shade 3 (Jerold)~
2 h 12107
~
if %object.vnum% == 12107
set allerta off
global allerta
unset conta
unset target
else
end
~
#12181
Covo Fenisce 1 (Jerold)~
0 g 100
~
if %actor.name% == garrick
global recita
wait 2 s
Say &1&xSalve Capo!&0
wait 1 s
mteleport garrick 598
wait 2 s
emote sbadiglia
wait 2 s
else
wait 1 s
say &1&xAltola'. E' meglio che tu mi dica la parola d'ordine!&0
set target %actor.name%
global target
set allerta on
global allerta
set conta 0
global conta
end
~
#12182
Covo Fenice 2 (Jerold)~
0 ab 100
~
if %allerta% == on
if %conta% > 0
say &1&xBhe! Te la sei cercata tu!&0
wait 1 s
set allerta off
global allerta
unset conta
mkill %target%
else
eval conta %conta%+1
global conta
else
end
end
~
#12183
Covo Fenice 3 (Jerold)~
0 d 1
anarchist~
if %speech% == anarchist
if %actor.name% == %target%
set allerta off
global allerta
wait 2 s
Say &3&xAh bene! Allora sei dei nostri!&0
mteleport %actor.name% 598
wait 3 s
unset conta
unset target
else
Say &1&xNon l'ho chiesto a te!&0
end
else
Say &1&xEhi mi prendi in giro? Stai attento!&0
end
~
$~
