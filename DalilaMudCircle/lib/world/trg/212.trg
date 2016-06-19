#21244
chiudi porte chyrellos (zephy)~
2 g 100
~
* Blocco temporaneo
return 1
halt
wdoor 21355 north purge
wdoor 21355 north description Un pannello di legno ricoperto di terra per mimetizzarsi tra le rocce.
wdoor 21355 north flags abcde
wdoor 21355 north name pannello
wdoor 21355 north room 21354
wdoor 21355 north key 748922
wdoor 21354 south purge
wdoor 21354 south description Un pannello di legno ricoperto di terra per mimetizzarsi tra le rocce.
wdoor 21354 south flags abcde
wdoor 21354 south name pannello
wdoor 21354 south room 21355
wdoor 21354 south key 748922
wdoor 21354 up purge
wdoor 21354 up description Una botola incassata nel soffitto
wdoor 21354 up flags abce
wdoor 21354 up name botola
wdoor 21354 up room 21353
wdoor 21354 up key 65473
wdoor 21353 down purge
wdoor 21353 down description Una botola nascosta sotto un asse del pavimento.
wdoor 21353 down flags abce
wdoor 21353 down name botola
wdoor 21353 down room 21354
wdoor 21353 down key 65473
wdoor 21353 west purge
wdoor 21353 west description Un Pannello di legno rivestito di intonaco, perfettamente mimetizzato con la parete.
wdoor 21353 west flags abce
wdoor 21353 west name pannello
wdoor 21353 west room 21352
wdoor 21353 west key 21352
wdoor 21352 east purge
wdoor 21352 east description Un Pannello di legno rivestito di intonaco, perfettamente mimetizzato con la parete.
wdoor 21352 east flags abce
wdoor 21352 east name pannello
wdoor 21352 east room 21353
wdoor 21352 east key 21352
wdoor 21352 west purge
wdoor 21352 west description Una porta di legno
wdoor 21352 west flags abc
wdoor 21352 west name porta
wdoor 21352 west room 21244
wdoor 21352 west key 657
wdoor 21244 east purge
wdoor 21244 east description Una porta di legno
wdoor 21244 east flags abc
wdoor 21244 east name porta
wdoor 21244 east room 21352
wdoor 21244 east key 657
 
~
#21352
Carica Grimaldelli (XePHy)~
2 g 100
~
* ----------------------------------------------------------------------------
* Tipo Enter ...all'ingresso carica un oggetto chiude le porte e cambia
* serratura. Non scatta se entrano mob.
if !(%actor.is_pc%)
  halt
end
* ----------------------------------------------------------------------------
* Attiva il Context in cui ZEDIT ha memorixato le variabili globali:
* vnum_grimaldello (oggetto che deve essere caricato nella room)
* door_flags (Flag usate per chiudere le porte)
* immune_exit (Se presente questa porta usa exit_flags ...valori default)
* dentini (Se presente usato per cambiare la serratura ad ogni ingresso)
* ...
context 1
* - Debug
if (!(%door_flags%) || !(%vnum_grimaldello%))
  eval hack_debug %room.STO-trigger-NON-funxiona%
  halt
end
* - valori default
eval exit_flags abc
* ----------------------------------------------------------------------------
* Assegna alla var room l'uid della stanxa.
eval room %self%
* ----------------------------------------------------------------------------
* Controlla che il Pg non abbia già il grimaldello di questa stanxa poi
* controlla che nella stanxa non ci sia già un grimaldello.
* In caso contrario carica l'oggetto.
if (!(%actor.has_item(%vnum_grimaldello%)%) && !(%room.contents(%vnum_grimaldello%)%))
  %load% obj %vnum_grimaldello%
end
* ----------------------------------------------------------------------------
* Chiude la porta alle spalle dello scassinatore
* - Arriva con teleport o simile ..nulla da chiudere.
if (%direction% == none)
  halt
