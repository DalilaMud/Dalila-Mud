#31500
Guardia cancelli Samurai (Nimrhuin)~
0 g 100
~
wait 1 s
say Se siete malvagi o se siete seguaci del clan Genji,lasciate il celeste impero..o saranno guai!!
~
#31501
Apertura cancelli Samurai (Nimrhuin)~
0 d 100
siamo sono qui in pace~
if %speech% == Siamo qui in pace || %speech% == Sono qui in pace
 wait 1 s
 remo sfera
 wait 1 s
 unlock cancello
 wait 1 s
 open cancello
 wait 2 s
 say Allora siate i benvenuti.
 wait 1 s
 hold sfera
end
~
#31502
Arti marziali (Nimrhuin)~
0 k 100
~
Nothing.
~
#31503
Esplosione 2 (Nimrhuin)~
1 c 100
sblocca anfratto~
wait 1 s
oecho &Y&1&xBOOOOOOOOOOOOOOOOOOOOOOM!!&0
~
#31504
Giu' nelle risaie (Rashan)~
2 g 100
~
wait 3 s
wecho &6Improvvisamente perdi l'equilibrio e inizi a scivolare verso il basso!&0
wait 1 s
wforce all down
~
#31505
Goro il Capoguardie (Rashan)~
0 g 100
~
wait 1 s
if %actor.vnum% == -1
 if %actor.align% > 0
  say Andate via da qui!! Sono al lavoro e non voglio essere disturbato!
 else
  say Dei criminali!!! All'attacco!!!
  wait 1 s
  kill %actor.name%
end
end
~
#31506
Harakiri! (Rashan)~
0 g 100
~
if %actor.vnum% == -1
set flag 1
global flag
 wait 1 s
 emo si punta la sua lama al petto e prende un grosso respiro..
 wait 1 s
 say Ho disobbedito agli ordini del mio Shogun..non posso vivere con questa macchia!
 wait 1 s
 say Pensate ancora che io sia un Samurai degno di vivere?
 wait 1 s
 emo vi guarda in attesa di una risposta
