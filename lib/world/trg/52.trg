#5200
Stabbata~
0 h 100
~
%send% %actor.name% &RAvverti una fitta alla schiena quando Jekin il Re degli Assassini ti colpisce alle spalle!&0
%echoaround% %actor% &VJekin il Re degli Assassini pianta nella schiena un pugnale &zdalla lama nera&V di %actor%. Uno schizzo di sangue quasi ti raggiunge!&0
%damage% %actor% 250
mkill %actor%
~
#5201
Beholder~
0 k 100
~
eval cast %random.10%
if (%cast% == 1)
dg_cast 'shocking grasp' %random.char%
dg_cast 'blade barrier' %random.char%
dg_cast 'sleep' %random.char%
end
if (%cast% == 2)
dg_cast 'color spray' %random.char%
dg_cast 'cone of cold' %random.char%
dg_cast 'silence' %random.char%
end
if (%cast% == 3)
dg_cast 'poison' %random.char%
dg_cast 'chill touch' %random.char%
dg_cast 'levitate' %random.char%
end
if (%cast% == 4)
dg_cast 'paralize' %random.char%
dg_cast 'lightning bolt' %random.char%
dg_cast 'peace' %random.char%
end
if (%cast% == 5)
dg_cast 'disintegrate' %random.char%
dg_cast 'minute meteor' %random.char%
dg_cast 'fear' %random.char%
end
if (%cast% == 6)
dg_cast 'energy drain' %random.char%
dg_cast 'harm' %random.char%
dg_cast 'time distortion' %random.char%
end
if (%cast% == 7)
dg_cast 'chain lightning' %random.char%
dg_cast 'blindness' %random.char%
dg_cast 'magic missile' %random.char%
end
if (%cast% == 8)
dg_cast 'fireshield' %random.char%
dg_cast 'fear' %random.char%
dg_cast 'acid arrow' %random.char%
end
if (%cast% == 9)
dg_cast 'flame arrow' %random.char%
dg_cast 'earthquake' %random.char%
dg_cast 'call lightning' %random.char%
end
if (%cast% == 10)
dg_cast 'fireball' %random.char%
dg_cast 'burning hands' %random.char%
dg_cast 'curse' %random.char%
end
~
#5202
Decapitatore~
0 k 5
~
%echo% Non completo
~
#5203
Replicante si trasforma~
0 k 100
~
%echo% &BIl tuo attacco costringe %self.name% a mostrare la sua vera natura!&0
%echo% &RE' un replicante!!!&0
%load% mob 5222
%purge% %self%
halt
~
#5204
Il Replicante 2~
0 n 100
~
%load% obj 5207
mkill %random.char%
indossa corazza
~
#5205
Vecchietta~
0 g 100
~
%echo% %self.name% si volta e ti sorride.
wait 1s
say Vuoi una tazza di the giovanotto?
wait 1s
smile
~
#5206
Poetessa~
0 k 5
~
%echo% &c%self.name% suona una dolce e melanconica melodia...&0
%send% %actor.name% &gHai molto sonno.....&0
dg_cast 'peace' %actor%
dg_cast 'sleep' %actor%
dg_cast 'sleep' %actor%
dg_cast 'sleep' %actor%
dg_cast 'peace' %actor%
~
#5207
The vecchietta~
0 d 1
volentieri si~
eval oggetto %self.inventory(5219)%
if (oggetto)
dai pozione %actor.name%
smile
end
else
say Oh... che sbadata...
wait 1s
say Non ne ho piu'!
~
#5208
maschera ghignante~
1 j 100
~
wait 1s
%echo% &rUn macabro ghigno ancestrale risuona nell'aria.&0
~
#5209
Capitano basha in prigione~
0 k 5
~
osend %actor.name% &RCon una potente spallata %self.name% ti scaraventa nella botola sottostante!&d
oechoaround %actor.name% Con una potente spallata %self.name% scaraventa %actor.name% nella botola sottostante!
%teleport% %actor% 5280
~
#5210
Ciondolo Lunatico~
1 b 10
~
eval caso %random.30%
if (%self.worn_by%)
if %caso% > 9
%echo% La pietra lunare pulsa, ma non accade nulla
end
if %caso% == 9
%send% %self.worn_by% &GLa pietra lunare pulsa, &Euna luce violacea ti avvolge&0
dg_cast 'time distortion' %self.worn_by%
end
if %caso% == 1
%send% %self.worn_by% &GLa pietra lunare pulsa, &guna luce marrone ti avvolge&0
dg_cast 'sense life' %self.worn_by%
end
if %caso% == 2
%send% %self.worn_by% &GLa pietra lunare pulsa, &runa luce scarlatta ti avvolge&0
dg_cast 'paralize' %self.worn_by%
end
if %caso% == 3
%send% %self.worn_by% &GLa pietra lunare pulsa, &wuna luce avorio ti avvolge&0
dg_cast 'detect sneak' %self.worn_by%
end
if %caso% == 4
%send% %self.worn_by% &GLa pietra lunare pulsa, &zuna luce nera ti avvolge&0
dg_cast 'blindness' %self.worn_by%
end
if %caso% == 5
%send% %self.worn_by% &GLa pietra lunare pulsa, &Vuna luce verde ti avvolge&0
dg_cast 'detect invisibility' %self.worn_by%
end
if %caso% == 6
%send% %self.worn_by% &GLa pietra lunare pulsa, &Runa luce rossa ti avvolge&0
dg_cast 'silence' %self.worn_by%
end
if %caso% == 7
%send% %self.worn_by% &GLa pietra lunare pulsa, &Cuna luce azzurrina ti avvolge&0
dg_cast 'blink' %self.worn_by%
end
if %caso% == 8
%send% %self.worn_by% &GLa pietra lunare pulsa, &Wuna luce bianca ti avvolge&0
dg_cast 'sanctuary' %self.worn_by%
end
else
%echo% La pietra lunare pulsa, ma non accade nulla
halt
end
~
#5211
Lord Dakker(1)~
0 g 100
~
say Salve milord
wait 3s
say Benvenuto nel mio salotto
wait 3s
say Lasciate che vi racconti una delle mie mirabolanti imprese
wait 3s
say Un giorno mi trovai in quel di Jiroch
wait 3s
say Quando a un tratto udii la voce di una donzella in pericolo
wait 3s
say Accorsi in suo aiuto e vidi che Tyrano, il Roc e Kaius la volevano rapire
wait 3s
say Aaaahhhh.... Vossignoria capira' come mi sentii sopraffatto dall'ira
wait 3s
say Non appena sguainai la mia famigerata spada 'L'ammazza Dei'...
wait 3s
say ... il Roc volo' via per lo spavento ...
wait 3s
say ... Kaius si mise ad implorare pieta' ...
wait 3s
say ... e Tyrano uggiolava come un cagnolino indifeso ...
wait 3s
say Cosi' salvai la damigella e godei delle sue grazie
wait 3s
smile
~
#5212
Lord Dakker(2)~
0 k 50
~
eval caso %random.4%
if %caso% == 4
say Ohibo' ma che fate!
dg_cast 'peace' %actor%
end
if %caso% == 1
say Milord basta cosi' o saro' costretto a farvi del male
dg_cast 'peace' %actor%
end
if %caso% == 2
say Tutto cio' e' alquanto disdicevole
dg_cast 'peace' %actor%
end
if %caso% == 3
say Voi siete un villano!
dg_cast 'peace' %actor%
end
~
#5213
pubblico~
0 g 100
~
%echo% &BIl musico interrompe la sua melodia, disturbato...&0
wait 1s
emo si volta
wait 1s
say Hai interrotto il musico! Bastardo!
kill %actor%
~
#5214
Mummia (1)~
0 k 10
~
eval caso %random.5%
if %caso% <= 4
dg_cast 'curse' %actor%
else
%echo% &GUn pezzo della mummia cade a terra e si trasforma immediatamente in una mummia piu' piccola&0
mload mob 5250
end
~
#5215
Richiesta parola d'ordine~
0 g 100
~
emo ti sbarra il passo
wait 1s
say Parola d'ordine
~
#5216
Controlla parola d'ordine~
0 d 100
morte alle lamie~
say Ok, puoi passare
wait 1s
Unlock cancello nord
open cancello nord
~
#5217
Chiusura del portone~
0 q 100
north~
nod
wait 1s
close cancello north
lock cancello north
~
#5218
ambasciata di sendaria a chyrellos - guardia~
0 q 100
~
if (%actor.clan% != 8)
if (%actor.level% <= 70)
if (%direction% == West)
say Solo gli appartenenti al Regno di Sendaria possono entrare
%echoaround% %actor% &B%actor.name% cerca di passare ma %self.name% lo blocca.&0
%send% %actor% &wCerchi di passare ma %self.name% ti blocca.&0
return (0)
end
end
end
~
#5219
Anello farlocco~
1 j 100
~
%echo% Non appena &W%actor.name% indossa l'anello questo comincia a pulsare e vibrare&0
wait 4s
%echo% &BLe pulsazioni dell'anello si fanno sempre piu' forti&0
wait 4s
%echo% &ZOra le vibrazioni si trasmettono ad %actor.name% che trema visibilmente&0
wait 4s
%echo% &VUn immensa energia si accumula nel corpo di %actor.name%!&0
wait 4s
%echo% &RSta per accadere qualcosa!&0
wait 2s
%force% %actor% scorreggia
wait 1s
%force% %actor% say Aaaaahhhh...
~
#5220
scarpette~
1 j 100
~
%echoaround% %actor% &E%actor.name% esegue dei graziosissimi movimenti di leggiadra danza&0
%send% %actor% &ENon puoi fare a meno di eseguire un magistrale passo di danza!&0
~
#5221
Verga infernale Malus~
1 j 100
~
if %actor.name% == Malus
%echo% la verga infernale vibra ed emette scintille infuocate
wait 3s
%load% mob 5295
end
~
#5222
Reset delle porte~
2 f 100
~
%door% 5200 east flags ab
%door% 5201 west flags ab
%door% 5216 west flags ab
%door% 5217 east flags ab
%door% 5223 west flags abe
%door% 5224 east flags abe
%door% 5261 west flags ab
%door% 5263 east flags ab
%door% 5261 east flags ab
%door% 5262 west flags ab
%door% 5264 north flags ab
%door% 5267 south flags ab
%door% 5284 south flags ab
%door% 5285 north flags ab
%door% 5285 west flags ab
%door% 5287 east flags ab
%door% 5285 east flags ab
%door% 5286 west flags ab
%door% 5287 down flags abe
%door% 5299 up flags abe
%door% 5292 up flags abe
%door% 5280 down flags abe
%door% 5297 east flags abe
%door% 5288 west flags abe
%door% 5229 east flags abe
%door% 5225 south flags abcd
%door% 5207 north flags abcd
%door% 5233 north flags ab
%door% 5234 south flags ab
%door% 5239 north flags abcdh
%door% 5238 south flags abcdh
%door% 5211 east flags ab
%door% 5211 north flags ab
%door% 5212 south flags ab
%door% 5268 east flags ab
%door% 5269 west flags ab
%door% 5268 west flags ab
%door% 5260 east flags ab
%door% 5271 north flags ab
%door% 5273 south flags ab
%door% 5271 east flags ab
%door% 5271 west flags ab
%door% 5272 east flags ab
%door% 5274 west flags ab
%door% 5248 east flags ab
%door% 5249 west flags ab
%door% 5247 east flags ab
%door% 5248 west flags ab
%door% 5253 east flags ab
%door% 5254 west flags ab
%door% 5257 north flags ab
%door% 5253 south flags ab
%door% 5252 north flags ab
%door% 5252 south flags ab
%door% 5247 south flags ab
%door% 5256 north flags ab
%door% 5247 west flags ab
%door% 5255 north flags ab
%door% 5250 east flags ab
%door% 5251 west flags ab
%door% 5251 south flags ab
%door% 5245 east flags ab
%door% 5246 west flags ab
%door% 5246 east flags ab
%door% 5255 up flags abe
%door% 5258 flags abe
%door% 12005 west flags ab
%door% 5260 north purge
%door% 5260 east purge
%door% 5260 west purge
%door% 5296 up flags e
~
#5223
Campanellino alhandir~
1 j 100
~
if %actor.name% == Alhandir
%echo% il campanellino emette un suono delicato
wait 3s
%load% mob 5294
end
~
#5292
Teleport area virex~
2 d 1
teletrasporto~
if %actor.name% == virex
   wait 5s
   %echoaround% %actor.name% &W%actor.name%&W scompare all'improvviso&0
   %send% %actor.name% &WUn0improvvisa luce intensa ti avvolge&0
%teleport% %actor.name% 10100
end
~
#5293
anti ident~
1 p 100
~
%echo% Si si, ti piacerebbe saperlo...
return 0
~
#5294
gatto alhandir~
0 n 100
~
%echo% &W%self.name% compare in una nuvoletta di fumo...&0
%load% obj 5263 %self% inv
follow Alhandir
wear anello
say Miiaaaoooo
~
#5295
porta di roma~
0 n 100
~
%door% 1487590 east room 12005
~
#5296
purga anello~
1 j 100
~
if %actor.vnum% == 5295
halt
end
if %actor.vnum% == 5294
halt
end
%purge% %self%
halt
~
#5297
diavoletto malus~
0 n 100
~
%echo% %self.name% &Zcompare in una nuvoletta di fumo...&0
%load% obj 5263 %self% inv
follow Malus
wear anello
~
#5298
purga obj~
1 l 100
~
%purge% %self%
halt
~
#5299
Lingotto replicante~
1 g 100
~
if %actor.name% == Silion
%load% obj 5260
end
~
$~