end
* - Controlla se c'e' una porta immune alle door_flags, immune è pensato per la 
* - porta d'ingresso all'area.
if ((%immune_exit%) && (%direction% == %immune_exit%))
  eval door_flags %exit_flags%
  eval dentini 3
end
* - Genera un nuovo numero di chiave.
switch %dentini%
  case 3
    eval combinaxione 100 + %random.899%
  break
  case 4
    eval combinaxione 1000 + %random.8999%
  break
  case 5
    eval combinaxione 10000 + %random.89999%
  break
  case 6
    eval combinaxione 100000 + %random.899999%
  break
  case 7
    eval combinaxione 1000000 + %random.8999999%
  break
  case 8
    eval combinaxione 10000000 + %random.89999999%
  break
  case 9
    eval combinaxione 100000000 + %random.899999999%
  break
  default
    %asound% &BSi sente un debole rumore d'ingranaggi&0
    %echo% &BSi sente un debole rumore d'ingranaggi&0
    eval combinaxione 100 + %random.899%
  break
done
* - e ora chiudiamo! Se %door_flags% è uguale ad 'ab' le porte non vengono chiuse 
if %door_flags% != ab
  %asound% *CLICK*
  %asound% &WIl passaggio si richiude rapidamente.&0
  %door% %room.vnum% %direction% flags %door_flags%
  %door% %room.vnum% %direction% key %combinaxione%
  * - Chiude anche dall'altro lato
  switch %direction%
    case east
      eval roomto %room.east(room)%
      eval revdir west
    break
    case west
      eval roomto %room.west(room)%
      eval revdir east
    break
    case north
      eval roomto %room.north(room)%
      eval revdir south
    break
    case south
      eval roomto %room.south(room)%
      eval revdir north
    break
    case up
      eval roomto %room.up(room)%
      eval revdir down
    break
    case down
      eval roomto %room.down(room)%
      eval revdir up
    break
  done  
  * - Genera un nuovo numero di chiave per la serratura nell'altra stanxa.
  eval dentini %roomto.dentini%
  switch %dentini%
    case 3
      eval combinaxione 100 + %random.899%
    break
    case 4
      eval combinaxione 1000 + %random.8999%
    break
    case 5
      eval combinaxione 10000 + %random.89999%
    break
    case 6
      eval combinaxione 100000 + %random.899999%
    break
    case 7
      eval combinaxione 1000000 + %random.8999999%
    break
    case 8
      eval combinaxione 10000000 + %random.89999999%
    break
    case 9
      eval combinaxione 100000000 + %random.899999999%
    break
    default
      %asound% &BSi sente un debole rumore d'ingranaggi&0
      %echo% &BSi sente un debole rumore d'ingranaggi&0
      eval combinaxione 100 + %random.899%
    break
  done
  %door% %roomto.vnum% %revdir% flags %door_flags%
  %door% %roomto.vnum% %revdir% key %combinaxione%
  *-Invia un echo posticipato
  wait 1 s
  %echo% *CLICK*
  %echo% &WIl passaggio si richiude rapidamente.&0
end
* ----------------------------------------------------------------------------
 
~
#21353
carica grimaldello5d (zephy)~
2 g 100
~
if !(actor.has_item(51015))
 halt
else
 wforce %actor.name% esamina rastrelliera
 wload obj 51015
end
~
#21354
carica grimaldello6d~
2 g 100
~
if !(actor.has_item(51016))
 halt
else
 wforce %actor.name% esamina nicchia
 wload obj 51016
end
~
#21355
carica grimald7d + settaaffresco (zephy)~
2 c 100
*~
return 0
halt
context %actor.id%
eval linblu %random.20%
global linblu
eval linred %random.20%
global linred
eval linyel %random.20%
global linyel
eval lingre %random.20%
global lingre
eval conto %linred% + (%linblu% * %linyel%) + %lingre%
global conto
 