end
~
#31507
Interrogativo di Yasutsuna (Rashan)~
0 d 100
si Si no No~
if %flag% == 1
if %speech% == si || %speech% == Si
set flag 0
global flag
say Un uomo d'onore ha sempre una parola per perdonare il prossimo..voglio farvi un dono
wait 1 s
rem wak
wait 1 s
dai wak %actor.name%
drop wak
wait 1 s Vi dono la mia lama corta..fanne un uso in nome del giusto e nel nome dello Shogun!
elseif %speech% == no || %speech% == No
set flag 0
global flag
emo allontana la spada dal petto non trovando il coraggio di togliersi la vita
wait 1 s
say Sono un samurai reietto...non trovo la forza di sacrificarmi, ma tu non vivrai a lungo per raccontare quello che hai appena visto!
wait 1 s
stand
mkill %actor.name%
end
end
~
#31508
Le Guardie del &6Celeste Impero&0 (Rashan)~
0 i 100
~
wait 1 s
emo si guarda intorno alla ricerca di qualcosa di sospetto
~
#31509
Cassaforte dei Ronin (Rashan)~
2 g 100
~
wait 1 s
wecho &1Non appena metti piede nella stanza,senti il tuo piede che fa scattare un interruttore..&0
wait 1 s
wecho &1Un rumore assordante e poi la stanza viene completamente avvolta nelle FIAMME!!!&0
%damage% %actor% 100
~
#31510
L'anello delle Fiamme Eterne (Rashan)~
1 j 100
~
wait 1 s
%send% %actor.name% &1Senti il tuo corpo riscaldarsi e venire avvolto da fiamme viola!!!&0
%damage% %actor.name% 300
%echoaround% %actor.name% &1Il corpo di %actor.name% viene avvolto dalle fiamme mentre urla dal dolore&0
~
#31511
Yamazaki delle Fiamme (Rashan)~
0 k 50
~
wait 4 s
say Io, Yamazaki delle Fiamme difendero' il patrimonio dei Ronin a costo della mia vita!!
wait 1 s
emo combatte con tutte le sue energie saettando tra le fiamme
~
#31512
Morte di Yamazaki (Rashan)~
0 f 100
~
mecho &1Yamazaki si accascia a terra, e le fiamme che avvolgono il suo corpo si spengono lentamente...&0
~
#31513
L'allenamento dello Yokozuna (Rashan)~
0 b 100
~
emo si allena sferrando potentissimi puntellamenti a un muro di cemento
wait 3 s
emo si ferma per qualche secondo, si concentra e riprende il suo allenamento
~
#31514
L'armatura di Goro-Ho (Rashan)~
1 j 100
~
wait 1 s
%send% %actor.name% &2L'armatura di Goro-Ho si scompone in numerosi parti e in un turbine d'acqua si indossa sul tuo corpo..&0
%echoaround% %actor.name% &2L'armatura di Goro-Ho si scompone in numerosi parti e in un turbine d'acqua si avvolge intorno a %actor.name%&0
wait 1 s
if %actor.align% < 600
%send% %actor.name% &3La reliquia ti giudica indegno per poterla indossare e ti avvolge in una potentissima scarica elettrica!!!&0
%echoaround% %actor.name% &3%actor.name% viene avvolto da una potentissima scarica elettrica e si accascia a terra urlando!!&0
%damage% %actor.name% 3000
else
%send% %actor.name% &2La reliquia emana un luce verde smeraldo e si illumina mentre la indossi!&0
%echoaround% %actor.name% &2%actor.name% risplende di una luce verde smeraldo&0
end
~
#31515
La morte del Drago Verde (Rashan)~
0 f 100
~
mload obj 31564
say Mi avete sconfitto..siete degni di possedere la Sacra reliquia di Goro-Ho...
emo si accascia a terra emettendo un ultimo getto di gas dalle narici
mecho &2Il Drago scompare sciogliendosi completamente,rivelando al suo interno la reliquia di Goro-Ho&0
return 0
end
~
#31516
Il panda coccoloso (Rashan)~
0 g 100
~
wait 1 s
emo si volta verso di te e ti guarda con i suoi occhioni dolci..
wait 2 s
emo torna a sgranocchiare il suo bambu'
~
#31517
Farfalla (Nimrhuin)~
0 k 100
~
wait 1
eval casuale %random.40%
if %casuale% > 20 && %casuale% < 26
 mecho &1&xUna farfalla gigante agita le ali e una densa polvere rosso fuoco si espande ovunque.&0
 wait 1 s
 mecho &1La tua pelle si infiamma al contatto con la polvere!&0
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_affect %actor.name% fireshd 0 1
elseif %casuale% < 11 && %casuale% > 1
 mecho &3&xUna densa polvere gialla riempie l'aria della stanza non appena una farfalla gigante scuote le sue ali!&0
 wait 1 s
 mecho &3La polvere ti blocca il respiro!&0
 dg_cast 'disintegrate' %actor.name%
 dg_affect %actor.name% silence 0 10
elseif %casuale% > 10 && %casuale% < 21
 wait 1 s
 mecho &2&xUna polvere verde scaturita dalle ali di una farfalla gigante ti colpisce agli occhi accecandoti!&0
 dg_cast 'blindness' %actor.name%
 dg_cast 'blindness' %actor.name%
 dg_cast 'blindness' %actor.name%
 dg_cast 'blindness' %actor.name%
 dg_cast 'blindness' %actor.name%
elseif %casuale% == 1
 mecho &7&xUna polvere bianca aleggia ovunque attorno a te, e l'aria si fa improvvisamente piu' pesante...&0
 wait 1 s
 mecho &7Rimani paralizzato!&0
 dg_affect %actor.name% paralized 0 2
elseif %casuale% > 25 && %casuale% < 34
 mecho &5&xUna brillante polvere rosa esce dalle ali di una farfalla gigante e ti colpisce in pieno!&0
 wait 1 s
 mecho &5L'unica cosa che riesci a pensare e' dormire...&0
 dg_affect %actor.name% sleep 0 4
