#5400
Jiroch maledetta (FAy)~
0 g 100
~
if %actor.vnum% == -1
 if %actor.clan% == 7
  wait 1 s
  emo si china servilmente di fronte al padrone.
 else
  wait 1 s
  emo si avventa sul nemico!
  kill %actor.name%
 end
end
~
#5429
aziz~
0 d 100
dio~
wait 20
say Saluta Mars quando lo vedi!
wait 30
smile
~
#5432
kareem~
0 d 100
mappa~
wait 20
say portatemi un arco
wait 10
~
#5433
kareem2~
0 j 100
~
wait 10
inv
~
#5434
kareem3~
0 e 1
bow~
wait 10
smile
wait 30
say Grandioso! quello che cercavo :)
wait 10
say udsne
wait 30
wave
~
#5501
Porta Teslar (by Phantom)~
2 d 0
numenor~
if %actor.level% < 71
wait 1 s
wecho &6&xAl suono delle antiche parole il muro si illumina di rune ne sei abbagliato!&0
wait 2 s
wecho &6&xLentamente le rune si spengono mostrando un passaggio segreto&0
wait 1 s
wdoor 5501 east room 5502
wait 1 s
wdoor 5502 west room 5501
wait 180 s
wecho &6&xUn improvviso lampo di luce ti acceca per un attimo!&0
wait 2 s
wecho &6&xIl passaggio e' scomparso, al suo posto ora c'e' una solida parete di roccia.&0
wdoor 5501 east purge
wdoor 5502 west purge
end
else end
end
~
#5502
Danno nel prendere Runa x Teslar (by Phantom)~
1 g 100
~
wait 3
oecho Un raggio di luce scaturisce improvviso dalla Runa, attraversando il corpo di %actor.name%
odamage %actor% 150
~
$~
