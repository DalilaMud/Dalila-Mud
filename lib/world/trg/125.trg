#12500
Capoguardie1~
0 d 1
eccomi~
   
     if %actor.vnum% == %vnum_guardie%
       if %speech% == eccomi
         if %fatto% != on
           set azione 0
           global azione
           set somma 0
           global somma
           set guardia 0
           global guardia
           if !(%ufficio%)
             bug Non mi e' stato assegnato un ufficio.
             wait 1 s
             mgoto 28150
             halt
           end
           if !(%vnum_guardie%)
             bug Non mi sono state assegnate guardie.
             halt
           end       
           if !(%caserma%)
             bug Non e' stata settata la caserma dove riunire la ronda.
             halt
           end
           if !(%grado%)
             set grado 21
             global grado
             halt
           end         
           wait 1
           mgoto %ufficio%
           wait 1
           set giro 1
           global giro
           emo controlla la disposizione dei posti di guardia.   
           if !(%1%)
             set 1 %caserma%
           end 
           set stanza %1%
           global stanza
           if !(%1a%)
             set 1a 1
           end
           set quanti %1a%
           global quanti
           wait 1 s
           say Guardie!!!
           wait 1  
           set fatto on
           global fatto
         end     
         eval somma %somma% + 1
         global somma
         if %somma% < %numero_guardie%
           if %libere% != on
             wait 2
             eval guardia %guardia% + 1
             global guardia
             if %guardia% <= %quanti%
               say settore %regno_id%
               wait 1 s
               say grado %grado%
               wait 1 s
               say cella %cella%
               wait 1 s
               say postazione %stanza%
               wait 1 s
               mload mob %vnum_guardie%
             else
               eval giro %giro% + 1
               global giro
               set guardia 0
               set stanza null
               set quanti 0
               if %giro% == 1
                 set stanza %1%
                 set quanti %1a%
               elseif %giro% == 2
                 set stanza %2%
                 set quanti %2a%
               elseif %giro% == 3
                 set stanza %3%
                 set quanti %3a%
               elseif %giro% == 4
                 set stanza %4%
                 set quanti %4a%
               elseif %giro% == 5
                 set stanza %5%
                 set quanti %5a%
               elseif %giro% == 6
                 set stanza %6%
                 set quanti %6a%
               elseif %giro% == 7
                 set stanza %7%
                 set quanti %7a%
               elseif %giro% == 8
                 set stanza %8%
                 set quanti %8a%
               elseif %giro% == 9
                 set stanza %9%
                 set quanti %9a%
               end
               global stanza
               global quanti
               wait 4
               if %stanza% == null
                 set stanza %caserma%
                 global stanza
                 wait 2
                 say Oggi sei di ronda
                 wait 2
                 say settore %regno_id%
                 wait 1 s
                 say grado %grado%
                 wait 1 s
                 say cella %cella%
                 wait 1 s
                 say postazione %caserma%
               else
                 if %stanza% != 0
                   say settore %regno_id%
                   wait 1 s
                   say grado %grado%
                   wait 1 s
                   say cella %cella%
                   wait 1 s
                   say postazione %stanza%
                 else
                   set libere on
                   global libere
                   say Le guardie da piazzare di guardia sono finite, le altre andranno ora in giro per la citta'
                   wait 3 s
                   say Oggi sei di ronda
                   wait 1 s
                   say settore %regno_id%
                   wait 1 s
                   say grado %grado%
                   wait 1 s
                   say cella %cella%
                   wait 1 s
                   say postazione %caserma%  
                 end
               end
               wait 1 s
               mload mob %vnum_guardie%
             end
           else
             wait 1
             say Oggi sei di ronda
             wait 2
             say settore %regno_id%
             wait 1 s
             say grado %grado%
             wait 1 s
             say cella %cella%
             wait 1 s
             say postazione %caserma%
             wait 1 s 
             mload mob %vnum_guardie%
           end          
         else
           set giro 1
           global giro
           unset guardia
           set fatto off
           global fatto
           set libere off
           global libere
           wait 2
           say Tutte le sentinelle sono al loro posto di guardia.
           wait 3
           mat %caserma% say riposo
         end
       end
     end
~
#12501
Trigger da eliminare~
0 n 100
~
   halt
   
~
#12502
araldo1~
0 d 1
1 2 3 manifesto testo cambia seguimi piazzati proprietario regno~
if %actor.name% /= segretario
set %speech%
if %speech% /= proprietario
global proprietario
foll %proprietario%
end
if %speech% /= regno
global regno
end
end
if %actor.clan% == %regno%
if %actor.rank% > 10
if %speech% == seguimi
wait 2
stand
wait 4
foll %actor.name%
set lavoro off
global lavoro
elseif %speech% == piazzati
wait 2
sit
wait 4
foll araldo
set lavoro on
global lavoro
end
if %azione% == 0
if %speech% == cambia manifesto
wait 4
say ora ditemi cosa scrivere precedendo la frase con la parola &11,2 o 3&0 a seconda della riga che si vuole aggiornare
set azione cambio2
global azione
end
if %speech% == cambia testo
wait 4
say Bene, ora ditemi cosa dire, precedendo la frase con la parola &1testo&0
set azione cambio1
global azione
end
elseif %azione% == cambio1
if %speech% /= testo
set %speech%
global testo
wait 6
emote si ripete mentalmente la frase e infine sorride
set azione 0
global azione
end
elseif %azione% == cambio2
if %speech% /= 1
set %speech%
global 1
wait 4
emote scrive rapidamete il testo su un cartellone per poi metterselo al collo
set azione 0
global azione
end
if %speech% /= 2
set %speech%
global 2
wait 4
emote scrive rapidamete il testo su un cartellone per poi metterselo al collo
set azione 0
global azione
end
if %speech% /= 3
set %speech%
global 3
wait 4
emote scrive rapidamete il testo su un cartellone per poi metterselo al collo
set azione 0
global azione
end
end
end
end
~
#12503
Araldo2~
0 b 50
~
eval timer %timer% + 1
global timer
if %timer% >= 30000
  mgoto 28150
  wait 3 s
  say purgami
end
if %lavoro% == on && %testo% != 0
  eval random %random.2%
  if %random% == 1
    say %testo%
  end
end
~
#12504
araldo3~
0 e 0
ti guarda~
wait 2 s
%echo% --------------------------------------
mecho %1%
mecho %2%
mecho %3%
%echo% --------------------------------------
~
#12505
MOB1~
0 ab 100
~
if %casa% == 0
set casa %self.room%
global casa
end
eval timer %timer% + 1
global timer
if %timer% > 175
emote torna a casa
mgoto 2
mpurge
unset timer
end
eval random %random.10%
if %random% < 5
eval back %back% + 1
global back
if %random% == 1
n
elseif %random% == 2
s
elseif %random% == 3
w
elseif %random% == 4
e
end
end
if %back% > 7
mgoto %casa%
end
~
#12506
Soldati1~
0 ab 100
~
if %casa% == 0
set casa %self.room%
global casa
set piazzata on
global piazzata
set attenti on
global attenti
end
eval timer %timer% + 1
global timer
if %timer% > 175
emote torna a casa
mgoto 2
mpurge
unset timer
end
if %azione% != on
eval random %random.14%
if %random% < 5
eval back %back% + 1
global back
if %random% == 1
n
elseif %random% == 2
s
elseif %random% == 3
w
elseif %random% == 4
e
end
end
if %back% > 4
mgoto %casa%
unset back
end
end
kill nemico
~
#12507
Soldati2~
0 g 100
~
wait 5
kill nemico
~
#12508
Soldati3~
0 i 100
~
wait 5
kill nemico
~
#12509
Truppe assedio 1~
0 d 1
assalta~
if %actor.level% > 91
set %speech%
global assalta
end
~
#12510
Truppe assedio 2~
0 ab 100
~
if %assalta% != 0 && %piazzato% != on
%assalta% 5
set piazzato on
global piazzato
set casa %self.room%
global casa
end
if %piazzato% == on
eval timer %timer% + 1
global timer
eval vita %vita% + 1
global vita
if %vita% > 200
unset vita
mgoto 2
mpurge
sit
end
end
if %random% < 12
eval back %back% + 1
global back
if %random% == 1
n
elseif %random% == 2
s
elseif %random% == 3
w
elseif %random% == 4
e
end
end
if %back% > 6
mgoto %casa%
unset back
end
~
#12523
deirane1~
0 d 1
guardia avanti stanza follami sfollami stop devo attenti riposo~
if %actor.name% == Makud || %actor.name% == Lisandro
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
if %actor.name% == Rodric || %actor.name% == Khrom || %actor.name% == Mazzaferro || %actor.name% == alorie
set pass ok
global pass
end
if %actor.name% ==  khardan || %actor.name% == magicorty || %actor.name% == sirgyles || %actor.name% == rox
set pass ok
global pass
end
if %actor.rank% > 6
set pass ok
global pass
end
if %actor.name% ==  masterhous || %actor.name% == verena
set pass ok
global pass
end
if %actor.rank% > 5
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
mat 3 drop all
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
mat 3 drop all
end
end
end
else
wait 1 s
emote guarda con aria indifferente Lord %Actor.name%
end
end
unset pass
~
#12525
guardiano reale~
0 d 100
devo~
say My trigger commandlist is not complete!
if %actor.clan% == 5 && %actor.rank% > 15
end
~
#12526
segretario~
0 d 1
regno araldo ferramenta contratto addestramento premio stipendio comandi progetto buono azzera credito notifica~
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
if %speech% == ferramenta
mload mob 12606
wait 3
say funzionario %actor.name%
wait 6
say segui %actor.name%
end
if %speech% == comandi
wait 5
say Eccoli: &1 comandi, contratto, progetto, notifica, buono, ferramenta, addestramento, premio, stipendio, azzera, credito &0
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
give progetto %actor.name%
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
nod
wait 5
mload obj 62
give noti %actor.name%
wait 1 s
say ecco a voi una Notifica di costruzione Reale
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
~
#12527
Deirane2 ed esercito~
0 e 0
ti dice, 'comandi'~
wait 1 s
emote sta aspettando ordini da %actor.name%, che lo guarda con aria decisa.
set attenti on
global attenti
~
#12528
deirane3 ed esercito~
0 e 0
ti dice, 'finito'~
wait 1 s
nod
wait 1 s
emote si rilassa visibilmente mentre viene congedato da %actor.name%
unset attenti
~
#12529
deirane4~
0 ab 100
~
  
if %self.room% > 1000000 && %self.room% < 2000000
  if %azione% != on
    emote decide di tornare in citta'
    if !%postazione% || !%cella%
      mgoto 28150
      say purgami
    else
      mgoto %postazione%
    end
  end
end
   
if %settata% != on
  set settata on
  global settata
  set attenti on
  global attenti
  set azione on
  global azione
  eval casual %random.3% + 3
  wait %casual% s
  say Eccomi
end
if %settata% == on
  if %azione% != on
    set direzione %random.4%
    eval crono %crono% + %direzione%
    global direzione
    global crono
    if %direzione% == 1
      n
    elseif %direzione% == 2
      e
    elseif %direzione% == 3
      s
    elseif %direzione% == 4
      w
    end
    unset direzione
    if %crono% > 3000
      mgoto %postazione%
    end
  end
