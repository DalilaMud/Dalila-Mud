#2000
Sacerdote Silue (1) (Sheep)~
0 d 1
prova~
if %actor.vnum% != -1
halt
end
if %actor.level% > 70
wait 2 s
inch %actor.name%
wait 2 s
say Mi spiace, potente %actor.name% ma non vi sono missioni qui per quelli come voi.
halt
end
if %missione% == 1
wait 2 s
say Mi spiace %actor.name%, ma non vi posso assegnare una nuova missione finche' %pg% non completera' la sua.
halt
end
if %mob% == 1
wait 2 s
say Spiacente %actor.name%, ma dovrete aspettare un po' di tempo prima che una nuova missione sia disponibile.
halt
end
set missione 1
set counter 0
global counter
global missione
set mob 1
global mob
set livello %actor.level%
global livello
set pg %actor.name%
global pg
wait 2 s
sor
wait 1 s
ann
wait 1 s
say Molto bene %pg%, Silue accetta di mettere alla prova la vostra fede in lei...
wait 2 s
if %livello% < 20
eval quest %random.3%
if %quest% == 1
say Esiste un ricco possidente terriero, di nome Sir Teinko.
wait 2 s
say Quest'uomo e' molto malvagio e la Dama vuole che sia fatta giustizia.
wait 2 s
say Portatemi il Globo Luminoso di Dama Silue, e la ricompensa sara' vostra, %pg%.
mat ttgls mpurge teinko
mat ttgls mload mob 11248
mat ttgls mforce teinko mload obj 542
mat ttgls mforce teinko get all
mat ttgls mforce teinko indo all
mat ttgls mforce teinko hold emblema
halt
elseif %quest% == 2
say Chioma di Demonio e' un pirata molto malvagio, adora torturare gli innocenti
wait 2 s
say La Dama vorrebbe che sia fatta giustizia.
wait 2 s
say Portatemi il Globo Luminoso di Dama Silue, e la ricompensa sara' vostra, %pg%.
mat fcdgl mpurge chioma
mat fcdgl mload mob 18000
mat fcdgl mforce chioma mload obj 542
mat fcdgl mforce chioma get all
mat fcdgl mforce chioma indo all
mat fcdgl mforce chioma hold chiave
else
say Un crudele Hobgoblin regna su un gruppo di immondi Coboldi, non lontano da Chyrellos.
wait 2 s
say La Dama vuole che il regno di questa immonda creatura abbia fine
wait 2 s
say Portatemi il Globo Luminoso di Dama Silue, e la ricompensa sara' vostra, %pg%.
mat tdgl mpurge djano
mat tdgl mload mob 18308
mat tdgl mforce djano mload obj 542
mat tdgl mforce djano get all
mat tdgl mforce djano indo all
mat tdgl mforce djano hold chiave
end
halt
elseif %livello% < 40
eval quest %random.3%
if %quest% == 1
say Esiste un tempio dedicato a Thalos ormai profanato dopo la sua distruzione...
wait 2 s
say Ora e' abitato da una creatura blasfema: un beholder!
wait 2 s
say Portatemi il Globo Luminoso di Dama Silue, e la ricompensa sara' vostra, %pg%.
mat smtb mpurge pbqgl
mat smtb mload mob 5200
mat smtb mforce pbqgl mload obj 542
mat smtb mforce pbqgl get all
mat smtb mforce pbqgl indo all
mat smtb mforce pbqgl hold flauto
elseif %quest% == 2
say Una volta in queste lande visse un grande e nobile guerriero: Krel
wait 2 s
say Purtroppo fu ucciso e maledetto da uno stregone che lo costringe a rinascere...
wait 2 s
say ...sotto forma di demone: liberatelo dal male!
wait 2 s
say Portatemi il Globo Luminoso di Dama Silue, e la ricompensa sara' vostra, %pg%.
mat lbkt mpurge krel
mat lbkt mload mob 2031
mat lbkt mforce krel mload obj 542
mat lbkt mforce krel get all
mat lbkt mforce krel indo all
else
say Nella citta' maledetta di Ghenna esiste una creatura immonda: la Gorgimera.
wait 2 s
say La sua esistenza e' un'offesa alla Dama.
wait 2 s
say Portatemi il Globo Luminoso di Dama Silue, e la ricompensa sara' vostra, %pg%.
mat fgggl mpurge gorgimera
mat fgggl mload mob 18808
mat fgggl mforce gorgimera mload obj 542
end
halt
else
eval quest %random.3%
if %quest% == 1
say Nei recessi piu' profondi della citta' Drow esiste una creatura immonda
wait 2 s
say Dama Silue non puo' permettere che questa creatura offenda il Creato
wait 2 s
say Portatemi il Globo Luminoso di Dama Silue, e la ricompensa sara' vostra, %pg%.
mat srdgl mpurge yochlol
mat srdgl mload mob 5111
mat srdgl mforce yochlol mload obj 542
mat srdgl mforce yochlol get chiave
mat srdgl mforce yochlol hold chiave
halt
elseif %quest% == 2
say Nel grande deserto di polvere, esiste una enorme piramide..
wait 2 s
say Nei suoi recessi giace Ramses, un'orrido e blasfemo non morto.
wait 2 s
say Portatemi il Globo Luminoso di Dama Silue, e la ricompensa sara' vostra, %pg%.
mat srpgl mpurge ramses
mat srpgl mload mob 5317
mat srpgl mforce ramses mload obj 542
mat srpgl mforce ramses get all
mat srpgl mforce ramses indo maschera
halt
else
say Nel blasfemo cimitero di Rasheen esiste uno spaventoso Dracolich
wait 2 s
say La sua esistenza offende Dama Silue...
wait 2 s
say Portatemi il Globo Luminoso di Dama Silue, e la ricompensa sara' vostra, %pg%.
mat sdrgl mpurge ddglp
mat sdrgl mload mob 18521
mat sdrgl mforce ddglp mload obj 542
mat sdrgl mforce ddglp get all
mat sdrgl mforce ddglp indo all
end
halt
end
~
#2001
Sacerdote Silue (2) (Sheep)~
0 j 100
~
if %object.vnum% == 543
 wait 1 s
 emote aggrotta la fronte...
 wait 2 s
 say Mi spiace %actor.name%, ma questo Globo di Luce e' ormai inutilizzabile, la missione e' fallita.
 set missione 0
 global missione
 halt
end
if %object.vnum% != 542
 halt
end
 if %actor.name% != %pg%
 wait 2 s
 say Dama Silue affido' questa missione a %pg%, e non a voi %actor.name%
 wait 2 s
 say Non posso darvi alcuna ricompensa
 set missione 0
 global missione
 halt
end
wait 2 s
sor
wait 1 s
ann
say Molto bene %pg%, vedo che la vostra Fede in Silue e' salda: avete completato la vostra missione.
set missione 0
global missione
mat xxxxxx drop all
if %actor.level% < 20
 set livello 15
elseif %actor.level% < 40
 set livello 35
else
 set livello 55
end
eval moltip %random.10%
eval divisore %random.10%
eval premio %livello%*10
eval premio %premio%*%moltip%
eval premio %premio%/%divisore%
global premio
wait 2 s
say Ecco la vostra ricompensa, %pg%
wait 1 s
if %premio% <= 400
mload obj 544
 give rosso %pg%
 posa rosso
 wait 1 s
 inch %pg%
 halt
elseif %premio% <= 800
 mload obj 545
 give giallo %pg%
 posa giallo
 wait 1 s
 inch %pg%
 halt
elseif %premio% <= 950
 mload obj 546
 give blu %pg%
 posa blu
 wait 1 s
 inch %pg%
 halt
else
 mload obj 547
 give bianco %pg%
 posa bianco
 wait 1 s
 inch %pg%
 halt
end
~
#2002
Globo luce (1) (Sheep)~
1 g 100
~
if %get% == 1
set get 2
global get
oecho &9&xIl Globo pian piano si spegne&0
otransform 543
halt
end
set get 1
global get
~
#2003
Globo luce (2) (Sheep)~
1 hi 100
~
if %give% == 1
halt
end
wait 1 s
oecho &9&xIl Globo pian piano si spegne&0
set give 1
global give
otransform 543
~
#2004
Contatore PuntiFede (Sheep)~
0 j 100
~
if %object.vnum% == %statuetta%
mat xxxxxx drop statuetta
wait 1 s
msend %actor.name% Quando consegnerai la statuetta all'altare...
msend %actor.name% ...potrai decidere chi deve essere giustiziato con il comando "giustizia X"
msend %actor.name% Dove al posto di X devi mettere il nome di un PG attualmente presente
msend %actor.name% Ricorda che solo il Papa puo' usare questo comando
set giustiziere 1
global giustiziere
end
if %object.vnum% == %nbase%
 wait 1
 eval puntigrazia %puntigrazia% + 1
 global puntigrazia
 mat %controllore% say crea 1
elseif %object.vnum% == %nbase% + 1
 wait 1
 eval puntigrazia %puntigrazia% + 2
 global puntigrazia
 mat %controllore% say crea 2
elseif %object.vnum% == %nbase% + 2
 wait 1
 eval puntigrazia %puntigrazia% + 4
 global puntigrazia
 mat %controllore% say crea 4
elseif %object.vnum% == %nbase% + 3
 wait 1
 eval puntigrazia %puntigrazia% + 8
 global puntigrazia
 mat %controllore% say crea 8
else
 halt
end
wait 1 s
mecho %risucchiamento%
if %puntifede% >= %cap%
 halt
end
if %object.vnum% == %nbase%
 wait 1
 mat xxxxxx drop diamante
mat xxxxxx drop fiamma
 eval puntifede %puntifede% + 50
 global puntifede
elseif %object.vnum% == %nbase% + 1
 wait 1
 mat xxxxxx drop diamante
mat xxxxxx drop fiamma
 eval puntifede %puntifede% + 100
 global puntifede
