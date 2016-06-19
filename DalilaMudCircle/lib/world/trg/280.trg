#28000
giardiniere~
0 g 100
~
if %actor.vnum% == -1 && %actor.rank% <= 25
if %direction% == west
wait 1 s
emote sistema la siepe
wait 1 s
say Salve %actor.name% !! Siete in visita al grande Owain??  EhEh
wait 1 s
say Spero non abbiate cattive intenzioni!!
wait 2 s
emote riprende a lavorare alla siepe
else
wait 1 s
say Arrivederci a presto, nobile %actor.name%
end
else
end
~
#28001
ronda1~
0 ab 100
~
wait 10 s
emote prendono l'elmetto per il giro di ronda.
wait 3 s
n
wait 3 s
say Cambio della guardia!!
wait 3 s
emote danno il cambio alla sentinella che si unisce alla ronda.
wait 5 s
say Giro d'ispezione!
wait 2 s
w
wait 3 s
s
wait 20 s
emote guardano il giardiniere.
wait 4 s
n
wait 20 s
e
wait 20 s
say A dopo!!
wait 2 sec
s
~
#28002
ronda2~
0 ab 100
~
wait 20 s
say Giro d'ispezione del castello!
wait 6 s
up
w
wait 20 s
n
wait 15 s
emote controlla finestre e porte.
wait 10 s
n
wait 10 s
emote saluta formalmente la guardia a custodia degli appartamenti del Re.
wait 10 s
e
wait 20 s
e
wait 20 s
s
wait 10 s
say Sulla terrazza e' tutto ok!!
wait 10 s
e
wait 20 s
e
wait 20 s
s
wait 10 s
smile
wait 10 s
w
wait 30 s
down
down
wait 10 s
emote assapora i profumi provenienti dalla cucina.
wait 3 s
say Attenti!! Viene prima l'incolumita' del nostro Re e poi le gioie del palato!!
wait 20 s
w
wait 20 s
w
wait 10 s
say Cambio della guardia!!!
wait 4 s
emote da' il cambio alla sentinella che si avvia verso la caserma.
wait 10 s
n
say Riposo!!
~
#28003
cameriere~
0 d 1
pesce verdura carne frutta torta pagnotta pizza focaccia dolce pane zuppa~
wait 1
if %speech% == pesce
give pesce %actor.name%
elseif %speech% == verdura
give verdura %actor.name%
elseif %speech% == carne
give carne %actor.name%
elseif %speech% == frutta
give frutta %actor.name%
elseif %speech% == torta
give torta %actor.name%
elseif %speech% == pagnotta
give pagnotta %actor.name%
elseif %speech% == focaccia
give focaccia %actor.name%
elseif %speech% == pizza
give pizza %actor.name%
elseif %speech% == dolce
give dolce %actor.name%
elseif %speech% == pane
give pane %actor.name%
elseif %speech% == zuppa
give zuppa %actor.name%
end
~
#28004
buffone~
0 ab 100
~
wait 20 s
eval num %random.5%
if %num% == 1
emote &1comincia una serie di capriole per la stanza.&0
wait 3 s
emote &8...e finisce per schiantarsi sul malcapitato cameriere.&0
wait 3 s
bonk
wait 1 s
say Ehm.. &x&18-)&0
halt
elseif %num% == 2
say Vi raccontero' una breve storiella....
wait 5 s
say ...quando me la saro' ricordata :)
laugh
halt
elseif %num% == 3
emote &8PPPRRRRRRRR&0
laugh
halt
elseif %num% == 4
emote si sta destreggiando con TRE birilli...
wait 3 s
say ora con QUATTRO......poi CINQUE....
wait 3 s
emote e' concentrato al massimo sui SEI birilli....ohhhhhh
wait 3 s
emote OP OP OP OOOOOOOOOO.....
bonk buffone
bonk buffone
bonk buffone
bonk buffone
bonk buffone
wait 3 s
smile
wait 2 s
bonk buffone
say et voila' :)
halt
elseif %num% == 5
say ...e ora guardate queste incredibili ombre cinesi.
eval ombra %random.3%
wait 2 s
if %ombra% == 1
emote     &x&6o___/&0
emote     &x&6 / \&0
elseif %ombra% == 2
emote     &x&1\_/&0
emote     &x&1 O&0
emote     &x&1(@)&0
elseif %ombra% == 3
emote     &x&2_/^\T&0
emote     &x&2 \_/^&0
end
end
~
#28005
barman~
0 d 1
lista lamorketto drago amaro argentario fruttato portello~
wait 1
if %speech% == lista
say Ecco la disponibilita' :
say Lamorketto, un rosso appena dolce ottimo con la carne.
say Argentario, rosato da abbinare con qualsiasi portata.
say Fruttato,   dolcissimo con frutta tritata.
say Portello,   l'eccezionale bianco di mia produzione.
say Amaro,      un eccellente digestivo.
say Drago,      una birra speciale di sei malti selezionati.
elseif %speech% == lamorketto
mload obj 28013
give lamorketto %actor.name%
curt
elseif %speech% == argentario
mload obj 28010
give argentario %actor.name%
curt
elseif %speech% == fruttato
mload obj 28011
give frutatto %actor.name%
curt
elseif %speech% == portello
mload obj 28012
give portello %actor.name%
curt
elseif %speech% == amaro
mload obj 28014
give amaro %actor.name%
curt
elseif %speech% == drago
mload obj 28010
give drago %actor.name%
curt
end
~
#28006
musico~
0 d 1
musica~
wait 1
emote comincia ad arpeggiare qualche nota.
eval num %random.17%
if %num% == 1
say Ecco per voi. :) !!MUSIC(assolpiano.mid) 
elseif %num% == 2
say Ecco per voi. :) !!MUSIC(avvent.mid) 
elseif %num% == 3
say Ecco per voi. :) !!MUSIC(bookdays.mid) 
elseif %num% == 4
say Ecco per voi. :) !!MUSIC(ciucca.mid) 
elseif %num% == 5
say Ecco per voi. :) !!MUSIC(daggerfall.mid) 
elseif %num% == 6
say Ecco per voi. :) !!MUSIC(evfalls.mid) 
elseif %num% == 7
say Ecco per voi. :) !!MUSIC(foreign.mid) 
elseif %num% == 8
say Ecco per voi. :) !!MUSIC(guerra.mid) 
elseif %num% == 9
say Ecco per voi. :) !!MUSIC(lbcastle.mid) 
elseif %num% == 10
say Ecco per voi. :) !!MUSIC(lothlori.mid) 
elseif %num% == 11
say Ecco per voi. :) !!MUSIC(life.mid) 
elseif %num% == 12
say Ecco per voi. :) !!MUSIC(melodia1.mid) 
elseif %num% == 13
say Ecco per voi. :) !!MUSIC(moonlight.mid) 
elseif %num% == 14
say Ecco per voi :) !!MUSIC(palazzo1.mid) 
elseif %num% == 15
say Ecco per voi. :) !!MUSIC(palazzo2.mid) 
elseif %num% == 16
say Ecco per voi. :) !!MUSIC(palazzo3.mid) 
else
say Ecco per voi. :) !!MUSIC(papapaaa.mid) 
end
~
#28007
entrata alla grotta sotterranea~
2 d 100
ariel~
if %speech% == ariel
wait 5
wecho lo spirito del vento ti mostra la strada per il giardino segreto
wait 5
wecho le piante si spostano per lasciarti passare scoprendo l'entrata di una caverna
wait 5
wecho il vento magico ti solleva piano
wait 5
wteleport %actor% 12
wait 1
wforce %actor% look
wait 5
wecho il vento magico ti adagia delicatamente al suolo mentre ti carezza il viso
end
else end
~
#28008
Sentinella Castello (by Phantom)~
0 g 100
~
if %actor.vnum% == -1
wait 2 s
  if %self.room% == 28004
    if %direction% != west
      tell owain Sire scusate il disturbo,&7 %actor.name% &0 e' all'&4&xuscita del castello&0
    end
  end
  
  if %self.room% == 28002
    if %direction% != east
      tell owain Sire scusate il disturbo,&7 %actor.name% &0 e' all' &1&xingresso del castello&0
    end
  end
  
  if %actor.rank% >= 27
    wait 1 s
    say Altezza ...
    wait 1 s
    bow %actor.name%
    wait 1 s
    say i miei omaggi.
  end
      
  if %actor.rank% == 26
    wait 1 s
    emo scatta sugli attenti.
  end
end
~
#28009
Telecomando Re (by Phantom)~
1 c 1
irpa~
if %actor.rank% >= 28 && %actor.clan% == 8
  if %actor.vnum% == -1
    %send% %actor% Ok
    wait 1 s
    %echo% %actor.name% suona un campanello d'argento.
    wait 2 s
    %echo% Una guardia reale arriva di corsa.
    %load% mob 28014
    wait 1 s
    %force% reale6 bow
    wait 1 s
    %echo% Una guardia Reale dice, 'Ai Vostri ordini Sire'
  end
  
