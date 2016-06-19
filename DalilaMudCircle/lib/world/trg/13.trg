#1300
lama nera (Faymond)~
1 gh 100
~
* Lista comandi
oecho &9La lama ondeggia nell'aria e rimane ritta in verticale sul terreno!!&0
~
#1301
Fiore di loto (faymond)~
1 g 100
~
* Lista comandi
wait 3s
oecho &4Il fiore si dissolve nell'aria come polvere di stelle...!&0
opurge fiore
~
#1302
Stallone prova (Faymond)~
0 e 0
night sfiora lo stallone che svanisce nel nulla~
* Lista comandi
hit night
~
#1303
Accesso magazzini aldar (Nimrhuin)~
2 d 100
ciliegghe~
* Lista comandi
wait 1 s
wsend %actor.name% &1&xImprovvisamente ti ritrovi in un altro luogo, circondato da beni di grande valore.&0
wteleport %actor.name% 2822
wait 1
wforce %actor.name% look
~
#1310
Maiale (faymond)~
0 e 0
ti abbraccia~
* Lista comandi
wait 4 s
say OINK!! OINK!! :))) OINK!!!!!
hit salome
~
#1311
Cintura di meditazione nera~
1 c 1
fulmina medita ritorno~
* Lista comandi
if %cmd% == fulmina
%echo% &3%actor.name% si siede in meditazione.. un fulmine cade dal cielo colpendoti in pieno!&0
elseif %cmd% == medita
%echo% &5%actor.name% incrocia le gambe e chiude gli occhi.. le sue ferite si richiudono!&0
elseif %cmd% == ritorno
%echo% &7%actor.name% si trasforma in nuvola e sparisce nel vento!!&0
else 
end
~
#1312
new trigger~
0 n 100
~
* Lista comandi
wait 4 s
%teleport% 1sec 20400
~
#1320
Mercenario (faymond)~
0 g 100
~
* Lista comandi
if %charmato% != on
wait 2 s
emote ti guarda con sospetto.
wait 1 s
say Ehi tu... Combatti con me! 
wait 2 s
 : saltella in segno di sfida.
wait 3 s
say Avanti! Fatti sotto! Se sarai in grado di abbattermi la mia spada sara' al tuo servizio!
else
end
~
#1321
Mercenario2 (Faymond)~
0 l 30
~
* Lista comandi
set charmato on
global charmato
dg_cast 'peace' %actor.name%
emote si getta ai tuoi piedi disperato
wait 2 s
segui %actor.name%
dg_cast 'peace' %actor.name%
wait 2 s
say ora sei tu il mio padrone.. per sempre!
detach mob me all
end
~
#1330
Avatar Dioniso 2~
0 n 100
~
* Lista comandi
wait 30 s
%echo% &7L'immagine di Dioniso svanisce nell'aria come un soffio di fumo!&0
mgoto 28150
~
#1331
&1&xCaricamento mercenario&0 (Nimrhuin)~
0 n 100
~
* Lista comandi
wait 1
set cambianome on
global cambianome
set appostato off
global appostato
~
#1332
&1&xPagamento venditore&0 (Nimrhuin)~
0 m 100
~
* Lista comandi
wait 1 s
say Grazie!
wait 5
eval pagamento %amount% + %pagamento%
global pagamento
say Con i soldi che mi hai dato ti puoi permettere:
wait 1 s
if %pagamento% >= 5000
 say Un bellissimo imp mercenario!
 wait 1
else
 say Niente, spilorcio!
 wait 5
 say Dammi altri soldi!
end
~
#1333
&1&xAcquisto imp&0 (Nimrhuin)~
0 d 100
compraimp~
* Lista comandi
if %pagamento% >= 5000
 wait 1 s
 say Ottima scelta!
 wait 1
 say Ecco a lei un magnifico imp mercenario, signor %actor.name%!
 wait 1
 mload mob 1418
 wait 2
 pat %actor.name%
 mforce imp fol %actor.name%
 wait 1
 mforce imp say Ai suoi comandi, padrone!
 eval pagamento %pagamento% - 5000
else
 emo si guarda nelle tasche...
 wait 1 s
 say Ehi!
 wait 1
 say Non provare a fregarmi, non mi hai dato abbastanza soldi!
end
~
#1334
&1&xImp seguimi&0 (Nimrhuin)~
0 d 100
seguimi~
* Lista comandi
if %actor.name% == %proprietario%
 wait 1
 say Sissignore!
 wait 1
 fol %proprietario%
 wait 1
 say Ai suoi comandi, padrone!
else
 wait 1
