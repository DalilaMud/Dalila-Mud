#834
richiamo alce (zephy)~
1 j 100
~
if %caricato% != on
if %actor.name% == maqroll
   set caricato on
   global caricato
   wait 1 s
osend %actor.name% Porti il fischietto alla bocca e vi soffi dentro...
osend %actor.name% ... ne esce uno strano suono, simile ad un muggito..
oechoaround %actor.name% %actor.name% porta un oggetto alla bocca, subito dopo...
oechoaround %actor.name% ...si sente uno strano muggito
   wait 2 s
oecho Un cucciolo d'alce si avvicina trotterellando e inizia a seguire %actor.name%
wait 1 s
   oload mob 64
oforce alce foll %actor.name%
   else
   end
   else
   end
~
#835
allontanamento alce (zephy)~
1 c 2
addio~
wait 1 s
if %actor.name% != maqroll
   halt
   end
oforce %actor.name% say A presto amico mio...
osend %actor.name% Il Cucciolo d'Alce muggisce un saluto, poi si allontana... silenziosamente
oechoaround %actor.name% Il Cucciolo d'alce muggisce un saluto, poi si allontana.. silenziosamnte...
   opurge alce
   set caricato off
   global caricato
~
$~
