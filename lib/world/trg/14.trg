#1404
Bauletto caramelle (Nimrhuin)~
1 g 100
~
wait 1 s
osend %actor.name% &7&xSenti un vapore fluire dal bauletto...&0
~
#1405
Eterno Errante (1) (Sheep)~
0 n 100
~
mload obj 3541
indo mantello
~
#1406
Eterno Errante (2) (Sheep)~
0 f 100
~
return 0
mecho &1&xNon appena l'Eterno Errante si accascia al suolo subito si rialza riprendendendo il suo eterno vagare&0
mload mob 538
~
#1407
Imp Erratico (Sheep)~
0 n 100
~
mpurge hkyo
wait 1 s
mpurge cadavere
mload mob 537
mgoto 28150
say purgami
~
#1408
Caronos (1) (Sheep)~
0 n 100
~
mload obj 3543
indo occhio
wait 2 s
mecho &1&xCaronos emette un ruggito tanto terrificante da atterrire qualsiasi cosa nel raggio di molte miglia!&0
~
#1409
Caronos (2) (Sheep)~
0 k 10
~
mecho &1&xDalle ferite inferte a Caronos nascono nuovi mostri!&0
wait 1 s
mload mob 540
~
#1410
Caronos (3) (Sheep)~
0 i 100
~
mecho &1&xIl terrificante rumore prodotto dai passi di Caronos ti trasforma le gambe in gelatina...&0
~
#1411
Caronos (4) (Sheep)~
0 f 100
~
mecho &1&xLo schianto del corpo di Caronos che crolla a terra fa tremare tutto per chilometri...&0
return 0
mload mob 540
mload mob 540
mload mob 540
mload mob 540
~
#1412
Inno Sendar (Sheep)~
0 b 5
~
emote intona con voce soave ma ferma l'inno di Sendaria...
wait 1 s
say Li sento cantare
wait 1 s
say con tutta la voce
wait 1 s
say i padri di gloria antica
wait 1 s
say ora il sangue non scorre piu'
wait 1 s
say non abbiamo piu' invasori
wait 1 s
say e i figli dei padri
wait 1 s
say ora hanno una casa
wait 1 s
say e una liberta' nuova
wait 1 s
say per tornare a sperare
wait 1 s
say E i figli dei figli
wait 1 s
say ora stanno tranquilli
wait 1 s
say che nessuno li puo' disturbare
wait 1 s
say E tutto questo canto
wait 1 s
say genera una luce possente
wait 1 s
say mia guida nel buio
wait 1 s
say Non ho piu' bisogno di fuggire alla notte
wait 1 s
say perche' senza Luna o Sole
wait 1 s
say avro' sempre una luce
wait 1 s
say verso cui navigare
wait 1 s
say Mia madre e bambina
wait 1 s
say Mia regina e contadina
wait 1 s
say E' sole nei campi
wait 1 s
say E' fuoco d'inverno
wait 1 s
say E' la mia casa
wait 1 s
say Lo e' ora
wait 1 s
say E in eterno!
wait 2 s
~
#1413
Amuleto tigre (1) (Sheep)~
1 j 100
~
if %tigre% == 1
wait 1 s
osend %actor.name% Hai gia' avuto la tua tigre.. l'hai persa?
halt
end
set tigre 1
global tigre
wait 1 s
osend %actor.name% Non appena indossi l'amuleto senti una possente presenza accanto a te...
oechoaround %actor.name% &3&xUna possente tigre si materializza al fianco di %actor.name%&0
wait 1 s
oload mob 542
oforce ttttk foll %actor.name%
~
#1414
Amuleto Tigre (2) (Sheep)~
1 b 3
~
if %tigre% == 0
halt
end
set tigre 0
global tigre
oecho &3&xLa tigre ritorna nell'amuleto!&0
opurge ttttk
~
#1415
Tigre battaglia (Sheep)~
0 n 100
~
eval tigre %random.20%
if %tigre% == 1
wait 2 s
mpurge kttt
mecho &3&xIl medaglione della tigre si frantuma!&0
end
wait 2 s
emote emette un ruggito di pura potenza....
~
#1416
Amuleto Lupo (1) (Sheep)~
1 j 100
~
if %lupo% == 1
wait 1 s
osend %actor.name% Hai gia' avuto il tuo lupo.. l'hai perso?
halt
end
set lupo 1
global lupo
wait 1 s
osend %actor.name% Non appena indossi l'amuleto senti una strana presenza accanto a te...
oechoaround %actor.name% &9&xUn magnifico lupo grigio si materializza al fianco di %actor.name%&0
wait 1 s
oload mob 543
oforce llllp foll %actor.name%
~
#1417
Amuleto Lupo (2) (Sheep)~
1 b 2
~
if %lupo% == 0
halt
end
set lupo 0
global lupo
oecho &9&xIl lupo ritorna nell'amuleto!&0
opurge llllp
~
#1418
Lupo grigio (Sheep)~
0 n 100
~
eval lupo %random.20%
if %lupo% == 1
wait 2 s
mpurge pllll
mecho &9&xIl medaglione del lupo si frantuma!&0
end
wait 2 s
emote ulula tanto forte da far ghiacciare il sangue nelle vene...
~
#1419
Amuleto Orso (1) (Sheep)~
1 j 100
~
if %orso% == 1
wait 1 s
osend %actor.name% Hai gia' avuto il tuo orso.. l'hai perso?
halt
end
set orso 1
global orso
wait 1 s
osend %actor.name% Non appena indossi l'amuleto senti una enorme presenza accanto a te...
oechoaround %actor.name% &7&xUn enorme orso bianco si materializza al fianco di %actor.name%&0
wait 1 s
oload mob 544
oforce oooop foll %actor.name%
~
#1420
Amuleto Orso (2) (Sheep)~
1 b 5
~
if %orso% == 0
halt
end
set orso 0
global orso
oecho &7&xL'orso ritorna nell'amuleto!&0
opurge oooop
~
#1421
Orso bianco (Sheep)~
0 n 100
~
eval orso %random.100%
if %orso% == 1
wait 2 s
mpurge poooo
mecho &7&xIl medaglione dell'orso si frantuma!&0
end
wait 2 s
emote scuote l'aria con un verso profondo e potente
~
#1422
Amuleto Pantera (1) (Sheep)~
1 j 100
~
if %pantera% == 1
wait 1 s
osend %actor.name% Hai gia' avuto la tua pantera.. l'hai persa?
halt
end
set pantera 1
global pantera
wait 1 s
osend %actor.name% Non appena indossi l'amuleto senti una inquietante presenza accanto a te...
oechoaround %actor.name% &9&xUna conturbante pantera si materializza al fianco di %actor.name%&0
wait 1 s
oload mob 545
oforce ppppp foll %actor.name%
~
#1423
Amuleto Pantera (2) (Sheep)~
1 b 2
~
if %pantera% == 0
halt
end
set pantera 0
global pantera
oecho &9&xLa pantera ritorna nell'amuleto!&0
opurge ppppp
~
#1424
Pantera nera (Sheep)~
0 n 100
~
eval pantera %random.20%
if %pantera% == 1
wait 2 s
mpurge apppp
mecho &9&xIl medaglione della pantera si frantuma!&0
end
wait 2 s
emote ruggisce con rabbia...
~
#1425
Amuleto aquila (1) (Sheep)~
1 j 100
~
if %aquila% == 1
wait 1 s
osend %actor.name% Hai gia' avuto la tua aquila.. l'hai persa?
halt
end
set aquila 1
global aquila
wait 1 s
osend %actor.name% Non appena indossi l'amuleto senti un'ombra sopra di te...
oechoaround %actor.name% &7Un'aquila enorme prende a volteggiare sopra il capo di %actor.name%&0
wait 1 s
oload mob 546
oforce aaaap foll %actor.name%
~
#1426
Amuleto aquila (2) (Sheep)~
1 b 5
~
if %aquila% == 0
halt
end
set aquila 0
global aquila
oecho &7L'aquila ritorna nell'amuleto!&0
opurge aaaap
~
#1427
Aquila bianca (Sheep)~
0 n 100
~
eval aquila %random.15%
if %aquila% == 1
wait 2 s
mpurge paaaa
mecho &7Il medaglione dell'aquila si frantuma!&0
end
wait 2 s
emote volteggia maestosamente qui sopra...
~
#1428
Runa rossa (Sheep)~
1 j 100
~
set runa 1
global runa
wait 1 s
return 0
oechoaround %actor.name% &1%actor.name% afferra la runa che scompare mentre viene avvolto da un magico muro di fuoco&0
osend %actor.name% &1Non appena afferri la runa vieni avvolto da un magico muro di fuoco&0
dg_affect %actor.name% fireshd 0 2
wait 1 s
osend %actor.name% &1&xLa Runa diventa cosi' incandescente che sei costretto a posarla!&0
oload mob 547
oforce %actor.name% rimu runa
oforce %actor.name% posa runa
~
#1429
Imp rosso (Sheep)~
0 n 100
~
wait 1
mecho &1&xLa Runa viene consumata dal fuoco...&0
mpurge rrrrp
mgoto 28150
say purgami
~
#1430
Runa Bianca (Sheep)~
1 j 100
~
set runa 1
global runa
wait 1 s
return 0
oechoaround %actor.name% &7%actor.name% afferra la runa che scompare mentre viene avvolto da una luce divina&0
osend %actor.name% &7Non appena afferri la runa vieni avvolto da una luce divina&0
dg_affect %actor.name% sanct 0 2
wait 1 s
osend %actor.name% &1&xLa Runa diventa cosi' incandescente che sei costretto a posarla!&0
oload mob 548
oforce %actor.name% rimu runa
oforce %actor.name% posa runa
~
#1431
Imp bianco (Sheep)~
0 n 100
~
wait 1
mecho &1&xLa Runa viene consumata dal fuoco...&0
mpurge rbrbp
mgoto 28150
say purgami
~
#1432
Runa azzurra (Sheep)~
1 j 100
~
set runa 1
global runa
wait 1 s
return 0
oechoaround %actor.name% &6%actor.name% afferra la runa che scompare mentre pare spostarsi qualche passo in la'&0
osend %actor.name% &6Non appena afferri la runa ti senti differente&0
dg_affect %actor.name% blink 0 2
wait 1 s
osend %actor.name% &1&xLa Runa diventa cosi' incandescente che sei costretto a posarla!&0
oload mob 549
oforce %actor.name% rimu runa
oforce %actor.name% posa runa
~
#1433
Imp azzurro (Sheep)~
0 n 100
~
wait 1
mecho &1&xLa Runa viene consumata dal fuoco...&0
mpurge rarap
mgoto 28150
say purgami
~
#1434
Runa verde (Sheep)~
1 j 100
~
set runa 1
global runa
wait 1 s
return 0
oechoaround %actor.name% %actor.name% afferra la runa che scompare mentre la sua immagine di divide
osend %actor.name% Non appena afferri la runa la tua immagine si divide
dg_affect %actor.name% mirrorimage 0 2
wait 1 s
osend %actor.name% &1&xLa Runa diventa cosi' incandescente che sei costretto a posarla!&0
oload mob 550
oforce %actor.name% rimu runa
oforce %actor.name% posa runa
~
#1435
Imp verde (Sheep)~
0 n 100
~
wait 1
mecho &1&xLa Runa viene consumata dal fuoco...&0
mpurge rvrvp
mgoto 28150
say purgami
~
#1436
Runa di protezione (Sheep)~
1 j 100
~
set runa 1
global runa
wait 1 s
return 0
oechoaround %actor.name% %actor.name% &9&xafferra la runa che scompare mentre viene avvolto da protezioni magiche&0
osend %actor.name% &9&xNon appena afferri la runa ti senti protetto da potenti magie&0
dg_affect %actor.name% mirrorimage 0 2
dg_affect %actor.name% fireshd 0 2
dg_affect %actor.name% blink 0 2
dg_affect %actor.name% sanct 0 2
wait 1 s
osend %actor.name% &1&xLa Runa diventa cosi' incandescente che sei costretto a posarla!&0
oload mob 551
oforce %actor.name% rimu runa
oforce %actor.name% posa runa
~
#1437
Imp nero (Sheep)~
0 n 100
~
wait 1
mecho &1&xLa Runa viene consumata dal fuoco...&0
mpurge rprpp
mgoto 28150
say purgami
~
#1438
Caronos (5) (Sheep)~
0 b 20
~
dg_cast 'fire breath'
~
#1439
Sogotacon (1) (Sheep)~
0 n 100
~
mload obj 3554
indo tentacolo
wait 1 s
mecho &4&xSogotacon lancia un grido che lacera il tempo e lo spazio facendo piombare tutto intorno a se' nel piu' nero terrore&0
~
#1440
Sogotacon (2) (Sheep)~
0 k 10
~
mecho &4&xI tentacoli che recidi dal corpo di Sogotacon continuano a vivere!&0
wait 1 s
mload mob 553
~
#1441
Sogatocon (3) (Sheep)~
0 i 100
~
mecho &4&xIl rumore di Sogatocon che avanza distruggendo tutto e' come quello di mille uragani...&0
~
#1442
Sogatocon (4) (Sheep)~
0 f 100
~
mecho &4&xIl rombo di un miliardo di tuoni accompagna infine l'accasciarsi di Sogatocon a terra...&0
return 0
mload mob 553
mload mob 553
mload mob 553
mload mob 553
~
#1443
Sogatocon (5) (Sheep)~
0 b 20
~
dg_cast 'frost breath'
~
#1444
Posa runa (Sheep)~
1 g 100
~
if %runa% != 1
halt
end
wait 1
osend %actor.name% &1&xLa runa e' incandescente! Non puoi toccarla!&0
oforce %actor.name% posa runa
~
#1445
Nyrugantep (1) (Sheep)~
0 n 100
~
mload obj 3555
indo esoscheletro
wait 1 s
mecho &2&xLa venuta di Nyrugantep non puo' significare altro che distruzione e terrore!&0
~
#1446
Nyrugantep (2) (Sheep)~
0 k 3
~
mecho &2&xDalle immonde fauci Nyrugantep lancia una schifosa sostanza appicicosa avvolgendoti completamente!&0
dg_cast 'paralize' %actor.name%
~
#1447
Nyrugantep (3) (Sheep)~
0 b 40
~
eval nyru %random.5%
if %nyru% == 1
mecho &2&xNyrugantep depone un orrendo uovo gelatinoso...&0
mload mob 555
halt
end
dg_cast 'acid breath'
~
#1448
Nyrugantep (uovo) (Sheep)~
0 ab 5
~
mecho &2&xL'uovo di Nyrugantep si schiude liberando una delle sue orribili progenie!&0
mload mob 556
mload obj 3556
mgoto 28150
say purgami
~
#1449
Nyrugantep (4) (Sheep)~
0 f 100
~
return 0
mecho &2&xL'urto con l'enorme massa di Nyrugantep che si accascia al suolo sconfitto fa tremare le fondamenta della terra!&0
~
#1450
Nyrugantep (uovo2) (Sheep)~
0 f 100
~
return 0
mecho &2&xL'immondo uovo va in mille pezzi liberando una nube d'acido!&0
dg_cast 'acid breath'
mload mob 557
~
#1451
Nyrugantep (5) (Sheep)~
0 i 100
~
mecho &2&xLa distruzione e l'orrore portata dai movimenti di Nyrugantep e' come quella di mille terremoti&0
~
#1452
Imp Necrofago (Sheep)~
0 n 100
~
wait 1
mpurge cadavere
mgoto 28150
say purgami
~
#1453
Tornado magico (4) (Sheep)~
0 f 100
~
return 0
mecho &6&xIl tornado magico si dissolve nell'aria intorno a te...&0
mload mob 557
~
#1454
Il flagello dei gremlins (Sheep)~
1 c 1
ecranac~
wait 1 s
oecho &3&xUn'ondata di energia luminosa si sprigiona dall'anello e parte in cerca di un Gremlin da distruggere&0
opurge gremlin
wait 1 s
osend %actor.name% Ti senti un po' fiacco...
odamage %actor.name% 50
~
#1455
Il flagello dei gremlins (2) (Sheep)~
1 c 1
canarce~
wait 1 s
eval sfg %random.10%
if %sfg% == 1
oecho &1&xUna strana energia magica fuoriesce dall'anello e crea un Gremlin!&0
oload mob 522
elseif %sfg% == 2 || %sfg% == 4
osend %actor.name% L'energia che esce dall'anello ti sopraffa' uccidendoti!
oechoaround %actor.name% &1&xUn'ondata di energia fuoriesce dall'anello di %actor.name%, uccidendolo&0
wait 1 s
odamage %actor.name% 3000
else
oecho &1&xIl Flagello dei Gremlins sprigiona qualche scintilla e nulla piu'...&0
end
~
#1456
Gallina uova d'oro (Sheep)~
0 ab 10
~
emote starnazza un po' e poi depone un uovo completamente d'oro!
mload obj 3558
wait 1 s
posa uovo
~
#1457
Lama del Caos (Sheep x Zarkon)~
1 j 100
~
wait 1 s
if %actor.name% != artemis
osend %actor.name% &1&xVieni distrutto dalla tua stessa follia&0
oechoaround %actor.name% &9&xLa Lama del Caos ingoia nei neri abissi l'anima di %actor.name%&0
wait 1 s
odamage %actor.name% 5000
halt
end
wait 1 s
oechoaround artemis &9&xViene circondato dall'Oscura Volonta' dei Grandi Antichi&0
osend artemis &9&xSenti l'Oscuro potere degli insondabili abissi del Caos in te&0
~
#1458
Lama del Caos (2) (Sheep x Zarkon)~
1 c 1
fuggi~
oechoaround artemis &9&xArtemis scompare inghiottito dall'Oscurita'&0
osend artemis &7&xLe pecore sono davvero molto belle&0
oteleport artemis 12
~
#1459
Pallina matta (1) (Sheep)~
1 g 100
~
wait 1
osend %actor.name% Non resisti alla tentazione di lanciare la pallina matta!
oechoaround %actor.name% %actor.name% lancia la pallina matta per terra con forza!
wait 1
oforce %actor.name% posa pallina
~
#1460
Pallina matta (2)  (Sheep)~
1 h 100
~
wait 1
oecho La pallina matta comincia a rimbalzare qui intorno come impazzita!
~
#1461
Amuleto Amplificante (Sheep)~
1 j 100
~
if %actor.vnum% != -1
wait 1
odamage %player% 3000
halt
end
if %actor.level% < 40
wait 1 s
oechoaround %actor.name% &1%actor.name% viene circondato da un'aura rossa&0
osend %actor.name% &1Senti una nuova potenza vivere in te&0
halt
else
wait 1 s
oechoaround %actor.name% &1&xLa testa di %actor.name% esplode in mille pezzi!&0
osend %actor.name% &1&xLa tua potenza ora e' troppo grande! La tua mente non sa contenerla!&0
wait 1
odamage %actor.name% 4000
halt
~
#1462
Amuleto Amplificante (2) (Sheep)~
1 i 100
~
if %actor.vnum% != -1
halt
end
set player %actor.name%
global player
~
#1463
Amuleto Amplificante (3) (Sheep)~
1 h 100
~
if %actor.vnum% != -1
halt
end
set player %actor.name%
global player
~
#1464
Collana di Perle di Rugiada~
1 j 100
~
wait 1 s
%send% %actor.name% &7Il tuo corpo viene avvolto dalle mistiche nebbie di Avalon,donandoti poteri straordinari!!&0
%echoaround% %actor.name% &7%actor.name%&0 viene avvolto dalle eteree nebbie di Avalon..
~
#1465
Nebbia (Sheep x Rashan)~
1 c 4
avalon~
if %actor.name% != moel
wait 1 s
osend %actor.name% &7&xVieni avvolto dalle nebbie...&0
wait 1 s
osend %actor.name% &7&xLe nebbie si dissolvono pian piano...&0
halt
end
wait 1 s
osend %actor.name% &7&xVieni avvolta dalle nebbie...&0
oteleport %actor.name% 593
halt
~
#1466
Talpa Scappa (Sheep)~
0 ab 3
~
mecho &9&xLa talpa scava una buca nel terreno e scompare velocemente!&0
mload obj 3561
mgoto 28150
say purgami
~
#1467
Buca Talpe (Sheep)~
1 b 2
~
oecho &9&xUna enorme talpa fuoriesce dalla buca!&0
oload mob 563
~
#1468
Talpa (2) (Sheep)~
0 n 100
~
eval purgabuca %random.100%
if %purgabuca% <= 85
wait 1
mecho &9&xAlla fuoriuscita della talpa la buca collassa richiudendosi&0
mpurge bbbbt
end
eval oggetto %random.10%
if %oggetto% == 1
mload obj 532
indo antic
elseif %oggetto% == 2
mload obj 533
indo antic
end
~
#1469
Talpa (3) (Sheep)~
0 k 10
~
mecho &9&xLa talpa si fa prendere dal panico e scompare sottoterra scavando velocissima&0
mload obj 3561
mgoto 28150
say purgami
~
#1470
Amuleto della Vita (Sheep)~
1 j 100
~
wait 2
osend %actor.name% &2&xNon appena indossi l'amuleto senti una enorme energia vitale pervadere il tuo corpo&0
oechoaround %actor.name% &2&x%actor.name% pare ringiovanire a vista d'occhio!&0
~
#1471
Pianta della Vita (1) (Sheep)~
0 n 100
~
return 0
wait 1
mpurge cadavere
if %seme% == 1
halt
end
mload obj 522
hold seme
~
#1472
Pianta della Vita (2) (Sheep)~
0 f 100
~
mforce mtxat mkill %actor.name%
return 0
mecho &2&xNon pare che sia possibile uccidere la pianta della vita...&0
mload mob 564
eval gg %random.10%
if %gg% == 1
mecho &6Un enorme guardiano delle foreste sorge dalla terra per difendere la Pianta sacra a Therion&0
mload mob 312
mforce mtxat kill %actor.name%
else
halt
end
~
#1473
Pianta della vita (3) (Sheep)~
0 c 100
cogli~
if %seme% == 1
halt
end
return 0
wait 1 s
mecho &6&xUna lieve brezza soffia inclinando per qualche istante la pianticella...&0
wait 1 s
rimu seme
dai seme %actor.name%
posa seme
set seme 1
global seme
~
#1474
Verme Anziano Dormiglione (1) (Sheep)~
0 b 5
~
emote pare cosi' stanco...
wait 1 s
emote si sdraia con un gran tonfo e comincia a dormire...
wait 1 s
dormi
~
#1475
Miga (Sheep)~
0 l 40
~
mecho &9&xMiga spruzza una nube di nero inchiostro e fugge!&0
dg_cast 'peace' %actor.name%
dg_cast 'blind' %actor.name%
eval fuga %random.4%
if %fuga% == 1
mgoto 2095262
elseif %fuga% == 2
mgoto 2095263
elseif %fuga% == 3
mgoto 2095362
elseif %fuga% == 4
mgoto 2095363
end
dg_cast 'heal'
dg_cast 'heal'
dg_cast 'heal'
~
#1476
Medaglione Lanower (1) (Sheep)~
1 j 100
~
wait 2
osend %actor.name% Il medaglione comincia ad assorbire la tua energia vitale per tramutarla in energia magica!
set pg %actor.name%
global pg
set wear 1
global wear
wait 2
osend %actor.name% Capisci che l'unico modo per interrompere il processo e' appoggiare per qualche istante l'oggetto a terra
~
#1477
Medaglione Lanower (2) (Sheep)~
1 b 100
~
if %wear% != 1
halt
end
odamage %pg% 10
~
#1478
Medaglione Lanower (3) (Sheep)~
1 h 100
~
wait 1 s
osend %actor.name% Il collegamento tra te ed il medaglione e' rotto: ora non tramutera' piu' la tua forza vitale in magia
set wear 0
global wear
~
#1479
Calza Befana (Sheep)~
1 j 100
~
wait 2
oecho &3&xUna Dolce Pioggia di Caramelle ti sommerge...&0
oload obj 1441
oload obj 1442
oload obj 1443
oload obj 1444
oload obj 1445
oload obj 1446
oload obj 1447
~
#1480
Passanti (Sheep)~
0 g 50
~
if %actor.vnum% != -1
 halt
