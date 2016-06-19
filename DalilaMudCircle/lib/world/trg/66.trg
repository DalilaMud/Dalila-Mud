#6600
musica~
0 d 100
musica~
* Lista comandi
wait 2 s
say Benone. Volete un po' di musica eh?
wait 1 s
smile
wait 1 s
say Vediamo cosa posso suonarvi.
wait 2 s
emote si guarda attorno pensieroso ma poi subito gli brillano gli occhi.
eval num %random.3%
wait 4 s
if %num% == 1
say Eccovi un po di musica rilassante.  !!MUSIC(bookdays.mid)
elseif %num% == 2
say Perfetto. Ecco per voi un sound dolce dolce. !!MUSIC(forrest.mid)
else
say Pronto per voi questo ritmo allegro. !!MUSIC(life.mid)
end
~
#6601
infogenerali~
0 d 100
zonanewbie scacchiera sotterranei esercito magazzini porto castello casa mestieri informazioni~
* Lista comandi
wait 1 s
if %speech% == zonanewbie
say Esci dalle porte nord della citta', poi dirigiti ad ovest e infine a nord.
say La' troverai mob ed equipaggiamenti adatti ai primi livelli.
elseif %speech% == scacchiera
say Supera la zona newbie proseguendo verso nord fino ad uscire dalla miniwild.
say Pochi passi ancora a nord e la troverai sulla tua destra.
say La' troverai mob ed equipaggiamenti adatti per livelli leggermente superiori.
say Una seconda scacchiera con mob piu' difficili si trova a sud della miniwild.
elseif %speech% == sotterranei
say Li trovi a ovest del Tempio in cui sei nato, scendendo poi alcuni gradini.
say E' un'area molto facile e simpatica dove ogni newbie puo' trarne giovamento.
elseif %speech% == esercito
say Appena uscito dalle porte sud della citta' sulla strada per il Porto....
say ...troverai la Caserma dove potrai fare domanda per essere ammesso.
say Il Comandante in carica e' Ramku, chiedi a lui.
elseif %speech% == magazzini
say A SudEst di Piazzale degli Eroi sorgono i magazzini della citta'.
say Potrai vendere o acquistare SOLO i prodotti creati da voi giocatori.
say Altri negozi si trovano attorno al Tempio di Lamorka.
elseif %speech% == porto
say Lamorka ha il piu bel porto di Daresia! Esci dalle porte sud della citta' e....
say ...dirigiti ancora un pochino a sud. Ti troverai nel ricco porto della citta'
say dove potrai godere della vista del faro, potrai divertirti alla taverna e alla bisca,
say potrai acquistare barche e attrezzi per la pesca da Adno e potrai far partire
say le tue merci per ogni angolo della Daresia.
say Attento pero'! Al porto trovi anche la Forca costruita per tutti coloro che vogliono
say turbare l'operosa vita della nostra citta'!
elseif %speech% == castello
say Il Castello di Re Otak il Grande e' normalmente chiuso ai cittadini tranne....
say ..che in occasione di solenni festeggiamenti o per udienze personali con il re.
say Esci dalle porte nord della citta', dirigiti a nord e poi un pochino ad est.
say Entrerai nei bellissimi giardini del castello dove il giardiniere del Re ti accogliera'
say cortesemente e da dove potrai assistere al famoso cambio della guardia reale di ronda!
elseif %speech% == casa
say Per ottenere una casa rivolgiti ad un Funzionario.
elseif %speech% == mestieri
say Tutto cio' che ti serve lo trovi nel negozio a Sud del Tempio.
say Tutti i locali di produzione sono in via dei Mestieri a Sud di Piazzale degli Eroi.
elseif %speech% == informazioni
say Puoi chiedermi circa.....
say zonanewbie scacchiera sotterranei esercito magazzini porto castello casa mestieri
end
~
#6604
Mappa Lamorka Kizeridis~
0 d 1
si~
* Lista comandi
if %speech% == si && %attesa% == on
   wait 1 s
   say Certamente, ecco a Voi
   mload obj 6707
   wait 1 s
   give mappa %actor.name%
   wait 1 s
   mforce %actor% look mappa
   wait 6 s
   drop mappa
   set attesa off
   global attesa
end
~
#6605
Mappa Lamorka Kizeridis (2)~
0 g 100
~
* Lista comandi
wait 1 s
if %actor.vnum% == -1
   say Salve, volete una mappa della nostra ridente citta'?
   set attesa on
   global attesa
   wait 13 s
   if %attesa% == on
     say Vedo che non vi interessa, se cambiate idea tornate a trovarmi.
     set attesa off
     global attesa
   end
