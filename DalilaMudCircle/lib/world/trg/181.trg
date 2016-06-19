#18100
levrien~
0 d 1
levrien~
wait 1 s
emote aggrotta le sopracciglia e cerca di ricordare
wait 1 s 
say Non ricordo molto bene, credo che un levrien corrisponda a 4 unita' marine.
wait 2 s
emote sembra avere la gola secca
say Dammi 5 coin!
wait 3 s
say Voglio bere
wait 1 s
emote allunga la mano verso di te
~
#18101
Verita~
0 m 5
~
if %amount% > 4
wait 1 s
say Grazie, %actor.name%
wait 1 s
say Ora ricordo bene sono 6 unita' marine.
wait 1 s
say E ora lasciatemi in pace!
end
~
#18102
Gorgo~
2 d 1
atlantide~
wait 5 s
wsend %actor% Ad un tratto il mare calmo comincia ad agitarsi
wait 1 s
wsend %actor% Sotto di te si forma un gorgo che di inghiotte improvvisamente.....
wait 1 s
wteleport %actor% 18100
wait 1 s
wsend %actor% Ti trovi all'interno del gorgo.
~
#18104
Caserma Elenia (Khud)~
2 g 100
~
if %direction% == south
set target %actor.name%
if %actor.name% == yaemon
wait 4
wforce guardia inchina %actor.name%
wait 4
wforce guardia say &3Salve Generale !!! E' tutto sotto controllo.&0
elseif %actor.name% == dolmant
wait 4
wforce guardia inchina %actor.name%
wait 4
wforce guardia say &3Salve Sire !!! Tutto procedede senza problemi.&0
elseif %actor.name% == xenon
wait 4
wforce guardia inchina %actor.name%
wait 4
wforce guardia say &3Salve Generale !!! E' tutto sotto controllo.&0
else
wait 4
wecho &3Entri nella grande Caserma del Regno Eleno...&0
wait 4
wecho &3e rimani stupefatto dalla potenza delle armi che vedi in giro !&0
wait 4
wecho &3attento a cio' che fai, le guardie hanno ricevuto ordini precisi&0
wait 4
wecho &1e sai che il Generale non perdona i ladri !&0
wait 4
wforce guardia say &1Salve %actor.name%, Il Generale ci aveva avvertito del suo arrivo.&0
end
end
end
end
~
#18105
Herjan , ristorante Rosa Dei Venti (Khud)~
0 g 100
~
wait 12
smile %actor.name%
wait 6
emote da una pacca sulla spalla al suo giovane figlio
wait 12
say Salve %actor.name% , si sieda pure e consumi il suo pasto, senza badare ai nostri discorsi se cio' la infastidisce.
wait 12
emote inizia a parlare a bassa voce
say e cosi' figlio, hai appena sentito la storia di Ghotran il troll deforme
wait 12
say nonostante il tradimento della sua tribu' egli rimase fedele agli uomini
wait 12
say che lo valutavano solo dal suo aspetto.
wait 12
say Ma alla fine fu proprio grazie a lui che Elenia vinse la guerra.
wait 12
say non scordero' mai quel piccolo troll !
wait 12
say ricorda figlio mio, non giudicare nessuno se non dalle sue azioni.
wait 12
emote riprende a mangiare soddisfatto
wait 12
look %actor.name%
wait 6
say Buon Appetito !
~
#18106
Il Ponte Dei Coboldi (Khud)~
2 g 100
~
if %actor.vnum% == -1
set target %actor.name%
eval num %random.5%
if %num% == 1
wait 2
wsend %target% scivoli e CADI !!!
wteleport %target% 18330
wait 3
wdamage %target% 20
wsend %target% CHE MALE !!!
wecho %target% scivola e CADE !!!!
wait 3
wecho Lo vedi sparire mentre cade nel burrone...
wforce %target% look
else end
end
else end
~
#18107
Gioco delle 5 carte - parte I (Khud)~
0 g 100
~
if %actor.vnum% == -1
wait 6
say Ehila avventuriero !
wait 6
say mi sembri uno in gamba, che ne dici di fare una scommessa ?
wait 6
say vedi queste 5 carte ? la prima e' lo stemma di Elenia, se riesci a riconoscerla dopo che le avro' mischiate ti daro' un premio !
wait 6
say eddai ! ti faccio giocare con appena 50 monete !
else end
end
~
#18108
Gioco delle 5 carte - parte II (Khud)~
0 m 1
~
if %amount% < 50
wait 3
say mi spiace, troppi pochi soldi !
give %amount% coin %actor.name%
elseif %amount% > 50
wait 3
say no guarda, non gioco con poste cosi' alte, solo 50 coin
give %amount% coin %actor.name%
elseif %amount% == 50
junk 50 coin
wait 2
say Bene bene, guarda attentamente !!!
wait 3
emote ti mostra che la carta centrale ha lo stemma di Elenia
wait 6
say e ora.... attento !!!
wait 3
emote mischia le carte con maestria, molto velocemente !!!
wait 2
wait 3
eval numy %random.5%
wait 3
say Ok.... dove sta la carta ?
wait 2
emote sorride scoprendo la carta numero %numy% ... pare essere quella !!!
set giocata on
global giocata
else
end
end
end
end
~
#18109
Gioco delle 5 carte - parte III (Khud)~
0 d 1
1 2 3 4 5~
eval num %random.5%
if %giocata% == on 
if %speech% == 1 && %num% == 1
say hai vinto !!! che occhio veloce !!!
mload obj 21499
drop premio
mforce %actor.name% get premio
unset giocata
elseif %speech% == 2 && %num% == 2
say hai vinto !!! che occhio veloce !!!
mload obj 21499
drop premio
mforce %actor.name% get premio
unset giocata
elseif %speech% == 3 && %num% == 3
say hai vinto !!! che occhio veloce !!!
mload obj 21499
drop premio
mforce %actor.name% get premio
unset giocata
elseif %speech% == 4 && %num% == 4
say hai vinto !!! che occhio veloce !!!
mload obj 21499
drop premio
mforce %actor.name% get premio
unset giocata
elseif %speech% == 5 && %num% == 5
say hai vinto !!! che occhio veloce !!!
mload obj 21499
drop premio
mforce %actor.name% get premio
unset giocata
else
say hai perso !!! ahahah sono troppo abile !
wait 3
say mi terro' i tuoi soldi
grin
unset giocata
end
end
else end
~
#18110
La Morte di TYRANO (Khud)~
0 f 100
~
emote &1&xvacilla e infine cade spruzzando sangue ovunque, con un ultimo ruggito pare rialzarsi feroce... ma poco dopo crolla a terra senza vita !&0
emote &1&xLa piu' potente delle creature... e' morta ! Complimenti avventuriero !&0
~
#18111
Allarme Coboldi (Khud)~
2 g 100
~
if %actor.vnum% == -1
if %direction% == west
wait 2
wecho Appena entri qualcuno si accorge di te, immediatamente senti un urlo...
wait 2
wecho &1&x ALLARME !!! &0&1 Gli umani ci attaccano, presto armatevi ! &0
end
else end
end
~
#18112
La morte di Djano (Khud)~
0 f 100
~
say Maledetti umani... mi avete battuto !
say Vi aspetto all' inferno !!! Ahahahahah
emote tenta di colpirti con un ultimo sforzo, ma non ha fortuna. Cade a terra morto.
~
#18113
Guardiano di Ferus --- TRIGGER 1~
0 g 100
~
if %actor.vnum% == -1
if %direction% != north
if %actor.name% != Ferus
wait 2
say Salve %actor.name%, benvenuto al Tempio del Dio Ferus
wait 2
say Vuole attraversare il Portale per entrare nel Tempio?
else if %actor.name% == Ferus
wait 2
inchina Ferus
wait 2
say Onore a Voi, mia Divinita' !
endif
endif
end
~
#18114
Guardiano di Ferus --- TRIGGER 2~
2 d 100
si no~
if %speech% == si
wait 2
wsend %actor% La Sacerdotessa Guardiana ti accompagna attraverso il Portale Spazio-Temporale...
wait 5
wteleport %actor% 60
wforce %actor% look
else
if %speech% == no
wait 2
wsend %actor% La Sacerdotessa Guardiana si inchina, e ti augura Buona Fortuna
endif
end
~
#18115
Il Tempio Di Khud --- TRIGGER 1~
2 g 100
~
if %actor.level% < 71
if %actor.vnum% == -1
if %direction% != west
wait 5
wecho &6&xUn brivido ti percorre la schiena...&0
wait 10
wecho &6&xC'e' qualcosa di strano in questo luogo...&0
wait 10
wecho &6&xCome i segni di una battaglia epocale, senza tempo&0
wait 10
wecho &6&xForse e' meglio andarsene...&0
end
end
end
~
#18116
Il Tempio Di Khud --- TRIGGER 2~
2 h 100
~
if %actor.level% < 71
if %object.vnum% == 1124
wpurge
wload mob 1114
wforce sacerdote vis
wait 20
wforce sacerdote say Benvenuto Avventuriero...
wait 5
wsend %actor% Un Gran Sacerdote di Khud ti sorride
wait 20
wsend %actor% Dal nulla compare una iscrizione, che la forza divina ti obbliga a leggere...
wait 20
wsend %actor% Benvenuto al Tempio di Khud, il Grande Dio Della Guerra.
wait 5
wsend %actor% Se sarai valoroso ti attendono ricchezze e gloria, ma
wait 5
wsend %actor% e' molto piu' probabile che il tuo destino sia una morte
wait 5
wsend %actor% orribile e senza ricordo.
wait 30
wsend %actor% Il sacerdote ti fa strada all' interno del tempio...
wait 10
wteleport %actor% 1191
wforce %actor% look
wpurge
wecho &6&x %actor.name% scompare nel nulla...&0
end
end
~
#18117
Tempio Di Khud --- TRIGGER 3~
0 d 1
statuetta~
if %actor.name% == stuart || %actor.name% == Elroh || %actor.name% == Shiva
if %speech% == statuetta
wait 5
say Come desiderate signore, che Khud possa proteggervi.
mload obj 1124
wait 5
dai statuetta %actor.name%
wait 3
inchina %actor.name%
end
else
wait 5
say Khud non vi considera degno di darmi ordini !
end
~
#18118
Tempio Di Khud --- TRIGGER 4~
0 d 1
sacrifica~
if %actor.name% == stuart || %actor.name% == khud || %actor.name% == Elroh
set %speech%
global sacrifica
kill %sacrifica%
end
~
#18119
Tempio Di Khud --- TRIGGER 5~
0 d 1
fedele guerriero mistico martello~
if %actor.name% == stuart || %actor.name% == Elroh
if %speech% == fedele
say Come desiderate signore, che Khud possa proteggervi.
mload obj 1129
wait 5
dai veste %actor.name%
wait 3
inchina %actor.name%
else
if %speech% == guerriero
say Come desiderate signore, che Khud possa proteggervi.
mload obj 1126
wait 5
dai veste %actor.name%
wait 3
inchina %actor.name%
else
if %speech% == mistico
say Come desiderate signore, che Khud possa proteggervi.
mload obj 1127
wait 5
dai veste %actor.name%
wait 3
inchina %actor.name%
else
if %speech% == martello
say Come desiderate signore, che Khud possa proteggervi.
mload obj 1128
wait 5
dai veste %actor.name%
wait 3
inchina %actor.name%
end
end
end
end
else
wait 5
say Khud non vi considera degno di darmi ordini !
end
~
#18120
Quest Khud --- TRIGGER 1~
2 d 100
khud~
wait 4
wecho &6&xImprovvisamente una tempesta di luce avvolge la stanza...&0
wait 10
wecho &6&xI Sigilli sono infine rotti, dopo secoli...&0
wait 10
wload obj 1131
wecho &6&xTutto sembra tornare normale, ma c'e' qualcosa sul pavimento che attira la tua attenzione&0
wait 10
wecho &6&xUna strana pergamena...&0
end
~
#18121
Quest Khud --- TRIGGER 2~
2 h 100
~
if %actor.level% < 71
if %object.vnum% == 1124
wait 2
wpurge
wait 5
wecho &6&xUna forza mistica infonde coraggio al tuo spirito...&0
wait 10
wecho &6&xBenvenuto avventuriero, la battaglia ti aspetta!&0
wait 10
wteleport %actor% 1044
wecho %actor.name% scompare nel nulla...
wforce %actor% look
end
end
~
#18122
Quest Khud --- TRIGGER 3~
2 h 100
~
if %actor.level% < 71
if %object.vnum% == 1124
wait 2
wpurge
wait 5
wecho &6&xUna forza mistica infonde coraggio al tuo spirito...&0
wait 10
wecho &6&xBenvenuto avventuriero, la battaglia ti aspetta!&0
wait 10
wteleport %actor% 1100
wecho %actor.name% scompare nel nulla...
wforce %actor% look
end
end
~
#18123
Quest Khud --- TRIGGER 4~
2 g 100
~
wait 5
wecho &6&xC'e' qualcosa di strano in questo posto...&0
wait 10
wecho &6&xUn' incomprensibile forza mistica che sembra attendere una tua mossa...&0
end
~
#18124
Quest Khud --- TRIGGER 5~
2 g 100
~
wait 30
wforce drago wake
wait 30
wforce drago emote si guarda attorno, lentamente....
wait 30
wforce drago emote TI HA VISTO !
wait 30
wforce drago stand
wait 30
wforce drago say Umano vattene dalla mia tana maledetta o sarai il mio prossimo pasto !!!!
wait 30
wforce drago emote si prepara ad attaccarti... attento !!!
wait 60
wforce drago kill %actor.name%
end
~
#18125
Oggetto Maledetto (Khud)~
1 j 100
~
wait 5
oload mob 18100
oforce squalo kill %actor%
end
~
#18126
prova~
1 j 100
~
oecho prova
end
~
$~