elseif %object.vnum% == %nbase% + 2
 wait 1
 mat xxxxxx drop diamante
mat xxxxxx drop fiamma
 eval puntifede %puntifede% + 200
 global puntifede
elseif %object.vnum% == %nbase% + 3
 wait 1
 mat xxxxxx drop diamante
mat xxxxxx drop fiamma
 eval puntifede %puntifede% + 400
 global puntifede
end
if %puntifede% > %cap%
 eval diff %cap% - %puntifede%
 eval sott %diff% / 50
 mat %controllore% say distruggi %sott%
 set puntifede %cap%
 global puntifede
end
~
#2005
Altare Silue (1) (Sheep)~
0 k 100
~
msend %actor.name% %annientamento%
wait 1
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
dg_cast 'disintegrate' %actor.name%
~
#2006
Altare Silue (2) (Sheep)~
0 d 1
cura grazia~
wait 1 s
if %speech% == cura di Silue
 if %puntigrazia% < 20
  mecho &7&xL'altare brilla per qualche istante, ma non succede nulla&0
  halt
 end
 mecho &7&xL'altare brilla per qualche istante...&0
 dg_cast 'heal' %actor.name%
elseif %speech% == grazia di Silue
 if %puntigrazia% < 40
  mecho &7&xL'altare brilla per qualche istante, ma non succede nulla&0
  halt
 end
 mecho &7&xL'altare brilla per qualche istante...&0
 dg_affect %actor.name% sanct 70 6
end
~
#2007
Altare Silue (3) (Sheep)~
0 d 1
altare~
set valido 0
if %actor.name% == alhandir || %actor.name% == irin || %actor.name% == lain || %actor.name% == bridge || %actor.name% == esmeralda || %actor.name% == cry || %actor.name% == yuna
 set valido 1
end
if %valido% == 1
 if %speech% == altare di Silue
  wait 1 s
  eval cap4 %cap%/4
  eval cap2 %cap%/2
  eval cap3 %cap%*3
  eval cap5 %cap3%/4
  if %puntifede% <= %cap4%
   msend %actor.name% &7&xL'altare di Silue brilla flebilmente...&0
   elseif %puntifede% <= %cap2%
   msend %actor.name% &7&xL'altare di Silue brilla intensamente...&0
   elseif %puntifede% <= %cap5%
   msend %actor.name% &7&xL'altare di Silue brilla molto intensamente...&0
   else
   msend %actor.name% &7&xL'altare di Silue brilla con pieno vigore...&0
  end
 end
end
~
#2008
Altare Silue (4) (Sheep)~
0 n 100
~
set cap 2000
global cap
msend cptifedesilue conteggio
~
#2009
Altare di Silue (5) (Sheep)~
0 d 100
manto~
set valido 0
if %actor.name% == alhandir || %actor.name% == irin || %actor.name% == lain || %actor.name% == bridge || %actor.name% == esmeralda || %actor.name% == cry || %actor.name% == yuna
 set valido 1
end
if %valido% == 1
 if %speech% == manto di Silue
  wait 1 s
  if %puntifede% < 1500
   mecho &7&xL'altare brilla per qualche secondo, ma non pare succedere nulla...&0
   halt
  end
  mecho &7&xL'altare brilla per qualche istante...&0
  mload obj 555
  posa manto
  return 0
  eval puntifede %puntifede%-1500
  global puntifede
  mat cptifedesilue say distruggi 30
 end
end
~
#2010
Altare di Silue (6) (Sheep)~
0 d 1
angelo~
set valido 0
if %actor.name% == alhandir || %actor.name% == irin || %actor.name% == lain || %actor.name% == bridge || %actor.name% == esmeralda || %actor.name% == cry || %actor.name% == yuna
 set valido 1
end
if %valido% == 1
 if %speech% == angelo di Silue
  wait 1 s
  if %puntifede% < 1000
   mecho L'altare brilla per qualche secondo, ma non pare succedere nulla...
   halt
  end
  mecho L'altare brilla per qualche istante...
  mload mob 577
  wait 1 s
  mforce acs say Dama Silue mi invia a proteggervi, %actor.name%.
wait 2
mforce %actor.name% say tu ora mi seguirai, in nome di %god%
  eval puntifede %puntifede%-1000
  global puntifede
  mat cptifedesilue say distruggi 20
 end
end
~
#2011
Altare di Silue (7) (Sheep)~
0 d 1
luce~
set valido 0
if %actor.name% == alhandir || %actor.name% == irin || %actor.name% == lain || %actor.name% == bridge || %actor.name% == esmeralda || %actor.name% == yuna
 set valido 1
end
if %valido% == 1
 if %speech% == luce sacra di Silue
  wait 1 s
  if %puntifede% < 400
   mecho &7&xL'altare brilla per qualche secondo, ma non pare succedere nulla...&0
   halt
  end
  mecho &7&xL'altare brilla per qualche istante...&0
  mload obj 556
  mload obj 556
  mload obj 556
  mload obj 556
  posa all.pzl
  eval puntifede %puntifede%-400
  global puntifede
  mat cptifedesilue say distruggi 8
 end
end
~
#2012
Altare di Silue (8) (Sheep)~
0 d 1
preghiere~
if %speech% == preghiere
msend %actor.name% Ecco le preghiere disponibili ai fedeli di Silue:
msend %actor.name% -------------------------------------------------
msend %actor.name% cura di Silue
msend %actor.name% grazia di Silue
msend %actor.name% -------------------------------------------------
msend %actor.name% -------------------------------------------------
msend %actor.name% Ecco le preghiere riservate agli adepti del culto
msend %actor.name% -------------------------------------------------
msend %actor.name% altare di Silue
msend %actor.name% luce sacra di Silue
msend %actor.name% angelo di Silue
msend %actor.name% manto di Silue
msend %actor.name% -------------------------------------------------
msend %actor.name% -------------------------------------------------
msend %actor.name% Ecco le preghiere riservate al capo del culto
msend %actor.name% -------------------------------------------------
msend %actor.name% giustizia di Silue
end
~
#2013
Sacerdote di Silue (3) (Sheep)~
0 g 100
~
if %direction% == north
  wait 1 s
  inch %actor.name%
  wait 1 s
  if %actor.name% == alhandir
    say Salute a voi Templare Alhandir, che la Luce vi protegga
  else
   say Benvenuto nel Tempio di Dama Silue, %actor.name%. Che la Luce di Silue illumini il tuo cammino
  end
end
~
#2014
Via raccoglimento (Nimrhuin)~
0 e 100
messaggio~
if %raccolti% < %controllo%
 wait 1
 drop all
 get 2 ptofede2
 if %controllino% == 0
  if %puntigrazia% >= 2
   eval puntigrazia %puntigrazia% - 2
  else
   eval puntigrazia 0
  end
  eval controllino 1
  global controllino
 end
 mat xxxxxx drop all
 mat %altare% say puntigrazia %puntigrazia%
 wait 1
 mat %altare% say puntifede %puntifede%
 halt
end
if %actor.name% == %naltare% || %actor.name% == %burattino%
wait 1
 get ptofede
 eval controllo %controllo% + 1
 global controllo
 msend %burattino% parlami...
end
~
#2015
Raccoglimento (Nimrhuin)~
0 e 100
prendi~
wait 1
eval raccolti %raccolti% + 1
global raccolti
if %object.name% == ptofede2
 eval puntigrazia %puntigrazia% + 1
 global puntigrazia
elseif %object.name% == ptofede1
 eval puntifede %puntifede% + 50
 global puntifede
end
~
#2016
Caricamento raccoglitore (Nimrhuin)~
0 n 100
~
set controllino 0
global controllino
set puntifede 0
global puntifede
set puntigrazia 0
global puntigrazia
eval controllo 0
global controllo
eval raccolti 0
global raccolti
eval condizione 0
global condizione
set naltare &7&xAltare di Silue&0
global naltare
set altare altaresilue
global altare
set burattino burattinosilue
global burattino
~
#2017
Burattino Silue (Nimrhuin)~
0 e 100
messaggio~
wait 2
msend cptifedesilue conteggio
~
#2018
Altare di Silue (9) (Nimrhuin)~
0 d 100
puntigrazia puntifede~
if %actor.name% == %ncontrollore%
 if %speech% /= puntigrazia
  set %speech%
  global puntigrazia
 elseif %speech% /= puntifede
  set %speech%
  global puntifede
 end
end
~
#2019
Caricamento altare Silue (Nimrhuin)~
0 n 100
~
wait 1
msend cptifedesilue conteggio
set risucchiamento &7&xL'altare viene circondato di una accecante luce per qualche istante mentre il diamante si consuma&0
global risucchiamento
set controllore cptifedesilue
global controllore
set annientamento &7&xLa luce di Silue ti consuma&0
global annientamento
set ncontrollore Il controllore dei punti fede di Silue
global ncontrollore
set cap 3000
global cap
set nbase 544
global nbase
set god &7&xSilue&0
global god
set statuetta 577
global statuetta
~
#2020
Creazione punti (Nimrhuin)~
0 d 100
crea distruggi~
if %actor.name% == %naltare%
 if %speech% /= crea
  set %speech%
  set i 0
  wait 1
  while %i% < %crea%
   eval i %i% + 1
   mload obj 2037
   if %puntigrazia% < 100
    mload obj 2038
    eval puntigrazia %puntigrazia% + 1
   end
  done
  drop all
 elseif %speech% /= distruggi
  set %speech%
  get %distruggi% ptofede1
  mat xxxxxx drop all
 end