end
~
#12530
NPG -1~
0 ab 100
~
if %monarca% != 0
if %azione% == 0 || %azione% == cambio || %going% == nullo
eval negozio %random.135%
set quanti 1
set going nullo
if %negozio% <= 30
set vnum %alimentare% 
set going alimentare
eval dove %random.10%
set quanti 2
end
if %negozio% > 30 && %negozio% <= 50
set vnum %legname% 
set going legname
eval dove %random.9%
end
if %negozio% > 50 && %negozio% <= 70
set vnum %fabbro% 
set going fabbro
eval dove %random.8%
end
if %negozio% > 70 && %negozio% <= 80
set vnum %gioielliere% 
set going gioielliere
eval dove %random.5%
end
if %negozio% > 80 && %negozio% <= 100
set vnum %armiere% 
set going armiere
eval dove %random.7%
end
if %negozio% > 100 && %negozio% <= 115
set vnum %pellicceria% 
set going pellicceria
eval dove %random.8%
set quanti 2
end
if %negozio% > 115 && %negozio% <= 135
set vnum %alchimista% 
set going alchimista
set dove 1
end
global going
global quanti
if %dove% == 1
if %going% == alimentare
set compro pane
elseif %going% == legname
set compro amuleto
elseif %going% == fabbro
set compro armatura
elseif %going% == gioielliere
set compro orecchino
elseif %going% == armiere
set compro spada
elseif %going% == pellicceria
set compro gambali
elseif %going% == alchimista
set compro pozione
end
elseif %dove% == 2
if %going% == alimentare
set compro pagnotta
elseif %going% == legname
set compro bracciale
elseif %going% == fabbro
set compro guanti
elseif %going% == gioielliere
set compro anello
elseif %going% == armiere
set compro ascia
elseif %going% == pellicceria
set compro parabraccia
end
elseif %dove% == 3
if %going% == alimentare
set compro verdura
elseif %going% == legname
set compro anello
elseif %going% == fabbro
set compro gambali
elseif %going% == gioielliere
set compro amuleto
elseif %going% == armiere
set compro mazza
elseif %going% == pellicceria
set compro armatura
end
elseif %dove% == 4
if %going% == alimentare
set compro frutta
elseif %going% == legname
set compro armadio
elseif %going% == fabbro
set compro parabraccia
elseif %going% == gioielliere
set compro bracciale
elseif %going% == armiere
set compro lancia
elseif %going% == pellicceria
set compro borsa
end
elseif %dove% == 5
if %going% == alimentare
set compro pesce
elseif %going% == legname
set compro tavolo
elseif %going% == fabbro
set compro elmo
elseif %going% == gioielliere
set compro pietra
elseif %going% == armiere
set compro arco
elseif %going% == pellicceria
set compro vestito
end
elseif %dove% == 6
if %going% == alimentare
set compro dolce
elseif %going% == legname
set compro orecchino
elseif %going% == fabbro
set compro cintura
elseif %going% == armiere
set compro pugnale
elseif %going% == pellicceria
set compro guanti
end
elseif %dove% == 7
if %going% == alimentare
set compro torta
elseif %going% == armiere
set compro freccia
elseif %going% == legname
set compro scrivania
elseif %going% == fabbro
set compro stivali
elseif %going% == pellicceria
set compro stivali
end
elseif %dove% == 8
if %going% == alimentare
set compro focaccia
elseif %going% == legname
set compro cassapanca
elseif %going% == fabbro
set compro scudo
elseif %going% == pellicceria
set compro mantello
end
elseif %dove% == 9
if %going% == alimentare
set compro pizza
end
elseif %dove% == 10
if %going% == alimentare
set compro carne
end
end
say going %going%
say compro %compro%
say vnum %vnum%
if %vnum% == 0
say mgoto recettore1%going%1%monarca%
mgoto recettore1%going%1%monarca%
say vado
else
mgoto %vnum%
emote entra nel negozio
global vnum
global compro
set azione shopping
global azione
end
end
if %azione% == shopping
set soldi1 %self.gold%
global soldi1
wait 6
lista %compro%
wait 3
buy %quanti% %compro%
wait 5
set soldi2 %self.gold%
global soldi2
emote conta le sue monete
end
if %azione% == cazzeggio
eval tempo %tempo% + 1
global tempo
set direzione %random.4%
if %direzione% == 1
n
elseif %direzione% == 2
e
elseif %direzione% == 3
s
elseif %direzione% == 4
w
end
if %tempo% > 30
set azione cambio
global azione
eval reset %reset% + 1
global reset
if %reset% > 200
unset reset
unset alimentare
unset alimentare2
unset legname
unset legname2
unset fabbro
unset fabbro2
unset gioielliere
unset gioielliere2
unset alchimista
unset alchimista2
unset armiere
unset armiere2
unset pellicceria
unset pellicceria2
end
mat 3 drop all
unset tempo
end
end
if %self.gold% < 4000
mload obj 91
mat 3001 drop buono2
mat 3001 get buono2
end
end
~
#12531
NPG -2~
0 d 1
monarca ecco quel set fanno 1~
if %actor.level% > 71
if %speech% /= set
if %speech% /= monarca
%speech%
global monarca
end
end
end
if %actor.name% /= recettore
wait 1
if %settata% == on
if %speech% /= 1
say eval %going% %speech% - 1
say global %going% (going)
eval %going% %speech% - 1
global %going%
unset settata
end
end
end
~
#12532
NPG -3~
0 e 0
conta le sue monete~
if %actor.level% > 92 || %actor.alias% /= npg
wait 2
if %soldi1% > %soldi2%
unset fanno
unset compro
unset vnum
unset going
unset quanti
unset soldi1
unset soldi2
mat 3 drop all
set azione cazzeggio
global azione
emote se ne va soddisfatto
else
if %going% == alimentare
set coso %alimentare2%
elseif %going% == legname
set coso %legname2%
elseif %going% == fabbro
set coso %fabbro2%
elseif %going% == gioielliere
set coso %gioielliere2%
elseif %going% == armiere
set coso %armiere2%
elseif %going% == pellicceria
set coso %pellicceria2%
elseif %going% == alchimista
set coso %alchimista2%
end
eval %going%2 %coso% + 1
global %going%2
eval coso %coso% + 1
global coso
if %coso% > 5
shake
wait 3
say Basta, ora e' troppo, non c'e' mai niente in questo negozio
wait 4
emote se ne va con aria contrariata cercando un altro %going%
unset compro
unset vnum
unset quanti
unset %going%
unset %going%2
unset going
unset coso
else
emote tira un sospiro non trovando %compro%
wait 4
emote dice al negoziante che ripassera' piu' tardi
unset going
unset compro
unset vnum
unset quanti
end
unset coso
set azione cazzeggio
global azione
end
end
end
~
#12533
NPG -4~
0 e 0
ti dice, 'settata~
if %actor.name% /= recettore
set settata on
global settata
end
~
#12534
CAVALLI -1~
0 e 0
ti guarda~
mload mob 12601
mecho vittima Fame:	%fame%
mforce vittima mgoto 2
~
#12535
CAVALLI -2~
0 e 0
ti monta~
set proprietario %actor.name%
global proprietario
unset sgroppato
~
#12536
CAVALLI -3~
0 ab 100
~
if %start% != ko
if %start% != on
set start on
global start
set fame 1
global fame
end
if %self.room% != 3189 && %self.room% != 21652 && %self.room% != 9831 && %self.room% != 6811
eval timer %timer% + 1
global timer
end 
if %timer% > 5
eval fame %fame% + 1
global fame
unset timer
if %fame% > 92 && %fame% < 97
emote &6&xnon vivra' ancora a lungo senza cibo&0
end
if %fame% > 96 && %fame% < 100
emote &6&xsta per morire di fame&0
end
if %fame% > 99
emote &6&ximprovvisamente si irrigidisce e si affloscia a terra, morto!&0
if %sgroppato% != on
mforce %proprietario% smonta
end
sgancia carre
rem carre
drop all
mgoto 2
mpurge
set start ko
global start
end
end
if %fame% < 1
set fame 1
global fame
end
if %mangiato% > 16
mload obj 12619
mat 3 quaff pozio
emote appare ora piu' riposato e meno affamato
end
unset mangiato
end
~
#12537
CAVALLI -4~
0 j 100
~
if %object.type% == FOOD
if %object.name% /= fieno
eval mangiato %mangiato% + %object.val0%
global mangiato
wait 1
mat 3 drop all.fieno
eval fame %fame% - 5
if %fame% < 1
set fame 1
end
global fame
end
end
~
#12538
CAVALLI -5~
0 e 0
scende dalle tue spalle~
set sgroppato on
global sgroppato
~
#12539
CONTRATTO1~
0 d 1
contratto io comandi accetto finito dichiaro si no completa~
if %speech% == comandi
say Eccoli: &1contratto, io, finito, comandi, si, no, completa&0
end 
if %speech% == contratto
if %domanda% < 1
wait 1 s
nod
wait 1 s
set 1 %actor.name%
global 1
say Va bene, messer %actor.name%, chi intende stipulare un contratto con voi?
set domanda 1
global domanda
set pg 1
global pg
end
end
if %domanda% == 1
if %speech% == io
if %pg% <= 9
eval pg %pg% +1
global pg
set %pg% %actor.name%
global %pg%
wait 1 s
emote prende appunti
wait 1 s
say I partecipanti al contratto ora sono: %1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
wait 2 s
say Ci sono altri? Altrimenti dovete solo dirmi &1'completa'&0.
elseif %pg% == 10
wait 1 s
say Mi spiace, accetto contratti tra un minimo di due fino ad un massimo di 10 persone.
end
end
end
if %domanda% == 1
if %speech% == completa
if %pg% > 1
set domanda 2
global domanda
wait 1 s 
nod
wait 1 s
say Bene, ora dovete dettarmi il contratto, messer %1%, dovrete poi tutti approvarlo.
wait 1 s
say Scrivero' qualunque frase che contenga la parola &1'io', 'accetto', 'si', 'no' &5oppure&0 &1'dichiaro'&0, &5ditemi pure..&0
wait 1 s
say Quando avete finito, dovete solo dire &1'finito&0
elseif %pg% == 1
wait 1 s
shake
wait 1 s
say Mi spiace, fare contratti da soli al momento non ha molta utilita', %1%...
end
end
end
if %domanda% == 2
if %speech% != completa
if %speech% != finito
if %actor.name% == %1%
wait 1 s
if %frase% == 20
say Uh, direi che cosi' puo' bastare, e' la undicesima frase che mi vorreste dettare!
elseif %frase% >= 11
eval frase %frase% +1
global frase
set %frase% %speech%
global %frase%
emote continua a scrivere su una pergamena ...
elseif %frase% == 0
set 11 %speech%
global 11
set frase 11
global frase
emote scrive velocemente su una pergamena la prima frase...
else
wait 1 s
say ...mi spiace &actor.name%, e' importante che detti solo colui che ha iniziato la procedura burocratica
end
end
end
end
end
if %speech% == finito
wait 1 s
if %frase% == 0
say ma non mi avete dettato nulla!
else
set domanda 3
global domanda
say Bene, ora vi rileggero' tutto:
wait 2 s
say &4 %11% &0
say &4 %12% &0
say &4 %13% &0
say &4 %14% &0
say &4 %15% &0
say &4 %16% &0
say &4 %17% &0
say &4 %18% &0
say &4 %19% &0
say &4 %20% &0
wait 2 s
say ...Ora vi chiedero' di approvare questo contratto uno ad uno, per approvare dite &1'si'&0, &5se qualcuno dira'&0 &1'no'&0, &5dovrete ridettarmi da zero il contratto.&0
wait 2 s
say Messer %1%, approvate il contratto?
set approva %1%
global approva
set confermando 1
global confermando
end
end
if %domanda% == 3
if %speech% == si
wait 1 s
if %actor.name% == %approva%
if %confermando% == %pg%
nod
wait 1 s
say Bene, abbiamo finito, ora vi prego di pazientare un attimo signori mentre preparo il contratto in %pg% copie (una a testa).
wait 1 s
mload obj 12565
mload obj 12565
mload obj 12565
mload obj 12565
mload obj 12565
mload obj 12565
mload obj 12565
mload obj 12565
mload obj 12565
mload obj 12565
write 1.contratto
%11%
%12%
%13%
%14%
%15%
%16%
%17%
%18%
%19%
%20%
------
firme:
------
%1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
Certifica il Funzionario Burocratico di Deira
/s
write 2.contratto
%11%
%12%
%13%
%14%
%15%
%16%
%17%
%18%
%19%
%20%
...
firme:
...
%1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
Certifica il Funzionario Burocratico di Deira
/s
write 3.contratto
%11%
%12%
%13%
%14%
%15%
%16%
%17%
%18%
%19%
%20%
...
firme:
...
%1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
Certifica il Funzionario Burocratico di Deira
/s
write 4.contratto
%11%
%12%
%13%
%14%
%15%
%16%
%17%
%18%
%19%
%20%
...
firme:
...
%1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
Certifica il Funzionario Burocratico di Deira
/s
write 5.contratto
%11%
%12%
%13%
%14%
%15%
%16%
%17%
%18%
%19%
%20%
...
firme:
...
%1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
Certifica il Funzionario Burocratico di Deira
/s
write 6.contratto
%11%
%12%
%13%
%14%
%15%
%16%
%17%
%18%
%19%
%20%
...
firme:
...
%1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
Certifica il Funzionario Burocratico di Deira
/s
write 7.contratto
%11%
%12%
%13%
%14%
%15%
%16%
%17%
%18%
%19%
%20%
...
firme:
...
%1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
Certifica il Funzionario Burocratico di Deira
/s
write 8.contratto
%11%
%12%
%13%
%14%
%15%
%16%
%17%
%18%
%19%
%20%
...
firme:
...
%1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
Certifica il Funzionario Burocratico di Deira
/s
write 9.contratto
%11%
%12%
%13%
%14%
%15%
%16%
%17%
%18%
%19%
%20%
...
firme:
...
%1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
Certifica il Funzionario Burocratico di Deira
/s
write 10.contratto
%11%
%12%
%13%
%14%
%15%
%16%
%17%
%18%
%19%
%20%
------
firme:
------
%1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
Certifica il Funzionario Burocratico di Deira
/s
give contratto %1%
give contratto %2%
give contratto %3%
give contratto %4%
give contratto %5%
give contratto %6%
give contratto %7%
give contratto %8%
give contratto %9%
give contratto %10%
mjunk all
unset 1
unset 2
unset 3
unset 4
unset 5
unset 6
unset 7
unset 8
unset 9
unset 10
unset 11
unset 12
unset 13
unset 14
unset 15
unset 16
unset 17
unset 18
unset 19
unset 20
unset approva
unset confermando
unset domanda
unset frase
unset pg
else
nod
eval confermando %confermando% + 1
global confermando
if %confermando% == 2
set approva %2%
global approva
elseif %confermando% == 3
set approva %3%
global approva
elseif %confermando% == 4
set approva %4%
global approva
elseif %confermando% == 5
set approva %5%
global approva
elseif %confermando% == 6
set approva %6%
global approva
elseif %confermando% == 7
set approva %7%
global approva
elseif %confermando% == 8
set approva %8%
global approva
elseif %confermando% == 9
set approva %9%
global approva
elseif %confermando% == 10
set approva %10%
global approva
end
wait 1 s
say Ora tocca a %approva% confermare, accetta questo contratto?
end
end
end
end
if %speech% == no
if %actor.name% == %approva%
wait 1 s
look %approva%
wait 1 s
emote sospira deluso
wait 1 s
say Ebbene...grazie a %actor.name% ci tocchera' ricominciare tutto daccapo signori.
unset conferma
unset confermando
unset 11
unset 12
unset 13
unset 14
unset 15
unset 16
unset 17
unset 18
unset 19
unset 20
set domanda 2
global domanda
wait 2 s
say Ora, messer %1% la prego di dettarmi di nuovo le frasi da inserire nel contratto con le solite parole chiavi &1(dichiaro, accetto, si, no, io)&0
end
end
~
#12540
CONTROLLO MAGAZZINO~
0 d 1
permessi eccomi funzionario set preleva esamina prendi get 2 3 4 5 6 7 8 9 10~
if %speech% == permessi
wait 2
say Oltre al Monarca %monarca% hanno accesso a questi magazzini:
say 1- %funzionario% (&1Funzionario primo&0)
say 2- %2%
say 3- %3%
say 4- %4%
say 5- %5%
say 6- %6%
say 7- %7%
say 8- %8%
say 9- %9%
say 10- %10%
end
if %actor.name% == %monarca%
if %speech% == funzionario
wait 5
say Che il nuovo funzionario si faccia avanti (&1Say Eccomi&0)
set carica funzionario
global carica
end
end
if %speech% == eccomi
if %carica% == funzionario
set funzionario %actor.name%
global funzionario
wait 2
unset carica
nod
else
set %cambio% %actor.name%
global %cambio%
unset cambio
end
end
if %actor.name% == %funzionario% || %actor.name% == %monarca%
if %speech% == cambia permessi
set cambio on
global cambio
wait 4
say Che numero? (2-10)
end
if %cambio% == on
if %speech% > 1 && %speech% < 11
set cambio %speech%
global cambio
wait 5
say Bene, che il nuovo privilegiato si faccia avanti (&1Say eccomi&0)
end
end
end
if %actor.level% > 70
if %speech% /= set
%speech%
if %speech% /= armeria
global armeria
elseif %speech% /= fabbro
global fabbro
elseif %speech% /= falegname
global falegname
elseif %speech% /= pellicceria
global pellicceria
elseif %speech% /= alchimista
global alchimista
elseif %speech% /= alimentare
global alimentare
elseif %speech% /= gioielleria
global gioielleria
elseif %speech% /= monarca
global monarca
end
end
end
if %actor.name% == %monarca% || %actor.name% == %funzionario% || %actor.name% == %2% || %actor.name% == %3% || %actor.name% == %4% || %actor.name% == %5%
set pass ok
end
if %actor.name% == %6% || %actor.name% == %7% || %actor.name% == %8% || %actor.name% == %9% || %actor.name% == %10%
set pass ok
end
if %pass% == ok
if %speech% /= preleva
set stanza2 0
if %speech% == preleva armeria
set stanza2 %armeria%
elseif %speech% == preleva fabbro
set stanza2 %fabbro%
elseif %speech% == preleva falegname
set stanza2 %falegname%
elseif %speech% == preleva pellicceria
set stanza2 %pellicceria%
elseif %speech% == preleva alchimista
set stanza2 %alchimista%
elseif %speech% == preleva alimentare
set stanza2 %alimentare%
elseif %speech% == preleva gioielleria
set stanza2 %gioielleria%
end
global stanza2
if %stanza2% != 0
wait 3
nod
wait 5
say Ditemi pure cosa prendere, messer %actor.name%
end
end
if %stanza2% != 0
if %speech% /= prendi || %speech% /= get
set pg2 %actor.name%
global pg2
mat %stanza2% %speech%
wait 1
drop all
unset stanza2
wait 5
say Ecco a voi
end
end
if %speech% /= esamina
set stanza 0
if %speech% == esamina armeria
set stanza %armeria%
elseif %speech% == esamina fabbro
set stanza %fabbro%
elseif %speech% == esamina falegname
set stanza %falegname%
elseif %speech% == esamina pellicceria
set stanza %pellicceria%
elseif %speech% == esamina alchimista
set stanza %alchimista%
elseif %speech% == esamina alimentare
set stanza %alimentare%
elseif %speech% == esamina gioielleria
set stanza %gioielleria%
end
global stanza
if %stanza% != 0
wait 3
set casa %self.room%
global casa
mtele %actor.name% %stanza%
mgoto %stanza%
mforce %actor.name% look
mtele %actor.name% %casa%
mgoto %casa%
unset casa
unset stanza
end
end
end
~
#12541
CONTROLLO MAGAZZINO 2~
0 e 1
prendi~
if %object.vnum% < 12699
if %object.name% /= diamante || %object.name% /= topazio || %object.name% /= smeraldo || %object.name% /= rubino || %object.name% /= zaffiro
eval gioielleriaD %gioielleriaD% + 1
global gioielleriaD
end
if %object.name% /= pelle || %object.name% /= pelliccia || %object.name% /= stoffa
eval pellicceriaD %pellicceriaD% + 1
global pellicceriaD
end
if %object.type% == potion
eval alchimistaD %alchimistaD% + 1
global alchimistaD
end
if %object.name% /= quercia || %object.name% /= mogano || %object.name% /= betulla || %object.name% /= frassino || %object.name% /= castagno || %object.name% /= noce || %object.name% /= abete || %object.name% /= pioppo
if %object.type% != scudo
eval falegnameD %legnameD% + 1
global falegnameD
end
end
if %object.type% == food
eval cibo2 %cibo2% + 1
global cibo2
end
if %object.type% == armor
if %object.name% /= oro || %object.name% /= rame || %object.name% /= argento || %object.name% /= stagno || %object.name% /= ferro || %object.name% /= zinco
eval fabbroD %fabbroD% + 1
global fabbroD
end
end
if %object.type% == weapon || %object.type% == bow || %object.name% /= arco || %object.name% /= freccia
eval armeriaD %armeriaD% + 1
global armeriaD
end
end
~
#12542
CONTROLLO MAGAZZINO 3~
0 j 100
~
if %object.vnum% < 12699
if %object.name% /= diamante || %object.name% /= topazio || %object.name% /= smeraldo || %object.name% /= rubino || %object.name% /= zaffiro
eval gioielleriaP %gioielleriaD% + 1
global gioielleriaP
mat %gioielleria% drop all
end
if %object.name% /= pelle || %object.name% /= pelliccia || %object.name% /= stoffa
eval pellicceriaP %pellicceriaP% + 1
global pellicceriaP
mat %pellicceria% drop all
end
if %object.type% == potion
eval alchimistaP %alchimistaP% + 1
global alchimistaP
mat %alchimista% drop all
end
if %object.name% /= quercia || %object.name% /= mogano || %object.name% /= betulla || %object.name% /= frassino || %object.name% /= castagno || %object.name% /= noce || %object.name% /= abete || %object.name% /= pioppo
if %object.type% != scudo
eval falegnameP %legnameP% + 1
global falegnameP
mat %falegname% drop all
end
end
if %object.type% == food
eval cibo %cibo% + 1
global cibo
mat %alimentare% drop all
end
if %object.type% == armor || %object.type% /= scudo
if %object.name% /= oro || %object.name% /= rame || %object.name% /= argento || %object.name% /= stagno || %object.name% /= ferro || %object.name% /= zinco
eval fabbroP %fabbroP% + 1
global fabbroP
mat %fabbro% drop all
end
end
if %object.type% == weapon || %object.type% == bow || %object.name% /= arco || %object.name% /= freccia
eval armeriaP %armeria% + 1
global armeriaP
mat %armeria% drop all
end
set pg %actor.name%
global pg
end
~
#12543
CONTROLLO MAGAZZINO 4~
0 ab 100
~
if %pellicceriaD% > 0
mat pellicceria1%monarca% say set pg %pg%
mat pellicceria1%monarca% say set quanti + %pellicceriaD%
unset pellicceriaD
mat %pellicceria% drop all
end
if %pellicceriaP% > 0
mat pellicceria1%monarca% say set pg %pg%
mat pellicceria1%monarca% say set quanti - %pellicceriaP%
unset pellicceriaP
mat %pellicceria% drop all
end
if %gioielleriaD% > 0
mat preziose1%monarca% say set pg %pg%
mat preziose1%monarca% say set quanti + %gioielleriaD%
unset gioielleriaD
mat %alchimista% drop all
end
if %gioielleriaP% > 0
mat preziose1%monarca% say set pg %pg%
mat preziose1%monarca% say set quanti - %gioielleriaP%
unset gioielleriaP
mat %gioielleria% drop all
end
if %alchimistaD% > 0
mat erbe1%monarca% say set pg %pg%
mat erbe1%monarca% say set quanti + %alchimistaD%
unset alchimistaD
mat %alchimista% drop all
end
if %alchimistaP% > 0
mat erbe1%monarca% say set pg %pg%
mat erbe1%monarca% say set quanti - %alchimistaP%
unset alchimistaP
mat %alchimista% drop all
end
if %falegnameD% > 0
mat legname1%monarca% say set pg %pg%
mat legname1%monarca% say set quanti - %falegnameD%
unset falegnameD
mat %falegname% drop all
end
if %falegnameP% > 0
mat legname1%monarca% say set pg %pg%
mat legname1%monarca% say set quanti + %falegnameD%
unset falegnameP
mat %falegname% drop all
end
if %cibo% > 0
mat cibo1%monarca% say set pg %pg%
mat cibo1%monarca% say set quanti - %cibo%
unset cibo
mat %alimentare% drop all
end
if %cibo2% > 0
mat cibo1%monarca% say set pg %pg2%
mat cibo1%monarca% say set quanti + %cibo2%
unset cibo2
end
if %fabbroP% > 0
mat metalli1%monarca% say set pg %pg%
mat metalli1%monarca% say set quanti - %armeriaP%
unset fabbroP
mat %fabbro% drop all
end
if %FabbroD% > 0
mat metalli1%monarca% say set pg %pg2%
mat metalli1%monarca% say set quanti + %armeriaD%
unset FabbroD
end
if %armeriaP% > 0
mat metalli1%monarca% say set pg %pg%
mat metalli1%monarca% say set quanti - %armeriaP%
unset armeriaP
mat %armeria% drop all
end
if %armeriaD% > 0
mat metalli1%monarca% say set pg %pg2%
mat metalli1%monarca% say set quanti + %armeriaD%
unset armeriaD
end
set pg 0
set pg2 0
unset pg
unset pg2
~
#12546
Venditore_Bodyguard~
0 d 1
prodotti compro setta 1 2 3 4 5 6 7 8 9 10~
if %actor.level% > 71
if %speech% == setta
set 1a 20
global 1a
set 2a 25
global 2a
set 3a 30
global 3a
set 4a 35
global 4a
set 5a 40
global 5a
set 6a 45
global 6a
set 7a 50
global 7a
set 8a 55
global 8a
set 9a 60
global 9a
set 10a 70
global 10a
end
end
if %speech% == prodotti
wait 2
say Ecco la lista delle guardie disponibili:
say   Livello:     Prezzo:
say 1- %1a%        22000
say 2- %2a%        28750        
say 3- %3a%        37000       
say 4- %4a%        46750      
say 5- %5a%        58000
say 6- %6a%        70750
say 7- %7a%        85000
say 8- %8a%        100750
say 9- %9a%        118000
say 10- %10a%       157000
end
if %azione% == 0
if %speech% == compro
set azione compro
global azione
wait 2
say che numero?
end
end
if %azione% == compro
if %speech% < 11
if %speech% == 1
set cambio 22000
global cambio
elseif %speech% == 2
set cambio 28750
global cambio
elseif %speech% == 3
set cambio 37000
global cambio
elseif %speech% == 4
set cambio 46750
global cambio
elseif %speech% == 5
set cambio 58000
global cambio
elseif %speech% == 6
set cambio 70750
global cambio
elseif %speech% == 7
set cambio 85000
global cambio
elseif %speech% == 8
set cambio 100750
global cambio
elseif %speech% == 9
set cambio 118000
global cambio
elseif %speech% == 10
set cambio 157000
global cambio
end
set vnum 1269%speech%
global vnum
if %speech% == 10
set vnum 12690
global vnum
end
wait 3
say Bene, vi prego di darmi allora le %cambio% coins necessarie per assumere la %speech% guardia in elenco
set azione pagare
global azione
end
end
~
#12547
Venditore_bodyguard2~
0 m 100
~
if %azione% == pagare
if %amount% >= %cambio%
wait 3
nod
mload mob %vnum%
say proprietario %actor.name%
wait 5
say ecco a voi
unset cambio
unset pagare
unset azione
unset vnum
unset timer
else
wait 2
drop %amount% coins
wait 5
say non bastano
end
else
wait 3
say ?
drop %amount% coins
end
~
#12548
Venditore_bodyguard3~
0 ab 100
~
if %azione% != 0
eval timer %timer% + 1
global timer
if %timer% > 12
unset timer
unset pagare
unset azione
unset cambio
unset vnum
emote si guarda attorno alla ricerca del suo interlocutore
wait 1 s
emote e' ora nuovamente disponibile per la vendita
end
end
~
#12549
Negozianti1~
0 j 100
~
if %object.name% == buono reale
nod
wait 1 s
mload obj 12555
mat 3001 drop teso
mat 3001 get teso
mat 3001 drop buono
say Vi rigrazio, Sir %Actor.name%
else
if %actor.name% == %nome%
if %receive% == on
if %object.name% == %ogg%
eval tot %num% +1
global tot
set num %tot%
global num
else
if %ricevuto% != on
set num 1
set ogg %object.name%
set ricevuto on
set costo %object.cost%
global num
global ogg
global ricevuto
global costo
else
wait 5
shake
wait 5
say volevate fare il furbo eh?
wait 1 s
say ora dovremo ricominciare daccapo...riprendete la vostra roba
drop all
unset tot
unset costo
unset ricevuto
unset num
end
end
else
say &1Non cercare di fregarmi!&0
drop all.%object%
end
else
say &1Non e' il tuo turno %actor.name%! &0
end
end
~
#12550
Ferramenta~
0 d 1
posa stanza segui finito funzionario~
if %actor.name% == Il segretario reale
if %speech% /= segui
%speech%
wait 2
nod
end
if %speech% /= funzionario
set %speech%
global funzionario
wait 2
nod
end
end
if %actor.name% == %funzionario%
if %speech% == finito
wait 2
emote se ne va
wait 2
mgoto 2
foll ferra
end
if %speech% == stanza
wait 2
say stanza numero: %self.room%
end
if %speech% == fissa
if %self.room% > 39999 && %self.room% < 40200
wait 5
emote tira un sospiro, inizia a montare rapidamente una cassaforte nel muro
wait 1 s
mload mob 12607
wait 5
Say Finito, vi bastera' darle la roba da tenere al sicuro
wait 5
say Per riprenderla non dovrete fare altro che dirle &1'Say posa <oggetto>'&0
wait 8
say Ora il dovere mi chiama, a presto signori
wait 5
emote si dirige a passi rapidi verso il Palazzo Reale
mgoto 2
else
wait 5
emote si guarda attorno....
wait 5
say Qui???? Ma siete pazzo!
end
end
end
~
#12551
CASSAFORTE~
0 d 1
prendi proprietario permessi eccomi posa 1 2 3 4 5~
sit
if %actor.name% == %1% || %actor.name% == %2% || %actor.name% == %3% || %actor.name% == %4% || %actor.name% == %5%
if %speech% /= prendi
set %speech%
%speech%
mat 1 put %prendi% in merce
end
if %speech% /= posa
set %speech%
mat 1 get %posa% in merce
%speech%
get merce
end
end
if %speech% == proprietario
wait 3
if %1% == 0
nod
mload obj 12600
wait 3
set 1 %actor.name%
global 1
else
say Ho gia' un proprietario!
end
end
if %actor.name% == %1%
if %speech% == cambia permessi
set cambio on
global cambio
wait 4
say Che numero? (2-5)
end
if %cambio% == on
if %speech% > 1 && %speech% < 6
set cambio %speech%
global cambio
wait 5
say Bene, che il nuovo privilegiato si faccia avanti (&1Say eccomi&0)
end
end
end
if %speech% == eccomi
if %cambio% > 1 && %cambio% < 6 
set %cambio% %actor.name%
global %cambio
wait 5
say Salve a voi, %actor.name%
unset cambio
end
end
if %speech% == permessi
wait 3
say 1- Proprietario: %1%
say Privilegiati:
say 2- %2%
say 3- %3%
say 4- %4%
say 5- %5%
end
if %speech% == annulla
if %cambio% != 0
wait 4
nod
unset cambio
end
end
follow cassa
~
#12552
CASSAFORTE2~
0 e 0
ti guarda~
mload mob 12601
drop merce
mforce %actor.name% look in merce
get merce
mforce vittima mgoto 2
mat 2 mpurge
~
#12554
Bodyguard1~
0 d 1
comandi alzati stop seguimi fermati proprietario dormi alzati~
if %actor.name% /= venditore
if %speech% /= proprietario
if %proprietario% == 0
set %speech%
global proprietario
end
end
end
if %stipendio% < 2
if %actor.name% == %proprietario%
if %speech% == dormi
wait 5
yawn
wait 1 s
sleep
end
if %speech% == alzati
wait 4
stand
end
if %speech% == comandi
wait 3
say &1 Ecco la lista: dormi, alzati, stop, seguimi, fermati, alzati
end
if %speech% == seguimi
wait 4
say Come desiderate, %Actor.name%
wait 6
foll %actor.name%
end
if %speech% == fermati
wait 7
foll %self%
end
if %speech% == stop
stop
wait 1
stop
end
end
end
~
#12555
Bodyguard2~
0 ab 100
~
eval pagare %pagare% + 1
global pagare
if %pagare% > 21000
eval stipendio %stipendio% + 1
global stipendio
eval cifra (%self.level% * %self.level% * 6 + 2000)
global cifra
if %stipendio% > 3
emote si autolicenzia visti i mancati pagamenti
mgoto 2
mpurge
end
end
~
#12556
Bodyguard3~
0 e 0
ti guarda~
mload mob 12601
mecho vittima Proprietario: 	%proprietario%
if %stipendio% > 0
mecho vittima Stipendio da pagare alla guardia (%cifra% coins x %stipendio%)
end
mforce vittima mgoto 2
mat 2 mpurge
~
#12557
Bodyguard4~
0 m 100
~
if %stipendio% > 0
if %amount% > %cifra%
eval stipendio %stipendio% - 1
global stipendio
wait 4
smile
wait 5
Say Grazie per lo stipendio, al vostro servizio %actor.name%
wait 5
bow
else
wait 5
say Non mi servono al momento
wait 5
drop %amount% coins
end
end
~
#12559
buoni mercato interno~
0 j 100
~
if %object.name% == buono reale
mforce %actor.name% junk buono
wait 5
mload obj 12555
mat 3001 drop teso
mat 3001 get teso
mat 3001 drop buono
say Vi rigrazio, Sir %Actor.name%
end
~
#12560
magazziniere1~
0 d 1
posare prendere~
if %speech% == prendere
get all in 1.scorta
get all in 2.scorta
get all in 3.scorta
get all in 4.scorta
get all in 5.scorta
get all in 6.scorta
get all in 7.scorta
get all in 8.scorta
get all in 9.scorta
get all in 10.scorta
end
if %speech% == posare
put all.%1% in 1.scorta
put all.%2% in 2.scorta
put all.%3% in 3.scorta
put all.%4% in 4.scorta
put all.%5% in 5.scorta
put all.%6% in 6.scorta
put all.%7% in 7.scorta
put all.%8% in 8.scorta
put all.%9% in 9.scorta
put all.%10% in 10.scorta
end
~
#12561
NEGOZI NUOVI TELL~
0 e 1
1~
if %speech% /= 1
eval resto %speech% - 1
global resto
wait 1
give %resto% coins controllore
end
~
#12562
NEGOZI NUOVI -1~
0 d 1
no bilancio preleva permessi eccomi richiesta appalto titolo alza abbassa fine completo proprietario magazzino negozio prodotti cambia~
%echo% Non attivo causa bug
~
#12563
NEGOZI NUOVI -2~
0 d 1
finito fatto 1 2 3 4 5 6 7 8 9 10 si compro elimina~
%echo% Non attivo causa bug
~
#12564
NEGOZI NUOVI -3~
0 e 1
prendi~
eval stock %stock% +1
global stock
eval tot %tot% + %object.cost%
global tot
if %appalto% == on
unset tot
end
~
#12565
NEGOZI NUOVI -4~
0 j 100
~
if %object.name% == prodotto prova
else
if %object.name% == buono reale
mforce %actor.name% junk buono
wait 5
mload obj 12555
mat 3001 drop teso
mat 3001 get teso
mat 3001 drop buono
say Vi rigrazio, Sir %Actor.name%
else
if %compro% != on
if %actor.name% == %1a%
set pass ok
global pass
set vendendo %1%
global vendendo
set cassa 1
global cassa
end
if %actor.name% == %2a%
set pass ok
global pass
set vendendo %2%
global vendendo
set cassa 2
global cassa
end
if %actor.name% == %3a%
set pass ok
global pass
set vendendo %3%
global vendendo
set cassa 3
global cassa
end
if %actor.name% == %4a%
set pass ok
global pass
set vendendo %4%
global vendendo
set cassa 4
global cassa
end
if %actor.name% == %5a%
set pass ok
global pass
set vendendo %5%
global vendendo
set cassa 5
global cassa
end
if %actor.name% == %6a%
set pass ok
global pass
set vendendo %6%
global vendendo
set cassa 6
global cassa
end
if %actor.name% == %7a%
set pass ok
global pass
set vendendo %7%
global vendendo
set cassa 7
global cassa
end
if %actor.name% == %8a%
set pass ok
global pass
set vendendo %8%
global vendendo
set cassa 8
global cassa
end
if %actor.name% == %9a%
set pass ok
global pass
set vendendo %9%
global vendendo
set cassa 9
global cassa
end
if %actor.name% == %10a%
set pass ok
global pass
set vendendo %10%
global vendendo
set cassa 10
global cassa
end
wait 2
mgoto %magazzino%
put all in 11.scorta
get all in 11.scorta
if %self.gold% => %tot%
if %pass% != ok
drop all
end
mgoto %negozio%
say farebbero %tot% monete, ma essendo la percentuale in ricezione pari a %valore%...
wait 2
eval tot (%tot% * %valore%) / 100
global tot
say ...il tutto vale %tot% monete
wait 3
give %tot% coins %actor.name%
unset stock
unset tot
elseif %self.gold% < %tot%
mgoto %negozio%
wait 2
say Mi spiace ma non ho abbastanza soldi per pagarvi al momento, messer %actor.name%
wait 2
drop all
unset stock
unset tot
set rosso on
global rosso
end
if %rosso% != on
if %pass% == ok
mgoto %magazzino%
say posare
put all.%vendendo% in %cassa%.scorta
wait 4
get all.%vendendo% in %cassa%.scorta
if %stock% < %astock%
say stock sotto %astock%
unset tot
put all.%vendendo% in %cassa%.scorta
wait 1
say prendere
drop all
mgoto %negozio%
eval residuo %astock% - %stock%
global residuo
say Vi mancano ancora %residuo% unità per completare lo stock di merce
elseif %stock% >= %astock%
say stock sopra %astock%
if %self.gold% >= %tot%
drop all
mgoto %negozio%
wait 5
say Avete completato egregiamente il vostro lavoro, eccovi i soldi che vi spettano
wait 5
Say Dunque, %pappalto% per cento di %tot%.....
wait 5
emote fa un rapido calcolo su un foglietto
eval tot (%tot% * %pappalto%) / 100
global tot
wait 5
say Ecco a voi %tot% monete
wait 5
give %tot% coins %actor.name%
elseif %self.gold% < %tot%
put all.%vendendo% in %cassa%.scorta
drop all
unset tot
mgoto %negozio%
say Mi spiace, ma non ho abbastanza soldi per pagarvi l'appalto
end
unset %cassa%a
end
end
end
end
end
unset stock
unset tot
unset cassa
unset rosso
set nome null
global nome
unset pass
unset residuo
unset vendendo
unset timer
end
~
#12566
NEGOZI PIU NUOVI --1~
0 d 1
compro buongiorno resto 1 2 3 4 5 6 7 8 9 10~
if %actor.name% /= npg
if %speech% > 0 && %speech% < 11
wait 3
nod
wait 3
if %speech% == 1
set prodotto %1%
elseif %speech% == 2
set prodotto %2%
elseif %speech% == 3
set prodotto %3%
elseif %speech% == 4
set prodotto %4%
elseif %speech% == 5
set prodotto %5%
elseif %speech% == 6
set prodotto %6%
elseif %speech% == 7
set prodotto %7%
elseif %speech% == 8
set prodotto %8%
elseif %speech% == 9
set prodotto %9%
elseif %speech% == 10
set prodotto %10%
end
say %prodotto%
end
end
if %actor.name% /= controllore
if %speech% == buongiorno
wait 5
say Buongiorno a voi
end
if %speech% /= resto
wait 2
set %speech%
give %resto% coins controllore
mat 3 drop prodotto
end
end
~
#12567
NEGOZI PIU NUOVI --2~
0 ab 100
~
if %settata% != on
set settata on
global settata
if %self.name% /= falegname
set num 9
global num
set 1 orecchino
global 1
set 2 amuleto
global 2
set 3 bracciale
global 3
set 4 anello
global 4
set 5 armadio
global 5
set 6 tavolo
global 6
set 7 porta
global 7
set 8 scrivania
global 8
set 9 cassapanca
global 9
end
if %self.name% /= fabbro
set num 8
global num
set 1 armatura
global 1
set 2 guanti
global 2
set 3 gambali
global 3
set 4 parabraccia
global 4
set 5 elmo
global 5
set 6 cintura
global 6
set 7 stivali
global 7
set 8 scudo
global 8
end
if %self.name% /= gioielliere
set num 5
global num
set 1 orecchino
global 1
set 2 anello
global 2
set 3 amuleto
global 3
set 4 bracciale
global 4
set 5 pietra
global 5
end
if %self.name% /= armiere
set num 7
global num
set 1 spada
global 1
set 2 ascia
global 2
set 3 mazza
global 3
set 4 lancia
global 4
set 5 arco
global 5
set 6 freccia
global 6
set 7 pugnale
global 7
end
if %self.name% /= pellicceria
set num 8
global num
set 1 gambali
global 1
set 2 parabraccia
global 2
set 3 armatura
global 3
set 4 borsa
global 4
set 5 vestito
global 5
set 6 guanti
global 6
set 7 stivali
global 7
set 8 mantello
global 8
end
if %self.name% /= alimentare
set num 10
global num
set 1 pagnotta
global 1
set 2 focaccia
global 2
set 3 pizza
global 3
set 4 pane
global 4
set 5 carne
global 5
set 6 torta
global 6
set 7 dolce
global 7
set 8 verdura
global 8
set 9 frutta
global 9
set 10 pesce
global 10
end
if %self.name% /= alchimista
set num 1
global num
set 1 pozione
global 1
end
end
~
#12568
RECETTORE1~
0 d 1
chi cambia il la negozio di un una aggiornamento vado lista setta 1 2 3 4 5 6 7 8 9 10~
%echo% non attivo causa bug
~
#12569
CONTROLLORE1~
0 d 1
ditemi buongiorno configura eccomi finiti 1~
if %speech% == buongiorno a voi
if %actor.level% > 71
mload obj 1299
give prodotto negoz
set azione dentro
global azione
wait 4
say Vi prego di prestarmi attenzione solo un istante mentre controllo la vostra merce
wait 4
set coins1 %self.gold%
global coins1
buy prodotto
wait 1
set coins2 %self.gold%
global coins2
eval resto %coins1% - %coins2%
global resto
say resto %resto%
wait 8
eval a%going% %resto%
global a%going%
eval going %going% + 1
global going
wait 5
mjunk all
mat 3 drop prodotto
say Bene, abbiamo finito, a presto
set azione go
global azione
unset resto
unset coins1
unset coins2
end
end
if %speech% == configura
wait 3
say chi mi comanda parli ora
set configura on
global configura
end
if %configura% == on
if %speech% == eccomi
wait 2
say Va bene, %actor.name%
set recettore %actor.vnum%
global recettore
set stanza %self.room%
global stanza
set azione start
global azione
unset configura
end
end
if %actor.vnum% == %recettore%
if %speech% == ditemi pure
wait 2
nod
wait 3
say 1 + %a1%
unset a1
unset 1
if %negozi% >= 2
wait 8
say 1 + %a2%
unset a2
unset 2
end
if %negozi% >= 3
wait 8
say 1 + %a3%
unset a3
unset 3
end
if %negozi% >= 4
wait 8
say 1 + %a4%
unset a4
unset 4
end
if %negozi% >= 5
wait 8
say 1 + %a5%
unset a5
unset 5
end
if %negozi% >= 6
wait 8
say 1 + %a6%
unset a6
unset 6
end
if %negozi% >= 7
wait 8
say 1 + %a7%
unset a7
unset 7
end
if %negozi% >= 8
wait 8
say 1 + %a8%
unset a8
unset 8
end
if %negozi% >= 9
wait 8
say 1 + %a9%
unset a9
unset 9
end
if %negozi% == 10
wait 8
say 1 + %a10%
unset a10
unset 10
end
set azione locazione
global azione
if %going% != 0
unset going
end
end
if %azione% == start || %azione% == locazione
if %speech% /= 1
eval cambio %cambio% + 1
global cambio
eval %cambio% %speech% - 1
global %cambio%
end
if %speech% == finiti
set azione go
global azione
set negozi %cambio%
global negozi
set going 1
global going
unset timer
unset cambio
wait 2
nod
end
end
end
~
#12570
CONTROLLORE2~
0 ab 100
~
if %self.gold% < 1000
mload obj 12579
mat 3002 drop soldi
mat 3002 get soldi
end
if %azione% == start
mgoto %stanza%
unset negozi
set cambio null
unset cambio
wait 3
say lista locazioni
end
if %azione% == go
if %going% <= %negozi%
if %going% == 1
mgoto %1%
elseif %going% == 2
mgoto %2%
elseif %going% == 3
mgoto %3%
elseif %going% == 4
mgoto %4%
elseif %going% == 5
mgoto %5%
elseif %going% == 6
mgoto %6%
elseif %going% == 7
mgoto %7%
elseif %going% == 8
mgoto %8%
elseif %going% == 9
mgoto %9%
elseif %going% == 10
mgoto %10%
end
wait 2
say buongiorno
end
end
if %azione% == go
if %going% > %negozi%
mgoto %stanza%
wait 2
say cambia prezzi
end
end
if %azione% == locazione
eval timer %timer% + 1
global timer
if %timer% >= 200
unset timer
set cambio null
unset cambio
unset negozi
say lista locazioni
end
end
~
#12571
nascita CONTROLLORE~
0 ab 100
~
if %start% != on
set start on
global start
mload mob 12551
wait 2
say configura
end
~
#12572
entra a palazzo (Cristal)~
0 d 100
devo entrare~
* Lista comandi
if %speech% == devo entrare
if %actor.clan% == 5 && %actor.rank% > 18
%echo% La guardia ai cancelli da un cenno ad un altra che sta all'argano ed i cancelli inizian lentamente ad aprirsi per far passare %actor.name%
%teleport% %actor.name% 27000
elseif %actor.name% == rakahrt
%teleport% %actor.name% 27000
  else
    wait 2 s
    say Mi dispiace, non vi e' permesso l'accesso al palazzo.
  end