end
wait 2 s
~
#31518
Il combattimento con il Vampiro (Rashan)~
0 k 33
~
wait 1
if %actor% == NULL
  halt
end
eval colpi %random.4%
if %colpi% == 1
 mecho &1%self.name% si getta verso il collo di %actor.name% e lo morde con forza succhiando il suo sangue!&0
 dg_cast 'energy drain' %actor.name%
elseif %colpi% == 2
 mecho &6%self.name% colpisce %actor.name% con forza trasmettendogli una scarica gelida!&0
 dg_cast 'chill touch' %actor.name%
elseif %colpi% == 3
 mecho &0%self.name% sputa della saliva verde negli occhi di %actor.name%!
 dg_cast 'poison' %actor.name%
elseif %colpi% == 4
 mecho &5%self.name% soffia ovunque del gas tossico cercando di soffocarti!&0
 dg_cast 'gas breath'
end
 
~
#31519
L'arrivo nella cella del Vampiro (Rashan)~
0 g 100
~
wait 1 s
mecho &1Appena entri in questa stanza una sensazione di terrore ti assale..&0
wait 1 s
mecho &1Una creatura terribile e sanguinaria e' stata imprigionata qui..&0
wait 1 s
emo esce dall'ombra e ti assale urlando
kill %actor.name%
~
#31520
La morte del Vampiro (Rashan)~
0 f 100
~
emo lancia un urlo raccapricciante mentre si accascia a terra in preda al dolore
mecho &6Il corpo del vampiro si scoglie rivelando le sembianze di un povero vecchio sdentato e rachitico..&0
mload obj 1948
mload obj 1945
return 0
 
~
#31521
La Maschera di Pietra (Rashan)~
1 j 100
~
if !%actor.is_pc%
  halt
end
 
eval check (%actor.align%-1000)
wait 1 s
%send% %actor.name% &7Appena indossi la maschera di pietra una serie di ganci escono dall'artefatto e si agganciano alla tua testa lacerandola&0
%echoaround% %actor.name% &7Appena %actor.name% indossa l'artefatto una serie di ganci si attaccano alla sua testa&0
wait 1 s
%damage% %actor.name% 400
%send% %actor.name% &7Senti una nuova forza e energia scorrere attraverso il tuo corpo,mentre ogni briciolo di umanita' in te sparisce&0
%echoaround% %actor.name% &7%actor.name% si accascia al suolo urlando in preda al dolore e alla disperazione..&0
wait 1 s
if %check% <= 0 || %check% > 500
 wait 1 s
 %send% %actor.name% &7Senti la tua coscienza che ti abbandona,e perdi ogni traccia di te stesso trasformandoti in un vampiro!&0
 %echoaround% %actor.name% &7%actor.name% si contorce dagli spasmi mentre si trasforma in una terribile bestia succhiasangue..&0
 %damage% %actor.name% 8000
 oload mob 31548
else
 dg_affect %actor.name% deathdance 0 10
 %send% %actor.name% &7La tua volonta' ha controllato il potente artefatto e riesci a godere dei suoi spaventosi poteri!&0
 %echoaround% %actor.name% &7%actor.name% si rialza con una demoniaca e spaventosa luce negli occhi..&0
end
 
~
#31522
La Maschera genera Vampiri (Rashan)~
0 n 100
~
say MuahahahaHAhahAhAHhha!!! Succhiero' il vostro sangue bestie inferiori!!
~
#31523
Mago spaccone (Nimrhuin)~
0 k 100
~
wait 1
eval tiro %random.50%
if %tiro% <= 5
 mecho &2&xHikozaemon alza un dito al cielo e una strana luce verde prende forma sopra di esso.&0
 wait 1 s
 dg_cast 'acid arrow' %actor.name%
elseif %tiro% <= 12
 mecho &9&xLa rabbia di Hikozaemon si concentra in mille lame affilate!&0
 wait 1 s
 dg_cast 'blade barrier'
elseif %tiro% <= 16
 mecho &3&xHikozaemon batte tre volte a terra un bastone e il terreno viene ricoperto da una fitta rete di saette!&0
 wait 1 s
 dg_cast 'chain lightning' %actor.name%