end
~
#28010
Guardia Reale (by Phantom)~
0 c 0
ordina~
if %actor.rank% >= 29
set chek 0
  
  if %arg% == difendimi
    %force% %actor% say Guardie a me!!!
    rescue %actor.name%
    wait 3
    rescue %actor.name%
    wait 3
    rescue %actor.name%
    set chek 1
  end
  
  if %arg% == apri portone
    %force% %actor% say Guardia aprite il portone del castello.
    wait 1 s
    say Come Voi ordinate Sire.
    wait 1 s
    bow
    wait 1 s
    %echo% Una guardia reale si allontana di corsa
    mgoto 28149
    wait 15 s
    mgoto 28005
    %echo% Una guardia Reale arriva di corsa
    west
    mload obj 28004
    unlock portone
    open portone
    wait 3 s
    %echo% Una guardia reale si allontana di corsa
    mgoto 28149
    wait 15 s
    mat 28150 drop palazzo
    mgoto %actor.name%
    %echo% Una guardia Reale arriva di corsa
    wait 1 s
    say Il portone del castello e' aperto Sire.
    set chek 1
  end 
  if %arg% == chiudi portone
    %force% %actor% say Guardia chiudete il portone del castello.
    wait 1 s
    say Come Voi ordinate Sire.
    wait 1 s
    bow
    wait 1 s
    %echo% Una guardia reale si allontana di corsa
    mgoto 28149
    wait 15 s
    mgoto 28005
    %echo% Una guardia Reale arriva di corsa
    west
    mload obj 28004
    close portone
    lock portone
    wait 3 s
    %echo% Una guardia reale si allontana di corsa
    mgoto 28149
    wait 15 s
    mat 28150 drop palazzo
    mgoto %actor.name%
    %echo% Una guardia Reale arriva di corsa
    wait 1 s
    say Il portone del castello e' chiuso Sire.
    set chek 1
  end
  
  if %arg% == vai via
    %force% %actor% say Guardia tornate ai vostri alloggi.
    wait 1 s
    say Come Voi ordinate Sire.
    wait 1 s
    bow
    wait 1 s
    %echo% Una guardia reale si allontana di corsa
    mgoto 28150
    wait 5 s
    say purgami
    set chek 1
  end
  
  if %chek% == 0
    %send% %actor% >>>---(O.O)--->
    %send% %actor% Comandi Attivi:
    %send% %actor% ORDINA apri portone 
    %send% %actor% ORDINA chiudi portone
    %send% %actor% ORDINA vai via
    %send% %actor% ORDINA difendimi
  end
end
  
~
#28011
Apritemi (Nimrhuin)~
1 c 3
apritemi~
wait 1
if %actor.room% == 28042
 oechoaround %actor.name% &7&8Un forte lampo di luce avvolge la figura di %actor.name% che scompare con il fragore di un tuono!&0
 osend %actor.name% &7&8Vieni avvolto da un'intensa luce e ti ritrovi al sicuro fuori dalla prigione.&0
 oteleport %actor.name% 28041
 oforce %actor.name% look
end
~
#28012
Liberatelo (Nimrhuin)~
1 c 3
liberatelo~
wait 1
if %actor.room% == 28042
 oecho &3&xUn bagliore tenue accompagna la comparsa di un mistico uscio.&0
 wait 1
 oecho &3Imbocchi con sicurezza l'apertura e ti ritrovi nelle pacifiche vie di Lamorka&0
 oteleport all 28041
 oforce all look
