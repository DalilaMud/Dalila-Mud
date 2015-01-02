#5700
Drago 1 (ardanos)~
1 j 100
~
if %caricato% != on
if %actor.name% == ardanos
set caricato on
global caricato
wait 1 s
oecho Vedi arrivare dal cielo un enorme drago...
wait 1 s
oload mob 5700
oforce drago foll ardanos
else
end else
end
~
#5701
Drago 2 (ardanos)~
1 c 1
libera~
if %actor.name% != ardanos
halt
end
osend %actor.name% Lasci libero il tuo drago.
oechoaround %actor.name% Ardanos lascia libero il suo drago.
opurge Azuth
set caricato off
global caricato
~
$~