end
~
#2021
Il purgatore (Nimrhuin)~
0 ab 100
~
wait 1
mpurge
~
#2022
Caricamento altare Shaarr (Nimrhuin)~
0 n 100
~
wait 1
msend cptifedeshaarr conteggio
set risucchiamento &1&xIl teschio prende improvvisamente fuoco e viene consumato in pochi istanti&0
global risucchiamento
set controllore cptifedeshaarr
global controllore
set annientamento &1&xLe fiamme di Shaarr ti consumano&0
global annientamento
set ncontrollore Il controllore dei punti fede di Shaarr
global ncontrollore
set cap 3000
global cap
set nbase 2039
global nbase
set god &1&xShaarr&0
global god
set statuetta 580
global statuetta
~
#2023
Caricamento altare Xhyphys (Nimrhuin)~
0 n 100
~
wait 1
msend cptifedexhyphys conteggio
set risucchiamento &xL'altare inghiotte la fiamma.&0
global risucchiamento
set controllore cptifedexhyphys
global controllore
set annientamento &xLa corruzione di Xhyphys ti pervade&0
global annientamento
set ncontrollore Il controllore dei punti fede di Xhyphys
global ncontrollore
set cap 3000
global cap
set nbase 2043
global nbase
set god &5Xhyphys&0
global god
set statuetta 578
global statuetta
~
#2024
Caricamento altare Therion (Nimrhuin)~
0 n 100
~
wait 1
msend cptifedetherion conteggio
set risucchiamento &6&xL'altare avvinghia il fiore&0
global risucchiamento
set controllore cptifedetherion
global controllore
set annientamento &6&xLe lame di Therion ti trafiggono&0
global annientamento
set ncontrollore Il controllore dei punti fede di Therion
global ncontrollore
set cap 3000
global cap
set nbase 2047
global nbase
set god &6&xTherion&0
global god
set statuetta 579
global statuetta
~
#2025
Caricamento raccoglitore Shaarr (Nimrhuin)~
0 n 100
~
set controllino 0
global controllino
set puntifede 0
global puntifede
set puntigrazia 0
global puntigrazia
eval controllo 0
global controllo
eval raccolti 0
global raccolti
eval condizione 0
global condizione
set naltare &1&xAltare di Shaarr&0
global naltare
set altare altareshaarr
global altare
set burattino burattinoshaarr
global burattino
~
#2026
Caricamento raccoglitore Xhyphys (Nimrhuin)~
0 n 100
~
set controllino 0
global controllino
set puntifede 0
global puntifede
set puntigrazia 0
global puntigrazia
eval controllo 0
global controllo
eval raccolti 0
global raccolti
eval condizione 0
global condizione
set naltare &5Altare di Xhyphys&0
global naltare
set altare altarexhyphys
global altare
set burattino burattinoxhyphys
global burattino
~
#2027
Caricamento raccoglitore Therion (Nimrhuin)~
0 n 100
~
set controllino 0
global controllino
set puntifede 0
global puntifede
set puntigrazia 0
global puntigrazia
eval controllo 0
global controllo
eval raccolti 0
global raccolti
eval condizione 0
global condizione
set naltare &6&xAltare di Therion&0
global naltare
set altare altaretherion
global altare
set burattino burattinotherion
global burattino
~
#2028
Burattino Shaarr (Nimrhuin)~
0 e 100
messaggio~
wait 2
msend cptifedeshaarr conteggio
~
#2029
Burattino Xhyphys (Nimrhuin)~
0 e 100
messaggio~
wait 2
msend cptifedexhyphys conteggio
~
#2030
Burattino Therion (Nimrhuin)~
0 e 100
messaggio~
wait 2
msend cptifedetherion conteggio
~
#2031
Cuore di Shaarr (1) (Sheep)~
1 g 100
~
if %get% == 1
otransform 559
halt
end
set get 1
global get
~
#2032
Cuore di Shaarr (2) (Sheep)~
1 hi 100
~
if %give% == 1
halt
end
wait 1 s
oecho &1&xIl cuore viene divorato dalle fiamme!&0
set give 1
global give
otransform 559
~
#2033
Autoquest Shaarr (Sheep)~
0 d 1
prova~
if %actor.vnum% != -1
halt
end
if %actor.level% > 70
wait 2 s
inch %actor.name%
wait 2 s
say Andatevene miserabile %actor.name%, e pregate che l'ira di Shaarr non si volga mai verso di voi
halt
end
if %missione% == 1
wait 2 s
say %pg% sta gia' eseguendo la Volonta' di Shaarr! Tornate in un altro momento, %actor.name%   
halt
end
if %mob% == 1
wait 2 s
say Malvagio %actor.name%, dovrete aspettare un po' di tempo prima di poter strappare qualche cuore.
halt
end
set missione 1
set counter 0
global counter
global missione
set mob 1
global mob
set livello %actor.level%
global livello
set pg %actor.name%
global pg
wait 2 s
soggh
wait 1 s
ann
wait 1 s
say Molto bene %pg%, Shaarr accetta che voi soffriate e moriate per la sua maggior gloria
wait 2 s
if %livello% < 20
eval quest %random.3%
if %quest% == 1
say Esiste una stupida regina nel Feudo di Astrakan, felice della propria vita priva colma di gioia.
wait 2 s
say Fatele conoscere il significato della Volonta' di Shaarr
wait 2 s
say Portatemi il suo cuore, e la ricompensa sara' vostra, %pg%.
mat armiria mpurge armiria
mat rfcmsn mload mob 16005
mat rfcmsn mforce armiria mload obj 558
mat rfcmsn mforce armiria mgoto 16065
mat armiria mforce armiria get arazzo
halt
elseif %quest% == 2
say Rintanato nella foresta di Lanowar vive un cavaliere, fiero delle sue molte battaglie vinte contro  il male...
wait 2 s
say Shaarr ritiene che sia giunta l'ora di insegnargli da quale parte il vero Potere risiede...
wait 2 s
say Portatemi il suo cuore, e la ricompensa sara' vostra, %pg%.
mat dcxqs mpurge dcxqs
mat rfcmsn mload mob 10015
mat rfcmsn mforce dcxqs mload obj 558
mat rfcmsn mforce dcxqs mgoto 10009
mat dcxqs mforce dcxqs get all
mat dcxqs mforce dcxqs indo all
halt
else
say Nel felice villaggio di Sean Wast la vita procede tranquilla, grazie alla protezione di un potente mago...
wait 2 s
say mostrate agli stupidi abitanti del villaggio quanto sia grande il potere di Shaarr! Uccidete il mago!
wait 2 s
say Portatemi il suo cuore, e la ricompensa sara' vostra, %pg%.
mat ashvar mpurge ashvar
mat rfcmsn mload mob 18404
mat rfcmsn mforce ashvar mload obj 558
mat rfcmsn mforce ashvar mgoto 18428
mat ashvar mforce ashvar get all
mat ashvar mforce ashvar indo all
mat ashvar mforce ashvar hold bacc
halt
end
elseif %livello% < 40
eval quest %random.3%
if %quest% == 1
say Nella infetta citta' di Kuna, vive uno stupido paladino che resiste alle forze del male!
wait 2 s
say dimostrategli quanto vana sia la sua lotta!
wait 2 s
say Portatemi il suo cuore, e la ricompensa sara' vostra, %pg%.
mat ptxqs mpurge ptxqs
mat rfcmsn mload mob 24003
mat rfcmsn mforce ptxqs mload obj 558
mat rfcmsn mforce ptxqs mgoto 24011
mat ptxqs mforce ptxqs get all
mat ptxqs mforce ptxqs indo all
mat ptxqs mforce ptxqs hold chia
halt
elseif %quest% == 2
say  Tzegano e' il capoclan dell'accampamento gitano nei pressi della citta' di Chyrellos...
wait 2 s
say  Nella sua stoltezza egli crede di detenere il Potere. Mostrategli il vero significato del Potere! 
wait 2 s
say Portatemi il suo cuore, e la ricompensa sara' vostra, %pg%.
mat tzegano mpurge tzegano
mat rfcmsn mload mob 17438
mat rfcmsn mforce tzegano mload obj 558
mat rfcmsn mforce tzegano mgoto 17461
mat tzegano mforce tzegano get all
mat tzegano mforce tzegano indo all
mat tzegano mforce tzegano impu mazza
halt
else
say Nella citta' di Leah, Re Morgan regna ostentando il suo piccolo ed insignificante potere.
wait 2 s
say Mostrate a quella stupida citta' chi e' che detiene il vero Potere uccidendo senza pieta' il Re!
wait 2 s
say Portatemi il suo cuore, e la ricompensa sara' vostra, %pg%.
mat mlrlqs mpurge mlrlqs
mat rfcmsn mload mob 16312
mat mlrlqs mforce mlrlqs mload obj 558
mat rfcmsn mforce mlrlqs mgoto 16327
mat mlrlqs mforce mlrlqs get all
mat mlrlqs mforce mlrlqs indo all
mat mlrlqs mforce mlrlqs hold chia
end
halt
else
eval quest %random.3%
if %quest% == 1
say Re Kajafesh regna sulla citta' di uomini lucertola di Gashha ma non pare voler chinare il capo...
wait 2 s
say ...innanzi a Colei che detiene il Vero Potere. Mostrategli la forza di Shaarr!
wait 2 s
say Portatemi il suo cuore, e la ricompensa sara' vostra, %pg%.
mat kajafesh mpurge kajafesh
mat rfcmsn mload mob 30000
mat rfcmsn mforce kajafesh mload obj 558
mat rfcmsn mforce kajafesh mgoto 30000
mat kajafesh mforce kajafesh get all
mat kajafesh mforce kajafesh indo all
mat kajafesh mforce kajafesh hold pipa
halt
elseif %quest% == 2
say In Daresia vive il Supremo Cavaliere, del Sacro Ordine dei Paladini, e' superfluo dirvi che...
wait 2 s
say ...La Maestra del Dolore non apprezza la sua esistenza. Ponetele fine.
wait 2 s
say Portatemi il suo cuore, e la ricompensa sara' vostra, %pg%.
mat scqxs mpurge scqxs
mat rfcmsn mload mob 7900
mat rfcmsn mforce scqxs mload obj 558
mat rfcmsn mforce scqxs mgoto 7900
mat scqxs mforce scqxs get all
mat scqxs mforce scqxs indo all
mat scqxs mforce scqxs hold chiave
halt
else
say  A Camelot vive un impavido cavaliere che piu' volte ha osato sfidare Shaarr! Sir Lancillotto!
wait 2 s
say Spezzate il suo corpo e la sua volonta' affinche' si inchini anch'egli alla Signora!   
wait 2 s
say Portatemi il suo cuore, e la ricompensa sara' vostra, %pg%.
mat clxqs mpurge clxqs
mat rfcmsn mload mob 17312
mat rfcmsn mforce clxqs mload obj 558
mat rfcmsn mforce clxqs mgoto 17359
mat clxqs mforce clxqs get gua
mat clxqs mforce clxqs indo all
end
halt
end
~
#2034
Guardie Xhyphys (1) (Sheep)~
0 d 1
aprite sbloccate passo~
if %speech% == passo nel nome di Xyphys
  if %bloccata% == 0
    wait 1 s
    unlock porta
    apri porta
    wait 5 s
    chiudi porta
    lock porta
  else
    halt
  end
