#31301
EVOCAZIONI PANDION~
1 j 100
~
set mostro 4



































set base 20295
wait 2 s
%echo% ti concentri per richiamare un mostro da dimensioni parallele
set punto %random.4%
eval danno %actor.level% * 5
eval mostro %base% + %punto%
oload mob %mostro%
%damage% %actor% %danno%
~
#31303
Djinni (Faymond)~
1 j 100
~
wait 2 s




































oecho \&1\FRrrrrrrrrRrrrrrrrrrrrr...... L'anello al dito di %actor.name% comincia ad eruttare vapore!!\&0
oload mob 31303
wait 2 s
otransform 2569
oecho \&7\Dal fumo prende improvvisamente forma un vorticoso, potente spirito dell'aria!!\&0
%force% djinn foll %actor.name%
end
~
#31304
Arrivo pg ai Drow (Nimrhuin)~
2 g 100
~
if %actor.vnum% == -1























 if %direction% == west
  wait 1 s
  wforce guardia say Ehi tu! Dove credi di andare? Queste caverne non sono luogo di visite!
  wait 1 s
  wforce guardia say Sei per caso un amico di quei nani che piu' volte han provato a prendere i nostri giacimenti di adamianto?
  wforce guardia emo ti punta la spada alla gola.
  wait 1 s
  wforce guardia Forza rispondi! Si o no! Scegli! Morte o vita!
 end
else end
end
~
#31305
Risposta pg guardia (Nimrhuin)~
2 d 100
si si' no~
if %speech% == si || %speech% == si'























 wait 1 s
 wforce guardia say Morirai!!
 wait 1 s
 wforce guardia kill %actor.name%
elseif %speech% == no
 wait 2 s
 wforce guardia say Non ho troppi motivi per ucciderti... anche se il tuo sangue pare piacere a quelle driader!
 wforce guardia say Attento ti tengo d'occhio... al tuo primo errore...
 wait 1 s
 wforce guardia emo mostra la propria abilita' con la spada.
end
~
#31306
Guardie Drow (Nimrhuin)~
0 e 1
guarda.~
wait 1 s























say Ehi che hai da guardare?
wait 2 s
say Attento ci potresti rimettere la vita a disturbare i Drow...
wait 1 s
say E sei fortunato che io sono arruolato... quelle driadi ti salterebbero addosso per sentire il sapore del tuo sangue!
~
#31307
Guerriero Drow (Nimrhuin)~
0 e 100
ti guarda.~
wait 2 s

































say Ehi tu! Che hai da guardare?
wait 1 s
say Non mi piacciono i tipi curiosi... sta' attento!
wait 1 s
say Ma questa volta ti e' andata bene... mi serve uno curioso come te
wait 2 s
say Dimmi, sei interessato a svolgere una missione d'esplorazione per me?
wait 3 s
emo ti guarda con occhio bieco.
wait 1 s
say forza rispondi!
~
#31308
Risposta pg Guerriero (Nimrhuin)~
0 d 100
si si' no~
if %speech% == si || %speech% == si'

































 wait 2 s
 say Bene, ascoltami.
 wait 1 s
 say Noi Drow non siamo esploratori... e questo lo puoi notare in ogni nostra azione.
 wait 3 s
 say Ma raramente qualcuno nasce con quest'insana idea in mente...
 emo sospira profondamente.
 wait 4 s
 say Tra di essi, purtroppo, c'era mio figlio...
 wait 2 s
 say Quel dissennato, se lo avessi tra le mie mani...
 emo stringe in modo spaventoso la spada sibilando.
 wait 4 s
 say Una volta e' partito, girovagando, verso il nord, e lassu', verso la cima della montagna.
 wait 1 s
 say E non e' piu' tornato.
 wait 5 s
 say Benche' siano passati ormai qualche giorno ha il sangue di suo padre, e so che e' vivo.
 wait 2 s
 say Trovalo, altrimenti...
 emo ti guarda con sguardo molto espressivo...
elseif %speech% == no
 wait 1 s
 say Osi rifiutarmi il tuo aiuto dopo che le guardie non ti hanno ucciso a vista?
 wait 1 s
 say Ingrato!
 kill %actor.name%
else end
end
~
#31309
Reazione Guerriero Drow (Nimrhuin)~
0 j 31363
~
wait 1 s






