elseif %tiro% <= 25
 if %self.hit% < 400
  mecho &9Un'aura oscura ricopre Hikozaemon mentre, pur di non morire, cerca aiuto nei defunti!&0
  wait 1 s
  dg_affect Hikozaemon deathdance 0 3
 else
  &1&xFiamme guizzanti ricoprono il braccio alzato di Hikozaemon per prendere la forma di una freccia!&0
  wait 1 s
  dg_cast 'flame arrow' %actor.name%
 end
elseif %tiro% <= 30
 mecho &6Hikozaemon cerca di sanare le proprie ferite a tuo discapito!&0
 wait 1 s
 dg_cast 'energy drain' %actor.name%
elseif %tiro% <= 32
 if %self.hit% < 200
  mecho &9&xHikozaemon si affida ad un oscuro rituale nella speranza di risollevare le proprie sorti.&0
  wait 1 s
  dg_affect %actor.name% paralized 0 1
 else
  mecho &5Hikozaemon perde fiducia nelle sue capacita' magiche, afferra un corto pugnale e taglia una corda!&0
  wait 1 s
  mecho &1Un pesante masso crolla dal soffitto colpendoti in pieno!&0
  dg_cast 'disintegrate' %actor.name%
  dg_cast 'disintegrate' %actor.name%
  dg_cast 'disintegrate' %actor.name%
  dg_cast 'disintegrate' %actor.name%
 end
elseif %tiro% <= 35 && %summonati% < 3
 if %summonati% < 4
  mecho &3Hikozaemon prende un pugno di terra e la getta innanzi a se' con un gesto soddisfatto e preoccupato.&0
  wait 1 s
  mecho &3&xUn enorme elementale della terra si alza lentamente e ti attacca!&0
  mload mob 1954
  mforce evocato3 kill %actor.name%
  mforce evocato3 kill %actor.name%
  mforce evocato3 kill %actor.name%
  eval summonati (%summonati%+1)
  global summonati
 end
elseif %tiro% <= 40
 if %summonati% < 4
  mecho &1&xUna luce accecante appare in mezzo alla stanza interponendosi tra te e Hikozaemon.&0
  wait 1 s
  mecho &1&xUn guizzante elementale del fuoco prende forma e ti attacca!&0
  mload mob 1953
  mforce evocato2 kill %actor.name%
  mforce evocato2 kill %actor.name%
  mforce evocato2 kill %actor.name%
  eval summonati (%summonati%+1)
  global summonati
 end
else
 if %summonati% < 4
  mecho &7&xUn irreale turbine d'aria scuote l'interno della stanza girando vorticosamente al suo centro.&0
  wait 1 s
  mecho &6&xDistingui i lineamenti di un elementale dell'aria che si muove verso di te attaccandoti!&0
  mload mob 1952
  mforce evocato1 kill %actor.name%
  mforce evocato1 kill %actor.name%
  mforce evocato1 kill %actor.name%
  eval summonati (%summonati%+1)
  global summonati
 end
end
wait 2 s
~
#31524
La danzatrice (Rashan)~
0 g 100
~
wait 1 s
say Oh mio eroe, per solo 1000 coins ti mostrero' i piaceri della carne..
wait 1 s
emo inzia a ballare sinuosamente e sensualmente...
~
#31525
La danzatrice bastarda (Rashan)~
0 m 1000
~
wait 1 s
emo ti guarda e accenna un sorriso malizioso...
say Devi essere molto ricco oltre a essere molto bello..
wait 3 s
emo ti si avvicina...e ti attacca improvvisamente con uno stiletto!
say Dammi il tuo denaro, maledetto maiale!!!
wait 1 s
kill %actor.name%
~
#31526
Contatore di un ora RL (Rashan)~
0 ab 100
~
if %mob%==1
 if %counter% == 277
  set mob 0
  global mob
 else
  eval counter %counter%+1
  global counter
 end