end
~
#12573
ammazza il tonno~
0 g 100
~
if %actor.clan% == 5
then kil %actor%
~
#12574
esci da palazzo (Cristal)~
0 d 100
devo uscire~
* Lista comandi
if %speech% == devo uscire
if %actor.clan% == 5 && %actor.rank% > 20
%echo% la guardia da un segnale e i cancelli si aprono lentamente per far passare %actor.name%
%teleport% %actor.name% 12522
elseif %actor.name% == rakahrt
%teleport% %actor.name% 12522
else
wait 2 s
say Mi dispiace, non vi e' permesso attraversare questi cancelli
end
end
~
#12577
controllo guardie elite (Kain)~
0 i 100
none~
* No Script
eval room %self.room%
eval vnum %room.vnum%
* se non compreso nei vnum indicati imposta il controllo come falso (0) altrimenti come vero (1)
if %vnum% < 27000 || %vnum% > 27099
  eval controllo1 0
else
  eval controllo1 1
end
if %vnum% < 2800 || %vnum% > 3199
  eval controllo2 0
else
  eval controllo2 1
end
if %vnum% < 12500 || %vnum% > 12699
  eval controllo3 0
else
  eval controllo3 1
end
if %vnum% < 2120000 || %vnum% > 2129999
  eval controllo4 0
