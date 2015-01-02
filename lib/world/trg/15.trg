#1500
Sacerdote Silue (1) (Sheep)~
0 k 50
~
wait 1 s
mecho &7&xUna luce accecante si sprigiona dalle mani del Sacerdote!&0
say &7&xLa Luce di Silue mi protegge, in nome di Silue ti bandisco da questo Tempio!&0
dg_cast 'teleport' %actor.name%
~
#1501
Sacerdote di Silue (2) (Sheep)~
0 g 100
~
wait 1 s
if %actor.name% == alhandir
sor alhandir
wait 1 s
inch alhandir
wait 2 s
say Benveuto Alhandir, che la Luce della Dama sia con te
halt
end
sor %actor.name%
wait 2 s
say Benvenuto nel tempio della Dama, che la Sua Luce possa illuminarti
~
#1502
Sacerdote Silue (3) (Sheep)~
0 d 1
prova~
if %missione% == 1
wait 2 s
say Mi spiace %actor.name%, ma una missione e' gia' in corso..
wait 2 s
say Non posso assegnarne altre finche' %pg% non completera' la sua prova
halt
end
if %mob% == 1
wait 2 s
say Mi spiace %actor.name%, ma dovrete attendere prima che sia possibile ottenere un'altra prova...
halt
end
set missione 1
global missione
set mob 1
global mob
set pg %actor.name%
global pg
set livello %actor.level%
global livello
wait 1 s
say Volete dunque affrontare una prova per la maggior gloria di Silue?
wait 2 s
say Molto bene, avete un'ora di tempo per...
mat ttgls mpurge teinko
mat ttgls mload mob 11248
mat ttgls mforce teinko mload obj 542
mat ttgls mforce teinko get all
mat ttgls mforce teinko hold emblema
mat ttgls mforce teinko indo all
mat ttgls mforce teinko posa spada
mat ttgls mforce teinko mpurge spada
~
#1503
Sfera di Silue (1) (Sheep)~
1 g 100
~
if %get% == 0
eval %get%+1
halt
end
otransform 543
~
#1504
Sfera di Silue (2) (Sheep)~
1 hi 100
~
if %globo% == 1
halt
end
wait 2 s
set globo 1
global globo
oecho &9&xIl globo pian piano si spegne&0
otransform 543
~
#1505
Sacerdote di Silue (4) (Sheep)~
0 j 100
~
if %object.vnum% == 543
wait 2 s
emote aggrotta la fronte...
wait 2 s
say Mi spiace, ma questo globo e' ormai inutilizzabile, non posso assegnarti la ricompensa
set missione 0
global missione
halt
end
if %object.vnum% != 542
halt
end
if %actor.name% != %pg%
set missione 0
global missione
wait 2 s
say Mi spiace %actor.name%, ma questa era una missione assegnata a %pg%
wait 2 s
say Non posso concederti alcuna ricompensa...
halt
end
wait 2 s
sor %pg%
wait 1 s
ann
say Molto bene %pg%, vedo che avete svolto la vostra missione con coraggio
wait 1 s
set missione 0
global missione
eval divisore %random.10%
eval multip %random.10%
eval premio %livello%*10
eval premio %premio%*%multip%
eval premio %premio%/%divisore%
say %livello%
say %premio%
say m %multip%
say d %divisore%
if %premio% <= 400
mload obj 544
dai diamante %pg%
posa diamante
say ecco la tua ricompensa, %pg%
halt
elseif %premio% <= 800
mload obj 545
dai diamante %pg%
posa diamante
say ecco la tua ricompensa, %pg%
halt
elseif %premio% <= 950
mload obj 546
dai diamante %pg%
posa diamante
say ecco la tua ricompensa, %pg%
halt
else
mload obj 547
dai diamante %pg%
posa diamante
say ecco la tua ricompensa, %pg%
halt
end
~
#1506
Tempio Silue (Contatore) (Sheep)~
2 h 100
~
if %object.vnum% == 544
eval puntifede %puntifede%+50
global puntifede
wecho %puntifede%
wpurge rosso
else
halt
end
~
$~