say Mio... figlio! Ucciso... ucciso... no nessun nano puo' essere riuscito ad uccidere MIO figlio!
wait 3 s
say Sei stato tu!!!
say E ora subirai la sua stessa fine!
wait 1 s
kill %actor.name%
~
#31310
Cuoco (Nimrhuin)~
0 g 100
~
wait 1 s























say Cosa ci fai in casa mia??
say Chi sei??
wait 1 s
say Grr... non sei un Drow... non sono accetti gli altri esseri qui!
wait 1 s
say Dimmi come ti chiami!
emo agita il proprio coltello da cucina...
wait 2 s
emo e' spazientito.
say Fffff non mi interessa il tuo nome, magari la matrona mi dara' un premio se porto il tuo sangue alle sue driadi!
~
#31311
Alchimista (Nimrhuin)~
0 g 100
~
if %actor.vnum% == -1
wait 1 s
say CHI osa entrare di soppiatto nel mio laboratorio?
say Non tollero ALCUNA interruzione!
wait 1 s
emo inizia a ringhiare e fischiare.
stand
wait 1 s
emo ti lancia addosso una boccetta che esplode e ti avvolge tra le fiamme!
dg_cast 'fire breath' %actor.name%
end
~
#31312
Uscita per il palazzo (Nimrhuin)~
2 h 31340
~
wait 1 s





























wecho &1Appoggi la scala al muro, arriva perfettamente alla fessura in alto!&0
wait 1 s
wecho &1Arrampicandoti riesci facilmente a passarci, ma e' meglio sbrigarsi, la scala sembra piuttosto instabile...&0
wdoor 31327 up room 31331
wait 15 s
wecho &1La scala perde il suo appoggio a terra e scivola a terra, rompendosi in mille pezzi.&0
wdoor 31327 up purge
wpurge scala
~
#31313
Coordinatore drow (Nimrhuin)~
0 g 100
~
wait 1 s




























mecho &9&xSenti un rumore dietro di te, e una serie di ingranaggi iniziano a muoversi.&0
wait 1 s
mecho &9&xTi volti di scatto e trovi che la porta dietro di te e' sbarrata, a prova di fuga.&0
close porta
lock porta
wait 3 s
say Ehi, tu devi essere il nuovo schiavo venuto dalle parti alte
wait 1 s
emo ti scruta.
wait 2 s
say Sembri un po' alto per essere un nano...
wait 1 s
say Mah, la cosa non mi riguarda, prendi un piccone e scava!
wait 3 s
say E non provare a fuggire...
~
#31314
Morte Coordinatore (Nimrhuin)~
0 e 100
e' morto! R.I.P.!~
if %actor.name% == Un coordinatore drow




























 wait 3 s
 say Bah, quel bastardo e' morto, anche se l'assassino non mi ispira piu' fiducia.
 wait 1 s
 emo ti guarda biecamente.
 wait 2 s
 say Io me ne vado, magari i miei concittadini non mi uccideranno per aver perso la battaglia.
 wait 1 s
 emo si infila nel passaggio ad ovest e scompare.
 w
 mgoto 28150
 wait 3 s
 say purgami
end
~
#31315
Morte arcieri (Nimrhuin)~
0 f 100
~
mecho &1Il grido di morte dell'arciere attira l'attenzione di una guardia nei paraggi.&0
%load% mob 31307
~
#31316
Arrivo guardie (Nimrhuin)~
2 g 100
~
if %actor.vnum% == -1 && %direction% == west

























 wait 1 s
 wload mob 31307
 wait 1
 wforce guardia say Un nano ha sfondato le linee!! A morteeeeeeeee!
 wait 1
 wforce guardia kill %actor.name%
end
~
#31317
Gnomo prigioniero (Nimrhuin)~
0 e 0
e' morto! R.I.P.!~
if %actor.name% == Il capitano delle guardie














 wait 2 s
 say Ohhhh questa voce tanto odiata...
 wait 1 s
 say Finalmente emette un gemito di dolore...
 wait 2 s
 say Chi sara' stato a compiere il gesto fuori dalla mia portata
 wait 1 s
 say Magari e' persino un benefattore!
 wait 2 s
 say In tal caso, mio caro avventuriero
 wait 1 s
 say Fai un boun gesto a questo povero essere
 wait 2 s
 say Sono ormai l'ombra di cio' che ero
 wait 1 s
 say Con gli occhi feriti nemmen posso piangere
 wait 2 s
 say Cura le mie ferite, un semplice gesto
 wait 1 s
 say Puo' ridar vigore al corpo mio codesto