~
#21356
Combinaxione Rotante (Phan)~
1 c 100
premi~
*Serratura a combinaxione rotante
context 1
* Il PG usa il comando Premi e le rune nelle 5 caselle ruotano in
* modo asincrono, se compone la sequenxa giusta lo scrigno da un premio.
*- Se  la prima volta o se qualcuno ha vinto assegna ad %a% numeri nuovi.
if %engaged% && %engaged% == true
  halt
end
set engaged true
global engaged
if (!(%a%))
  echostaff Trigger aggira CONTEXT guasto... nulla di grave  un promemoria per Phan!!!
  set c 1
  while %c% < 6
    context %c%
    set a 1
    eval a (%random.7% + 1)
    eval c %c% +1
    global a
  done
end
%echo% &wClick&d
set c 1
while %c% < 6
  context %c%
  eval r%c% %a%
  eval c %c% +1
done
*%echo% VP - %r1% %r2% %r3% %r4% %r5%
if %arg% == rosso || %arg% == 1
  %echoaround% %actor% %actor.name% preme il pulsante &Rrosso&d.
  %echoaround% %actor% Le rune sullo scrigno iniziano a ruotare.
  %send% %actor% Premi il pulsante &Rrosso&d...
  %send% %actor% Alcune rune iniziano a girare...\r\n
  wait 1 s
  %send% %actor% La prima e la seconda runa si spostano in alto.\r\n
  wait 1 s
  %send% %actor% La terza runa si sposta in basso.\r\n
  wait 1 s
  %send% %actor% Le rune si fermano e pulsanti si sbloccano.\r\n
  eval r1 %r1% + 1
  eval r2 %r2% + 1
  eval r3 %r3% - 1
elseif %arg% == verde || %arg% == 2
  %echoaround% %actor% %actor.name% preme il pulsante &Vverde&d.
  %echoaround% %actor% Le rune sullo scrigno iniziano a ruotare.
  %send% %actor% Premi il pulsante &Vverde&d...
  %send% %actor% Alcune rune iniziano a girare...
  wait 1 s
  %send% %actor% La seconda e la terza runa si spostano in alto.
  wait 1 s
  %send% %actor% La quarta runa si sposta in basso.
  wait 1 s
  %send% %actor% Le rune si fermano e pulsanti si sbloccano.
  eval r2 %r2% + 1
  eval r3 %r3% + 1
  eval r4 %r4% - 1
elseif %arg% == lilla || %arg% == 3
  %echoaround% %actor% %actor.name% preme il pulsante &Eviola&d.
  %echoaround% %actor% Le rune sullo scrigno iniziano a ruotare.
  %send% %actor% Premi il pulsante &Elilla&d...
  %send% %actor% Alcune rune iniziano a girare...
  wait 1 s
  %send% %actor% La prima, la terza e la quinta runa si spostano in basso.
  wait 1 s
  %send% %actor% La seconda e la quarta si spostano in alto.
  wait 1 s
  %send% %actor% Le rune si fermano e pulsanti si sbloccano.
  eval r1 %r1% - 1
  eval r2 %r2% + 1
  eval r3 %r3% - 1
  eval r4 %r4% + 1
  eval r5 %r5% - 1
elseif %arg% == giallo || %arg% == 4
  %echoaround% %actor% %actor.name% preme il pulsante &Ggiallo&d.
  %echoaround% %actor% Le rune sullo scrigno iniziano a ruotare.
  %send% %actor% Premi il pulsante &Ggiallo&d...
  %send% %actor% Alcune rune iniziano a girare...
  wait 1 s
  %send% %actor% La quarta runa si sposta in alto.
  wait 1 s
  %send% %actor% La seconda e la terza runa si spostano in basso.
  wait 1 s
  %send% %actor% Le rune si fermano e pulsanti si sbloccano.
  eval r2 %r2% - 1
  eval r3 %r3% - 1
  eval r4 %r4% + 1