end
if %speech% == aprite e bloccate
  if %actor.name% == sagat
    wait 1 s
    unlock porta
    apri porta
    wait 3 s
    chiudi porta
    lock porta
    set bloccata 1
    global bloccata
    halt
  end
end
if %speech% == sbloccate
  if %actor.name% == sagat
    set bloccata 0
    global bloccata
    halt
  end
end
 
~
#2035
Guardie Xhyphys (2) (Sheep)~
0 g 100
~
if %direction% == north
wait 3 s
chiudi porta
lock porta
~
#2036
Klatu veratu niktu (Sheep)~
2 d 1
clatu~
if %aperta% == 0
if %speech% == clatu veratu niktu
set aperta 1
global aperta
wait 1 s
wecho &5Una botola magicamente si delinea dove prima c'era solo il pavimento...&0
%door% 1233 down room 1234
end
else
wait 1 s
wecho &5Al suono delle parole la botola scompare dalla vista&0
%door% 1233 down purge
set aperta 0
global aperta
end
~
#2037
Sacerdote Xhyphys 2 (Nimrhuin)~
0 j 100
~
if %object.vnum% == 2054
 wait 1 s
 emo ritrae sdegnato le mani.
 wait 2 s
 say Non sfidare l'ira di Xhyphys, questa mazza e' ormai rotta e inutilizzabile!
 set missione 0
 global missione
 halt
elseif %object.vnum% != 2053
 halt
end
if %actor.name% != %pg%
 wait 2 s
 say Per Xhyphys questo e' un affronto, Egli aveva affidato la sua missione a %pg%!!!
 set missione 0
 global missione
 halt
end
wait 2 s
emo slancia le braccia al cielo e lascia cadere la testa sul petto.
wait 1 s
emo solleva la testa e annuisce con un'espressione che non sapresti se definire di felicita'.
wait 1 s
say Xhyphys e' molto soddisfatto di te, %pg%.
wait 1 s
say Ha deciso di ricompensarti.
set missione 0
global missione
mat xxxxxx drop all
if %actor.level% < 20
 set livello 15
elseif %actor.level% < 40
 set livello 35
else
 set livello 55
end
eval moltip %random.10%
eval divisore %random.10%
eval premio %livello%*10
eval premio %premio%*%moltip%
eval premio %premio%/%divisore%
global premio
if %premio% <= 400
 mload obj 2043
 give fiamma %pg%
 posa fiamma
 wait 1 s
 bow %pg%
 halt
elseif %premio% <= 800
 mload obj 2044
 give fiamma %pg%
 posa fiamma
 bow %pg%
 halt
elseif %premio% <= 950
 mload obj 2045
 give fiamma %pg%
 posa fiamma
 wait 1 s
 bow %pg%
 halt
else
 mload obj 2046
 give fiamma %pg%
 posa fiamma
 bow %pg%
 halt
end
~
#2038
Sacerdote Xhyphys 1 (Nimrhuin)~
0 d 100
prova~
if %actor.vnum% != -1
halt
end
if %actor.level% > 70
 wait 2 s
 emo rivolta gli occhi al cielo e gira la testa di scatto.
 wait 1 s
 say Xhyphys si compiace che anche gli immortali seguano la Via, ma la voce del Caos vi seguira' sempre mostrandovi la scelta corretta.
 wait 1
 say Non avete bisogno di qualcuno che vi consigli sulle azioni da fare.
 halt
end
if %missione% == 1
 wait 2 s
 say Altri saggi ascoltatori di Xhyphys stanno gia' eseguendo il suo volere, per ora Lui non ha bisogno di te.
 wait 1 s
 say Che il Caos possa seguirti nelle tue scelte.
 halt
end
if %mob% == 1
 wait 2 s
 say Sto attendendo che Xhyphys mi riveli la sua volonta'. Torna piu' tardi e ti sapro' dire se Egli necessita di una mano mortale.
 wait 1 s
 say Che il Caos possa seguirti nelle tue scelte.
 halt
end
set missione 1
global missione
set counter 0
global counter
set mob 1
global mob
set livello %actor.level%
global livello
set pg %actor.name%
global pg
wait 2 s
emo apre gli occhi di scatto.
wait 1 s
nod
wait 1 s
say Xhyphys e' lieto di vedere come molti contribuiscano allo sviluppo del Caos.
wait 2 s
eval quest %random.3%
if %livello% < 20
 if %quest% == 1
  say Un uomo osa sfidare apertamente Xhyphys, offrendo il suo aiuto ai poteri della natura.
  wait 2 s
  say Trova rifugio all'interno stesso di un albero, nella foresta al centro della daresia. Xhyphys vuole che sia raggiunto e punito.
  mat elandar mpurge elandar
  mat nimrhuinxyz mload mob 5624
  mat nimrhuinxyz mforce elandar mgoto 6155
  mat elandar mforce elandar mload obj 2053
  mat elandar mforce elandar get all
  mat elandar mforce elandar hold mazza
  mat elandar mforce elandar wear all
 elseif %quest% == 2
  say Una disgustosa bestia, nelle terre di Sean West, si e' appropriata di poteri che solo i figli del Caos dovrebbero avere.
  wait 2 s
  say Uccidila, ma fai attenzione, riesciura' ad evitare molti dei tuoi colpi.
  mat pantera mpurge pantera
  mat nimrhuinxyz mload mob 18408
  mat pantera mforce pantera mgoto 18435
  mat pantera mforce pantera get all
  mat pantera mforce pantera mload obj 2053
  mat pantera mforce pantera hold mazza
  mat pantera mforce pantera wear all
 elseif %quest% == 3
  say Un piccolo e detestabile elfo si e' alleato con alcuni stolti orchi sciamani per dar vita alla peggiore creazione che mai abbia visto la Daresia intera.
  wait 2 s
  say Stanno cercando di creare degli orchi ancora piu' potenti e sempre piu' ligi al dovere! Questo scempio deve essere fermato.
  mat mobquestxhyphys3 mpurge mobquestxhyphys3
  mat nimrhuinxyz mload mob 6211
  mat nimrhuinxyz mforce mobquestxhyphys3 mgoto 6214
  mat mobquestxhyphys3 mforce mobquestxhyphys3 mload obj 2053
  mat mobquestxhyphys3 mforce mobquestxhyphys3 hold mazza
 end
 halt
elseif %livello% < 40
 if %quest% == 1
  say Un adorabile Caos regnava nella citta' dei ragni prima che un essere sfacciato, una Ki-Rin, cercasse di cambiare la situazione.
  wait 2 s
  say Osa tentare di riportare l'ordine tra le creature naturali, cosa vi puo' essere di piu' detestabile! Xhyphys vuole che tu ridimensioni questo problema...
  mat ki-rin mpurge ki-rin
  mat nimrhuinxyz mload mob 6315
  mat ki-rin mforce ki-rin mgoto 6350
  mat ki-rin mforce ki-rin get all
  mat ki-rin mforce ki-rin mload obj 2053
  mat ki-rin mforce ki-rin hold mazza
 elseif %quest% == 2
  say Dimmi, trovi che possa esistere qualcosa di piu' deprecabile che avere disposizione di grandi poteri magici e sprecarli nello studio degli elementi naturali?
  wait 2 s
  say Sono questi i peggiori nemici di Xhyphys, che ti chiede di rintracciare Zirax l'eremita e punirlo. Dicono che i suoi ragni amino giocare con gli zingari...
  mat zirax mpurge zirax
  mat nimrhuinxyz mload mob 17447
  mat zirax mforce zirax mgoto 17474
  mat zirax mforce zirax get all
  mat zirax mforce zirax wear all
  mat zirax mforce zirax mload obj 2053
  mat zirax mforce zirax hold mazza
 else
  say Gli esseri umani sono cosi' stupidi! Avrebbero grandi doti di intelligenza ma riescono sempre a farsi assoggettare da uno di loro che si dice Re e benefattore...
  wait 1 s
  spit
  wait 1 s
  say Tale si dichiara quel Welmar, riporta il Caos naturale nella sua regione... togliendolo di mezzo.
  mat welmar mpurge welmar
  mat nimrhuinxyz mload mob 15001
  mat welmar mforce welmar mgoto 15017
  mat welmar mforce welmar get all
  mat welmar mforce welmar wear all
  mat welmar mforce welmar mload obj 2053
  mat welmar mforce welmar hold mazza
 end
 halt