end
~
#31527
Procedura di Autoquest per Nijimura (Rashan)~
0 g 100
~
if %actor.vnum% == -1
 if %mob% != 1
  wait 1 s
  say Salve Avventurieri!!! Benvenuti nella Gilda dei Cacciatori!!
  wait 1 s
  say Ho un lavoro per voi,ho bisogno di un prezioso oggetto nascosto chissa' dove nel Celeste Impero..
  wait 2 s
  eval casuale %random.5%
  if %casuale% == 1
   say Un orribile drago blu massacra i contadini delle risaie,portatemi il suo teschio e vi ricompensero'!!
   eval quest 1
   global quest
  elseif %casuale% == 2
   say Si dice che nel bosco Kaede viva il Dio della Montagna Kimmun Kamui,portatemi la sua pelle e vi ricompensero'!!
   eval quest 2
   global quest
  elseif %casuale% == 3
   say Si dice che Toshiaki,il leader del Clan Genji possiede dei guanti favolosi.Portatemeli e sarete ricompensati!!
   eval quest 3
   global quest
  elseif %casuale% == 4
   say La gilda dei Cacciatori ha catturato una farfalla gigante,portatemi le sue ali e ve le paghero' a peso d'oro!!
   rem chiave
   dai chiave %actor.name%
   drop chiave
   eval quest 4
   global quest
  elseif %casuale% == 5
   say Wang-Cheng,il predone delle Risaie combatte con il suo artiglio magico.Portatemelo e ve lo paghero' a peso d'oro!!!
   eval quest 5
   global quest
  end
  eval counter 0
  global counter
  set mob 1
  global mob
 end
end
~
#31528
Procedura di Autoquest per Nijimura 2 (Rashan feat. IL GRANDE Nimrhuin)~
0 j 100
~
 if %object.vnum% == 31523 && %quest% == 1
    say Grazie Avventurieri per aver recuperato il teschio del drago assassino!
    wait 1 s
    say Per ricompensarvi,preghero' per voi affinche gli Dei vi proteggano!
    dgaffect %actor.name% sanc 0 6
    mpurge teschio
    eval counter 0
    global counter
    set mob 1
    global mob
    set quest 0
    global quest
   elseif %object.vnum% == 1910 && %quest% == 2
    say Grazie avventurieri per aver recuperato la pelle del Dio della montagna!!
    wait 1 s
    say Per ricompensarvi preghero' le potenze del mondo dei morti affinche' vi proteggano!
    dg_affect %actor.name% deathdance 0 6
    mpurge kimmun
    eval counter 0
    global counter
    set mob 1
    global mob
    set quest 0
    global quest
   elseif %object.vnum% == 1946 && %quest% == 4
    say Grazie avventurieri per aver recuperato questo paio di ali di farfalla!!
    wait 1 s
    say Per ricompensarvi utilizzero' su di voi una polverina miracolosa..si dice che sdoppi l'immagine di chi la usa!
    wait 1 s
    emo prende una manciata di polvere da un sacchetto e la lancia su %actor.name%
    dg_affect %actor.name% mirrorimage 0 6
    mpurge farfalla
    eval counter 0
    global counter
    set mob 1
    global mob
    set quest 0
    global quest
   elseif %object.vnum% == 31520 && %quest% == 5
    say Grazie avventurieri per aver rubato l'artiglio del perfido Wang-Cheng!!
    wait 1 s
    say Per ricompensarvi vi copriro' d'oro!
    mload obj 30023
    drop montagna
    mpurge artiglio
    eval counter 0
    global counter
    set mob 1
    global mob
    set quest 0
    global quest
   elseif %object.vnum% == 1937 && %quest% == 3
    say Grazie avventurieri per aver rubato i guanti del malvagio Toshiaki!!
    wait 1 s
    say Per ricompensarvi preghero' il Dio del fuoco affinche' vi protegga!
    dg_affect %actor.name% fireshd 0 6
    mpurge gloves
    eval counter 0
    global counter
    set mob 1
    global mob
    set quest 0
    global quest
   end