end
~
#6610
Prigioni 2 - Sendaria (JErold)~
2 d 1
evadi~
* Lista comandi
eval recita1 %random.3%
global recita1
eval recita2 %random.3%
global recita2
eval recita3 %random.3%
global recita3
eval recita4 %random.3%
global recita4
if %recita1% == 3
set recita1a &3&xLa tua abilita' di ladro riesce a sbloccare la serratura a tempo... esci presto!!&0
elseif %recita1% == 2
set recita1a &3&xEsci presto! Hai sbloccato la porta della cella!&0
else
set recita1a &3&xEsci! non c'e tempo da perdere! tra poco la serratura si riblocchera!&0
end
if %recita2% == 3
set recita2a &1&xDannazione! I passi di una guardia ti deconcentrano!&0
elseif %recita2% == 2
set recita2a &1&xIl pezzo di ferro usato per evadere si rompe nella serratura... ne devi trovare un altro!&0
else
set recita2a &1&xLa polvere della cella ti fa starnutire e perdere di concetrazione!&0
end
if %recita3% == 3
set recita3a &3&xAnche se non sei un ladro riesci a sbloccare la porta della cella... esci presto!&0
elseif %recita3% == 2
set recita3a &3&xEsci presto! Hai sbloccato la porta della cella!&0
else
set recita3a &3&xEsci! non c'e tempo da perdere! tra poco la serratura si riblocchera!&0
end
if %recita4% == 3
set recita4a &1&xEhi! Per preparare un evasione ci vuole tempo!&0
elseif %recita4% == 2
set recita4a &1&xNon ora! Non ora!&0
else
set recita4a &1&xSenti i passi delle guardie..meglio stare fermi per un po!&0
end
if %lock% == OFF
set lock ON
global lock
if %actor.class% == alcione
eval percentuale_fuga %random.1000%
global percentuale_fuga
if %percentuale_fuga% > 998
wecho %actor.name% %recita1a%
wdoor 6813 north flags a
wait 10 s
wdoor 6813 north flags bcd
else
wecho %actor.name% %recita2a%
end
else
eval percentuale_fuga1 %random.2000%
global percentuale_fuga1
if %percentuale_fuga1% > 1998
wecho %actor.name% %recita3a%
wdoor 6813 north flags a
wait 10 s
wdoor 6813 north flags bcd
else
wecho %actor.name% %recita2a%
end
end
else
wecho %actor.name% %recita4a%
end
~
#6611
Prigioni 3 - Sendaria (Jerold)~
2 d 0
devo uscire~
* Lista comandi
if %speech% == devo uscire
if %actor.clan% == 6 && %actor.rank% >= 17
wait 2 s
wsend %actor.name% Ad un tuo cenno una guardia ti fa uscire
wait 2 s
wteleport %actor.name% 6812
wforce %actor.name% look
else
end
else
end
~
#6612
Prigioni 4 - Sendaria (Jerold)~
2 d 1
libera~
* Lista comandi
if %actor.clan% == 6 && %actor.rank% >= 17
   unset %libera%
set %speech%
global libera
set animazione on
global animazione
set timer 0
global timer
wecho &2&xUn enorme guardiano entra nella cella &0
wait 1 s
wecho &2&x Un enorme guardiano dice, 'Il prigioniero da liberare mi dica &1&xEccomi&0'
~
#6613
Prigioni 5 - Sendaria (Jerold)~
2 d 0
devo entrare~
* Lista comandi
   if %speech% == devo entrare
if %actor.clan% == 6 && %actor.rank% >= 21
wait 2 s
wsend %actor.name% Ad un tuo cenno una guardia ti fa entrare
wait 2 s
wteleport %actor.name% 6813
wforce %actor.name% look
else
end
else
end
~
#6614
Prigioni 6 - Sendaria (Jerold)~
2 d 1
Eccomi~
* Lista comandi
   if %animazione% == on