elseif %arg% == azzurro || %arg% == 5
  %echoaround% %actor% %actor.name% preme il pulsante &Cazzurro&d.
  %echoaround% %actor% Le rune sullo scrigno iniziano a ruotare.
  %send% %actor% Premi il pulsante &Cazzurro&d...
  %send% %actor% Alcune rune iniziano a girare...
  wait 1 s
  %send% %actor% La terza runa si sposta in basso.
  wait 1 s
  %send% %actor% Le ultime due rune si spostano in alto.
  wait 1 s
  %send% %actor% Le rune si fermano e pulsanti si sbloccano.
  eval r3 %r3% - 1
  eval r4 %r4% + 1
  eval r5 %r5% + 1
*messi per testare i premi
elseif %arg% == xephyroplus
  %send% %actor% Test premio
  eval r1 1
  eval r2 2
  eval r3 3
  eval r4 4
  eval r5 5
elseif %arg% == phanthegana
  %send% %actor% Test premio
  eval r1 3
  eval r2 4
  eval r3 6
  eval r4 7
  eval r5 8
else
  %echoaround% %actor% %actor.name% preme il pulsante &Wbianco&d.
  %echoaround% %actor% Le rune sullo scrigno iniziano a ruotare vorticosamente.
  %send% %actor% Premi il pulsante &Wbianco&d...
  %send% %actor% Tutte le rune iniziano a ruotare velocemente mischiandosi.
  wait 3 s
  %send% %actor% Le rune si fermano.
  set c 1
  while %c% < 6
    eval r%c% (%random.7% + 1)
    eval c %c% +1
  done
end
* Causa guasto del context cancello e riscrivo le variabili globali
unset a
unset a
unset a
unset a
unset a
context 1
eval a %r1%
global a
context 2
eval a %r2%
global a
context 3
eval a %r3%
global a
context 4
eval a %r4%
global a
context 5
eval a %r5%
global a
*- Elabora i valori
set c 1
set e
set fu &d   .:'  &Z:
set fc &d -::    &W:
set fd &d   ':.  &Z:
while %c% < 6
  context %c% 
  
  set xu
  eval xu %a%
  if (%xu% > 8)
    eval xu 1
  elseif (%xu% <= 0)
    eval xu 8
  end
  if %xu% == 1
    set bu T
    set bc &RX
    set bd E
  elseif %xu% == 2
    set bu X
    set bc &VE
    set bd P
  elseif %xu% == 3
    set bu E
    set bc &CP
    set bd H
  elseif %xu% == 4
    set bu P
    set bc &GH
    set bd Y
  elseif %xu% == 5
    set bu H
    set bc &EY
    set bd A
  elseif %xu% == 6
    set bu Y
    set bc &WA
    set bd N
  elseif %xu% == 7
    set bu A
    set bc &BN
    set bd T
  elseif %xu% == 8
    set bu N
    set bc &rT
    set bd X
  else
    set bu T
    set bc &zO
    set bd X
  end
  set e %e%%bc%
  set fu %fu% %bu% &Z:
  set fc %fc% %bc% &Z:
  set fd %fd% %bd% &Z:
  eval r%c% %xu%
  eval c %c% +1
done
*%echo% VF - %r1% %r2% %r3% %r4% %r5%
* Mostra risultato
set fu %fu% &d ':.
set fc %fc% &d   ::-
set fd %fd% &d .:'
%echo% &d
%echo% &d     .::=====================::.
%echo% %fu%
%echo% %fc%
%echo% %fd%
%echo% &d     '::=====================::'
%echo% &d       &wo  &R0   &V0   &E0   &G0   &C0  &wo
set won 0
 
*Elargisce il premio
if %e% == &RX&VE&CP&GH&EY
  set won 1
  eval room %self.room%
  %echo% Lo scrigno si apre con uno scatto, dal suo interno rotola fuori un grimaldello.
  if (%actor.has_item(51018)% || %room.contents(51018)%)
    eval room 51010 + (%random.4% + 3)
    %load% obj %room%
  else
    %load% obj 51018
  end
  %echo% Lo scrigno si richiude.
