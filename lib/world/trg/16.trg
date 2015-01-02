#1600
Aggancio Padrone Manovali (by Phantom)~
0 e 0
Stai seguendo~
* Lista comandi
wait 1 s   
%echo% Sara' un piacere lavorare per voi Messere  %victim.name%
set padrone %victim.name%
global padrone
set manovale %actor.name%
global manovale
set oggetto %object.name%
global oggetto
set bersaglio %target.name%
global bersaglio
set argomento %arg%
global argomento
~
#1601
Test Manovali~
0 d 1
ciao~
* Lista comandi
   
wait 1 s
foll cyrgo
wait 3 s
foll me
smile
~
#1602
Xi Padrone (by Phantom)~
0 e 0
ti ordina di 'seguimi'~
* Lista comandi
wait 1 s
if %victim.name% == %padrone%
  wait 1
  stand
  follow %padrone%
  dg_affect %actor% charm 18 20 
  say Si Padron %victim.name%
else
  say Non lavoro per voi.
end     
~
#1603
Collana Nexus (Nimrhuin)~
1 j 100
~
* Lista comandi
wait 1 s
if %actor.name% != Nexus
 oecho &2&xUn tuono rimbomba attorno a te e un fumo verde ti circonda&0
 wait 1
 oecho &3Una voce poderosa ti investe, "Questa collana non e' destinata a te!"&0
 oforce %actor.name% remo perlenexus
 oforce %actor.name% drop perlenexus
 oload mob 6112
 wait 1
 oforce drago kill %actor.name%
else
 oecho &7&8La collana di perle riluce al contatto con la pelle di Nexus, irradiando la stanza di luci.&0
 dg_affect nexus sanct 0 2
end
~
#1610
Contatore DieTime (Offar)~
0 ab 100
~
* Lista comandi
   
    
    switch %time%
      case 150
        wake
        stand
        say Purtroppo tra mezz'ora dovro' tornare a casa.
        break
      case 4
        wake
        stand
        say Addio amico mio, e' stato un piacere aiutarti!
        remove all
        drop all
        %echo% Un manovale si allontana borbottando sottovoce.
        mgoto 28150
        wait 3 s
        say purgami
        * mpurge %self%
        break
     case 2
        set phan 1
        global phan
        break
     case 0
       if %self.vnum% == 1601
         set time 1000
       elseif %self.vnum% == 1602
         set time 1500
       elseif %self.vnum% == 1603
         set time 2000
       elseif %self.vnum% == 1604
         set time 2500
       else
         set time 500
       end
       global time
       break
     default  
       * Remake by Phantom
       break
   done
    
   if %phan% != 1
     eval time %time% - 1
     global time
   end
~
#1620
capricci~
0 b 90
~
* Lista comandi
eval number %random.20%
~
#1621
prova dg_effect~
0 d 100
vai~
* Lista comandi
dg_affect %self.alias% 'charm' 18 20 
say via
~
#1630
fatica~
0 e 100
prendi~
* Lista comandi
   
halt
   if %fatica% == null
     set fatica 1
     global fatica
   elseif %fatica% == 30
     say ALT! Sono esausto, devo riposarmi.
   * mat 6619 follow gwenad
     siedi
     wait 20 s
   * follow offar
   * set riposo 10
   * global riposo
     stand
     unset fatica
   else
     eval fatica %fatica% + 1
     global fatica
   end