end
if %actor.clan% == 7
 wait 1 s
 emote si guarda intorno preoccupato... evidentemente la tua presenza qui lo innervosisce.
 halt
end
if %actor.rank% == 30 && %actor.clan% == 8
 wait 1 s
 emote Accenna un timido sorriso al Governatore di Sendaria, mentre prova a trattenere il respiro.
 halt
end
wait 1 s
sor %actor.name%
~
#1481
Guardie Tribunale Chyrellos (Sheep)~
0 g 100
~
if %actor.clan% != 2
halt
end
wait 2
switch %actor.rank%
case 30
emote scatta sull'attenti!
wait 1 s
if %direction% == east
say Benvenuto Sire Dolmant!
else
say Arrivederci Sire Dolmant!
end
break
case 27
emote scatta sull'attenti!
wait 1 s
if %direction% == east
say Benvenuto Siniscalco!
else
say Arrivederci Siniscalco!
end
break
case 18
emote scatta sull'attenti!
wait 1 s
if %direction% == east
say Benvenuto Giudice!
else
say Arrivederci Giudice!
end
break   
case 13
emote scatta sull'attenti!
wait 1 s
if %direction% == east
say Benvenuto Inquisitore!
else
say Arrivederci Inquisitore!
end
break
default
halt
break
done
~
#1482
Chiave di Dolmant (Sheep)~
1 g 100
~
wait 5
osend %actor.name% Non appena tocchi la chiave, essa si dissolve in un mucchietto di polvere.
otransform 536
~
#1483
Guardie Cattedrale (1) (Sheep)~
0 g 100
~
if %direction% == south && %actor.name% == dolmant
unset controllo
unset ospite
halt
end
if %direction% == south
unset ospite
halt
end
wait 5
switch %actor.name%
case dolmant
emote scatta sull'attenti!
wait 2 s
say Agli ordini Maesta'!
set controllo 1
global controllo
break
default
set ospite %actor.name%
global ospite
if %controllo% == 1
wait 2 s
say Benvenuto %actor.name%
wait 2 s
say Ora il vostro arrivo sara' annunciato al Re.
mat 22232 mecho &7&x%actor.name% vuole essere ricevuto dal Re, dobbiamo ammetterlo alla Vostra presenza?&0
else
wait 2 s
say Sua Maesta Re Dolmant III non e' presente al momento, siete pregato di allontanarvi.
wait 5
emote ti guarda con sguardo gelido... io fossi in te non la farei arrabbiare.
end
break
done
~
#1484
Guardie Cattedrale (2) (Sheep)~
0 d 100
guardie~
if %actor.name% != dolmant
if %provocazione% == 0
wait 1 s
look %actor.name%
wait 1 s
say %actor.name% noi riceviamo ordini solo da Sire Dolmant.
wait 1 s
say Vi consigliamo di non riprovare ad osare di darci ordini.
set provocazione 1
global provocazione
else
wait 1 s
say %actor.name%, siete gia' stato avvisato di non osare darci ordini...
wait 1 s
say Avete dieci secondi di tempo per andarvene, o saremo costretti a neutralizzarvi.
wait 1 s
emote Porta la mano alla spada e la sguaina&0
wait 9 s
unset provocazione
mkill %actor.name%
end
halt
end
if %speech% == guardie aprite
set controllo 1
global controllo
wait 5
unlock porta
apri porta
wait 10 s
chiudi porta
lock porta
end
~
#1485
Guardie Cattedrale (3) (Sheep)~
2 d 100
aprite~
if %actor.name% != dolmant
halt
end
wait 1 s
wforce andromeda sor dolmant
wait 1 s
wforce andromeda say Arrivederci Sire
wait 1 s
wforce andromeda mat 22230 mforce 2.gcd unlock porta
wforce andromeda mat 22230 mforce 2.gcd apri porta
wait 10 s
wforce andromeda mat 22230 mforce 2.gcd chiudi porta
wforce andromeda mat 22230 mforce 2.gcd lock porta
~
#1486
Andromeda (Sheep)~
0 g 100
~
if %actor.name% == dolmant
wait 1 s
inch dolmant
halt
end
if %direction% == north
wait 1 s
inch %actor.name%
wait 1 s
say Benvenuto %actor.name%, Re Dolmant vi sta aspettando.
else
wait 1 s
inch %actor.name%
wait 1 s
say Arrivederci, %actor.name%
end
~
#1487
Guardie Dolmant (1)~
0 g 100
~
if %actor.vnum% == 572
fol self
mforce gdd fol self
wait 2 s
mforce gdd inch dolmant
inch dolmant
wait 2 s
say A presto Maesta'
wait 1 s
mforce gdd s
s
set redolmant 0
global redolmant
halt
end
if %actor.name% == dolmant
wait 1 s
mecho Le guardie scattano sull'attenti e vanno a posizionarsi alle spalle del Re
set redolmant 1
global redolmant
wait 3 s
follow dolmant
mforce 2.gdd follow dolmant
halt
end
if redolmant == 1
halt
else
halt
wait 1 s
say Voi non potete stare qui in assenza del Re!
wait 1 s
emote sguaina la spada!
say %actor.name%, avete dieci secondi per andarvene, dopodiche' vi abbatteremo.
wait 10 s
mkill %actor.name%
end
~
#1488
Dominio di Dolmant (1) (Sheep)~
2 c 100
accetta~
if %actor.name% != dolmant
halt
end
%force% dolmant emote prende un campanello di cristallo dalla scrivania e lo fa squillare una volta
%force% gdd mat gcd mecho &7Il limpido squillo di un campanello trilla&0
wait 1 s
%force% gdd mat gcd mforce 2.gcd sor
wait 2 s
%force% gdd mat gcd mforce gcd say Re Dolmant vi attende, entrate pure %ospite%
wait 1 s
%force% gdd mat gcd mforce 2.gcd unlock porta
%force% gdd mat gcd mforce 2.gcd apri porta
wait 15 s
%force% gdd mat gcd mforce 2.gcd chiudi porta
%force% gdd mat gcd mforce 2.gcd lock porta
~
#1489
Dominio di Dolmant (2) (Sheep)~
2 c 100
attendi~
if %actor.name% != dolmant
halt
end
wforce dolmant emote prende un campanello di cristallo dalla scrivania e lo fa squillare due volte
wforce gdd mat gcd mecho &7&xIl limpido squillo di un campanello trilla un paio di volte&0
wait 2 s
wforce gdd mat gcd mforce gcd say Re Dolmant vi prega di attendere qui qualche minuto.
wait 1 s
wforce gdd mat gcd mforce gcd say Mettetevi pure comodo, Sire Dolmant vi ricevera' appena possibile.
~
#1490
Dominio Dolmant (3) (Sheep)~
2 c 100
caccia~
if %actor.name% != dolmant
halt
end
wforce dolmant emote prende un campanello di cristallo dalla scrivania e lo fa squillare tre volte
wforce gdd mat gcd mecho &7&xIl limpido squillo di un campanello trilla per tre volte&0
wait 2 s
wforce gdd mat gcd mforce gcd say Re Dolmant non intende ricevervi %ospite%
wait 2 s
wforce gdd mat gcd mforce gcd say Siete pregato di andarvene
~
#1491
Dominio di Dolmant (4) (Sheep)~
2 c 100
aprite~
if %actor.name% != dolmant
halt
end
wforce dolmant emote prende un campanello di cristallo dalla scrivania e lo fa trillare
wforce gdd mat gcd mforce 2.gcd unlock porta
wforce gdd mat gcd mforce 2.gcd apri porta
wait 20 s
wforce gdd mat gcd mforce 2.gcd chiudi porta
wforce gdd mat gcd mforce 2.gcd lock porta
~
#1492
Dominio di Dolmant (5) Sheep~
0 c 100
piazza~
if %actor.name% != dolmant
halt
end
mecho &7&xDolmant viene avvolto da un'abbagliante luce bianca e scompare!&0
mteleport dolmant 21208
set redolmant 0
global redolmant
fol self
mforce 2.gdd fol self
~
#1493
Dominio di Dolmant (6) Sheep~
0 c 100
fuga~
if %actor.name% != dolmant
halt
end
mecho &7&xDolmant viene avvolto da un'abbagliante luce bianca e scompare!&0
mteleport dolmant 21678
set redolmant 0
global redolmant
fol self
mforce 2.gdd fol self
~
#1494
Dominio di Dolmant (7) (Sheep)~
0 c 100
botola~
if %arg.room% != 22232
halt
end
if %actor.name% != dolmant
halt
end
mforce dolmant emote fa un gesto ad una delle guardie...
wait 1 s
emote sogghigna...
wait 1 s
emote tira una leva nascosta..
wait 2
mechoaround %arg% Una botola si apre sotto i piedi di %arg% che cade di sotto!
msend %arg% Il terreno viene a mancarti sotto i piedi! Si e' aperta improvvisamente una botola!
wait 1
msend %arg% Cadi per molto tempo!
mteleport %arg% 21208
~
#1495
Dominio di Dolmant (8) (Sheep)~
2 c 100
opzioni~
if %actor.name% != dolmant
halt
end
wsend dolmant accetta >>>>> Serve per far entrare qualcuno quando ti arriva l'avviso che qualcuno aspetta
wsend dolmant attendi >>>>> Serve per far attendere qualcuno quando ti arriva l'avviso che qualcuno aspetta
wsend dolmant caccia >>>>> Serve per far cacciare qualcuno quando ti arriva l'avviso che qualcuno aspetta
wsend dolmant aprite >>>>> Serve per far aprire la porta, quando un tuo ospite se ne deve andare
wsend dolmant piazza >>>>> Vai in piazza
wsend dolmant fuga >>>>> Fuggi in un posto segreto
wsend dolmant botola x >>>>> Sostituisci la x con il nome di chi ti sta rompendo le scatole, e divertiti
~
#1496
Diamante Magico (Sheep)~
1 g 100
~
otransform 538
~
#1497
Polvere nera (Sheep)~
1 h 100
~
otransform 537
~
#1498
Testa drow (Sheep)~
1 gi 100
~
if %actor.name% == cabal
wait 2 s
osend cabal Lo sguardo ghignante di questa testa ti riporta alla mente l'incubo della prigionia...
wait 2 s
osend cabal La risata della Matrona rimbomba nella tua mente...
end
if %victim.name% == cabal
wait 2 s
osend cabal Lo sguardo ghignante di questa testa ti riporta alla mente l'incubo della prigionia...
wait 2 s
osend cabal La risata della Matrona rimbomba nella tua mente...
end
~
#1499
Altare Silue (Sheep)~
1 c 4
benedizione~
wait 3 s
if %actor.name% != alhandir
halt
end
osend alhandir &7&xSenti il potere della Luce sorgere nella tua mente, e fluire attraverso le tue mani&0
wait 2 s
oechoaround %arg% &7&x%arg% viene circondato dalla pura Luce accecante di Silue&0
osend %arg% &7&xOra la Luce di Silue e' con te e non devi piu' temere nulla&0
dg_affect %arg% sanct 0 5
~
$~