end
if %e% == &CP&WA&CP&CP&WA
  set won 1
  eval room %self.room%
  %echo% Lo scrigno si apre con uno scatto, dal suo interno rotola fuori una razione di cibo.
  %load% obj 2546
  %echo% Lo scrigno si richiude.
end
if %e% == &CP&GH&WA&BN&rT
  set won 1
  %echo% Le rune emettono un sinistro bagliore di luce.
  %echo% Lo scrigno inizia a ticchettare lentamente.
  set c 1
  while %c% < 20
    wait 2 s
    %echo% &wTIC&d
    wait 1 s
    %echo% &RTAC&d
    eval c %c% +1
  done
  %echo% &WUna LUCE ACCECANTE INNONDA LA STANZA&d
  %echo% &gUn essere mostruoso appare nella stanza&d
  %load% mob 33125
end
* Causa guasto del context cancello e riscrivo le variabili globali
unset a
unset a
unset a
unset a
unset a
if (%won% == 0)
  context 1
  eval a %r1%
  global a
  context 2
  eval a %r2%
  global a
  context 3
  eval a %r3%
  global a
  context 4
  eval a %r4%
  global a
  context 5
  eval a %r5%
  global a
end
unset engaged
context 1
set engaged false
global engaged
context 0
 
~
#21357
conta le aste (Zephy)~
2 c 100
conta~
* ---------------------------------------------------------------------------
* Scrigno nella stanxa con grimaldello a 7/8 dentini. usa il context 0 solo
* per non confondere le globali usate dalla Room che usa il Context 1.
context 0
* ---------------------------------------------------------------------------
* Controlla che siano già stati generati i bastoncini colorati, in caso 
* contrario li genera casualmente.
if (!(%shangai%) || (%shangai% == 0))
  eval linblu %random.20%
  global linblu
  eval linred %random.20%
  global linred
  eval linyel %random.20%
  global linyel
  eval lingre %random.20%
  global lingre
  eval conto %linred% + (%linblu% * %linyel%) + %lingre%
  global conto
  set shangai 1
  global shangai
end
* ----------------------------------------------------------------------------
* Mostra i bastoncini colorati dello shangai, risponde ad ogni singolo colore.
set count 1
if %arg% == blu
  while %count% <= %linblu%
    %send% %actor.name% &6 /&0
    eval count %count% + 1
  done
elseif %arg% == rossi
  while %count% <= %linred%
    %send% %actor.name% &1 /&0
    eval count %count% + 1
  done
elseif  %arg% == verdi
  while %count% <= %lingre%
    %send% %actor.name% &2/&0
    eval count %count% + 1
  done
elseif %arg% == gialli
  while %count% <= %linyel%
    %send% %actor.name% &3 /&0
    eval count %count% + 1
  done
else
  %send% %actor.name% Specifica bene il colore di cosa vuoi contare..
end
 
~
#21358
apri scrigno (zephy)~
1 c 100
componi~
* ---------------------------------------------------------------------------
* Se il Pg indovina il numero di bastoncini totali dello Shangai lo scrigno
* regala un grimaldello.
* ...
* Ricordate a NESSUNO di Attivare varexists su tutto !!!
*if !(%self.varexists(shangai)%)
*  %echo% Shangai NON esiste
*end
eval room %self.room%
if ((%room.shangai%) && (%room.shangai% == 1) && (%arg% == %room.conto%))
* - Controlla se Il Pg ha già il grimaldello o se si trova nella stanxa.
  if (%actor.has_item(51018)% || %room.contents(51018)%)
    %send% %actor.name% &5Una voce dall'oscurita' ti sussurrra, Non fare il furbo con noi, puoi prendere solo un grimaldello per volta!&0
  else
    %echo% Lo scrigno si apre con uno scatto, dal suo interno rotola fuori un raffinato grimaldello.
    %load% obj 51018
    %echo% Lo scrigno si richiude.
  end