else
  eval controllo4 1
end
*%echostaff% 1:%controllo1% 2:%controllo2% 3:%controllo3%
* se tutti i controlli sono falsi il vnum e' vietato
if %controllo1% == 0
  if %controllo2% == 0
    if %controllo3% == 0
      if %controllo4% == 0
        say Qui non posso stare! Torno in caserma!
        follow me
        %teleport% %self% 12500
      end
    end
  end
end
 
~
#12590
arena1~
0 d 1
morto combattenti est arena spalti si no nord ovest inizio annulla resetta~
if %reset% > 100
if %speech% == resetta
set reset 0
nod
global reset
mload mob 12595
mgoto 2
end
end
if %actor.name% == Un giudice
if %speech% /= morto
if %speech% == %1% e' morto
set cadavere 1
global cadavere
end
if %speech% == %2% e' morto
set cadavere 2
global cadavere
end
if %speech% == %3% e' morto
set cadavere 3
global cadavere
end
if %speech% == %4% e' morto
set cadavere 4
global cadavere
end
if %speech% == %5% e' morto
set cadavere 5
global cadavere
end
if %speech% == %6% e' morto
set cadavere 6
global cadavere
end
if %speech% == %7% e' morto
set cadavere 7
global cadavere
end
if %speech% == %8% e' morto
set cadavere 8
global cadavere
end
if %speech% == %9% e' morto
set cadavere 9
global cadavere
end
if %speech% == %10% e' morto
set cadavere 10
global cadavere
end
if %speech% == %11% e' morto
set cadavere 11
global cadavere
end
if %speech% == %12% e' morto
set cadavere 12
global cadavere
end
if %speech% == %13% e' morto
set cadavere 13
global cadavere
end
if %speech% == %14% e' morto
set cadavere 14
global cadavere
end
if %speech% == %15% e' morto
set cadavere 15
global cadavere
end
if %speech% == %16% e' morto
set cadavere 16
global cadavere
end
if %speech% == %17% e' morto
set cadavere 17
global cadavere
end
if %speech% == %18% e' morto
set cadavere 18
global cadavere
end
if %speech% == %19% e' morto
set cadavere 19
global cadavere
end
if %speech% == %20% e' morto
set cadavere 20
global cadavere
end
wait 1
nod
unset %cadavere%
unset cadavere
eval dentro %dentro% - 1
global dentro
if %dentro% == 1
wait 5
say Ebbene, ABBIAMO UN VINCITORE!
mat %31% say Ebbene, ABBIAMO UN VINCITORE!
mat %32% say Ebbene, ABBIAMO UN VINCITORE!
mat %33% say Ebbene, ABBIAMO UN VINCITORE!
mat %34% say Ebbene, ABBIAMO UN VINCITORE!
mat %35% say Ebbene, ABBIAMO UN VINCITORE!
mteleport %1% 29504
mteleport %2% 29504
mteleport %3% 29504
mteleport %4% 29504
mteleport %5% 29504
mteleport %6% 29504
mteleport %7% 29504
mteleport %8% 29504
mteleport %9% 29504
mteleport %10% 29504
mteleport %11% 29504
mteleport %12% 29504
mteleport %13% 29504
mteleport %14% 29504
mteleport %15% 29504
mteleport %16% 29504
mteleport %17% 29504
mteleport %18% 29504
mteleport %19% 29504
mteleport %20% 29504
wait 5
say Complimenti vivissimi
mload mob 12595
mgoto 2
wait 3
clap
wait 2 s
wait ....e con questo e' tutto
mpurge
unset inizio
unset arena
unset 1
unset 2
unset 3
unset 4
unset 5
unset 6
unset 7
unset 8
unset 9
unset 10
unset 11
unset 12
unset 13
unset 14
unset 15
unset 16
unset 17
unset 18
unset 19
unset 20
unset 31
unset 32
unset 33
unset 34
unset 35
unset 36
unset 37
unset 38
unset 39
unset 40
unset cambio2
unset cambia
end
end
end
if %speech% == combattenti
if %starter% >= 1 || %inizio% == on
wait 2
say Ecco i %dentro% partecipanti: %1% %2% %3% %4% %5% %6% %7% %8% %9% %10%
else
wait 3
say Non c'e' nessuno dentro al momento
end
end
if %speech% == inizio
if %actor.name% == %1%
wait 5
nod
wait 3
say l'arena partira' tra meno di un minuto
set starter 20
global starter
else
wait 4
emote guarda %actor.name% attonito
wait 5
say Non l'avete avviata voi questa arena
end
end
if %arena% == 0
if %inizio% != on
if %speech% == arena
if %dentro% < 20
if %dentro% == 0
set reset 0
global reset
wait 3
say Volete dunque iniziare un'arena voi non essendocene una aperta immagino....
wait 5
say Bene, l'arena partira' tra circa 5 minuti, se volete farla partire prima non avete che da dirmelo (&1Say inizio&0)
end
set arena on
global arena
wait 3
say Dovete pagare tanti soldi quanti il vostro livello per 10
wait 2
eval prezzo %actor.level% * 10
global prezzo
say Nel vostro caso fanno %prezzo% coins
unset prezzo
set timer 1
global timer
else
wait 3
say Le iscrizioni hanno gia' raggiunto il tetto massimo di 20
end
end
end
if %speech% == spalti
if %starter% >= 1 || %inizio% == on
set arena tribuna
global arena
wait 3
say Tribuna nord, est od ovest?
set timer 1
global timer
else
wait 2
say Mi spiace ma non c'e' nessuno nell'arena da guardare al momento
end
end
end
if %speech% == est || %speech% == ovest || %speech% == nord
if %arena% == tribuna
if %speech% == nord
if %actor.rank% < 8
set azz on
global azz
end
end
if %azz% != on
wait 2
say Bene, ora vi prego di darmi le 200 monete del biglietto
set arena spalti
global arena
set tribuna %speech%
global tribuna
else
wait 2
say Non potete entrare in quella tribuna, dovreste saperlo...bisogna essere un nobile o un funzionario
unset azz
end
end
end
if %domanda% == on
if %actor.name% == %nome%
if %speech% == si
wait 2
say Dovrete darmi altre 1000 monete allora
set domanda2 on
global domanda2
unset domanda
end
if %speech% == no
wait 2
emote scrolla le spalle
wait 3
say Va bene, buon divertimento
wait 2
emote ti conduce agli spalti %tribuna%
if %tribuna% == ovest
mtele %actor.name% 29511
elseif %tribuna% == est
mtele %actor.name% 29512
elseif %tribuna% == nord
mtele %actor.name% 29514
end
unset timer
unset tribuna
unset nome
unset domanda
unset arena
end
end
end
~
#12591
arena2~
0 m 100
~
if %domanda2% == on
if %amount% >= 1000
if %cambio2% == 0
set cambio2 31
global cambio2
else
eval cambio2 %cambio2% + 1
global cambio2
end
set %cambio2% %actor.name%
global %cambio2%
wait 3
say Gotevi lo spettacolo :)
wait 3
emote ti conduce agli spalti %tribuna%
if %tribuna% == ovest
mtele %actor.name% 29511
elseif %tribuna% == est
mtele %actor.name% 29512
elseif %tribuna% == nord
mtele %actor.name% 29514
end
unset timer
unset tribuna
unset nome
unset arena
unset domanda2
end
end
if %arena% == on
eval prezzo %actor.level% * 70
global prezzo
if %amount% >= %prezzo%
wait 4
say Bene, vi avvisero' quando sara' il momento di entrare e provvedero' quindi direttamente a portarvi nell'arena, andate pure intanto
eval cambia %cambia% + 1
global cambia
if %cambia% == 1
set starter 1
global starter
end
set %cambia% %actor.name%
global %cambia%
eval dentro %dentro% + 1
global dentro
unset prezzo
unset arena
unset timer
else
wait 3
say questi soldi non bastano
wait 2
give %amount% coins %actor.name%
end
end
if %arena% == spalti
if %domanda2% != on
wait 2
if %amount% >= 200
set nome %actor.name%
global nome
if %inizio% == on
say Presto...e' gia' iniziato.....buon divertimento!
wait 2
emote ti conduce agli spalti %tribuna%
if %tribuna% == ovest
mtele %actor.name% 29511
elseif %tribuna% == est
mtele %actor.name% 29512
elseif %tribuna% == nord
mtele %actor.name% 29514
end
unset tribuna
unset timer
else
if %cambio2% < 35
say Volete poter godere della cronaca degli eventi dal campo?
set domanda on
global domanda
else
say Buona visione
wait 2
emote ti conduce agli spalti %tribuna%
if %tribuna% == ovest
mtele %actor.name% 29511
elseif %tribuna% == est
mtele %actor.name% 29512
elseif %tribuna% == nord
mtele %actor.name% 29514
end
unset timer
end
end
else
wait 2
give %amount% coins %actor.name%
wait 3
say non bastano
end
end
end
~
#12592
arena3~
0 ab 100
~
if %timer% >= 1
eval timer %timer% + 1
global timer
end
if %timer% >= 10
unset timer
unset arena
unset nome
unset domanda
unset domanda2
unset tribuna
emote cerca la persona con cui stava parlando
wait 3
emote scrolla le spalle
wait 2
emote mostra di essere nuovamente disponibile
end
if %starter% >= 1
eval starter %starter% + 1
global starter
end
if %starter% == 19
te %1% Verrete trasferito in arena tra circa un minuto
te %2% Verrete trasferito in arena tra circa un minuto
te %3% Verrete trasferito in arena tra circa un minuto
te %4% Verrete trasferito in arena tra circa un minuto
te %5% Verrete trasferito in arena tra circa un minuto
te %6% Verrete trasferito in arena tra circa un minuto
te %7% Verrete trasferito in arena tra circa un minuto
te %8% Verrete trasferito in arena tra circa un minuto
te %9% Verrete trasferito in arena tra circa un minuto
te %10% Verrete trasferito in arena tra circa un minuto
te %11% Verrete trasferito in arena tra circa un minuto
te %12% Verrete trasferito in arena tra circa un minuto
te %13% Verrete trasferito in arena tra circa un minuto
te %14% Verrete trasferito in arena tra circa un minuto
te %15% Verrete trasferito in arena tra circa un minuto
te %16% Verrete trasferito in arena tra circa un minuto
te %17% Verrete trasferito in arena tra circa un minuto
te %18% Verrete trasferito in arena tra circa un minuto
te %19% Verrete trasferito in arena tra circa un minuto
te %20% Verrete trasferito in arena tra circa un minuto
end
if %starter% >= 24
if %cambia% == 1
unset starter
unset 1
set dentro 0
global dentro
wait 3
emote si guarda intorno perplesso
wait 2
say %1% forse voleva solo fare un giro turistico solitario per l'arena...peccato niente da fare
wait 5
else
set inizio on
global inizio
wait 5
say Che i combattimenti abbiano inizio, le iscrizioni sono chiuse!
wait 2
say Combatteranno %1% %2% %3% %4% %5% %6% %7% %8% %9% %10% %11% %12% %13% %14% %15% %16% %17% %18% %19% %20%
unset starter
set partenza %self.room%
global partenza
mgoto 29517
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
mload mob 12594
if %31% != 0
say %31% say
else
say 1 say
end
if %32% != 0
say %32% say
else
say 1 say
end
if %33% != 0
say %33% say
else
say 1 say
end
if %34% != 0
say %34% say
else
say 1 say
end
if %35% != 0
say %35% say
else
say 1 say
end
mteleport %1% 29517
mforce %1% emote entra in arena
mteleport %2% 29517
mforce %2% emote entra in arena
if %cambia% > 2
mteleport %3% 29517
mforce %3% emote entra in arena
end
if %cambia% > 3
mteleport %4% 29517
mforce %4% emote entra in arena
end
if %cambia% > 4
mteleport %5% 29517
mforce %5% emote entra in arena
end
if %cambia% > 5
mteleport %6% 29517
mforce %6% emote entra in arena
end
if %cambia% > 6
mteleport %7% 29517
mforce %7% emote entra in arena
end
if %cambia% > 7
mteleport %8% 29517
mforce %8% emote entra in arena
end
if %cambia% > 8
mteleport %9% 29517
mforce %9% emote entra in arena
end
if %cambia% > 9
mteleport %10% 29517
mforce %10% emote entra in arena
end
if %cambia% > 10
mteleport %11% 29517
mforce %11% emote entra in arena
end
if %cambia% > 11
mteleport %12% 29517
mforce %12% emote entra in arena
end
if %cambia% > 12
mteleport %13% 29517
mforce %13% emote entra in arena
end
if %cambia% > 13
mteleport %14% 29517
mforce %14% emote entra in arena
end
if %cambia% > 14
mteleport %15% 29517
mforce %15% emote entra in arena
end
if %cambia% > 15
mteleport %16% 29517
mforce %16% emote entra in arena
end
if %cambia% > 16
mteleport %17% 29517
mforce %17% emote entra in arena
end
if %cambia% > 17
mteleport %18% 29517
mforce %18% emote entra in arena
end
if %cambia% > 18
mteleport %19% 29517
mforce %19% emote entra in arena
end
if %cambia% > 19
mteleport %20% 29517
mforce %20% emote entra in arena
end
mpurge
mgoto %partenza%
unset partenza
wait 5
emote ritorna raggiante
wait 5
say Che lo scontro abbia inizio!
end
end
eval reset %reset% + 1
global reset
~
#12593
giudice1~
0 d 1
say uscita~
if %actor.vnum% == 12595
eval cambio %cambio% + 1
global cambio
set %cambio% %speech%
global %cambio%
end
if %actor.name% == %target%
if %speech% == uscita
mat %1% &7&B %actor.name% ha lasciato l'arena &0
mat %2% &7&B %actor.name% ha lasciato l'arena &0
mat %3% &7&B %actor.name% ha lasciato l'arena &0
mat %4% &7&B %actor.name% ha lasciato l'arena &0
mat %5% &7&B %actor.name% ha lasciato l'arena &0
mgoto 29504
say %actor.name% e' morto
mtele %actor.name% 29504
mload obj 12595
give arena %actor.name%
mforce %actor.name% quaff arena
end
end
~
#12594
giudice2~
0 e 0
entra in arena~
if %target% == 0
foll %actor.name%
set target %actor.name%
global target
mforce %actor.name% nord
end
~
#12595
giudice3~
0 e 0
morto~
wait 5
if %actor.name% == %target%
mat %1% &7&Bsay %actor.name% e' stato ucciso &0!
mat %2% &7&Bsay %actor.name% e' stato ucciso &0!
mat %3% &7&Bsay %actor.name% e' stato ucciso &0!
mat %4% &7&Bsay %actor.name% e' stato ucciso &0!
mat %5% &7&Bsay %actor.name% e' stato ucciso &0!
mgoto 29504
say %actor.name% e' morto
mtele %actor.name% 29504
mload obj 12595
give arena %actor.name%
mforce %actor.name% quaff arena
end
~
#12596
giudice4~
0 e 1
poir judicandus~
if %actor.name% == %target%
mat %1% &7&x %actor.name% cerca di curarsi con delle spells curative&0
end
~
#12597
giudice4~
0 e 1
gpuorm abrazak vabratgtub ghabogtub yufogpuor gibqheabral vrubt yfaw wuffaf~
if %actor.name% == %target%
mat %1% &7&x %actor.name% cerca di corazzarsi con i suoi incantesimi&0
end
~
#12598
giudice5~
0 e 1
noselacri iqum qpurr mosailla qpabraw sabrairuko diesilla gsfil unsoftl yriwo kariq wubeho~
if %actor.name% == %target%
mat %1% &1&x %actor.name% colpisce %victim.name% con uno spell offensivo!&0
end
~
#12599
giudice6~
0 e 1
panico fugge~
if %actor.name% == %target%
mat %1% &7 %actor.name% si da' rapidamente alla fuga &0
mgoto %actor.name%
wait 5
mgoto %actor.name%
eval flee %flee% + 1
global flee
if %flee% == 4
wait 1 s
mat %1% &6%actor.name% pare proprio che sia in un brutto momento a vedere quanto scappa&0
mat %2% &6%actor.name% pare proprio che sia in un brutto momento a vedere quanto scappa&0
mat %3% &6%actor.name% pare proprio che sia in un brutto momento a vedere quanto scappa&0
mat %4% &6%actor.name% pare proprio che sia in un brutto momento a vedere quanto scappa&0
mat %5% &6%actor.name% pare proprio che sia in un brutto momento a vedere quanto scappa&0
end
if %flee% == 7
wait 1 s
mat %1% &6 Sta diventando una vera abitudine darsi alla fuga per %actor.name%!&0
mat %2% &6 Sta diventando una vera abitudine darsi alla fuga per %actor.name%!&0
mat %3% &6 Sta diventando una vera abitudine darsi alla fuga per %actor.name%!&0
mat %4% &6 Sta diventando una vera abitudine darsi alla fuga per %actor.name%!&0
mat %5% &6 Sta diventando una vera abitudine darsi alla fuga per %actor.name%!&0
end
if %flee% == 10
wait 1 s
mat %1% &6 Forse bisognerebbe dire ad %actor.name% che questa non e' la gara dei cento metri....come sembrerebbe vedendolo scappare ad ogni scontro&0
mat %2% &6 Forse bisognerebbe dire ad %actor.name% che questa non e' la gara dei cento metri....come sembrerebbe vedendolo scappare ad ogni scontro&0
mat %3% &6 Forse bisognerebbe dire ad %actor.name% che questa non e' la gara dei cento metri....come sembrerebbe vedendolo scappare ad ogni scontro&0
mat %4% &6 Forse bisognerebbe dire ad %actor.name% che questa non e' la gara dei cento metri....come sembrerebbe vedendolo scappare ad ogni scontro&0
mat %5% &6 Forse bisognerebbe dire ad %actor.name% che questa non e' la gara dei cento metri....come sembrerebbe vedendolo scappare ad ogni scontro&0
end
end
~
#12600
giudice6~
0 e 1
urta infilza trapassa colpisce pungola sferza azzanna randella frantuma lacera malmena pesta frusta pugnala~
if %actor.name% == %target%
if %victim.name% == %vittima%
eval volte %volte% + 1
global volte
if %volte% == 7
unset volte
eval scontro %scontro% + 1
global scontro
if %scontro% == 1
mat %1% &1 %actor.name% sta continuando a menare fendenti notevoli contro %vittima%&0
mat %2% &1 %actor.name% sta continuando a menare fendenti notevoli contro %vittima%&0
mat %3% &1 %actor.name% sta continuando a menare fendenti notevoli contro %vittima%&0
mat %4% &1 %actor.name% sta continuando a menare fendenti notevoli contro %vittima%&0
mat %5% &1 %actor.name% sta continuando a menare fendenti notevoli contro %vittima%&0
end
if %scontro% == 2
mat %1% &1 %vittima% continua ad incassare i colpi di %actor.name% &0
mat %2% &1 %vittima% continua ad incassare i colpi di %actor.name% &0
mat %3% &1 %vittima% continua ad incassare i colpi di %actor.name% &0
mat %4% &1 %vittima% continua ad incassare i colpi di %actor.name% &0
mat %5% &1 %vittima% continua ad incassare i colpi di %actor.name% &0
end
if %scontro% == 3
mat %1% &1 %actor.name% ormai non ha lasciato neanche una parte di %vittima% priva di ferite&1
mat %2% &1 %actor.name% ormai non ha lasciato neanche una parte di %vittima% priva di ferite&1
mat %3% &1 %actor.name% ormai non ha lasciato neanche una parte di %vittima% priva di ferite&1
mat %4% &1 %actor.name% ormai non ha lasciato neanche una parte di %vittima% priva di ferite&1
mat %5% &1 %actor.name% ormai non ha lasciato neanche una parte di %vittima% priva di ferite&1
end
if %scontro% > 3
mat %1% &1 %actor.name% sta combattendo contro un enorme ammasso rossastro! ....che sia ancora %vittima% ?&1
mat %2% &1 %actor.name% sta combattendo contro un enorme ammasso rossastro! ....che sia ancora %vittima% ?&1
mat %3% &1 %actor.name% sta combattendo contro un enorme ammasso rossastro! ....che sia ancora %vittima% ?&1
mat %4% &1 %actor.name% sta combattendo contro un enorme ammasso rossastro! ....che sia ancora %vittima% ?&1
mat %5% &1 %actor.name% sta combattendo contro un enorme ammasso rossastro! ....che sia ancora %vittima% ?&1
end
end
end
end
if %victim.name% != %vittima%
if %victim.name% != 0
unset scontro
mat %1% &1 %actor.name% ha attaccato %victim.name%!&0
mat %2% &1 %actor.name% ha attaccato %victim.name%!&0
mat %3% &1 %actor.name% ha attaccato %victim.name%!&0
mat %4% &1 %actor.name% ha attaccato %victim.name%!&0
mat %5% &1 %actor.name% ha attaccato %victim.name%!&0
set vittima %victim.name%
global vittima
unset scontro
wait 3 s
end
end
~
#12601
giudice7~
0 e 1
schiena~
if %actor.name% == %target%
mat %1% &1&x %actor.name% pianta un coltello nella schiena di %victim.name%!&0
mat %2% &1&x %actor.name% pianta un coltello nella schiena di %victim.name%!&0
mat %3% &1&x %actor.name% pianta un coltello nella schiena di %victim.name%!&0
mat %4% &1&x %actor.name% pianta un coltello nella schiena di %victim.name%!&0
mat %5% &1&x %actor.name% pianta un coltello nella schiena di %victim.name%!&0
eval schiena %schiena% + 1
global schiena
if %schiena% == 3
wait 1 s
mat %1% &6La strategia di %actor.name% sta dando i suoi effetti, sara' la 2-3 volta che usa il backstab&0
mat %2% &6La strategia di %actor.name% sta dando i suoi effetti, sara' la 2-3 volta che usa il backstab&0
mat %3% &6La strategia di %actor.name% sta dando i suoi effetti, sara' la 2-3 volta che usa il backstab&0
mat %4% &6La strategia di %actor.name% sta dando i suoi effetti, sara' la 2-3 volta che usa il backstab&0
mat %5% &6La strategia di %actor.name% sta dando i suoi effetti, sara' la 2-3 volta che usa il backstab&0
end
if %schiena% == 5
wait 1 s
mat %1% &6E' incredibile cosa %actor.name% riesca a fare con quell'arma alle spalle dei suoi nemici!&0
mat %2% &6E' incredibile cosa %actor.name% riesca a fare con quell'arma alle spalle dei suoi nemici!&0
mat %3% &6E' incredibile cosa %actor.name% riesca a fare con quell'arma alle spalle dei suoi nemici!&0
mat %4% &6E' incredibile cosa %actor.name% riesca a fare con quell'arma alle spalle dei suoi nemici!&0
mat %5% &6E' incredibile cosa %actor.name% riesca a fare con quell'arma alle spalle dei suoi nemici!&0
end
if %schiena% == 7
wait 1 s
mat %1% &6 %actor.name% non e' decisamente un tipo a cui girare le spalle se si tiene alla vita!&0
mat %2% &6 %actor.name% non e' decisamente un tipo a cui girare le spalle se si tiene alla vita!&0
mat %3% &6 %actor.name% non e' decisamente un tipo a cui girare le spalle se si tiene alla vita!&0
mat %4% &6 %actor.name% non e' decisamente un tipo a cui girare le spalle se si tiene alla vita!&0
mat %5% &6 %actor.name% non e' decisamente un tipo a cui girare le spalle se si tiene alla vita!&0
end
end
~
#12602
giudice8~
0 e 1
calcio stivali pulisce petto pieno~
if %actor.name% == %target%
mat %1% &1&x %actor.name% sferra un potente calcio contro %victim.name%&0
mat %2% &1&x %actor.name% sferra un potente calcio contro %victim.name%&0
mat %3% &1&x %actor.name% sferra un potente calcio contro %victim.name%&0
mat %4% &1&x %actor.name% sferra un potente calcio contro %victim.name%&0
mat %5% &1&x %actor.name% sferra un potente calcio contro %victim.name%&0
eval calcio %calcio% + 1
global calcio
if %calcio% == 3
wait 1 s
mat %1% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %2% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %3% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %4% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %5% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
end
if %calcio% == 5
wait 1 s
mat %1% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %2% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %3% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %4% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %5% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
end
if %calcio% == 8
wait 1 s
mat %1% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %2% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %3% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %4% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
mat %5% &6I calci di %actor.name% continuano a centrare i suoi avversari, buona tecnica &0
end
end
~
#12603
giudice9~
0 e 1
poderosa spallata~
if %actor.name% == %target%
mat %1% &1&x %actor.name% manda a terra %victim.name%&0
mat %2% &1&x %actor.name% manda a terra %victim.name%&0
mat %3% &1&x %actor.name% manda a terra %victim.name%&0
mat %4% &1&x %actor.name% manda a terra %victim.name%&0
mat %5% &1&x %actor.name% manda a terra %victim.name%&0
eval bash %bash% + 1
global bash
if %bash% == 4
wait 1 s
mat %1% &6Il bash di %actor.name% si sta rivelando una buona arma non c'e' che dire &0
mat %2% &6Il bash di %actor.name% si sta rivelando una buona arma non c'e' che dire &0
mat %3% &6Il bash di %actor.name% si sta rivelando una buona arma non c'e' che dire &0
mat %4% &6Il bash di %actor.name% si sta rivelando una buona arma non c'e' che dire &0
mat %5% &6Il bash di %actor.name% si sta rivelando una buona arma non c'e' che dire &0
end
if %bash% == 7
wait 1 s
mat %1% say &6 Fossi in un mago girerei alla larga da %actor.name%, i suoi bash sono micidiali!&0
mat %2% say &6 Fossi in un mago girerei alla larga da %actor.name%, i suoi bash sono micidiali!&0
mat %3% say &6 Fossi in un mago girerei alla larga da %actor.name%, i suoi bash sono micidiali!&0
mat %4% say &6 Fossi in un mago girerei alla larga da %actor.name%, i suoi bash sono micidiali!&0
mat %5% say &6 Fossi in un mago girerei alla larga da %actor.name%, i suoi bash sono micidiali!&0
end
if %bash% == 9
wait 1 s
mat %1% say &6 Sembra che gli avversari di %actor.name% oramai si buttino a terra da soli pur di evitare il suo bash!&0
mat %2% say &6 Sembra che gli avversari di %actor.name% oramai si buttino a terra da soli pur di evitare il suo bash!&0
mat %3% say &6 Sembra che gli avversari di %actor.name% oramai si buttino a terra da soli pur di evitare il suo bash!&0
mat %4% say &6 Sembra che gli avversari di %actor.name% oramai si buttino a terra da soli pur di evitare il suo bash!&0
mat %5% say &6 Sembra che gli avversari di %actor.name% oramai si buttino a terra da soli pur di evitare il suo bash!&0
end
end
~
#12604
giudici~
0 ab 100
~
eval timer %timer% + 1
global timer
if %timer% > 100
mgoto 2
mpurge
end
~
#12605
Esercito (by Phantom)~
0 c 1
plotone~
if %formazione% == colonnaincorso
  %send% %actor% Esecuzione ordine precende in corso.
  halt