if %actor.name% == %libera%
wecho Perfetto ora vi trasferisco fuori
wait 2 s
wsend %actor.name% Il guardiano ti fa uscire dalla cella... in malo modo.
wait 1 s
wteleport %actor.name% 6812
set animazione off
global animazione
unset %libera%
else
wecho non ci provare
end
else
end
~
#6615
Bazaar Faber~
2 d 1
magazzino~
* Lista comandi
if %speech% == magazzino
if %actor.name% == Faber
wait 2 s
wsend %actor.name% In un baleno appare una scala a chiocciola verso il basso.
wait 2 s
wteleport %actor.name% 6822
wforce %actor.name% look
else
end
else
end
~
#6620
Nullapawa~
0 c 100
~
Nothing.
~
#6650
Lamorka prova (jerold)~
2 abg 100
~
* Lista comandi
wteleport all 6846
~
#6651
Porta est Lamorka~
2 ab 100
~
* Lista comandi
wteleport all 6647
~
#6652
porta nord Lamorka~
2 ab 100
~
* Lista comandi
wteleport all 6848
~
#6658
dragone~
0 d 1
guardia avanti stanza follami sfollami stop devo attenti riposo~
* Lista comandi
if %actor.name% /= capoguardie
if %speech% == guardia libera
unset azione
end
if %speech% /= stanza
set %speech%
if %speech% != 0
global stanza
set piazzata on
global piazzata
end
wait 1
nod
wait 1
mgoto %stanza%
wait 5
emote si posiziona di guardia seguendo gli ordini
end
end
if %piazzata% == on
if %attenti% == on
wait 1 s
if %actor.rank% >= 21
set pass ok
global pass
end
if %pass% == ok
if %speech% == riposo
wait 1 s
nod
wait 1 s
say Come lei desidera, Lord %actor.name%
unset azione
end
if %speech% == attenti
wait 1 s
nod
wait 1 s
emote scatta sull'attenti!
set azione on
global azione
end
if %speech% == follami
say Come lei desidera, Lord %Actor.name%
wait 1 s
foll %actor.name%
end
if %speech% == sfollami 
wait 1 s
foll %self%
end
if %speech% == stop
stop
end
if %speech% == devo entrare
wait 4
if %self.room% == 27000
nod
wait 4
mload obj 27000
unlock cancella
open cancella
mforce %actor% nord
close cancella
lock cancella
mat 2 drop all
end
end
if %speech% == devo uscire
wait 4
if %self.room% == 27001
nod
wait 4
mload obj 27000
unlock cancella
open cancella
mforce %actor% sud
close cancella
lock cancella
mat 2 drop all
end
end
end
else
wait 1 s
emote guarda con aria indifferente Lord %Actor.name%
end
unset pass
end
~
#6659
guardiacitta~
0 ab 50
~
* Lista comandi
wait 1 s
say Bene bene! !!SOUND(guardia.wav)
wait 2 s
smile
~
#6660
zoltanrisata2~
0 ab 25
~
* Lista comandi
wait 1 s
say E' tutto sotto controllo. !!SOUND(risata2.wav)
~
#6665
nobilelamorka~
0 ab 30
~
* Lista comandi
wait 1 s
say Salve. !!SOUND(nobile.wav)
~
#6670
SendariaArsenale1 (by Lord Igor)~
0 ab 100
~
* Lista comandi
   eval num %random.11%
   if %num% == %numbis%
     eval num %num% - 1
   end
   eval numbis %num%
   global numbis
   if %sentinella% == dorme
    if %num% == 1
      say  ... gna gnu gna gnaaa ai Suoi ordini Signo..re Roonfff
    elseif %num% == 2
      emote si abbraccia nel sonno.
    elseif %num% == 3
      say Oste ancora del vino ... per favoo..re   SGRUUUN.
    elseif %num% == 4
      emote sembra cadere dal barile.
    elseif %num% == 5
      emote si massaggia il naso.
    elseif %num% == 6
     emote sorride felice nel sonno.
    elseif %num% == 7
      say No no deve dire ...gna gna gnu..Parola d'ordine gna...gnu..gna...Arsenale SGRUNNF
    elseif %num% == 8
      ' Ronf .... Ronf.....
      wait 1 s
      'Fiiiiiiiiiii
   else
      ' Ronf....fiiiiiiii
    end
   end
   if %sentinella% == vigile
    if %num% == 1
      emote si strofina le mani.
    elseif %num% == 2
      emote si gratta l'orecchio sinistro.
    elseif %num% == 3
      emote sbadiglia.
    elseif %num% == 4
      emote si infila un dito nel naso.
    elseif %num% == 5
      emote ti guarda distrattamente.
    elseif %num% == 6
      emote lentamente chiude i suoi occhi.
      set sentinella dorme
      global sentinella
      wait 1
      say Quando avete preso ...SGRUUNNNF ...una decisione avvertitemi ...HiC. 
    elseif %num% == 7
      emote si gratta la pancia.
    elseif %num% == 8
      emote si da uno schiaffo per cercare di restare sveglia.
    elseif %num% == 9
      emote si conta le dita.
    elseif %num% == 10
      emote ciao!!!!
    else
      emote gioca nervosa con un fischietto che porta al collo.
    end
   end
   if %sentinella% == apre
    if %num% == 1
      emote ti guarda le scarpe.
    elseif %num% == 2
      emote emette un lungo e lento fischio.
      wait 1
    elseif %num% == 3
      say In che parte dell'arsenale volete andare?
    elseif %num% == 4
      say Vi porto nell'Arsenale del Cibo o in quello delle Pozioni e Attrezzature Magiche?
    elseif %num% == 5
      say La violazione dell'arsenale e' punita con la morte.
    elseif %num% == 6
      say Ditemi in quale stanza dell'arsenale volete andare!!! Magazzino Generico ?!?
    elseif %num% == 7
      say Lo sapevate che questa stanza e' stata progettata dal famoso Otlas Otla!?
    elseif %num% == 8
      say Dietro quella parete c'e' l'arsenale delle Armi.
    else
     emote ti osserva pensierosa.
    end
end
~
#6671
SendariaArsenale2 (by Lord Igor)~
0 d 100
miiiaaao~
* Lista comandi
if %actor.name% == un gatto
set sentinella dorme
global sentinella
wait 1 s
mecho La sentinella ti mostra le sue tonsille mentre sbadiglia.
wait 1 s
mecho Una Sentinella ridacchia nel sonno.
end
~
#6672
SendariaArsenale3 (by Lord Igor)~
0 d 100
sveglia hey~
* Lista comandi
if %sentinella% == dorme
  set sentinella neutra
  global sentinella
  wait 1 s
  say Chi e'? Che c'e'!!!
  wait 1 s
  mecho Una sentinella lentamente apre i suoi occhi gonfi di sonno.
  wait 2 s
  mechoaround %actor% Una sentinella osserva %actor.name%
  msend %actor% La sentinella ti guarda ... ma non sembra riconoscerti.
  wait 1 s
  say Ditemi la parola d'ordine ... oppure andate via!
  tell owain %actor.name% e' all'arsenale Maesta'!
  bug %actor.name% sveglia la sentinella dell'arsenale di Sendaria.
  set sentinella vigile
  global sentinella
elseif %sentinella% == apre
  set sentinella neutra
  global sentinella
  wait 1 s
  grin
  wait 1 s
  say Ah! Ah! Ahhh!!!
  wait 1 s
  say questo e' un Trigger by Mister Trigger
  wait 2 s
  say Gloria al Fantasma di Sendaria!
  set sentinella apre
  global sentinella