else
  if %arg% != %room.conto%
    %send% %actor.name% &5Una voce dall'oscurita' ti sussurrra,  Se non sai neppure contare, forse è meglio che cambi gilda!&0
  else
    %echo% Non accade nulla!
  end
end
* ---------------------------------------------------------------------------
* Cambia comunque il numero di bastoncini e obbliga ad un nuovo conteggio.
eval linbluogg %random.20%
setroom linblu %linbluogg%
eval linredogg %random.20%
setroom linred %linredogg%
eval linyelogg %random.20%
setroom linyel %linyelogg%
eval lingreogg %random.20%
setroom lingre %lingreogg%
eval contoogg %room.linblu% + %room.linred% * %room.linyel% + %room.lingre%
setroom conto %contoogg%
setroom shangai 1
* ---------------------------------------------------------------------------
 
~
#21359
lezione scassinare (zephy)~
0 d 100
grimaldelli scassinare serrature~
mechoaround %actor.name% %self.name% fissa %actor.name% per un attimo, poi gli si rivolge.
msend %actor.name% %self.name% ti si fissa per un attimo, contrariato.
Say Uhmm, cosi' %actor.name% vorresti saperne di piu' sul funzionamento delle serrature e magari dei grimaldelli eh?
wait 3 s
Say Molto bene allievo, apri bene le orecchie:
wait 3 s
Say le serrature meccaniche (quelle magiche meglio lasciarle perdere per ora) non sono altro che un meccanismo composto da un cuneo, o una piccola robusta sbarretta di metallo, che serve a bloccare la porta e 
say da una  serie di, chiamiamoli, dentini disposti in sequenze diverse in ogni serratura.
wait 10 s
Say Questi dentini sono collegati al cuneo e girandoli piu' o meno tutti contemporaneamente lo fanno scattare, aprendo la serratura. Esistono 10 tipi diversi di dentini e dovrai imparare a riconoscerli tutti se sei 
say davvero interessato all'arte dello scasso.
grin
wait 10 s
Say Ovviamente essi sono interni alla serratura e non possono esser visti, stara' alla tua arguzia riconoscerli.
emote estrae una serratura smontata da una delle sue tante sacche e mostra i dentini al suo interno:
mecho &6X E P H Y R A N T O &0
wait 10 s
Say Il modo piu' facile per aprire una serratura e' usare la sua propria chiave, che combacia perfettamente con tutti i suoi dentini interni e li faccia girare contemporaneamente...
wait 5s 
Say Tuttavia ritengo che ci sia un modo piu' elegante, a mio parere.
emote mostra, per un attimo fugace, un &9&Ygrimaldello nero&0.
wait 3 s
say Ne esistono di vari tipi, a seconda del numero di dentini che riescono a far girare contemporaneamente. Migliore e' il tuo strumento, maggiore il numero di dentini che riesce ad incastrare in una sola volta!
wait 5 s
Say Naturalmente non essendo una chiave, stara' alla tua abilita' indovinare la giusta sequenza di dentini e poi 
say manovrare in modo che il grimaldello inganni la serratura, simulando la forma di una chiave.
wait 5 s
say ecco come fare:
say &6Scassina nome_oggetto direzione combinazione&0
wait 3 s
Say Troverai un grimaldello in grado di incastrare 3 dentini di la', insieme all'equipaggiamento per i novizi.
say Dovremmo averne anche degli altri, migliori, ma...
grin
say non ricordo dove siano....
say DATTI DA FARE, Allievo!
~
#21360
Serrature (by Phantom)~
1 c 3
*~
if %cmd% == installa
  * Serve nome e direxione in cui si trova la porta.
  if !%arg%
    %send% %actor% USO: Installa direzione
    halt
  end
  * Puntatore alla stanxa in cui si trova l'oggetto
  eval stanxa %self.room%
    
  * Punta alla direxione in cui installare la serratura e controlla che sia valida.
  eval direx %arg.mudcommand%
  
  switch %direx%
    case north
      set indirk %stanxa.north(key)%
      set indirb %stanxa.north(bits)%
      set rid south
      set moor %stanxa.north(vnum)%
      break
    case east
      set indirk %stanxa.east(key)%
      set indirb %stanxa.east(bits)%
      set rid west
      set moor %stanxa.east(vnum)%
      break
    case south
      set indirk %stanxa.south(key)%
      set indirb %stanxa.south(bits)%
      set rid north
      set moor %stanxa.south(vnum)%
      break
    case west
      set indirk %stanxa.west(key)%
      set indirb %stanxa.west(bits)%
      set rid east
      set moor %stanxa.west(vnum)%
      break   
    case up
      set indirk %stanxa.up(key)%
      set indirb %stanxa.up(bits)%
      set rid down
      set moor %stanxa.up(vnum)%
      break
    case down
      set indirk %stanxa.down(key)%
      set indirb %stanxa.down(bits)%
      set rid up
      set moor %stanxa.down(vnum)%
      break
    default
      %send% %actor% USO: Installa direzione - Direzione deve essere un punto cardinale valido
      halt
    break
  done
  
  if !%indirk%
    %send% %actor% Non ci sono serrature li!
    halt
  end
  
  * Controlla il proprietario
  if !%stanxa.owner%
    %send% %actor% Non sei in una casa.
    halt
  else
    if %stanxa.owner% != %actor.idnum%
      %send% %actor% Puoi cambiare solo la serratura di casa tua.
      * DeBUG %echo% %stanxa.owner% - %actor.idnum% - %actor.id%
      halt
    end
  end 
  
  * Legge il numero di dentini della serratura
  eval dentini %self.val0%
  * - Genera un nuovo numero di chiave.
  switch %dentini%
    case 3
      eval combinaxione 100 + %random.899%
    break
    case 4
      eval combinaxione 1000 + %random.8999%
    break
    case 5
      eval combinaxione 10000 + %random.89999%
    break
    case 6
      eval combinaxione 100000 + %random.899999%
    break
    case 7
      eval combinaxione 1000000 + %random.8999999%
    break
    case 8
      eval combinaxione 10000000 + %random.89999999%
    break
    case 9
      eval combinaxione 100000000 + %random.899999999%
    break
    default
      eval combinaxione 100 + %random.899%
    break
  done
  
  * Possibilità di rottura della serratura (Disattivata)
  eval bonus %actor.prof(fabbro)% + %actor.prof(gioielliere)% + 10
  eval bonus %bonus% / 10
  eval bonus %bonus% + %actor.int% + %actor.dex%
  
  eval caos %random.50%
  if %caos% <= %bonus%
    %door% %stanxa.vnum% %direx% key %combinaxione%
    %door% %moor% %rid% key %combinaxione%
  
    %send% %actor% Smonti dalla porta la vecchia serratura, la posi e monti al suo posto quella nuova.
    %echoaround% %actor% %actor.name% armeggia con la serratura della porta.
    
    * La serratura viene trasformata in quella smontata ormai inutilixxabile.
    otransform -51060
  
  else
    %send% %actor% Hai poca esperienza con queste cose!
    wait 1
    %send% %actor% Non riesci a smontare la vecchia serratura.
    %echoaround% %actor% %actor.name% armeggia nervosamente con la serratura della porta.
  end
