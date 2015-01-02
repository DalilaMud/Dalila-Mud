#31100
Stella di Zoya (Faymond)~
1 j 100
~
wait 1 s
%echo% &3&xLa stella brilla sul collo di Zoya avvolgendoti per un istante nel suo splendido bagliore!&0
~
#31101
Lo stiletto Renaji (Faymond)~
1 j 100
~
if %actor.name% != Garrick
wait 2 s
oecho &5Lo stiletto Renaji dice, 'Tu.. non sei il mio padrone!!'&0
wait 3 s
%echoaround% %actor% &1%actor.name% urla di dolore non appena impugna lo stiletto!!&0
%send% %actor% &1Non appena impugni lo stiletto lanci un urlo di dolore!!!&0
wait 1 s
%send% %actor% &1L'arma ti perfora la mano con mille spine e in un istante ti dissangua mortalmente!&0
%damage% %actor% 550
else
wait 1 s
%send% %actor% &8Senti scuro potere bruciare nella tua mano come fuoco!!&0
%echoaround% %actor% &8Non appena Garrick impugna l'arma i suoi occhi si fanno per un istante trasparenti..&0
wait 1 s
%echo% &5Lo stiletto Renaji dice, 'Le mani del mio padrone... le riconosco... mie...&0
end
~
#31102
Lo stiletto Renaji 2 (Faymond)~
1 g 100
~
wait 2 s
set punto %random.6%
if %punto% == 1
oecho &5Lo stiletto Renaji dice, 'MmmH.. Stringimi tra le tue mani..'&0
elseif %punto% == 2 
oecho &5Lo stiletto Renaji dice, 'Ahah.. Ora lasciate che io danzi nelle carni!'&0
elseif %punto% == 3
oecho &5Lo stiletto Renaji dice, '..Salve, o mio padrone.. he..he..'&0
elseif %punto% == 4
oecho &5Lo stiletto Renaji dice, '..Non lasciarmi mai piu..'&0
elseif %punto% == 5
oecho &5Lo stiletto Renaji dice, 'Sono tuo.. ti condurro' alla vittoria...'&0
elseif %punto% == 6
oecho Lo stiletto Renaji vibra e pronuncia le parole, '&5judicandant whiede&0'
end
~
#31104
Mendicante saggio (Faymond)~
0 m 6
~
if %amount% <= 232
wait 2
say ...non ti sarai sprecato? Non vorrei diventare ricco. Bah.. giovani..
else
wait 2 s
emote lascia che le monete scivolino nella sua grande tasca
wait 2 s
emote sorride, felicissimo!!! 
wait 4 s
set punto %random.8%
if %punto% == 1
say ...un consiglio... mai svegliare lo zio Buck quando dorme, senza un fiore davvero speciale!
elseif %punto% == 2
say ...sapevi che il potente Sid nutre profondo odio per i troll del fiume?
elseif %punto% == 3
say nella citta' sotterranea non guardare in faccia la mantide!!
elseif %punto% == 4
say ricorda, il corridoio d'energia puo' essere letale per i potenti!!
elseif %punto% == 5
say ...l'antica miniera e' stata abbandonata nonostante la sua ricchezza... chissa' come mai!
elseif %punto% == 6
say Non sempre l'oro e' bello come lo si dipinge!
elseif %punto% == 7
say un potente mago vive nella grotta dei cristalli... le sue magie, dicono, sono a tutti sconosciute.
elseif %punto% == 8
say fai attenzione a cio' che raccogli..
else
end
end
~
#31105
Anello nero di Buz (Faymond)~
1 j 100
~
wait 2 s
%echoaround% %actor% \&4\%actor% infila un anello al dito e.. per un istante ti sembra che circondato da nero fumo.. poi nulla.\&0
%send% %actor% \&7\Non appena infili l'anello sentiti il battito aumentare.. e la vista annebbiarsi per un istante.\&0
~
#31106
Arti marziali (Nimrhuin)~
0 k 100
~
wait 1
eval colpi %random.10%
if %colpi% == 1
 mecho &3%self.name% sferra un calcio basso colpendo %actor.name% alle caviglie, che resta in piedi per pura fortuna.&0
 dg_cast 'disintegrate' %actor.name%
elseif %colpi% == 2
 mecho &1%self.name% salta e colpisce %actor.name% al volto con un possente calcio volante!&0
 dg_cast 'disintegrate' %actor.name%
elseif %colpi% == 3
 mecho &4&xLo sguardo intenso di %self.name% si trasforma in un sorriso compiaciuto mentre tira un calcio al petto di %actor.name%.&0
 dg_cast 'disintegrate' %actor.name%
elseif %colpi% == 4
 msend %actor.name% &1&xAvverti un improvviso dolore alla coscia destra e vedi una profonda ferita infertati da %self.name%.&0
 dg_cast 'disintegrate' %actor.name%