else
  set sentinella neutra
  global sentinella
  wait 1 s
  say Sono gia' sveglio! Allora che volete?
  wait 2 s
  say Mi state prendendo in giro !?!?!
  wait 2 s
  say Andate fuori dai piedi, ho cose piu' importanti da fare !!!
  wait 2 s
  mecho La sentinella si sistema sul grosso barile e lentamente torna a dormire.
  set sentinella dorme
  global sentinella
end
~
#6673
SendariaArsenale4 (by Lord Igor)~
0 d 1
qwerty~
* Lista comandi
if %sentinella% == dorme
  set sentinella neutra
  global sentinella
  wait 1 s
  mecho La sentinella apre lentamente un occhio.
  wait 2 s
  say Scio scio, andate via, brutti fantasmi, lasciatemi riposare in pace.
  set sentinella dorme
  global sentinella
elseif %sentinella% == vigile
  set sentinella neutra
  global sentinella
  wait 1
  mecho La sentinella si gratta pensierosa la testa
  wait 2 s
  say Uhmmm!!!!
  wait 2 s
  say Bene!!! Si si mi sembra che sia proprio questa la parola d'ordine!
  wait 1 s
  mecho La sentinella tira una piccola leva dal pomello rosso.
  set sentinella apre
  global sentinella
else
  wait 1 s
  say Si ho capito ... mica dormo io!!!
end
~
#6674
SendariaArsenale5 (by Lord Igor)~
2 d 1
bene~
* Lista comandi
if %actor.name% == una sentinella && %ascensore% != chiuso
   wait 2 s
   wecho Screeeee Screeeeeeeeck
   wait 1 s
   wecho Una parete mobile chiude l'uscita a sud.
   wdoor %roomsent% s purge
   wdoor %roomsent% w purge
   wdoor %roomsent% e purge
   wdoor %room1% n purge
  wdoor %room2% e room %roomsent%
  wdoor %room2% e description Una parete mobile.
  wdoor %room2% e flag ab
  wdoor %room2% e name parete
  
  wdoor %room3% w room %roomsent%
  wdoor %room3% w description Una parete mobile.
  wdoor %room3% w flag ab
  wdoor %room3% w name parete
  
   set ascensore chiuso
   global ascensore
elseif %actor.name% == una sentinella
   wecho Una sentinella si addormenta e cade dal barile.
   wait 1 s
   wecho La sentinella ti guarda in cagnesco e si rimette a sedere sul barile.
else
end
~
#6675
SendariaArsenale6 ( By Lord Igor )~
2 d 100
squit~
* Lista comandi
if %actor.name% == un topo
  set roomsent 6835
  global roomsent
  set room1 6687
  global room1
  set room2 6836
  global room2
  set room3 6825
  global room3
  set roomtrap 6813
  global roomtrap
  set sbronxo 28075
  global sbronxo
  set gatto 28076
  global gatto
  set ascensore aperto
  global ascensore
  wait 1 s
  wecho Il gatto sornione salta fulmineo sul malcapitato topo e lo ingoia in un boccone.
  wait 1 s
  wecho Il gatto sornione soddisfatto si lecca i baffi.
  wait 1 s
  wforce gatto say Miaoooo!!
  wait 1 s
  wecho &1&xSDRANNNNNNGGGG&0
  wait 1 s
  wecho Qualcosa nel meccanismo segreto che muove questa stanza non funziona.
  wait 1 s
  wecho Si dice che Otlas Otla non abbia curato molto questa sua invenzione.
  wpurge
  wload mob %sbronxo%
  wload mob %gatto%
  wdoor %roomsent% w purge
  wdoor %roomsent% e purge
  wdoor %roomsent% s room %room1%
  wdoor %roomsent% s description parete
  wdoor %roomsent% s flag a
  wdoor %roomsent% s name parete
  wdoor %room1% n room %roomsent%
  wdoor %room1% n description Una parete mobile in grado di nascondere il passaggio.
  wdoor %room1% n flag a
  wdoor %room1% n name parete
  
  wdoor %room2% e room %roomsent%
  wdoor %room2% e description Una parete mobile.
  wdoor %room2% e flag ab
  wdoor %room2% e name parete
  
  wdoor %room3% w room %roomsent%
  wdoor %room3% w description Una parete mobile.
  wdoor %room3% w flag ab
  wdoor %room3% w name parete
  
  wait 1 s
  wforce gatto say Miiiaaao
  wait 1 s
  wecho Una sentinella canticchia nel sonno ' This is the Circle of Life...la la la...la la la! Ronf ...'
else
  halt