else
  return 0
end
~
#21701
Segretario Eleno~
0 d 1
lettera penna spilla scrigno chiave ritira contratto~
if (%actor.clan% == 2)
 if (%actor.rank% > 32)
  if (%speech% == ritira)
   if (%actor.rank% > 54)
    wait 1 s
    give %self.gold% coin %actor.name%
    say Eccoti il mio guadagno quotidiano
   else
   say Solo il sovrano e i Pari possono ritirare
   end
  elseif (%speech% == lettera)
   if (%actor.gold% >= 10)
    wait 1 s
    mload obj 21705
    say Il Regno vi fornisce una lettera con Sigillo Eleno, Fatene buon uso...
    give lettera  %actor.name%
    %actor.gold(-10)%
    %self.gold(10)%
   else
   say Ci vogliono 10 monete per quello...
   end
  elseif (%speech% == penna)
   if (%actor.gold% >= 10)
    wait 1 s
    mload obj 21706
    say Eccovi la vostra penna
    give penna %actor.name%
    %actor.gold(-10)%
    %self.gold(10)%
   else
   say Ci vogliono 10 monete per quello...
   end
  elseif (%speech% == spilla)
   if (%actor.gold% >= 10)
    wait 1 s
    mload obj 21708
    say Eccovi la vostra Spilla Elena, portatela addosso con orgoglio
    give spilla %actor.name%
    %actor.gold(-10)%
    %self.gold(10)%
   else
   say Ci vogliono 10 monete per quello...
   end
  elseif (%speech% == scrigno)
   if (%actor.gold% >= 50)
    wait 1 s
    mload obj 21703
    say Eccovi il vostro magnifico scrigno
    give scrigno %actor.name%
    %actor.gold(-50)%
    %self.gold(50)%
   else
   say Ci vogliono 50 monete per quello...
   end
  elseif (%speech% == chiave)
   if (%actor.gold% >= 30)
    wait 1 s
    mload obj 21704
    say Eccovi la chiave del Regno Eleno... non perdetela!
    give chiave %actor.name%
    %actor.gold(-30)%
    %self.gold(30)%
   else
   say Ci vogliono 30 monete per quello...
   end
  elseif (%speech% == contratto)
   if (%actor.gold% >= 20)
    wait 1 s
    mload obj 90
    say Eccovi un copntratto per la vendita di case
    give contratto %actor.name%
    %actor.gold(-20)%
    %self.gold(20)%
   else
   say Ci vogliono 20 monete per quello...
   end
  end
 else
 say Solo i funzionari Eleni possono usufruire dei miei servigi.
 end