say Seguiro' solo gli ordini del mio padrone, impostore!
end
~
#1335
&1&xCaricamento mercenario 2&0 (Nimrhuin)~
0 e 100
simpatia~
* Lista comandi
wait 1
if %actor.name% == hornegar
 set proprietario %victim.name%
 global proprietario
 wait 1
 set cambianome off
 global cambianome
end
~
#1336
&1&xResto&0 (Nimrhuin)~
0 d 100
resto~
* Lista comandi
if %pagamento% != 0
 wait 1 s
 say Oh, certo
 wait 1 s
 say Ecco a lei.
 give %pagamento% coins %actor.name%
 eval pagamento 0
 global pagamento
else
 say Non ho resto da darle!
end
~
#1337
&1&xPostazione&0 (Nimrhuin)~
0 d 100
postazione~
* Lista comandi
if %actor.name% == %proprietario%
 wait 1 s
 say Molto bene
 set postazione %self.room%
 global postazione
 set appostato on
 global appostato
end
~
#1338
&1&xRitorno&0 (Nimrhuin)~
0 d 100
riposo~
* Lista comandi
if %actor.name% == %proprietario%
 wait 1 s
 emo scatta sull'attenti, si inchina e se ne va.
 fol self
 mteleport self %postazione%
end
~
#1349
Brollo (Faymond)~
0 d 1
fame cibo mangiare cibarmi appetito pizza~
* Lista comandi
wait 1 s
say Ma certo... corro a vedere se c'e' qualcosa in cucina per voi...
wait 1 s
unlock port u
apri port u
up
get pizza
down
wait 1 s
close port u
lock port u
wait 1 
say Ecco una pizza fumante!!
dai pizza %actor.name%
~
#1350
Uscita cella Necrarchia (Faymond)~
2 c 100
1sparisci~
* Lista comandi
%send% %actor% &1..Il tuo corpo svanisce lentamente portandoti fuori dall'incubo...&0
%echoaround% %actor% &1%actor.name% svanisce nell'aria con una risata!!&0
wait 1 s
wteleport %actor% 1381
end
~
#1351
Liberati dalla necrarchia!~
2 c 100
1espelli~
* Lista comandi
%send% %actor% &1..Decidi di liberare i tuoi prigionieri..&0
%echoaround% %actor% &1%actor.name% sogghigna ed esegue un cerchio con le mani... prima di sparire.&0
wteleport %actor% 1381
wait 2 s
%echo% &7Tutto inizia a girare vorticosamente...&0
wait 2 s
%echo% &7L'incubo si spegne improvvisamente, ti ritrovi all'esterno del tempio col fiatone.&0
wteleport all 1579484
~
#1352
Araldo necrarchi~
0 g 100
~
* Lista comandi
if %actor.name% == Nagash
emote si inchina profondamente
say Salve, mio eccelso padrone!
else
wait 1 s
emote trema come se fosse scosso con vigore.
wait 2 s
say  Lontano da qui mortali: triste e' il destino di chi osa sfidare i necrarchi!
else
end
~
#1353
&3Cattiveria Dracolich&0 (Nimrhuin)~
0 ab 1
~
* Lista comandi
wait 1
echo &0&xL'anima fiera del drago si agita nel corpo non morto...&0
wait 1 s
dg_cast 'acid breath' %proprietario%
~
#1354
&3Evocazione dracolich&0 (Nimrhuin)~
0 n 100
~
* Lista comandi
wait 1
set cambianome on
global cambianome
~
#1355
&3Vendita bestiole&0 (Nimrhuin)~
0 e 100
simpatia~
* Lista comandi
wait 2
if %cambianome% == on
 set proprietario %victim.name%
 global proprietario
 set cambianome off
 global cambianome
end
~
#1356
&3Venditore bestiole&0 (Nimrhuin)~
0 d 100
lista~
* Lista comandi
wait 1 s
emo si guarda attorno furtivamente...
wait 1 s
say Ehi, amico, guarda qua che bestie di prima qualita'...
wait 1
if %dracolich% > 0
 wait 1 s
 say dammi 5000 monete, e questo splendido dracolich sara' disponibile, solo per te! E' un'occasione rara trovare una simile bestia domata in vendita!
end
if %cammelli% > 0
 wait 1 s
 say Se proprio non hai voglia di spendere ti posso sempre dare uno di questi cammelli per sole 2000 monete...
end
wait 2 s
say La consegna viene effettuata a pagamento avvenuto...
wait 1 s
emo ti guarda biecamente...
~
#1357
&3Venditore bestiole 2&0 (Nimrhuin)~
0 m 2000
~
* Lista comandi
wait 1 s
if %amount% >= 5000 && %dracolich% >= 1
 say Un intenditore!
 mload mob 1421
 wait 1
 pat %actor.name%
 wait 1 s
 say Credimi, e' un vero affare...
 say Fai solo attenzione ai suoi... scherzi
 wait 1 s
 emo anticipa ogni tua parola e ti mostra l'enorme drago non morto.
 eval dracolich %dracolich% - 1
 global dracolich