~
#31529
Gli arcieri della seconda cerchia di mura (Rashan)~
2 g 100
~
if %actor.vnum% == -1
eval casuale %random.10%
wait 2
if %casuale% == 1
 %send% %actor.name% &5Una freccia partita da una feritoia della seconda cerchia di mura ti colpisce in pieno!&0
 %echoaround% %actor.name% &5Una freccia partita da una feritoia della seconda cerchia di mura colpisce in pieno %actor.name%!&0
 %damage% %actor.name% 150
end
end
~
#31530
Gli arcieri nella terza cerchia di mura (Rashan)~
2 g 100
~
if %actor.vnum% == -1
eval casuale %random.5%
wait 2
if %casuale% == 1
 %send% %actor.name% &1Un tiratore scelto della terza cerchia di mura ti colpisce in pieno con una freccia!&0
 %echoaround% %actor.name% &1Un tiratore scelto della terza cerchia di mura colpisce in pieno %actor.name% con una freccia!&0
 %damage% %actor.name% 170
end
end
~
#31531
Il fischio delle guardie (Rashan)~
2 g 100
~
if %direction% == south
 wait 1 s
 wecho &6Un fischio lacera l'aria mettendo tutte le guardie della zona in allarme, devi essere stato avvistato!&0
~
#31532
L'avvertimento della Guardia (Rashan)~
0 g 100
~
if %direction% == south
 wait 1 s
 emo si piazza al centro della strada
 say ALT! L'accesso qui non e' consentito alle persone non autorizzate!
~
#31533
La guardia fischia mentre combatte (Rashan)~
0 k 20
~
wait 1 s
emo porta un fischietto alla bocca ed emette un suono fortissimo che quasi ti stordisce!
say ALLARME! Degli intrusi stanno forzando la zona!!
~
#31534
La stanza delle evocazioni (Rashan)~
2 g 100
~
if %actor.vnum% == -1
if %check% != 1 & %check% != 2
wait 1 s
wecho &1Un'abbagliante luce rossa permea il centro di questa stanza.&0
wait 1 s
wecho &1&xUn piccolo demone esce dalla luce e avanza a passi veloci verso di te!&0
wload mob 1948
set check 1
global check
elseif %check% == 1
wait 1 s
wecho &3&xLuci e fumo invadono la stanza, provi un terribile dolore agli occhi che ti impedisce di scorgere qualsiasi cosa.&0
wload mob 1949
wait 2 s
wecho &1&xQuando riesci nuovamente a distinguere le forme davanti a te credi di aver perso la vista, un demone grande come la parete ti preclude ogni possibilita' di fuga!&0
set check 2
global check
end
end
~
#31535
Load &1Hikozaemon&0 (Nimrhuin)~
0 n 100
~
eval summonati 0
global summonati
~
#31545
Fischietto di Cabal (Rashan)~
1 j 100
~
if %caricato% != on
if %actor.name% == Cabal
set caricato on
global caricato
wait 1 s
oecho &9&xUn enorme corvo nero scende dal cielo fino a posarsi sulla spalla di Cabal..&0
wait 1 s
oload mob 3599
oforce btljuc foll cabal
else
end
else
end
~
#31546
Beetlejuice (Rashan)~
0 g 100
~
wait 1 s
glare %actor.name%
~
#31547
Beetlejuice 2 (Rashan)~
0 k 100
~
wait 1 s
emo si alza in picchiata e si getta contro il suo avversario!
~
#31548
Morte di Beetlejuice (Rashan)~
0 f 100
~
mecho &9&xIl cadavere di Beetlejuice scompare in una densa nuvola di fumo nero..&0
mpurge cadavere
return 0
end
~
#31554
Fischietto di Cabal (Rashan)~
1 j 100
~
if %tigre% == 1
wait 1 s
osend %actor.name% Solo Cabal puo' controllare Beetlejuice!
halt
end
set tigre 1
global tigre
wait 1 s
osend %actor.name% Non appena porti il fischietto alla bocca, un corvo si avvicina a te e si posa sulla tua spalla..
oechoaround %actor.name% &9Un magnifico e elegante corvo si posa sulla spalla di %actor.name%
wait 1 s
oload mob 3599
oforce btljuc foll %actor.name%
~
$~