end
~
#6676
SendariaArsenale PERMESSI ( by Lord Igor - Nulla )~
2 d 1
pozioni armi cibo generico uscita~
* Lista comandi
if %ascensore% == chiuso
if %actor.clan% == 8 && %actor.rank% >= 50
    wforce sentinella say Tenetevi forte si parte!!!
    wait 1 s
    wforce gatto say miaooo
    if %speech% == armi || %speech% == pozioni
      wait 1 s
      wecho Una sentinella muove una leva dal pomello verde.
      wait 1 s
      wecho Senti un forte rumore di argani.
      wait 1 s
      wecho La parete a ovest si apre
      wdoor %roomsent% w room %room2%
      wdoor %roomsent% w description Una parete mobile.
      wdoor %roomsent% w flag a
      wdoor %roomsent% w name parete
      wdoor %room2% e room %roomsent%
      wdoor %room2% e description Una parete mobile.
      wdoor %room2% e flag a
      wdoor %room2% e name parete
      wforce sentinella say Siamo arrivati sani e salvi ... grazie al cielo!
      wait 1 s
      wforce gatto say Miaoooo
      set ascensore armi
      global ascensore
    end
    if %speech% == cibo || %speech% == generico
      wait 1 s
      wecho Una sentinella muove una leva dal pomello giallo.
      wait 1 s
      wecho Senti un rumore assordante provenire dall'altro lato della parete.
      wait 2 s
      wecho la parete a est si apre
      wdoor %roomsent% e room %room3%
      wdoor %roomsent% e description Una parete mobile.
      wdoor %roomsent% e flag a
      wdoor %roomsent% e name parete
      wdoor %room3% w room %roomsent%
      wdoor %room3% w description Una parete mobile.
      wdoor %room3% w flag a
      wdoor %room3% w name parete
      wforce sentinella say Siamo arrivati sani e salvi ... Bisognerebbe brindare per festeggiare!
      wait 1 s
      wforce gatto say Miaoooo!!!
      set ascensore cibo
      global ascensore
    end
    if %speech% == uscita
      wait 1 s
      wecho Una sentinella muove una leva dal pomello bianco.
      wait 1 s
      wecho Senti un forte rumore di argani.
      wait 2 s
      wecho La parete a sud si apre
      wdoor %roomsent% s room %room1%
      wdoor %roomsent% s description Una parete mobile.
      wdoor %roomsent% s flag a
      wdoor %roomsent% s name parete
      wdoor %room1% n room %roomsent%
      wdoor %room1% n description Una parete mobile.
      wdoor %room1% n flag a
      wdoor %room1% n name parete
      set ascensore aperto
      global ascensore
      wforce sentinella say Eccoci all'uscita....ancora vivi!!!
      wpurge
      wload mob %sbronxo%
      wload mob %gatto%
      wait 1 s
      wforce gatto say Miiiaaao
    end
  else
    if %actor.name% != una sentinella
      wait 1 s
      wforce gatto say Miaoooo!!!
      wait 1 s
      wecho Una sentinella sogghigna malefica.
      wait 1 s
      wecho La sentinella tira una leva dal pomello nero.
      wait 1 s
      wecho Una enorme botola si apre nel pavimento
      wdoor %roomsent% d room %roomtrap%
      wdoor %roomsent% d description Piu' che una botola e' scomparso l'intero pavimento, tranne la parte dove siede la guardia.
      wdoor %roomsent% d flag a
      wdoor %roomsent% d name botola
      wait 1 s
      wforce sentinella say Volevi fare il furbo con me !? Buon viaggio e Buon divertimento %actor.name%!
      wait 1 s
      wsend %actor% La sentinella ti spinge con forza nella botola.
      wechoaround %actor% La sentinella salta fulminea giu' dal barile e spinge %actor.name% nella botola.
      wforce %actor% d
      wdoor %roomsent% d purge  
      set ascensore chiuso
      global ascensore
      wait 1 s
      wecho Una sentinella si risistema sulla botte.
      wait 1 s
      wecho La sentinella prende da una tasca una fiaschetta di pelle e beve felice.
      wait 1 s
      wforce sentinella say Ah com'e' buono il vino di Sendaria!!! Maa..aya meeeette sonno.
    end
  end
elseif %ascensore% != chiuso && %ascensore% != aperto && %actor.name% != una sentinella
  wforce gatto say Miiaooo!!!
  wait 1 s
  wforce sentinella say Come lei desidera!
  wdoor %roomsent% w purge
  wdoor %roomsent% e purge
  wdoor %roomsent% s purge
  wdoor %room1% n purge
  
  wdoor %room2% e room %roomsent%
  wdoor %room2% e description Una parete mobile.
  wdoor %room2% e flag ab
  wdoor %room2% e name parete
  
  wdoor %room3% w room %roomsent%
  wdoor %room3% w description Una parete mobile.
  wdoor %room3% w flag ab
  wdoor %room3% w name parete
  wait 1 s
  wecho &7SBOOONG&0
  wait 1 s
  wecho La parete a sud si chiude di scatto
  wait 1 s
  wforce sentinella say Dove avete detto che volete andare?
  wait 1 s
  wforce gatto say Miaaaooo!!!
  set ascensore chiuso
  global ascensore
else
end
~
#6677
SendariaArsenale7 ( by Lord Igor )~
2 d 1
faticoso decisione sonno~
* Lista comandi
if %actor.name% == una sentinella
  wait 1 s
  wecho La sentinella urta con la testa una leva dal pomello bianco.
  wait 1 s
  wecho Una tenue e luminosa nebbia nasconde il pavimento della stanza.
  wait 1 s
  wecho Screeeeeeck ........ Screeeeck .....
  wdoor %roomsent% w purge
  wdoor %roomsent% e purge
  wdoor %roomsent% s room %room1%
  wdoor %roomsent% s description Una parete mobile.
  wdoor %roomsent% s flag a
  wdoor %roomsent% s name parete
  wdoor %room1% n room %roomsent%
  wdoor %room1% n description Una parete mobile.
  wdoor %room1% n flag a
  wdoor %room1% n name parete
    
  wdoor %room2% e room %roomsent%
  wdoor %room2% e description Una parete mobile.
  wdoor %room2% e flag ab
  wdoor %room2% e name parete
  
  wdoor %room3% w room %roomsent%
  wdoor %room3% w description Una parete mobile.
  wdoor %room3% w flag ab
  wdoor %room3% w name parete
  
  wpurge
  wload mob %sbronxo%
  wload mob %gatto%
  wait 1 s
  wforce gatto say Miiiaaao
  set ascensore aperto
  global ascensore
