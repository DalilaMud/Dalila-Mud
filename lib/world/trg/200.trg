#20008
Mietitrice (Sheep)~
1 j 100
~
wait 1 s
oecho &RLe rune incise sulla lama della Mietitrice brillano incandescenti per qualche istante...&0
if %actor.name% == orome
halt
else
osend %actor.name% &RVieni avvolto da fiamme magiche che divorano completamente le tue carni, riducendoti in cenere&0
odamage %actor.name% 5000
end
~
#20009
Pietra Therion (Sheep)~
1 g 100
~
if %actor.vnum% != -1
halt
end
wait 1 s
osend %actor.name% Non appena raccogli la pietra, essa si tramuta in qualcos'altro...
if %actor.alias% == lexus
otransform 20054
wait 1 s
oecho &cEcco che dal nulla si materializza Soffio... il messaggero di Therion!&d
oload mob 20062
halt
end
if %actor.alias% == krog
otransform 20055
wait 1 s
oecho &cEcco che dal nulla si materializza Soffio... il messaggero di Therion!&d
oload mob 20062
halt
else
otransform 41500
halt
end
~
#20010
Polvere segreta (Sheep)~
1 c 2
reverti~
otransform 20053
~
#20014
Scudo Krog (Sheep)~
1 j 100
~
if %actor.name% == krog
wait 1 s
oecho &WLa lettera &RK&W incisa sullo scudo diventa incandescente non appena Krog impugna lo scudo&0
halt
else
wait 1 s
osend %actor.name% &RUna potente scossa risale il braccio che regge lo scudo!&0
wait 1 s
odamage %actor.name% 5000
end
~
#20015
Ascia McFly (Sheep)~
1 j 100
~
if %actor.name% != mcfly
wait 1 s
osend %actor.name% &RNon sei abbastanza in gamba per reggere il DE CA PI TA TO RE! L'ascia pare accorgersene e ti taglia la testa! ZAC!&d
oechoaround %actor.name% &RIl DE CA PI TA TO RE taglia la testa a %actor.name%&d
odamage %actor.name% 5000
halt
else
wait 1 s
oecho &R %actor.name% sogghigna con fare malvagio mentre lecca la lama del DE CA PI TA TO RE&d
halt
~
#20016
Soffio (1) (Sheep)~
0 n 100
~
wait 2 s
say Presto, giovane Eroe, consegnami il Sigillo di Therion!
~
#20017
Soffio (2) (Sheep)~
0 j 100
~
wait 1 s
say Grazie, giovane Eroe...
mat 1001 drop sigillo
wait 1 s
mecho &cSoffio scompare in un soffio di vento...&d
mgoto xxxxxx
~
#20025
SesamoVocale (by Phantom)~
2 d 1
Morituras morituras~
* Lista comandi
wait 1 s
wecho &6La terra inizia a tremare&0 
wait 2 s
wecho &6Con un violento boato la terra si apre improvvisamente&0
wait 1 s
wforce all sit
wait 1 s
wdoor 1451441 down room 20025
wait 1 s
wdoor 20025 up room 1451441
wait 180 s
wdoor 1451441 down purge
wdoor 20025 up purge
wecho &6Lentamente la terra si richiude&0
~
#20026
Lancia Demostenee (Sheep)~
1 j 100
~
if %actor.name% == demostenee
wait 1 s
oecho &cLa lancia del Duca di Acaia sbrilluccica di energie magiche... fa quasi male agli occhi guardarla!&0
halt
else
wait 1 s
odamage %actor.name% 500
dg_cast 'teleport' %actor.name%
osend %actor.name% &cLa voce di Demostenee rimbomba nella tua mente: questa lancia sara' la tua rovina! &0
end
~
#20027
Martello di Azatoth (Sheep)~
1 j 100
~
if %actor.name% == sagat
wait 1 s
oechoaround %actor.name% &eL'immondo Martello di Azatoth emette un sinistro bagliore violastro mentre viene impugnato dall'Arcivescovo di Xhyphys&0
osend %actor.name% &eImpugnando questo martello ti rendi visibile all'Occhio di Azatoth. La follia del Caos Finale riempie la tua mente&0
wait 1 s
oforce %actor.name% grida Ph'nglui mglw'nafh Xhyphys R'lyeh wgah'nagl fhtagn
halt
else
wait 1 s
osend %actor.name% &eLa Follia irrompe nella tua mente come un fiume in piena&d
oechoaround %actor.name% sembra strano...
wait 2 s
osend %actor.name% &eLa visione del Caos Finale distrugge tutti i tuoi pensieri e la tua volonta'&d
oechoaround %actor.name% sembra davvero strano...
wait 2 s
oforce %actor.name% grida Ph'nglui mglw'nafh Xhyphys R'lyeh wgah'nagl fhtagn
oechoaround %actor.name% va in mille pezzi!
osend %actor.name% &eIl tuo corpo e la tua mente non possono sopportare questa visione..... addio, F O L L E!&d
odamage %actor.name% 5000
end
~
#20028
Armeria (Sheep)~
1 c 100
armami~
if %actor.name% == orome
oecho &RL'armeria brilla per qualche istante di un'arcana luce magica&d
wait 1 s
oecho L'armeria produce le due spade di Orome!
oload obj 20028
oload obj 20008
halt
end
else
oecho &RIl pentacolo produce qualche scintilla...&d
end
~
#20029
Spaccaossa (Sheep)~
1 j 100
~
if %actor.name% == shailin
wait 1 s
osend %actor.name% &WSenti una grande energia provenire dalla S p a C c a Ossa&d
oechoaround %actor.name% Shailin fa volteggiare furiosamente la S p a C c a Ossa sopra la sua testa
halt
else
osend %actor.name% &WSenti una potente scarica di energia partire dalla S p a C c a Ossa e percorrerti la colonna vertebrale.... non e' piacevole!&0
odamage %actor.name% 500
end
~
#20030
Anello Dragone Verde~
1 b 15
~
oecho &VL'anello del Dragone Verde sprigiona una nuvola di acido corrosivo!&d
dg_cast 'acid breath'
~
#20053
Gemma Therion 1 (Sheep)~
1 g 100
~
oat 1 oecho ciao
~
#20072
Occhio cieco 1 (Sheep)~
1 j 100
~
if %actor.name% != ultorii
wait 1 s
oechoaround %actor.name% &WNon appena %actor.name% si conficca l'occhio nell'orbita lo vedi dissolversi nell'aria... che strano!&d
osend %actor.name% &WTi senti strano, molto strano... ti senti malissimo! E poi... ehi ma dove sei finito?&d
odamage %actor.name% 450
oteleport %actor.name% 20522
halt
end
~
#20073
Occhio cieco 2 (Sheep)~
1 c 1
portale~
wait 1 s
osend %actor.name% &W5&d
wait 1 s
osend %actor.name% &W4&d
wait 1 s
osend %actor.name% &W3&d
wait 1 s
osend %actor.name% &W2&d
wait 1 s
osend %actor.name% &W1&d
wait 1 s
oechoaround %actor.name% &WTutto ad un tratto %actor.name% si dissolve nell'aria con un abbagliante lampo di luce!&d
osend %actor.name% &WL'occhio cieco emana un lampo di energia...&0
oteleport %actor.name% 20532
halt
~
#20074
Magia Selvaggia (Sheep)~
1 b 10
~
set cb %self.worn_by%
set iv Il Vuoto
if %cb.name% == %iv%
halt
end
oecho &GLo scettro della magia selvaggia inizia a brillare intensamente...&d
wait 2 s
oecho &GLo scettro sta per rilasciare una magia selvaggia!&d
wait 2 s
eval magia %random.10%
switch %magia%
case 1
dg_cast 'teleport' %cb.alias%
break
case 2
dg_cast 'feast' %cb.alias%
break
case 3
dg_cast 'paralize' %cb.alias%
oecho &G Lo scettro prova a paralizzare %cb.alias% ... &d
break
case 4
dg_cast 'sanctuary' %cb.alias%
break
case 5
dg_cast 'fear'
break
case 6
dg_cast 'heal' %cb.alias%
break
case 7
dg_cast 'blink' %cb.alias%
break
case 8
dg_cast 'energy drain' %cb.alias%
break
case 9
dg_cast 'sense life' %cb.alias%
break
case 10
dg_cast 'levitate' %cb.alias%
break
done
~
#20075
Ali Clive 1 (Sheep)~
1 b 50
~
set iv Il Vuoto
set cb %self.worn_by%
if %cb.name% == clive
halt
end
if %cb.name% == %iv%
halt
end
wait 1 s
osend %cb.name% &cLe ali cominciano a sbattere e tu decolli! Ma... ehi non sai volare tu.. non con questi affari almeno!&d
oechoaround %cb.name% &c%cb.name% comincia a volare di qua e di la'... non pare saper governare molto bene queste ali!&d
wait 2 s
osend %cb.name% &RTi schianti al suolo con un gran botto! Che male!&d
oechoaround %cb.name% &R%cb.name% si schianta al suolo con un gran botto! Che dolore!&0
odamage %cb.alias% 500
~
#20076
Ali Clive 2 (Sheep)~
1 j 100
~
wait 1 s
if %actor.name% != clive
halt
end
osend %actor.name% Se vuoi volare con le tua ali, usa il comando "vola"
~
#20077
Ali Clive 3 (Sheep)~
1 c 1
vola~
if  %actor.affect(fly)% == 1
wait 1 s
osend %actor.name% Stai gia' volando!
halt
end
dg_cast 'fly' %actor.name%
wait 1 s
osend %actor.name% &cDispieghi le tue Ali da GabbianO e ti libri in volo.... ah... la liberta'!&d
oechoaround %actor.name% &c%actor.name% si libra in volo sbattendo le sue Ali da GabbianO&d
~
#20078
Phobos e Deimos (Sheep)~
1 j 100
~
wait 1 s
if %actor.name% == knives
osend %actor.name% &RSenti la  sete di SANGUE di Shaarr offuscarti i pensieri&d
oechoaround %actor.name% &RGli occhi di %actor.name% si velano di rosso!&d
halt
else
osend %actor.name% &RPer un attimo hai la visione del volto di Shaarr.. &d
wait 1 s
osend %actor.name% &RSenti il suo Potere divorarti le carni... povero mortale!&d
wait 1 s
oechoaround %actor.name% &R%actor.name% brucia circondato da fiamme rosso sangue...&d
odamage %actor.name% 5000
~
#20079
Pedana teletrasportante di Urdul (Sheep)~
2 g 100
~
set empty
set char %random.char%
while %char% != 
wait 1 s
wecho &WStrani lampi &Ggialli&W e &Rrossi &Wpercorrono le pareti della piccola stanzetta... &d
wait 2 s
wecho &WUna voce femminile riempie la stanza scandendo un conto alla rovescia...&d
wait 1 s
wecho &WCinque...&d
wait 1 s
wecho &GQuattro...&d
wait 1 s
wecho &BTre...&d
wait 1 s
wecho &CDue...&d
wait 1 s
wecho &RUno!&d
wait 1 s
if %char.alias% == %empty%
wecho &WMa non succede nulla... &d
else
wecho &WDalle strette pareti si sprigiona un'ondata di &Cenergia magica&W che investe %char.name%&d
wait 1 s
dg_cast 'teleport' %char%
end
set char %random.char%
done
halt
~
#20080
Urdul greet (Sheep)~
0 g 100
~
wait 1 s
say Benvenuto!
wait 1 s
say Prova la mia fantastica pedana magica!
wait 1 s
say ne sarai soddisfatto!
wait 1 s
ann
wait 1 s
emote zompetta allegramente qui intorno...
~
#20087
Bracciali Collera Divina (Sheep)~
1 j 100
~
if %actor.name% == disarm
wait 1 s
osend %actor.name% &WSenti la Collera degli Dei scorrerti nel sangue!&d
oechoaround %actor.name% &W%actor.name% appare circonfuso da un'abbagliante aura bianca per alcuni istanti...&d
halt
else
osend %actor.name% &WLa collera degli antichi Dei si abbatte su di te con tutta la sua furia!&d
odamage %actor.name% 5000
end
~
#20090
Kizeridis Vello d'Oro~
0 b 100
~
* Lista comandi
eval numero %random.10%
if %numero% == 1
say E' arrivato il Vello D'Oroooo.....avvicinateviiiiii!!. Donneeee, uominiiiii, bambini......abbiamo il vestito che fa per voooi! Presto Presto!
end
if %numero% == 2
say Veeestitiiii, stooooffe, sete rarissimeeeeeee.....tutto dal Vello D'oroooooooo!
end
~
#20091
Kizeridis Vello d'Oro (2) (UG by Offar)~
0 g 100
~
* Lista comandi
if %actor.vnum% == -1
emote si inchina profondamente e sorride
else
end
~
#20092
SEGUACI DI KHUD --> Skizzo (Faymond)~
1 j 100
~
* Lista comandi
wait 3 s
set a %actor.name%
if %a% == Elroh || %a% == Stuart
%send% %actor% &7Senti il divino potere di Khud fortificare il tuo corpo in un'aura magica!&0
%echoaround% %actor% &7Per un istante il corpo di %actor.name% e' circondato da bianco potere divino!&0
else
%send% %actor% &1La veste di Khud sembra scaldarsi attorno al tuo corpo.. no... brucia..!&0
%echoaround% %actor% &1La veste indossata da %actor.name% sembra bruciare di fiamme invisibili..!&0
wait 2 s
%send% %actor% &1E' incandescente... urli dal dolore... ti senti sciogliere...&0
%echoaround% %actor% &3%actor.name% urla dal dolore in preda a spasmi e il suo corpo sembra lentamente sbiadire!&0
wait 2 s
%send% %actor% &1Improvvisamente tutto diventa buio per un istante.. senti il tuo corpo teletrasportarsi nell'etere..&0
%echoaround% %actor% &3..Improvvisamente svanisce nel nulla!!!&0
oteleport %actor% 1195
wait 1 s
oforce %actor% look
wait 2 s
%force% vendic say ..tu... sei un profanatore delle sacre vesti.
wait 3 s
%force% vendic emote ti indica... una forza misteriosa e arcana ti induce a levarti la veste per distruggerla!
wait 2 s
%force% %actor% remov khud
wait 1 s
%force% %actor% give khud khud
end
~
#20093
Il vendicatore si incazza (Faymond)~
0 j 100
khud~
* Lista comandi
wait 2 s
say ...morirai!!!
wait 2 s
kill %actor.name%
~
#20145
Prova Orione Load~
1 n 100
~
* Lista comandi
oecho ok
~
#20150
Maestro dei Vini 2 (by Dajkesh)~
0 d 100
rosato rosso bianco portello argentario fruttato lamorketto birra malti drago amaro digestivo lista salve scusate vino~
* Lista comandi
wait 1
if %speech% == scusate || %speech% == salve || %speech% == vino
switch %random.11%
case 1
wait 1
say Si'? Se volete del vino sono qui per servirvi...
break
case 2
wait 1
say Si'? Se volete del vino sono qui per servirvi...
break
case 3
wait 1
say Ditemi quale vino volete gustare e io saro' lieto di porgervene una coppa.
break
case 4
wait 1
say Gradite forse un bicchiere di buon &7Portello bianco&0?
break
case 5
wait 1
say Forse vi aggrada gustare del &1fruttato rosso&0 di Lamorka?
break
case 6
wait 1
say Se posso permettermi un consiglio, abbiamo dell'ottimo &1&xLamorketto&0.
break
case 7
wait 1
say Forse vossignoria potrebbe gradire il nostro &5&xrosato&0 dell'Argentario.
break
case 8
wait 1
say Prego, sono ai vostri ordini.
break
case 9
wait 1
say Ditemi pure, desiderate forse del vino?
break
case 10
wait 1
say Per un dopopasto, non c'e' niente di meglio di un amaro digestivo.
break
case 11
wait 1
say Se siete amanti della birra posso darvi un'ottima birra speciale: I sei malti del Drago.
break
default
wait 1
say Si'? Se volete del vino sono qui per servirvi...
break
done
elseif %speech% == lista
say Ecco la disponibilita' :
say &1&xLamorketto&0, un rosso appena dolce ottimo con la carne.
say &5&xArgentario&0, rosato da abbinare con qualsiasi portata.
say &1Fruttato&0,   dolcissimo con frutta tritata.
say &7Portello&0,   l'eccezionale bianco di mia produzione.
say &2&xAmaro&0,      un eccellente digestivo.
say &3Drago&0,      una birra speciale di sei malti selezionati.
elseif %speech% == portello || %speech% == bianco
say Ah, ve ne intendete dunque!
%load% obj 28012
emote versa dell'ottimo &7Portello bianco&0 dentro un bicchierino.
say Ecco a voi.
give portello %actor.name%
elseif %speech% == rosato || %speech% == argentario 
say Come desiderate!
%load% obj 28010
emote solleva una bottiglia di &5&xrosato&0 dell'Argentario.
emote ne versa lentamente il contenuto in un bicchiere a calice.
wait 1
say Ecco a voi.
give argentario %actor.name%
elseif %speech% == lamorketto
say Ai vostri ordini!
%load% obj 28013
emote versa un po' di pregiato &1&xLamorketto&0 in un bicchiere.
say Bevetelo lentamente per assaporarne il gusto, mi raccomando.
give lamorketto %actor.name%
elseif %speech% == fruttato
%load% obj 28011
say Come preferite!
emote spilla del &1fruttato rosso&0 da una botte.
say Sentirete che delizia!
give fruttato %actor.name%
elseif %speech% == birra || %speech% == malti || %speech% == drago
%load% obj 28015
say Benissimo, una birra in arrivo!
emote afferra una cannula e versa una birra densa e profumata in un boccale.
say Sono sicuro che vi piacera'!
give drago %actor.name%
elseif %speech% == amaro || %speech% == digestivo
%load% obj 28014
say Vi servo subito!
emote prepara un bicchierino.
emote tira fuori una bottiglia lavorata a forma di... si direbbe un faro.
say Ecco qui, e' molto forte!
give amaro %actor.name%
end
~
#20509
Torre di Babele (fay - Nulla)~
0 c 100
dormi~
* Lista comandi
wait 2 s
%send% %actor% &5Immediatamente ti addormenti in un turbinio di colori...&0
mteleport %actor% 20510
%force% %actor% sleep
~
#20574
Portale Magico di Helsyn (by Phantom)~
1 c 4
salta~
* Lista comandi
if %arg% == nel portale
oechoaround %actor% %actor.name% entra in un portale.
%send% %actor% L'oscurita' ti avvolge, quasi cercasse di soffocarti.
   %teleport% %actor% 2124163
wait 1 s
%send% %actor% Ma all'improvviso luce, tanta accecante luce.
end
~
#20575
Specchio Magico di Helsyn (by Phantom)~
1 c 4
attraversa~
* Lista comandi
if %arg% == specchio
oechoaround %actor% %actor.name% entra in uno specchio.
%send% %actor% La tua immagine sembra dividersi, moltiplicarsi, ma alla fine ritorna ad essere quella di sempre.
   %teleport% %actor% 18132
wait 1 s
%send% %actor% Sei in un altro luogo, e forse, anche in un altro tempo
end
~
$~
