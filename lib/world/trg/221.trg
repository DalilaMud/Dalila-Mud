#22100
Prigioni 2 - Elenia (Jerold)~
2 d 1
evadi~
eval recita1 %random.3%
global recita1
eval recita2 %random.3%
global recita2
eval recita3 %random.3%
global recita3
eval recita4 %random.3%
global recita4
if %recita1% == 3
set recita1a &3&xLa tua abilita' di ladro riesce a sbloccare la serratura a tempo... esci presto!!&0
elseif %recita1% == 2
set recita1a &3&xEsci presto! Hai sbloccato la porta della cella!&0
else
set recita1a &3&xEsci! non c'e tempo da perdere! tra poco la serratura si riblocchera!&0
end
if %recita2% == 3
set recita2a &1&xDannazione! I passi di una guardia ti deconcentrano!&0
elseif %recita2% == 2
set recita2a &1&xIl pezzo di ferro usato per evadere si rompe nella serratura... ne devi trovare un altro!&0
else
set recita2a &1&xLa polvere della cella ti fa starnutire e perdere di concetrazione!&0
end
if %recita3% == 3
set recita3a &3&xAnche se non sei un ladro riesci a sbloccare la porta della cella... esci presto!&0
elseif %recita3% == 2
set recita3a &3&xEsci presto! Hai sbloccato la porta della cella!&0
else
set recita3a &3&xEsci! non c'e tempo da perdere! tra poco la serratura si riblocchera!&0
end
if %recita4% == 3
set recita4a &1&xEhi! Per preparare un evasione ci vuole tempo!&0
elseif %recita4% == 2
set recita4a &1&xNon ora! Non ora!&0
else
set recita4a &1&xSenti i passi delle guardie..meglio stare fermi per un po!&0
end
if %lock% == OFF
set lock ON
global lock
if %actor.class% == alcione
eval percentuale_fuga %random.500%
global percentuale_fuga
if %percentuale_fuga% > 498
wecho %actor.name% %recita1a%
wdoor 22138 south flags a
wait 10 s
wdoor 22138 south flags bcd
else
wecho %actor.name% %recita2a%
end
else
eval percentuale_fuga1 %random.1000%
global percentuale_fuga1
if %percentuale_fuga1% > 998
wecho %actor.name% %recita3a%
wdoor 22138 south flags a
wait 10 s
wdoor 22138 south flags bcd
else
wecho %actor.name% %recita2a%
end
end
else
wecho %actor.name% %recita4a%
end
~
#22101
Prigioni 3 - Elenia (Jerold)~
2 d 0
devo uscire~
if %speech% == devo uscire
if %actor.name% == yaemon || %actor.name% == demostenee || %actor.name% == boccadoro || %actor.name% == kurten || %actor.name% == lavinia || %actor.name% == dolmant
wait 2 s
wsend %actor.name% Ad un tuo cenno una guardia ti fa uscire
wait 2 s
wteleport %actor.name% 22136
wforce %actor.name% look
else
end
else
end
~
#22102
Prigioni 4 - Elenia (Jerold~
2 d 1
libera~
if %actor.name% == yaemon || %actor.name% == demostenee || %actor.name% == boccadoro || %actor.name% == kurten || %actor.name% == lavinia || %actor.name% == dolmant
set %speech%
global libera
wait 2 s
wecho  Un robusto guardiano arriva e porta fuori %libera% 
wait 2 s
wsend %libera% Il guardiano ti fa uscire dalla cella... in malo modo.
wait 2 s
wteleport %libera% 22136
wforce %libera% look
unset libera
else
end
~
$~