~
#1631
Generic Speech (Helsyn)~
0 de 1
get' 'prendi~
* Lista comandi
eval number %random.10%
switch %number%
case 10
say Sei uno sporco schiavista!!!
say Ti odio! Non mi paghi abbastanza!!!
say Sfruttatore!!!
break
case 9
say Capo, che dici, pausa?
say Inizio ad essere stanco!!!
say Non e' abbastanza per oggi?
break
case 8
say Posso assentarmi un attimo?
wait 1
say Ho un impegno altrove!!!!
break
case 7
say Urka!!!! Mi son scordato il fuoco in casa!!!!
wait 2
say Speriamo di avere ancora una casa!!!!
break
case 6
say Fammi pensare....
wait 4
say Perche' mai ho scelto questo lavoro?
wait 1
say Schiavista!!!!
break
default
wait 2
say Ti prego, basta, lasciami riposare!!!!
wait 2
break
done
~
#1632
Venditore spada (Nimrhuin)~
0 g 100
~
* Lista comandi
wait 2 s
emo da' un colpo di tosse.
wait 1 s
say Ehi, avventuriero, hai tempo di ascoltare un povero vecchio?
wait 1 s
say Ho da venderti un oggetto che potrebbe interessarti...
wait 2 s
say Questa magnifica spada in puro argento, tua per due milioni di pezzi d'oro, un vero affare!
wait 5 s
say Detto tra noi, e' molto piu' di una semplice spada...
wait 1 s
emo da' un altro colpo di tosse.
say Un mago dei tempi passati infuse nell'argento di cui e' fatta quest'arma una potente magia...
wait 2 s
say Solo due armi furono create con quel prezioso materiale, e questa e' una delle due.
wait 1 s
say I poteri sono ancora in parte sconosciuti, e solo quelli noti bastano a renderla gia' micidiale!
wait 3 s
say Se ti interessa dammi i soldi in pagamento unico, e ti consegnero' l'arma.
wait 2 s
emo accenna un sorriso mostrando i denti gialli e deformi.
~
#1633
Venditore spada 2 (Nimrhuin)~
0 m 2000000
~
* Lista comandi
wait 1 s
say Ottima scelta!
wait 1 s
emo si apre in un largo sorriso facendo risaltare i pochi denti.
wait 2 s
emo sfodera la lunga spada che sfavilla nel buio dell'eterna notte Daresiana.
wait 1 s
say Eccola a voi... Un'arma potentissima, mi dole il cuore a lasciarvela...
wait 1 s
say Fatene buon uso, la sua potenza sarebbe distruttiva nelle mani di persone sbagliate!
give spada %actor.name%
drop spada
~
#1634
Spada di cristallo 1 (Nimrhuin)~
1 j 100
~
* Lista comandi
if %actor.vnum% == -1 && %actor.level% <= 70
 wait 1 s
 oechoaround %actor.name% &9&x%self.name% risplende al contatto con il calore della mano di %actor.name%&0
 osend %actor.name% &9&x%self.name% risplende al contatto con il calore della tua mano!&0
 eval casuale %random.46%
 wait 1 s
 if %casuale% == 1
  oechoaround %actor.name% &7&xUn'energia senza pari percorre l'intero corpo di %actor.name%, che inizia a risplendere vivamente!&0
  osend %actor.name% &7&xUn'energia senza pari avvolge il tuo corpo donandoti una sensazione di calore e protezione.&0
  dg_affect %actor.name% sanct 0 3
 elseif %casuale% >= 2 && %casuale% <= 10
  oechoaround %actor.name% &3Non appena %actor.name% stringe l'elsa della spada una lucida barriera lo circonda.&0
  osend %actor.name% &3Uno scudo brillante ti circonda non appena stringi l'impugnatura dell'arma!&0
  dg_affect %actor.name% shield 0 10
  dg_affect %actor.name% aid 0 10
  dg_affect %actor.name% protlightn 0 10
  dg_affect %actor.name% protfire 0 10
  dg_affect %actor.name% antimagic 0 10
 elseif %casuale% >= 11 && %casuale% <= 16
  oechoaround %actor.name% &6&xIl peso di %actor.name% sembra annullarsi improvvisamente, tanto da farlo sollevare da terra nel piu' totale silenzio.&0
  osend %actor.name% &6&xIgnorando il tuo peso inizi a galleggiare nell'aria muovendoti nel piu' totale silenzio.&0
  dg_affect %actor.name% levitate 0 12
  dg_affect %actor.name% sneak 0 12
 elseif %casuale% >= 17 && %casuale% <= 24
  oechoaround %actor.name% &4&xQuando %actor.name% impugna la spada una luce divina si apre un varco nel cielo e lo illumina.
  osend %actor.name% &4&xUna luce divina inonda te e il tuo cammino, benedendoti.&0
  dg_affect %actor.name% bless 0 18
  dg_affect %actor.name% aid 0 18
 elseif %casuale% == 25
  oechoaround %actor.name% &2&xLa spada conferisce d'un tratto un enorme potere ad %actor.name%, e la sua immagine si duplica piu' volte!
  osend %actor.name% &2&xLa spada vibra, nulla sembra accadere.&0
  osend %actor.name% &2&xGuardandoti attorno noti alcune immagini riflesse di te, rimani interdetto...&0
  dg_affect %actor.name% mirrorimage 0 1
 elseif %casuale% == 26
  oechoaround %actor.name% &7I muscoli della mano di %actor.name% si contraggono in una forte stretta, e lentamente si rilassano nuovamente.&0
  osend %actor.name% &7Un formicolio pervade improvvisamente il braccio con cui stringi l'arma, risalmendo velocemente a tutto il corpo... rimani paralizzato!&0
  dg_affect %actor.name% paralized 0 4
 elseif %casuale% >= 27 && %casuale% <= 32
  oechoaround %actor.name% &2Una punta aguzza fuoriesce dalla spada iniettando un temibile veleno nelle vene di %actor.name%!&0
  osend %actor.name% &2Un dolore lancinante pervade la mano con cui stringi l'arma e ti contorci mentre un terribile veleno si diffonde nel tuo corpo!
  dg_affect %actor.name% poison 0 18
 elseif %casuale% >= 33 && %casuale% <= 36
  oechoaround %actor.name% &5&xLa luce al centro della spada si intensifica in un istante avvolgendo %actor.name% attorno al volto!&0
  osend %actor.name% &5&xUna luce senza pari ti avvolge appena tocchi la spada, non riesci a pensare a nient'altro che dormire...&0
  oforce %actor.name% sleep
  dg_affect %actor.name% sleep 0 3
 elseif %casuale% >= 37 && casuale <= 42
  oechoaround %actor.name% &1La spada si muove con una forza propria e ferisce %actor.name% alla mano!&0
  osend %actor.name% &1Ti accorgi della volonta' propria di quest'arma quando si muove di scatto ferendoti alla mano!&0
  %damage% %actor.name% 150
 elseif %casuale% >= 43 && %casuale% <= 45
  oechoaround %actor.name% &1La spada fugge dalla stretta di %actor.name% andandolo a trafiggere ad una gamba!&0
  osend %actor.name% &1Questa maledetta spada non vuole saperne di sottomettersi a te! Sfugge dalla tua presa e trafigge la tua gamba!&0
  oforce %actor.name% remove spada
  %damage% %actor.name% 550
 elseif %casuale% == 46
  oechoaround %actor.name% &9&xUno spirito di pura luce guizza fuori dalla lama e, con un ghigno malvagio, attacca %actor.name%!&0
  osend %actor.name% &9&xUno spirito di pura luce guizza fuori dalla lama della spada e ti attacca con dipinto un ghigno malvagio in volto!&0
  oload mob 1415
  oforce spiritello kill %actor.name%
 end
end
~
#1635
Spada di cristallo 2 (Nimrhuin)~
1 c 1
grandenimrhuin~
* Lista comandi
oforce spini rim
~
#1636
Spada di cristallo 3 (Nimrhuin)~
1 c 6
grandenimrhuin~
* Lista comandi
halt
~
#1637
Spada di cristallo 4 (Nimrhuin)~
1 b 100
~
* Lista comandi
oforce spini grandenimrhuin
~
#1638
Spada di cristallo 5 (Nimrhuin)~
1 c 1
rimuovi coltello~
* Lista comandi
if %cmd% == rimuovi coltello
 oecho &1Ok&0
else
 oecho &5Ok&0