else
 if %quest% == 1
  say Sono i draghi, i draghi! Sono loro la peggior parte del creato, il piu' grande potere della natura, l'incarnazione della rigidita' degli ordini...
  wait 2 s
  say Il drago dei cristalli, che soffia in caverne abitate da pipistrelli, rappresenta una minaccia per il culto di Xhyphys: Egli vuole che tu lo elimini.
  mat mobxhyphys7 mpurge mobxhyphys7
  mat nimrhuinxyz mload mob 16206
  mat mobxhyphys7 mforce mobxhyphys7 mgoto 16247
  mat mobxhyphys7 mforce mobxhyphys7 get all
  mat mobxhyphys7 mforce mobxhyphys7 wear all
  mat mobxhyphys7 mforce mobxhyphys7 mload obj 2053
  mat mobxhyphys7 mforce mobxhyphys7 hold mazza
 elseif %quest% == 2
  say Camelot, che fantastica citta' era... fazioni distinte, guerra civile, Caos... ma si e' rovinata!
  wait 2 s
  say Quell'Artu' un giorno si e' dichiarato Re di ogni gente e ha iniziato a pacificare le parti, rovinando quanto Xhyphys gradiva. Uccidilo.
  mat mobxhyphys8 mpurge mobxhyphys8
  mat nimrhuinxyz mload mob 17304
  mat mobxhyphys8 mforce mobxhyphys8 mgoto 17370
  mat mobxhyphys8 mforce mobxhyphys8 get all
  mat mobxhyphys8 mforce mobxhyphys8 mload obj 2053
  mat mobxhyphys8 mforce mobxhyphys8 wear all
  mat mobxhyphys8 mforce mobxhyphys8 hold mazza
 else
  say Il Caos e' liberta', liberta' di agire, di uccidere, di aiutare. Quando il Caos viene rinchiuso tende a morire...
  wait 1 s
  say ... ancor piu' se il luogo di prigionia e' il duro corpo di un drago rosso.
  wait 2 s
  say Con ignominia il libero spirito della rabbia e' stato rinchiuso, in un simile corpo, nei recessi delle fogne di Deira. Xhyphys vuole che tu lo liberi!
  mat mobxhyphys9 mpurge mobxhyphys9
  mat nimrhuinxyz mload mob 7040
  mat mobxhyphys9 mforce mobxhyphys9 mgoto 7328
  mat mobxhyphys9 mforce mobxhyphys9 mload obj 2053
  mat mobxhyphys9 mforce mobxhyphys9 hold mazza
 end
end
wait 1 s
say Portami la Mazza di Xhyphys, ed Egli ti ricompensera'.
~
#2039
Sacerdote Xhyphys 3 (Nimrhuin)~
0 g 100
~
if %direction% == south
 wait 1 s
 emo volge gli occhi al cielo e abbandona le braccia lungo il corpo.
 wait 1 s
 say Xhyphys ti accoglie nel suo tempio, figlio del Caos.
end
~
#2040
Mazza Xhyphys 1 (Nimrhuin)~
1 g 100
~
if %get% == 1
 otransform 2054
 oecho &9&xAl tuo tocco la mazza si spezza in due parti ormai inutilizzabili&0
 halt
 set get 2
 global get
 halt
end
set get 1
global get
~
#2041
Mazza Xhyphys 2 (Nimrhuin)~
1 hi 100
~
if %give% == 1
halt
end
wait 1 s
oecho &9&xAl tuo tocco la mazza si spezza in due parti ormai inutilizzabili&0
set give 1
global give
otransform 2054
~
#2042
Altare di Xhyphys 1 (Nimrhuin)~
0 d 100
abbandono incarnazione~
wait 1 s
if %speech% == abbandono al Caos
 if %puntigrazia% < 20
  mecho &5L'altare inizia ad illuminarsi, ma non succede nulla&0
  halt
 end
 mecho &5L'altare si illumina ed emette sinistri rumori.&0
 wait 1 s
 mecho &5Ti abbandoni al Caos, e senti la sua potenza rigenerare il tuo corpo.&0
 dg_cast 'cure critic' %actor.name%
 dg_cast 'cure critic' %actor.name%
elseif %speech% == incarnazione del Caos
 if %puntigrazia% < 40
  mecho &5L'altare inizia ad illuminarsi, ma non succede nulla&0
  halt
 end
 mecho &5L'altare si illumina ed emette sinistri rumori.&0
 wait 1 s
 mecho &5Chiudi gli occhi e senti il potere del Caos entrare in te per combattere al tuo fianco.&0
 dg_affect %actor.name% deathdance 0 4
end
~
#2043
Altare di Xhyphys 2 (Nimrhuin)~
0 d 100
altare~
set valido 0
if %actor.name% == Sagat || %actor.name% == Cassius || %actor.name% == Seth || %actor.name% == christian
 set valido 1
end
if %actor.name% == Shazarak || %actor.name% == inuyasha || %actor.name% == eligor || %actor.name% == smigol
set valido 1
end
if %valido% == 1
 if %speech% == altare di xhyphys
  wait 1 s
  eval cap4 %cap%/4
  eval cap2 %cap%/2
  eval cap3 %cap%*3
  eval cap5 %cap3%/4
  if %puntifede% <= %cap4%
  msend %actor.name% &5L'altare di Xhyphys emette flebili luci e vaghi lamenti...&0
  elseif %puntifede% <= %cap2%
  msend %actor.name% &5L'altare di Xhyphys emette una luce intensa e alcune urla...&0
  elseif %puntifede% <= %cap5%
  msend %actor.name% &5L'altare di Xhyphys emette una luce molto intensa e strazianti urla di dolore...&0
  else
  msend %actor.name% &5La stanza e' inondata dall'abbagliante luce e dalle terribili urla di dolore che provengono dall'altare di Xhyphys&0
  end
 end
end
~
#2044
Altare di Xhyphys 3 (Nimrhuin)~
0 d 100
elmo~
set valido 0
if %actor.name% == Sagat || %actor.name% == Cassius || %actor.name% == Seth || %actor.name% == christian
 set valido 1
end
if %actor.name% == Shazarak || %actor.name% == inuyasha || %actor.name% == eligor || %actor.name% == smigol
set valido 1
end
if %valido% == 1
 if %speech% == elmo di Xhyphys
  wait 1
  if %puntifede% < 1500
   mecho &5L'altare inizia ad illuminarsi, ma non succede nulla&0
   halt
  end
  mecho &5L'altare si illumina ed emette sinistri rumori.&0
  mload obj 1460
  drop elmo
  return 0
  eval puntifede %puntifede%-1500
  global puntifede
  mat cptifedexhyphys say distruggi 30
 end
end
~
#2045
Altare di Xhyphys 4 (Nimrhuin)~
0 d 100
protettore~
set valido 0
if %actor.name% == Sagat || %actor.name% == Cassius || %actor.name% == Seth || %actor.name% == christian
 set valido 1
end
if %actor.name% == Shazarak || %actor.name% == inuyasha || %actor.name% == eligor || %actor.name% == smigol
set valido 1
end
if %valido% == 1
 if %speech% == protettore del Caos
  wait 1 s
  if %puntifede% < 1000
   mecho &5L'altare inizia ad illuminarsi, ma non succede nulla&0
   halt
  end
  mecho &5L'altare si illumina ed emette sinistri rumori.&0
  mload mob 2048
  wait 1 s
  mforce nachzehrer say Per volere di Xhyphys ti seguiro' nella lotta per il Caos, %actor.name%.
wait 2
mforce %actor.name% say tu ora mi seguirai, in nome di %god%
  eval puntifede %puntifede%-1000
  global puntifede
  mat cptifedexhyphys say distruggi 20
 end
end
~
#2046
Altare di Xhyphys 5 (Nimrhuin)~
0 d 100
dono~
set valido 0
if %actor.name% == Sagat || %actor.name% == Cassius || %actor.name% == Seth || %actor.name% == christian
 set valido 1
end
if %actor.name% == Shazarak || %actor.name% == inuyasha || %actor.name% == eligor || %actor.name% == smigol
set valido 1
end
if %valido% == 1
 if %speech% == dono di Xhyphys
  wait 1 s
  if %puntifede% < 400
mecho &5L'altare inizia ad illuminarsi, ma non succede nulla&0
   halt
  end
mecho &5L'altare si illumina ed emette sinistri rumori.&0
  mload obj 1461
  mload obj 1461
  mload obj 1461
  drop all.poz
  eval puntifede %puntifede%-400
  global puntifede
  mat cptifedexhyphys say distruggi 8
 end
end
~
#2047
Altare di Xhyphys 6 (Nimrhuin)~
0 d 100
preghiere~
if %speech% == preghiere
 msend %actor.name% Ecco le preghiere disponibili ai fedeli di Xhyphys:
 msend %actor.name% ---------------------------------------------------
 msend %actor.name% abbandono al Caos
 msend %actor.name% incarnazione del Caos
 msend %actor.name% ---------------------------------------------------
 msend %actor.name% ---------------------------------------------------
 msend %actor.name% Ecco le preghiere riservate agli adepti del culto
 msend %actor.name% ---------------------------------------------------
 msend %actor.name% altare di Xhyphys
 msend %actor.name% elmo di Xhyphys
 msend %actor.name% protettore del Caos
 msend %actor.name% dono di Xhyphys
msend %actor.name% ---------------------------------------------------
msend %actor.name% ---------------------------------------------------
msend %actor.name% Ecco le preghiere riservate al capo del culto
msend %actor.name% ---------------------------------------------------
msend %actor.name% distruzione di Xhyphys
end
~
#2048
Pietra Therion 1 (Nimrhuin)~
1 g 100
~
if %give% == 1
halt
end
if %get% == 1
 otransform 2056
 oecho &9&xAppena afferri la Pietra della Vita essa si spegne diventando un volgare sasso.&0
 set get 2
 global get
 halt
end
set get 1
global get
~
#2049
Pietra Therion 2 (Nimrhuin)~
1 hi 100
~
if %give% == 1
 halt