else
 say Un mezzo sicuro per le traversate nel deserto...
 wait 1 s
 say Non e' forse quello che tutti desiderano in questi luoghi?
 wait 1 s
 mload mob 1422
 wait 1
 pat %actor.name%
 emo ti porge le briglia del camelide con un sorriso equivoco in faccia.
 eval cammelli %cammelli% - 1
 global cammelli
end
~
#1358
&3Venditore bestiole 3&0 (Nimrhuin)~
0 n 100
~
* Lista comandi
wait 1
eval dracolich 1
global dracolich
eval cammelli 5
global cammelli
~
#1368
Campanaccio (fay)~
1 c 1
suona~
* Lista comandi
wait 1 s
%echo% &3Il campanaccio di %actor.name% suona impazzito annunciando.. la fine del mondo!&0
~
#1370
Chitarra (Faymond)~
1 j 100
0~
* Lista comandi
set punto %random.6%
 global punto
wait 1 s
if %punto% == 1 
oecho &4Dolci note vibrano dalla chitarra di %actor.name% quando la pizzica con delicatezza!&0
elseif %punto% == 2
oecho &7%actor.name% accarezza le corde della sua chitarra in un giro musicale perfetto!&0
elseif %punto% == 3
oecho &4%actor.name% suona una nota, ahime' stonata, ma riprende subito il ritmo!&0
elseif %punto% == 4
oecho &5%actor.name% sfiora la sua chitarra in una melodia sopraffina!&0
elseif %punto% == 5
oecho &6La chitarra di %actor.name% suona soave e delicata...&0
elseif %punto% == 6
oecho &7%actor.name% suona con vigore la sua canzone.. avvolgente..&0
else
end
~
#1371
Prova (Nimrhuin)~
0 d 100
cambiami~
* Lista comandi
wait 1 s
set actor.hit 20
set actor.name UnPgDiProva
set actor.sex 1
set actor.fantasma off
~
#1380
Morte Avatar (Fay)~
0 f 100
~
* Lista comandi
emo si dissolve istantaneamente nell'aria!
mgoto 28150
~
#1382
anello tera ivellios (Fay)~
1 j 100
~
* Lista comandi
wait 1 s
if %actor.name% == Ivellios
%echo% &7L'anello splende al dito di Ivellios brillando del suo amore per Tera&0
elseif %actor.name% == Tera
%echo% &3L'anello avvolge il dito di Tera in un abbraccio.. l'amore per Ivellios lo fa brillare&0
else
end
~
#1383
Avatar necrarchi~
2 c 100
evocazione~
* Lista comandi
wait 1 s
if %actor.name% == Nagash
%teleport% avnagash 1381
wpurge avnagash
%echo% &7Una nebbia si vorma vorticosamente attorno a Nagash circondandolo!&0
wload mob 1384
%force% avatar segui nagash
elseif %actor.name% == Dioniso
%teleport% avdionis 1381
wpurge avdionis
%echo% &1L'ombra di Dioniso si contorce e scatta impazzita!&0
wload mob 1383
%force% avatar segui dioniso
elseif %actor.name% == Destiny
%teleport% avdestiny 1381
wpurge avdestiny 
wload mob 1385
%echo% &5Dalla schiena di Destiny emergono enormi, orridi tentacoli vivi!&0
%force% avata kill destiny
else
end
~
#1384
Avatar Nagash~
0 c 100
1fuggi~
* Lista comandi
if %actor.name% == Nagash
wait 1 s
%echo% &7Improvvisamente la nebbia intorno a Nagash vortica su se stessa come un tifone!&0
%echo% &7In uno scuro baleno schizza nel cielo portandolo con lei!&0
%teleport% nagash 5460
%teleport% nebbia 5460
else
end
~
#1385
Avatar Destiny 1~
0 k 100
~
* Lista comandi
wait 1 s
%send% %actor% &5Uno dei tentacoli di Destiny si scaglia su di te e penetra le tue carni!&0
%echoaround% %actor% &5%actor.name% viene colpito in pieno da uno dei tentacoli di Destiny!&0
dg_cast 'energy drain' %actor.name%
~
#1386
Avatar Dioniso~
0 c 100
3clone~
* Lista comandi
if %actor.name% == Dioniso
wait 1 s
eval morteav %random.8%
if %morteav% == 8
%echo% &7L'ombra di Dioniso si scioglie in se stessa prima di riuscire a clonarlo!&0
mgoto 28150
else
%load% mob 1368
%echo% &7L'ombra di Dioniso si accende.. dalle sue forme prende vita una copia del Necrarca!&0
else
end
~
#1387
Zombie (Faymond)~
0 n 100
~
* Lista comandi
wait 2 s 
emote arranca grottescamente e crolla in ginocchio sulle membra tumefatte.
wait 1 s
emote china il capo ciondolante ai piedi del nuovo padrone!
~
#1388
MINOTAURO (Faymond)~
0 n 100
~
* Lista comandi
wait 1 s
say WHoOooooo.... 
wait 2 s
emote agita il grosso cranio e alza il braccio muscoloso!
~
#1389
Troll (Faymond)~
0 n 100
~
* Lista comandi
wait 2 s
set punto %random.4%
if %punto% == 1
say GwhaR.. aglHi OrhdIGni SghiGNoRE!!!
elseif %punto% == 2
say GhuT?! SNorRT!!!
elseif %punto% == 3
say FghhhhH!!!!! IO UCCCIDERHA PehR TE!
elseif %punto% == 4
say MiHO PAhDRONE......
else
end
~
#1390
PROVALOAD (Faymond)~
0 n 100
~
* Lista comandi
wait 1 s
emote soffia al cielo in segno di saluto!
wait 1 s
emote ti osserva impavido con i piccoli occhi di fuoco.
~
#1391
Cancelli di Roma (faymond)~
2 gn 100
~
* Lista comandi
wait 2 s 
%echo% &1E' terribile!! All'orizzonte al posto dell'antica citta' c'e' qualcosa di mostruoso!&0
~
#1392
Load~
0 n 100
~
* Lista comandi
set mobbus %random.3%
global mobbus
mobbus = mobbus - 1
wait 1 s
mload mob 165%mobbus%
%force% 1sec follow %actor.name%
%teleport% 2sec 20400
~
#1393
Ristorante Lamorka (Faymond)~
2 d 1
peperoni melanzane pizza pasta~
* Lista comandi
wait 1 s
if %speech% == peperoni
    if %actor.gold% >= 400
    %force% %actor% give 400 coins viviana