end
set curato 0
global curato
~
#31318
Gnomo prigioniero 2 (Nimrhuin)~
0 e 0
ti fissa e pronuncia le parole: 'judicandus gofuaeg'~
if %curato% == 0














 set curato 1
 global curato
 wait 2 s
 say E finalmente son libero di fuggire!
 wait 1 s
 say Non ti posso aiutare con alcun gesto,
 wait 2 s
 say Ma qualcosa di mio e' ancora rimasto,
 wait 1 s
 say Prendi questi occhiali, prima di partire!
 wait 1 s
 remove occhiali
 wait 2 s
 give occhiali %actor.name%
 drop occhiali
end
~
#31319
Gnomo prigioniero 3 (Nimrhuin)~
0 e 0
ti fissa e pronuncia le parole: 'judicandus qfuhuq'.~
if %curato% == 0














 set curato 1
 global curato
 wait 2 s
 say E finalmente son libero di fuggire!
 wait 1 s
 say Non ti posso aiutare con alcun gesto,
 wait 2 s
 say Ma qualcosa di mio e' ancora rimasto,
 wait 1 s
 say Prendi questi occhiali, prima di partire!
 wait 1 s
 remove occhiali
 wait 2 s
 give occhiali %actor.name%
 drop occhiali
end
~
#31320
Gnomo prigioniero 4 (Nimrhuin)~
0 e 0
ti fissa e pronuncia le parole: 'judicandus dies'.~
if %curato% == 0














 set curato 1
 global curato
 wait 2 s
 say E finalmente son libero di fuggire!
 wait 1 s
 say Non ti posso aiutare con alcun gesto,
 wait 2 s
 say Ma qualcosa di mio e' ancora rimasto,
 wait 1 s
 say Prendi questi occhiali, prima di partire!
 wait 1 s
 remove occhiali
 wait 2 s
 give occhiali %actor.name%
 drop occhiali
end
~
#31321
Gnomo prigioniero 5 (Nimrhuin)~
0 e 0
ti fissa e pronuncia le parole: '&5cure light&0'.~
if %curato% == 0














 set curato 1
 global curato
 wait 2 s
 say E finalmente son libero di fuggire!
 wait 1 s
 say Non ti posso aiutare con alcun gesto,
 wait 2 s
 say Ma qualcosa di mio e' ancora rimasto,
 wait 1 s
 say Prendi questi occhiali, prima di partire!
 wait 1 s
 remove occhiali
 wait 2 s
 give occhiali %actor.name%
 drop occhiali
end
~
#31322
Gnomo prigioniero 6 (Nimrhuin)~
0 e 0
ti fissa e pronuncia le parole: 'poir'.~
if %curato% == 0














 set curato 1
 global curato
 wait 2 s
 say E finalmente son libero di fuggire!
 wait 1 s
 say Non ti posso aiutare con alcun gesto,
 wait 2 s
 say Ma qualcosa di mio e' ancora rimasto,
 wait 1 s
 say Prendi questi occhiali, prima di partire!
 wait 1 s
 remove occhiali
 wait 2 s
 give occhiali %actor.name%
 drop occhiali
end
~
#31323
Gnomo prigioniero 7 (Nimrhuin)~
0 n 100
~
wait 1














set curato 1
global curato
~
#31324
Gnomo prigioniero 8 (Nimrhuin)~
0 f 100
~
mecho &6&xLo gnomo, trafitto dall'ultimo colpo, cade picchiando il volto sul terreno.&0














mecho &6Senti un rumore di vetri rotti, e spostando il volto dello gnomo con lo stivale vedi come i suoi occhiali siano distrutti e inutilizzabili.&0
mload mob 1416
~
#31325
Distruzione occhiali (Nimrhuin)~
0 n 100
~
wait 1














get occhiali cad
mteleport self 28150
say purgami
~
#31395
Prova n (Nimrhuin)~
0 d 100
cambiati~
wait 1


dg_cast 'disguise' %actor.name%
~
#31396
Prova 3 (Nimrhuin)~
2 g 100
~
wait 1

wecho %self.vnum%
wecho (2) %vnum%
~
#31397
Prova2 (Nimrhuin)~
0 d 100
prendi~
wait 1 s
say %actor.id%
say Prova
~
#31398
Prova (Nimrhuin)~
0 d 100
messaggio~
wait 1










eval creanim 0
global creanim
get all.creanim
wait 3 s
say %creanim%
~
#31399
Master of Spinis (Nimrhuin)~
1 b 100
~
halt


















~
$~