end
wait 1 s
oecho &9&xLa Pietra della Vita essa si spegne diventando un volgare sasso.&0
set give 1
global give
otransform 2056
~
#2050
Sacerdote di Therion 1 (Sheep)~
0 d 100
prova~
if %actor.vnum% != -1
halt
end
if %actor.level% > 70
wait 2 s
inch %actor.name%
wait 2 s
emote borbotta qualcosa nella sua strana lingua..
wait 1 s
say Hum huuuumm brum mi spiace, %actor.name%, ma gli esseri come voi sono al di fuori dell'Ordine Naturale, non vi e' concesso di aiutare Therion.
halt
end
if %missione% == 1
wait 2 s
say Humm rum hum dovremo aspettare che %pg% termini la sua missione prima di affidarne un'altra...
halt
end
if %mob% == 1
wait 2 s
say Hum hm Therion al momento non ha bisogno di aiuto, prode %actor.name%, tornate tra qualche tempo.
halt
end
set missione 1
set counter 0
global counter
global missione
set mob 1
global mob
set livello %actor.level%
global livello
set pg %actor.name%
global pg
wait 2 s
ann
wait 1 s
emote borbotta soddisfatto..
wait 1 s
say Hm..molto bene %pg%, Therion accetta il vostro aiuto!  
wait 2 s
if %livello% < 20
eval quest %random.3%
if %quest% == 1
say Humm hum...molto tempo fa un grosso male colpi' la foresta di Haon-Dor e molte gentili creature furono corrotte
wait 2 s
say Aiutate a risanare lo scempio causato dall'emisario di Xhyphys estirpando l'abominio vegetale da lui generato!
wait 2 s
say Portate la Pietra della Vita, e la ricompensa sara' vostra, %pg%.
mat erbacce mpurge erbacce
mat rfcmsn mload mob 6115
mat rfcmsn mforce erbacce mload obj 2055
mat rfcmsn mforce erbacce mgoto 6128
mat erbacce mforce erbacce get all
halt
elseif %quest% == 2
say Hruumbum un perfido mago vive nascosto nelle profonde caverne di Moria! Per seguire le sue arti magiche ha sconvolto l'equilibrio di quel luogo..   
wait 2 s
say Therion desidera che voi ristabiliate l'ordine delle caverne eliminando il mago.
wait 2 s
say Portate la Pietra della Vita, e la ricompensa sara' vostra, %pg%.
mat mmqs mpurge mmqs
mat rfcmsn mload mob 4100
mat rfcmsn mforce mmqs mload obj 2055
mat rfcmsn mforce mmqs mgoto 4114
mat mmqs mforce mmqs get all
halt
else
say  Hummruuum Alcune persone desiderano andare contro la Natura, prolungando la propria giovinezza: per far questo usano riti che turbano l'ordine naturale delle cose!  
wait 2 s
say Lady Death e' una di queste persone: la sua bellezza e' mantenuta al costo della vita di innumerevoli creature innocenti. Ristabilite l'ordine! 
wait 2 s
say Portate la Pietra della Vita, e la ricompensa sara' vostra, %pg%.
mat ldxqt mpurge ldxqt
mat rfcmsn mload mob 11280
mat rfcmsn mforce ldxqt mload obj 2055
mat rfcmsn mforce ldxqt mgoto 11283
mat ldxqt mforce ldxqt soggh
halt
end
elseif %livello% < 40
eval quest %random.3%
if %quest% == 1
say Hruuumbummbararum Arachnos e' una delle innumerevoli immonde creature generate dalle forze del Caos!
wait 2 s
say Ponete fine alla sua esistenza blasfema in modo che la Natura possa risanare la sua tana...   
wait 2 s
say Portate la Pietra della Vita, e la ricompensa sara' vostra, %pg%.
mat arxqt mpurge arxqt
mat rfcmsn mload mob 6314
mat rfcmsn mforce arxqt mload obj 2055
mat rfcmsn mforce arxqt mgoto 6399
mat arxqt mforce arxqt get all
halt
elseif %quest% == 2
say Hruuum bum, a volte la follia corrompe anche le creature piu' nobili, come gli Elfi.     
wait 2 s
say Elstar, nella sua stoltezza, crede di poter cambiare la natura con i suoi orribili esperimenti. Ponete fine alla sua follia!   
wait 2 s
say Portate la Pietra della Vita, e la ricompensa sara' vostra, %pg%.
mat elstar mpurge elstar
mat rfcmsn mload mob 6210
mat rfcmsn mforce elstar mload obj 2055
mat rfcmsn mforce elstar mgoto 6259
mat elstar mforce elstar get all
mat elstar mforce elstar indo all
mat elstar mforce elstar hold lama
halt
else
say Hrummm bummm esiste una strega che si e' messa al servizio delle forze del Caos... vive nell'impuro luogo conosciuto come Cimitero di Rasheen
wait 2 s
say Purgate la Daresia dalla sua immonda presenza...
wait 2 s
say Portate la Pietra della Vita, e la ricompensa sara' vostra, %pg%.
mat srxqt mpurge srxqt
mat rfcmsn mload mob 18522
mat srxqt mforce srxqt mload obj 2055
mat srxqt mforce srxqt mgoto 18551
mat srxqt mforce srxqt get all
mat srxqt mforce srxqt indo all
mat srxqt mforce srxqt hold chia
end
halt
else
eval quest %random.3%
if %quest% == 1
say Hruuumbararuum le forze del Caos sanno corrompere anche le piu' nobili e potenti creature! 
wait 2 s
say Il possente Drago Verde di Haon-Dor e' stato da tempo mutato in un orrendo Dracolich! Bisogna rimediare!   
wait 2 s
say Portate la Pietra della Vita, e la ricompensa sara' vostra, %pg%.
mat dvxqt mpurge dvxqt
mat rfcmsn mload mob 5630
mat rfcmsn mforce dvxqt mload obj 2055
mat rfcmsn mforce dvxqt mgoto 6143
halt
elseif %quest% == 2
say Hruumrumbum esistono arti oscure e blasfeme che non devono essere praticate perche' turbano la Natura!    
wait 2 s
say Punite il necromante che vive nell'Alta Torre della Magia per la sua follia!   
wait 2 s
say Portate la Pietra della Vita, e la ricompensa sara' vostra, %pg%.
mat ntxqt mpurge ntxqt   
mat rfcmsn mload mob 2549
mat rfcmsn mforce ntxqt mload obj 2055
mat rfcmsn mforce ntxqt mgoto 2625
mat ntxqt mforce ntxqt get all
mat ntxqt mforce ntxqt indo all
halt
else
say  Hrumbararuum un Dracolich rappresenta il Caos che agisce e corrompe la nobilta' della Natura.   
wait 2 s
say  Nei sotterranei della Cattedrale maledetta ne vive uno.... liberate il mondo dalla sua presenza.
wait 2 s
say Portate la Pietra della Vita, e la ricompensa sara' vostra, %pg%.
mat wyrenthoth mpurge wyrenthoth   
mat rfcmsn mload mob 18705
mat rfcmsn mforce wyrenthoth mload obj 2055
mat rfcmsn mforce wyrenthoth mgoto 18749
mat wyrenthoth mforce wyrenthoth get chia
mat wyrenthoth mforce wyrenthoth hold chia
end
halt
end
~
#2051
Sacerdote di Therion 2 (Nimrhuin)~
0 j 100
~
if %object.vnum% == 2056
 wait 1 s
 say Hm, huum...
 wait 2 s
 say Questa e' ormai solo una volgare Pietra, non interessa Therion.
 set missione 0
 global missione
 halt
elseif %object.vnum% != 2055
 halt
end
if %actor.name% != %pg%
 wait 2 s
 say Huum, hm. Therion aveva dichiarato il suo volere a %actor.name%, e non a te.
 set missione 0
 global missione
 halt
end
wait 2 s
say Huum, hm.
wait 1 s
emo annuisce lentamente e una strana espressione di gioia appare su quello che diresti essere il suo volto.
wait 1 s
say Therion si compiace di avere un cosi' valido seguace, difensore dell'ordine della natura.
wait 1 s
say Per questo ha deciso di ricompensarti.
set missione 0
global missione
mat xxxxxx drop all
if %actor.level% < 20
 set livello 15
elseif %actor.name% < 40
 set livello 35
else
 set livello 55
end
eval moltip %random.10%
eval divisore %random.10%
eval premio %livello%*10
eval premio %premio%*%moltip%
eval premio %premio%/%divisore%
global premio
if %premio% <= 400
 mload obj 2047
 give premio %pg%
 posa premio
 wait 1 s
 bow %pg%
 halt
elseif %premio% <= 800
 mload obj 2048
 give premio %pg%
 posa premio
 bow %pg%
 halt
elseif %premio% <= 950
 mload obj 2049
 give premio %pg%
 posa premio
 wait 1 s
 bow %pg%
 halt
else
 mload obj 2050
 give premio %pg%
 posa premio
 bow %pg%
 halt
end
~
#2052
Sacerdote di Therion 3 (Nimrhuin)~
0 g 100
~
if %direction% == west
 wait 2 s
 emo muove lentamente un ramo verso di te.
 wait 2 s
 mecho &6&xUna voce possente ti accoglie, 'Salve, %actor.name%. Therion ti da' il benvenuto.'. Comprendi essere la voce del Suo Sacerdote.&0
end
~
#2053
Altare di Therion 1 (Nimrhuin)~
0 d 100
ristoro illusione~
wait 1 s
if %speech% == ristoro di Therion
 if %puntigrazia% < 20
  mecho &6&xL'altare inizia ad illuminarsi, ma non succede nulla.&0
  halt
 end
 mecho &6&xL'altare emette una luce flebile ma decisa e le piante che lo circondano germogliano con vigore.&0
 wait 1 s
 mecho &6&xPiante ed animali d'ogni tipo ti circondano sussurrandoti parole di dolce riposo.&0
 dg_cast 'cure critic' %actor.name%
 dg_cast 'cure critic' %actor.name%
elseif %speech% == illusione di Therion
 if %puntigrazia% < 40
  mecho &6&xL'altare inizia ad illuminarsi, ma non succede nulla&0
  halt
 end
 mecho &6&xL'altare emette una luce flebile ma decisa e le piante che lo circondano germogliano con vigore.&0
 wait 1 s
 mecho &6&xChiudendo gli occhi senti il potere di Therion scorrere nel tuo sangue. Riaprendoli vedi molte immagini in tutto uguali a te.&0
 dg_affect %actor.name% mirrorimage 0 6