else
end
~
#6678
SendariaArsenale8 (by Lord Igor)~
0 d 1
miaooo miaoooo miiaooo miaaaooo~
* Lista comandi
if %actor.name% == un gatto
if %sentinella% == apre
set sentinella neutra
global sentinella
elseif %sentinella% == neutra
set sentinella apre
global sentinella
else
end
end
~
#6679
SendariaArsenale ARTIFICIAL ZONE RESET (by Lord Igor)~
0 ab 100
~
* Lista comandi
say Squit
~
#6680
ArsenaleSendaria-Allarme intruso (By Lord Igor)~
2 g 100
5~
* Lista comandi
wait 2 s
wecho Una raffica di vento ti congela il sangue.
if %actor.clan% == 6 && %actor.rank% >= 11
halt
end
wait 2 s
wsend %actor.name% Due mani spettrali escono dal terreno, ti afferrano trascinandoti nel nulla.
wteleport %actor.name% 6813
wait 1 s
wforce %actor.name% look
~
#6681
Pidocchi :)~
0 g 30
~
* Lista comandi
   wait 1 s
   mload mob 1009
   foll %actor.name%
~
#6682
Pidocchi-Effect~
0 ab 100
~
* Lista comandi
mforce all emote si gratta.
~
#6683
SendariaSegrNew~
0 d 1
regno araldo contratto addestramento premio stipendio comandi progetto notifica buono azzera credito~
* Lista comandi
if %regno% == 0
  set regno %actor.clan%
  global regno
end
if %actor.level% > 70
  if %speech% /= regno
    set %speech%
    global regno
  end
end
if %actor.clan% == %regno%
  if %speech% == araldo
    mload mob 12614
    wait 3
    say Proprietario %actor.name%
    wait 2
    say Regno %regno%
  end
  
  if %actor.clan% == 8 && %actor.rank% >= 18
    if %speech% == comandi
      wait 5
      say Eccoli:  comandi, contratto, progetto, notifica, buono, addestramento, premio, stipendio, azzera, credito
    elseif %speech% == progetto
      nod
      wait 5
      mload obj 61
      give progetto %actor.name%
      wait 5
      say ecco a voi un Progetto reale
    elseif %speech% == contratto
      nod
      wait 5
      mload obj 90
      give contratto %actor.name%
      wait 5
      say eccovi un Contratto per vendere una abitazione
    elseif %speech% == addestramento
      wait 5
      mload obj 12581
      give adde %actor.name%
      eval debito %debito% + 25000
      global debito
    elseif %speech% == premio
      wait 5
      mload obj 12580
      eval debito %debito% + 10000
      global debito
    elseif %speech% == stipendio
      mload obj 12582
      eval debito %debito% + 5000
      global debito
    elseif %speech% == notifica
      wait 1s
      mload obj 62
      give notifica %actor.name%
    elseif %speech% == buono
      wait 2
      nod
      wait 5
      mload obj 63
      give buono %actor.name%
      wait 1 s
      say ecco a voi un Buono Reale
      eval buoni %buoni% + 1
      global buoni
    elseif %speech% == credito
      wait 1 s
      if %buoni% == 0
        say non vi sono buoni da riscattare dalle casse del regno, Lord %Actor.name%
      else
        say Sono stati ritirati %buoni% buoni da 10k
      end
      wait 5
      say Mentre il debito da pagare causato dal mantenimento delle truppe e' di %debito%
    end
   
    if %speech% == azzera 
      if %actor.name% == Rodric || %actor.name% == dolmant || %actor.name% == Feanor ||%actor.name% == otak
        nod
        wait 5
        emote strappa una pergamena
        unset buoni
        unset debito
      else
        say non puo' ordinarmi questo, sir %actor.name%
      end
    end
  end
  give all %actor.name%
end
~
#6684
Creazione di Propugnator Gaiae (Nimrhuin)~
0 d 100
propugnator~
* Lista comandi
wait 1
if %actor.name% == Alhandir && %propugnatorgaiae% != 1
 say Ecco il tuo cavallo.
 wait 1
 mload mob 1671
 set propugnatorgaiae 1
 global propugnatorgaiae
end
~
#6697
caffe' (Nulla)~
0 d 100
curami~
if %actor.canbeseen%
if %speech% == Curami
if %actor.gold% >= 40
wait 1 s
Emote annuisce solennemente.
wait 1 s
Emote ti prepara un caffe' e ci spreme dentro due limoni interi.
Emote ti passa una brodaglia, appena mandi giu un sorso...
%force% %actor% vomita
%force% %actor% say jjee mal di test.... BLEEAAAAAAAAAA
%force% %actor% vomita
%force% %actor% Emote ciondola a destra e a sinistra.
%force% %actor% say mi sento meglio
set %actor% Drunk == 0
else
say Non hai abbastanza soldi!!!
end
end
else
wait 1 s
say io non tratto con chi non posso vedere
end
~
#6698
Viviana la patata dolce (Nulla)~
0 d 100
Pasta || Peperoni || Pizza || Melanzane~
if %actor.canbeseen%
if %speech% == Pasta
if %actor.gold% >= 40
wait 1 s
say ecco a voi un bel piatto di pasta fumante!
%force% %actor% dai 40 coin viviana
%load% obj 1858
dai pasta %actor.name%
else
say Non hai abbastanza soldi!!!
end
end
  if %speech% == Pizza