end
~
#28013
Cucina     (Zephyrantes)~
0 d 1
arrosto pesce fritto pizza patate pastarella millefoglie aragosta caviale coniglio~
if %speech% == arrosto
%load% obj 1425
say Ottima scelta! Assaggiatelo guarnito dalle migliori spezie di Jiroch!
give arrosto %actor.name%
elseif %speech% == pesce || %speech% == fritto
%load% obj 1426
say Ecco a Voi! Appena portato dal Golfo di Lamorka
give pesce %actor.name%
elseif %speech% == pizza
%load% obj 1856
say Fatta ancora con la ricetta delle nonne di campagna! Genuina e nutriente.
give pizza %actor.name%
elseif %speech% == patate
%load% obj 3003
say Un contorno delizioso! Provatele con l'arrosto di coniglio.
give patate %actor.name%
elseif %speech% == pastarella
%load% obj 3049
say Prendete pure, le nostre paste sono sempre fresche!
give pasta %actor.name%
elseif %speech% == millefoglie
%load% obj 3052
say Degna corona per un pasto da RE!
give millefoglie %actor.name%
elseif %speech% == aragosta
%load% obj 15019
say Assaporatela con calma, di aragoste cosi' il golfo non ne e' certo pieno.
give aragosta %actor.name%
elseif %speech% == caviale
%load% obj 15020
say Questa delicatezza da il meglio col pane bianco o bruscato.
give caviale %actor.name%
elseif %speech% == coniglio
%load% obj 64
say Gustatevi questo Coniglio! Ci abbiamo messo tanta cura nel prepararlo quanto i cacciatori nel prenderlo!
give coniglio %actor.name%
else
say Potreste ripetere per favore? c'e' confusione qui e non vorrei sbagliare ricetta..
say Che piatto avete chiesto di preciso?
end
~
#28032
mordicchio trigger~
1 b 10
(null)~
oecho un piccolo opossum sbuca dalle pieghe del vestito
wait 10 s
oecho un piccolo opossum ti guarda con gli occhioni neri e lucidi
wait 10 s
oecho un piccolo opossum sembra volerti dire qualcosa
wait 10 s
oecho un piccolo opossum sparisce fra le pieghe del vestito
~
#28060
dadi~
0 j 100
~
if %object.vnum% == 28060
wait 1
mpurge goccia
wait 3 s
say &3 Magnifico! Giochiamo!! &0
eval numx %random.9% + 3
wait 3 s
emote &3 lancia i dadi con uno strano effetto!!! &0
wait 4 s
say &8 Ho fatto un bel %numx% !! &0
wait 3 s
say &3 Ora tocca a te!! &0
wait 3 s
emote &3 passa i dadi a %actor.name% &0
wait 3 s
mforce %actor% emote &6 agita bene i dadi..sogghigna..e tira!!! &0
wait 3 s
eval numy %random.11% + 1
mforce %actor% say &x&6 Ho fatto un %numy% !! &0
wait 4 s
if %numx% == %numy%
say &3 Abbiamo pareggiato....ecco il premio di consolazione!!! &0
mload obj 28061
give buono %actor.name%
elseif %numx% > %numy%
say &x&1 Ti ho spaccato in due ...ehehe.. &0
wait 2 s
say &3 Un'altra partita?? &0
else
say &3 E che cavolo...hai barato...tieni il premio!! &0 !!SOUND(gold.wav)
wait 3 s
mload obj 28060
mload obj 28060
give 2 goccia %actor.name%
end
else
say &6 Potrei ucciderti se ci riprovi!! &0
end
~
#28061
kyla1~
0 j 100
~
if %object.vnum% == 28060
wait 1
mpurge goccia
wait 3 s
set key ok
set ide %actor.name%
global key
global ide
say Ami il Rischio eh? 
wait 3 s
emote &3 scopre un tavolo rotondo con numeri dall'1 al 9 incisi sopra. &0
wait 3 s
say Su che numero vuoi Rischiare? 
else
wait 2 s
say &x&1 Non cercare di fregarmi !! &0
end
~
#28062
kyla2~
0 d 100
1 2 3 4 5 6 7 8 9~
if %key% == ok
if %ide% == %actor.name%
unset key
unset ide
wait 2 s
grin
eval num %random.9%
wait 3 s
mforce %actor% emote &6 lancia una pietra sferica e liscia sul tavolo!! &0
wait 5 s
mforce %actor% emote &6 vede la pietra girare e rigirare sui solchi del tavolo. &0
wait 5 s
mforce %actor% emote &6 segue con lo sguardo speranzoso la sfera che rallenta sempre piu'. &0
wait 5 s
mforce %actor% emote &6 quasi sviene quando la pietra si ferma sul numero &x&6 %num% !! &0
wait 3 s
if %speech% == %num%
say &x&1 Complimenti!! HAI VINTO !!! &0 !!SOUND(gold.wav)
mload obj 28060
mload obj 28060
mload obj 28060
mload obj 28060
mload obj 28060
mload obj 28060
wait 4 s
say &1 Ecco il tuo premio!!! &0
give 6 goccia %actor.name%
else
say &3 Peccato, hai PERSO!! Riprova dai!! &0 !!SOUND(graveyrd.wav)
end
else
say &3 Lasciamo finire la partita di %ide% per favore!! &0
end
else
end
~
#28063
pesca~
0 j 100
~
if %object.vnum% == 28060
wait 1 
mpurge goccia
wait 2 s
smile %actor.name%
wait 2 s
say &3 Perfetto. Vediamo se hai Fortuna oggi!! &0
eval num %random.20%
wait 3 s
emote &3 infila la mano in una cornucopia. &0
wait 4 s
emote &3 estrae lentamente un bussolotto col numero &x&1 %num% !! &0
wait 4 s
if %num% == 2  
say &x&1 WOW, hai VINTO il miglior premio!!! &0 !!SOUND(gold.wav)
mload obj 28060
mload obj 28060
mload obj 28060
mload obj 28060
mload obj 28060
wait 3 s
say &3 Ecco CINQUE gocce per %actor.name% !! &0
give 5 goccia %actor.name%
halt
elseif %num% == 20
say &x&1 Hai vinto un buonissimo premio!!! &0 !!SOUND(gold.wav)
mload obj 28060
mload obj 28060
mload obj 28060
mload obj 28060
wait 3 s
say &3 Ecco QUATTRO gocce per %actor.name% !! &0
give 4 goccia %actor.name%
halt
elseif %num% == 4 
say &3 Bene. Hai vinto un buon premio!! &0
mload obj 28064
wait 3 s
say &3 Ecco l'ambitissimo Cappello del Rischio per %actor.name% !! &0
give cappello %actor.name%
wait 1 s
laugh
halt
elseif %num% == 18
say &3 Bene. Hai vinto un buon premio!! &0
mload obj 28064
wait 3 s
say &3 Ecco l'ambitissimo Cappello del Rischio per %actor.name% !! &0
give cappello %actor.name%
laugh
halt
elseif %num% == 6 
say &3 Hai vinto una comoda pozione rinfrescante. &0
mload obj 28062
wait 3 s
give pozione %actor.name%
halt
elseif %num% == 16
say &3 Hai vinto una comoda pozione rinfrescante. &0
mload obj 28062
wait 3 s
give pozione %actor.name%
halt
elseif %num% == 8 
say &3 Complimenti hai vinto un'ottima pozione curativa. &0
mload obj 28063
wait 3 s
give pozione %actor.name%
halt
elseif %num% == 14
say &3 Complimenti hai vinto un'ottima pozione curativa. &0
mload obj 28063
wait 3 s
give pozione %actor.name%
halt
elseif %num% == 10 
say &3 Bello!! Hai vinto un BUONO per una consumazione alla taverna qui sopra. &0
wait 3 s
say &3 Attento a non ubriacarti!! &0
mload obj 28061
give buono %actor.name%
laugh
halt
elseif %num% == 12
say &3 Bello!! Hai vinto un BUONO per una consumazione alla taverna qui sopra. &0
wait 3 s
say &3 Attento a non ubriacarti!! &0
mload obj 28061
give buono %actor.name%
laugh
halt
else
say &3 Ops, che sfortuna.... non hai vinto nulla. Riprova. &0
end
else
say &x&1 Cerchi di imbrogliare?? &0
end
~
#28064
ventuno1~
0 j 100
~
if %object.vnum% == 28060
wait 1
mpurge goccia
wait 1 s
smile %actor.name%
say &3 Il 21 e' il gioco preferito dal defunto Re Otak. &0
wait 4 s
emote &3 prende un nuovo mazzo di carte e lo mischia con grande maestria. &0
wait 4 s
mforce %actor% emote &6 spacca il mazzo!! &0
wait 3 s
eval pg1 %random.10%
eval pg2 %random.10%
eval banco1 %random.10%
eval banco2 %random.10%
global pg1
global pg2
global banco1
global banco2
set key ok
global key
set ide %actor.name%
global ide
emote &3 allunga a %actor.name% una carta scoperta,un %pg1% ,e una coperta. &0
tell %actor.name% un %pg2% !!
wait 5 s
emote &3 scopre la prima delle due carte del banco, un %banco1% !! &0
wait 4 s
grin
wait 3 s
say &3 Cosa fai? &0
wait 1 s
else
say &x&1 Devo chiamare le guardie?? &0
end
~
#28065
ventuno2~
0 d 1
carta stop~
wait 3 s
if %key% == ok 
if %ide% == %actor.name%
if %speech% == carta
eval pg3 %random.10%
global pg3
emote &3 allunga a %actor.name% un'altra carta coperta. &0
tell %actor.name% un %pg3% !!
wait 4 s
if %banco1% + %banco2% > 18
say &3 Il banco e' servito!! &0
set banco3 0
global banco3
wait 1 s
grin
else
say &3 Carta per il banco!! &0
emote &3 prende un'altra carta dal mazzo. &0
eval banco3 %random.10%
global banco3
end
wait 3 s
say &3 Ne vuoi un'altra? &0
unset key
set key ok1
global key
else
end
else
say &3 Lasciamo finire la partita di %ide% per favore!! &0
end
else
end
if %key% == ok 
if %ide% == %actor.name%
if %speech% == stop
say &3 Ok, %actor.name% ha deciso di fermarsi!! &0
wait 4 s
if %banco1% + %banco2% > 18
say &3 Il banco e' servito!! &0
set banco3 0
wait 1 s
grin
else
say &3 Carta per il banco!! &0
emote &3 prende un'altra carta dal mazzo. &0
eval banco3 %random.10%
end
wait 3 s
if %banco1% + %banco2% + %banco3% > 16
say &3 Il banco e' servito!! &0
set banco4 0
wait 1 s
grin
else
say &3 Ancora carta per il banco!! &0
emote &3 prende un'altra carta dal mazzo. &0
eval banco4 %random.10%
end
wait 5 s
say &3 Vediamo il risultato!! &0
wait 2 s
eval pgtot %pg1% + %pg2%
eval bancotot %banco1% + %banco2% + %banco3% + %banco4%
unset pg1
unset pg2
unset banco1
unset banco2
unset key
unset ide
mforce %actor.name% emote &x&6 ha totalizzato %pgtot% !!!!! &0
if %pgtot% > 21
wait 2 s
say &3 %actor.name% ha  sballato !!! Il banco vince !!! &0
halt
else
end
wait 3 s
say &3 il banco invece ha fatto %bancotot% !!!!! &0
if %bancotot% > 21
wait 2 s
say &3 Il Banco ha sballato!!!! %actor.name% VINCE!!! &0 !!SOUND(gimmee.wav)
mload obj 28060
mload obj 28060
mload obj 28060
wait 4 s
say &3 Eccoti il premio !!! &0
give 3 goccia %actor.name%
halt
else
end
wait 3 s
if %pgtot% > %bancotot%
say &x&1 %actor.name% ha VINTO !!!!! &0 !!SOUND(gold.wav)
mload obj 28060
mload obj 28060
mload obj 28060
wait 4 s
say &3 Eccoti il premio !!! &0
give 3 goccia %actor.name%
else
say &3 Hai PERSO !!!!! Grazie per il contributo. &0
end
else
end
else
say &3 Lasciamo finire la partita di %ide% per favore!! &0
end
else
end  
~
#28066
ventuno3~
0 d 1
si no~
wait 4 s
if %key% == ok1 
if %ide% == %actor.name%
if %speech% == si
eval pg4 %random.10%
emote &3 allunga a %actor.name% l'ultima carta coperta. &0
tell %actor.name% un %pg4% !!
wait 4 s
if %banco1% + %banco2% + %banco3% > 16
say &3 Il banco e' servito!! &0
set banco4 0
wait 1 s
grin
else
say &3 Ultima carta anche per il banco!! &0
emote &3 prende un'altra carta dal mazzo. &0
eval banco4 %random.10%
end
wait 5 s
say &3 Scopriamo le carte !!! &0
wait 2 s
eval pgtot %pg1% + %pg2% + %pg3% + %pg4%
eval bancotot %banco1% + %banco2% + %banco3% + %banco4%
unset pg1
unset pg2
unset pg3
unset banco1
unset banco2
unset banco3
unset key
unset ide
mforce %actor.name% emote &x&6 ha totalizzato %pgtot% !!!!! &0
if %pgtot% > 21
wait 2 s
say &3 %actor.name% ha  sballato !!! Il banco vince !!! &0
halt
else
end
wait 3 s
say &3 il banco invece ha fatto %bancotot% !!!!! &0
if %bancotot% > 21
wait 2 s
say &3 Il Banco ha sballato!!!! %actor.name% VINCE!!! &0 !!SOUND(gimmee.wav)
mload obj 28060
mload obj 28060
mload obj 28060
wait 4 s
say &3 Eccoti il premio !!! &0
give 3 goccia %actor.name%
halt
else
end
wait 3 s
if %pgtot% > %bancotot%
say &x&1 %actor.name% ha VINTO !!!!! &0 !!SOUND(gold.wav)
mload obj 28060
mload obj 28060
mload obj 28060
wait 4 s
say &3 Eccoti il premio !!! &0
give 3 goccia %actor.name%
else
say &3 Hai PERSO !!!!! Grazie per il contributo. &0
end
else
end
else
say Lasciamo finire la partita di %ide% per favore!!
end
else
end
if %key% == ok1 
if %ide% == %actor.name%
if %speech% == no
say &3 Bene, %actor.name% si ferma a tre carte!! &0
wait 4 s
if %banco1% + %banco2% + %banco3% > 16
say &3 Anche il banco si ferma!! &0
set banco4 0
wait 1 s
grin
else
say &3 Ultima carta per il banco!! &0
emote &3 prende un'altra carta dal mazzo. &0
eval banco4 %random.10%
end
wait 5 s
say &3 Scopriamo le carte !!! &0
wait 2 s
eval pgtot %pg1% + %pg2% + %pg3%
eval bancotot %banco1% + %banco2% + %banco3% + %banco4%
unset pg1
unset pg2
unset pg3
unset banco1
unset banco2
unset banco3
unset key
unset ide
mforce %actor.name% emote &x&6 ha totalizzato %pgtot% !!!!! &0
if %pgtot% > 21
wait 2 s
say &3 %actor.name% ha  sballato !!! Il banco vince !!! &0
halt
else
end
wait 3 s
say &3 il banco invece ha fatto %bancotot% !!!!! &0
if %bancotot% > 21
wait 2 s
say &3 Il Banco ha sballato!!!! %actor.name% VINCE!!! &0 !!SOUND(gimmee.wav)
mload obj 28060
mload obj 28060
mload obj 28060
wait 4 s
say &3 Eccoti il premio !!! &0
give 3 goccia %actor.name%
halt
else
end
wait 3 s
if %pgtot% > %bancotot%
say &x&1 %actor.name% ha VINTO !!!!! &0 !!SOUND(gold.wav)
mload obj 28060
mload obj 28060
mload obj 28060
wait 4 s
say &3 Eccoti il premio !!! &0
give 3 goccia %actor.name%
else
say &3 Hai PERSO !!!!! Grazie per il contributo. &0
end
else
end
else
say Lasciamo finire la partita di %ide% per favore!!
end
else
end
~
#28067
corsa1~
0 j 100
~
if %object.vnum% == 28060
wait 1
mpurge goccia
wait 3 s
set key ok1
set ide %actor.name%
global key
global ide
say &3 Su quale topo vuoi scommettere? Se non lo sai guarda il Regolamento. &0
else
wait 1 s
say &x&1 Sei un baro o che cosa? &0
end
~
#28068
corsa2~
0 d 1
spidi ammazzagatti quattroventi cordileone saetta~
wait 2 s
if %key% == ok1
if %ide% == %actor.name%
unset key
set key ok2
global key
if %speech% == spidi
say &3 Benissimo, %actor.name%  ha scommesso su Spidi di Jiroch !! &0 !!SOUND(whip.wav)
set bet1 Spidi
global bet1
wait 3 s
say &3 Piazzato o vincente? &0
elseif %speech% == ammazzagatti
say &3 Bene, %actor.name%  ha scommesso su Ammazzagatti di Cimmura !! &0 !!SOUND(mortemob2.wav)
set bet1 Ammazzagatti
global bet1
wait 3 s
say &3 Piazzato o vincente? &0
elseif %speech% == quattroventi
say &3 Bene, %actor.name%  ha scommesso su Quattroventi di Deira !! &0 !!SOUND(vento1.wav)
set bet1 Quattroventi
global bet1
wait 3 s
say &3 Piazzato o vincente? &0
elseif %speech% == cordileone
say &3 Bene, %actor.name%  ha scommesso su Cordileone di Chyrellos !! &0 !!SOUND(spam.wav)
set bet1 Cordileone
global bet1
wait 3 s
say &3 Piazzato o vincente? &0
else
say &3 Bene, %actor.name%  ha scommesso su Saetta di Lamorka !! &0 !!SOUND(fireball.wav)
set bet1 Saetta
global bet1
wait 3 s
say &3 Piazzato o vincente? &0
end
else
say &6 Un momento, questa e' la corsa di %ide% !!! &0
end
else
end
~
#28069
corsa3~
0 d 1
piazzato vincente~
if %key% == ok2
if %ide% == %actor.name%
unset key
unset ide
wait 3 s
emote &3 sistema i topini sul Via e un pezzetto di formaggio sul Traguardo. &0
wait 4 s
say &3 Pronti......VIAAAAAA!! &0
wait 3 s
eval pos1 %random.5%
if %pos1% == 1
say &3 Dopo il via %bet1% e' gia' in testa ma il gruppo dietro e' agguerrito. &0
set pu1 3
elseif %pos1% == 2
say &3 Il gruppo parte compatto fin dall'inizio. %bet1% sembra in buona posizione. &0
set pu1 2
elseif %pos1% == 3
say &3 %bet1% parte spintonando tutti e guadagna la testa della corsa!! &0
set pu1 3
elseif %pos1% == 4
say &3 %bet1% non vuole saperne di correre oggi e saltella felice! &0 !!SOUND(giggle.wav) 
set pu1 1
else
say &3 Accidenti. %bet1% e' travolto dagli avversari..sembra riprendersi pero'!! &0
set pu1 2
end
wait 7 s
eval pos2 %random.6%
if %pos2% == 1
say &3 Alla prima curva %bet1% viaggia in testa che e' una meraviglia !!! &0
say &6 Il gruppo fatica a stargli dietro. &0
set pu2 5
elseif %pos2% == 2
say &3 In pieno curvone %bet1% si fa valere nel gruppo. &0
say &6 Azzanna un avversario che cosi perde contatto. &0 !!SOUND(fight4.wav)
set pu2 4
elseif %pos2% == 3
say &3 Nel curvone il gruppo viaggia compatto. &0
say &6 %bet1% e' in difficolta' ma tiene il passo!! &0
set pu2 3
elseif %pos2% == 4
say &3 %bet1% scivola e sbatte contro un avversario!! &0 !!SOUND(fall1.wav)
say &6 Per fortuna si riprende! L'altro topo giura vendetta!! &0
set pu2 2
elseif %pos2% == 5
say &3 %bet1% affronta il curvone molto bene e guadagna spazio. &0
say &6 Col muso sgambetta un avversario!! &0
set pu2 5
else
say &3 Un avversario si avvicina a %bet1% e gli morde la coda....incredibile!!! &0
say &6 %bet1% perde contatto dalla testa. &0
set pu2 1
end
wait 8 s
eval pos3 %random.7%
if %pos3% == 1
say &3 Sul rettilineo di meta' gara %bet1% conduce con un buon vantaggio sugli inseguitori!! &0
set pu3 7
elseif %pos3% == 2
say &3 A meta' gara %bet1% e' in testa ma il vantaggio e' poco. &0
set pu3 6
elseif %pos3% == 3
say &3 A meta' del rettilineo in testa ci sono tre soli topi e %bet1% e' tra questi!! &0
set pu3 5
elseif %pos3% == 4
say &3 %bet1% e' un po' staccato dalla testa quando siamo a meta' gara. &0
set pu3 4
elseif %pos3% == 5
say &3 Grande mossa di %bet1% che supera due avversari!! &0
set pu3 5
elseif %pos3% == 6
say &3 %bet1% rallenta per rispondere agli insulti!! &0 !!SOUND(death4.wav)
set pu3 2
else
say &3 Male male!! %bet1% e' ultimo. Ma puo' ancora farcela! Forza %bet1% !! &0
set pu3 1
end
wait 7 s
eval pos4 %random.7%
if %pos4% == 1
say &3 All'ultima curva %bet1% tiene saldamente la testa della corsa. &0 !!SOUND(level3.wav)
set pu4 9
elseif %pos4% == 2
say &3 Nell'ultimo curvone %bet1% comanda la corsa con un leggero vantaggio sul secondo. &0
set pu4 7
elseif %pos4% == 3
say &3 A meta' dell'ultima curva %bet1% e' in testa insieme a tre avversari. &0
set pu4 6
elseif %pos4% == 4
say &3 A meta' della seconda curva %bet1% accelera per lo sprint finale!! &0
set pu4 5
elseif %pos4% == 5
say &3 %bet1% non e' lontano dalla testa quando siamo a meta' curvone. &0
set pu4 4
elseif %pos4% == 6
say &3 Accidenti, %bet1% ha sbandato e ha perso terreno. E' quarto ora!! &0
set pu4 3
else
say &3 Nooo.crashhh. %bet1% e' andato dritto senza girare. &0 !!SOUND(death5.wav)
say &6 Ora serve solo un miracolo. &0
set pu4 2
end
wait 7 s
eval pos5 %random.7%
if %pos5% == 1
say &3 Incredibile scatto di %bet1% che brucia metri e metri verso il traguardo!!! &0
say &6 Favoloso!! &0 !!SOUND(handwind.wav)
set pu5 11
elseif %pos5% == 2
say &3 Grandi possibilita' per %bet1% che guida la corsa a pochi metri dal traguardo!!! &0
set pu5 9
elseif %pos5% == 3
say &3 Discrete possibilita' per %bet1% che e' in testa di mezzo baffo!! &0
set pu5 8
elseif %pos5% == 4
say &3 %bet1% deve dare il meglio di se' per conquistare il podio!! &0
set pu5 6
elseif %pos5% == 5
say &3 Poche possibilita' di arrivare primo per %bet1% !! Forse secondo! &0
set pu5 4
elseif %pos5% == 6
say &3 %bet1% esce malissimo dalla curva e si trova in ultima posizione ora!! &0
set pu5 2
else
say &3 Gli scommettitori inveiscono contro %bet1% che sta mangiando polvere la' dietro! &0
say &6 Che disastro!! &0 !!SOUND(death1.wav)
set pu5 1
end
wait 8 s
eval pos6 %random.6%
say &6 Sulla linea del traguardo........ &0
if %pos6% == 1
say &3 %bet1% sembra essere nettamente in vantaggio!!!! &0 !!SOUND(level2.wav)
set pu6 6
elseif %pos6% == 2
say &3 %bet1% sembrerebbe farcela per mezza coda!! &0
set pu6 5
elseif %pos6% == 3
say &3 %bet1% si fa largo a calci!! &0 !!SOUND(donkey.wav)
set pu6 4
elseif %pos6% == 4
say &3 %bet1% passa alla pari con altri concorrenti! Finale incerto!! &0
set pu6 3
elseif %pos6% == 5
say &3 %bet1% sembra non farcela maledizione! &0
set pu6 2
else
say &3 %bet1% si e' fermato a fare un bisognino!!! &0 !!SOUND(cry2.wav)
set pu6 -3
end
wait 7 s
eval tot %pu1% + %pu2% + %pu3% + %pu4% + %pu5% + %pu6%
say &6 Ora vediamo il piazzamento di %bet1% ....  &0
wait 4 s
if %speech% == piazzato
if %tot% > 29
say &x&1 Magnifico!! %bet1% e' PRIMO!! e %actor.name% vince 3 gocce!! &0 !!SOUND(yahoo.wav)
mload obj 28060
mload obj 28060
mload obj 28060
wait 2 s
give 3 goccia %actor.name%
halt
elseif %tot% > 24
say &3 SECONDO!! Bella gara di %bet1% e %actor.name% vince 2 gocce!! &0 !!SOUND(gold.wav)
mload obj 28060
mload obj 28060
wait 2 s
give 2 goccia %actor.name%
halt
elseif %tot% > 19
say &3 %bet1% e' arrivato TERZO e %actor.name% vince il premio di consolazione. &0
mload obj 28061
wait 2 s
give buono %actor.name%
halt
elseif %tot% > 15
say &3 Solo il Quarto posto per %bet1%. Peccato. &0
halt
else 
say &3 ULTIMO. Che vergogna per %bet1%!!! &0
end
elseif %speech% == vincente
if %tot% > 29
say &x&1 Alla grande!! %bet1% e' PRIMO!! e %actor.name% vince 5 gocce!! &0 !!SOUND(yahoo.*)
mload obj 28060
mload obj 28060
mload obj 28060
mload obj 28060
mload obj 28060
wait 2 s
give 5 goccia %actor.name%
halt
elseif %tot% > 24
say &3 SECONDO!! Bella gara di %bet1% e %actor.name% vince il premio di consolazione. !! &0
mload obj 28061
wait 2 s
give buono %actor.name%
halt
elseif %tot% > 19
say &3 %bet1% e' arrivato Terzo. &0
halt
elseif %tot% > 15
say &3 Solo il Quarto posto per %bet1%. Peccato. &0
halt
else
say &3 ULTIMO. Che vergogna per %bet1%!!! &0
end
else
end
unset bet1
else
say &6 Lasciamo finire la partita di %ide% per favore!! &0
end
else
end
~
#28070
buonobirra~
0 j 100
~
if %object.vnum% == 28061
wait 1
mpurge buono
wait 3 s
say Oh bene, vedo che hai Rischiato...eheh
wait 1 s
smile %actor.name%
wait 2 s
say Eccoti la tua bella birra gratis!!! Alla salute!
wait 3 s
mload obj 28065
giv boccale %actor.name%
wait 2 s
say Alla prossima.
else
end
~
#28090
Jonathan~
0 g 100
~
wait 1 s
smile
wait 1 s
say Salve %actor.name% !!
wait 1 s
emote si alza dalla sedia a dondolo e rimescola il contenuto della pignatta
wait 3 s
say Che meraviglia! Stufato di Mazzugoru appena pescato! Mai potresti sentire niente di piu' squisito anche tu girassi tutta Daresia! Vuoi favorire?
wait 6 s
emote ti porge una scodella di legno nella quale ha versato lo stufato
wait 2 s
emote si rimette seduto a giochicchiare con la sua pipa
~
#28091
Jonathan1~
0 d 100
indovinello~
set via on
global via
wait 1 s
smile %actor.name%
wait 1 s
emote si raddrizza sulla sedia dandosi un tono pacatamente beffardo.
wait 2 s
say vediamo se sei furbo come credi....dimmi di cosa sto parlando..
wait 3 s
say &3 Agguantata (su Lei ci si contava..) &0
wait 3 s
say &x&4 Messa alle strette (e cio' fu salutare) &0
wait 3 s
say &1 Fini' col dar le carte e c'indicava..... &0
wait 3 s
say &8 ...da quale parte si doveva andare. &0
wait 1 s
say Eheh...che cos'e'?
~
#28092
Jonathan2~
0 d 0
mano~
wait 1 s
if %via% == on
if %speech% == mano
say &x&1 BRAVO!!!! &0 !!SOUND(grandioso.wav)
wait 1 
say Ecoti questa chiave....apre il ripostiglio.
wait 1 s
rem chiave
give chiave %actor.name%
unset via
else
say &x&4 Sbagliato!! &0 !!SOUND(no4.wav)
wait 1 s
say Dovrai richiedermi l'enigma per avere un'altra chance.
unset via
end
else
end
~
#28098
undefined~
1 c 100
~
Nothing.
~
#28099
antonia~
0 d 100
pronto~
wait 1 s
smile
wait 1 s
emote ti porge il gelato
wait 2 s
say Allora....che differenza c'e' tra un mud orientato al...etc etc...
wait 3 s
laugh
wait 2 s
say dai sorridi...sei su DALILA2 MUD!!!! 
wait 1 s
wave
~
#28100
PomoloDiPorta~
1 c 100
premi~
oecho ciao
~
#28101
PomoloDiPorta (2) by Phantom~
2 c 100
tira anello~
if %arg% != anello
wsend %actor% Cosi non funxiona!
halt
end
wechoaround %actor% %actor.name% tira un anello di metallo verso il basso.
wsend %actor% Tiri con forza l'anello verso il basso.
wait 2s
wasound &1Senti l'acuto suono del metallo che stride contro la roccia&0
wecho &1Senti l'acuto suono del metallo che stride contro la roccia&0
wait 5s
wecho &7La grata di bronzo lentamente si apre&0
wait 3s
wecho &1&xAlcune statue iniziano a muoversi ...Non sono Squali di pietra SONO VIVI !!!&0 
wait 1s
wload mob 28199
wload mob 28199
wload mob 28199
wait 1s
wload mob 28199
wait 2s
wload mob 28199
wait 3s
wload mob 28199
wait 5s
wecho &7La grata di bronzo e' completamente aperta.&0
wdoor 28197 east purge
wdoor 28197 east room 28196
wdoor 28197 east flags a
wdoor 28197 east name grata
wdoor 28196 west flags a
wait 60s
wdoor 28197 east flags abcd
wdoor 28196 west flags abcd
wecho &7La grata di bronzo lentamente si richiude.&0
~
#28102
Legna il topo (by Phantom) v. 1~
0 g 100
~
if %actor.alias% == un topo
  wait2 s
  say &1Maledetti Topi !!!&0
  wait 1 s
  hit topo6
end
if %actor.alias% == un gatto
  wait 2 s
  say &3 Scio' scioo brutto gattaccio!!! &0
  wait 1 s
  mforce gatto6 flee
end
~
#28103
AutoGatto mangia TopoMoto (by Phantom)~
0 g 100
~
if %actor.alias% == un topo
  wait 2 s
  mecho Un gatto si lancia fulmineo sul topo.
  wait 2 s
  hit topo6
end
~
#28104
Rog il Superbo (by Phantom)~
0 g 100
~
wait 5 s
say E chiudete quella porta maleducati!!! VOLETE FARMI AMMALARE!!!
wait 1 s
mecho Rog va verso la porta.
wait 1 s
mecho Rog chiude con violenza la porta.
wait 1 s
mecho SSSBBBOOOONNNKKKKK!!!!
close porta
wait 2 s
say E che diamine !!! Non ho ancora finito la mia stupenda lapide, non posso rischiare di morire ora! 
~
#28105
La Tana del Sorcio (by Phantom)~
1 b 60
~
if %self.varexists(totaltopo)%
if %totaltopo% > 90
halt
end
else
set totaltopo 1
global totaltopo
end
set totaltopo %totaltopo%
eval num %random.36%
if %num% == 1
  oecho Un piccolo topo esce dalla sua tana.
  oload mob 28100
  eval totaltopo %totaltopo% + 1
end
if %num% == 2
  oecho Un topo lascia la sua tana.
  oload mob 28101
  eval totaltopo %totaltopo% + 1
end
global totaltopo
~
#28106
La Tana del Sorcio Fix (Sheep)~
1 n 100
~
set totaltopo 1
global totaltopo
~
#28110
Il Gabbiano (by Phantom)~
0 g 100
~
set lup 1
while %lup% <= 3
  set caos %random.4%
  
  switch %caos%
  case 1
    nord
  break
  case 2
    sud
  break
  case 3
    est
  break
  case 4
    ovest
  break
  done
  
wait 2 s
eval lup %lup% + 1
done
~
#28111
Ponte di corda (by Phantom)~
2 g 100
~
set carico %actor.peso%
eval carico %carico% + %actor.carry_weight%
wait 1 s
if %self.vnum% == 28146
  if %direction% != south
    halt
  end
  %force% corvo6 peso %carico%
end
if %self.vnum% == 28148
  if %direction% != north
    halt
  end
  %force% corvo6 peso -%carico%
end
if %self.vnum% == 28141
  if %direction% != south
    halt
  end
  %force% corvo6 peso -%carico%
end
if %self.vnum% == 28142
  if %direction% != north
    halt
  end
  %force% corvo6 peso %carico%
end
if %self.vnum% == 28140
  %force% corvo6 peso -%carico%
end
~
#28112
Ponte di corda II (by Phantom)~
0 c 100
peso~
if %actor.vnum% == 28148
  wait 1 s
  set peso %arg%
  eval pesoponte %pesoponte% + %peso%
  global pesoponte
  say Craaaaaaa craaaaaaaa ....
end
set g 2
if %pesoponte% < 0
  set pesoponte 1
  global pesoponte
end
if %pesoponte% > 200
  while %g% < 7
   mat 2814%g% %echo% Le vecchie corde del ponte stridono, alcune iniziano a sfilacciarsi.
   eval g %g% + 1
  done
end
wait 1 s
set g 2
if %pesoponte% > 400
  while %g% < 7
   mat 2814%g% %echo% SSSCCCCCRRRAAAAAAAAKKKK !!!.
   mat 2814%g% %echo% UNA CORDA SI SPEZZA !!!.
   mat 2814%g% %echo% Il ponte di corde non sara' in grado di reggere questo peso a lungo.
   eval g %g% + 1
  done
end
wait 1 s
set g 2
if %pesoponte% > 600
  while %g% < 7
   mat 2814%g% %echo% striickk...StriccccK...SSSTTTTTRRRAAAAAAAAKKKK !!!.
   mat 2814%g% %echo% Una delle corde piu' grosse si strappa con violenza !!!
   mat 2814%g% %echo% Il ponte oscilla verso il basso ... con fatica riesci a restare in equilibrio.
   mat 2814%g% %echo% Alcune assi di legno si staccano e precipitano nell'abisso.
   eval g %g% + 1
  done
end
set g 2
if %pesoponte% > 800
  while %g% < 7
   mat 2814%g% %echo% scricck...sTriccck...SCrOoOCcCKKKK !!!
   mat 2814%g% %echo% La corda a cui ti reggevi si spezza !!!
   wait 1 s
   mat 2814%g% %echo% Troppo peso su questo vecchio e maledetto ponte!!!
   mat 2814%g% %echo% Il ponte si gira su stesso sbalzandoti fuori ... Cadiiii!!!!
   mat 2814%g% %echo% Precipiti verso le nere nubi che nascondono il fondo del burrone.
   mat 2814%g% %force% all east
   mat 2814%g% mpurge
   eval g %g% + 1
  done
  mat 28148 %echo% Il ponte di corde oscilla violento, le sue corde si intrecciano.
  mat 28148 %echo% Vedi alcune persone precipitare nell'abisso tra le nere nuvole.
  mat 28141 %echo% Il ponte di corde oscilla violento, le sue corde si intrecciano.
  mat 28141 %echo% Vedi alcune persone precipitare nell'abisso tra le nere nuvole.
  set pesoponte 0
  global pesoponte
  mgoto 28149
end
~
#28113
Porta Shundar (by Phantom)~
2 d 1
*s*h*u*n*d*a*r* shundar~
wait 1 s
if %speech% /= shundar
  if %actor.room% == 28141
    %echoaround% %actor% Un fulmine di luce verde parte dalla porta e colpisce improvviso %actor.name%
    eval danno %actor.hit% - 20
     %send% %actor% Un fulmine di luce scaturisce dalla porta colpendoti in pieno.
     %damage% %actor% %danno%  
  end
  halt
end
wait 3 s
%echo% La porta di metallo sembra diventare trasparente.
wdoor 28141 north room 28139
wait 100 s
wdoor 28141 north purge
%echo% La porta di metallo emana un forte bagliore verde.
~
#28114
Cado dal Ponte (by Phantom)~
2 d 0
craaaa~
%force% all clear
%echo% .
%echo% .
%echo% . 
%echo% Una rapido volo verso un'ancor piu' rapida morte. 
wait 5 s
%force% all clear
%echo% .
%echo% .
%echo% . 
%echo% Entri nelle nere nuvole che nascondono l'abisso.
wait 5 s
%force% all clear
%echo% .
%echo% .
%echo% . 
%echo% Un fulmine ti sfiora, la caduta sembra non dover terminare mai.
wait 5 s
%force% all clear
%echo% .
%echo% .
%echo% . 
%echo% L'oscurita' delle nuvole nasconde il fondo dell'abisso, la velocita' aumenta.
wait 5 s
%force% all clear
%echo% .
%echo% .
%echo% . 
%echo% Forse sei caduto in un abisso senza fine, cadere per l'eternita'.
wait 5 s
%force% all clear
%echo% .
%echo% .
%echo% . 
%echo% La forza del vento ti costringe a chiudere i tuoi occhi.
wait 5 s
%force% all clear
%echo% .
%echo% .
%echo% . 
%echo% Una risata riecheggia nella tua mente, tutto questo non ha senso non e' reale.
wait 3 s
%force% all sleep
%teleport% all 140%random.9%4%random.99%
~
#28115
Gigante di Luce (by Phantom)~
0 g 100
~
if %actor.vnum% == -1
  wait 10 s
  %echo% Un violento bagliore di luce verde illumina la sala.
  wait 1 s
  dg_cast 'chain lightning' %actor.name%
  wait 2 s
  dg_cast 'blind' %actor.name%
end
~
#28116
L'anello di scambio (by Phantom)~
1 c 1
ruota~
say My trigger commandlist is not complete!
~
#28118
TocToc (by Phantom)~
2 c 100
bussa~
%echo% &7Toc Toc&0
%echoaround% %actor% %actor.name% bussa con forza sulla porta&0&0.
%send% %actor% Bussi con forza sulla porta.
wait 2 s
%echo% &6Uno spioncino si apre nell'occhio di pesce inciso sulla porta &0
wait 2 s
%echo% &1Qualcuno osserva con sospetto dallo spioncino e dice '&3Torna in mare merluzzo d'acqua dolce!!!&1'&0
set portacovo 1
global portacovo
~
#28119
TocToc2 (by Phantom)~
2 d 0
Cosi saluto quella balena di tua madre~
wait 1 s
if %portacovo% == 1
set portacovo 2
global portacovo
set count 0
global count
wait 1 s
%echo% &1Qualcuno esclama '&6Ahh un cliente affezionato! Entrate, entrate!&1'&0 
wait 1 s
%echo% &7Uno spioncino si chiude&0
wait 1 s
%echo% &7Una porta senza serratura si apre&0
wait 1 s
%echoaround% %actor% %actor.name% viene trascinato dentro rapidamente. 
%send% %actor% &1&xUn braccio ti afferra e ti porta dentro.&0
wait 1 s
if %actor.alias% /= uomo
wpurge uomo6
else
wteleport %actor% 28120
end
set portacovo 0
global portacovo
end
~
#28120
TocToc3 (by Phantom)~
2 b 100
~
if %portacovo% == 1
eval count %count% + 1
global count
end
if %count% > 3
set count 0
global count
set portacovo 0
global portacovo
%echo% &1Qualcuno dice '&6Vai a marcire il qualche secchio del porto pesce di fondo!!!&1'&0
wait 1 s
%echo% &7Uno spioncino si chiude&0
end
set caso %random.26%
if %caso% == 3
if %portacovo% == 0
wload mob 28119
wecho Un uomo incapucciato sotto una mantella nera arriva da sud.
wait 1 s
wforce uomo6 bussa
wait 10 s
wforce uomo6 say Cosi saluto quella balena di tua madre
end
end
~
#28130
L'Ascia Sdentata (by Phantom)~
2 c 1
decapita~
if !(%arg%)
%send% %actor% Decapitare chi?
halt
end
set condannato %arg%
if !(%condannato.name%)
%send% %actor% Non lo vedi qui.
halt
end
set condannato %condannato.name%
if %condannato% == boia
%send% %actor% Il Boia non ha nessuna intenzione di farsi la barba con la propria ascia!
halt
end
if %condannato.room% != %actor.room%
%send% %actor% Non e' qui.
halt
end
if %actor.rank% < 28
  %echo% &5Il Boia dice, 'Prendo ordini solo da Sua Maesta' !!!'&0
  halt
end
%send% %actor% Ok
global condannato
set hp %condannato.hit%
eval danno %hp% / 2
if !(%danno%)
%echo% &5Il Boia dice, 'Nah ... oggi mi gira la testa.'&0
wait 2 s
%echo% &5Il Boia dice, 'L'ultima volta che ho lavorato in queste condizioni, l'ho tagliato a pezzi ma non sono riuscito ad ucciderlo!!!'&0
halt
end
wait 5 s
%echo% &6 L'assistente del boia sistema una cesta di vimini ai piedi del ceppo. &0
wait 2 s
%echo% &6 L'assistente del boia costringe %condannato% ad inginocchiarsi di fronte al ceppo.&0
wait 3 s
%echo% &6 Con calma l'assistente del boia lega con una cinta di cuoio il busto di %condannato% alla base del ceppo. &0
wait 4 s
%echo% &6 Il boia si sistema il cappuccio e osserva con attenzione il collo del condannato. &0
wait 5 s
%echo% &6 Il boia solleva lentamente l'enorme ascia.&0
wait 2 s
%echo% &6 Il boia inarca la schiena per imprimere forza al colpo.&0
wait 1 s
%echo% &6 L'enorme ascia inesorabile e fulminea cade sul collo di %condannato% &0
wait 1 s
eval caso %random.5%
switch %caso%
case 1
%echoaround% %condannato% L'ascia del boia non riesce a troncare di netto il collo di %condannato%.
wait 1 s
%send% %condannato% Un dolore atroce attraversa il tuo corpo mentre la fredda lama del boia recide il tuo collo
wait 1 s
%send% %condannato% Il boia non ti ha colpito in pieno, la ferita e' grave ma sei ancora vivo.
wait 1 s
%damage% %condannato% %danno%
wait 3 s
%echoaround% %condannato% %condannato% urla in preda al dolore, il sangue sgorga copioso dalla profonda ferita.
wait 1 s
%echo% &6 Il boia fissa con aria furiosa il suo assistente.&0
wait 2 s
%echo% &5Il Boia dice, 'Ti sei dimenticato un'altra volta di affilare la lama della mia ascia!'&0
wait 2 s
%echo% &6 Il boia solleva lentamente l'enorme ascia.&0
wait 2 s
%echo% &5Il Boia esclama, 'Siamo qui per ucciderli, non per torturarli!'&0
wait 2 s
%echo% &6 Il boia inarca la schiena per imprimere forza al colpo.&0
wait 1 s
%echo% &6 L'enorme ascia inesorabile e fulminea cade sul collo di %condannato% &0
case 2
case 3
case 4
case 5
eval danno %condannato.hit% +500
%damage% %condannato% %danno%
break
done
wait 1 s
%echo% &6 La testa di %condannato% cade nella cesta di vimini. &0
if %caso% > 4
%echo% &5L'Assistente del boia esclama, 'Un colpo perfetto Signore! Un taglio pulito e indolore!!!'&0
wait 2 s
end
%echo% %danno% %condannato.hit%
~
#28131
NoQuit~
2 c 1
quit~
%send% %actor% Non puoi svignartela cosi!
~
#28146
Prova Timer~
1 f 100
~
%echo% BOOOOOMMMMM
~
#28147
Attiva Poxioni (by Phantom)~
1 n 100
~
wait 6 s
 %echo% Prova Load
 oecho Prova
otimer 1000
~
#28148
Degrado Poxioni (by Phantom)~
1 g 100
~
if %actor.vnum% != -1
  halt
end
if %self.vnum% == 11220
  if %self.timer% < 5
    wait 1 s
    %damage% %actor% 10
    %send% %actor% %self.shortdesc% e' bollente !!! Ti ustioni la mano !!!
    wait 1 s
    %send% %actor% %self.shortdesc% ribolle tra le tue mani.
  end
wait 2 s
%echo% %self.shortdesc% esplode con un forte boato!
wait 1 s
%damage% %actor% 30
%send% %actor% Alcune schegge di vetro ti colpiscono... che dolore incommensurabile!!!
otransform 11228
halt
end 
if %self.vnum% == 11228
  wait 1 s
  %damage% %actor% 10
  %send% %actor% Il vetro scheggiato della fiala ti taglia .
  wait 2 s
  %send% %actor% Il graffio e' piccolo, ma fa un male incredibile!!!
  %echoaround% %actor% %actor.name% fa una smorfia di dolore.
  halt
end
~
#28149
Dado (by Phantom)~
1 h 100
~
   set naso 6
   set base 28140
   wait 1 s
   %echo% Un dado rotola e rimbalza.
   %echo% !!MUSIC(dice.wav)
   set punto %random.6%
   eval naso %base% + %punto%
   otransform %naso%
~
#28150
PurgaRoom~
2 d 1
purgami~
wait 3 s
   wpurge
   %echo% &7Come vuoi.&0 @ \\ll ^ &l
~
#28151
Nebbia Vs Boorg I (by Phantom)~
0 d 1
cerco~
if %actor.name% == boorg il cacciatore di taglie
set vittima cyrgo
global vittima
say %vittima% 
end
set vittima cyrgo
global vittima
set prova ciccio
global prova
~
#28152
Nebbia Prova Act~
0 e 0
Ti si gela il sangue quando senti il grido di morte di~
%echo% Actor %actor.name%
%echo% Victim %victim.name%
%echo% Object %object.name%
%echo% target %target.name%
%echo% Arg %arg%
~
#28160
Boorg Incassa (by Phantom)~
0 m 1
10~
if !(%stato%)
set stato inattivo
global stato
end
wait 1 s
%echo% Boorg lentamente e con cura conta le monete.
wait 5 s
say Hmmm
if %amount% < 50000
wait 1 s
say %amount% monete non bastano torna quando avrai abbastanza denaro!
give %amount% coins %actor.name%
halt
end
if %stato% != inattivo
wait 1 s
say Non posso accettare ho gia' preso un impegno, e sto partendo per una battuta di caccia.
give %amount% coins %actor.name%
halt
end
wait 1 s
say Si questi possono bastare ... per mettere una taglia sulla testa di qualcuno!!!
wait 1 s
say Non saranno restituiti in nessun caso! Questa e' la regola di Boorg!!!
wait 1 s
say Dimmi il nome della mia prossima preda! Come si chiama?
set stato attivo
global stato
set mandante %actor.name%
global mandante
~
#28161
Boorg Ascolta (by Phantom)~
0 c 1
say~
if %actor.name% == %self.name%
say %arg%
else
%force% %actor% say %arg%
end
if %actor.name% == %mandante%
if %stato% == attivo
set vittima %arg%
global vittima
set stato caccia
global stato
mremember %vittima% say Ah finalmente ti ho trovato %vittima%!!!
wait 1 s
say Bene da oggi daro' la caccia a %vittima% ...presto lo vedrai penzolare in quella gabbia!!!
wait 1 s
grin
wait 1 s
say Non si accettano reclami di nessun tipo ...fuori dai piedi ora mi devo preparare!
end
end
~
#28162
Boorg Si sveglia (by Phantom)~
0 n 100
~
   wait 3 s
   set stato inattivo
   global stato
   say ciao
   %echo% Prova %room.vnum% %room.east%
~
#28169
Uscita Demoni (Nimrhuin)~
2 g 100
~
wait 20 s
%send% %actor% &9&xAttorno a te tutto perde forma e dimensione... ti ritrovi in un posto senza luogo e senza tempo.&0
wteleport %actor.name% 1654
~
#28170
Alano Sud (by Phantom)~
0 c 100
sud~
if %actor.name% == %x%
%echo% %self.name% ringhia feroce e ti blocca il passaggio verso sud.
wait 1 s
say Grrr...rrr
elseif %actor.name% != %self.name%
%force% %actor% sud
else
sud
end
set x cyrgo
global x
~
#28171
The Hunter Preso1 (by Phantom)~
0 d 1
Grrr...rrr~
if %actor.name% == un alano
set s 7
mforce alano6 foll %x%
global s
end
~
#28181
The Hunter SOvest (by Phantom)~
0 c 100
west~
   
   if %actor.name% == %x% 
   set direx ovest
   global direx
   end
   %force% %actor% ovest
   
~
#28182
The Hunter SEst (by Phantom)~
0 c 100
est~
   
   if %actor.name% == %x%
   set direx est
   global direx
   end
   %force% %actor% est
   
~
#28183
The Hunter SNord (by Phantom)~
0 c 100
nord~
   
   if %actor.name% == %x%
   set direx nord
   global direx
   end
   %force% %actor% nord
   
~
#28184
The Hunter SSud (by Phantom)~
0 c 100
sud~
   
   if %actor.name% == %x%
   set direx sud
   global direx
   end
   mforce %actor% sud
   
~
#28185
The Hunter NoScappa (by Phantom)~
0 c 100
scappa~
if %actor.name% == %x% 
%send% %actor% Non riesci a scappare.
else
%force% %actor% scappa
end
~
#28186
The Hunter NoFlee (By Phantom)~
0 c 100
flee~
%send% %actor% Non riesci a scappare.
~
#28187
The Hunter NoGruppo (by Phantom)~
0 c 100
gruppo~
%send% %actor% Sei inseguito ... stanno per prenderti ... scappa presto!!!
~
#28188
The Hunter NoGroup (by Phantom)~
0 c 100
group~
%send% %actor% Non c'e' tempo per queste cose ... scappa scappa!!!
~
#28189
The Hunter (by Phantom)~
0 e 0
va' verso~
  if %actor.name% == %x%
   eval cont %cont% + 1
   if %cont% >= 30
   set cont 0
   %teleport% alano%s% %p1%
   end
   mat alano%s% %force% alano%s% %m1%
   global cont
   set p1 %p2%
   set p2 %p3%
   set p3 %p4%
   set p4 %p5%
   set p5 %p6%
   set p6 %p7%
   set p7 %p8%
   set p8 %p9%
   set p9 %p10%
   set p10 %p11%
   set p11 %p12%
   set p12 %self.room%
   
   set m1 %m2%
   set m2 %m3%
   set m3 %m4%
   set m4 %m5%
   set m5 %m6%
   set m6 %m7%
   set m7 %m8%
   set m8 %m9%
   set m9 %m10%
   set m10 %m11%
   set m11 %m12%
   set m12 %direx%
   
   set lup 1
   while %lup% <= 12
     global m%lup%
     global p%lup%
     eval lup %lup% + 1
   done
   
   end
  
~
#28190
Reincarnaxione (by Phantom)~
0 l 20
~
   say TUUU MORIRAI CON ME !!!
   if %actor.vnum% == -1
     %echoaround% %actor.name% Un guerriero di pietra si avventa su %actor.name%
     msend %actor.name% Un guerriero di pietra si lancia su di te cercando di immobilizzarti.
   else
     %echo% Un guerriero di pietra si avventa feroce su %actor.name%
   end
   
   dg_cast 'earthquake' %actor%
   %echo% Un guerriero di pietra esplode in mille frammenti di roccia.
   mload obj 28150
   wait 5
   mgoto 28150
   wait 3 s
   say purgami
~
#28191
Reincarnaxione II (by Phantom)~
1 b 100
~
   if %rinascita% > 90
     halt
   end
   
   set cresce %random.3%
   eval rinascita %rinascita% + %cresce%
   global rinascita
   
   if %cresce% == 3
    %echo% Alcune rocce al suolo emanano un improvviso bagliore verde.
   end
   
   if %rinascita% > 40
     %echo% Alcuni frammenti di roccia si riuniscono prendendo sembianze umane.
     oload mob 28150
     oload mob 28150
     otransform 28153
     set rinascita 100
     global rinascita
   end
~
#28192
Reincarnaxione III (by Phantom)~
0 b 100
~
   if %tex% != fatto
     set count 1
     say Lode al Potente Shundar l'unico Signore e Padrone della Daresia!
     set tex fatto
     global tex
   end
 wait 3 s 
 if %tex% == fatto
   
   set tox %random.6%
   eval count %count% + %tox%
   global count
   
   if %tox% == 3
     say Sottomettevi al potere di Shundar !!! Unico Signore e Padrone della Daresia!
   end
   
   if %count% >= 1000
     say Shundar da te nasco a te torno!
     wait 2 s
     emo unisce le mani
     wait 3 s
     %echo% Il guerriero di Shundar scompare in un lampo luce verde.
     wait 5
     mgoto 28150
     wait 3 s
     say purgami
   end
   
 end
~
#28193
LadroIpnotius (by Phantom)~
0 g 100
~
if %actor.vnum% == -1
  if %actor.level% > 5
    emo compie un gesto con la mano serrandola a pugno.
    set caos %random.3%
    switch %caos%
    case 1
    dg_cast 'blind' %actor.name%
    break
    case 2
    dg_cast 'silence' %actor.name%
    break
    case 3
    dg_cast 'poison' %actor.name%
    break
    done
tell cyrgo %actor.name%
    wait 1 s
    say Piegati al volere di Shundar!
  end
else
  
  if %actor.level% < 33
    say &1&x Che SHUNDAR ti protegga!!!&0
    wait 1 s
    %force% %actor% bow guerriero6
    halt
  end  
  if %actor.name% /= guardia
    %force% %actor% bow guerriero6
    wait 1 s
    %echo% %actor.name% cerca di ribellarsi al controllo della mente.
    wait 1 s
    %force% %actor% say NON MI AVRAI SHUNDAR!!!
    wait 1 s
    %force% %actor% hit guerriero6
    halt
  end
end 
halt
~
#28194
SCRILL (by Phantom)~
1 h 100
~
wait 1 s
%echo% Una scatola di bronzo si apre mentre urta sul terreno.
wait 1 s
%echo% Una miriade di piccoli insetti neri esce dalla scatola.
%load% mob 28151
~
#28195
&7SCRILL&0 mangiatutto (by Phantom)~
0 b 100
~
   %echo% I famelici SCRILL iniziano a divorare la scatola di bronzo
   mpurge scatola6
   wait 3 s
   %echo% I famelici SCRILL si avventano su tutto quel che trovano potrebbero mangiare anche i sassi!
   mpurge roccia6
   mpurge all
   %echo% I famelici SCRILL hanno divorato tutto ... ora iniziano a mangiarsi tra loro!!!
   wait 3 s
   %echo% L'ultimo SCRILL rimasto inizia a mordersi da solo!!! Che scena raccapricciante!
   %teleport% scrill6 28150
   wait 3 s
   say purgami
~
#28196
ProvaPhantom~
1 g 100
cancella~
%echo% - %actor.clan% -
~
#28197
Mosquitos Story (by Phantom)~
0 d 0
x~
set x %speech%
global x
~
#28198
The Eater (by Phantom)~
0 e 1
dice ucciso sud arriva opens closes fuggire!~
%echo%  ***Prova***
%echo% Actor >>> %actor.name%  RoomIn %actor.room%
%echo% Object %object% - %object.vnum% - %object.room%
%echo%  Target %target%
%echo% ARG %arg.name%
%echo%  Arg %arg% %speech%
~
#28199
Barbatrucco (by Phantom)~
2 b 100
~
wforce all clear
%echo% &7&x Che strano ... qui sta nevicando.&0
set phanto 0
while %phanto% <= 23
set loo &9.&0
set phan 0
while %phan% <= 3
eval num %random.6%
switch %num%
case 1
set loo %loo% &9......&7*&9....&0
break
case 2
set loo %loo% &9....&7&x*&0&9....&0
break
default
set loo %loo% &9..........&0
break
done
eval phan %phan% + 1
done
%echo% %loo%
eval phanto %phanto% + 1
done
wforce all look dado
~
$~