end
set chek 0
  if !(%fronte%)
    set fronte nord
    global fronte
  end
  
  if %arg% == dietrofront
    if %fronte% == sud
      set ex nord
    elseif %fronte% == nord
      set ex sud
    elseif %fronte% == est
      set ex ovest
    elseif %fronte% == ovest
      set ex est
    else
      set ex sud
    end
    set arg fronte %ex%
  end 
  
  if %arg% /= fronte
    if %arg% /= fronte nord
      set ex %fronte%
      set fronte nord
    elseif %arg% /= fronte sud
      set ex %fronte%
      set fronte sud
    elseif %arg% /= fronte est
      set ex %fronte%
      set fronte est
    elseif %arg% /= fronte ovest
      set ex %fronte%
      set fronte ovest
    else
      %send% %actor% Il plotone e' schierato verso %fronte%
      halt
    end
    global fronte
    
    if %ex% == %fronte%
      if %marcia% != fermi
        %send% %actor% Il plotone sta gia marciando verso %fronte%.
      else
        %send% %actor% Il plotone e' gia' schierato verso %fronte%.
        wait 1 s
        %send% %actor% I tuoi uomini ti guardano perplessi.
        halt
      end
    end
    
    if (%ex% == nord && %fronte% == est) || (%ex% == est && %fronte% == sud)
      set fianco Fianco Destr
    elseif (%ex% == sud && %fronte% == ovest) || (%ex% == ovest && %fronte% == nord)
      set fianco Fianco Destr
    elseif (%ex% == sud && %fronte% == nord) || (%ex% == ovest && %fronte% == est)
      set fianco Dietrofront
    elseif (%ex% == nord && %fronte% == sud) || (%ex% == est && %fronte% == ovest)
      set fianco Dietrofront
    else
     set fianco Fianco Sinistr
    end
    
    set chek 1
  end
  
  if %arg% == marcia
    %force% %actor% say %self.name% in Marcia.
    set marcia normale
    global marcia
    halt  
  end 
  if %arg% == alt
    %force% %actor% say %self.name% ALT!!!
    set marcia fermi
    global marcia
    halt  
  end 
  if %arg% /= formazione
    if %arg% /= colonna
      if %formazione% != colonnaincorso && %formazione% != colonna
        %echo% Le truppe si schierano in colonna
        set formazione colonnaincorso
        global formazione
      else
        %send% %actor% Le tue truppe sono gia' allineate in colonna.
      end
    else
      %send% %actor% Che tipo di formazione vuoi assegnare alle truppe?
    end
    halt  
  end
  
  if %arg% == boh
    set chek 1  
  end
  
  if %chek% == 0
    %force% %actor% cls
    %send% %actor% 
    %send% %actor% #############################################
    %send% %actor% # Il Plotone e' schierato verso %fronte%
    %send% %actor% #                                           #
    %send% %actor% # Comandi Attivi:                           #
    %send% %actor% #                                           #
    %send% %actor% # PLOTONE Fronte Punto Cardinale            #
    %send% %actor% # PLOTONE DietroFront                       #
    %send% %actor% # PLOTONE Alt                               #
    %send% %actor% # PLOTONE Marcia                            #
    %send% %actor% # PLOTONE Formazione Tipo di formazione:    #
    %send% %actor% #                           - Colonna -     #
    %send% %actor% #
  elseif %marcia% != fermi
    %force% %actor.name% say %self.name% %fianco%.
    wait 1 s
    %echo% %self.name% cambia la direzione di marcia, ora si dirige verso %fronte%.
  else
    %force% %actor.name% say %self.name% ATTENTI.
    wait 1 s
    %echo% %self.name% scatta sugli attenti.
    wait 2 s
    %force% %actor.name% say %self.name% %fianco%.
    wait 1 s
    %echo% %self.name% con una perfetta sincronia di movimenti si schiera verso %fronte%.
    wait 2 s
    %force% %actor.name% say %self.name% RIPOSO.
    wait 1 s
    %echo% %self.name% assume la posizione di riposo.    
  end
