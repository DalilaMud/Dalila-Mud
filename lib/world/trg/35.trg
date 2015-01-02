#3500
Follettobomba (Sheep)~
1 h 100
~
wait 1 s
set room %self.is_inroom%
if %room.vnum% == 0
osend %actor.alias% Non puoi innescare la bomba mentre la hai addosso! Devi posarla per terra!
halt
end
osend %actor.name% Ecco... ora faresti meglio a scappare!!!
oechoaround %actor.name% %actor.name% innesca la bomba! Corri!
set conta 31
otransform 3563
while %conta% != 0
eval conta %conta% - 1
wait 1 s
oecho &R%conta%&d
wait 1 s
oload mob 3510
done
otransform 3562
eval caso %random.5%
global caso
if %caso% == 1
oecho &VLa bomba va in pezzi!&d
%purge% self
else
halt
end
~
#3501
Gatto mangia topo (Sheep)~
0 gi 100
~
wait 1
set char %random.char%
if %char.vnum% == 28100 || %char.vnum% == 28101
say MEEOOOOOOOOW
kill %char.alias%
else
halt
end
~
#3502
Bomba (Sheep)~
1 h 100
~
wait 1 s
set room %self.is_inroom%
if %room.vnum% == 0
osend %actor.alias% Non puoi innescare la bomba mentre la hai addosso! Devi posarla per terra!
halt
end
otransform 3565
osend %actor.name% Ecco... ora faresti meglio a scappare!!!
oechoaround %actor.name% %actor.name% innesca la bomba! Corri!
wait 1 s
oecho &rO&g.............................&Rx&d
wait 1 s
oecho &rO&g............................&Rx&d
wait 1 s
oecho &rO&g...........................&Rx&d
wait 1 s
oecho &rO&g..........................&Rx&d
wait 1 s
oecho &rO&g.........................&Rx&d
wait 1 s
oecho &rO&g........................&Rx&d
wait 1 s
oecho &rO&g.......................&Rx&d
wait 1 s
oecho &rO&g......................&Rx&d
wait 1 s
oecho &rO&g.....................&Rx&d
wait 1 s
oecho &rO&g....................&Rx&d
wait 1 s
oecho &rO&g...................&Rx&d
wait 1 s
oecho &rO&g..................&Rx&d
wait 1 s
oecho &rO&g.................&Rx&d
wait 1 s
oecho &rO&g................&Rx&d
wait 1 s
oecho &rO&g...............&Rx&d
wait 1 s
oecho &rO&g..............&Rx&d
wait 1 s
oecho &rO&g.............&Rx&d
wait 1 s
oecho &rO&g............&Rx&d
wait 1 s
oecho &rO&g...........&Rx&d
wait 1 s
oecho &rO&g..........&Rx&d
wait 1 s
oecho &rO&g.........&Rx&d
wait 1 s
oecho &rO&g........&Rx&d
wait 1 s
oecho &rO&g.......&Rx&d
wait 1 s
oecho &rO&g......&Rx&d
wait 1 s
oecho &rO&g.....&Rx&d
wait 1 s
oecho &rO&g....&Rx&d
wait 1 s
oecho &rO&g...&Rx&d
wait 1 s
oecho &rO&g..&Rx&d
wait 1 s
oecho &rO&g.&Rx&d
wait 1 s
oecho &rO&g&Rx&d
wait 2 s
oecho &RBOOOOOMMM!!!&d
odamage all 5000
%purge% self
~
$~