%echo% Viviana la cuoca sorride e serve delle gustose cotolette di peperoni!
%load% obj 1855
     else
%echo% Viviana la cuoca guarda %actor.name% perplessa... non ha abbastanza soldi!
end
elseif %speech% == pizza
     if %actor.gold% >= 300
    %force% %actor% give 400 coins viviana
%echo% Viviana la cuoca sforna una pizza fumante e la porge a %actor.name% con un sorriso!
%load% obj 1856
else
%echo% Viviana la cuoca scuote la testa... devi poter pagare la sua cucina!
end
elseif %speech% == melanzane
   if %actor.gold% >= 500
   %force% %actor% give 500 coins viviana
%echo% Viviana la cuoca estrae dal forno le sue speciali melanzane!
%load% obj 1854
else
%echo% Viviana protesta... non lavora gratis!!
end
elseif %speech% == pasta
if %actor.gold% >= 400
%force% %actor% give 400 coins viviana
%echo% Viviana prende il denaro e mette la pasta sul fuoco.. pochi minuti ed e' servita!!&0
%load% obj 1858
else
%echo% Viviana molla un sonoro ceffone a %actor.name%.. i manicaretti si pagano!
end
else
end
* Fine trigger
~
#1394
Fresie magiche~
1 j 100
~
* Lista comandi
wait 1 s
%send% %actor% &7Gli splendidi fiori brillano dentro di te ricordandoti che qualcuno ti pensa&0
~
#1396
Artefatto Esplode x Faymod (by Phantom)~
0 f 100
~
* Lista comandi
emo &1&xEsplode in mille pezzi!!!&0
mload obj 1309
mgoto 28150
~
#1397
flauto magico~
1 c 1
suona~
* Lista comandi
wait 2 s
%send% %actor% &7Le splendide note del flauto scivolano nell'aria sospinte dal tuo soffio leggero..&0
%echoaround%  %actor% &7Il suono di un flauto ti accarezza le orecchie...magico...&0
%load% mob 31113
wait 1 s
%force% 1sec tell faymond TI STO CERCANDOOOOOOOOOOOOO
opurge 1sec
~
#1398
Isola dei sogni~
0 c 0
ritorna~
* Lista comandi
wait 1 s
emote sorride ad %actor.name%... per lui e' giunto il momento di tornare alla realta'!!!
wait 1 s
mteleport %actor.name% 12500
~
#1399
prot!!!~
1 c 1
mangia~
* Lista comandi
wait 1 
%echo% &7PROOOOOOOOOOOOOOOOOOOOOOT!!!&0
~
$~
