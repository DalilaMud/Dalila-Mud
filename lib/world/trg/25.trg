#2500
Accesso alle prove di Therion (Sheep)~
2 c 100
ammetti~
* Lista comandi
if %actor.name% != krogg
halt
end
if %arg.room% != 752
halt
end
wecho Krogg chiude gli occhi e mormora alcune parole che non riesci a comprendere.
wait 2 s
wsend %arg% &2&xLa cortina di fronde dei salici si spalanca, mostrando sul fondo della stanza un buio buco nel terreno in cui le radici degli alberi si inabissano nei meandri della terra.&0&0
wait 3 s
wsend %arg% &6&xStai osservando la piccola voragine e improvvisamente, senza che tu muova un muscolo, vieni  attratto in essa da una forza misteriosa.....&0
wait 3 s
wsend %arg% ...perdi la cognizione del tempo e dello spazio......
wteleport %arg% 799
~
#2501
Clap clap (Sheep)~
2 c 100
clap~
* Lista comandi
wait 2
wforce %actor.name% emote batte una volta le mani...
wait 2
wecho &1&xLe torce improvvisamente si accendono...&0
wteleport all 1509
~
#2502
Mad'Har (Sheep)~
0 g 100
~
* Lista comandi
wait 1 s
if %direction% == south
say Chi sei tu per osare avvicinarti a questo portone?
wait 1 s
emote si gratta delle orribili croste sul viso...
wait 1 s
emote pare aver perso interesse nei tuoi confronti..
end
~
#2503
Stanza tesoro (Sheep)~
2 g 100
~
* Lista comandi
wait 1 s
wecho &7Gli scheletri paiono muoversi impercettibilmente mentre una voce spettrale rimbomba nella stanza...&0
wait 1 s
wecho &7Vogliamo altri tesori questi non bastano!&0
~
#2504
Fuoco Shaarr (Sheep)~
1 g 100
~
* Lista comandi
if %actor.vnum% != -1
wait 1
oecho &9&xLa stanza si riempie di arpie mandate da Shaarr a difendere il Fuoco!&0
oload mob 591
oload mob 591
oload mob 591
oload mob 591
oload mob 591
oload mob 591
oload mob 591
oload mob 591
halt
end
wait 2
osend %actor.name% &3&xIl fuoco di Shaarr ti brucia!&0
odamage %actor.name% 50
oforce %actor.name% posa fuoco
~
#2505
Trono d'ombra (Sheep)~
2 b 20
~
* Lista comandi
wecho &9&xUn &f&x&4lampo&0&x&9 squarcia il vuoto della stanza.&0
~
#2506
Morte Specchio (Sheep)~
0 f 100
~
* Lista comandi
return 0
mecho &6Non appena lo specchio va in frantumi senti mille urla intorno a te! Cosa succede!?&0
mload mob 593
mload mob 593
mload mob 593
mload mob 593
mload mob 593
mload mob 593
mload mob 592
~
#2507
Caricamento Specchio (Sheep)~
0 n 100
~
* Lista comandi
wait 30 s
mpurge
~
#2508
Specchio (2) (Sheep)~
2 g 100
~
* Lista comandi
if %imm%==1
halt
else
set imm 1
global imm
wait 1
wsend %actor.name% Appena entrato di ritrovi di fronte un enorme specchio, 
wsend %actor.name% resti stupito nel vedere quanto sia vivida la tua immagine riflessa.
wload mob 594
end
~
#2509
Specchio (3) (Sheep)~
0 c 100
esamina~
* Lista comandi
if %imm% == 1
halt
end
if %arg% == specchio
set imm 1
global imm
wait 1
mecho &6La tua immagine fuoriesce dallo specchio!&0
mload mob 594
end
~
#2510
Immagine (Sheep mod.Zephy)~
0 d 100
yugo~
* Lista comandi
if %speech% == yugo
   wait 1 s
   msend %actor.name% &6La nebbia muta in uno sciame di insetti che ti sollevano e ti trasportano in un altra dimensione.&0
   mteleport %actor.name% 1551
   setroom imm 0
   mpurge
   mload mob 592
   mgoto 28150
   say purgami
   halt
   end
~
#2511
SpecchioRoom (Sheep)~
2 b 20
~
* Lista comandi
wecho &f&4&xScariche elettriche all'interno della nebbia sembrano volerti dirigere verso lo specchio&0
~
#2512
SwarmRoom (Sheep)~
2 b 20
~
* Lista comandi
wecho &2&xSenti il ronzio incessante degli insetti risuonare in tutta la stanza.&0
~
#2513
Pick Wisteron (Sheep)~
2 c 100
pick~
* Lista comandi
if recinto /= %arg%
wsend %actor.name% &9&xCredi davvero di poter scassinare una porta posta da Shaarr?&0
%damage% %actor.name% 100
wechoaround %actor.name% &9&x%actor.name% pare scosso da tremiti mortali...&0
end
~
#2514
PickWir (2) (Sheep)~
2 c 100
scassina~
* Lista comandi
if recinto /= %arg%
wsend %actor.name% &9&xCredi davvero di poter scassinare una porta posta da Shaarr?&0
%damage% %actor.name% 100
wechoaround %actor.name% &9&x%actor.name% pare scosso da tremiti mortali...&0
end
~
#2515
Wisteron (1) (Sheep)~
0 k 50
~
* Lista comandi
msend %actor.name% &2&xLe nere mandibole di terrore del Wisteron ti stritolano!&0
mechoaround %actor.name% &2&xIl Wisteron azzanna %actor.name% stritolandolo tra le possenti mandibole&0
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
~
#2516
Saluto Dharma (Sheep)~
0 g 100
~
* Lista comandi
if %direction% == south
wait 2
emote ruggisce: &5Temerario  colui che osa disturbare il sonno della Grande Shaarr senza consenso della sua Apocalisse&0
end
~
#2517
Cancelli di Lamorka (Sheep)~
0 d 1
chiudi apri~
* Lista comandi
if %actor.clan% != 8
halt
end
if %actor.rank% <= 24
halt
end
wait 2 
if %speech% == chiudi
if %close% == 1
say I cancellata sono gia' chiusi Signore!
halt
end
emote scatta sull'attenti!
wait 2
say subito Signore!
emote si allontana...
mgoto rfcmsn
mload obj 574
mgoto 6847
emote arriva di corsa...
close cancellata
lock cancellata
wait 2
emote sbuffa e riparte di corsa!
mgoto 6846
emote arriva con affanno..
close cancellata
lock cancellata
wait 2
emote se ne va fischiettando...
mgoto rfcmsn
mat xxxxx posa chiave
wait 2 s
mgoto 6682
emote arriva fischiettando...
say I Cancelli sono chiusi!
set close 1
global close
end
if %speech% == apri
if %close% == 0
say I cancellata sono gia' aperti Signore!
halt
end
emote scatta sull'attenti!
wait 2
say subito Signore!
emote si allontana
mgoto rfcmsn
mload obj 574
mgoto 6847
emote arriva di corsa...
unlock cancellata
open cancellata
wait 2
emote sbuffa e riparte di corsa!
mgoto 6846
emote arriva con affanno..
unlock cancellata
open cancellata
wait 2
emote se ne va fischiettando...
mgoto rfcmsn
mat xxxxx posa chiave
wait 2 s
mgoto 6682
emote arriva fischiettando...
say I cancellata sono aperti!
set close 0
global close
end
~
#2518
Controllo Circo (1) (Sheep)~
0 m 10
~
* Lista comandi
   if %actor.level% > 20
   wait 2 s
   say Mi spiace %actor.name% ma voi siete davvero troppo alto per entrare qui! Potreste spaventare gli animali!
   give %amount% coin %actor.name%
   halt 
   end
   if %actor.level% < 10
   wait 2 s
   say mi spiace %actor.name%, ma voi siete troppo piccino! Le belve feroci potrebbero divorarvi!
   wait 2 s
   give %amount% coin %actor.name%
   halt
   end
   wait 2 s
   eval diff %amount%-10
   if %diff% > 0
   wait 2 s
   say Mi avete dato troppi soldi! Eccovi il resto!
   give %diff% coin %actor.name%
   end
   say Molto bene %actor.name%, vi auguro di divertirvi nel nostro Circo!
   wait 2 s
   mechoaround %actor.name% L'omino apre la tenda e conduce %actor.name% all'interno
   msend %actor.name% L'omino apre la tenda e ti conduce all'interno
   mteleport %actor.name% 8801
~
#2519
Controllo Circo (2) (Sheep)~
0 g 100
~
* Lista comandi
   wait 1 s
   say Benvenuto al nostro fantastico Circo!
   wait 1 s
   say Volete entrare a vederne le meraviglie? Vedrete fantastici animali, potenti maghi e molte altre attrazioni!
   wait 1 s
   say Il biglietto costa solo 10 monete! Visitate il Circo Daresiano!
   wait 1 s
   say Venghino venghino Siore e Siori