else
say Solo il popolo Eleno puo' usufruire dei miei servigi, Vattene!
end
~
#21702
Indovino~
0 g 100
~
wait 2 s
say Buongiorno %actor.name%,
wait 2 s
say hai %actor.age% anni
wait 2 s
say sei alto %actor.altezza%
wait 2 s
say e sei di classe %actor..class%
wait 2 s
say cosa posso fare per te?
~
#21703
Ambasciata Sendar a chyrellos~
0 q 100
~
if (%actor.clan% != 8)
if (%actor.level% <= 70)
if (%direction% == West)
say Solo gli appartenenti al Regno di Sendaria possono entrare
%echoaround% %actor% %actor.name% cerca di passare ma %self.name% lo blocca.
%send% %actor% Cerchi di passare ma %self.name% ti blocca.
return (0)
end
end
end
~
#21704
Lacrime di Dolmant~
0 c 1
versa_lacrima~
if %actor.name% == archimedix
%echo% Archimedix prende Una boccetta di cristallo contenente una Lacrima di Dolmant
wait 2s
otransform 21702
%echo% Il vero Testamento compare in qualche secondo
end
~
#21705
Entrata primo magazzino di Chyrellos~
2 g 100
Magazzino Regno Primo~
if %actor.clan% == 2
if %actor.rank% > 32
wteleport %actor.name% 21235
wforce %actor.name% look
else
wecho L'accesso e' consentito solo ai funzionari Eleni!
wait 5
wteleport %actor.name% 2134863
wforce %actor.name% look
end
end
~
#21706
lista segretario eleno~
2 c 100
list~
%echo% Qui potete trovare:
%echo% Lettera Elena          10 monete
%echo% Penna Elena            10 monete
%echo% Spilla Elena           10 monete
%echo% Scrigno Eleno          50 monete
%echo% chiave Elena           30 monete
%echo% Contratto vendita case        20 monete
~
#21707
Non puoi scrivere in bacheca~
2 c 1
scrivi~
%echo% Puoi scrivere solo nella bacheca della tua Citta'
~
$~