~
#1639
&3N&1&xa&0&3t&1&xa&0&3l&1&xe&0&3!&0 (Nimrhuin)~
1 j 100
~
* Lista comandi
wait 1 s
oecho &7&xSembra nevicare...&0
oecho &7&x           *              *       *                *   &0
oecho &7&x *       *                      *       *           *  &0
oecho &7&x   *         *                    *  *       *       * &0
oecho &7&x     *           *  *         *        *               &0
oecho &7&x  *       *              *       *                 *   &0
oecho &7&x*   *        *      *   *                              &0
oecho &7&x *              *       *         *           *        &0
oecho &7&x      *                      *         * *             &0
oecho &7&x           *           *       *               *       &0
oecho &7&x   *        *          *   *      *      *             &0
oecho &7&x                *     *        *      *                &0
oecho &7&x *        *      *                    *   *        *   &0
oecho &7&x    *       *       *         *  * *      *            &0
oecho &7&x           *   *      *     *       *               *  &0
oecho &7&x  *       *                    *                  *    &0
oecho &7&x        *         *   *          *  *                  &0
oecho &7&x     *                *  *  &2/\&7            * * *        &0
oecho &7&x            *    *       * &2/__\&7        *   *        *  &0
oecho &7&x       &2/\&7 *           * *  &2/  \&7         *  &2/\&7         *&0
oecho &7&x  *   &2/__\&7 *         *   *&2/____\&7          &2/__\&7*        &0
oecho &7&x      &2/  \&7         *      &2/    \&7 *        &2/  \&7         &0
oecho &7&x  *  &2/____\&7    *        *&2/______\&7        &2/____\&7   * *  &0
oecho &7&x*    &2/    \&7        *  *  &2/      \&7 *      &2/    \&7 *      &0
oecho &7&x *  &2/______\&7   *        &2/________\&7  *   &2/______\&7      *&0
oecho &0&2_______&1||&2___________________&1||&2_____________&1||&2__________&0
~
#1650
Cuoco2 (Nimrhuin)~
0 g 100
~
* Lista comandi
wait 1
emo alza gli occhi dal piatto che sta preparando e ti guarda storto.
wait 3
switch %random.6%
case 1
 say Via di qui prima di ritrovarti un coltello tra gli occhi!
break
case 2
 emo abbassa tranquillamente lo sguardo, prende un pugnale e te lo lancia a 10 centimetri dalla testa!
break
case 3
 say Oh qualcuno qua non ci tiene alla vita... VIA DI QUI!!
break
case 4
 say Oh arriva della selvaggina per arricchire i miei piatti! Avvicinati, amico!
break
case 5
 say Allievi, volete vedere come muore un intruso?
break
default
 say Fermo dove sei...
 emo lancia un pugnale in direzione della tua gamba, ferendoti di striscio.
break
done
~
#1651
&9&xCovo delle ombre&0 (Nimrhuin)~
0 d 1
forza e onore~
* Lista comandi
if %speech% == forza e onore
 if %actor.name% == Dolmant
  wait 2 s
  say Salve, Sire
  wait 1 s
  say Benvenuto nel Covo delle Ombre
  wait 1 s
  mteleport Dolmant 1688
 else
  if %actor.name% == Murdock || %actor.name% == Kain
   wait 2 s
   say Salve, Ombra
   wait 1 s
   mteleport %actor.name% 1688
  else
   wait 1 s
   say Una spia!!!
   wait 1 s
   mecho &3Un anziano pescatore urla, 'Allarme!!'&0
   wait 1
   mteleport %actor.name% 1690
 end
end
end
~
#1652
Kimmun Kamui (Nimrhuin)~
0 e 100
svegliato~
* Lista comandi
wait 2 s
emo apre lentamente gli occhi e si alza sulle possenti zampe.
wait 1 s
alz
emo lancia un terribile ruggito che fa tremare le pareti!
wait 1 s
mkill %actor.name%
~
#1653
Nagamune e Nagamori (Nimrhuin)~
0 g 100
~
* Lista comandi
wait 1 s
if %actor.vnum% == -1
 emo scatta in posizione e corre in difesa di Toshiaki!
end
~
#1654
Prigioni Genji (Nimrhuin)~
2 g 100
~
* Lista comandi
if %actor.vnum% == -1 && %direction% == west
 wait 1 s
 wecho &1Un sordo boato scuote le mura della prigione. Tremi al pensiero che qualcosa possa aver emesso un simile verso.&0
end
~
#1655
Prova dg_affect (Nimrhuin)~
0 g 100
~
* Lista comandi
if %actor.vnum% == -1
 wait 1 s
 dg_affect %actor.name% sleep 0 1