~
#2520
Purgatore (Sheep)~
1 j 100
~
* Lista comandi
opurge
~
#2521
Arcangelo Giustizia (1) (Sheep)~
0 k 100
~
* Lista comandi
msend %actor.name% &7&xL'Arcangelo ti colpisce con la sua Sacra Spada, polverizzandoti&0
mechoaround %actor.name% &7&xL'Arcangelo colpisce %actor.name% con la sua Sacra Spada, polverizzandolo&0
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
~
#2522
Arcangelo (1) (Sheep)~
0 c 100
giustizia~
* Lista comandi
if %actor.name% != alhandir
halt
end
if %giustiziere% != 1
wait 2
msend %actor.name% &7&xPer fare giustizia... serve un Arcangelo...&0
halt
end
if %arg.vnum% != -1
wait 2
msend %actor.name% Non puoi fare questo....
halt
end
set giustiziere 0
global giustiziere
mecho &7&xLa stanza si colma della Luce e della Gloria di Silue...&0
wait 2 s
msend %actor.name% &7&xUn vento impetuoso ti spazza mentre con gli occhi della mente vedi l'Arcangelo scendere su %arg%&0 
mat %arg% msend %arg% &7&xUna luce abbacinante ti acceca mentre un Arcangelo ti compare innanzi... ohi ohi&0
mat %arg% mechoaround %arg% &7&xUna luce abbacinante ti acceca ed un Arcangelo compare&0
mat %arg% mload mob 598
wait 2 s
mat asxv mforce asxv mgoto %arg%
mat %arg% mforce asxv mkill %arg%
wait 10 s
mat asxv mforce asxv mgoto %arg%
mat asxv mforce asxv kill %arg%
wait 10 s
mat asxv mforce asxv mgoto %arg%
mat asxv mforce asxv kill %arg%
wait 20 s
mat asxv mecho &7&xL'Arcangelo scompare...&0
mat asxv mforce asxv mgoto 28150
mat asxv mforce asxv say purgami
~
#2523
Arcieri Lamorka (1) (Sheep)~
2 c 100
pick~
* Lista comandi
if %arg% == c || %arg% == ca || %arg% == can || %arg% == canc || %arg% == cance || %arg% == cancel || %arg% == cancell || %arg% == cancella || %arg% == cancellat || %arg% == cancellata
wait 16 s
wsend %actor.name% &1&xGli arcieri sopra le mura si accorgono del tuo tentativo di forzare la cancellata!&0
wait 1
wsend %actor.name% &1&xUn nugolo di freccie si dirige verso di te!&0
eval danno %random.300%
%damage% %actor.name% %danno%
wait 1
wsend %actor.name% &1&xTi stanno riducendo un puntaspilli!&0
wechoaround %actor.name% &1&x%actor.name% viene colpito da un nugolo di freccie!&0
else
wait 1 s
halt
end
~
#2524
Arcieri Lamorka (2) (Sheep)~
2 c 100
scassina~
* Lista comandi
if %arg% == c || %arg% == ca || %arg% == can || %arg% == canc || %arg% == cance || %arg% == cancel || %arg% == cancell || %arg% == cancella || %arg% == cancellat || %arg% == cancellata
wait 16 s
wsend %actor.name% &1&xGli arcieri sopra le mura si accorgono del tuo tentativo di forzare la cancellata!&0
wait 1
wsend %actor.name% &1&xUn nugolo di freccie si dirige verso di te!&0
eval danno %random.300%
%damage% %actor.name% %danno%
wait 1
wsend %actor.name% &1&xTi stanno riducendo un puntaspilli!&0
wechoaround %actor.name% &1&x%actor.name% viene colpito da un nugolo di freccie!&0
else
wait 1 s
halt
end
~
#2525
Juzar (1) (Sheep)~
2 c 1
xxx~
* Lista comandi
if %actor.vnum% != 699
halt
end
wecho Juzar tira le redini del drago....
wait 1 s
wecho &2&xIl drago spiega le sue ali, ed incomincia a batterle...&0
wait 1 s
wecho &2&x...Improvvisamente il drago si alza in volo ed il carro con lui!&0
%teleport% all 699
~
#2526
Juzar (2) (Sheep)~
2 c 1
xxx~
* Lista comandi
if %actor.vnum% != 699
halt
end
wecho Juzar tira le redini del drago....
wait 1 s
wecho &2&xIl drago spiega le sue ali, ed incomincia a batterle...&0
wait 1 s
wecho &2&x...Improvvisamente il drago si alza in volo ed il carro con lui!&0
%teleport% all 698
~
#2527
Cascata Tenchu (1) (Sheep)~
2 c 1
est~
* Lista comandi
wait 2
wsend %actor.name% &9&xCerchi con un balzo di arrivare dall'altra parte della stanza, ma improvvisamente scivoli.&0
wechoaround %actor.name% &9&x%actor.name% scivola mentre cerca di arrivare dall'altra parte della stanza!&0
%damage% %actor.name% 100
%teleport% %actor.name% 697
~
#2528
Cascata Tenchu (2) (Sheep)~
2 c 100
inchinati~
* Lista comandi
wait 2
wsend %actor.name% &6&xTi inchini, e col tuo peso il masso su cui stavi si sposta piu' avanti... un piccolo balzo ed entri nel Tempio&0'
wechoaround %actor.name% &6&x%actor.name% si inchina...&0
%teleport% %actor.name% 684
~
#2529
Cascata Tenchu (3) (Sheep)~
2 c 100
bow~
* Lista comandi
wait 2
wsend %actor.name% &6&xTi inchini, e col tuo peso il masso su cui stavi si sposta piu' avanti... un piccolo balzo ed entri nel Tempio&0
wechoaround %actor.name% &6&x%actor.name% si inchina...&0
%teleport% %actor.name% 684
~
#2530
Protettore Divino (1) (Sheep)~
0 d 0
tu ora mi seguirai, in nome di~
* Lista comandi
if %evocato% != 0
halt
end
set owner %actor.name%
global owner
set evocato 1
global evocato
wait 2
fol %actor.name%
wait 2
inch %actor.name%
say Cosi' sia
set fol 1
global fol
tell %actor.name% I comandi per usarmi sono "fermati" e "seguimi" detti col say
~
#2531
Protettore Divino (2) (Sheep)~
0 d 1
fermati seguimi~
* Lista comandi
if %actor.name% != %owner%
halt
end
if %speech% == fermati
wait 2
if %fol% == 0
say Non ti sto seguendo, %owner%
halt
end
say Come desideri, %owner%
fol self
set fol 0
global fol
halt
end
if %speech% == seguimi
wait 2
if %fol% == 1
say Ti sto gia' seguendo, %owner%
halt
end
say Come desideri, %owner%
fol %owner%
set fol 1
global fol
halt
end
~
#2532
Tiro al bersaglio (Sheep)~
0 m 100
~
* Lista comandi
set punti 0
global punti
set hit %actor.hitroll%
global hit
if %actor.class% == alcione
set classe 2
global classe
elseif %actor.class% == peloi
set classe 1
global classe
elseif %actor.class% == genidian
set classe 0
global classe
elseif %actor.class% == pandion
eval classe -2
global classe
elseif %actor.class% == cyrinic
eval classe -2
global classe
end
wait 2 s
if %amount% > 100
say Mi hai dato troppi soldi, %actor.name%
eval diff %amount% - 100
wait 1 s
dai %diff% coins %actor.name%
end
say Molto bene! Molto bene!!
wait 3 s
emote pare ghignarsela sotto i baffi...
say Abbiamo un tiratore di coltelli qui!
wait 3 s
say Facci vedere la tua bravura, %actor.name%!
msend %actor.name% Sharp il guercio ti da 5 coltelli da lancio..
mechoaround %actor.name% Sharp il guercio da 5 coltelli da lancio a %actor.name%
grin
wait 3 s
say Silenzio prego... lasciate concentrare %actor.name%
wait 3 s
say Ecco il primo bersaglio....
wait 3 s
mecho &3&xUna paperella spunta dall'altra parte del baraccone sull'apposita pedana...&0
wait 4 s
mforce %actor.name% emote soppesa il coltello e si prepara al lancio...
eval tiro %random.10%
wait 5 s
mforce %actor.name% emote lancia il coltello...
wait 1
eval tiro %tiro%+%classe%
eval tiro %tiro%+%hit%
if %tiro% >= 8
eval punti %punti%+10
mecho &7Il coltello si pianta nell'occhio della papperella!&0
wait 1 s
fischia
say Che mira!
elseif %tiro% >= 5 && %tiro% < 8
mecho &7Il coltello manca di un soffio la paperella... peccato!&0
else
mecho &7Il coltello manca di molto la paperella... che figura!&0
end
wait 4 s
say Ed ecco il secondo bersaglio....
wait 5 s
mecho &6&xUn palloncino spunta sulla pedana...&0
wait 5 s
mforce %actor.name% emote soppesa il coltello e si prepara al lancio...
eval tiro %random.10%
wait 5 s
mforce %actor.name% emote lancia il coltello...
wait 5
eval tiro %tiro%+%classe%
eval tiro %tiro%+%hit%
if %tiro% >= 9
eval punti %punti%+15
mecho &7Il coltello fa esplodere il palloncino!&0
wait 1 s
fischia
say Che mira!
elseif %tiro% >= 6 && %tiro% < 9
mecho &7Il coltello manca di un soffio il palloncino che ballonzola....&0
else
mecho &7Il coltello manca clamorosamente il palloncino e si pianta nella pancia di un peluche...&0
end
wait 5 s
say Ed ecco il terzo bersaglio....
wait 5 s
mecho &9&xUn carta con l'Asso di Picche si alza sulla pedana....&0
wait 5 s
mforce %actor.name% emote soppesa il coltello e si prepara al lancio...
eval tiro %random.10%
wait 5 s
mforce %actor.name% emote lancia il coltello...
wait 5
eval tiro %tiro%+%classe%
eval tiro %tiro%+%hit%
if %tiro% >= 10
eval punti %punti%+20
mecho &7Il coltello c'entra il bersaglio proprio nel mezzo! Fantastico!&0
wait 5 s
fischia
say Che mira!
elseif %tiro% >= 7 && %tiro% < 10
mecho &7Il coltello manca di poco la carta.... acc!&0
else
mecho &7Il coltello oltrepassa la carta, esce dal baraccone e prosegue la sua corsa fuori dalla vista!&0
wait 5 s
mecho &7&x...si sente un grido di dolore venire da fuori! Accidenti.... conviene nascondere i coltelli!&0
end
wait 5 s
say Ed ecco il quarto bersaglio....
wait 5 s
mecho &3&xUn tappo di sughero si alza sulla pedana....&0
wait 5 s
mforce %actor.name% emote soppesa il coltello e si prepara al lancio...
eval tiro %random.10%
wait 5 s
mforce %actor.name% emote lancia il coltello...
wait 5
eval tiro %tiro%+%classe%
eval tiro %tiro%+%hit%
if %tiro% >= 11
eval punti %punti%+25
mecho &7Il coltello spacca in due il tappo di sughero! WOW!&0
wait 5 s
fischia
say Che mira!
elseif %tiro% >= 8 && %tiro% < 11
mecho &7Il coltello manca di poco il tappo.... sigh!&0
else
mecho &7Il coltello manca alla grande il tappo... ma a cosa diavolo ha mirato?&0
end
wait 5 s
say Ed ecco il quinto bersaglio....
wait 5 s
mecho &3&xUna monetina si alza sulla pedana....&0
wait 5 s
mforce %actor.name% emote soppesa il coltello e si prepara al lancio...
wait 5 s
mforce %actor.name% emote suda un po'...
eval tiro %random.10%
wait 5 s
mforce %actor.name% emote lancia il coltello...
wait 5 s
eval tiro %tiro%+%classe%
eval tiro %tiro%+%hit%
if %tiro% >= 12
eval punti %punti%+30
mecho &7Il coltello prende la monetina nel mezzo spaccandola in due!&0
wait 5 s
fischia
say Che mira!
elseif %tiro% >= 9 && %tiro% < 12
mecho &7Il coltello manca di poco la monetina.... sigh!&0
else
mecho &7Il coltello oltrepassa la monetina incurante e prosegue la sua corsa verso la gloria...&0
end
wait 5 s
say Molto bene.... avete totalizzato %punti% punti!
wait 5 s
say Ecco il vostro premio...
wait 5 s
sor %actor.name%
if %punti% <= 30
mload obj 8800
dai panda %actor.name%
posa panda
halt
elseif %punti% <= 50
mload obj 8801
dai coltello %actor.name%
posa coltello
halt
elseif %punti% <= 80
mload obj 8802
dai anello %actor.name% 
posa anello
halt
elseif %punti% <= 99
mload obj 8803
dai guanti %actor.name%
posa guanti
halt
else
wait 2 s
say Avete vinto il primo premio! Complimenti!
mload obj 8804
dai occhiali %actor.name%
posa occhiali
halt
end
~
#2533
Maciste (Sheep)~
0 m 100
~
* Lista comandi
set dam %actor.damroll%
global dam
if %actor.class% == alcione
set classe 0
global classe
elseif %actor.class% == peloi
set classe 1
global classe
elseif %actor.class% == genidian
set classe 2
global classe
elseif %actor.class% == pandion
eval classe -2
global classe
elseif %actor.class% == cyrinic
eval classe -2
global classe
end
if %amount% > 100
say Mi hai dato troppi soldi, %actor.name%
eval diff %amount% - 100
wait 1 s
dai %diff% coins %actor.name%
end
wait 2 s
emote emette una fragorosa risata... che polmoni deve avere costui!
wait 3 s
say Sei sicuro di riuscire a impugnare il martello, piccolo %actor.name%?
wait 3 s
emote ride ancor piu' fragorosamente di prima...
wait 3 s
say Va bene... vediamo cosa sai fare...
wait 4 s
msend %actor.name% Maciste ti porge quello che ti pare un &9&xENORME E PESANTE MARTELLO...&0
mechoaround %actor.name% Maciste porge a %actor.name% un piccolo martello... pare leggero!
wait 4 s
mforce %actor.name% emote si sputa sulle mani...
eval dam %dam%+%classe%
global dam
eval tiro %random.10%
eval tiro %tiro%*%dam%
wait 3 s
mforce %actor.name% emote alza il martello e si prepara ad abbatterlo.....
wait 3 s
mforce %actor.name% emote cala con veemenza il martello!
if %tiro% >= 70
wait 3 s
mecho &7Il percussore sale....&0
wait 3 s
mecho &7Il percussore continua a salire....&0
wait 3 s
mecho &7Il percussore sale sempre piu' su....&0
wait 3 s
mecho &7&xGOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOONG! Il percussore colpisce la campana!&0
wait 3 s
fischia
wait 2 s
say Complimenti vivissimi! Sei davvero forte %actor.name%! Ecco il tuo premio
mload obj 8812
dai catene %actor.name%
posa catene
halt
elseif %tiro% >= 50 && %tiro% < 70
wait 3 s
mecho &7Il percussore sale....&0
wait 3 s
mecho &7Il percussore continua a salire....&0
wait 3 s
mecho &7Il percussore sale sempre piu' su....&0
wait 3 s
mecho &7Il percussore si arresta poco prima della campana... peccato!&0
wait 3 s
say Complimenti %actor.name%! Bella prestazione! Per poco non vincevi il superpremio!
wait 3 s
say Eccoti il premio!
wait 2 s
mload obj 8810
dai guanti %actor.name%
posa guanti
halt
elseif %tiro% >= 30 && %tiro% < 50
wait 3 s
mecho &7Il percussore sale....&0
wait 3 s
mecho &7Il percussore continua a salire....&0
wait 3 s
mecho &7Il percussore si arresta con mestizia...&0
wait 3 s
sor
say Bella prestazione!
wait 2
emote se la ride sotto i baffi...
wait 2
say Eccoti un bel premio!
wait 2
mload obj 8809
dai cinto %actor.name%
posa cinto
halt
elseif %tiro% >= 10 && %tiro% < 30
wait 3 s
mecho &7Il percussore sale....&0
wait 3 s
mecho &7Il percussore si ferma miserrimamente...&0
wait 3 s
ridi
wait 3 s
say Bel colpo, %actor.name%!
wait 3 s
mload obj 8811
dai pozione %actor.name%
posa pozione
halt
else
wait 3 s
mecho &1&x%actor.name% manca clamorosamente il bersaglio e picchia il martello sul suo stesso piede!&0
wait 1 s
mforce %actor.name% rim
wait 1 s
mforce %actor.name% rim
wait 1 s
mforce %actor.name% rim
wait 1 s
mforce %actor.name% urla &1&xAHHHHHHIAAAAAAAAAAAAAAAAAAAAAA&0
wait 2 s
ridi
wait 1 s
say Bella botta! Mi spiace ma non hai vinto nulla %actor.name%
wait 2 s
say Vuoi riprovare?
wait 1 s
grin
halt
end
~
#2534
Ahzthothekhon (Sheep)~
0 k 100
~
* Lista comandi
msend %actor.name% &5Ahzthothekhon ti avvolge in spire di melma e morte, stritolandoti&0
mechoaround %actor.name% &5Ahzthothekhon avvolge %actor.name% in spire di melma e morte, stritolandolo&0
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
~
#2535
Cerberus (Sheep)~
0 k 100
~
* Lista comandi
msend %actor.name% &1&xCerberus ti azzanna alla gola, tranciandoti la giugulare&0
mechoaround %actor.name% &1&xUna delle teste di Cerberus azzanna %actor.name% alla gola!&0
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
~
#2536
Custode foreste (Sheep)~
0 k 100
~
* Lista comandi
msend %actor.name% &6Il Custode ti colpisce in modo devastante con i suoi arti lignei, accartocciandoti&0
mechoaround %actor.name% &6Il Custode cala un braccio sulla testa di %actor.name%, accartocciandolo&0
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
~
#2537
Giustizia Silue (Sheep)~
0 d 1
giustizia~
* Lista comandi
set valido 0
if %actor.name% == alhandir
set valido 1
end
if %valido% == 1
if %speech% == giustizia di Silue
wait 1 s
if %puntifede% < 3000
mecho &7&xL'altare brilla per qualche secondo, ma non pare succedere nulla...&0
halt
end
mecho &7&xL'altare brilla per qualche istante...&0
mload obj 577
posa statuetta
eval puntifede %puntifede%-3000
global puntifede
mat cptifedesilue say distruggi 60
end
end
~
#2538
Giustiziere Xhyphys (Sheep)~
0 c 100
giustizia~
* Lista comandi
if %actor.name% != sagat
halt
end
if %giustiziere% != 1
wait 2
msend %actor.name% &7&xPer fare giustizia... serve Ahzthothekhon...&0
halt
end
if %arg.vnum% != -1
wait 2
msend %actor.name% Non puoi fare questo....
halt
end
set giustiziere 0
global giustiziere
mecho &5La stanza si colma di un soffocante gas viola...&0
wait 2 s
msend %actor.name% &5La tua mente vacilla mentre percepisci l'immagine di Ahzthothekhon che dilania %arg%&0 
mat %arg% msend %arg% &5Una creatura di melma e morte viene vomitata dalla terra innanzi a te...&0
mat %arg% mechoaround %arg% &5Una creatura di melma e morte compare all'improvviso...&0
mat %arg% mload mob 310
wait 2 s
mat mxpma mforce mxpma mgoto %arg%
mat %arg% mforce mxpma mkill %arg%
wait 10 s
mat mxpma mforce mxpma mgoto %arg%
mat mxpma mforce mxpma kill %arg%
wait 10 s
mat mxpma mforce mxpma mgoto %arg%
mat mxpma mforce mxpma kill %arg%
wait 20 s
mat mxpma mecho &5L'orripilante creatura scompare...&0
mat mxpma mforce mxpma mgoto 28150
mat mxpma mforce mxpma say purgami
~
#2539
Distruzione di Xhyphys (Sheep)~
0 d 1
distruzione~
* Lista comandi
set valido 0
if %actor.name% == sagat || %actor.name% == sheep
set valido 1
end
if %valido% == 1
if %speech% == distruzione di Xhyphys
wait 1 s
if %puntifede% < 3000
mecho &5L'altare resta silente...&0
halt
end
mecho &5L'altare all'improvviso vomita miliardi di insetti...&0
mload obj 578
posa statuetta
eval puntifede %puntifede%-3000
global puntifede
mat cptifedexhyphys say distruggi 60
end
end
~
#2540
Vittoria Shaarr (Sheep)~
0 d 1
vittoria~
* Lista comandi
set valido 0
if %actor.name% == knives
set valido 1
end
if %valido% == 1
if %speech% == vittoria di Shaarr
wait 1 s
if %puntifede% < 3000
mecho &1&xL'altare non reagisce...&0
halt
end
mecho &1&xL'altare avvampa in una gloria di fuoco e fiamme....&0
mload obj 580
posa statuetta
eval puntifede %puntifede%-3000
global puntifede
mat cptifedeshaarr say distruggi 60
end
end
~
#2541
Giustiziere Shaarr (Sheep)~
0 c 100
giustizia~
* Lista comandi
if %actor.name% != knives
halt
end
if %giustiziere% != 1
wait 2
msend %actor.name% &1&xPer fare giustizia... serve Cerberus.&0
halt
end
if %arg.vnum% != -1
wait 2
msend %actor.name% Non puoi fare questo....
halt
end
set giustiziere 0
global giustiziere
mecho &1&xLa stanza si riempie di fiamme!&0
wait 2 s
msend %actor.name% &1&xAd un tratto ricevi la visione di Cerberus che divora %arg%...&0 
mat %arg% msend %arg% &1&xCerberus, il Cane Infernale, compare innanzi a te!&0
mat %arg% mechoaround %arg% &1&xDa un'improvvisa esplosione di fiamme fuoriesce Cerberus..&0
mat %arg% mload mob 311
wait 2 s
mat msxas mforce msxas mgoto %arg%
mat %arg% mforce msxas mkill %arg%
wait 10 s
mat msxas mforce msxas mgoto %arg%
mat msxas mforce msxas kill %arg%
wait 10 s
mat msxas mforce msxas mgoto %arg%
mat msxas mforce msxas kill %arg%
wait 20 s
mat msxas mecho &1&xCerberus scompare avvolto dalle fiamme...&0
mat msxas mforce msxas mgoto 28150
mat msxas mforce msxas say purgami
1
~
#2542
Giustiziere Therion (Sheep)~
0 c 100
giustizia~
if %actor.name% != krogg
halt
end
if %giustiziere% != 1
wait 2
msend %actor.name% &2&xPer fare giustizia... serve il Custode.&0
halt
end
if %arg.vnum% != -1
wait 2
msend %actor.name% Non puoi fare questo....
halt
end
set giustiziere 0
global giustiziere
mecho &2&xLa vegetazione intorno a te pare ancora piu' viva del solito...&0
wait 2 s
msend %actor.name% &2&xHai la sensazione che %arg% stia passando un brutto momento...&0 
mat %arg% msend %arg% &2&xIl Custode delle Foreste nasce dalla terra innanzi a te!&0
mat %arg% mechoaround %arg% &2&xDalla terra nasce il custode della foresta...&0
mat %arg% mload mob 312
wait 2 s
mat mtxat mforce mtxat mgoto %arg%
mat %arg% mforce mtxat mkill %arg%
wait 10 s
mat mtxat mforce mtxat mgoto %arg%
mat mtxat mforce mtxat kill %arg%
wait 10 s
mat mtxat mforce mtxat mgoto %arg%
mat mtxat mforce mtxat kill %arg%
wait 20 s
mat mtxat mecho &2&xIl Custode delle Foreste scompare...&0
mat mtxat mforce mtxat mgoto 28150
mat mtxat mforce mtxat say purgami
~
#2543
Purificazione Therion (Sheep)~
0 d 1
purificazione~
set valido 0
if %actor.name% == krogg || %actor.name% == sheep
set valido 1
end
if %valido% == 1
if %speech% == purificazione di Therion
wait 1 s
if %puntifede% < 3000
mecho &6&xL'altare non reagisce...&0
halt
end
mecho &6&xL'altare sprigiona le fragranze ed i profumi dell'estate e della primavera....&0
mload obj 579
posa statuetta
eval puntifede %puntifede%-3000
global puntifede
mat cptifedetherion say distruggi 60
end
end
~
#2544
Ottovolante (Sheep)~
0 m 50
~
* Lista comandi
wait 2 s
if %amount% > 50
say Mi hai dato troppi soldi, %actor.name%
eval diff %amount% - 50
wait 1 s
dai %diff% coins %actor.name%
end
wait 3 s
say Abbiamo un coraggioso!
wait 5 s
say E sia, %actor.name%, se non temete le insidie dell'ottovolante...
wait 3 s
say ...preparatevi ad una delle esperienze piu' emozionanti della vostra vita!
wait 3 s
emote tira una leva....
wait 5 s
msend %actor.name% &1&xIl carrello dell'ottovolante si ferma e attende il tuo ingresso... non pare molto sicuro!&0
mechoaround %actor.name% &1&xLa carrozza dell'ottovolante attende che %actor.name% salga e si metta comodo...&0
wait 5 s
mforce %actor.name% emote si avvicina al carrello e si accomoda....
wait 4 s
msend %actor.name% Ma dove diamine stanno le cinture di sicurezza? Non ci sono!? Mah! Sara' meglio tenersi forte!
mechoaroundd %actor.name% &7Pare che %actor.name% sia un filino preoccupato per la mancanza delle cinture... che fifone!&0
wait 5 s
say Si parte!
wait 5 s
emote tira un'altra leva...
wait 1 s
emote si sposta nella cabina di controllo....
mgoto 1000
dg_cast 'invisibility' inserviente
mteleport %actor.name% 8809
wait 2 s
mat %actor.name% mforce %actor.name% look
wait 2 s
mat ccxoc mecho &1&xLa carrozza parte portandosi via %actor.name% verso l'ignoto... speriamo bene!&0
mat %actor.name% mecho &6&xIl carrello comincia pian piano la sua corsa... hai un pochino di nausea&0
wait 5 s
mat %actor.name% mecho &6&xLa carrozza prende sempre piu' velocita'... tutto il mondo ti pare vorticare!&0
wait 5 s
eval tiro %random.7%
if %tiro% == 1
set controllo 1
end
if %controllo% != 1
mteleport %actor.name% 8810
mat %actor.name% mforce %actor.name% look
wait 2 s
mat %actor.name% mecho &1&xLa corsa continua.. il carrello prende sempre piu' velocita'!&0
wait 5 s
mat %actor.name% mecho &6Senti il vento fischiarti nelle orecchie, la velocita' ti impedisce di tenere gli occhi aperti..&0
wait 5 s
mat %actor.name% mecho &2&x... la velocita' e' tale che non riesci piu' a distinguere le forme intorno a te...!&0
wait 5 s
eval tiro %random.6%
if %tiro% == 1
set controllo 1
end
end
if %controllo% != 1
mteleport %actor.name% 8811
mat %actor.name% mforce %actor.name% look
wait 5 s
mat %actor.name% mecho &7&xSenti lo stomaco salirti fino in gola... ma quanto corre questo coso??????&0
wait 5 s
mat %actor.name% mecho &3&x....ti viene da rimettere... non ce la fai piu'! E' terribile!&0
wait 5 s
mat %actor.name% mecho &1&xStai urlando come un pazzo ma non riesci a sentire la tua voce per la velocita'!&0
wait 5 s
eval tiro %random.5%
if %tiro% == 1
set controllo 1
end
end
if %controllo% != 1
mteleport %actor.name% 8812
mat %actor.name% mforce %actor.name% look
wait 5 s
mat %actor.name% mecho &6FUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU&0
wait 5 s
mat %actor.name% mecho &7&xMa chi te l'ha fatto fare di salire su questa macchina infernale!!&0
wait 5 s
mat %actor.name% mecho &1&xPreghi i tuoi Dei.. e fai voto che se ti fanno scendere vivo andrai tutti i giorni al tempio!&0
wait 5 s
eval tiro %random.4%
if %tiro% == 1
set controllo 1
end
end
if %controllo% != 1
mteleport %actor.name% 8814 
mat %actor.name% mforce %actor.name% look
wait 5 s
mat %actor.name% mehco &4&xFai sempre piu' fatica a reggerti... la velocita' del carrello e' ormai insostenibile!&0
wait 5 s 
mat %actor.name% mecho &3&xLe ruote del carrello sulla rotaia sprigionano scintille incandescenti... forse frena!&0
wait 5 s
mat %actor.name% mecho &1&xSei circondato da milioni di scintille mentre l'inerzia ti sta facendo perdere la presa!&0
eval tiro %random.3%
if %tiro% == 1
set controllo 1
end
end
if %controllo% != 1
mteleport %actor.name% 8815
mat %actor.name% mforce %actor.name% look
wait 5 s
mat %actor.name% mecho &3&xForse sta finendo.... la velocita' e' ormai diminuita di molto... &0
wait 5 s
mat %actor.name% mecho &7&xSi.. per fortuna questo dannato trabiccolo rallenta... sta per fermarsi&0
wait 5 s
mat %actor.name% mecho &2&xSenti tutte le budella sottosopra..... blaaahh che saporaccio in bocca!&0
wait 5 s
mat %actor.name% mecho &5&xPer non parlare della testa che gira.. gira..&0
wait 5 s
mat %actor.name% mecho &1&xFinalmente il carrello si ferma...e' finita grazie al cielo!&0
mteleport %actor.name% 8808
mat %actor.name% mforce %actor.name% look
wait 2
mechoaround %actor.name% &2%actor.name% pare un po'.... scosso...&0
wait 2 s
vis
mgoto 8808
emote esce dalla cabina di controllo ridacchiando...
wait 5 s
msend %actor.name% &7&xScendi dal carrello... traballi un po'...&0
mechoaround %actor.name% &7&x%actor.name% pare traballare un po'...&0
wait 2 s
mforce %actor.name% vomita
wait 2 s
mforce %actor.name% vomita
wait 2 s
mforce %actor.name% vomita
wait 2 s
rida
say E che sara' mai.. per un giretto di ottovolante! Smettetela di sporcare!
wait 2 s
sor
wait 2 s
say Eccovi un simpatico ricordo della vostra gita sull'Ottovolante! Cosi' vi ricorderete di noi!
wait 2 s
mload obj 8815
dai medaglia %actor.name%
posa medaglia
halt
end
if %controllo% == 1
mat %actor.name% mecho &3&xIl carrello gira troppo veloce.. non riesci a tenerti!&0
wait 5 s
mat %actor.name% mecho &1&xVieni sbalzato fuori dalla carrozza come un sasso da una fionda!&0
wait 5 s
mat %actor.name% mecho &6&xVOOOOOOOLAAAAAAAAAREEEEEEEEEEEEEEEEEEEEEEE............&0
wait 4 s
mat %actor.name% mecho &6&xOH OH!!!&0
wait 2 s
mat %actor.name% mforce %actor.name% urla &6&xVOLOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO&0
wait 5 s
mat %actor.name% msend %actor.name% &1&xHai pensato a come atterrare, vero??&0
wait 5 s
eval cheat %random.10%
if %cheat% == 1
mteleport %actor.name% 8813
else
mteleport %actor.name% 1545557
end
mat %actor.name% mforce %actor.name% yyy
mat %actor.name% msend %actor.name% &3&xAtterri da vero campione! Prendi una gran culata!&0
mat %actor.name% mechoaround %actor.name% &1&x%actor.name% piove dal cielo come una meteora!&0
wait 2 s
mat %actor.name% mforce %actor.name% look
vis
mgoto 8808
wait 1 s
emote esce dalla cabina di controllo con una strana espressione allarmata...
halt
end
~
#2545
Danno ottovolante (Sheep)~
2 c 100
yyy~
* Lista comandi
eval danno %random.50%
%damage% %actor.name% %danno%
~
#2546
Vanga (Sheep)~
1 c 1
scava~
* Lista comandi
if %cmd% == s
%force% %actor.name% south
end
if %actor.room% != 1501573
wait 2
osend %actor.name% cominci a scavare una buca...
oechoaround %actor.name% %actor.name% inizia a scavare come un pazzo..
wait 5 s
osend %actor.name% Ehi.. ma qui non c'e' niente!
oechoaround %actor.name% %actor.name% pare essersi stufato di scavare qui...
else
wait 2
osend %actor.name% cominci a scavare una buca...
oechoaround %actor.name% %actor.name% inizia a scavare come un pazzo..
wait 5 s
osend %actor.name% Hai trovato qualcosa! Pare l'ingresso ad una specie di grotta sotterranea!
oechoaround %actor.name% Ha trovato qualcosa!
%door% 1501573 down room 1718
end
~
#2547
Room Cannone (1) (Sheep)~
2 d 1
&1&xAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH&0~
* Lista comandi
eval danno %random.30%
wdamage %actor.name% %danno%
~
#2548
Cannone (Sheep)~
0 j 100
~
* Lista comandi
if %object.vnum% != 8822
wait 2 s
say Ma non puoi usare questo come protezione per la testa!
halt
end
mat xxxxx drop pentola
wait 2 s
say Molto bene! Hai trovato una protezione per la testa, posso spararti con il cannone!
wait 5 s
emote ti infila la pentola in testa!
wait 5 s
emote ti invita ad entrare nella bocca del cannone...
wait 5 s
mecoaround %actor.name% %actor.name% emote si avvicina riluttante alla ENORME bocca del cannone...
msend %actor.name% ti avvicini con baldanza alla bocca del cannone... ma sara' saggio?!?!?
wait 5 s
mechoaround %actor.name% %actor.name% si infila dentro il cannone.. non pare molto tranquillo!
msend %actor.name% Balzi con agilita' dentro la bocca del cannone.. solo la testa spunta fuori...
wait 5 s
say Ed ora, un numero eccezzzzzzzzzzzzzzzzzionale!
wait 5 s
say Spareremo il prode %actor.name% con questo cannone, centrando il bersaglio!
wait 5 s
mechoaround %actor.name% %actor.name% deglutisce...
msend %actor.name% provi a deglutire.. ma hai la bocca un pochino secca.
wait 5 s
emote si avvicina al cannone e ne modifica l'alzo..
wait 5 s
say ecco... e' tutto pronto, non temere %actor.name%, sta tranquillo, sei in mano ad un professionista!
wait 4 s
rida
wait 5 s
emote si avvicina alla miccia del cannone...
wait 4 s
emote accende la miccia..
wait 2 s
mecho &3&x5........................................&1*&0
msend %actor.name% Oddio..
wait 2 s
mecho &3&x4.............................&1*&0
msend %actor.name% oddioooo...
wait 2 s
mecho &3&x3.................&1*&0
msend %actor.name% oddioooooo...
wait 2 s
mecho &3&x2.......&1*&0
msend %actor.name% oddddiooooooo...
wait 2 s
mecho &3&x1.&1*&0
msend %actor.name% ODDDDDDIOOOOOOOOOO..............
wait 2 s
mecho &1&xBOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOM&0
wait 1
mforce %actor.name% say &1&xAAAAAAAAAAAAAHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH&0
mechoaround %actor.name% &9&xUn enorme nuvolone nero di fumo esce dal cannone...&0
eval sparo %random.10%
if %sparo% != 1
wait 1
msend %actor.name% Voli come una scheggia verso il bersaglio!
wait 1
msend %actor.name% Impatti contro il bersaglio! Ehi come e' morbidoso!
mechoaround %actor.name% %actor.name% impatta contro il bersaglio... pare sano e salvo!
wait 3 s
say Eh eh... visto, che non c'era nulla da temere?!
wait 5 s
msend %actor.name% ti rialzi.. pari tutto intero!
wait 5 s
emote si avvicina a %actor.name%
wait 5 s
say ecco il premio per il vostro coraggio!
mload obj 8823
dai casco %actor.name%
posa casco
halt
end
wait 1
msend %actor.name% stai volando come una scheggia!
mechoaround %actor.name% Oddio.. ma dove e' finito %actor.name% !?
mteleport %actor.name% 8823
wait 2 s
mat %actor.name% mforce %actor.name% urla &6&xVOLOOOOOOOOOOOO&0
wait 2 s
mat %actor.name% msend %actor.name% &6&x&fVoooooooolaaaaaaaareeeeeeeeee&0
toss
wait 5 s
mat %actor.name% msend %actor.name% &4&x&fVoooooooolaaaaaaaareeeeeeeeee&0
emote si gratta la testa...
wait 5 s
say Sono sicuro che il vostro amico sta benissimo...
mat %actor.name% msend %actor.name% &4&x&fVoooooooolaaaaaaaareeeeeeeeee&0
wait 5 s
mat %actor.name% msend %actor.name% &3&x&fVoooooooolaaaaaaaareeeeeeeeee&0
emote fischietta e fa finta di niente...
wait 3 s
eval land %random.10%
mat %actor.name% msend %actor.name% &1&xAtterri di testa! Fortuna avevi la pentola!&0
mat %actor.name% mechoaround %actor.name% &1&x%actor.name%&0 piove dal cielo come una meteora!
switch %land%
case 1
mteleport %actor.name% 2135256
break
case 2
mteleport %actor.name% 2414461
break
case 3
mteleport %actor.name% 2125055
break
case 4
mteleport %actor.name% 2094954
break
case 5
mteleport %actor.name% 1400400
break
case 6
mteleport %actor.name% 1599400
break
case 7
mteleport %actor.name% 31553
break
case 8
mteleport %actor.name% 28025
break
case 9
mteleport %actor.name% 6813
break
case 10
mteleport %actor.name% 8813
break
done
~
#2549
Uovo di Pasqua (Sheep)~
1 c 2
rompi~
* Lista comandi
if %self.vnum% != 583
halt
end
if %arg% == uovo
if %rotto% == 1
halt
end
set rotto 1
global rotto
oecho &6&xTANTI AUGURI DI BUONA PASQUA DA TUTTO LO STAFF DI DALILA2!&0
wait 1 s
oecho &1&xCOMPLIMENTI! HAI TROVATO UNA BELLISSIMA SORPRESA!&0
eval sorpresa %random.4%
switch %sorpresa%
case 1
oload obj 585
break
case 2
oload obj 586
break
case 3
oload obj 587
break
case 4
oload obj 588
break
done
otransform 584
end
~
#2550
Guardie Pari (1) (Sheep)~
0 g 100
~
* Lista comandi
wait 1 s
if %actor.clan% == 2
if %actor.rank% >= 26
emote scatta sull'attenti
wait 1
say Signore! 
msend %actor.name% i comandi per le guardie sono guardie andiamo, guardie riposo, guardie saluto
else
end
end
~
#2551
Guardie Pari (2) (Sheep)~
0 d 1
guardie~
* Lista comandi
   if %actor.clan% != 2