end
  
~
#12606
Esercito marcia (by Phantom)~
0 ab 100
~
   if %formazione% == colonnaincorso
     if !(%fronte%)
       set fronte nord
       global fronte
     end
     
     if %fronte% == sud
       set ex nord
     elseif %fronte% == nord
       set ex sud
     elseif %fronte% == est
       set ex ovest
     elseif %fronte% == ovest
       set ex est
     else
       %echo% Fronte Non settato avvertite Phantom
     end
          
     set phan 1
     while %phan% < 5
       %load% mob 12602
       wait 1 s
       %force% fanti1 foll fanteria
       group all
       wait 2 s
       gtell vai %ex%
       eval phan %phan% + 1
     done
     set formazione colonna
     global formazione
     gtell viaggiamo in colonna
     halt
   end
   
   if %marcia% == fermi
     halt
   elseif %marcia% == normale
     wait 5 s
     %fronte%
     gtell marcia %fronte%
   end
   
~
#12607
Group Act Eserciti (by Phantom)~
0 e 0
dice al gruppo,~
 set testo %arg%
 global testo
 
 if %actor.vnum% == -1
   halt
 end
 
 if %arg% /= vai nord
   nord
   set direx sud
 elseif %arg% /= vai sud
   sud
   set direx nord
 elseif %arg% /= vai est
   est
   set direx ovest
 elseif %arg% /= vai ovest
   ovest
   set direx est
 elseif %arg% /= marcia nord
   set direx nord
 elseif %arg% /= marcia sud
   set direx sud
 elseif %arg% /= marcia est
   set direx est
 elseif %arg% /= marcia ovest
   set direx ovest
 elseif %arg% /= viaggiamo in colonna
   %purge% fanti1
   set swap %m1%
   set m1 %m4%
   set m4 %swap%
   set swap %m2%
   set m2 %m3%
   set m3 %swap%
   set formaxione colonna
   global formaxione
   set lup 1
   while %lup% <= 4
     global m%lup%
     eval lup %lup% + 1
   done
   halt
 else
   halt
 end
 if %formaxione% != colonna
   set m0 %m1%
   set m1 %m2%
   set m2 %m3%
   set m3 %m4%
   set m4 %direx%
 elseif %m4% != nil
   set m0 %m1%
   set m1 %m2%
   set m2 %m3%
   set m3 %m4%
   set m4 %direx%
   %m0%
 elseif %m3% != nil
   set m0 %m1%
   set m1 %m2%
   set m2 %m3%
   set m3 %direx%
   set m4 nil
   %m0%
 elseif %m2% != nil
   set m0 %m1%
   set m1 %m2%
   set m2 %direx%
   set m3 nil
   set m4 nil
   %m0%
 else
   set m0 %m1%
   set m1 %direx%
   set m2 nil
   set m3 nil
   set m4 nil
   %m0%
 end
    
 set lup 0
 while %lup% <= 4
   global m%lup%
   eval lup %lup% + 1
 done
  