end
~
#2054
Altare di Therion 2 (Nimrhuin)~
0 d 100
altare~
set valido 0
if %actor.name% == krog || %actor.name% == sheep
 set valido 1
end
if %valido% == 1
 if %speech% == altare di Therion
  wait 1 s
  eval cap4 %cap%/4
  eval cap2 %cap%/2
  eval cap3 %cap%*3
  eval cap5 %cap3%/4
  if %puntifede% <= %cap4%
   msend %actor.name% &6&xL'altare di Therion brilla appena d'una luce tenue.&0
  elseif %puntifede% <= %cap2%
   msend %actor.name% &6&xL'altare di Therion brilla con decisione e le piante che lo attanagliano iniziano a germogliare.&0
  elseif %puntifede% <= %cap5%
   msend %actor.name% &6&xL'altare di Therion brilla molto intensamente e le piante che su di esso si appoggiano germogliano con vigore.&0
  else
   msend %actor.name% &6&xL'altare di Therion emana una luce abbagliante. Le piante che lo circondano germogliano e danno abbondanti frutti.&0
  end
 end
end
~
#2055
Altare di Therion 3 (Nimrhuin)~
0 d 100
veste~
set valido 0
if %actor.name% == krog || %actor.name% == sheep
 set valido 1
end
if %valido% == 1 && %speech% == veste di Therion
 wait 1 s
 if %puntifede% < 1500
  mecho &6&xL'altare inizia ad illuminarsi, ma non succede nulla&0
  halt
 end
 mecho &6&xL'altare emette una luce flebile ma decisa e le piante che lo circondano germogliano con vigore.&0
 mload obj 2057
 drop veste
 return 0
 eval puntifede %puntifede% - 1500
 global puntifede
 mat cptifedetherion say distruggi 30
end
~
#2056
Premi Shaarr (Sheep)~
0 j 100
~
if %object.vnum% == 559
 wait 1 s
 emo sibila frasi di profondo sdegno
 wait 2 s
 say Credevi di prenderti gioco di Shaarr stupido %actor.name%? Ella ti punira'!
 set missione 0
 global missione
 halt
elseif %object.vnum% != 558
 halt
end
if %actor.name% != %pg%
 wait 2 s
 say La missione fu affidata a %pg%, Shaarr non ti concedera' alcuna ricompensa %actor.name%!    
 set missione 0
 global missione
 halt
end
wait 2 s
emo sogghigna soppesando il cuore...
wait 2 s
say Vedo che il nostro nemico ha assaggiato l'ira di Shaarr per mano tua, %pg%.
wait 1 s
say Shaarr ha deciso di ricompensarti per la tua dedizione a Lei!
wait 1 s
set missione 0
global missione
mat xxxxxx drop all
if %actor.level% < 20
 set livello 15
elseif %actor.level% < 40
 set livello 35
else
 set livello 55
end
eval moltip %random.10%
eval divisore %random.10%
eval premio %livello%*10
eval premio %premio%*%moltip%
eval premio %premio%/%divisore%
global premio
if %premio% <= 400
 mload obj 2039
 give teschio %pg%
 posa teschio
 wait 1 s
 bow %pg%
 halt
elseif %premio% <= 800
 mload obj 2040
 give teschio %pg%
 posa teschio
 wait 1 s
 bow %pg%
 halt
elseif %premio% <= 950
 mload obj 2041
 give teschio %pg%
 posa teschio
 wait 1 s
 bow %pg%
 halt
else
 mload obj 2042
 give teschio %pg%
 posa teschio
 wait 1 s
 bow %pg%
 halt
end
~
#2057
Altare di Shaarr 1 (Sheep)~
0 d 100
altare~
set valido 0if %actor.name% == mcfly || %actor.name% == cabal || %actor.name% == knives || %actor.name% == tetsuo
set valido 1
end
if %actor.name% == elrik || %actor.name% == crystin || %actor.name% == nion || %actor.name% == darkshade
set valido 1
end
if %valido% == 1
if %speech% == altare di shaarr
wait 1 s
eval cap4 %cap%/4
eval cap2 %cap%/2
eval cap3 %cap%*3
eval cap5 %cap3%/4
if %puntifede% <= %cap4%
msend %actor.name% &1&xDall'altare di Shaarr sgorga solo un piccolo rigagnolo di sangue...&0
elseif %puntifede% <= %cap2%
msend %actor.name% &1&xDall'altare si Shaarr sgorga un intenso fiotto di sangue...&0
elseif %puntifede% <= %cap5%
msend %actor.name% &1&xDall'altare di Shaarr fluisce un possente getto di sangue...&0
else
msend %actor.name% &1&xUna maestosa fontana di sangue prorompe dall'altare di Shaarr&0
end
end
end
~
#2058
Altare di Shaarr 2 (Sheep)~
0 d 1
vita fuoco~
wait 1 s
if %speech% == vita di Shaarr
if %puntigrazia% < 20
mecho &1&xL'altare inizia ad illuminarsi, ma non succede nulla&0
halt
end
mecho &1&xL'altare si illumina ed il flusso di sangue aumenta...&0
wait 1 s
mecho &1&xUn flusso di sangue fluisce dall'altare alla tua bocca!&0
dg_cast 'cure critic' %actor.name%
dg_cast 'cure critic' %actor.name%
elseif %speech% == fuoco di Shaarr
if %puntigrazia% < 40
mecho &1&xL'altare inizia ad illuminarsi, ma non succede nulla&0
halt
end
mecho &1&xUn fiotto di sangue si trasforma in fuoco mentre ti avvolge....&0
wait 1 s
mecho &1&x...il Fuoco Di Shaarr ora ti protegge!&0
dg_affect %actor.name% fireshd 0 4
end
~
#2059
Altare di Shaarr 3 (Sheep)~
0 d 100
sangue~
set valido 0if %actor.name% == mcfly || %actor.name% == cabal || %actor.name% == knives || %actor.name% == tetsuo
set valido 1
end
if %actor.name% == elrik || %actor.name% == crystin || %actor.name% == nion || %actor.name% == darkshade
set valido 1
end
if %valido% == 1
if %speech% == sangue di Shaarr
wait 1 s
if %puntifede% < 400
mecho &1&xL'altare inizia ad illuminarsi, ma non succede nulla&0
halt
end
mecho &1&xUn getto di sangue si alza fino alla volta per poi scendere e infilarsi in alcune fiale!&0
mload obj 568
mload obj 568
mload obj 568
drop all.poz
eval puntifede %puntifede%-400
global puntifede
mat cptifedeshaarr say distruggi 8
end
end
~
#2060
Altare di Shaarr 4 (Sheep)~
0 d 100
furia~
set valido 0if %actor.name% == mcfly || %actor.name% == cabal || %actor.name% == knives || %actor.name% == tetsuo
set valido 1
end
if %actor.name% == elrik || %actor.name% == crystin || %actor.name% == nion || %actor.name% == darkshade
set valido 1
end
if %valido% == 1
if %speech% == furia di Shaarr
wait 1 s
if %puntifede% < 1000
mecho &1&xL'altare inizia ad illuminarsi, ma non succede nulla&0
halt
end
mecho &1&xUn enorme getto di sangue fuoriesce dall'Altare e da forma ad una Furia!&0
wait 1 s
mload mob 585
wait 1 s
mforce fmaxs say Shaarr mi manda per assisterti nella tua battaglia per la Sua Gloria
wait 2
mforce %actor.name% say tu ora mi seguirai, in nome di %god%
eval puntifede %puntifede%-1000
global puntifede
mat cptifedeshaarr say distruggi 20
end
end
~
#2061
Altare di Shaarr 5 (Sheep)~
0 d 100
artigli~
set valido 0if %actor.name% == mcfly || %actor.name% == cabal || %actor.name% == knives || %actor.name% == tetsuo
set valido 1
end
if %actor.name% == elrik || %actor.name% == crystin || %actor.name% == nion || %actor.name% == darkshade
set valido 1
end
if %valido% == 1
if %speech% == artigli di Shaarr
wait 1
if %puntifede% < 1500
mecho &1&xL'altare inizia ad illuminarsi, ma non succede nulla&0
halt
end
mecho &1&xUn fiotto si sangue assume la forma di un paio di spaventosi artigli!&0
mload obj 569
drop artigli
return 0
eval puntifede %puntifede%-1500
global puntifede
mat cptifedeshaarr say distruggi 30
end
end
~
#2062
Altare di Shaarr 6 (Sheep)~
0 d 100
preghiere~
if %speech% == preghiere
msend %actor.name% Ecco le preghiere disponibili ai fedeli di Shaarr:
msend %actor.name% ---------------------------------------------------
msend %actor.name% vita di Shaarr
msend %actor.name% fuoco di Shaarr
msend %actor.name% ---------------------------------------------------
msend %actor.name% ---------------------------------------------------
msend %actor.name% Ecco le preghiere riservate agli adepti del culto
msend %actor.name% ---------------------------------------------------
msend %actor.name% altare di Shaarr
msend %actor.name% sangue di Shaarr
msend %actor.name% furia di Shaarr
msend %actor.name% artigli di Shaarr
msend %actor.name% ---------------------------------------------------
msend %actor.name% ---------------------------------------------------
msend %actor.name% Ecco le preghiere riservate al capo del culto
msend %actor.name% ---------------------------------------------------
msend %actor.name% vittoria di Shaarr
end
~
#2063
Altare di Therion 4 (Nimrhuin)~
0 d 100
siero~
set valido 0
if %actor.name% == krog || %actor.name% == sheep
 set valido 1
end
if %valido% == 1 && %speech% == Siero di vita
 wait 1 s
 if %puntifede% < 400
  mecho &6&xL'altare inizia ad illuminarsi, ma non succede nulla&0
  halt
 end
 mecho &6&xL'altare emette una luce flebile ma decisa e le piante che lo circondano germogliano con vigore.&0
 mload obj 2060
 mload obj 2060
 mload obj 2060