elseif %colpi% == 5
 msend %actor.name% &7La guancia sinistra ti si infiamma quando %self.name% ti colpisce con un possente gancio.&0
 dg_cast 'disintegrate' %actor.name%
elseif %colpi% == 6
 msend %actor.name% &7&x%self.name% ti fa vacillare con un diretto al naso!&0
 dg_cast 'disintegrate' %actor.name%
elseif %colpi% == 7
 mecho &4%actor.name% emette un rantolo di dolore quando %self.name% gli rompe una costola con una potente gomitata.&0
 dg_cast 'disintegrate' %actor.name%
elseif %colpi% == 8
 mecho &xIl calcio alto di %self.name% colpisce %actor.name% al mento, accecandolo dal dolore.&0
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'blindness' %actor%
elseif %colpi% == 9
 mecho &3&x%self.name% fa un piccolo balzo in avanti, colpendo %actor.name% al petto con una dolorosa gomitata.&0
 dg_cast 'disintegrate' %actor.name%
end
wait 6 s
~
#31107
Bimbo di Delsenora (Faymond)~
1 h 100
~
wait 2 s
set punto %random.5%
if %punto% == 1
%echo% il piccolo Aran gattona felice inseguendo una farfalla con gli occhi
elseif %punto% == 2
%echo% il piccolo Aran ti osserva con occhi da sogno... sorride e gioca con le sue dita
elseif %punto% == 3
%echo% il piccolo Aran gattona fino ai tuoi piedi e si abbraccia alla tua gamba, sorridendo solare
elseif %punto% == 4
%echo% &5il piccolo Aran dice, '..mm.mamma..mma..'&0
wait 1 s
%echo% poi sorride, dolcissimo.
elseif %punto% == 5
%echo% il piccolo Aran ti saluta e si esprime in un sorriso che quasi ti strappa una lacrima..
else
end
end
~
#31108
Anello teletrasportante~
1 j 100
~
wait 1 s
oteleport %actor% 31102
oteleport %actor.name% 31102
oteleport actor 31102
~
#31109
Bimbo Delse (2)~
1 g 100
~
if %actor.name% == Delsenora
wait 1 s
%echo% Il piccolo Aran si stringe felice al petto della mamma
wait 1 s
%echo% La abbraccia e si volta a sorridere..
elseif %actor.name% == Redarion
wait 1 s
%echo% Il piccolo Aran decolla sollevato dal papa' che lo mette sulle sue spalle..
wait 1 s
%echo% alza le braccine al cielo e le agita felice!
else
wait 1 s
%echo% Il piccolo Aran guarda stupito le braccia che lo prendono..
wait 1 s
%echo% poi, si rende conto...
wait 2 s
%echo% &3Il piccolo Aran urla, 'BwaWaaaaaaaaaaaaa BwwwwwwWAaaaaaaaaaaaa MaaaaAmmMMaaaaaa'&0
end
~
#31110
Boccetta (faymond)~
1 c 7
apri apre boccetta ~
wait 1 s
oecho la boccetta ribolle impazzita!!!
wait 2 s
oecho dalla boccetta con un gran frastuono salta fuori uno spirito dell'aria!!
~
#31113
Spada danzante (Faymond)~
1 j 100
~
if %spada% != on
set spada on
global spada
wait 3 s
%echo% &1Improvvisamente una spada fantasma si materializza e danza nell'aria intorno a %actor.name%!!&0
oload mob 31113
wait 2 s
%force% 1secr foll %actor.name%
end
else
end
~
#31114
Spada danzante 2 (Faymond)~
1 j 100
100~
wait 3 s
%echo% &1Improvvisamente una spada fantasma si materializza e danza nell'aria intorno a %actor.name%!!&0
opurge 1secr
oload mob 31113
wait 2 s
%force% 1secr foll %actor.name%
end
else
end
~
#31117
Caduta nel buco (Faymond)~
2 g 100
~
wait 6 s
%echo% &7Improvvisamente mentre decidi sul da farsi senti il terreno franare sotto i tuoi piedi!!&0
%echo% &7Precipiti con un mare di terra per qualche interminabile secondo...&0
wteleport all 31105
~
#31120
Laden (Faymond)~
0 e 0
ti guarda~
if %incazzato% != on
set incazzato on
global incazzato
wait 1 s
emo ti guarda a sua volta e stringe i denti..
wait 3 s
say allora.. che diavolo hai da guardare?
else
wait 2 s
say evidentemente oggi non hai voglia di vivere... a te!
wait 2 s
kill %actor.name%
kill all
end
~
#31121
Oste (Faymond)~
0 e 100
svegliato~
wait 2 s
emo si frega gli occhi..
wait 2 s
say Gr..GRunf!? Chi.. cosa..
wait 1 s
emo ti guarda
wait 3 s
say ..bah.. lasciami dormire. Prendetevi tutto quello che volete.
wait 2 s
emo si accomoda nuovamente sul bancone.
dormi
end
~
#31123
new trigger~
0 j 100
scalpo~
wait 1 s
say ECCEZIONALE!!!
wait 2 s
grin
say Sparite dall'underground, bestie schifose!
wait 1 s
junk scalpo
emo butta un sacco in terra.
wait 2 s
say questa e' la vostra ricompensa. E finche' questi troll esisteranno potrete averne ancora.
emo torna ai suoi affari in silenzio.
~
#31124
Sigfrid 1 (Faymond)~
0 g 100
~
wait 3 s
say maledetti schifosi troll... Paghero' caro chi mi portera' il loro scalpo!!
set punto %random.6%
if %punto% == 1
emo accarezza la sua sciabola con due dita.
elseif %punto% == 2
emo ti scruta dietro i lunghi capelli.
elseif %punto% == 3
emo annuisce con un ghigno fastidioso.
elseif %punto% == 4
say ..potresti forse essere tu?
elseif %punto% == 5
emo fa saltare la sciabola da una mano all'altra.
elseif %punto% == 6
say ..vivono nella foce dell'Esidur. Pensateci bene.
end
~
#31141
Fiume sotterraneo (Faymond)~
2 g 100
~
wait 40 s
wecho &6Il fiume gorgoglia melodiosamente!!&0
wpurge coins
~
#31142
Sid (Faymond)~
0 e 0
troll arriva~
wait 1 s
emo scatta in piedi e rotea fulmineo la spada!
kill troll
say Maledettissime bestie bastarde!!
~
#31146
Cascata (Faymond)~
2 g 100
~
wait 3 s
%send% %actor% Sei travolto e sbattuto sulle pietre con violenza micidiale!!
%damage% %actor% 150
wteleport %actor.name% 31147
~
#31153
La miniera crolla!!!!~
2 c 100
lavora vena~
wait 1 s
wecho &1UN BOATO MICIDIALE!!! LA TERRA TREMA!!! La struttura portante del complesso sta crollando!! Esci presto!!&0
wait 5 s
%damage% all 100
~
#31176
Mago alternativo (Faymond)~
0 k 100
~
wait 4 s
set punto %random.4%
if %punto% == 1
emote guarda il cielo e urla le parole: '&5marsor danohari&0'.
%send% %actor% &1Un dolore lancinante esplode sul tuo petto!!&0
%echoaround% %actor% &1%actor.name% urla dal dolore quando il suo petto si squarcia!!&0
dg_cast 'disinteg' %actor%
elseif %punto% == 2
emote stringe i pugni e pronuncia le parole: '&5qfuhuq poir&0'.
mecho Le sue ferite si richiudono a velocita' eccezionale!!
dg_cast 'heal'
dg_cast 'heal'
elseif %punto% == 3
emote allarga le braccia e pronuncia le parole: '&5kahaqet&0'.
%echo% &4Improvvisamente un terribile sciame di insetti prende forma nella stanza con un gran frastuono!!!!&0
mload mob 31175
mforce sciam kill %actor%
elseif %punto% == 4
emote &3urla quando un &5cerchio di energia&3 esplode intorno a lui colpendoti in pieno!&0
dg_cast 'minute meteor' %actor%
else
end
~
#31181
Kall (Nimrhuin)~
1 j 100
~
wait 1 s
oechoaround %actor.name% &1&xAppena %actor.name% indossa l'oggetto, questo entra in simbiosi con lui.&0
osend %actor.name% &1&xAppena indossi l'oggetto esso si attacca al tuo corpo e diventa parte di te!&0
~
#31182
Zio Buck (Faymond)~
0 e 100
svegliato~
wait 2 s
emote si stropiccia gli occhi
wait 2 s
emote ti osserva, sussulta e punta tremante uno strano oggetto verso di te.
wait 1 s
say ...FUORI!! FUORI DI QUI!!! SUBITO!!! FUORI O SPARO!!
wait 6 s
say MI HAI SENTITO??? ESCI DI QUI!!!
wait 7 s
say M..Maledetto!! Ti avevo avvertito!!! Prendi questo!
wait 2 s
emote preme alcuni grilletti e.... BOOOOOOOOOM!
%echo% \&1\Un'ondata di pallini di metallo esplode sul tuo addome in una nuvola di sangue...\&0
dg_cast 'disintegrate' %actor%
wait 1 s
kill %actor.name%
~
#31183
Anti-Immortal (Nimrhuin)~
0 g 100
~
wait 1
if %actor.level% > 70
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
 dg_cast 'disintegrate' %actor.name%
end
~
#31184
&1&xSala del trono Aldar&0 (Nimrhuin)~
0 d 100
arrestate~
wait 1
if (%actor.clan% == 5 && %actor.rank% >= 25)
 set %speech%
 kill %arrestate%
end
 
~
$~