if %actor.gold% >= 30
wait 1 s
say ecco a voi una bella pizza fumante!
%force% %actor% dai 30 coin viviana
%load% obj 1856
dai pizza %actor.name%
else
say Non hai abbastanza soldi!!!
end
end
  if %speech% == Peperoni
if %actor.gold% >= 40
wait 1 s
say ecco a voi i miei migliori peperoni!
%force% %actor% dai 40 coin viviana
%load% obj 1855
dai peperoni %actor.name%
else
say Non hai abbastanza soldi!!!
end
end
  if %speech% == Melanzane
if %actor.gold% >= 50
wait 1 s
say ecco a voi le mie migliori melanzane!
%force% %actor% dai 50 coin viviana
%load% obj 1854
dai melanzane %actor.name%
else
say Non hai abbastanza soldi!!!
end
end
else
wait 1 s
say io non tratto con chi non posso vedere
end
~
#6699
Lingotto (by Nulla)~
0 j 100
~
if %actor.canbeseen%
  if %object.vnum% == 2266
    %echo% Il tesoriere posa delicatamente %object.name% su una bilancia, poi lo ripone dentro una cassaforte.
    wait 1 s
    if %self.has_item(2266)%
      %purge% %object%
      *%at% 1 drop lingotto
      wait 1 s
      say bene... 100 monete a libbra meno il sei per cento di interessi fa... 94 coins.
      %self.gold(94)%
      dai 94 coins %actor.name%
      * drop all
    else
      say Non ci provare, altrimenti chiamo le guardie!!!
    end
  else
    say non mi occupo di questo tipo di merce.
    return 0
  end
else
 Say Non Tratto con chi non posso vedere.
 return 0
end
~
#6700
camilla1~
0 g 100
~
* Lista comandi
wait 1 
if %direction% == south && %actor.level% < 10
wait 1 s
tell %actor.name% Salve %actor.name% ,tutto bene??
~
#6701
vecchio6700~
0 g 30
~
* Lista comandi
wait 3
say La vita e' un gioco...
wait 2 s
say Gioca come meglio credi. . .
wait 2 s
smile
wait 2 s
say Scherzando e ridendo s'e' fatta notte
wait 1 s
wave %actor.name%
~
#6702
vecchio6700~
0 i 50
~
* Lista comandi
wait 2 s
eval num %random.2%
if %num% == 1
emote si allunga faticosamente per accendere le luci della citta'.
else
emote con mestiere smorza le lanterne del viale fino a spegnerle.
end
~
#6703
mob6672~
0 g 100
~
* Lista comandi
wait 1 s
say &x&4 Salve Nobile %actor.name% ,cosa desidera oggi? &0
~
#6704
sentinellacitta~
0 g 100
~
wait 1 s
if %actor.clan% == 8
if %actor.rank% > 25
emote scatta sull'attenti
wait 1
say Signore! 
else
end
end
~
#6710
Banca1~
0 b 30
~
* Lista comandi
wait 5 s
say Deposita qui i tuoi contanti amico!!!
wait 10 s
smile
say Questa e' la banca piu' sicura di tutta la Daresia!!
wait 15 s
say Vai tranquillo con noi.
~
#6719
sentcastello~
0 d 100
guardia~
* Lista comandi
if %actor.vnum% == -1
if %actor.clan%==8 && %actor.rank%>=21
    if %speech% == guardia apri
      if %actor.rank% == 29
        wait 1 s
        say Agli ordini Maesta'!!
        wait 1 s
        bow %actor.name%
      end
      
      if %actor.rank% >= 26 && %actor.rank% < 29
        wait 1 s
        say Agli ordini Vostra Eccellenza.
      end
      
      if %actor.rank% < 27
        wait 1 
        say Agli ordini!
      end
      mload obj 28004
      unlock portone
      open portone
      junk palazzo
      wait 1 s
      say La via e' libera!
    elseif %speech% == guardia chiudi
      say Chiudo subito!!
      wait 1 
      mload obj 28004
      close portone
      lock portone
      junk palazzo
    end
  else
    wait 1 
    say Non vi conosco. Allontanatevi.
  end