posa all.siero
 eval puntifede %puntifede% - 400
 global puntifede
 mat cptifedetherion say distruggi 8
end
~
#2064
Altare di Therion 5 (Nimrhuin)~
0 d 100
elementale~
set valido 0
if %actor.name% == krog || %actor.name% == sheep
 set valido 1
end
if %valido% == 1 && %speech% == Elementale di pietra
 wait 1 s
 if %puntifede% < 1000
  mecho &6&xL'altare inizia ad illuminarsi, ma non succede nulla&0
  halt
 end
 mecho &6&xL'altare emette una luce flebile ma decisa e le piante che lo circondano germogliano con vigore.&0
 mload mob 2050
 mforce elementale say &5Therion mi invia in tuo aiuto, %actor.name%&0
wait 2
mforce %actor.name% say tu ora mi seguirai, in nome di %god%
 eval puntifede %puntifede%-1000
 global puntifede
 mat cptifedetherion say distruggi 20
end
~
#2065
Altare di Therion 6 (Nimrhuin)~
0 d 100
preghiere~
if %speech% == preghiere
 msend %actor.name% Ecco le preghiere disponibili ai fedeli di Therion:
 msend %actor.name% ---------------------------------------------------
 msend %actor.name% ristoro di Therion
 msend %actor.name% illusione di Therion
 msend %actor.name% ---------------------------------------------------
 msend %actor.name% ---------------------------------------------------
 msend %actor.name% Ecco le preghiere riservate agli adepti del culto
 msend %actor.name% ---------------------------------------------------
 msend %actor.name% altare di Therion
 msend %actor.name% siero di vita
 msend %actor.name% elementale di pietra
msend %actor.name% veste di Therion
msend %actor.name% ---------------------------------------------------
msend %actor.name% ---------------------------------------------------
msend %actor.name% Ecco le preghiere riservate al capo del culto
msend %actor.name% ---------------------------------------------------
msend %actor.name% purificazione di Therion
end
~
#2078
Avviso di quest (Nimrhuin)~
2 g 100
~
wait 1
switch %self.vnum%
 case 779
 wsend piagolo &6&x%actor.name% si e' qualificato con il rank di &7Iniziato&0 di &6&xTherion&0
 break
 case 780
 wsend piagolo &6&x%actor.name% si e' qualificato con il rank di &2Druido&0 di &6&xTherion&0
 break
 case 781
 wsend piagolo &6&x%actor.name% si e' qualificato con il rank di &0Arcidruido di &6&xTherion&0
 break
 case 782
 wsend piagolo &6&x%actor.name% si e' qualificato con il rank di &4&xGuardiano della Vita&0
 break
 case 783
 wsend piagolo &6&x%actor.name% si e' qualificato con il rank di Ramingo&0
 break
done
~
#2079
Echo papale (Nimrhuin)~
1 c 7
echop~
if %actor.name% == Sagat || %actor.name% == Alhandir || %actor.name% == McFly || %actor.name% == piagolo || %actor.name% == Nimrhuin
 %echo% %arg%
end
~
#2080
Entrata nelle risposte di Therion (Nimrhuin)~
2 g 100
~
wait 1 s
if %self.vnum% == 796
 wecho Prendi la borsa perche' la tua fame ti assilla.
 wecho Ma il corpo non puo' dominare la volonta'. 
 wecho Mi dispiace, non sei adatto a diventare un degno Fedele di Therion
elseif %self.vnum% == 797
 wecho Avvisi l'uomo della perdita che ha subito ed egli ti ringrazia offrendoti un
 wecho pasto caldo, un letto tranquillo e dei nuovi vestiti.  La tua scelta e' stata
 wecho quella dell'onesta', puoi accedere alla prossima prova.
elseif %self.vnum% == 794
 wecho Non appena cogli il fiore esso pianta le sue radici nel tuo cuore
 wecho trasformandoti in un suo burattino.  Therion punisce sempre l'avidita' degli
 wecho uomini.  Tuttavia egli decide nella sua immensa saggezza di liberarti dalle
 wecho catene del fiore.  Sei ora libero di andare.
elseif %self.vnum% == 793
 wecho Con attenzione e delicatezza liberi il terreno intorno alla magica pianticella.
 wecho Quando hai finito osservi allibito un mutamento pazzesco.  Il fiore emana un
 wecho bagliore possente mentre la sua aura magica abbraccia l'intera foresta.  Ora
 wecho che e' libero dai propri vincoli il potere della pianta assorbe tutta la vita
 wecho intorno a te, e moriresti se non fosse che qualcuno ti teletrasporta altrove.
 wecho La tua scelta non e' stata saggia, ma almeno hai dimostrato di non essere avido
 wecho di potere.
elseif %self.vnum% == 792
 wecho Scelta saggia, quella di non turbare l'equilibrio.  Se la Natura ha deciso che
 wecho il fiore deve morire, evidentemente esso e' piu' pericoloso di quello che
 wecho credi.  Cio' che Therion decide per le sue creature e' sempre cosa buona.  Puoi
 wecho accedere alla prossima prova.
elseif %self.vnum% == 790
 wecho Un animo caritatevole e misericordioso e' la prova della tua fedelta'.
elseif %self.vnum% == 789
 wecho La magia non puo' sostituire il potere della Natura.  Studiarla non ti sarebbe
 wecho di alcun aiuto.
elseif %self.vnum% == 788
 wecho La magia non puo' sostituire il potere della Natura.  Tieni a mente questa
 wecho lezione mentre affronti la Quarta Prova
elseif %self.vnum% == 786
 wecho Il tuo colpo rabbioso stacca la testa del coboldo che muore all'istante.  Le
 wecho provocazioni del coboldo erano pesanti, ma un fedele di Therion deve possedere il
 wecho dominio di se'.
elseif %self.vnum% == 785
 wecho Come puo' uno stupido coboldo mettere in atto simili minacce?  Anche un
 wecho ragazzotto armato di bastone potrebbe fargli del male.  E' solo un pallone
 wecho gonfiato, non vale neanche la pena di sporcare la tua arma.  Hai scelto
 wecho saggiamente.  Il dominio di se nasce dalla consapevolezza, non dalla forza di
 wecho volonta'.
elseif %self.vnum% == 784
 wecho La tua forza di volonta' ti trattiene per qualche istante, ma appena il coboldo
 wecho ti sputa in faccia per l'ennesima volta non riesci a trattenerti e lo uccidi
 wecho con un solo colpo.  Il dominio di se non nasce dalla volonta', ma dalla
 wecho consapevolezza.  Come avrebbe potuto uno stupido coboldo mettere in atto le
 wecho proprie minacce?  Anche un ragazzotto armato di bastone lo avrebbe potuto
 wecho mettere al tappeto.
end
~
#2081
Uscita stanze prova Therion (Nimrhuin)~
2 g 100
~
wait 3 s
if %self.vnum% == 798
 eval c1 %random.4%
 eval c2 %random.3%
 if %c1% <= %c2%
  eval c2 %c2% + 1
 end
 switch %c1%
 case 1
  set uscita1 north
 break
 case 2
  set uscita1 east
 break
 case 3
  set uscita1 south
 break
 case 4
  set uscita1 west
 break
 done
 switch %c2%
 case 1
  set uscita2 north
 break
 case 2
  set uscita2 east
 break
 case 3
  set uscita2 south
 break
 case 4
  set uscita2 west
 break
 done
 %door% 798 %uscita1% room 797
 %door% 798 %uscita2% room 796
else
 eval c1 %random.4%
 eval c2 %random.3%
 eval c3 %random.2%
 if %c1% <= %c2%
  eval c2 %c2% + 1
 end
 if %c1% <= %c3%
  eval c3 %c3% + 1
 end
 if %c2% <= %c3%
  eval c3 %c3% + 1
 end
 switch %c1%
 case 1
  set uscita1 north
 break
 case 2
  set uscita1 east
 break
 case 3
  set uscita1 south
 break
 case 4
  set uscita1 west
 break
 done
 switch %c2%
 case 1
  set uscita2 north
 break
 case 2
  set uscita2 east
 break
 case 3
  set uscita2 south
 break
 case 4
  set uscita2 west
 break
 done
 switch %c3%
 case 1
  set uscita3 north
 break
 case 2
  set uscita3 east
 break
 case 3
  set uscita3 south
 break
 case 4
  set uscita3 west
 break
 done
 eval nuscita1 %self.vnum% - 1
 eval nuscita2 %self.vnum% - 2
 eval nuscita3 %self.vnum% - 3
 %door% %self.vnum% %uscita1% room %nuscita1%
 %door% %self.vnum% %uscita2% room %nuscita2%
 %door% %self.vnum% %uscita3% room %nuscita3%
end
wait 1
%force% all look
wait 60 s
if %actor.room% == %self.vnum%
 wecho &1L'indecisione e' una brutta cosa.&0
 wait 1 s
 if %self.vnum% == 798
  wecho &1Un ladruncolo si accorge della borsa e la ruba.&0
  wait 1
  teleport %actor.name% 779
 elseif %self.vnum% == 795
  wecho &1Il fiore consuma ogni cosa nella stanza, ma Therion nella sua infinita bonta' decide di salvarti.&0
  wait 1
  teleport %actor.name% 779
 elseif %self.vnum% == 791
  wecho &1Il dolore porta il mostriciattolo alla pazzia facendolo fuggire nelle grinfie del Caos.&0
  wait 1
  wteleport %actor.name% 780
 else
  wecho &1Il coboldo, stanco delle parole, attacca battaglia. Gli stacchi la testa con un colpo netto.&0
  wait 1
  wteleport %actor.name% 781
 end
end
%door% %self.vnum% north purge
%door% %self.vnum% east purge
%door% %self.vnum% south purge
%door% %self.vnum% west purge
~
$~
