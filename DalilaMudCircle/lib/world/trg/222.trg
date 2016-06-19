#22201
Guardia della cattedrale~
0 g 100
~
if %direction% == south & %actor.rank% == 60
unset controllo
unset ospite
halt
end
if %direction% == south
unset ospite
halt
end
wait 5
switch %actor.rank%
case 60
emote scatta sull'attenti!
wait 2 s
say Agli ordini Maesta'!
set controllo 1
global controllo
break
default
set ospite %actor.name%
global ospite
if %controllo% == 1
wait 2 s
say Benvenuto %actor.name%
wait 2 s
say Ora il vostro arrivo sara' annunciato al Sommo.
mat 22232 mecho %actor.name% vuole essere ricevuto dal Sommo, dobbiamo ammetterlo alla Vostra presenza?
else
wait 2 s
say Il Sommo non e' presente al momento, siete pregato di allontanarvi.
wait 5
emote ti guarda con sguardo gelido... io fossi in te non la farei arrabbiare.
end
break
done
~
#22202
guardie della cattedrale Kill~
0 d 100
guardie aprite~
if %actor.rank% != 60
if %provocazione% == 0
wait 1 s
look %actor.name%
wait 1 s
say %actor.name% noi riceviamo ordini solo dal Sommo.
wait 1 s
say Vi consigliamo di non riprovare ad osare di darci ordini.
set provocazione 1
global provocazione
else
wait 1 s
say %actor.name%, siete gia' stato avvisato di non osare darci ordini...
wait 1 s
say Avete dieci secondi di tempo per andarvene, o saremo costretti a neutralizzarvi.
wait 1 s
emote Porta la mano alla spada e la sguaina
wait 9 s
unset provocazione
mkill %actor.name%
end
halt
end
if %speech% == guardie aprite
set controllo 1
global controllo
wait 5
pren chia
unlock porta
apri porta
wait 10 s
chiudi porta
lock porta
end
~
#22203
Dominio di Dolmant (accetta)~
2 c 100
accetta~
if %actor.rank% != 60
halt
end
%force% %actor.name% emote prende un campanello di cristallo dalla scrivania e lo fa squillare una volta
%force% gdd mat gcd mecho Il limpido squillo di un campanello trilla
wait 1 s
%force% gdd mat gcd mforce gcd sor
wait 2 s
%force% gdd mat gcd mforce gcd say Il Sommo %actor.name% vi attende, entrate pure %ospite%
wait 1 s
%force% gdd mat gcd mforce gcd unlock porta
%force% gdd mat gcd mforce gcd apri porta
wait 15 s
%force% gdd mat gcd mforce gcd chiudi porta
%force% gdd mat gcd mforce gcd lock porta
~
#22204
Guardia personale Dolmant~
0 g 100
~
if %actor.vnum% == 22206
fol self
mforce gdd fol self
wait 2 s
mforce gdd inch 
inch %actor.name%
wait 2 s
say A presto Sommo'
wait 1 s
mforce gdd s
s
set redolmant 0
global redolmant
halt
end
if %actor.rank% == 60
wait 1 s
mecho Le guardie scattano sull'attenti e vanno a posizionarsi alle spalle del Sommo
set redolmant 1
global redolmant
wait 3 s
follow %actor.name%
mforce gdd follow %actor.name%
halt
end
if redolmant == 1
halt
else
halt
wait 1 s
say Voi non potete stare qui in assenza del Sommo!
wait 1 s
emote sguaina la spada!
say %actor.name%, avete dieci secondi per andarvene, dopodiche' vi abbatteremo.
wait 10 s
mkill %actor.name%
end
~
#22205
Dominio di dolnat (aprite)~
2 c 100
aprite~
if %actor.rank% != 60
halt
end
wforce %actor.name% emote prende un campanello di cristallo dalla scrivania e lo fa trillare
wforce gdd mat gcd mforce gcd unlock porta
wforce gdd mat gcd mforce gcd apri porta
wait 20 s
wforce gdd mat gcd mforce gcd chiudi porta
wforce gdd mat gcd mforce gcd lock porta
~
$~