end
~
#1656
Falco Denbei (Sheep)~
0 g 100
~
* Lista comandi
wait 2 s
if %actor.vnum% == -1
emo volge lo sguardo su di te fissandoti in maniera strana...
end
~
#1657
Nascita di Denbe (Sheep)~
0 f 100
~
* Lista comandi
mecho &1&xAppena il falco spira, un vortice di luce rossa esce dal suo petto...&0
mecho &1&xLa luce rossa prende improvvisamente la forma di un essere umano!&0
mload mob 1940
mforce denbe say &5&xFinalmente libero! Poveri pazzi! Ora sono libero!!!&0
return 0
end
~
#1658
Carica anello Denbei (Sheep)~
0 n 100
~
* Lista comandi
mload obj 1960
indo anello
end
~
#1659
Morte di Erminia (Sheep)~
0 f 100
~
* Lista comandi
mecho &1&xHAI OSATO UCCIDERE LA SACRA PECORA DI OROME! LA SUA FURIA TI COLPISCE!&0
%teleport% all 1690
return 0
halt
~
#1660
Saluto monaci (Sheep)~
0 g 100
~
* Lista comandi
wait 1 s
if %actor.vnum% == -1
emo sta meditando cosi profondamente che non si accorge nemmeno di te...
end
~
#1661
Araldo dello Shogun (Sheep)~
0 g 100
~
* Lista comandi
if %actor.vnum% == -1
wait 2 s
say IN NOME DELLO SHOGUN  
wait 2 s
say IO DICHIARO I POSSEDIMENTI DEL CONTE BARL
wait 2 s
say COME APPARTENENTI AL &6CELESTE IMPERO&0
wait 2 s
say TORNATE ALLE VOSTRE CASE, NON SIETE DESIDERATI SUL SUOLO DEL &6CELESTE IMPERO&0
end
~
#1662
Carica zanne Youkai (Sheep)~
0 n 100
~
* Lista comandi
mload obj 1971
indo zanne
halt
~
#1663
Nobile Daymio (Sheep)~
0 g 100
~
* Lista comandi
wait 2 s
if %actor.vnum% == -1
emote ti guarda con fare di superiorita', ma come ti permetti di entrare cosi' nel palazzo dello Shogun?
wait 2 s
say Dite, non vorrete essere ricevuto dallo Shogun prima di me?
wait 2 s
emote si volta dall'altra parte con un gesto stizzito
end
halt
~
#1664
Pietra saltafosso (Sheep)~
0 j 100
~
* Lista comandi
if %object.vnum% == 1979
wait 1 s
%echo% &1&xAPPENA INSERITA LA PIETRA NELL'INCAVO, UN PORTALE MAGICO SI CREA VERSO OVEST!&0
%door% 3482 west room 3483
wait 10 s
%echo% &1&xIL PORTALE SI RICHIUDERA' SE NON ENTRATE IN FRETTA!!&0
wait 10 s
%echo% &1&xIL PORTALE STA PER RICHIUDERSI!&0
wait 10 s
%echo% &1&xIL PORTALE SI E' RICHIUSO!&0
%door% 3482 west purge
end
halt
~
#1665
Il saluto del Samurai (Sheep)~
0 g 100
~
* Lista comandi
if %actor.vnum% == -1
wait 1 s
emote alza la punta della katana verso di te.
wait 1 s
say Questo e' l'appartamento privato dello Shogun
wait 1 s
emote assume una posizione di difesa
wait 1 s
say Vattene subito, o morirai.
end
halt
~
#1666
Le guardie dello Shogun (Sheep)~
0 k 100
~
* Lista comandi
wait 3 s
eval guardie %random.3%
if %guardie% == 1
mecho %self.name% &1&xchiama le &0&6Sacre Guardie&1&x in aiuto!&0
mload mob 1964
mforce sgds fol potente
end
~
#1667
Triste morte dell'incavo (Sheep)~
0 f 100
~
* Lista comandi
mecho &1&xUN ONDA DI ENERGIA FUORIESCE DALL'INCAVO, BRUCIANDO TUTTO!&0
mecho &4&xORA CHE IL MECCANISMO E' STATO DISTRUTTO, NON POTRETE USARLO PER TORNARE IN DARESIA!&0
return 0
mpurge
mgoto 28150
halt
~
#1668
Anello di Nalef (Sheep)~
1 j 100
~
* Lista comandi
if %actor.name% == nalef
oechoaround %actor.name% &3&xUn alone di luce circonda Nalef che scompare&0
osend %actor.name% &3&xVieni trasportato altrove dal Sacro Potere Ovino&0
oteleport nalef 20401
else
oechoaround %actor.name% &3&xUn alone di luce circonda %actor.name% che scompare&0
osend %actor.name% &3&xVieni trasportato altrove&0
oteleport %actor.name% 1690
end
~
#1669
Prove del dado (Sheep)~
1 g 100
~
* Lista comandi
ogecho prova
~
#1670
Rovi (Nimrhuin)~
2 g 100
~
* Lista comandi
if %actor.vnum% == -1
 wdamage all 30
 wait 1
 wecho &1La fitta rete di spini ti graffia in ogni punto del corpo!&0
end
~
#1671
&7Cascata&0 (Nimrhuin)~
2 c 100
guarda cascata~
* Lista comandi
wait 1 s
wecho &7Noti una grossa ombra scura dietro la cascata.&0
wait 1 s
wecho &7Bagnandoti completamente infili un braccio sotto l'acqua e trovi un passaggio dietro alla cascata!&0
wdoor 20426 north room 20427
wait 20 s
wecho &7Ti distrai un attimo e quando ti rigiri non riesci piu' a intravedere null'altro che acqua.&0
wdoor 20426 north purge
~
#1672
&7Cascata2&0 (Nimrhuin)~
2 c 100
look cascata~
* Lista comandi
wait 1 s
wecho &7Noti una grossa ombra scura dietro la cascata.&0
wait 1 s
wecho &7Bagnandoti completamente infili un braccio sotto l'acqua e trovi un passaggio dietro alla cascata!&0
wdoor 20426 north room 20427
wait 20 s
wecho &7Ti distrai un attimo e quando ti rigiri non riesci piu' a intravedere null'altro che acqua.&0
wdoor 20426 north purge
~
#1673
Anello di Traslocazione (Sheep)~
1 j 100
~
* Lista comandi
  wisflagged %actor.room% PRIGIONE cntrl
  if %cntrl% == vero
    wait 1 s
    osend %actor.name% &7Pfff.....&0
    halt
  end
  
  wait 1 s
  odamage %actor.name% 100
  osend %actor.name% D'un tratto ti senti strano...
  oechoaround %actor.name% %actor.name% perde pian piano consistenza...
  eval luogo %random.4%
  
  if %luogo% == 1
    oteleport %actor.name% 21208
  elseif %luogo% == 2
    oteleport %actor.name% 6620
  elseif %luogo% == 3
    oteleport %actor.name% 2414859
  elseif %luogo% == 4
    oteleport %actor.name% 12500
  end
  
  oechoaround %actor.name% %actor.name% compare con uno schiocco ed uno schianto!