end
~
#6720
guardiaspeciale~
0 d 100
guardie~
* Lista comandi
if %actor.clan% != 8
wait 1 s
emote guarda male %actor.name%
halt
end
wait 1 s
if %actor.rank% > 28
if %speech% == guardie andiamo
say Ok Comandante!!
follow %actor.name%
wait 1 s
say Pronto!!
elseif %speech% == guardie alt
emote &6scatta sull'attenti!&0
elseif %speech% == guardie aiuto
say &x&1Banzai!!&0
assist %actor.name%
elseif %speech% == guardie copertura
emote &6fa da scudo col proprio corpo!&0
elseif %speech% == guardie riposo
nod
follow %self%
elseif %speech% == guardie parata
emote &x&3 marcia ordinatamente in quadrato con un passo cadenzato&0
elseif %speech% == guardie sguainate
emote &1sorride minacciosamente e sguaina l'arma di ordinanza&0
elseif %speech% == guardie calma
emote &6rilassa i muscoli e abbassa l'arma&0
elseif %speech% == guardie saluto
emote &4si inchina con fierezza&0
end
else
emote guarda male %actor.name%
end
~
#6743
chiudi porte alcioni sendar (zephy)~
2 g 100
~
wdoor 6707 north purge
wdoor 6707 north description Un pannello di legno ricoperto di terra per mimetizzarsi tra le rocce.
wdoor 6707 north flags abcde
wdoor 6707 north name pannello
wdoor 6707 north room 6713
wdoor 6707 north key 748922
wdoor 6713 south purge
wdoor 6713 south description Un pannello di legno ricoperto di terra per mimetizzarsi tra le rocce.
wdoor 6713 south flags abcde
wdoor 6713 south name pannello
wdoor 6713 south room 6707
wdoor 6713 south key 748922
wdoor 6713 up purge
wdoor 6713 up description Una botola incassata nel soffitto
wdoor 6713 up flags abce
wdoor 6713 up name botola
wdoor 6713 up room 6705
wdoor 6713 up key 65473
wdoor 6705 down purge
wdoor 6705 down description Una botola nascosta sotto un asse del pavimento.
wdoor 6705 down flags abce
wdoor 6705 down name botola
wdoor 6705 down room 6713
wdoor 6705 down key 65473
wdoor 6705 west purge
wdoor 6705 west description Un Pannello di legno rivestito di intonaco, perfettamente mimetizzato con la parete.
wdoor 6705 west flags abce
wdoor 6705 west name pannello
wdoor 6705 west room 6701
wdoor 6705 west key 6701
wdoor 6701 east purge
wdoor 6701 east description Un Pannello di legno rivestito di intonaco, perfettamente mimetizzato con la parete.
wdoor 6701 east flags abce
wdoor 6701 east name pannello
wdoor 6701 east room 6705
wdoor 6701 east key 6701
wdoor 6701 west purge
wdoor 6701 west description Una porta di legno
wdoor 6701 west flags abc
wdoor 6701 west name porta
wdoor 6701 west room 6743
wdoor 6701 west key 657
wdoor 6743 east purge
wdoor 6743 east description Una porta di legno
wdoor 6743 east flags abc
wdoor 6743 east name porta
wdoor 6743 east room 6701
wdoor 6743 east key 657
~
#6770
mob6770~
0 g 100
~
* Lista comandi
wait 1 s
if %direction% == up
emote ti guarda con stupore.
wait 2 s
emote sospira forte e afferra il suo bastone di bambu'
wait 2 s
emote comincia a far roteare il suo bastone sopra la tua testa.
wait 2 s
say Lo scontro e' inevitabile. Attaccami se hai coraggio. (uccidi fantoccio)
else
end
~
#6771
mob6771~
0 g 100
~
* Lista comandi
wait 1 s
if %direction% == west
emote lavora una risorsa di pietra immaginaria.
wait 2 s
emote crea un lingotto di Costanza e lo porge al Fabbro.
else
end
~
#6772
mob6772~
0 e 0
Un fantasma minatore crea un lingotto di Costanza e lo porge al Fabbro.~
* Lista comandi
wait 3 s
emote lavora un lingotto di Costanza
wait 2 s
emote forgia il pugnale della Temperanza.
wait 1 s
grin
wait 1 s
say Bene. Ora possiamo combattere!!!
~
#6773
mob6774~
0 g 100
~
* Lista comandi
wait 1 s
if %direction% == west
emote misura la distanza tra una parete e l'altra.
wait 2 s
emote misura la distanza tra soffitto e pavimento.
wait 2 s
emote misura la distanza fra il suo naso e la punta dei suoi piedi.
wait 2 s
say Cosa desidera Messer %actor.name% ?
else
end
~
#6774
mob6775~
0 g 100
~
* Lista comandi
wait 1 s
say Ah Carne Fresca Finalmente!!!
wait 2 s
emote infila la bistecca di un cittadino Daresiano nel forno.
wait 2 s
emote ti versa alcune gocce d'olio sul corpo e un po' di sale sulla testa.
wait 3 s
say Preferisci le patate o i piselli come contorno?
~
#6775
mob6777~
0 g 100
~
* Lista comandi
wait 1 s
if %direction% == north
say Shhhh! Che mi spaventi i pesci.
wait 2 s
emote ti guarda.
wait 2 s
say Mi sembra di aver visto un bel Tonno!!!
wait 1 s
laugh
else
end
~
#6776
mob6779~
0 g 100
~
* Lista comandi
wait 1 s
emote parla.
wait 2 s
emote sta zitto.
wait 2 s
laugh
~
#6792
NULLA~
0 c 100
~
Nothing.
~
#6800
Bazaar di faber~
2 d 1
magazzino~
* Lista comandi
if %speech% == magazzino
if %actor.name% == Faber || %actor.name% ==Xabbu || %actor.name% ==Nameless || %actor.name% == Gabael
wait 1 s
wteleport %actor.name% 6856
wforce %actor.name% look
else
end
else
end
~
#6801
Bazaar Faber (2)~
2 d 1
scambio~
* Lista comandi
if %speech% == scambio
if %actor.name% == Faber || %actor.name% ==Xabbu || %actor.name% ==Nameless || %actor.name% == Gabael
wait 1 s
wteleport %actor.name% 6849
wforce %actor.name% look
else
end
else
end
~
$~