wait 1 s
emote guarda con sprezzo %actor.name%
halt
end
wait 1 s
if %actor.rank% >= 26
if %speech% == guardie andiamo
say Agli ordini, %actor.name%
follow %actor.name%
wait 1 s
say Pronto!!
elseif %speech% == guardie riposo
wait 1 s
say Agli ordini
wait 1 s
follow self
elseif %speech% == guardie saluto
emote si inchina con fierezza
end
else
emote guarda male %actor.name%
end
~
#2552
Pifferaio (Camp) (Sheep)~
0 c 100
camp~
* Lista comandi
wait 1
~
#2553
Guanti artigliati (Sheep)~
1 j 100
~
* Lista comandi
if %actor.name% != earien
halt
end
wait 1
oechoaround %actor.name% &1%actor.name% assume un'espressione inquietante...&0
osend %actor.name% &1Ti senti.... inquietante!&0
~
#2554
Tunnel (1) (Sheep)~
2 g 100
~
* Lista comandi
wait 1 s
wecho &9&xSenti uno sbatacchiare d'ali alle tue spalle... qualcosa ti sfiora i capelli!&0
wait 2 s
wecho &9&
wecho &9&x                    _-.                       .-_&0
wecho &9&x                _..-'(                       )`-.._&0
wecho &9&x              ./'. '||\\.       (\_/)       .//||` .`\.&0
wecho &9&x          ./'.|'.'||||\\|..     )&1*&9.&1*&9(    ..|//||||`.`|.`\.&0
wecho &9&x        ./'..|'.|| |||||\```````  "  ''''''''/||||| ||.`|..`\.&0
wecho &9&x      ./'.||'.|||| ||||||||||||.     .|||||||||||| ||||.`||.`\.&0
wecho &9&x     /'|||'.|||||| ||||||||||||{     }|||||||||||| ||||||.`|||`\&0
wecho &9&x    '.|||'.||||||| ||||||||||||{     }|||||||||||| |||||||.`|||.`&0
wecho &9&x   '.||| ||||||||| |/'   ``\||/`     '\||/''   `\| ||||||||| |||.`&0
wecho &9&x   |/' \./'     `\./          |/\   /\|          \./'     `\./ `\|&0
wecho &9&x   V    V         V          }' `\ /' `{          V         V    V&0
wait 1 s
wforce all urla AAAAAAAAAAAAAAAAAAAAAAIUTO! CHE PAURA!
~
#2555
Tunnel (2) (Sheep)~
2 g 100
~
* Lista comandi
wait 1 s
wecho &1Qualcosa di viscido ti sfiora la caviglia...&0
wait 2 s
wecho &2&x                         __ &0
wecho &2&x           ---_ ...... _/_ - &0   
wecho &2&x          /  .      ./ .'&3&x*&0\ \ &0   
wecho &2&x          : '         /__-'   \. &0
wecho &2&x         /                      )&0
wecho &2&x       _/                  >   .' &0
wecho &2&x     /   .   .       _.-" /  .'   &0
wecho &2&x     \           __/"     /.'/|   &0
wecho &2&x       \ '--  .-" /     //' |\|  &0
Wecho &2&x        \|  \ | /     //_ _ |/|  &0
wecho &2&x         `.  \:     //|_ _ _|\|  &0
wecho &2&x         | \/.    //  | _ _ |/|  &0
wecho &2&x          \_ | \/ /    \ _ _ \\\ &0
wecho &2&x              \__/      \ _ _ \|\&0
wait 1 s
wforce all urla AaaaaaaaaaaaaaaHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
~
#2556
Tunnel (3) (Sheep)~
2 g 100
~
* Lista comandi
wait 1 s
wecho &7Un rumore di catene strascicate ti fa rizzare tutti i peli...&0
wait 2 s
wforce all urla AIUTO! UN FANTASMAAAAAA!
  
~
#2557
Tunnel (4) (Sheep)~
2 g 100
~
* Lista comandi
wait 1 s
wecho &1&xSenti che qualcosa di spaventoso ti sta fissando...&0
wait 2 s
wecho &1 ,                                                               ,
wecho &1 \'.                                                           .'/
wecho &1  ),\                                                         /,(
wecho &1 /__\'.                                                     .'/__\
wecho &1 \  `'.'-.__                                           __.-'.'`  /
wecho &1  `)   `'-. \                                         / .-'`   ('
wecho &1  /   _.--'\ '.          ,               ,          .' /'--._   \
wecho &1  |-'`      '. '-.__    / \             / \    __.-' .'      `'-|
wecho &1  \         _.`'-.,_'-.|/\ \    _,_    / /\|.-'_,.-'`._         /
wecho &1   `\    .-'       /'-.|| \ |.-"   "-.| / ||.-'\       '-.    /`
wecho &1     )-'`        .'   :||  / -.\\ //.- \  ||:   '.        `'-(
wecho &1    /          .'    / \\_ |  /&xo&0&1`^'&xo&0&1\  | _// \    '.          \
wecho &1    \       .-'    .'   `--|  `"/ \"`  |--`   '.    '-.       /
wecho &1     `)  _.'     .'    .--.; |\__"__/| ;.--.    '.     '._  ('
wecho &1     /_.'     .-'  _.-'     \\ \/^\/ //     `-._  '-.     '._\
wecho &1     \     .'`_.--'          \\     //          `--._`'.     /
wecho &1      '-._' /`            _   \\-.-//   _            `\ '_.-'
wecho &1          `<     _,..--''`|    \`"`/    |`''--..,_     >`
wecho &1           _\  ``--..__   \     `'`     /   __..--``  /_
wecho &1          /  '-.__     ``'-;    / \    ;-'``     __.-'  \
wecho &1         |    _   ``''--..  \'-' | '-'/  ..--''``   _    |
wecho &1         \     '-.       /   |/--|--\|   \       .-'     /
wecho &1          '-._    '-._  /    |---|---|    \  _.-'    _.-'
wecho &1              `'-._   '/ / / /---|---\ \ \ \'   _.-'`
wecho &1                   '-./ / / / \`---`/ \ \ \ \.-'
wecho &1                       `)` `  /'---'\  ` `(`
wecho &1                      /`     |       |     `\
wecho &1                     /  /  | |       | |  \  \
wecho &1                 .--'  /   | '.     .' |   \  '--.
wecho &1                /_____/|  / \._\   /_./ \  |\_____\
wecho &1               (/      (/'     \) (/     `\)      \)&0
wait 1 s
wforce all urla UN GARGOYLE!!!! CHE PAAURA!
~
#2558
Maialini fangosi (Sheep)~
0 l 40
~
* Lista comandi
mecho &5&xIl maialino e' molto viscido e fangoso e ti sfugge!&0
dg_cast 'peace' %actor.name%
eval fuga %random.4%
if %fuga% == 1
mgoto 8819
elseif %fuga% == 2
mgoto 8820
elseif %fuga% == 3
mgoto 8821
elseif %fuga% == 4
mgoto 8822
end
~
#2559
Tunnel (5) (Sheep)~
2 g 100
~
* Lista comandi
wait 1 s
wecho &3&xUn rumore ti fa alzare la testa.... O MAMMA!&0
wait 2 s
wecho &3                                             ,--,  ,.-.&0
wecho &3               ,                   \,       '-,-`,'-.' | ._&0
wecho &3              /|           \    ,   |\          ) )/  / `-,',&0
wecho &3              [ '          |\  /|   | |        /  \|  |/`  ,`&0
wecho &3              | |       ,.`  `,` `, | |  _,...(   (      _',&0
wecho &3              \  \  __ ,-` `  ,  , `/ |,'      Y     (   \_L\&0
wecho &3               \  \_\,``,   ` , ,  /  |         )         _,/&0
wecho &3                \  '  `  ,_ _`_,-,<._.<        /         /&0
wecho &3                 ', `>.,`  `  `   ,., |_      |         /&0
wecho &3                   \/`  `,   `   ,`  | /__,.-`    _,   `\&0
wecho &3               -,-..\  _  \  `  /  ,  / `._) _,-\`       \&0
wecho &3                \_,,.) /\    ` /  / ) (-,, ``    ,        |&0
wecho &3               ,` )  | \_\       '-`  |  `(               \&0
wecho &3             /  /```(   , --, ,' \   |`<`    ,            |&0
wecho &3             /  /_,--`\   <\  V /> ,` )<_/)  | \      _____)&0
wecho &3       ,-, ,`   `   (_,\ \    |   /) / __/  /   `----`&0
wecho &3      (-, \           ) \ ('_.-._)/ /,`    /&0
wecho &3      | /  `          `/ \\ V   V, /`     /&0
wecho &3   ,--\(        ,     <_/`\\     ||      /&0
wecho &3  (   ,``-     \/|         \-A.A-`|     /&0
wecho &3 ,>,_ )_,..(    )\          -,,_-`  _--`&0
wecho &3(_ \|`   _,/_  /  \_            ,--`&0
wecho &3 \( `   <.,../`     `-.._   _,-`&0
wecho &3  `                      ```&0
wforce all urla AAAAAAAHHHHHHHHHHHHHHHHHHHHH
~
#2560
Vecchietta del Circo (Sheep)~
0 m 100
~
* Lista comandi
if %randello% == 1
wait 1 s
say Hey.. ridatemi il randello prima!
dai %amount% coins %actor.name%
halt
end
if %amount% > 100
say Mi hai dato troppi soldi, %actor.name%
eval diff %amount% - 100
wait 1 s
dai %diff% coins %actor.name%
end
wait 1 s
set randello 1
global randello
emote emette una risata sguaiata..
wait 2
emote si inumidisce le labbra con la lingua
wait 2 s
say Bravo %actor.name%! Vediamo cosa sai fare con il randello!
wait 2 s
mload obj 8832
dai randello %actor.name%
posa randello
wait 2 s
emote chiude la porta in modo che tu non ti perda per il circo quando sarai bendato...
mload obj 8833
chiudi porta west
lock porta west
wait 3 s
say indossa il randello che cominciamo il gioco!
wait 2 s
emote se la ride sotto i baffi...
~
#2561
Randello (1) (Sheep)~
1 j 100
/c~
* Lista comandi
set luogo %actor.room%
if %luogo.vnum% != 8866
  %send% %actor.name% per giocare devi essere nel posto giusto!
  halt
end
set colpo 0
osend %actor.name% &1&xAppena impugni il randello, la dolce vecchina ti lega una benda sugli occhi!&0
oechoaround %actor.name% &1&xLa vecchina benda gli occhi di %actor.name%!&0
wait 2 s
osend %actor.name% &9&xNon ci vedi piu' nulla!&0
wait 3 s
osend %actor.name% &9&xLa vecchina comincia a farti girare piu' volte su te stesso... ti gira la testa!&0
oechoaround %actor.name% &3&xLa vecchina fa girare %actor.name% su se stesso piu' volte&0
wait 3 s
oforce vecchietta say Coraggio %actor.name%, vediamo cosa riesci a colpire!
while %colpo% <= 3
wait 3 s
osend %actor.name% Ti gira tutto.. ti viene un po' da vomitare
wait 3 s
osend %actor.name% Ti concentri e vibri una possente mazzata! Che forza!
oechoaround %actor.name% &1&x%actor.name% vibra una potente mazzata contro un nemico immaginario! Che ridere!&0
wait 1 s
eval colpo %random.4%
if %colpo% <= 3
osend %actor.name% Hai colpito solo l'aria!
oechoaround %actor.name% &6&x%actor.name% non colpisce altro che aria!&0
end
done
osend %actor.name% Hai preso qualcosa!
oechoaround %actor.name% &1&x%actor.name% centra uno dei vasi, mandandolo in frantumi!&0
eval mazzata %random.10%
wait 1 s
switch %mazzata%
case 1
oecho &5&xUn peluche cade dal vaso rotto!&0
oload obj 585
break
case 2
oecho &5&xUn peluche cade dal vaso rotto!&0
oload obj 586
break
case 3
oecho &5&xUn peluche cade dal vaso rotto!&0
oload obj 587
break
case 4
oecho &5&xUn peluche cade dal vaso rotto!&0
oload obj 588
break
case 5
oecho &3&xUn luccicante anello cade dal vaso rotto!&0
oload obj 8834
break
case 6
oecho &7&xDei bianchissimi guanti cadono dal vaso rotto!&0
oload obj 8835
break
case 7
oecho &6&xUn cappello piumato cade dal vaso rotto!&0
oload obj 8836
break
default
osend %actor.name% &7&xDal vaso esce tanta farina bianca! Sei tutto infarinato ora!&0
oechoaround %actor.name% &7&xDal vaso esce tanta farina bianca! %actor.name% e' tutto infarinato ora!&0
break
done
oforce %actor.name% rimu erxgp
oforce %actor.name% posa erxgp
oforce vecchietta sor
wait 2 s
oforce vecchietta clap
wait 2 s
oforce vecchietta say Bel colpo!
wait 2 s
oforce vecchietta say Vuoi provare di nuovo?
wait 2 s
oforce vecchietta sor
oforce vecchietta mat xxxxx posa all.erxgp
 
~
#2562
Vecchietta (2) (Sheep)~
0 e 0
decide di posare &1&xUn enorme randello&0~
* Lista comandi
if %object.vnum% == 8832
set randello 0
global randello
wait 2
prendi randello
unlock porta
apri porta
mat xxxxx posa chiavetta
wait 10 s
mat xxxxx posa randello
~
#2563
Free~
1 j 100
~
* Lista comandi
~
#2564
Up (Sheep)~
2 c 100
u~
* Lista comandi
%teleport% %actor.name% 2504852
wforce %actor.name% look
~
#2565
Controllo Circo (3) (Sheep)~
0 j 100
~
* Lista comandi
if %object.vnum% != 8847
halt
end   
if %actor.level% > 20
wait 1 s
say Mi spiace %actor.name% ma voi siete davvero troppo alto per entrare qui! Potreste spaventare gli animali!
wait 3
give biglietto %actor.name%
posa biglietto
halt
end
if %actor.level% < 10
wait 1 s
say mi spiace %actor.name%, ma voi siete troppo piccino! Le belve feroci potrebbero divorarvi!
wait 3
give biglietto %actor.name%
posa biglietto   
halt
end
wait 1 s
say Molto bene %actor.name%, vi auguro di divertirvi nel nostro Circo!
wait 2
mechoaround %actor.name% L'omino apre la tenda e conduce %actor.name% all'interno
msend %actor.name% L'omino apre la tenda e ti conduce all'interno
mteleport %actor.name% 8801
mat xxxxx posa biglietto
~
#2566
Caricamento Pentola (Sheep)~
0 n 20
~
* Lista comandi
mload obj 8822
~
#2567
Pugnale di Anubi (1) (Sheep)~
1 j 100
~
* Lista comandi
if %actor.name% == knives
wait 2
oechoaround knives &3Knives viene avvolto da un vortice di sabbia e... insetti!&0
osend knives &3Vieni avvolto da un vortice di sabbia ed insetti!&0
wait 1 s
oechoaround knives &3Gli insetti si disperdono...&0
osend knives &3Gli insetti ti riconoscono come legittimo proprietario del Pugnale e se ne vanno&0
halt
end
wait 2
oechoaround %actor.name% &3%actor.name% viene avvolto da un vortice di insetti!&0
osend %actor.name% &3Vieni avvolto da un vortice di insetti&0
oload mob 1121
oforce sciame mkill %actor.name%
~
#2568
Pupazzo Dorylis (Sheep)~
1 h 100
~
* Lista comandi
wait 2
oecho &2La bambolina si rizza in piedi e ti guarda con aria severa!&0
wait 1 s
oecho &2La bambolina comincia a gridare cose con tono isterico!&0
wait 1 s
eval frase %random.2%
if %frase% == 1
oecho &5La bambolina dice, 'Avrei voluto farti crescere, mi dispiace solo che tu non ci sia riuscito'&0
else
oecho &5La bambolina dice, 'tu non sei proprio capace di fare il VERO GDR!&0
end
~
#2569
Pupazzo Enrique (Sheep)~
1 h 100
~
* Lista comandi
wait 2
oecho &7Il pupazzetto&2 si rizza in piedi e ti ammicca...&0
wait 1 s
oecho &7Il pupazzetto&2 comincia a parlare con voce suadente...&0
wait 1 s
eval frase %random.2%
if %frase% == 1
oecho &7Il pupazzetto&5 dice, 'L'importante non e' fare una cosa bene o male, ma farla con stile'&0
else
oecho &7Il pupazzetto&5 dice, 'Lo stile e' il metro di tutto'&0
end
~
#2570
Uomo Punkettoso (1) (Sheep)~
0 g 100
~
* Lista comandi
if %direction% == east
wait 3
say AIUTO!!! I FREAK SI STANNO RIBELLANDO!!
wait 3
say SCAPPATE VIA PRIMA CHE VI PRENDANO!!
wait 2
emote si porta disperatamente le mani nei capelli verdi!
end
~
#2571
Imp bastardo (Sheep)~
0 g 100
~
* Lista comandi
wait 2
mecho &9&xLa gabbia si chiude alle tue spalle con un sinistro clangore metallico&0
mat rfcmsn mload mob 8843
mat rfcmsn mforce ibxcr mload obj 8852
mat rfcmsn mforce ibxcr mgoto 8852
mat ibxcr mforce ibxcr close gabbia
mat ibxcr mforce ibxcr lock gabbia
mat ibxcr mforce ibxcr mgoto xxxxx
wait 2 s
emote sogghigna MOLTO maleficamente...
wait 2 s
say Buooonaaaa papppaaaaa
~
#2572
Pupazzo Sarib (Sheep)~
1 h 100
~
* Lista comandi
wait 2
oecho &1Il pupazzetto&2 si rizza in piedi con fare bellicoso&0
wait 1 s
oecho &1Il pupazzetto &2agita le spade minacciosamente!&0
wait 1 s
eval frase %random.2%
if %frase% == 1
oecho &1Il pupazzetto &5dice, 'beheero' sangre dai vostri teschghi'&0
else
oecho &1Il pupazzetto &5dice, 'io spada, tu morto'&0
end
~
#2573
Pupazzo JimmyLee (Sheep)~
1 h 100
~
* Lista comandi
wait 2
oecho &4Il pupazzetto&2 si alza e volge lo sguardo su di te&0
wait 1 s
oecho &4Il pupazzetto &2si guarda intorno in cerca di piccioni&0
wait 1 s
eval frase %random.2%
if %frase% == 1
oecho &4Il pupazzetto &5dice, 'l'unico piccione buono e' un piccione morto'&0
else
oecho &4Il pupazzetto &5dice, 'morte al re piccione'&0
end
~
#2574
Pupazzo Faramir (Sheep)~
1 h 100
~
* Lista comandi
wait 2
oecho &2&xIl pupazzetto&0&2 salta in piedi con un agile balzo&0
wait 1 s
oecho &2&xIl pupazzetto &0&2comincia a farfugliare cose senza senso!&0
wait 1 s
eval frase %random.2%
if %frase% == 1
oecho &2&xIl pupazzetto&0 &5dice, 'la morte viene silenziosa come un'alce'&0
else
oecho &2&xIl pupazzetto &0&5dice, 'RIMETTETE L'OOC!'&0
end
~
#2575
Pupazzo Ramku (Sheep)~
1 h 100
~
* Lista comandi
wait 2
oecho &2Il pupazzetto si leva pian piano in piedi&0
wait 1 s
oecho &2Il pupazzetto fa un ampio sorriso...&0
wait 1 s
eval frase %random.2%
if %frase% == 1
oecho &2Il pupazzetto&0 &5dice, 'Chi vuole ascoltare la storia di come Tolnedra vinse la guerra contro Sendaria?'&0
else
oecho &2Il pupazzetto &0&5dice, 'Ho fatto una torta che racconta la vittoria di Tolnedra!'&0
end
~
#2576
Pupazzo Tyrano (Sheep)~
1 h 100
~
* Lista comandi
wait 2
oecho &2Il pupazzetto si alza sulle zampe posteriori...&0
wait 1 s
oecho &2Il pupazzetto ruggisce... se fosse un po' piu' grande potrebbe far paura!&0
wait 1 s
eval frase %random.2%
if %frase% == 1
oecho &2Il pupazzetto&0 &5dice, 'Sembra che %actor.name% indossi mutandine rosa. Combatte come una donnicciola!'&0
else
oecho &2Il pupazzetto &0&5dice, 'Sembra che %actor.name% indossi mutandine rosa. Combatte come una donnicciola!'&0
end
~
#2577
Pupazzo Baal (Sheep)~
1 h 100
~
* Lista comandi
wait 2
oecho &3Il pupazzetto &2balza in piedi con agilita'&0
wait 1 s
oecho &3Il pupazzetto &2sogghigna mostrandoti i suoi pugnali!&0
wait 1 s
eval frase %random.2%
if %frase% == 1
oecho &3Il pupazzetto&0 &5dice, 'puoi anche uccidermi...ma repoppero'...e con un equip nuovo di zecca! Muwahahhaah'&0
else
oecho &3Il pupazzetto &0&5dice, 'puoi anche uccidermi...ma repoppero'...e con un equip nuovo di zecca! Muwahahhaah'&0
end
~
#2578
Trecarte (1) (Sheep)~
0 m 50
~
* Lista comandi
if %brz% == 1
wait 1 s
say Fammi finire la barzelletta prima!
give %amount% coins %actor.name%
halt
end
if %gioco% == 1
wait 1 s
say Prima finiamo questa partita!
give %amount% coins %actor.name%
halt
end
eval diff %amount%-50
if %diff% > 0
wait 2 s
emote strabuzza gli occhi!
wait 2 s
say Mi avete dato troppi soldi! Eccovi il resto!
give %diff% coin %actor.name%
end
set gioco 1
global gioco
set player %actor.name%
global player
wait 2 s
emote strabuzza gli occhi!
wait 2 s
say E' evidente che vuoi giocare!
wait 2 s
emote strabuzza gli occhi!
wait 2 s
say Giochiamo!
wait 2 s
emote comincia a mischiare le tre carte molto velocemente!
wait 2 s
say La rossa vince, vince la rossa!
msend %actor.name% forse se stai attento riesci a seguire i movimenti della sua mano!
wait 2 s
say La mano e' piu' veloce dell'occhio! E' evidente!
wait 1 s
emote strabuzza gli occhi!
eval carta %random.3%
global carta
eval fintacarta %random.3%
wait 2 s
msend %actor.name% Ti pare proprio che la carta giusta sia la %fintacarta%!
say La rossa, vince la rossa signori!
wait 2 s
say Allora, quale carta scegli? La uno, la due o la tre?
~
#2579
Trecarte (2) (Sheep)~
0 d 1
uno due tre~
* Lista comandi
if %gioco% != 1
halt
end
if %actor.name% != %player%
wait 1 s
say Per piacere silenzio! Sta giocando %player%!
halt
end
if %speech% /= uno
set scelta 1
elseif %speech% /= due
set scelta 2
elseif %speech% /= tre
set scelta 3
end
wait 2 s
emote strabuzza gli occhi!
wait 3 s
emote gira pian piano la carta %scelta%
if %scelta% == %carta%
wait 2 s
emote strabuzza gli occhi!
wait 2 s
say Hai vinto! E' evidente!
set gioco 0
global gioco
else
wait 2 s
emote strabuzza gli occhi!
wait 2 s
say Hai perso! E' evidente!
wait 2 s
emote strabuzza gli occhi!
set gioco 0
global gioco
halt
end
eval vincita %random.8%
eval vincita 17+%vincita%
wait 2 s
emote strabuzza gli occhi!
wait 2 s
say Eccoti un bel premio!
mload obj 56%vincita%
give pupazz %actor.name%
posa pupazz
~
#2580
Saluto Bruno (Sheep)~
0 g 100
~
* Lista comandi
if %gioco% == 1
halt
end
set brz 0
global brz
wait 2 s
emote strabuzza gli occhi
wait 2 s
say Benvenuto %actor.name%!
wait 2 s
say Se vuoi giocare al gioco delle tre carte sono 50 monete!
wait 2 s
say E' evidente che vincerai bellissimi premi!
~
#2581
Trecarte (3) (Sheep)~
0 b 10
~
* Lista comandi
if %gioco% == 1
halt
end
set brz 1
global brz
emote strabuzza gli occhi
wait 2 s
say Ora vi racconto una storiella!
wait 2 s
rida
wait 2 s
eval barz %random.10%
switch %barz%
case 1
say Due Genidian con una pancia enorme arrivano innanzi al loro Precettore di Gilda.
wait 3 s
say Cosa vi e' successo, domanda il Precettore?
wait 3 s
emote strabuzza gli occhi
wait 2 s
say Maestro, e' stato lei la scorsa settimana a dirci di smetterla di fare gli stronzi!
break
case 2
say Knives e' rimasto a corto di soldi, decide allora di rapire il figlioletto di Slayer.
wait 2 s
say Va a Lamorka, rapisce il bimbo e gli dice: -Ti ho rapito, ora farai quello che ti ordino!-
wait 3 s
say Il bambino annuisce timidamente, allora Knives prende un foglio e scrive:
wait 2 s
say HO RAPITO TUO FIGLIO. SE LO VUOI RIVEDERE TUTTO INTERO, LASCIA UNA SACCO CON DENTRO 10 MILIONI DI MONETE ALLA QUERCIA DEI PELOI ENTRO LE TRE DI DOMANI
wait 3 s 
say Poi attacca il foglietto alla maglia del bambino e lo manda a casa dicendo di consegnare il foglio al padre.
wait 3 s
say Il giorno seguente, alle tre del pomeriggio, Knives va sotto la quercia e ci trova una busta. La apre trepidante: ci sono dentro 10 milioni di monete d'oro ed un biglietto che recita:
wait 2 s
say - Come hai potuto fare una cosa tanto orribile ad un tuo collega Alcione?
break
case 3
say Ieri ho conosciuto una ragazza... di faccia non e' un granche', ma di corpo.. ci va spesso!
break
case 4
say ci sono due isole, una fatta solo di pietre in cui vive un uomo ed una fatta solo di noci di cocco in cui vive un gorilla...
wait 2 s
say Dopo un po' di tempo l'uomo pensa: se ora lancio una pietra al gorilla, lui poi mi lancera' una noce di cocco...
wait 2 s
say Decide quindi di lanciarla, la lancia e... uccide il gorilla!
break
case 5
say Due Aldars sperduti nel grande deserto di polvere:
wait 2 s
say uno dice all'altro: - Devo darti due notizie, una bella e l'altra brutta! La brutta e' che anche oggi mangiamo sabbia!
wait 2 s
say l'altro, scoraggiato, dice: - E la bella?
wait 2 s
say - Guarda quanta ce n'e'!!!
break
case 6
wait 2 s
say Una bambina purtroppo priva di una gamba va dalla mamma:
wait 2 s
say -Mammina, mammina... per il prossimo Carnevale me lo cuci un bel vestito da Fatina Azzurra?
wait 2 s
say -Certo tesoro- fa la mamma - Ma perche' quello da ghiacciolo ti ha gia' stancata???
break
case 7
say Cabal va a trovare Knives nella prigione di Lamorka
wait 2 s
say - Allora hai avuto problemi con la lima che avevo messo nella torta?
wait 2 s
say - Si... mi operano domani...
break
case 8
say "Non e' questo il verso", come disse quello che s'impicco' per i piedi.
break
case 9
say Talos e Sarbyn sono soli soletti al parco, sentendosi alquanto arzilli si propongono una sfida:
wait 2 s
say - Senti... facciamo a chi piscia piu' lontano?
wait 2 s
say - Vai!- Incominciano a cercarseli e partono ad urinare, poco dopo:
wait 2 s
say - Io mi sono pisciato sui piedi...
wait 2 s
say - Hai vinto...
break
case 10
say Il vecchio mago Spini si sposa una giovincella diciottenne. La prima notte di nozze si spogliano entrambi ed il vecchio tira su tre dita...
wait 2 s
say La giovante tutta contenta:
wait 2 s
say - Oh mio caro, vuol dire che lo faremo tre volte stanotte?
wait 2 s
say - No, vuol dire che puoi scegliere il dito!
break
default
break
done
wait 2 s
emote strabuzza gli occhi!
wait 2 s
ridi
set brz 0
global brz
~
#2582
Fagiolo canterino (Sheep)~
1 b 80
~
* Lista comandi
oforce all scorr
~
#2583
Pentolaccia (3) (Sheep)~
0 c 100
yhtuityu~
* Lista comandi
wait 3
mat vecchierella mpurge erxgp
~
#2584
Piccione viaggiatore (1) (Sheep)~
0 j 100
~
* Lista comandi
if %go% == 1
halt
end
wait 1
eval peso %object.peso%+%peso%
global peso
if %peso% > 5
wait 2 s
mecho &7Il piccione non ce la fa a portare tutto questo peso!&0
posa all
set peso 0
global peso
halt
end
wait 2
msend %actor.name% Questo e' un piccione viaggiatore, ora devi dirgli da chi deve portare il tuo oggetto
wait 2
msend %actor.name% il comando e': vai tizio (***sostituisci tizio con il nome del bersaglio***)
~
#2585
Piccione viaggiatore (2) (Sheep)~
0 c 100
vai~
* Lista comandi
if %go% == 1
halt
end
if %arg.vnum% != -1
emote si guarda in giro stranito... ma dove dovrei andare!?
halt
end
wait 1 s
set go 1
global go
wait 1 s
mecho &7Il piccione prende il volo verso %arg%&0
mgoto rfcmsn
wait 20 s
mat %arg% &7Vedi arrivare da lontano un piccione bianco...&0
wait 5 s
mgoto %arg%
mecho &7Un piccione viaggiatore viene a posarsi accanto a te e lascia cadere qualcosa che teneva fra le zampe&0
wait 2 s
posa all
wait 3 s
mecho &7Il piccione riprende il volo&0
mgoto rfcmsn
wait 20 s
mgoto %actor.name%
mecho &7Il piccione e' tornato e viene appoggiarsi accanto a te... &0
posa all
wait 2 s
mecho &7Il piccione riprende il volo...&0
mgoto 28150
say purgami
~
#2586
Fischio x piccioni (1) (Sheep)~
1 j 100
~
opurge sidul
wait 1 s
osend %actor.name% Emetti un limpido fischio col tuo richiamo
oechoaround %actor.name% %actor.name% fischia nel suo richiamo per piccioni
wait 3 s
oload mob 1356
oecho &7Al tuo richiamo, un simpatico piccione arriva volando e si posa qui vicino in attesa di qualcosa...&0
wait 1 s
osend %actor.name% se darai qualcosa al tuo piccione, poi potrai dirgli di trasportarlo da chi vuoi tu
~
#2587
Bastone cornuto (Sheep)~
1 j 100
~
* Lista comandi
wait 1
osend %actor.name% &1Un'aura di un rosso cupo e malato ti circonda per qualche istante&0
oechoaround %actor.name% &1%actor.name% viene circondato da una cupa aura rossa&0
~
#2588
Glifo (Sheep)~
1 g 100
~
* Lista comandi
if %self.vnum% != 5810
halt
end
if %actor.class% != pandion
wait 1
osend %actor.name% &1Il glifo riconosce le tue mani come poco avvezze alla magia!&0
odamage %actor.name% 100
wait 1 s
osend %actor.name% &1Il glifo si dissolve&0
otransform 5311
halt
else
wait 1
osend %actor.name% &1Il glifo riconosce le tue mani come in grado di maneggiare la magia&0
end
~
#2589
Falce di luna (Sheep)~
1 j 100
~
* Lista comandi
if %actor.room% != 2464246
halt
end
wait 1 s
oecho &3Un vortice di sabbia si alza all'improvviso, risucchiandoti!&0
wait 2 s
oecho &3La sabbia ti entra nel naso e nella bocca, soffocandoti!&0
odamage all 200
wait 1 s
oteleport all 5814
~
#2590
Buca! (Sheep)~
2 g 100
~
* Lista comandi
if %actor.vnum% != -1
halt
end
wait 1
wecho &1&xSENTI LA TERRA CEDERE SOTTO I TUOI PIEDI! UNA TRAPPOLA!&0
wteleport all 5818
~
#2591
Arrampicati (Sheep)~
2 c 100
arrampicati~
* Lista comandi
wait 2
wsend %actor.name% &3Cominci ad arrampicarti facendo molta attenzione&0
wechoaround %actor.name% &3%actor.name% comincia ad arrampicarsi verso l'uscita&0
wait 10 s
wsend %actor.name% &3Stai salendo pian piano verso la luce.. un passetto per volta, mi raccomando, fai attenzione!&0
wechoaround %actor.name% &3%actor.name% continua ad arrampicarsi, lentamente ma con costanza&0
wait 10 s
wsend %actor.name% &3Appoggi cautamente il piede su una piccola sporgenza... forse questo e' un buon appoggio!&0
wechoaround %actor.name% &3%actor.name% appoggia cautamente il piede su una sporgenza...&0
wait 10 s
eval caduta %random.2%
if %caduta% == 1
wsend %actor.name% &3La presa ti sfugge e precipiti a terra!&0
wechoaround %actor.name% &3%actor.name% perde la presa e precipita! CHE BOTTA!&0
%damage% %actor.name% 100
halt
end
wsend %actor.name% &3Per un pelo non perdevi la presa! Tiri un bel respiro e continui la salita!&0
wechoaround %actor.name% &3%actor.name% perde la presa per un attimo! L'ha ripresa.. che colpo!&0
wait 10 s
wsend %actor.name% &3Sei sempre piu' stanco e la salita si fa sempre piu' difficoltosa... hai pochi appigli!&0
wechoaround %actor.name% &3%actor.name% pare in difficolta'.. non trova appigli!&0
wait 10 s
wsend %actor.name% &3Sei quasi giunto all'uscita, la speranza ti da nuove energie!&0
wechoaround %actor.name% &3%actor.name% e' quasi giunto all'uscita!&0
wait 10 s
eval caduta %random.5%
if %caduta% < 5
wsend %actor.name% &3Stai per mettere la testa fuori dalla buca!&0
wechoaround %actor.name% &3Sta per uscire dalla buca!&0
wait 2 s
wsend %actor.name% &1&xNOOO SCIVOLI PROPRIO ORA!!&0
%damage% %actor.name% 200
wechoaround %actor.name% &1&x%actor.name% SCIVOLA!&0
halt
end
wsend %actor.name% &3Stai per mettere la testa fuori dalla buca!&0
wechoaround %actor.name% &3Sta per uscire dalla buca!&0
wait 2 s
wsend %actor.name% &3Finalmente! Ti issi fuori dalla buca!&0
wechoaround %actor.name% &3%actor.name% e' uscito!!&0
wteleport %actor.name% 2465253
~
#2592
Drago Bruno (Sheep)~
0 f 100
~
* Lista comandi
return 0
mecho &3&xALLA MORTE DEL POTENTE DRAGO PARTE DELLA GROTTA COLLASSA RIVELANDO UN'USCITA!&0
%door% 5816 e room 2464756
~
#2593
Drago bruno (2) (Sheep)~
0 n 100
~
* Lista comandi
%door% 5816 e purge
~
#2594
Ronf (Sheep)~
2 b 30
~
* Lista comandi
wecho &3Un rumore ritmico e profondo ti mette in allerta..&0
wait 2 s
wecho &3Viene da nord... pare il respiro di una ENORME creatura... fa accapponare la pelle!&0
~
#2595
Bolla Planare (Sheep)~
1 j 100
~
* Lista comandi
wait 1
if %actor.room% < 1400400
osend %actor.name% Non puoi usare il Manto Planare qui!
halt
end
set room %actor.room%
osend %actor.name% &7Vieni avvolto in una bolla planare!&0
oechoaround %actor.name% &7%actor.name% viene avvolto da una bolla planare, scomparendo dalla vista&0
wait 1
oteleport %actor.name% 867
%door% 867 u room %room%
~
#2596
Fighting Roc (Sheep)~
0 k 50
~
* Lista comandi
wait 3 s
mecho &6&xIl Roc sbatte le enormi ali generando un vento tremendo!&0
dg_cast 'minute meteor'
~
#2597
Amanito (Dexter)~
1 j 100
~
* Lista comandi
if %caricato% != on
if %actor.name% == erestor
set caricato on
global caricato
wait 1 s
osend %actor.name% Porti alla bocca il flauto e suoni uno strano motivetto
oechoaround Erestor suona una strano motivetto col flauto.
wait 1 s
oforce %actor.name% shout Fungo, a me!
wait 2 s
oecho &1A&7m&1a&7n&1i&7t&1o&0 spunta dal terreno e inizia a seguire il suo padrone
wait 1 s
oload mob 820
oforce amanito foll erestor
else
end
else
end
~
#2598
Amanito 2 (Dexter)~
1 c 1
fermati~
* Lista comandi
wait 1 s
if %actor.name% != erestor
halt
end
oforce %actor.name% say Fermati
osend %actor.name% Il tuo &1A&7m&1a&7n&1i&7t&1o&0 affonda le radici nel terreno e si confonde col paesaggio
oechoaround Il Funghetto affonda le radici nel terreno e si integra nel paesaggio
opurge amanito
set caricato off
global caricato
~
#2599
Occhiali da segretaria (1) (Sheep)~
1 j 100
~
* Lista comandi
wait 1 s
if %actor.name% != earien
wait 1 s
osend %actor.name% &7Pessima idea...&0
odamage %actor.name% 5000
halt
end
oechoaround earien Earien assume un'aria accidiosa
osend earien Ora ti senti molto disponibile a sentire i problemi degli altri
wait 1 s
oforce earien say &5&x-| &1 Erminia e' molto bella!  &5|-&0
~
#2600
Kaius (1) (Sheep)~
0 k 70
~
* Lista comandi
wait 5 s
if %actor.vnum% != -1
soggh
wait 1 s
say Poveri stolti! Non usate questi mezzucci contro il grande Kaius!
wait 1 s
mecho &1&xKaius con un rapido gesto evoca una tempesta di meteore tutte intorno a se'!&0
dg_cast 'minute meteor'
dg_cast 'minute meteor'
dg_cast 'minute meteor'
dg_cast 'minute meteor'
halt
end
eval attacco %random.5%
switch %attacco%
case 1
msend %actor.name% &2&xKaius effettua un micidiale calcio volante colpendoti in pieno volto!&0
mechoaround %actor.name% &2&xKaius colpisce %actor.name% con un un micidiale calcio in volto!&0 
dg_cast 'paralize' %actor.name%
dg_cast 'paralize' %actor.name%
dg_cast 'disintegrate' %actor.name%
break
case 2
emote chiude gli occhi e pronuncia le parole: '&5ubzuguvuruhl grutq&0'.
emote scompare completamente dalla tua vista!
mgoto 998
wait 2 s
mat %actor.name% mecho &5Qualcuno chiede, 'Credete davvero di poter affrontare Kaius?'&0
wait 2 s
mat %actor.name% mecho &5Qualcuno chiede, 'Come pensate di colpire chi non vedete?'&0
wait 3 s
mat %actor.name% mecho &5Qualcuno dice, 'MORTEEEEEEEEEEEEEEEEEEEEE'&0
rigenerazione
wait 2 s
mgoto 5817
dg_cast 'color spray' %actor.name%
dg_cast 'color spray' %actor.name%
dg_cast 'color spray' %actor.name%
break
case 3
mecho &3&xKaius evoca un enorme martello luminoso che sconquassa tutto!&0
dg_cast 'minute meteor' %actor.name%
dg_cast 'earthquake' %actor.name%
break
case 4
emote si ferma un attimo, respira e soffia in faccia ad %actor.name%!
wait 1
dg_cast 'sleep' %actor.name%
dg_cast 'sleep' %actor.name%
dg_cast 'paralize' %actor.name%
dg_cast 'paralize' %actor.name%
dg_cast 'blind' %actor.name%
dg_cast 'blind' %actor.name%
break
case 5
dg_cast 'fire breath'
dg_cast 'gas breath'
dg_cast 'frost breath'
break
done
~
#2601
Kaius (2) (Sheep)~
0 l 30
~
* Lista comandi
wait 3 s
say Poveri idioti, ILLUSI!
wait 1 s
dg_cast 'heal' kaius
~
#2602
Kaius (3) (Sheep)~
0 f 100
~
* Lista comandi
return 0
mecho &6&xAlla morte del Grande Kaius senti grida furiose fuori la tenda!&0
mat rfcmsn mload mob 5821
mat rfcmsn mforce mipmk mgoto 2465250
mat rfcmsn mload mob 5821
mat rfcmsn mforce mipmk mgoto 2465251
mat rfcmsn mload mob 5821
mat rfcmsn mforce mipmk mgoto 2465249
mat rfcmsn mload mob 5821
mat rfcmsn mforce mipmk mgoto 2465149
mat rfcmsn mload mob 5821
mat rfcmsn mforce mipmk mgoto 2465151
mat rfcmsn mload mob 5821
mat rfcmsn mforce mipmk mgoto 2465450
~
#2603
Elementale guardiano (Sheep)~
0 g 100
~
* Lista comandi
if %actor.vnum% != -1
halt
end
wait 1 s
say Fermi, mortali. Non e' ancora giunta l'ora per raggiungere il Regno del Fuoco.
wait 2 s
say Prima la Pietra di Fiamma deve essere recuperata!
~
#2604
Room 50121 (Sheep)~
2 g 100
~
* Lista comandi
wait 1 s
wecho &1&xUn vortice di Energia' Malvagia prende la forma di Ombre Demoniache&0
wait 2 s
wecho &1&xLe ombre si dispongono a difesa dell'Altare e ringhiano al tuo arrivo&0
~
#2605
Ufficio Evildwarf (1) (Sheep)~
1 c 4
scosta~
* Lista comandi
if %arg% == libreria
oforce %actor.name% emote scosta la libreria
wait 2 s
oecho &9&xUn misterioso ed oscuro passaggio segreto compare allo spostarsi della libreria!&0
%door% 50183 n room 50184
wait 10 s
oecho &9&xLa libreria torna magicamente al suo posto, nascondendo il passaggio&0
%door% 50183 n purge
end
~
#2606
Ufficio Evildwarf (2) (Sheep)~
1 c 4
scosta~
* Lista comandi
if %arg% == lanterna
oforce %actor.name% emote scosta la lanterna
wait 2 s
oecho &9&xUn meccanismo collegato alla lanterna fa si' che il passaggio segreto verso l'ufficio si riapra&0
%door% 50184 s room 50183
wait 10 s
oecho &9&xIl passaggio segreto si richiude lentamente&0
%door% 50184 s purge
end
~
#2607
Ufficio Evildwarf (3) (Sheep)~
2 g 100
~
* Lista comandi
if %direction% != down
wait 1 s
wecho &9&xUn meccanismo fa richiudere il passaggio segrto alle tue spalle&0
end
~
#2608
Cancellata Alorn (1) (Sheep)~
0 d 1
apri~
* Lista comandi
if %actor.clan% == 9 && %actor.rank% >= 25
wait 1 s
mload obj 50054
hold chiave
wait 1 s
say Agli ordini
wait 1 s
unlock cancellata
apri cancellata
wait 10 s
chiudi cancellata
lock cancellata
wait 1 s
rimu chiave
emote ripone la chiave al sicuro
mat xxxxx drop chiave
else
halt
end
~
#2609
Statua Gulamis (1) (Sheep)~
1 c 4
esamina~
* Lista comandi
if %arg% == statua
if %n% == 0
eval n %random.1000%
global n
end
osend %actor.name% Oggi ci sono %n% ricordini di piccioni sulla statua di Gulamis
else
oforce %actor.name% esamina %arg%
end
~
#2610
Stivali del Demonio (Sheep)~
1 j 100
~
* Lista comandi
wait 1 s
osend %actor.name% &1Ti senti davvero strano&0 
wait 2 s
dg_affect %actor.name% flying 0 20
while %actor.mana% > 20
wait 2
eval d %random.4%
switch %d%
case 1
oforce %actor.name% n
break
case 2
oforce %actor.name% s
break
case 3
oforce %actor.name% e
break
case 4
oforce %actor.name% w
break
done
done
oforce %actor.name% posa all
oforce %actor.name% rimu demoniaci
oforce %actor.name% posa all
oforce %actor.name% land
~
#2611
Guardia elite aldaria (1) (Sheep)~
0 g 100
~
* Lista comandi
wait 1 s
if %actor.clan% == 5
  if %actor.rank% >= 25
    emote scatta sull'attenti
    wait 1
    say Signore!
    %send% %actor.name% [INFO] I comandi per le guardie sono:
    %send% %actor.name% guardie andiamo - guardie riposo - guardie saluto
  else
    emote ti guarda con sospetto.
  end
end
 
~
#2612
Guardie elite aldariane (2) (Sheep)~
0 d 1
guardie~
* Lista comandi
if %actor.clan% != 5
wait 1 s
emote guarda con sprezzo %actor.name%
halt
end
wait 1 s
if %actor.rank% >= 25
if %speech% == guardie andiamo
say Agli ordini, %actor.name%
follow %actor.name%
wait 1 s
say Pronto!!
elseif %speech% == guardie riposo
wait 1 s
say Agli ordini
wait 1 s
follow self
elseif %speech% == guardie saluto
emote si inchina con fierezza
end
else
emote guarda male %actor.name%
end
~
#2613
Diamante (3) (Sheep)~
1 j 100
~
* Lista comandi
if %self.vnum% == 537
otransform 538
end
~
#2614
Ciondolo oscuro (Sheep)~
1 j 100
~
* Lista comandi
if %actor.name% != erestor
osend %actor.name% &x&2Voci di &0&7f&4o&2l&7l&4e&2t&7t&4i&0&x&2 urlanti imprecano con vigore &f&1SPACCANDOTI&0&x&2 i timpani !&0
odamage %actor.name% 500
else
osend %actor.name% &3Sei un &0&7Fo&4ll&0et&7to&3 te !&0
end
~
#2615
Candelabro (Sheep)~
1 c 4
spingi~
* Lista comandi
set b braccio
if %arg% == settimo %b%
oforce %actor.name% emote spinge un braccio del candelabro
wait 2 s
oecho &9&xQuando spingi il settimo braccio del candelabro un oscuro passaggio segreto si apre davanti a te!&0
%door% 50228 n room 50229
wait 10 s
oecho &9&xSenti il rumore di un meccanismo che gira mentre il passaggio si richiude&0
%door% 50228 n purge
halt
elseif %arg% == primo %b% || %arg% == secondo %b% || %arg% == terzo %b% || %arg% == quarto %b% || %arg% == quinto %b% || %arg% == sesto %b% || %arg% == ottavo %b% || %arg% == nono %b% || %arg% == decimo %b% || %arg% == undicesimo %b%
wait 1 s
oforce %actor.name% emote spinge un braccio del candelabro
wait 1 s
osend %actor.name% &1&xOh oh... una freccia parte dal muro colpendoti in pieno!&0
oechoaround %actor.name% &1&xUna freccia diparte dal muro colpendo in pieno %actor.name%&0
odamage %actor.name% 200
else
oforce %actor.name% %cmd%
end
~
#2616
Ufficio Nion (1) (Sheep)~
1 c 4
scosta~
* Lista comandi
if %arg% == lanterna
oforce %actor.name% emote scosta la lanterna
wait 2 s
oecho &9&xUn meccanismo collegato alla lanterna fa si' che il passaggio segreto verso l'ufficio si riapra&0
%door% 50229 s room 50228
wait 10 s
oecho &9&xIl passaggio segreto si richiude lentamente&0
%door% 50229 s purge
else
oforce %actor.name% %cmd%
end
~
#2617
Ufficio Nion (2) (Sheep)~
2 g 100
~
* Lista comandi
if %direction% != down
wait 1 s
wecho &9&xUn meccanismo fa richiudere il passaggio segrto alle tue spalle&0
end
~
#2618
Crazy portals (1) (Sheep)~
2 g 100
~
* Lista comandi
%door% 50159 n purge
%door% 50159 s purge
%door% 50159 e purge
%door% 50159 w purge
eval room %random.30000%
%door% 50159 n room %room%
eval room %random.30000%
%door% 50159 e room %room%
eval room %random.30000%
%door% 50159 s room %room%
eval room %random.30000%
%door% 50159 w room %room%
~
#2619
Crazy portals (2) (Sheep)~
0 g 100
~
* Lista comandi
wait 2
msend %actor.name% &1&xUno dei portali ti attrae e precipiti in esso!&0
eval dir %random.4%
switch %dir%
case 1
mforce %actor.name% n
break
case 2
mforce %actor.name% s
break
case 3
mforce %actor.name% e
break
case 4
mforce %actor.name% w
break
done
~
#2620
free to use~
2 f 100
~
%echostaff% Error
~
#2630
Chocobo (Dexter)~
1 j 100
~
* Lista comandi
if %caricato% != on
if %actor.name% == knives
set caricato on
global caricato
wait 1 s
osend %actor.name% Porti alla bocca il flauto e suoni uno strano motivetto
oechoaround Knives suona una strano motivetto col flauto
wait 1 s
oecho Dai dintorni spunta fuori un specie di pollo gigantesco tutto giallo!!
oload mob 50500
oforce chocobo follow knives
else
end
else
end
~
#2631
Chocobo 2 (Dexter)~
1 c 1
libero~
* Lista comandi
wait 1 s
if %actor.name% != knives
halt
end
oforce %actor.name% say Sei libero
oechoaround Il Chocobo fa un piccolo inchino e se ne va
opurge chocobo
set caricato off
global caricato
~
#2660
Tunnel (6) (Sheep)~
2 g 100
~
* Lista comandi
wait 1 s
wecho &5&xUna strana sensazione ti pervade... qualcosa di strano vive qui&0
wait 2 s
wecho &5                                 _.-.
wecho &5                                  ._.-.\
wecho &5                     .^         _.-'=. \\
wecho &5                   .'  )    .-._.-=-..' \'.
wecho &5                .'   .'   _.--._-='.'   |  `.  ^.
wecho &5              .'   .'    _`.-.`=-./'.-. / .-.\ `. `.
wecho &5            .'    /      _.-=-=-/ | '._)`(_.'|   \  `.
wecho &5           /    /|       _.--=.'  `. (.-v-.)/    |\   \
wecho &5         .'    / \       _.-.' \-.' `-..-..'     / \   `.
wecho &5        /     /   `-.._ .-.'      `.'  " ". _..-'  |    |
wecho &5       '      |    |   /   )        \  /   \   \    \    `.
wecho &5      /      /    /   /   /\                \   \   |      \
wecho &5     /      /    /  .'  .'\ `.        .'     \   |   \      \
wecho &5    /      /    /  /   /   \  `-    -' .`.    .  \    \     |
wecho &5   |      /    / .''\.'    | `.      .'   `.   \  |    |    |
wecho &5  .'     /    / /   |      |      .'/       `.- `./    /    |
wecho &5  |     /    .-|   /--.    / `.    |    _.-''\   |     |    \
wecho &5 .'    /  .-'  |  /    `-.|       .'\_.'      `. |`.   |    |
wecho &5 |    |.-'     / /       /           \          \ \ `. \     \
wecho &5 |    /       /  |      /             \         |  `. `.|    |
wecho &5 |   |       /   `.     |      `   .'  \        /    \  \    /
wecho &5 |   |      ///.-'.\    |       \ /    `\      / /-.  \ |    |
wecho &5 |   /      \\\\    `    \.-     |    `-.\     |/   \\\\'.   |
wecho &5  \ |        \\\|        |      / \      |          //// |  /
wecho &5  | |         '''        |     /   \     |          |//  |  \
wecho &5  \ |                    |.-  |     \  .-|          ''   |  /
wecho &5   \|                    /    |    / ` ../               / /
wecho &5                         |'   /    |    /               | /
wecho &5                         \.'  |    | `./                |/
wecho &5                         /    \   /    \
wecho &5                         \ `. /   \    /
wecho &5                          |  |     '. '
wecho &5                          /  |      |  \
wecho &5                         /   |      /   `.
wecho &5                        | | | \   .'  `.. \
wecho &5                       / / / ._`. \.'-. \`/
wecho &5                       |/ / /  `'  `  |/|/
wecho &5                        \|\|&0
wforce all urla Un vampiroooo!!!!! AIUTOOOOOO
~
$~