~
#1674
Dado della Sorte (Sheep)~
1 h 100
~
* Lista comandi
wait 2 s
if %actor.level% >= 71
osend %actor.name% &7Gli immortali non sono soggetti alla sorte!&0
oechoaround %actor.name% &7Il dado si pianta nel terreno e resta immobile&0
oload mob 521
halt
end
if %actor.vnum% != -1
oecho &7Il dado non pare funzionare senza la volontarieta' del lanciatore...&0
oload mob 521
halt
end
eval tirodado %random.100%
wait 1 s
oecho &7Il dado comincia a rotolare come impazzito...&0
wait 1 s
oechoaround %actor.name% &6%actor.name% pare un tantino nervoso mentre osserva il dado girare...&0
osend %actor.name% &6&xSenti il sudore che ti si ghiaccia addosso mentre attendi il verdetto della sorte&0
wait 1 s
oecho &7...pian piano il dado rallenta la sua corsa&0
wait 2 s
osend %actor.name% &7Complimenti, hai realizzato un bel %tirodado%!&0
oechoaround %actor.name% &7%actor.name% ha realizzato un bel %tirodado%!&0
if %tirodado% == 5
wait 1 s
oload obj 3516
osend %actor.name% &3&xSenti il trillare di monetine d'oro!&0
oechoaround %actor.name% &3&xUna manciata di monete d'oro si e' materializzata!&0
elseif %tirodado% == 10
wait 2 s
oload obj 3517
osend %actor.name% &3&xSenti il trillare di molte monetine d'oro!&0
oechoaround %actor.name% &3&xUn sacchetto di monete d'oro si e' materializzato!&0
elseif %tirodado% == 29
wait 2 s
oload obj 3518
osend %actor.name% &3&xSenti il trillare di un mucchio di monete d'oro!&0
oechoaround %actor.name% &3&xUn sacco colmo di monete d'oro si e' materializzato!&0
elseif %tirodado% == 42
wait 2 s
oload obj 3519
osend %actor.name% &3&xSenti la musica che solo molto oro sa produrre!&0
oechoaround %actor.name% &3&xUn pesante forziere colmo d'oro si materializza dal nulla&0
elseif %tirodado% == 56
wait 2 s
osend %actor.name% &3&xUna incredibile pioggia di monete d'oro ti circonda!&0
oechoaround %actor.name% &3&xRimani inebetito mentre osservi monete d'oro piovere dal cielo!&0
wait 1 s
osend %actor.name% &3&0Non ti rimane che raccogliere questa pioggia miracolosa!&0
oload obj 3520
elseif %tirodado% == 74
wait 2 s
osend %actor.name% &3&xLe cateratte del cielo si aprono e vieni sommerso da una valanga d'oro!&0
oechoaround %actor.name% &3&xVieni ricoperto da una quantita' spropositata d'oro!&0
oload obj 3521
elseif %tirodado% == 89
wait 2 s
osend %actor.name% &3&xRimani stupefatto mentre innanzi a te compare una cornucopia vomitante oro!&0
oechoaround %actor.name% &3&xUna magica cornucopia si materializza e comincia a vomitare oro!&0
oload obj 3522
elseif %tirodado% == 2
wait 1 s
osend %actor.name% &1&xUn piccolo mostriciattolo si materializza innanzi a te!&0
oechoaround %actor.name% &1&xUn piccolo portale si apre e ne fuoriesce un mostriciattolo!&0
oload mob 522
elseif %tirodado% == 60
wait 2 s
oecho &1&xUn portale si apre e ne esce un mostriciattolo furioso!&0
oload mob 523
elseif %tirodado% == 65
wait 2 s
oecho &1&xUn portale si apre e ne esce un Gremlin demoniaco!&0
oload mob 524
elseif %tirodado% == 15
wait 2 s
oecho &7Una stupenda spada si materializza dal nulla e brilla di luce propria per qualche istante!&0
oload obj 3524
elseif %tirodado% == 24
wait 2 s
oecho &7Una stupenda ascia si materializza dal nulla e brilla di luce propria per qualche istante!&0
oload obj 3527
elseif %tirodado% == 36
wait 2 s
oecho &7Una stupenda mazza si materializza dal nulla e brilla di luce propria per qualche istante!&0
oload obj 3525
elseif %tirodado% == 51
wait 2 s
oecho &7Una stupenda lancia si materializza dal nulla e brilla di luce propria per qualche istante!&0
oload obj 3528
elseif %tirodado% == 71
wait 2 s
oecho &7Uno stupendo pugnale si materializza dal nulla e brilla di luce propria per qualche istante!&0
oload obj 3526
elseif %tirodado% == 66
wait 2 s
oecho &2&xTre piccole lucertole si materializzano dal nulla.... fiuu poteva andare peggio!&0
wait 1 s
oecho &2&xLe piccole lucertole cominciano a crescere a dismisura!&0
wait 2 s
oecho &1&xLe lucertole si sono tramutate in orrendi mostri giganteschi!&0
wait 1 s
oload mob 18105
oforce tyrano kill %actor.name%
oload mob 18105
oforce tyrano kill %actor.name%
oload mob 18105
oforce tyrano kill %actor.name%
elseif %tirodado% == 7
wait 2 s
osend %actor.name% &4&xTi senti risucchiato via da una strana forza!&0
oechoaround %actor.name% &4&x%actor.name% viene risucchiato da un vortice di energia!&0
wait 1 s
oteleport %actor.name% 28135
elseif %tirodado% == 20
wait 2 s
osend %actor.name% &5&xTi senti risucchiato via da una strana forza!&0
oechoaround %actor.name% &5&x%actor.name% viene risucchiato da un vortice di energia!&0
wait 1 s
oteleport %actor.name% 5597
elseif %tirodado% == 34
wait 2 s
osend %actor.name% &1&xTi senti risucchiato via da una strana forza!&0
oechoaround %actor.name% &1&x%actor.name% viene risucchiato da un vortice di energia!&0
oteleport %actor.name% 18161
elseif %tirodado% == 45
wait 2 s
osend %actor.name% &2&xTi senti risucchiato via da una strana forza!&0
oechoaround %actor.name% &2&x%actor.name% viene risucchiato da un vortice di energia!&0
wait 1 s
oteleport %actor.name% 1805
elseif %tirodado% == 54
wait 2 s
osend %actor.name% &3&xTi senti risucchiato via da una strana forza!&0
oechoaround %actor.name% &3&x%actor.name% viene risucchiato da un vortice di energia!&0
wait 1 s
oteleport %actor.name% 20510
elseif %tirodado% == 78
wait 2 s
osend %actor.name% &6&xTi senti risucchiato via da una strana forza!&0
oechoaround %actor.name% &6&x%actor.name% viene risucchiato da un vortice di energia!&0
wait 1 s
oteleport %actor.name% 9262
elseif %tirodado% == 81
wait 2 s
oechoaround %actor.name% &7&x%actor.name% viene risucchiato da un vortice di energia!&0
osend %actor.name% &7&xTi senti risucchiato via da una strana forza!&0
wait 1 s
oteleport %actor.name% 7113
elseif %tirodado% == 84
wait 2 s
osend %actor.name% &9&xTi senti risucchiato via da una strana forza!&0
oechoaround %actor.name% &9&x%actor.name% viene risucchiato da un vortice di energia!&0
wait 1 s
oteleport %actor.name% 200
elseif %tirodado% == 93
wait 2 s
oechoaround %actor.name% &5&x%actor.name% viene risucchiato da un vortice di energia!&0
osend %actor.name% &5&xTi senti risucchiato via da una strana forza!&0
wait 1 s
oteleport %actor.name% 12058
elseif %tirodado% == 97
wait 2 s
osend %actor.name% &3&xTi senti risucchiato via da una strana forza!&0
oechoaround %actor.name% &3&x%actor.name% viene risucchiato da un vortice di energia!&0
wait 1 s
oteleport %actor.name% 2047
elseif %tirodado% == 18
wait 2 s
oecho &9&xUn incredibile cavallo meccanico si materializza dal nulla!&0
wait 1 s
oload mob 529
elseif %tirodado% == 40
wait 2 s
oecho &6&xUn incredibile cavallo di ghiaccio si materializza dal nulla!&0
wait 1 s
oload mob 525
elseif %tirodado% == 50
wait 2 s
oecho &1&xUn incredibile cavallo di fuoco si materializza dal nulla!&0
wait 1 s
oload mob 526
elseif %tirodado% == 76
wait 2 s
oecho &9&xUn incredibile cavallo di nubi si materializza dal nulla!&0
wait 1 s
oload mob 528
elseif %tirodado% == 91
wait 2 s
oecho &9&xUn incredibile cavallo nero come la notte si materializza dal nulla!&0
wait 1 s
oload mob 530
else
global tirodado
set folle %actor.name%
global folle
oforce %actor.name% kka
halt
end
oload mob 521
halt
~
#1675
Imp purgatore (Sheep)~
0 n 100
~
* Lista comandi
wait 3 s
mpurge dado
mteleport self 28150
say purgami
~
#1676
Gremlin liscio (Sheep)~
0 f 100
~
* Lista comandi
mecho &3&xAlla morte del Gremlin, altri due ne rinascono dal suo corpo!&0
mload mob 522
mload mob 522
return 0
~
#1677
Gremlin infuriato (Sheep)~
0 f 100
~
* Lista comandi
msend %actor.name% &1&xNon ti hanno mai detto che e' una pessima idea uccidere i Gremlin?&0
mechoaround %actor.name% &1&xAlla morte del Gremlin altri due ne nascono dal suo corpo!&0
mload mob 523
mload mob 523
return 0
~
#1678
Gremlin demoniaco (Sheep)~
0 f 100
~
* Lista comandi
mecho &1&xNon appena il mostro muore altri due ne nascono dalle sue carni!&0
mload mob 524
mload mob 524
return 0
~
#1679
Lucky Weapons (Sheep)~
1 j 100
~
* Lista comandi
wait 1 s
osend %actor.name% &3&xTi senti molto fortunato ora!&0
oechoaround %actor.name% &3&x%actor.name% viene circondato da una calda luce dorata&0
wait 1 s
oechoaround %actor.name% &2&xUn quadrifoglio spunta ai piedi di %actor.name%!&0
osend %actor.name% &2&xUn quadrifoglio spunta ai tuoi piedi!&0
oload obj 3529
~
#1680
Man O'War..ghghghgh (Rashan)~
0 gi 100
~
* Lista comandi
wait 1 s
eval casuale %random.4%
if %casuale% == 1
say Man...gia....re...
elseif %casuale% == 2
say Zilv...viaaaaaaaaaaaahhhh....
emo muovi avanti indietro il bacino e muove le mani davanti a esso
elseif %casuale% == 3
say Borgo Dio!!!
elseif %casuale% == 4
say Oh no! Balo! Borgo dio!
end
~
#1681
Cavallo di ghiaccio (Sheep)~
0 b 2
~
* Lista comandi
eval sciogli %random.10%
if %sciogli% == 10
mecho &6&xIl cavallo di ghiaccio si sta sciogliendo!&0
wait 1 s
mteleport self 28150
say purgami
end
~
#1682
Cavallo di fuoco (Sheep)~
0 b 2
~
* Lista comandi
mecho &1&xUna salamandra di fuoco guizza fuori dal corpo del cavallo!&0
mload mob 527
~
#1683
Cavallo di nubi (Sheep)~
0 b 3
~
* Lista comandi
dg_cast 'chain lightning'
~
#1684
Cavallo meccanico (Sheep)~
0 b 1
~
* Lista comandi
mecho &9&xIl cavallo meccanico inizia a vibrare come impazzito mentre si ricarica di energia&0
wait 1 s
dg_cast 'refresh' meccanico
eval vibra %random.5%
if %vibra% == 5
wait 1 s
mecho &9&xLe vibrazioni sono fortissime! Pare un terremoto!&0
dg_cast 'earthquake'
dg_cast 'earthquake'
dg_cast 'earthquake'
end
~
#1685
Cavallo nero (Sheep)~
0 b 5
~
* Lista comandi
dg_cast 'fear'
~
#1686
Unlucky weapons (1) (Sheep)~
1 g 100
~
* Lista comandi
wait 1 s
osend %actor.name% Ma si... chi ci crede alla sfortuna?
~
#1687
Unlucky weapons (2) (Sheep)~
1 b 1
~
* Lista comandi
oecho &2&xL'arma maledetta emette un malsano bagliore verdastro&0
wait 1 s
eval sfiga %random.25%
global sfiga
switch %sfiga%
case 1
oteleport all 1400599
break
case 2
oteleport all 18132
break
case 3
oteleport all 31314
break
case 4
oteleport all 33199
break
case 19
oload mob 536
oload mob 536
oload mob 536
break
case 20
oload mob 1637
oload mob 1637
oload mob 1637
break
default
wait 1 s
oecho Fiuu... per stavolta pare non sia successo nulla di terribile...
break
done
~
#1688
Dado della Sorte (2) Sheep~
1 c 4
kka~
* Lista comandi
if %tirodado% == 31
wait 2 s
oecho &9&xUna strana arma si materializza improvvisamente dal nulla&0
oload obj 3530
wait 1 s
osend %folle% Questa bizzarra spada ti comunica una brutta sensazione....
elseif %tirodado% == 62
wait 2 s
oload obj 3532
oecho &9&xUna strana arma si materializza improvvisamente dal nulla&0
wait 1 s
osend %folle% Questo bizzarro pugnale ti comunica una brutta sensazione....
elseif %tirodado% == 68
wait 2 s
oload obj 3534
oecho &9&xUna strana arma si materializza improvvisamente dal nulla&0
wait 1 s
osend %folle% Questa bizzarra ascia ti comunica una brutta sensazione....
elseif %tirodado% == 87
wait 2 s
oload obj 3533
oecho &9&xUna strana arma si materializza improvvisamente dal nulla&0
wait 1 s
osend %folle% Questa bizzarra lancia ti comunica una brutta sensazione....
elseif %tirodado% == 95
wait 2 s
oecho &9&xUna strana arma si materializza improvvisamente dal nulla&0
oload obj 3531
wait 1 s
osend %folle% Questa bizzarra mazza ti comunica una brutta sensazione....
elseif %tirodado% == 22
wait 2 s
oecho &7&xUn anello magico compare dove un istante fa c'era il dado!&0
oload obj 3514
elseif %tirodado% == 38
wait 2 s
oecho &6&xUn anello magico compare dove un istante fa c'era il dado!&0
oload obj 3535
elseif %tirodado% == 99
wait 2 s
oecho &3&xUn anello magico compare dove un istante fa c'era il dado!&0
oload obj 3536
elseif %tirodado% == 70
wait 2 s
oecho &3&xUn gigantesco Drago Dorato si materializza improvvisamente!&0
oload mob 532
wait 1 s
oforce drago emote ruggisce facendo risuonare l'aria per migliaia di metri intorno a se'...
wait 1 s
osend %folle% Se solo trovassi il modo di ammaestrarlo, questo Drago potrebbe diventare tuo!
elseif %tirodado% == 26
wait 2 s
oecho &6&xUn turbine d'aria si leva improvvisamente dal dado, andando a formare un gigantesco tornado!&0
wait 1 s
oload mob 535
elseif %tirodado% == 47
wait 2 s
oecho &5&xUn simpatico e tenero maialino prende vita!&0
wait 1 s
oload mob 534
elseif %tirodado% == 59
wait 2 s
oecho &7&xDal nulla compare una tovaglia splendidamente ricamata!&0
wait 1 s
oload obj 3538
elseif %tirodado% == 12
wait 2 s
oecho &1&xUn portale prende improvvisamente forma...&0
oload obj 3539
wait 1 s
osend %folle% Forse non e' saggio star qui a controllare cosa uscira' dal portale.... fai finta di nulla e scappa!
elseif %tirodado% == 73
wait 2 s
oecho &1&xUn enorme nube di fumo rosso fuoriesce dal dado&0
wait 2 s
oecho &1&xLa nube rossa comincia ad assumere un aspetto vagamente inquietante...&0
wait 2 s
oecho &1&xLa nube si materializza in un mostro colossale!&0
oload mob 539
wait 1 s
osend %folle% Complimenti avventuriero! Hai liberato Caronos il Grande! Ora si che sono guai!
elseif %tirodado% == 79
wait 2 s
oecho &7Quello che sembra un uomo appare e comincia a camminare...&0
wait 1 s
oload mob 537
elseif %tirodado% == 85
wait 2 s
oecho &5&xD'un tratto comincia una simpatica pioggia di caramelle!&0
oload obj 1440
oload obj 1441
oload obj 1442
oload obj 1443
oload obj 1444
oload obj 1445
oload obj 1446
oload obj 1447
elseif %tirodado% == 4
wait 2 s
oecho &9&xUn amuleto raffigurante un animale compare all'improvviso&0
oload obj 3547
elseif %tirodado% == 16
wait 2 s
oecho &7Un amuleto raffigurante un animale compare all'improvviso&0
oload obj 3548
elseif %tirodado% == 25
wait 2 s
oecho &9&xUn amuleto raffigurante un animale compare all'improvviso&0
oload obj 3545
elseif %tirodado% == 28
wait 2 s
oecho &3&xUn amuleto raffigurante un animale compare all'improvviso&0
oload obj 3544
elseif %tirodado% == 53
wait 2 s
oecho &7&xUn amuleto raffigurante un animale compare all'improvviso&0
oload obj 3546
elseif %tirodado% == 14
wait 2 s
oecho &7Alcune rune magiche compaiono dove un istante fa c'era il dado&0
oload obj 3550
oload obj 3550
oload obj 3550
elseif %tirodado% == 63
wait 2 s
oecho &2Alcune rune magiche compaiono dove un istante fa c'era il dado&0
oload obj 3552
oload obj 3552
oload obj 3552
elseif %tirodado% == 83
wait 2 s
oecho &6Alcune rune magiche compaiono dove un istante fa c'era il dado&0
oload obj 3551
oload obj 3551
oload obj 3551
elseif %tirodado% == 88
wait 2 s
oecho &1Alcune rune magiche compaiono dove un istante fa c'era il dado&0
oload obj 3549
oload obj 3549
oload obj 3549
elseif %tirodado% == 58
wait 2 s
oecho &9&xUna runa magica compare dove un istante fa c'era il dado&0
oload obj 3553
elseif %tirodado% == 13
wait 2 s
oecho &4&xUn enorme nube di fumo blu fuoriesce dal dado&0
wait 2 s
oecho &4&xLa nube blu comincia ad assumere un aspetto vagamente inquietante...&0
wait 2 s
oecho &4&xLa nube si materializza in un mostro colossale!&0
oload mob 552
wait 1 s
osend %folle% Complimenti avventuriero! Hai liberato Sogotacon l'Antico! Ora si che sono guai!
elseif %tirodado% == 17
wait 2 s
oecho &2&xUn enorme nube di fumo verde fuoriesce dal dado&0
wait 2 s
oecho &2&xLa nube verde comincia ad assumere un aspetto vagamente inquietante...&0
wait 2 s
oecho &2&xLa nube si materializza in un mostro colossale!&0
oload mob 554
wait 1 s
osend %folle% Complimenti avventuriero! Hai liberato Nyrugantep il Blasfemo! Ora si che sono guai!
end
~
#1689
Anello dell'Eterno Sonno (Sheep)~
1 g 100
~
* Lista comandi
wait 1 s
osend %actor.name% La tua volonta' non puo' resistere alla tentazione di indossare questo anello!
oforce %actor.name% rimu all.ane
oforce %actor.name% indo sonno
oforce %actor.name% sleep
~
#1690
Killer (by Offar)~
0 d 1
uccidi~
* Lista comandi
if %speech% /= uccidi
set %speech%
global vittima
else
end
~
#1691
Anello di Protezione Magica (Sheep)~
1 j 100
~
* Lista comandi
if %actor.level% >= 71
osend %actor.name% Questo anello non puo' funzionare su di te!
halt
end
wait 1 s
oechoaround %actor.name% &1&x%actor.name% viene circondato da potenti barriere magiche!&0
osend %actor.name% Non appena infili l'anello, senti che potenti magie ti proteggono
dg_affect %actor.name% sanct 0 2
dg_affect %actor.name% blink 0 2
dg_affect %actor.name% mirrorimage 0 2
dg_affect %actor.name% fireshd 0 2
eval sfiga %random.6%
if %sfiga% == 3
wait 1 s
osend %actor.name% Il tuo corpo non riesce a sopportare tutta questa energia magica e si dissolve!
oechoaround %actor.name% &1&x%actor.name% esplode spargendo parti di se ovunque....!&0
wait 1 s
odamage %actor.name% 3000
oload obj 3537
end
halt
~
#1692
Imp purgatore (2) (Sheep)~
0 n 100
~
* Lista comandi
wait 1 s
mpurge 2.cadavere
mteleport self 28150
say purgami
~
#1693
Maialini magici (Sheep)~
0 g 100
~
* Lista comandi
%echo% Kill this trigger
~
#1694
Maialini magici (2) (Sheep)~
0 n 5
~
* Lista comandi
%echo% Kill this trigger
~
#1695
Tovaglia eterna libagione (Sheep)~
1 h 100
~
* Lista comandi
wait 1 s
oecho &7&xNon appena posi la magica tovaglia a terra, essa si imbandisce con le piu' squisite libagioni tu potessi sognare!&0
wait 2 s
eval tova %random.3%
if %tova% == 1
oload obj 1427
wait 1 s
oload obj 1682
wait 1 s
oload obj 1683
wait 1 s
oload obj 1684
wait 1 s
oload obj 3009
wait 1 s
oload obj 3028
wait 1 s
oload obj 6613
wait 1 s
oload obj 6676
elseif %tova% == 2
oload obj 1685
wait 1 s
oload obj 1686
wait 1 s
oload obj 1691
wait 1 s
oload obj 1855
wait 1 s
oload obj 1921
wait 1 s
oload obj 3013
wait 1 s
oload obj 3310
wait 1 s
oload obj 6675
elseif %tova% == 3
oload obj 1426
wait 1 s
oload obj 1854
wait 1 s
oload obj 2506
wait 1 s
oload obj 3008
wait 1 s
oload obj 3024
wait 1 s
oload obj 16005
wait 1 s
oload obj 16018
wait 1 s
oload obj 8120
end
~
#1696
Tornado magico (Sheep)~
0 g 100
~
* Lista comandi
wait 1 s
mechoaround %actor.name% &6&x%actor.name% viene risucchiato dal vento magico!&0
msend %actor.name% &6&xVieni risucchiato dal vortice!&0
dg_cast 'teleport' %actor.name%
~
#1697
Tornado magico (2) Sheep~
0 k 10
~
* Lista comandi
msend %actor.name% &6&xVieni risucchiato dal vortice!&0
mechoaround %actor.name% &6&x%actor.name% viene risucchiato dal vento magico!&0
dg_cast 'teleport' %actor.name%
~
#1698
Tornado magico (3) Sheep~
0 i 100
~
* Lista comandi
wait 1 s
dg_cast 'cone of cold'
dg_cast 'chain lightning'
~
#1699
Portale Inferi (Sheep)~
1 ab 100
~
* Lista comandi
eval demone %random.5%
if %demone% == 1
oecho &1&xUn terrificante Demone esce dal portale!&0
wait 1 s
oload mob 536
end
~
$~