~
#12608
Default setting Eserciti (by Phantom)~
0 n 100
~
 set m0 nil 
 set m1 nil
 set m2 nil
 set m3 nil
 set m4 nil
    
 set lup 0
 while %lup% <= 4
   global m%lup%
   eval lup %lup% + 1
 done
  
~
#12609
esercito5~
0 d 1
sentinelle stato distanza mob mab mib soldati reggente funzionario scout allerta eccomi monarca generale comandante smantella accampa bonk attacco~
if %actor.level% > 79
if %speech% /= sentinelle
eval %speech% - 20
global sentinelle
end
if %speech% /= abilita
set %speech%
global abilita
end
if %speech% /= soldati
eval %speech% - 20
global soldati
set soldatimax %soldati%
global soldatimax
nod
end
if %speech% /= mib
set %speech%
global mib
end
if %speech% /= mob
set %speech%
global mob
end
if %speech% /= mab
set %speech%
global mab
end
end
if %actor.name% == %monarca% || %actor.name% == %reggente%
if %speech% == reggente && %actor.name% == %monarca%
wait 5
say Che il nuovo Reggente si faccia avanti (&1Say Eccomi&0)
set carica reggente
global carica
end
if %speech% == generale
wait 5
say Che il mio nuovo generale si faccia avanti
set generale on
global generale
end
end
if %actor.name% == %monarca% || %actor.name% == %generale% || %actor.name% == %reggente% || %actor.name% == %comandante%
if %speech% == funzionario
wait 5
say Che il nuovo funzionario si faccia avanti (&1Say Eccomi&0)
set carica funzionario
global carica
end
end
if %actor.name% == %funzionario% || %actor.name% == %monarca% || %actor.name% == %generale% || %actor.name% == %reggente%
if %speech% == scout
wait 5
say Che il nuovo Scout si faccia avanti (&1Say Eccomi&0)
set carica gestore
global carica
end
end
if %monarca% == 0
if %speech% == monarca
wait 3
nod
set monarca %actor.name%
global monarca
set stato pacifico
global stato
set cibo 700
global cibo
set abilita 0
global abilita
set morale 500
global morale
set corazza 0
global corazza
set soldati 40
global soldati
set soldatimax 40
global soldatimax
set freschezza 1000
global freschezza
set caserma %self.room%
global caserma
set armature 0
global armature
set gambali 0
global gambali
set parabraccia 0
global parabraccia
set guanti 0
global guanti
set elmi 0
global elmi
set armi 0
global armi
set scudi 0
global scudi
set azione allerta
global azione
eval pf %soldati% * 10
global pf
set attenti on
global attenti
set sonno 1
global sonno
set gestore nessuno
global gestore
set funzionario nessuno
global funzionario
eval sentinelle %soldatimax% / 2
global sentinelle
set ampiezza 1
global ampiezza
end
end
if %speech% == eccomi
if %carica% != 0
set %carica% %actor.name%
global %carica%
wait 5
emote riconosce ora %actor.name% come %carica%
unset carica
end
if %generale% == on
set generale %actor.name%
global generale
wait 5
say Generale %actor.name%, ai vostri ordini
end
if %comandante% == on
set comandante %actor.name%
global comandante
wait 5
say Comandante %actor.name%, ai vostri ordini
end
end
if %actor.name% == %monarca% || %actor.name% == %generale% || %actor.name% == %reggente%
if %speech% == comandante
wait 5
say Che il mio nuovo comandante si faccia avanti
set comandante on
global comandante
end
end
if %actor.name% == %comandante% || %actor.name% == %generale% || %actor.name% == %monarca% || %actor.name% == %reggente%
if %speech% == stato difensivo && %stato% != difensivo
if %pagare% == 0
wait 5
nod
set stato difensivo
global stato
set fronte niente
global fronte
else
wait 5
say Dovete prima finire di pagarci gli stipendi, Sir %actor.name%
end
end
if %speech% == stato pacifico && %stato% != pacifico
if %pagare% == 0
wait 5
nod
set stato pacifico
global stato
else
wait 5
say Dovete prima finire di pagarci gli stipendi, Sir %actor.name%
end
end
if %stato% == difensivo && %self.room% > 1000000 
if %ampiezza% == on && %speech% /= distanza
wait 5
set ampiezza %ampiezza%
if %speech% == distanza minima
set ampiezza 1
elseif %speech% == distanza bassa
set ampiezza 2
elseif %speech% == distanza media
set ampiezza 3
elseif %speech% == distanza alta
set ampiezza 4
elseif %speech% == distanza massima
set ampiezza 5
end
global ampiezza
wait 5
if %speech% /= cambia
else
say La distanza attuale del fronte dall'esercito' e' ora di %ampiezza% leghe
end
end
if %speech% == cambia distanza fronte && %ampiezza% != on
set ampiezza on
global ampiezza
wait 5
say A quale distanza dovra' essere disposto il fronte dell'esercito? (&1minima, bassa, media, alta, massima)
end
end
if %speech% == allerta
if %azione% == riposo
if %leghe% <= 0
wait 5
nod
wait 1 s
emote riordina i ranghi ora pronto per ricevere ordini
set azione allerta
global azione
end
elseif %azione% == arruola
wait 5
emote e' ora pronto a ricevere nuovi ordini
set azione allerta
global azione
end
end
if %speech% == smantella
if %azione% == accampamento
wait 5
emote inizia come ordinato a smantellare il campo
wait 1 s
set azione smantellando
global azione
eval turni %soldati% / 3
global turni
emote necessitera' di %turni% minuti per smantellare l'accampamento
unset dormendo
unset dormita
elseif %azione% == accampando
wait 5
emote guarda stizzito %actor.name%
wait 5
emote ricompone i ranghi dopo aver ultimato di rimmettere via i tendaggi
set azione allerta
global azione
unset turni
end
end
if %speech% == accampa
if %azione% == allerta
wait 5
emote sospira rilassato
wait 5
emote inizia a preparare l'accampamento per riposare
wait 5
eval turni %soldati% / 2
global turni
emote necessitera' di circa %turni% minuti per completare l'operazione
set azione accampando
global azione
elseif %azione% == marcia
wait 5
emote puo' solo riposare durante le marce
wait 5
emote per accamparsi avrebbe bisogno di molto tempo e di vedere il suo ordine di marciare annullato
end
end
if %speech% /= bonk
wait 2
%speech%
end
end
if %speech% == All'attacco
if %nemico% == 0
if %actor.vnum% > 100
wait 6
emote riprende posizione per affrontare %actor.name%
set nemico %actor.name%
global nemico
set scontro %self.room%
global scontro
set azione2 %azione%
global azione2
unset delay
set azione guerra
global azione
set attaccante on
global attaccante
mgoto eserciziario
say 1 + %scontro%
end
end
end
~
#12610
esercito6~
0 e 0
ti da una botta in testa~
if %actor.name% != %self.name%
if %azione% != guerra
wait 3
if %actor.name% /= reggimento || %actor.name% /= esercito || %actor.name% /= distaccamento || %actor.name% /= gruppo || %actor.name% /= orda || %actor.name% /= armata
emote inizia una lotta furiosa contro %actor.name%
unset delay
set azione2 %azione%
global azione2
set azione guerra
global azione
set nemico %actor.name%
global nemico
set scontro %self.room%
global scontro
set attaccato on
global attaccato
mgoto eserciziario
say 1 + %scontro%
end
end
end
~
#12611
esercito7~
0 e 1
perso~
if %actor.name% == %nemico%
if %attaccante% == on
wait 3 s
end
mgoto %stanza%
wait 3
say eccomi
end
~
#12612
esercito8~
0 e 1
cessa~
if %actor.name% == %nemico%
unset stanza
unset scontro
unset nemico
wait 5
emote gioisce per la cruenta vittoria!
set azione %azione2%
global azione
unset azione2
end
~
#12620
MACRORISORSA1~
0 d 1
eccomi funzionario reggente gestore grandezza monarca tipo 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20~
%echo% Non attivo causa bug
~
#12621
MACRORISORSA2~
0 ab 100
~
mgoto 28150
if %azione% != 0
eval timer %timer% + 1
global timer
if %timer% > 13
emote ritorna alle sue mansioni in mancanza di ulteriori ordini
unset timer
unset quanti
unset prendendo
unset stock
unset azione
unset carica
end
end
if %vnum% != 0
eval repop %repop% + 1
global repop
if %repop% > %ritmo%
unset repop
eval portata %1b% + %2b% + %3b% + %4b% + %5b% + %6b% + %7b% + %8b% + %9b% + %10b%
global portata
eval portataMax %grandezza% * 20
global portataMax
if %portata% < %portatamax%
if %cambio% == 1
eval 1b %1b% + 1
global 1b
elseif %cambio% == 2
eval 2b %2b% + 1
global 2b
elseif %cambio% == 3
eval 3b %3b% + 1
global 3b
elseif %cambio% == 4
eval 4b %4b% + 1
global 4b
elseif %cambio% == 5
eval 5b %5b% + 1
global 5b
elseif %cambio% == 6
eval 6b %6b% + 1
global 6b
elseif %cambio% == 7
eval 7b %7b% + 1
global 7b
elseif %cambio% == 8
eval 8b %8b% + 1
global 8b
elseif %cambio% == 9
eval 9b %9b% + 1
global 9b
elseif %cambio% == 10
eval 10b %10b% + 1
global 10b
elseif %cambio% == 11
eval 11b %11b% + 1
global 11b
end
end
end
end
~
#12622
MACRORISORSA3~
0 e 0
ti guarda~
mload mob 12601
eval portata %1b% + %2b% + %3b% + %4b% + %5b% + %6b% + %7b% + %8b% + %9b% + %10b%
global portata
eval portataMax %grandezza% * 20
global portataMax
mecho vittima Riesci a pervenire ai seguenti dati sulla Risorsa:
mecho vittima Monarca:		%monarca%
mecho vittima Reggente:		%Reggente%
mecho vittima Funzionario:		%Funzionario%
mecho vittima Gestore:		%Gestore%
mecho vittima Grandezza		%grandezza%
mecho vittima Portata/Max		%portata% / %portatamax%
mecho vittima Tipologia		%tipo%
mecho vittima Produzione:		%prodotto%
mecho vittima STOCK PRESENTI:
mecho vittima 1:  %1%	%1b%
if %num% > 1
mecho vittima 2:  %2%	%2b%
end
if %num% > 2
mecho vittima 3:  %3%	%3b%
end
if %num% > 3
mecho vittima 4:  %4%	%4b%
end
if %num% > 4
mecho vittima 5:  %5%	%5b%
end
if %num% > 5
mecho vittima 6:  %6%	%6b%
end
if %num% > 6
mecho vittima 7:  %7%	%7b%
end
if %num% > 7
mecho vittima 8:  %8%	%8b%
end
if %num% > 8
mecho vittima 9:  %9%	%9b%
end
if %num% > 9
mecho vittima 10: %10%	%10b%
end
if %num% > 10
mecho vittima 11: %11%	%11b%
end
mforce vittima mgoto 2
~
#12623
MACRORISORSA4~
0 d 1
cambia preleva~
if %actor.name% == %gestore% || %actor.name% == %monarca% || %actor.name% == %funzionario% || %actor.name% == %reggente%
if %speech% == preleva
wait 3
set azione 2
global azione
emote aspetta di sapere che merce rilasciare
end
if %speech% == cambia produzione || %speech% == preleva
if %tipo% == argento || %tipo% == metalli
if %speech% == cambia produzione
wait 5
emote puo' essere indirizzata verso la produzione di Corazzamenti (1) o di armi (2)
set azione 1
global azione
end
else
if %tipo% == Oro
set 1a 12650
global 1a
set 1r 500
global 1r
end
if %tipo% /= argento || %tipo% /= metalli
if %tipo% /= argento
set 1a 12651
global 1a
set 1r 500
global 1r
elseif %tipo% /= metalli
set 1a 12658
global 1a
set 1r 220
global 1r
end
if %tipo% /= corazze
set 2a 12652
global 2a
set 2r 220
global 2r
set 3a 12653
global 3a
set 3r 160
global 3r
set 4a 12654
global 4a
set 4r 150
global 4r
set 6a 12656
global 6a
set 6r 150
global 6r
set 5a 12655
global 5a
set 5r 150
global 5r
set 7a 12657
global 7a
set 7r 150
global 7r
set 8a 12659
global 8a
set 8r 135
global 8r
set 9a 12643
global 9a
set 9r 140
global 9r
end
if %tipo% /= armi
set 2a 12661
global 2a
set 2r 150
global 2r
set 3a 12662
global 3a
set 3r 150
global 3r
set 4a 12663
global 4a
set 4r 150
global 4r
set 5a 12664
global 5a
set 5r 120
global 5r
set 6a 12665
global 6a
set 6r 150
global 6r
end
elseif %tipo% == cibo
set 1a 12666
global 1a
set 1r 40
global 1r
set 2a 12667
global 2a
set 2r 45
global 2r
set 3a 12668
global 3a
set 3r 50
global 3r
set 4a 12669
global 4a
set 4r 60
global 4r
set 5a 12670
global 5a
set 5r 60
global 5r
set 6a 12671
global 6a
set 6r 70
global 6r
set 7a 12672
global 7a
set 7r 65
global 7r
set 8a 12673
global 8a
set 8r 60
global 8r
set 9a 12674
global 9a
set 9r 65
global 9r
elseif %tipo% == pietre
set 1a 12675
global 1a
set 1r 130
global 1r
set 2a 12676
global 2a
set 2r 300
global 2r
elseif %tipo% == erbe
set 1a 12601
global 1a
set 1r 150
global 1r
set 2a 12602
global 2a
set 2r 150
global 2r
set 3a 12603
global 3a
set 3r 150
global 3r
set 4a 12604
global 4a
set 4r 150
global 4r
set 5a 12605
global 5a
set 5r 150
global 5r
set 6a 12606
global 6a
set 6r 150
global 6r
elseif %tipo% == preziose
set 1a 12677
global 1a
set 1r 350
global 1r
set 2a 12678
global 2a
set 2r 400
global 2r
set 3a 12679
global 3a
set 3r 400
global 3r
set 4a 12680
global 4a
set 4r 400
global 4r
set 5a 12681
global 5a
set 5r 250
global 5r
elseif %tipo% == selvaggina
set 1a 12682
global 1a
set 1r 140
global 1r
set 2a 12683
global 2a
set 2r 150
global 2r
set 3a 12684
global 3a
set 3r 170
global 3r
set 4a 12685
global 4a
set 4r 150
global 4r
set 5a 12686
global 5a
set 5r 150
global 5r
set 6a 12687
global 6a
set 6r 150
global 6r
set 7a 12688
global 7a
set 7r 160
global 7r
set 8a 12689
global 8a
set 8r 160
global 8r
set 9a 12690
global 9a
set 9r 80
global 9r
elseif %tipo% == legname
set 1a 12691
global 1a
set 1r 160
global 1r
set 2a 12692
global 2a
set 2r 100
global 2r
set 3a 12693
global 3a
set 3r 110
global 3r
set 4a 12694
global 4a
set 4r 100
global 4r
set 5a 12695
global 5a
set 5r 160
global 5r
set 6a 12696
global 6a
set 6r 150
global 6r
set 7a 12697
global 7a
set 7r 200
global 7r
set 8a 12645
global 8a
set 8r 110
global 8r
set 9a 12646
global 9a
set 9r 200
global 9r
set 10a 12649
global 10a
set 10r 220
global 10r
set 11a 12648
global 11a
set 11r 100
global 11r
end
wait 6
if %speech% == cambia produzione
emote aspetta di sapere Che numero in lista si intende far produrre
set azione 1
global azione
end
end
end
end
~
#12624
CONTROLLO MERCI 1~
0 d 1
controllore azzera monarca set cibo armi corazze oro argento pietra legname preziose selvaggina~
if %speech% == monarca
if %monarca% == 0
set monarca %actor.name%
global monarca
end
end
if %actor.name% == %monarca%
if %speech% /= controllore
set %speech%
global controllore
end
end
if %actor.name% == %monarca% || %actor.name% == %controllore%
if %speech% == azzera
wait 2
nod
wait 3
emote annulla tutti i conteggi
unset cambio
unset 1
unset 2
unset 3
unset 4
unset 5
unset 1b
unset 2b
unset 3b
unset 4b
unset 5b
end
end
if %speech% /= set
if %speech% /= pg
%speech%
global pg
elseif %speech% /= quanti
%speech%
global quanti
if %pg% != 0
if %pg% == %1%
eval 1b %1b% + %quanti%
if %1b% < 0
set 1b 0
end
global 1b
elseif %pg% == %2%
eval 2b %2b% + %quanti%
if %2b% < 0
set 2b 0
end
global 2b
elseif %pg% == %3%
eval 3b %3b% + %quanti%
if %3b% < 0
set 3b 0
end
global 3b
elseif %pg% == %4%
eval 4b %4b% + %quanti%
if %4b% < 0
set 4b 0
end
global 4b
elseif %pg% == %5%
eval 5b %5b% + %quanti%
if %5b% < 0
set 5b 0
end
global 5b
else
if %cambio% < 5
eval cambio %cambio% + 1
global cambio
set %cambio% %pg%
global %cambio%
eval %cambio%b %quanti%
global %cambio%b
end
end
end
unset quanti
unset pg
unset pass
end
end
~
#12625
CONTROLLO MERCI 2~
0 e 0
ti guarda~
mload mob 12601
mecho vittima Monarca:		%monarca%
mecho vittima Controllore:	%controllore%
mecho vittima Attuali merci in circolazione:
mecho vittima 1) %1%:	%1b%
mecho vittima 2) %2%:	%2b%
mecho vittima 3) %3%:	%3b%
mecho vittima 4) %4%:	%4b%
mecho vittima 5) %5%:	%5b%
mforce vittima mgoto 2
mat 2 mpurge
~
#12630
Eserciziario~
0 d 1
1 nemico~
wait 1
if %speech% /= 1
if %actor.name% != %self.name%
eval scontro %speech% - 1
global scontro
if %2154040% == %scontro%
say 1 + 2154040
elseif %2154041% == %scontro%
say 1 + 2154041
elseif %2154041% == %scontro%
say 1 + 2154042
elseif %2154041% == %scontro%
say 1 + 2154043
elseif %2154041% == %scontro%
say 1 + 2154044
elseif %2154041% == %scontro%
say 1 + 2154045
elseif %2154041% == %scontro%
say 1 + 2154046
elseif %2154041% == %scontro%
say 1 + 2154047
elseif %2154041% == %scontro%
say 1 + 2154048
elseif %2154041% == %scontro%
say 1 + 2154049
else
if %2154040% == 0
set stanza 2154040
global stanza
else
if %2154041% == 0
set stanza 2154041
global stanza
else
if %2154042% == 0
set stanza 2154042
global stanza
else
if %2154043% == 0
set stanza 2154043
global stanza
else
if %2154044% == 0
set stanza 2154044
global stanza
else
if %2154045% == 0
set stanza 2154045
global stanza
else
if %2154046% == 0
set stanza 2154046
global stanza
else
if %2154047% == 0
set stanza 2154047
global stanza
else
if %2154048% == 0
set stanza 2154048
global stanza
else
if %2154049% == 0
set stanza 2154049
global stanza
else
SAY MERDA!!!
end
end
end
end
end
end
end
end
end
end
set %stanza% %scontro%
global %stanza%
unset scontro
say 1 + %stanza%
end
end
end
~
#12631
Intermediario1~
0 d 1
1 eccomi~
wait 1
if %speech% == eccomi
say 1 + %numero%
end
if %speech% /= 1
eval numero %speech% - 1
global numero
end
~
#12632
CONTROLLO DENARO REGNI~
0 d 1
permessi eccomi funzionario monarca preleva~
if %speech% == monarca && %monarca% == 0
set monarca %actor.name%
global monarca
end
if %speech% == permessi
wait 2
say Oltre al Monarca %monarca% hanno accesso a questi magazzini:
say 1- %funzionario% (&1Funzionario primo&0)
say 2- %2%
say 3- %3%
say 4- %4%
say 5- %5%
end
if %actor.name% == %monarca%
if %speech% == funzionario
wait 5
say Che il nuovo funzionario si faccia avanti (&1Say Eccomi&0)
set carica funzionario
global carica
end
end
if %speech% == eccomi
if %carica% == funzionario
set funzionario %actor.name%
global funzionario
wait 2
unset carica
nod
else
set %cambio% %actor.name%
global %cambio%
unset cambio
end
end
if %actor.name% == %funzionario% || %actor.name% == %monarca%
if %speech% == cambia permessi
set cambio on
global cambio
wait 4
say Che numero? (2-5)
end
if %cambio% == on
if %speech% > 1 && %speech% < 6
set cambio %speech%
global cambio
wait 5
say Bene, che il nuovo privilegiato si faccia avanti (&1Say eccomi&0)
end
end
end
~
#12669
chiudi porte deira (zephy)~
2 g 100
~
wdoor 3056 north purge
wdoor 3056 north description Un pannello di legno ricoperto di terra per mimetizzarsi tra le rocce.
wdoor 3056 north flags abcde
wdoor 3056 north name pannello
wdoor 3056 north room 3045
wdoor 3056 north key 748922
wdoor 3045 south purge
wdoor 3045 south description Un pannello di legno ricoperto di terra per mimetizzarsi tra le rocce.
wdoor 3045 south flags abcde
wdoor 3045 south name pannello
wdoor 3045 south room 3056
wdoor 3045 south key 748922
wdoor 3045 up purge
wdoor 3045 up description Una botola incassata nel soffitto
wdoor 3045 up flags abce
wdoor 3045 up name botola
wdoor 3045 up room 3044
wdoor 3045 up key 65473
wdoor 3044 down purge
wdoor 3044 down description Una botola nascosta sotto un asse del pavimento.
wdoor 3044 down flags abce
wdoor 3044 down name botola
wdoor 3044 down room 3045
wdoor 3044 down key 65473
wdoor 3044 west purge
wdoor 3044 west description Un Pannello di legno rivestito di intonaco, perfettamente mimetizzato con la parete.
wdoor 3044 west flags abce
wdoor 3044 west name pannello
wdoor 3044 west room 3057
wdoor 3044 west key 3057
wdoor 3057 east purge
wdoor 3057 east description Un Pannello di legno rivestito di intonaco, perfettamente mimetizzato con la parete.
wdoor 3057 east flags abce
wdoor 3057 east name pannello
wdoor 3057 east room 3044
wdoor 3057 east key 3057
wdoor 3057 west purge
wdoor 3057 west description Una porta di legno
wdoor 3057 west flags abc
wdoor 3057 west name porta
wdoor 3057 west room 12669
wdoor 3057 west key 657
wdoor 12669 east purge
wdoor 12669 east description Una porta di legno
wdoor 12669 east flags abc
wdoor 12669 east name porta
wdoor 12669 east room 3057
wdoor 12669 east key 657
~
$~
