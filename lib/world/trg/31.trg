#3100
contatore vendita provvisorio~
0 e 1
decide~
if %tipo% == mate1
eval mate1 %mate1% + 1
global mate1
eval mate1tot %mate1tot% + 1
global mate1tot
end
if %tipo% == mate2
eval mate2 %mate2% + 1
global mate2
eval mate2tot %mate2tot% + 1
global mate2tot
end
if %tipo% == mate3
eval mate3 %mate3% + 1
global mate3
eval mate3tot %mate3tot% + 1
global mate3tot
end
if %tipo% == mate4
eval mate4 %mate4% + 1
global mate4
eval mate4tot %mate4tot% + 1
global mate4tot
end
if %tipo% == mate5
eval mate5 %mate5% + 1
global mate5
eval mate5tot %mate5tot% + 1
global mate5tot
end
if %tipo% == mate6
eval mate6 %mate6% + 1
global mate6
eval mate6tot %mate6tot% + 1
global mate6tot
end
if %tipo% == mate7
eval mate7 %mate7% + 1
global mate7
eval mate7tot %mate7tot% + 1
global mate7tot
end
if %tipo% == mate8
eval mate8 %mate8% + 1
global mate8
eval mate8tot %mate8tot% + 1
global mate8tot
end
if %tipo% == mate9
eval mate9 %mate9% + 1
global mate9
eval mate9tot %mate9tot% + 1
global mate9tot
end
if %tipo% == mate1v
eval mate1tot %mate1tot% + 1
global mate1tot
end
if %tipo% == mate2v
eval mate2tot %mate2tot% + 1
global mate2tot
end
if %tipo% == mate3v
eval mate3tot %mate3tot% + 1
global mate3tot
end
if %tipo% == mate4v
eval mate4tot %mate4tot% + 1
global mate4tot
end
if %tipo% == mate5v
eval mate5tot %mate5tot% + 1
global mate5tot
end
if %tipo% == mate6v
eval mate6tot %mate6tot% + 1
global mate6tot
end
if %tipo% == mate7v
eval mate7tot %mate7tot% + 1
global mate7tot
end
if %tipo% == mate8v
eval mate8tot %mate8tot% + 1
global mate8tot
end
if %tipo% == mate9v
eval mate9tot %mate9tot% + 1
global mate9tot
end
if %mate1tot% > 250
say basta1
end
if %mate2tot% > 250
say basta2
end
if %mate3tot% > 250
say basta3
end
if %mate4tot% > 250
say basta4
end
if %mate5tot% > 250
say basta5
end
if %mate6tot% > 250
say basta6
end
if %mate7tot% > 250
say basta7
end
if %mate8tot% > 250
say basta8
end
if %mate9tot% > 250
say basta9
end
~
#3101
contatore vendita provvisorio2~
0 d 1
stop mate1 mate2 mate3 mate4 mate5 mate6 mate7 mate8 mate9 mate1v mate2v mate3v mate4v mate5v mate6v mate7v mate8v mate9v resetta azzera~
if %actor.name% == roland || %actor.name% == fizban || %actor.name% == rodric
if %speech% == azzera
set mate1tot 0
set mate2tot 0
set mate3tot 0
set mate4tot 0
set mate5tot 0
set mate6tot 0
set mate7tot 0
set mate8tot 0
set mate9tot 0
global mate1tot
global mate2tot
global mate3tot
global mate4tot
global mate5tot
global mate6tot
global mate7tot
global mate8tot
global mate9tot
end
end

if %speech% == resetta
set mate1 0
set mate2 0
set mate3 0
set mate4 0
set mate5 0
set mate6 0
set mate7 0
set mate8 0
set mate9 0
global mate1
global mate2
global mate3
global mate4
global mate5
global mate6
global mate7
global mate8
global mate9
end
if %speech% == stop
wait 1 s
say %mate1%
say %mate2%
say %mate3%
say %mate4%
say %mate5%
say %mate6%
say %mate7%
say %mate8%
say %mate9%
end
if %speech% != stop && %speech% != resetta && %speech% != azzera
set tipo %speech%
global tipo
end
~
#3102
contatore totale provvisorio~
0 e 1
prende~
if %tipo% == mate1v
eval mate1tot %mate1tot% - 1
global mate1tot
end
if %tipo% == mate2v
eval mate2tot %mate2tot% - 1
global mate2tot
end
if %tipo% == mate3v
eval mate3tot %mate3tot% - 1
global mate3tot
end
if %tipo% == mate4v
eval mate4tot %mate4tot% - 1
global mate4tot
end
if %tipo% == mate5v
eval mate5tot %mate5tot% - 1
global mate5tot
end
if %tipo% == mate6v
eval mate6tot %mate6tot% - 1
global mate6tot
end
if %tipo% == mate7v
eval mate7tot %mate7tot% - 1
global mate7tot
end
if %tipo% == mate8v
eval mate8tot %mate8tot% - 1
global mate8tot
end
if %tipo% == mate9v
eval mate9tot %mate9tot% - 1
global mate9tot
end
if %mate1tot% < 250
say riprendi1
end
if %mate2tot% < 250
say riprendi2
end
if %mate3tot% < 250
say riprendi3
end
if %mate4tot% < 250
say riprendi4
end
if %mate5tot% < 250
say riprendi5
end
if %mate6tot% < 250
say riprendi6
end
if %mate7tot% < 250
say riprendi7
end
if %mate8tot% < 250
say riprendi8
end
if %mate9tot% < 250
say riprendi9
end
~
#3104
negoziante provvisorio vendere lingottaio~
0 d 1
vendo~
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %ver1% == off && %ver2% == off && %ver3% == off && %ver4% == off
say Mi spiace , non mi serve nulla
set full on
globa full
end
end
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %full% != on
set name %actor.name%
global name
wait 1 s
say %name% , io compro i seguenti prodotti a questi prezzi :
wait 1 s
if %ver1% != off
say Lingotti di ferro  &1%pre1%&5 monete l'uno
end
if %ver2% != off
say Lingotti di rame   &1%pre2%&5 monete l'uno
end
if %ver3% != off
say Lingotti di stagno &1%pre3%&5 monete l'uno
end
if %ver4% != off
say Lingotti di zinco  &1%pre4%&5 monete l'uno
end

wait 1 s
say Mi dia tutto quello che vuole vendere (max 20 per tipo) e poi dica 'finito'
wait 1 s
say Attento , se mi da qualcosa di diverso , lo potrebbe perdere!
set timer on
set volta 0
global timer
global volta
set vend on
global vend
end
end
unset full

~
#3105
negoziante provvisorio vendere Lingottaio 2~
0 d 1
bilancio cambia 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 basta1 riprendi1 basta2 riprendi2 basta3 riprendi3 basta4 riprendi4~
if %actor.name% == Rodric || %actor.name% == Fizban || %actor.name% == %primo% || %actor.name% == %secondo% || %actor.name% == %terzo%

if %speech% == bilancio
wait 1 s
say Attualmente ho %self.gold% monete in cassa.
end
if %speech% == cambia && %actor.name% != %terzo%
wait 1 s
say %actor.name% , mi dica le cifre del nuovo prezzo (4 cifre) , una per volta , partendo dalle migliaia.
set camb on
global camb
set p1 on
global p1
end

if %speech% < 10
if %camb% == off
eval pre%speech% %tot%
global pre%speech%
wait 1 s
say Va bene %actor.name% , il nuovo prezzo per il prodotto &1%speech%&5 e' &1%tot%&0
unset camb
unset tot
unset p1
unset p2
unset p3
end

if %camb% == on
if %p4% == on
eval p4 %speech%
global p4
wait 1 s
eval tot %p1%*1000+%p2%*100+%p3%*10+%p4%
global tot
say Il nuovo prezzo e' quindi &1%tot%&0
wait 1 s
unset p4
say Di che prodotto vuole cambiare il prezzo?
set camb off
global camb
end
if %p3% == on
eval p3 %speech%
global p3
set p4 on
global p4
end
if %p2% == on
eval p2 %speech%
global p2
set p3 on
global p3
end
if %p1% == on
eval p1 %speech%
global p1
set p2 on
global p2
end
end


end
end

if %actor.vnum% == 3100 && %speech% < 21
if %res4% == on
set num4 %speech%
global num4
unset res4
mgoto 3125
eval totale %num1%*%pre1%+%num2%*%pre2%+%num3%*%pre3%+%num4%*%pre4%
global totale
drop all
if %num1%+%num2%+%num3%+%num4% > 0
say Bene mi ha dato le seguenti cose:
wait 1 s
if %num1% > 0
say %num1% lingotti di ferro
end
if %num2% > 0
say %num2% lingotti di rame
end
if %num3% > 0
say %num3% lingotti di stagno
end
if %num4% >0
say %num4% lingotti di zinco
end
wait 1 s
say In totale le devo %totale% monete
give %totale% coins %name%
end
if %num1%+%num2%+%num3%+%num4% < 1
say Non mi ha dato nulla che io possa comprare
end
wait 1 s
say Arrivederci
wave
unset name
unset vend
end

if %res3% == on
set num3 %speech%
global num3
unset res3
set res4 on
global res4
end
if %res2% == on
set num2 %speech%
global num2
unset res2
set res3 on
global res3
end
if %res1% == on
eval num1 %speech%
global num1
unset res1
set res2 on
global res2
end
end




if %actor.vnum% == 3100
if %speech% == basta1
set ver1 off
global ver1
end
if %speech% == riprendi1
unset ver1
end
if %speech% == basta2
set ver2 off
global ver2
end
if %speech% == riprendi2
unset ver2
end
if %speech% == basta3
set ver3 off
global ver3
end
if %speech% == riprendi3
unset ver3
end
if %speech% == basta4
set ver4 off
global ver4
end
if %speech% == riprendi4
unset ver4
end
end
~
#3106
negoziante provvisorio comprare lingottaio~
0 bd 100
compro ferro rame stagno zinco finito~



if %vend% != on && %comp% != on && %comp& != off
If %speech% == compro
set name %actor.name%
global name
wait 1 s
say %name% , io vendo i seguenti prodotti :
wait 1 s
say Lingotti di ferro  
say Lingotti di rame
say Lingotti di stagno
say Lingotti di zinco
wait 1 s
say Che lingotti volete comprare?
set comp on
global comp
set timer on
set volta 0
global timer
global volta
end
end

if %comp% == on
if %speech% != compro && %actor.name% == %name% && %speech% != finito
set timer on
set volta 0
global timer
global volta
wait 1 s
say Va bene glieli vado a prendere
mgoto 3110
if %speech% == ferro
say mate1v
end
if %speech% == rame
say mate2v
end
if %speech% == stagno
say mate3v
end
if %speech% == zinco
say mate4v
end
take all.%speech%
mgoto 3125
wait 1 s
say Ecco , ora faccia 'list' e compri quello che desidera
wait 1 s
say Quando ha finito , per favore dica :'finito'
smi
set comp off
global comp
end
end

if %actor.name% == %name% && %comp% != on && %comp% != off
if %speech% == finito
unset volta
unset timer
wait 1 s
say Ok aspetti un attimo
mgoto 3110
say resetta
set res1 on
global res1
say mate1
if %ver1% != off
drop all.ferro
end
say mate2
if %ver2% != off
drop all.rame
end
say mate3
if %ver3% != off
drop all.stagno
end
say mate4
if %ver4% != off
drop all.zinco
end
say stop
end
end
if %volta% == 3 || (%speech% == finito && %actor.name% == %name% && %comp% == off)
wait 1 s
say Grazie e a presto %name%
unset name
unset comp
unset timer
unset volta
mgoto 3110
say mate1v
drop all.ferro
say mate2v
drop all.rame
say mate3v
drop all.stagno
say mate4v
drop all.zinco
wait 1 s
mgoto 3125
end

if %timer% == on && %comp% != off
if %volta% == 2
emote si spazientisce.
say Visto che non risponde non mi resta che salutarla
unset volta
unset timer
unset name
unset comp
unset vend
end
end
if %timer% == on
eval volta %volta% +1
global volta
end
~







#3107
negoziante provvisorio vendere assi legno~
0 d 1
vendo~
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %ver1% == off && %ver2% == off && %ver3% == off && %ver4% == off && %ver5% == off && %ver6% == off && %ver7% == off && %ver8% == off && %ver9% == off
say Mi spiace , non mi serve nulla
set full on
global full
end
end
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %full% != on
set name %actor.name%
global name
wait 1 s
say %name% , io compro i seguenti prodotti a questi prezzi :
wait 1 s
if %ver1% != off
say Assi di frassino  &1%pre1%&5 monete l'uno
end
if %ver2% != off
say Assi di mogano    &1%pre2%&5 monete l'uno
end
if %ver3% != off
say Assi di quercia   &1%pre3%&5 monete l'uno
end
if %ver4% != off
say Assi di castagno  &1%pre4%&5 monete l'uno
end
if %ver5% != off
say Assi di noce      &1%pre5%&5 monete l'uno
end
if %ver6% != off
say Assi di abete     &1%pre6%&5 monete l'uno
end
if %ver7% != off
say Assi di pioppo    &1%pre7%&5 monete l'uno
end
if %ver8% != off
say Assi di ciliegio  &1%pre8%&5 monete l'uno
end
if %ver9% != off
say Assi di betulla   &1%pre9%&5 monete l'uno
end
wait 1 s
say Mi dia tutto quello che vuole vendere (max 20 per tipo) e poi dica 'finito'
wait 1 s
say Attento , se mi da qualcosa di diverso , lo potrebbe perdere!
set timer on
set volta 0
global timer
global volta
set vend on
global vend
end
end
unset full
~
#3108
negoziante provvisorio vendere assi legno 2~
0 d 1
bilancio cambia 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 basta1 riprendi1 basta2 riprendi2 basta3 riprendi3 basta4 riprendi4 basta5 riprendi5 basta6 riprendi6 basta7 riprendi7 basta8 riprendi8 basta9 riprendi9~
if %actor.name% == Rodric || %actor.name% == Fizban || %actor.name% == %primo% || %actor.name% == %secondo% || %actor.name% == %terzo%

if %speech% == bilancio
wait 1 s
say Attualmente ho %self.gold% monete in cassa.
end
if %speech% == cambia && %actor.name% != %terzo%
wait 1 s
say %actor.name% , mi dica le cifre del nuovo prezzo (4 cifre) , una per volta , partendo dalle migliaia.
set camb on
global camb
set p1 on
global p1
end

if %speech% < 10
if %camb% == off
eval pre%speech% %tot%
global pre%speech%
wait 1 s
say Va bene %actor.name% , il nuovo prezzo per il prodotto &1%speech%&5 e' &1%tot%&0
unset camb
unset tot
unset p1
unset p2
unset p3
end

if %camb% == on
if %p4% == on
eval p4 %speech%
global p4
wait 1 s
eval tot %p1%*1000+%p2%*100+%p3%*10+%p4%
global tot
say Il nuovo prezzo e' quindi &1%tot%&0
wait 1 s
unset p4
say Di che prodotto vuole cambiare il prezzo?
set camb off
global camb
end
if %p3% == on
eval p3 %speech%
global p3
set p4 on
global p4
end
if %p2% == on
eval p2 %speech%
global p2
set p3 on
global p3
end
if %p1% == on
eval p1 %speech%
global p1
set p2 on
global p2
end
end


end
end

if %actor.vnum% == 3100 && %speech% < 21
if %res9% == on
set num9 %speech%
global num9
unset res9
mgoto 3136
eval totale %num1%*%pre1%+%num2%*%pre2%+%num3%*%pre3%+%num4%*%pre4%+%num5%*%pre5%+%num6%*%pre6%+%num7%*%pre7%+%num8%*%pre8%+%num9%*%pre9%
global totale
drop all
if %num1%+%num2%+%num3%+%num4%+%num5%+%num6%+%num7%+%num8%+%num9% > 0
say Bene mi ha dato le seguenti cose:
wait 1 s
if %num1% > 0
say %num1% assi di frassino
end
if %num2% >0
say %num2% assi di mogano
end
if %num3% > 0
say %num3% assi di quercia
end
if %num4% > 0
say %num4% assi di castagno
end
if %num5% > 0
say %num5% assi di noce
end
if %num6% >0
say %num6% assi di abete
end
if %num7% > 0
say %num7% assi di pioppo
end
if %num8% > 0
say %num8% assi di ciliegio
end
if %num9% > 0
say %num9% assi di betulla
end
wait 1 s
say In totale le devo %totale% monete
give %totale% coins %name%
end
if %num1%+%num2%+%num3%+%num4%+%num5%+%num6%+%num7%+%num8%+%num9% < 1
say Non mi ha dato nulla che io possa comprare
end
wait 1 s
say Arrivederci
wave
unset name
unset vend
end

if %res8% == on
set num8 %speech%
global num8
unset res8
set res9 on
global res9
end
if %res7% == on
set num7 %speech%
global num7
unset res7
set res8 on
global res8
end
if %res6% == on
set num6 %speech%
global num6
unset res6
set res7 on
global res7
end
if %res5% == on
set num5 %speech%
global num5
unset res5
set res6 on
global res6
end
if %res4% == on
set num4 %speech%
global num4
unset res4
set res5 on
global res5
end
if %res3% == on
set num3 %speech%
global num3
unset res3
set res4 on
global res4
end
if %res2% == on
set num2 %speech%
global num2
unset res2
set res3 on
global res3
end
if %res1% == on
eval num1 %speech%
global num1
unset res1
set res2 on
global res2
end
end
if %actor.vnum% == 3100
if %speech% == basta1
set ver1 off
global ver1
end
if %speech% == riprendi1
unset ver1
end
if %speech% == basta2
set ver2 off
global ver2
end
if %speech% == riprendi2
unset ver2
end
if %speech% == basta3
set ver3 off
global ver3
end
if %speech% == riprendi3
unset ver3
end
if %speech% == basta4
set ver4 off
global ver4
end
if %speech% == riprendi4
unset ver4
end
if %speech% == basta5
set ver5 off
global ver5
end
if %speech% == riprendi5
unset ver5
end
if %speech% == basta6
set ver6 off
global ver6
end
if %speech% == riprendi6
unset ver6
end
if %speech% == basta7
set ver7 off
global ver7
end
if %speech% == riprendi7
unset ver7
end
if %speech% == basta8
set ver8 off
global ver8
end
if %speech% == riprendi8
unset ver8
end
if %speech% == basta9
set ver9 off
global ver9
end
if %speech% == riprendi9
unset ver9
end
end
~
#3109
negoziante provvisorio comprare assi~
0 bd 100
compro frassino mogano quercia castagno noce abete pioppo ciliegio betulla finito~
if %vend% != on && %comp% != on && %comp& != off
If %speech% == compro
set name %actor.name%
global name
wait 1 s
say %name% , io vendo i seguenti prodotti :
wait 1 s
say Assi di frassino
say Assi di mogano
say Assi di quercia
say Assi di castagno
say Assi di noce
say Assi di abete
say Assi di pioppo
say Assi di ciliegio
say Assi di betulla
wait 1 s
say Che assi volete comprare?
set comp on
global comp
set timer on
set volta 0
global timer
global volta
end
end

if %comp% == on
if %speech% != compro && %actor.name% == %name% && %speech% != finito
set timer on
set volta 0
global timer
global volta
wait 1 s
say Va bene gliele vado a prendere
mgoto 3111
if %speech% == frassino
say mate1v
end
if %speech% == mogano
say mate2v
end
if %speech% == quercia
say mate3v
end
if %speech% == castagno
say mate4v
end
if %speech% == noce
say mate5v
end
if %speech% == abete
say mate6v
end
if %speech% == pioppo
say mate7v
end
if %speech% == ciliegio
say mate8v
end
if %speech% == betulla
say mate9v
end
take all.%speech%
mgoto 3136
wait 1 s
say Ecco , ora faccia 'list' e compri quello che desidera
wait 1 s
say Quando ha finito , per favore dica :'finito'
smi
set comp off
global comp
end
end

if %actor.name% == %name% && %comp% != on && %comp% != off
if %speech% == finito
unset volta
unset timer
wait 1 s
say Ok aspetti un attimo
mgoto 3111
say resetta
set res1 on
global res1
say mate1
if %ver1% != off
drop all.frassino
end
say mate2
if %ver2% != off
drop all.mogano
end
say mate3
if %ver3% != off
drop all.quercia
end
say mate4
if %ver4% != off
drop all.castagno
end
say mate5
if %ver5% != off
drop all.noce
end
say mate6
if %ver6% != off
drop all.abete
end
say mate7
if %ver7% != off
drop all.pioppo
end
say mate8
if %ver8% != off
drop all.ciliegio
end
say mate9
if %ver9% != off
drop all.betulla
end
say stop
end
end
if %volta% == 3 || (%speech% == finito && %actor.name% == %name% && %comp% == off)
wait 1 s
say Grazie e a presto %name%
unset name
unset comp
unset timer
unset volta
mgoto 3111
say mate1v
drop all.frassino
say mate2v
drop all.mogano
say mate3v
drop all.quercia
say mate4v
drop all.castagno
say mate5v
drop all.noce
say mate6v
drop all.abete
say mate7v
drop all.pioppo
say mate8v
drop all.ciliegio
say mate9v
drop all.betulla
wait 1 s
mgoto 3136
end

if %timer% == on && %comp% != off
if %volta% == 2
emote si spazientisce.
say Visto che non risponde non mi resta che salutarla
unset volta
unset timer
unset name
unset comp
unset vend
end
end
if %timer% == on
eval volta %volta% +1
global volta
end
~
#3110
negoziante provvisorio vendere travi legno~
0 d 1
vendo~
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %ver1% == off && %ver2% == off && %ver3% == off && %ver4% == off && %ver5% == off && %ver6% == off && %ver7% == off && %ver8% == off && %ver9% == off
say Mi spiace , non mi serve nulla
set full on
global full
end
end
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %full% != on
set name %actor.name%
global name
wait 1 s
say %name% , io compro i seguenti prodotti a questi prezzi :
wait 1 s
if %ver1% != off
say Travi di frassino  &1%pre1%&5 monete l'uno
end
if %ver2% != off
say Travi di mogano    &1%pre2%&5 monete l'uno
end
if %ver3% != off
say Travi di quercia   &1%pre3%&5 monete l'uno
end
if %ver4% != off
say Travi di castagno  &1%pre4%&5 monete l'uno
end
if %ver5% != off
say Travi di noce      &1%pre5%&5 monete l'uno
end
if %ver6% != off
say Travi di abete     &1%pre6%&5 monete l'uno
end
if %ver7% != off
say Travi di pioppo    &1%pre7%&5 monete l'uno
end
if %ver8% != off
say Travi di ciliegio  &1%pre8%&5 monete l'uno
end
if %ver9% != off
say Travi di betulla   &1%pre9%&5 monete l'uno
end
wait 1 s
say Mi dia tutto quello che vuole vendere (max 20 per tipo) e poi dica 'finito'
wait 1 s
say Attento , se mi da qualcosa di diverso , lo potrebbe perdere!
set timer on
set volta 0
global timer
global volta
set vend on
global vend
end
end
unset full
~
#3111
negoziante provvisorio vendere travi legno 2~
0 d 1
bilancio cambia 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 basta1 riprendi1 basta2 riprendi2 basta3 riprendi3 basta4 riprendi4 basta5 riprendi5 basta6 riprendi6 basta7 riprendi7 basta8 riprendi8 basta9 riprendi9~
if %actor.name% == Rodric || %actor.name% == Fizban || %actor.name% == %primo% || %actor.name% == %secondo% || %actor.name% == %terzo%

if %speech% == bilancio
wait 1 s
say Attualmente ho %self.gold% monete in cassa.
end
if %speech% == cambia && %actor.name% != %terzo%
wait 1 s
say %actor.name% , mi dica le cifre del nuovo prezzo (4 cifre) , una per volta , partendo dalle migliaia.
set camb on
global camb
set p1 on
global p1
end

if %speech% < 10
if %camb% == off
eval pre%speech% %tot%
global pre%speech%
wait 1 s
say Va bene %actor.name% , il nuovo prezzo per il prodotto &1%speech%&5 e' &1%tot%&0
unset camb
unset tot
unset p1
unset p2
unset p3
end

if %camb% == on
if %p4% == on
eval p4 %speech%
global p4
wait 1 s
eval tot %p1%*1000+%p2%*100+%p3%*10+%p4%
global tot
say Il nuovo prezzo e' quindi &1%tot%&0
wait 1 s
unset p4
say Di che prodotto vuole cambiare il prezzo?
set camb off
global camb
end
if %p3% == on
eval p3 %speech%
global p3
set p4 on
global p4
end
if %p2% == on
eval p2 %speech%
global p2
set p3 on
global p3
end
if %p1% == on
eval p1 %speech%
global p1
set p2 on
global p2
end
end


end
end

if %actor.vnum% == 3100 && %speech% < 21
if %res9% == on
set num9 %speech%
global num9
unset res9
mgoto 3126
eval totale %num1%*%pre1%+%num2%*%pre2%+%num3%*%pre3%+%num4%*%pre4%+%num5%*%pre5%+%num6%*%pre6%+%num7%*%pre7%+%num8%*%pre8%+%num9%*%pre9%
global totale
drop all
if %num1%+%num2%+%num3%+%num4%+%num5%+%num6%+%num7%+%num8%+%num9% > 0
say Bene mi ha dato le seguenti cose:
wait 1 s
if %num1% > 0
say %num1% travi di frassino
end
if %num2% > 0
say %num2% travi di mogano
end
if %num3% > 0
say %num3% travi di quercia
end
if %num4% > 0
say %num4% travi di castagno
end
if %num5% > 0
say %num5% travi di noce
end
if %num6% > 0
say %num6% travi di abete
end
if %num7% > 0
say %num7% travi di pioppo
end
if %num8% > 0
say %num8% travi di ciliegio
end
if %num9% > 0
say %num9% travi di betulla
end
wait 1 s
say In totale le devo %totale% monete
give %totale% coins %name%
end
if %num1%+%num2%+%num3%+%num4%+%num5%+%num6%+%num7%+%num8%+%num9% < 1
say Non mi ha dato nulla che io possa comprare
end
wait 1 s
say Arrivederci
wave
unset name
unset vend
end

if %res8% == on
set num8 %speech%
global num8
unset res8
set res9 on
global res9
end
if %res7% == on
set num7 %speech%
global num7
unset res7
set res8 on
global res8
end
if %res6% == on
set num6 %speech%
global num6
unset res6
set res7 on
global res7
end
if %res5% == on
set num5 %speech%
global num5
unset res5
set res6 on
global res6
end
if %res4% == on
set num4 %speech%
global num4
unset res4
set res5 on
global res5
end
if %res3% == on
set num3 %speech%
global num3
unset res3
set res4 on
global res4
end
if %res2% == on
set num2 %speech%
global num2
unset res2
set res3 on
global res3
end
if %res1% == on
eval num1 %speech%
global num1
unset res1
set res2 on
global res2
end
end




if %actor.vnum% == 3100
if %speech% == basta1
set ver1 off
global ver1
end
if %speech% == riprendi1
unset ver1
end
if %speech% == basta2
set ver2 off
global ver2
end
if %speech% == riprendi2
unset ver2
end
if %speech% == basta3
set ver3 off
global ver3
end
if %speech% == riprendi3
unset ver3
end
if %speech% == basta4
set ver4 off
global ver4
end
if %speech% == riprendi4
unset ver4
end
if %speech% == basta5
set ver5 off
global ver5
end
if %speech% == riprendi5
unset ver5
end
if %speech% == basta6
set ver6 off
global ver6
end
if %speech% == riprendi6
unset ver6
end
if %speech% == basta7
set ver7 off
global ver7
end
if %speech% == riprendi7
unset ver7
end
if %speech% == basta8
set ver8 off
global ver8
end
if %speech% == riprendi8
unset ver8
end
if %speech% == basta9
set ver9 off
global ver9
end
if %speech% == riprendi9
unset ver9
end
end
~
#3112
negoziante provvisorio comprare travi legno~
0 bd 100
compro frassino mogano quercia castagno noce abete pioppo ciliegio betulla finito~



if %vend% != on && %comp% != on && %comp& != off
If %speech% == compro
set name %actor.name%
global name
wait 1 s
say %name% , io vendo i seguenti prodotti :
wait 1 s
say Travi di frassino
say Travi di mogano
say Travi di quercia
say Travi di castagno
say Travi di noce
say Travi di abete
say Travi di pioppo
say Travi di ciliegio
say Travi di betulla
wait 1 s
say Che travi volete comprare?
set comp on
global comp
set timer on
set volta 0
global timer
global volta
end
end

if %comp% == on
if %speech% != compro && %actor.name% == %name% && %speech% != finito
set timer on
set volta 0
global timer
global volta
wait 1 s
say Va bene gliele vado a prendere
mgoto 3112
if %speech% == frassino
say mate1v
end
if %speech% == mogano
say mate2v
end
if %speech% == quercia
say mate3v
end
if %speech% == castagno
say mate4v
end
if %speech% == noce
say mate5v
end
if %speech% == abete
say mate6v
end
if %speech% == pioppo
say mate7v
end
if %speech% == ciliegio
say mate8v
end
if %speech% == betulla
say mate9v
end
take all.%speech%
mgoto 3126
wait 1 s
say Ecco , ora faccia 'list' e compri quello che desidera
wait 1 s
say Quando ha finito , per favore dica :'finito'
smi
set comp off
global comp
end
end

if %actor.name% == %name% && %comp% != on && %comp% != off
if %speech% == finito
unset volta
unset timer
wait 1 s
say Ok aspetti un attimo
mgoto 3112
say resetta
set res1 on
global res1
say mate1
if %ver1% != off
drop all.frassino
end
say mate2
if %ver2% != off
drop all.mogano
end
say mate3
if %ver3% != off
drop all.quercia
end
say mate4
if %ver4% != off
drop all.castagno
end
say mate5
if %ver5% != off
drop all.noce
end
say mate6
if %ver6% != off
drop all.abete
end
say mate7
if %ver7% != off
drop all.pioppo
end
say mate8
if %ver8% != off
drop all.ciliegio
end
say mate9
if %ver9% != off
drop all.betulla
end
say stop
end
end
if %volta% == 3 || (%speech% == finito && %actor.name% == %name% && %comp% == off)
wait 1 s
say Grazie e a presto %name%
unset name
unset comp
unset timer
unset volta
mgoto 3112
say mate1v
drop all.frassino
say mate2v
drop all.mogano
say mate3v
drop all.quercia
say mate4v
drop all.castagno
say mate5v
drop all.noce
say mate6v
drop all.abete
say mate7v
drop all.pioppo
say mate8v
drop all.ciliegio
say mate9v
drop all.betulla
wait 1 s
mgoto 3126
end

if %timer% == on && %comp% != off
if %volta% == 2
emote si spazientisce.
say Visto che non risponde non mi resta che salutarla
unset volta
unset timer
unset name
unset comp
unset vend
end
end
if %timer% == on
eval volta %volta% +1
global volta
end
~
#3113
negoziante provvisorio vendere travi metallo~
0 d 1
vendo~
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %ver1% == off && %ver2% == off && %ver3% == off && %ver4% == off
say Mi spiace , non mi serve nulla
set full on
globa full
end
end
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %full% != on
set name %actor.name%
global name
wait 1 s
say %name% , io compro i seguenti prodotti a questi prezzi :
wait 1 s
if %ver1% != off
say Travi di ferro  &1%pre1%&5 monete l'uno
end
if %ver2% != off
say Travi di rame   &1%pre2%&5 monete l'uno
end
if %ver3% != off
say Travi di stagno &1%pre3%&5 monete l'uno
end
if %ver4% != off
say Travi di zinco  &1%pre4%&5 monete l'uno
end

wait 1 s
say Mi dia tutto quello che vuole vendere (max 20 per tipo) e poi dica 'finito'
wait 1 s
say Attento , se mi da qualcosa di diverso , lo potrebbe perdere!
set timer on
set volta 0
global timer
global volta
set vend on
global vend
end
end
unset full

~
#3114
negoziante provvisorio vendere travi metallo 2~
0 d 1
bilancio cambia 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 basta1 riprendi1 basta2 riprendi2 basta3 riprendi3 basta4 riprendi4~
if %actor.name% == Rodric || %actor.name% == Fizban || %actor.name% == %primo% || %actor.name% == %secondo% || %actor.name% == %terzo%

if %speech% == bilancio
wait 1 s
say Attualmente ho %self.gold% monete in cassa.
end
if %speech% == cambia && %actor.name% != %terzo%
wait 1 s
say %actor.name% , mi dica le cifre del nuovo prezzo (4 cifre) , una per volta , partendo dalle migliaia.
set camb on
global camb
set p1 on
global p1
end

if %speech% < 10
if %camb% == off
eval pre%speech% %tot%
global pre%speech%
wait 1 s
say Va bene %actor.name% , il nuovo prezzo per il prodotto &1%speech%&5 e' &1%tot%&0
unset camb
unset tot
unset p1
unset p2
unset p3
end

if %camb% == on
if %p4% == on
eval p4 %speech%
global p4
wait 1 s
eval tot %p1%*1000+%p2%*100+%p3%*10+%p4%
global tot
say Il nuovo prezzo e' quindi &1%tot%&0
wait 1 s
unset p4
say Di che prodotto vuole cambiare il prezzo?
set camb off
global camb
end
if %p3% == on
eval p3 %speech%
global p3
set p4 on
global p4
end
if %p2% == on
eval p2 %speech%
global p2
set p3 on
global p3
end
if %p1% == on
eval p1 %speech%
global p1
set p2 on
global p2
end
end


end
end

if %actor.vnum% == 3100 && %speech% < 21
if %res4% == on
set num4 %speech%
global num4
unset res4
mgoto 3127
eval totale %num1%*%pre1%+%num2%*%pre2%+%num3%*%pre3%+%num4%*%pre4%
global totale
drop all
if %num1%+%num2%+%num3%+%num4% > 0
say Bene mi ha dato le seguenti cose:
wait 1 s
if %num1% > 0
say %num1% Travi di ferro
end
if %num2% > 0
say %num2% Travi di rame
end
if %num3% > 0
say %num3% Travi di stagno
end
if %num4% > 0
say %num4% Travi di zinco
end
wait 1 s
say In totale le devo %totale% monete
give %totale% coins %name%
end
if %num1%+%num2%+%num3%+%num4% < 1
say Non mi ha dato nulla che io possa comprare
end
wait 1 s
say Arrivederci
wave
unset name
unset vend
end

if %res3% == on
set num3 %speech%
global num3
unset res3
set res4 on
global res4
end
if %res2% == on
set num2 %speech%
global num2
unset res2
set res3 on
global res3
end
if %res1% == on
eval num1 %speech%
global num1
unset res1
set res2 on
global res2
end
end




if %actor.vnum% == 3100
if %speech% == basta1
set ver1 off
global ver1
end
if %speech% == riprendi1
unset ver1
end
if %speech% == basta2
set ver2 off
global ver2
end
if %speech% == riprendi2
unset ver2
end
if %speech% == basta3
set ver3 off
global ver3
end
if %speech% == riprendi3
unset ver3
end
if %speech% == basta4
set ver4 off
global ver4
end
if %speech% == riprendi4
unset ver4
end
end
~
#3115
negoziante provvisorio comprare travi metallo~
0 bd 100
compro ferro rame stagno zinco finito~



if %vend% != on && %comp% != on && %comp& != off
If %speech% == compro
set name %actor.name%
global name
wait 1 s
say %name% , io vendo i seguenti prodotti :
wait 1 s
say Travi di ferro  
say Travi di rame
say Travi di stagno
say Travi di zinco
wait 1 s
say Che travi volete comprare?
set comp on
global comp
set timer on
set volta 0
global timer
global volta
end
end

if %comp% == on
if %speech% != compro && %actor.name% == %name% && %speech% != finito
set timer on
set volta 0
global timer
global volta
wait 1 s
say Va bene gliele vado a prendere
mgoto 3113
if %speech% == ferro
say mate1v
end
if %speech% == rame
say mate2v
end
if %speech% == stagno
say mate3v
end
if %speech% == zinco
say mate4v
end
take all.%speech%
mgoto 3127
wait 1 s
say Ecco , ora faccia 'list' e compri quello che desidera
wait 1 s
say Quando ha finito , per favore dica :'finito'
smi
set comp off
global comp
end
end

if %actor.name% == %name% && %comp% != on && %comp% != off
if %speech% == finito
unset volta
unset timer
wait 1 s
say Ok aspetti un attimo
mgoto 3113
say resetta
set res1 on
global res1
say mate1
if %ver1% != off
drop all.ferro
end
say mate2
if %ver2% != off
drop all.rame
end
say mate3
if %ver3% != off
drop all.stagno
end
say mate4
if %ver4% != off
drop all.zinco
end
say stop
end
end
if %volta% == 3 || (%speech% == finito && %actor.name% == %name% && %comp% == off)
wait 1 s
say Grazie e a presto %name%
unset name
unset comp
unset timer
unset volta
mgoto 3113
say mate1v
drop all.ferro
say mate2v
drop all.rame
say mate3v
drop all.stagno
say mate4v
drop all.zinco
wait 1 s
mgoto 3127
end

if %timer% == on && %comp% != off
if %volta% == 2
emote si spazientisce.
say Visto che non risponde non mi resta che salutarla
unset volta
unset timer
unset name
unset comp
unset vend
end
end
if %timer% == on
eval volta %volta% +1
global volta
end
~



#3116
negoziante provvisorio vendere erborista1~
0 d 1
vendo~
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %ver1% == off && %ver2% == off && %ver3% == off && %ver4% == off && %ver5% == off && %ver6% == off && %ver7% == off
say Mi spiace , non mi serve nulla
set full on
global full
end
end
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %full% != on
set name %actor.name%
global name
wait 1 s
say %name% , io compro i seguenti prodotti a questi prezzi :
wait 1 s
if %ver1% != off
say Erbe pulite di assenzio      &1%pre1%&5 monete l'una
end
if %ver2% != off
say Erbe pulite di bardana       &1%pre2%&5 monete l'una
end
if %ver3% != off
say Erbe pulite di biancospino   &1%pre3%&5 monete l'una
end
if %ver4% != off
say Erbe pulite di borragine     &1%pre4%&5 monete l'una
end
if %ver5% != off
say Erbe pulite di calendula     &1%pre5%&5 monete l'una
end
if %ver6% != off
say Erbe pulite di celidonia     &1%pre6%&5 monete l'una
end
if %ver7% != off
say Erbe pulite di arnica        &1%pre7%&5 monete l'una
end
wait 1 s
say Le altre erbe le tratta Nicola , l'altro erborista.
wait 1 s
say Mi dia tutto quello che vuole vendere (max 20 per tipo) e poi dica 'finito'
wait 1 s
say Attento , se mi da qualcosa di diverso , lo potrebbe perdere!
set timer on
set volta 0
global timer
global volta
set vend on
global vend
end
end
unset full
~
#3117
negoziante provvisorio vendere erborista1 2~
0 d 1
bilancio cambia 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 basta1 riprendi1 basta2 riprendi2 basta3 riprendi3 basta4 riprendi4 basta5 riprendi5 basta6 riprendi6 basta7 riprendi7~
if %actor.name% == Rodric || %actor.name% == Fizban || %actor.name% == %primo% || %actor.name% == %secondo% || %actor.name% == %terzo%

if %speech% == bilancio
wait 1 s
say Attualmente ho %self.gold% monete in cassa.
end
if %speech% == cambia && %actor.name% != %terzo%
wait 1 s
say %actor.name% , mi dica le cifre del nuovo prezzo (4 cifre) , una per volta , partendo dalle migliaia.
set camb on
global camb
set p1 on
global p1
end

if %speech% < 10
if %camb% == off
eval pre%speech% %tot%
global pre%speech%
wait 1 s
say Va bene %actor.name% , il nuovo prezzo per il prodotto &1%speech%&5 e' &1%tot%&0
unset camb
unset tot
unset p1
unset p2
unset p3
end

if %camb% == on
if %p4% == on
eval p4 %speech%
global p4
wait 1 s
eval tot %p1%*1000+%p2%*100+%p3%*10+%p4%
global tot
say Il nuovo prezzo e' quindi &1%tot%&0
wait 1 s
unset p4
say Di che prodotto vuole cambiare il prezzo?
set camb off
global camb
end
if %p3% == on
eval p3 %speech%
global p3
set p4 on
global p4
end
if %p2% == on
eval p2 %speech%
global p2
set p3 on
global p3
end
if %p1% == on
eval p1 %speech%
global p1
set p2 on
global p2
end
end


end
end

if %actor.vnum% == 3100 && %speech% < 21
if %res7% == on
set num7 %speech%
global num7
unset res7
mgoto 3128
eval totale %num1%*%pre1%+%num2%*%pre2%+%num3%*%pre3%+%num4%*%pre4%+%num5%*%pre5%+%num6%*%pre6%+%num7%*%pre7%
global totale
drop all
if %num1%+%num2%+%num3%+%num4%+%num5%+%num6%+%num7% > 0
say Bene mi ha dato le seguenti cose:
wait 1 s
if %num1% > 0
say %num1% erbe pulite di assenzio
end
if %num2% > 0
say %num2% erbe pulite di bardana
end
if %num3% > 0
say %num3% erbe pulite di biancospino
end
if %num4% > 0
say %num4% erbe pulite di borragine
end
if %num5% > 0
say %num5% erbe pulite di calendula
end
if %num6% > 0
say %num6% erbe pulite di celidonia
end
if %num7% > 0
say %num7% erbe pulite di arnica
end
wait 1 s
say In totale le devo %totale% monete
give %totale% coins %name%
end
if %num1%+%num2%+%num3%+%num4%+%num5%+%num6%+%num7% < 1
say Non mi ha dato nulla che io possa comprare
end
wait 1 s
say Arrivederci
wave
unset name
unset vend
end


if %res6% == on
set num6 %speech%
global num6
unset res6
set res7 on
global res7
end
if %res5% == on
set num5 %speech%
global num5
unset res5
set res6 on
global res6
end
if %res4% == on
set num4 %speech%
global num4
unset res4
set res5 on
global res5
end
if %res3% == on
set num3 %speech%
global num3
unset res3
set res4 on
global res4
end
if %res2% == on
set num2 %speech%
global num2
unset res2
set res3 on
global res3
end
if %res1% == on
eval num1 %speech%
global num1
unset res1
set res2 on
global res2
end
end
if %actor.vnum% == 3100
if %speech% == basta1
set ver1 off
global ver1
end
if %speech% == riprendi1
unset ver1
end
if %speech% == basta2
set ver2 off
global ver2
end
if %speech% == riprendi2
unset ver2
end
if %speech% == basta3
set ver3 off
global ver3
end
if %speech% == riprendi3
unset ver3
end
if %speech% == basta4
set ver4 off
global ver4
end
if %speech% == riprendi4
unset ver4
end
if %speech% == basta5
set ver5 off
global ver5
end
if %speech% == riprendi5
unset ver5
end
if %speech% == basta6
set ver6 off
global ver6
end
if %speech% == riprendi6
unset ver6
end
if %speech% == basta7
set ver7 off
global ver7
end
if %speech% == riprendi7
unset ver7
end
end
~
#3118
negoziante provvisorio comprare erborista1~
0 bd 100
compro assenzio bardana biancospino borragine calendula celidonia arnica finito~
if %vend% != on && %comp% != on && %comp& != off
If %speech% == compro
set name %actor.name%
global name
wait 1 s
say %name% , io vendo i seguenti prodotti :
wait 1 s
say Erbe pulite di assenzio
say Erbe pulite di bardana
say Erbe pulite di biancospino
say Erbe pulite di borragine
say Erbe pulite di calendula
say Erbe pulite di celidonia
say Erbe pulite di arnica
wait 1 s
say Le altre erbe le tratta Nicola , l'altro erborista.
wait 1 s
say Che erbe volete comprare?
set comp on
global comp
set timer on
set volta 0
global timer
global volta
end
end

if %comp% == on
if %speech% != compro && %actor.name% == %name% && %speech% != finito
set timer on
set volta 0
global timer
global volta
wait 1 s
say Va bene gliele vado a prendere
mgoto 3114
if %speech% == assenzio
say mate1v
end
if %speech% == bardana
say mate2v
end
if %speech% == biancospino
say mate3v
end
if %speech% == borragine
say mate4v
end
if %speech% == calendula
say mate5v
end
if %speech% == celidonia
say mate6v
end
if %speech% == arnica
say mate7v
end
take all.%speech%
mgoto 3128
wait 1 s
say Ecco , ora faccia 'list' e compri quello che desidera
wait 1 s
say Quando ha finito , per favore dica :'finito'
smi
set comp off
global comp
end
end

if %actor.name% == %name% && %comp% != on && %comp% != off
if %speech% == finito
unset volta
unset timer
wait 1 s
say Ok aspetti un attimo
mgoto 3114
say resetta
set res1 on
global res1
say mate1
if %ver1% != off
drop all.assenzio
end
say mate2
if %ver2% != off
drop all.bardana
end
say mate3
if %ver3% != off
drop all.biancospino
end
say mate4
if %ver4% != off
drop all.borragine
end
say mate5
if %ver5% != off
drop all.calendula
end
say mate6
if %ver6% != off
drop all.celidonia
end
say mate7
if %ver7% != off
drop all.arnica
end
say stop
end
end
if %volta% == 3 || (%speech% == finito && %actor.name% == %name% && %comp% == off)
wait 1 s
say Grazie e a presto %name%
unset name
unset comp
unset timer
unset volta
mgoto 3114
say mate1v
drop all.assenzio
say mate2v
drop all.bardana
say mate3v
drop all.biancospino
say mate4v
drop all.borragine
say mate5v
drop all.calendula
say mate6v
drop all.celidonia
say mate7v
drop all.arnica
wait 1 s
mgoto 3128
end

if %timer% == on && %comp% != off
if %volta% == 2
emote si spazientisce.
say Visto che non risponde non mi resta che salutarla
unset volta
unset timer
unset name
unset comp
unset vend
end
end
if %timer% == on
eval volta %volta% +1
global volta
end
~

#3119
negoziante provvisorio vendere erborista2~
0 d 1
vendo~
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %ver1% == off && %ver2% == off && %ver3% == off && %ver4% == off && %ver5% == off && %ver6% == off && %ver7% == off
say Mi spiace , non mi serve nulla
set full on
global full
end
end
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %full% != on
set name %actor.name%
global name
wait 1 s
say %name% , io compro i seguenti prodotti a questi prezzi :
wait 1 s
if %ver1% != off
say Erbe pulite di ortica       &1%pre1%&5 monete l'una
end
if %ver2% != off
say Erbe pulite di alchimilla   &1%pre2%&5 monete l'una
end
if %ver3% != off
say Erbe pulite di achillea     &1%pre3%&5 monete l'una
end
if %ver4% != off
say Erbe pulite di enula        &1%pre4%&5 monete l'una
end
if %ver5% != off
say Erbe pulite di ginestra     &1%pre5%&5 monete l'una
end
if %ver6% != off
say Erbe pulite di vischio      &1%pre6%&5 monete l'una
end
if %ver7% != off
say Erbe pulite di belladonna   &1%pre7%&5 monete l'una
end
wait 1 s
say Le altre erbe le tratta Cesar , l'altro erborista.
wait 1 s
say Mi dia tutto quello che vuole vendere (max 20 per tipo) e poi dica 'finito'
wait 1 s
say Attento , se mi da qualcosa di diverso , lo potrebbe perdere!
set timer on
set volta 0
global timer
global volta
set vend on
global vend
end
end
unset full
~
#3120
negoziante provvisorio vendere erborista2 2~
0 d 1
bilancio cambia 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 basta1 riprendi1 basta2 riprendi2 basta3 riprendi3 basta4 riprendi4 basta5 riprendi5 basta6 riprendi6 basta7 riprendi7~
if %actor.name% == Rodric || %actor.name% == Fizban || %actor.name% == %primo% || %actor.name% == %secondo% || %actor.name% == %terzo%

if %speech% == bilancio
wait 1 s
say Attualmente ho %self.gold% monete in cassa.
end
if %speech% == cambia && %actor.name% != %terzo%
wait 1 s
say %actor.name% , mi dica le cifre del nuovo prezzo (4 cifre) , una per volta , partendo dalle migliaia.
set camb on
global camb
set p1 on
global p1
end

if %speech% < 10
if %camb% == off
eval pre%speech% %tot%
global pre%speech%
wait 1 s
say Va bene %actor.name% , il nuovo prezzo per il prodotto &1%speech%&5 e' &1%tot%&0
unset camb
unset tot
unset p1
unset p2
unset p3
end

if %camb% == on
if %p4% == on
eval p4 %speech%
global p4
wait 1 s
eval tot %p1%*1000+%p2%*100+%p3%*10+%p4%
global tot
say Il nuovo prezzo e' quindi &1%tot%&0
wait 1 s
unset p4
say Di che prodotto vuole cambiare il prezzo?
set camb off
global camb
end
if %p3% == on
eval p3 %speech%
global p3
set p4 on
global p4
end
if %p2% == on
eval p2 %speech%
global p2
set p3 on
global p3
end
if %p1% == on
eval p1 %speech%
global p1
set p2 on
global p2
end
end


end
end

if %actor.vnum% == 3100 && %speech% < 21
if %res7% == on
set num7 %speech%
global num7
unset res7
mgoto 3131
eval totale %num1%*%pre1%+%num2%*%pre2%+%num3%*%pre3%+%num4%*%pre4%+%num5%*%pre5%+%num6%*%pre6%+%num7%*%pre7%
global totale
drop all
if %num1%+%num2%+%num3%+%num4%+%num5%+%num6%+%num7% > 0
say Bene mi ha dato le seguenti cose:
wait 1 s
if %num1% > 0
say %num1% erbe pulite di ortica
end
if %num2% > 0
say %num2% erbe pulite di alchimilla
end
if %num3% > 0
say %num3% erbe pulite di achillea
end
if %num4% > 0
say %num4% erbe pulite di enula
end
if %num5% > 0
say %num5% erbe pulite di ginestra
end
if %num6% > 0
say %num6% erbe pulite di vischio
end
if %num7% > 0
say %num7% erbe pulite di belladonna
end
wait 1 s
say In totale le devo %totale% monete
give %totale% coins %name%
end
if %num1%+%num2%+%num3%+%num4%+%num5%+%num6%+%num7% < 1
say Non mi ha dato nulla che io possa comprare
end
wait 1 s
say Arrivederci
wave
unset name
unset vend
end


if %res6% == on
set num6 %speech%
global num6
unset res6
set res7 on
global res7
end
if %res5% == on
set num5 %speech%
global num5
unset res5
set res6 on
global res6
end
if %res4% == on
set num4 %speech%
global num4
unset res4
set res5 on
global res5
end
if %res3% == on
set num3 %speech%
global num3
unset res3
set res4 on
global res4
end
if %res2% == on
set num2 %speech%
global num2
unset res2
set res3 on
global res3
end
if %res1% == on
eval num1 %speech%
global num1
unset res1
set res2 on
global res2
end
end
if %actor.vnum% == 3100
if %speech% == basta1
set ver1 off
global ver1
end
if %speech% == riprendi1
unset ver1
end
if %speech% == basta2
set ver2 off
global ver2
end
if %speech% == riprendi2
unset ver2
end
if %speech% == basta3
set ver3 off
global ver3
end
if %speech% == riprendi3
unset ver3
end
if %speech% == basta4
set ver4 off
global ver4
end
if %speech% == riprendi4
unset ver4
end
if %speech% == basta5
set ver5 off
global ver5
end
if %speech% == riprendi5
unset ver5
end
if %speech% == basta6
set ver6 off
global ver6
end
if %speech% == riprendi6
unset ver6
end
if %speech% == basta7
set ver7 off
global ver7
end
if %speech% == riprendi7
unset ver7
end
end
~
#3121
negoziante provvisorio comprare erborista2~
0 bd 100
compro ortica alchimilla achillea enula ginestra vischio belladonna finito~
if %vend% != on && %comp% != on && %comp& != off
If %speech% == compro
set name %actor.name%
global name
wait 1 s
say %name% , io vendo i seguenti prodotti :
wait 1 s
say Erbe pulite di ortica
say Erbe pulite di alchimilla
say Erbe pulite di achillea
say Erbe pulite di enula
say Erbe pulite di ginestra
say Erbe pulite di vischio
say Erbe pulite di belladonna
wait 1 s
say Le altre erbe le tratta Cesar , l'altro erborista.
wait 1 s
say Che erbe volete comprare?
set comp on
global comp
set timer on
set volta 0
global timer
global volta
end
end

if %comp% == on
if %speech% != compro && %actor.name% == %name% && %speech% != finito
set timer on
set volta 0
global timer
global volta
wait 1 s
say Va bene gliele vado a prendere
mgoto 3115
if %speech% == ortica
say mate1v
end
if %speech% == alchimilla
say mate2v
end
if %speech% == achillea
say mate3v
end
if %speech% == enula
say mate4v
end
if %speech% == ginestra
say mate5v
end
if %speech% == vischio
say mate6v
end
if %speech% == belladonna
say mate7v
end
take all.%speech%
mgoto 3131
wait 1 s
say Ecco , ora faccia 'list' e compri quello che desidera
wait 1 s
say Quando ha finito , per favore dica :'finito'
smi
set comp off
global comp
end
end

if %actor.name% == %name% && %comp% != on && %comp% != off
if %speech% == finito
unset volta
unset timer
wait 1 s
say Ok aspetti un attimo
mgoto 3115
say resetta
set res1 on
global res1
say mate1
if %ver1% != off
drop all.ortica
end
say mate2
if %ver2% != off
drop all.alchimilla
end
say mate3
if %ver3% != off
drop all.achillea
end
say mate4
if %ver4% != off
drop all.enula
end
say mate5
if %ver5% != off
drop all.ginestra
end
say mate6
if %ver6% != off
drop all.vischio
end
say mate7
if %ver7% != off
drop all.belladonna
end
say stop
end
end
if %volta% == 3 || (%speech% == finito && %actor.name% == %name% && %comp% == off)
wait 1 s
say Grazie e a presto %name%
unset name
unset comp
unset timer
unset volta
mgoto 3115
say mate1v
drop all.ortica
say mate2v
drop all.alchimilla
say mate3v
drop all.achillea
say mate4v
drop all.enula
say mate5v
drop all.ginestra
say mate6v
drop all.vischio
say mate7v
drop all.belladonna
wait 1 s
mgoto 3131
end

if %timer% == on && %comp% != off
if %volta% == 2
emote si spazientisce.
say Visto che non risponde non mi resta che salutarla
unset volta
unset timer
unset name
unset comp
unset vend
end
end
if %timer% == on
eval volta %volta% +1
global volta
end
~


#3122
negoziante provvisorio vendere Pietre~
0 d 1
vendo~
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %ver1% == off && %ver2% == off && %ver3% == off
say Mi spiace , non mi serve nulla
set full on
globa full
end
end
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %full% != on
set name %actor.name%
global name
wait 1 s
say %name% , io compro i seguenti prodotti a questi prezzi :
wait 1 s
if %ver1% != off
say Pietre squadrate di granito  &1%pre1%&5 monete l'una
end
if %ver2% != off
say Pietre squadrate di marmo    &1%pre2%&5 monete l'una
end
if %ver3% != off
say Pietre squadrate di quarzo   &1%pre3%&5 monete l'una
end

wait 1 s
say Mi dia tutto quello che vuole vendere (max 20 per tipo) e poi dica 'finito'
wait 1 s
say Attento , se mi da qualcosa di diverso , lo potrebbe perdere!
set timer on
set volta 0
global timer
global volta
set vend on
global vend
end
end
unset full

~
#3123
negoziante provvisorio vendere Pietre 2~
0 d 1
bilancio cambia 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 basta1 riprendi1 basta2 riprendi2 basta3 riprendi3~
if %actor.name% == Rodric || %actor.name% == Fizban || %actor.name% == %primo% || %actor.name% == %secondo% || %actor.name% == %terzo%

if %speech% == bilancio
wait 1 s
say Attualmente ho %self.gold% monete in cassa.
end
if %speech% == cambia && %actor.name% != %terzo%
wait 1 s
say %actor.name% , mi dica le cifre del nuovo prezzo (4 cifre) , una per volta , partendo dalle migliaia.
set camb on
global camb
set p1 on
global p1
end

if %speech% < 10
if %camb% == off
eval pre%speech% %tot%
global pre%speech%
wait 1 s
say Va bene %actor.name% , il nuovo prezzo per il prodotto &1%speech%&5 e' &1%tot%&0
unset camb
unset tot
unset p1
unset p2
unset p3
end

if %camb% == on
if %p4% == on
eval p4 %speech%
global p4
wait 1 s
eval tot %p1%*1000+%p2%*100+%p3%*10+%p4%
global tot
say Il nuovo prezzo e' quindi &1%tot%&0
wait 1 s
unset p4
say Di che prodotto vuole cambiare il prezzo?
set camb off
global camb
end
if %p3% == on
eval p3 %speech%
global p3
set p4 on
global p4
end
if %p2% == on
eval p2 %speech%
global p2
set p3 on
global p3
end
if %p1% == on
eval p1 %speech%
global p1
set p2 on
global p2
end
end


end
end

if %actor.vnum% == 3100 && %speech% < 21
if %res3% == on
set num3 %speech%
global num3
unset res3
mgoto 3135
eval totale %num1%*%pre1%+%num2%*%pre2%+%num3%*%pre3%
global totale
drop all
if %num1%+%num2%+%num3% > 0
say Bene mi ha dato le seguenti cose:
wait 1 s
if %num1% > 0
say %num1% pietre squadrate di granito
end
if %num2% > 0
say %num2% pietre squadrate di marmo
end
if %num3% > 0
say %num3% pietre squadrate di quarzo
end
wait 1 s
say In totale le devo %totale% monete
give %totale% coins %name%
end
if %num1%+%num2%+%num3% < 1
say Non mi ha dato nulla che io possa comprare
end
wait 1 s
say Arrivederci
wave
unset name
unset vend
end

if %res2% == on
set num2 %speech%
global num2
unset res2
set res3 on
global res3
end
if %res1% == on
eval num1 %speech%
global num1
unset res1
set res2 on
global res2
end
end




if %actor.vnum% == 3100
if %speech% == basta1
set ver1 off
global ver1
end
if %speech% == riprendi1
unset ver1
end
if %speech% == basta2
set ver2 off
global ver2
end
if %speech% == riprendi2
unset ver2
end
if %speech% == basta3
set ver3 off
global ver3
end
if %speech% == riprendi3
unset ver3
end
end
~
#3124
negoziante provvisorio comprare Pietre~
0 bd 100
compro granito marmo quarzo finito~



if %vend% != on && %comp% != on && %comp& != off
If %speech% == compro
set name %actor.name%
global name
wait 1 s
say %name% , io vendo i seguenti prodotti :
wait 1 s
say Pietre squadrate di granito  
say Pietre squadrate di marmo
say Pietre squadrate di quarzo
wait 1 s
say Che pietre volete comprare?
set comp on
global comp
set timer on
set volta 0
global timer
global volta
end
end

if %comp% == on
if %speech% != compro && %actor.name% == %name% && %speech% != finito
set timer on
set volta 0
global timer
global volta
wait 1 s
say Va bene gliele vado a prendere
mgoto 3147
if %speech% == granito
say mate1v
end
if %speech% == marmo
say mate2v
end
if %speech% == quarzo
say mate3v
end

take all.%speech%
mgoto 3135
wait 1 s
say Ecco , ora faccia 'list' e compri quello che desidera
wait 1 s
say Quando ha finito , per favore dica :'finito'
smi
set comp off
global comp
end
end

if %actor.name% == %name% && %comp% != on && %comp% != off
if %speech% == finito
unset volta
unset timer
wait 1 s
say Ok aspetti un attimo
mgoto 3147
say resetta
set res1 on
global res1
say mate1
if %ver1% != off
drop all.granito
end
say mate2
if %ver2% != off
drop all.marmo
end
say mate3
if %ver3% != off
drop all.quarzo
end
say stop
end
end
if %volta% == 3 || (%speech% == finito && %actor.name% == %name% && %comp% == off)
wait 1 s
say Grazie e a presto %name%
unset name
unset comp
unset timer
unset volta
mgoto 3147
say mate1v
drop all.granito
say mate2v
drop all.marmo
say mate3v
drop all.quarzo
wait 1 s
mgoto 3135
end

if %timer% == on && %comp% != off
if %volta% == 2
emote si spazientisce.
say Visto che non rispondi non mi resta che salutarla
unset volta
unset timer
unset name
unset comp
unset vend
end
end
if %timer% == on
eval volta %volta% +1
global volta
end
~



#3125
negoziante provvisorio vendere gioielli~
0 d 1
vendo~
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %ver1% == off && %ver2% == off && %ver3% == off && %ver4% == off && %ver5% == off
say Mi spiace , non mi serve nulla
set full on
globa full
end
end
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %full% != on
set name %actor.name%
global name
wait 1 s
say %name% , io compro i seguenti prodotti a questi prezzi :
wait 1 s
if %ver1% != off
say Pietre pulite di diamante  &1%pre1%&5 monete l'una
end
if %ver2% != off
say Pietre pulite di topazio   &1%pre2%&5 monete l'una
end
if %ver3% != off
say Pietre pulite di smeraldo  &1%pre3%&5 monete l'una
end
if %ver4% != off
say Pietre pulite di rubino    &1%pre4%&5 monete l'una
end
if %ver5% != off
say Pietre pulite di zaffiro   &1%pre5%&5 monete l'una
end

wait 1 s
say Mi dia tutto quello che vuole vendere (max 20 per tipo) e poi dica 'finito'
wait 1 s
say Attento , se mi da qualcosa di diverso , lo potrebbe perdere!
set timer on
set volta 0
global timer
global volta
set vend on
global vend
end
end
unset full

~
#3126
negoziante provvisorio vendere gioielli 2~
0 d 1
bilancio cambia 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 basta1 riprendi1 basta2 riprendi2 basta3 riprendi3 basta4 riprendi4 basta5 riprendi5~
if %actor.name% == Rodric || %actor.name% == Fizban || %actor.name% == %primo% || %actor.name% == %secondo% || %actor.name% == %terzo%

if %speech% == bilancio
wait 1 s
say Attualmente ho %self.gold% monete in cassa.
end
if %speech% == cambia && %actor.name% != %terzo%
wait 1 s
say %actor.name% , mi dica le cifre del nuovo prezzo (4 cifre) , una per volta , partendo dalle migliaia.
set camb on
global camb
set p1 on
global p1
end

if %speech% < 10
if %camb% == off
eval pre%speech% %tot%
global pre%speech%
wait 1 s
say Va bene %actor.name% , il nuovo prezzo per il prodotto &1%speech%&5 e' &1%tot%&0
unset camb
unset tot
unset p1
unset p2
unset p3
end

if %camb% == on
if %p4% == on
eval p4 %speech%
global p4
wait 1 s
eval tot %p1%*1000+%p2%*100+%p3%*10+%p4%
global tot
say Il nuovo prezzo e' quindi &1%tot%&0
wait 1 s
unset p4
say Di che prodotto vuole cambiare il prezzo?
set camb off
global camb
end
if %p3% == on
eval p3 %speech%
global p3
set p4 on
global p4
end
if %p2% == on
eval p2 %speech%
global p2
set p3 on
global p3
end
if %p1% == on
eval p1 %speech%
global p1
set p2 on
global p2
end
end


end
end

if %actor.vnum% == 3100 && %speech% < 21
if %res5% == on
set num5 %speech%
global num5
unset res5
mgoto 3134
eval totale %num1%*%pre1%+%num2%*%pre2%+%num3%*%pre3%+%num4%*%pre4%+%num5%*%pre5%
global totale
drop all
if %num1%+%num2%+%num3%+%num4%+%num5% > 0
say Bene mi ha dato le seguenti cose:
wait 1 s
if %num1% > 0
say %num1% pietre pulite di diamante
end
if %num2% > 0
say %num2% pietre pulite di topazio
end
if %num3% > 0
say %num3% pietre pulite di smeraldo
end
if %num4% >0
say %num4% pietre pulite di rubino
end
if %num5% > 0
say %num5% pietre pulite di zaffiro
end

wait 1 s
say In totale le devo %totale% monete
give %totale% coins %name%
end
if %num1%+%num2%+%num3%+%num4%+%num5% < 1
say Non mi ha dato nulla che io possa comprare
end
wait 1 s
say Arrivederci
wave
unset name
unset vend
end


if %res4% == on
set num4 %speech%
global num4
unset res4
set res5 on
global res5
end
if %res3% == on
set num3 %speech%
global num3
unset res3
set res4 on
global res4
end
if %res2% == on
set num2 %speech%
global num2
unset res2
set res3 on
global res3
end
if %res1% == on
eval num1 %speech%
global num1
unset res1
set res2 on
global res2
end
end




if %actor.vnum% == 3100
if %speech% == basta1
set ver1 off
global ver1
end
if %speech% == riprendi1
unset ver1
end
if %speech% == basta2
set ver2 off
global ver2
end
if %speech% == riprendi2
unset ver2
end
if %speech% == basta3
set ver3 off
global ver3
end
if %speech% == riprendi3
unset ver3
end
if %speech% == basta4
set ver4 off
global ver4
end
if %speech% == riprendi4
unset ver4
end
if %speech% == basta5
set ver5 off
global ver5
end
if %speech% == riprendi5
unset ver5
end

end
~
#3127
negoziante provvisorio comprare gioielli~
0 bd 100
compro diamante topazio smeraldo rubino zaffiro finito~



if %vend% != on && %comp% != on && %comp& != off
If %speech% == compro
set name %actor.name%
global name
wait 1 s
say %name% , io vendo i seguenti prodotti :
wait 1 s
say Pietre pulite di diamante
say Pietre pulite di topazio
say Pietre pulite di smeraldo
say Pietre pulite di rubino
say Pietre pulite di zaffiro
wait 1 s
say Che pietre volete comprare?
set comp on
global comp
set timer on
set volta 0
global timer
global volta
end
end

if %comp% == on
if %speech% != compro && %actor.name% == %name% && %speech% != finito
set timer on
set volta 0
global timer
global volta
wait 1 s
say Va bene gliele vado a prendere
mgoto 3146
if %speech% == diamante
say mate1v
end
if %speech% == topazio
say mate2v
end
if %speech% == smeraldo
say mate3v
end
if %speech% == rubino
say mate4v
end
if %speech% == zaffiro
say mate5v
end

take all.%speech%
mgoto 3134
wait 1 s
say Ecco , ora faccia 'list' e compri quello che desidera
wait 1 s
say Quando ha finito , per favore dica :'finito'
smi
set comp off
global comp
end
end

if %actor.name% == %name% && %comp% != on && %comp% != off
if %speech% == finito
unset volta
unset timer
wait 1 s
say Ok aspetti un attimo
mgoto 3146
say resetta
set res1 on
global res1
say mate1
if %ver1% != off
drop all.diamante
end
say mate2
if %ver2% != off
drop all.topazio
end
say mate3
if %ver3% != off
drop all.smeraldo
end
say mate4
if %ver4% != off
drop all.rubino
end
say mate5
if %ver5% != off
drop all.zaffiro
end
say stop
end
end
if %volta% == 3 || (%speech% == finito && %actor.name% == %name% && %comp% == off)
wait 1 s
say Grazie e a presto %name%
unset name
unset comp
unset timer
unset volta
mgoto 3146
say mate1v
drop all.diamante
say mate2v
drop all.topazio
say mate3v
drop all.smeraldo
say mate4v
drop all.rubino
say mate5v
drop all.zaffiro
wait 1 s
mgoto 3134
end

if %timer% == on && %comp% != off
if %volta% == 2
emote si spazientisce.
say Visto che non risponde non mi resta che salutarla
unset volta
unset timer
unset name
unset comp
unset vend
end
end
if %timer% == on
eval volta %volta% +1
global volta
end
~



#3128
negoziante provvisorio vendere Natura~
0 d 1
vendo~
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %ver1% == off && %ver2% == off && %ver3% == off
say Mi spiace , non mi serve nulla
set full on
globa full
end
end
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %full% != on
set name %actor.name%
global name
wait 1 s
say %name% , io compro i seguenti prodotti a questi prezzi :
wait 1 s
if %ver1% != off
say Sacchetti di farina             &1%pre1%&5 monete l'uno
end
if %ver2% != off
say Prodotti alimentari di verdura  &1%pre2%&5 monete l'uno
end
if %ver3% != off
say Carne da cucinare               &1%pre3%&5 monete l'uno
end

wait 1 s
say Mi dia tutto quello che vuole vendere (max 20 per tipo) e poi dica 'finito'
wait 1 s
say Attento , se mi da qualcosa di diverso , lo potrebbe perdere!
set timer on
set volta 0
global timer
global volta
set vend on
global vend
end
end
unset full

~
#3129
negoziante provvisorio vendere Natura 2~
0 d 1
bilancio cambia 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 basta1 riprendi1 basta2 riprendi2 basta3 riprendi3~
if %actor.name% == Rodric || %actor.name% == Fizban || %actor.name% == %primo% || %actor.name% == %secondo% || %actor.name% == %terzo%

if %speech% == bilancio
wait 1 s
say Attualmente ho %self.gold% monete in cassa.
end
if %speech% == cambia && %actor.name% != %terzo%
wait 1 s
say %actor.name% , mi dica le cifre del nuovo prezzo (4 cifre) , una per volta , partendo dalle migliaia.
set camb on
global camb
set p1 on
global p1
end

if %speech% < 10
if %camb% == off
eval pre%speech% %tot%
global pre%speech%
wait 1 s
say Va bene %actor.name% , il nuovo prezzo per il prodotto &1%speech%&5 e' &1%tot%&0
unset camb
unset tot
unset p1
unset p2
unset p3
end

if %camb% == on
if %p4% == on
eval p4 %speech%
global p4
wait 1 s
eval tot %p1%*1000+%p2%*100+%p3%*10+%p4%
global tot
say Il nuovo prezzo e' quindi &1%tot%&0
wait 1 s
unset p4
say Di che prodotto vuole cambiare il prezzo?
set camb off
global camb
end
if %p3% == on
eval p3 %speech%
global p3
set p4 on
global p4
end
if %p2% == on
eval p2 %speech%
global p2
set p3 on
global p3
end
if %p1% == on
eval p1 %speech%
global p1
set p2 on
global p2
end
end


end
end

if %actor.vnum% == 3100 && %speech% < 21
if %res3% == on
set num3 %speech%
global num3
unset res3
mgoto 3133
eval totale %num1%*%pre1%+%num2%*%pre2%+%num3%*%pre3%
global totale
drop all
if %num1%+%num2%+%num3% > 0
say Bene mi ha dato le seguenti cose:
wait 1 s
if %num1% > 0
say %num1% sacchetti di farina
end
if %num2% > 0
say %num2% prodotti alimentari di verdura
end
if %num3% > 0
say %num3% pezzi di carne da cucinare
end
wait 1 s
say In totale le devo %totale% monete
give %totale% coins %name%
end
if %num1%+%num2%+%num3% < 1
say Non mi ha dato nulla che io possa comprare
end
wait 1 s
say Arrivederci
wave
unset name
unset vend
end

if %res2% == on
set num2 %speech%
global num2
unset res2
set res3 on
global res3
end
if %res1% == on
eval num1 %speech%
global num1
unset res1
set res2 on
global res2
end
end




if %actor.vnum% == 3100
if %speech% == basta1
set ver1 off
global ver1
end
if %speech% == riprendi1
unset ver1
end
if %speech% == basta2
set ver2 off
global ver2
end
if %speech% == riprendi2
unset ver2
end
if %speech% == basta3
set ver3 off
global ver3
end
if %speech% == riprendi3
unset ver3
end
end
~
#3130
negoziante provvisorio comprare Natura~
0 bd 100
compro farina verdura carne finito~



if %vend% != on && %comp% != on && %comp& != off
If %speech% == compro
set name %actor.name%
global name
wait 1 s
say %name% , io vendo i seguenti prodotti :
wait 1 s
say Sacchetti di farina  
say Prodotti alimentari di verdura
say Carne da cucinare
wait 1 s
say Che prodotto volete comprare: farina,verdura o carne? 
set comp on
global comp
set timer on
set volta 0
global timer
global volta
end
end

if %comp% == on
if %speech% != compro && %actor.name% == %name% && %speech% != finito
set timer on
set volta 0
global timer
global volta
wait 1 s
say Va bene gliele vado a prendere
mgoto 3145
if %speech% == farina
say mate1v
end
if %speech% == verdura
say mate2v
end
if %speech% == carne
say mate3v
end

take all.%speech%
mgoto 3133
wait 1 s
say Ecco , ora faccia 'list' e compri quello che desidera
wait 1 s
say Quando ha finito , per favore dica :'finito'
smi
set comp off
global comp
end
end

if %actor.name% == %name% && %comp% != on && %comp% != off
if %speech% == finito
unset volta
unset timer
wait 1 s
say Ok aspetti un attimo
mgoto 3145
say resetta
set res1 on
global res1
say mate1
if %ver1% != off
drop all.farina
end
say mate2
if %ver2% != off
drop all.verdura
end
say mate3
if %ver3% != off
drop all.carne
end
say stop
end
end
if %volta% == 3 || (%speech% == finito && %actor.name% == %name% && %comp% == off)
wait 1 s
say Grazie e a presto %name%
unset name
unset comp
unset timer
unset volta
mgoto 3145
say mate1v
drop all.farina
say mate2v
drop all.verdura
say mate3v
drop all.carne
wait 1 s
mgoto 3133
end

if %timer% == on && %comp% != off
if %volta% == 2
emote si spazientisce.
say Visto che non rispondi non mi resta che salutarla
unset volta
unset timer
unset name
unset comp
unset vend
end
end
if %timer% == on
eval volta %volta% +1
global volta
end
~


#3131
negoziante provvisorio vendere Caccia~
0 d 1
vendo~
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %ver1% == off && %ver2% == off && %ver3% == off
say Mi spiace , non mi serve nulla
set full on
globa full
end
end
if %vend% != on && %comp% != on && %comp% != off
If %speech% == vendo && %full% != on
set name %actor.name%
global name
wait 1 s
say %name% , io compro i seguenti prodotti a questi prezzi :
wait 1 s
if %ver1% != off
say Pelli lavorate     &1%pre1%&5 monete l'uno
end
if %ver2% != off
say Stoffe lavorate    &1%pre2%&5 monete l'uno
end
if %ver3% != off
say Pellicce lavorate  &1%pre3%&5 monete l'uno
end

wait 1 s
say Mi dia tutto quello che vuole vendere (max 20 per tipo) e poi dica 'finito'
wait 1 s
say Attento , se mi da qualcosa di diverso , lo potrebbe perdere!
set timer on
set volta 0
global timer
global volta
set vend on
global vend
end
end
unset full

~
#3132
negoziante provvisorio vendere Caccia 2~
0 d 1
bilancio cambia 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 basta1 riprendi1 basta2 riprendi2 basta3 riprendi3~
if %actor.name% == Rodric || %actor.name% == Fizban || %actor.name% == %primo% || %actor.name% == %secondo% || %actor.name% == %terzo%

if %speech% == bilancio
wait 1 s
say Attualmente ho %self.gold% monete in cassa.
end
if %speech% == cambia && %actor.name% != %terzo%
wait 1 s
say %actor.name% , mi dica le cifre del nuovo prezzo (4 cifre) , una per volta , partendo dalle migliaia.
set camb on
global camb
set p1 on
global p1
end

if %speech% < 10
if %camb% == off
eval pre%speech% %tot%
global pre%speech%
wait 1 s
say Va bene %actor.name% , il nuovo prezzo per il prodotto &1%speech%&5 e' &1%tot%&0
unset camb
unset tot
unset p1
unset p2
unset p3
end

if %camb% == on
if %p4% == on
eval p4 %speech%
global p4
wait 1 s
eval tot %p1%*1000+%p2%*100+%p3%*10+%p4%
global tot
say Il nuovo prezzo e' quindi &1%tot%&0
wait 1 s
unset p4
say Di che prodotto vuole cambiare il prezzo?
set camb off
global camb
end
if %p3% == on
eval p3 %speech%
global p3
set p4 on
global p4
end
if %p2% == on
eval p2 %speech%
global p2
set p3 on
global p3
end
if %p1% == on
eval p1 %speech%
global p1
set p2 on
global p2
end
end


end
end

if %actor.vnum% == 3100 && %speech% < 21
if %res3% == on
set num3 %speech%
global num3
unset res3
mgoto 3132
eval totale %num1%*%pre1%+%num2%*%pre2%+%num3%*%pre3%
global totale
drop all
if %num1%+%num2%+%num3% > 0
say Bene mi ha dato le seguenti cose:
wait 1 s
if %num1% > 0
say %num1% pelli lavorate
end
if %num2% > 0
say %num2% stoffe lavorate
end
if %num3% > 0
say %num3% pellicce lavorate
end
wait 1 s
say In totale le devo %totale% monete
give %totale% coins %name%
end
if %num1%+%num2%+%num3% < 1
say Non mi ha dato nulla che io possa comprare
end
wait 1 s
say Arrivederci
wave
unset name
unset vend
end

if %res2% == on
set num2 %speech%
global num2
unset res2
set res3 on
global res3
end
if %res1% == on
eval num1 %speech%
global num1
unset res1
set res2 on
global res2
end
end




if %actor.vnum% == 3100
if %speech% == basta1
set ver1 off
global ver1
end
if %speech% == riprendi1
unset ver1
end
if %speech% == basta2
set ver2 off
global ver2
end
if %speech% == riprendi2
unset ver2
end
if %speech% == basta3
set ver3 off
global ver3
end
if %speech% == riprendi3
unset ver3
end
end
~
#3133
negoziante provvisorio comprare Caccia~
0 bd 100
compro pelli stoffe pellicce finito~



if %vend% != on && %comp% != on && %comp& != off
If %speech% == compro
set name %actor.name%
global name
wait 1 s
say %name% , io vendo i seguenti prodotti :
wait 1 s
say Pelli lavorate  
say Stoffe lavorate
say Pellicce lavorate
wait 1 s
say Che prodotto volete comprare: pelle,stoffa o pelliccia? 
set comp on
global comp
set timer on
set volta 0
global timer
global volta
end
end

if %comp% == on
if %speech% != compro && %actor.name% == %name% && %speech% != finito
set timer on
set volta 0
global timer
global volta
wait 1 s
say Va bene gliele vado a prendere
mgoto 3144
if %speech% == pelle
say mate1v
end
if %speech% == stoffa
say mate2v
end
if %speech% == pelliccia
say mate3v
end

take all.%speech%
mgoto 3132
wait 1 s
say Ecco , ora faccia 'list' e compri quello che desidera
wait 1 s
say Quando ha finito , per favore dica :'finito'
smi
set comp off
global comp
end
end

if %actor.name% == %name% && %comp% != on && %comp% != off
if %speech% == finito
unset volta
unset timer
wait 1 s
say Ok aspetti un attimo
mgoto 3144
say resetta
set res1 on
global res1
say mate1
if %ver1% != off
drop all.pelle
end
say mate2
if %ver2% != off
drop all.stoffa
end
say mate3
if %ver3% != off
drop all.pelliccia
end
say stop
end
end
if %volta% == 3 || (%speech% == finito && %actor.name% == %name% && %comp% == off)
wait 1 s
say Grazie e a presto %name%
unset name
unset comp
unset timer
unset volta
mgoto 3144
say mate1v
drop all.pelle
say mate2v
drop all.stoffa
say mate3v
drop all.pelliccia
wait 1 s
mgoto 3132
end

if %timer% == on && %comp% != off
if %volta% == 2
emote si spazientisce.
say Visto che non rispondi non mi resta che salutarla
unset volta
unset timer
unset name
unset comp
unset vend
end
end
if %timer% == on
eval volta %volta% +1
global volta
end
~


#3140
cambia funzionari~
0 d 1
funzionari annulla primo secondo terzo salve~
if %actor.name% == Rodric
if %speech% == annulla
unset funzio
set new off
global new
end
if %speech% == funzionari && %funzio% != on
wait 1 s
say I funzionari da cui prendo ordini , oltre a Lei Maestà , sono:
wait 1 s
say 1)&1%primo%
say 2)&1%secondo%
say 3)&1%terzo%
wait 1 s
say Quale vuole cambiare , il &1primo&5 , il &1secondo&5 o il &1terzo&5 ?
set funzio on
global funzio
end
if %speech% != funzionari && %speech% != annulla && %speech% != salve
if %funzio% == on
wait 1 s
say Va bene , allora il nuovo funzionario mi saluti.(Say &1Salve&5)
set new %speech%
global new
end
end
end
if %new% != off
if %speech% == Salve
set %new% %actor.name%
global %new%
wait 1 s
say Va bene , ora prendo oredini anche da lei %actor.name%
unset funzio
set new off
global new
end
end

~
#3145
soldi~
0 d 1
soldi~
say ho %roland.gold% monete

~

#3150
ufficiale mercato interno~
0 d 1
affittare comprare si no bancarella magazzino~
if %affi2% == banc
if %speech% == si && %actor.name% == %name%
if %name.gold% < 1000
waut 2 s
say Non te lo puoi permettere , arrivederci.
unset affi
unset affi2
unset name
unset timer2
unset volta
end
if %name.gold% >= 1000
mforce %name% give 1000 coins ufficiale
wait 2 s
emot riscuote le 1000 monete.
wait 1 s
say Bene , comunico la tua richiesta al responsabile del mercato , riceverai presto la risposta tramite lettera.
wait 2 s
say Arrivederci
mgoto 3101
say Registra
say %name% ha affittato una bancarella
give all.coins segre
mgoto 3102
unset affi
unset affi2
unset name
unset timer2
unset volta
end
end
if %speech% == no && %actor.name% == %name%
wait 2 s
say Va bene arrivederci
unset affi
unset affi2
unset name
unset timer2
unset volta
end
else
end
if %affi2% == maga
if %speech% == si && %actor.name% == %name%
if %name.gold% < 2000
wait 2 s
say Non te lo puoi permettere , arrivederci.
unset affi
unset affi2
unset name
unset timer2
unset volta
end
if %name.gold% >= 2000
mforce %.name% give 2000 coins ufficiale
wait 1 s
emote riscuote le 2000 monete.
wait 2 s
say Bene , comunico la tua richiesta al responsabile del mercato , riceverai presto la risposta tramite lettera.
wait 2 s
say Arrivederci
mgoto 3101
say Registra
say %name% ha affittato un magazzino
give all.coins segre
mgoto 3102
unset affi
unset affi2
unset name
unset timer2
unset volta
end
end
if %speech% == no && %actor.name% == %name%
wait 2 s
say Va bene arrivederci
unset affi
unset affi2
unset name
unset timer2
unset volta
end
end

if %affi% == on && %affi2% != maga && %affi2% != banc
if %speech% == bancarella && %actor.name% == %name%
wait 2 s
emote sfoglia il suo registro.
wait 2 s
say Prendere in affitto una bancarella per una settimana costa 1000 monete da versare subito , accetti?
set affi2 banc
global affi2
set timer2 on
global timer2
unset timer
unset volta
end
if %speech% == magazzino && %actor.name% == %name%
wait 2 s
emote sfoglia il suo registro.
wait 2 s
say Prendere in affitto un magazzino per una settimana costa 2000 monete da versare subito , accetti?
set affi2 maga
global affi2
set timer2 on
global timer2
unset timer
unset volta
end
end

if %speech% == affittare && %affi% != on && %comp% != on
set name %actor.name%
global name
set affi on
global affi
set timer on
global timer
wait 2 s
nod
wait 1 s
say Cosa hai intenzione di prendere in affitto %name%, un magazzino o una bancarella?
else
end


if %comp2% == banc
if %speech% == si && %actor.name% == %name%
if %name.gold% < 10000
wait 2 s
say Non te lo puoi permettere , arrivederci.
unset comp
unset comp2
unset name
unset timer2
unset volta
end
if %name.gold% >= 10000
mforce %name% give 10000 coins ufficiale
wait 2 s
emot riscuote le 10000 monete.
wait 1 s
say Bene , comunico la tua richiesta al responsabile del mercato , riceverai presto la risposta tramite lettera.
wait 2 s
say Arrivederci
mgoto 3101
say Registra
say %name% ha comprato una bancarella
give all.coins segre
mgoto 3102
unset comp
unset comp2
unset name
unset timer2
unset volta
end
end
if %speech% == no && %actor.name% == %name%
wait 2 s
say Va bene arrivederci
unset comp
unset comp2
unset name
unset timer2
unset volta
end
end
if %comp2% == maga
if %speech% == si && %actor.name% == %name%
if %name.gold% < 20000
wait 2 s
say Non te lo puoi permettere , arrivederci.
unset comp
unset comp2
unset name
unset timer2
unset volta
end
if %name.gold% >= 20000
mforce %name% give 20000 coins ufficiale
wait 2 s
emot riscuote le 20000 monete.
wait 1 s
say Bene , comunico la tua richiesta al responsabile del mercato , riceverai presto la risposta tramite lettera.
wait 2 s
say Arrivederci
mgoto 3101
say Registra
say %name% ha comprato un magazzino
give all.coins segre
mgoto 3102
unset comp
unset comp2
unset name
unset timer2
unset volta
end
end
if %speech% == no && %actor.name% == %name%
wait 2 s
say Va bene arrivederci
unset comp
unset comp2
unset name
unset timer2
unset volta
end
end

if %comp% == on && %comp2% != maga && %comp2% != banc 
if %speech% == bancarella && %actor.name% == %name%
wait 2 s
emote sfoglia il suo registro.
wait 2 s
say Comprare una bancarella costa 10000 monete da versare subito , accetti?
set comp2 banc
global comp2
set timer2 on
global timer2
unset timer
unset volta
end
if %speech% == magazzino && %actor.name% == %name%
wait 2 s
emote sfoglia il suo registro.
wait 2 s
say Comprare un magazzino costa 20000 monete da versare subito , accetti?
set comp2 maga
global comp2
set timer2 on
global timer2
unset timer
unset volta
end
end

if %speech% == comprare && %affi% != on && %comp% != on
set name %actor.name%
global name
set comp on
global comp
set timer on
global timer
wait 2 s
nod
wait 1 s
say Cosa hai intenzione di comprare %name%, un magazzino o una bancarella?
end
~
#3151
ufficiale mercato interno2~
0 ab 100
~
if %timer% == on
if %volta% == 6
emote si spazientisce.
say Visto che non rispondi non mi resta che salutarti
unset volta
unset timer
unset affi
unset comp
end
if %volta% == 4
say Magazzino o bancarella?
end
if %volta% == 2
say Magazzino o bancarella?
end
end
if %timer% == on
eval volta %volta% +1
global volta
end

if %timer2% == on
if %volta% == 6
emote si spazientisce.
say Visto che non rispondi non mi resta che salutarti
unset volta
unset timer2
unset affi2
unset comp2
unset affi
unset comp
end
if %volta% == 4
say Si o no?
end
if %volta% == 2
say Si o no?
end
end
if %timer2% == on
eval volta %volta% +1
global volta
end
~
#3152
segretario1~
0 d 1
registra~
if %actor.vnum% == 3199
if %speech% == registra
say Va bene 
set reg on
global reg
end
end
~

#3153
segretario2~
0 d 1
ha riferisci note cancella 1 2 3 4 5 6 7 8 9 10 ripeti~
if %actor.vnum% == 3199
if %reg% == on
if %regnum1% != on
set reg1 %speech%
global reg1
set regnum1 on
global regnum1
unset reg
say Fatto
end
end
if %reg% == on
if %regnum2% != on
set reg2 %speech%
global reg2
set regnum2 on
global regnum2
unset reg
say Fatto
end
end
if %reg% == on
if %regnum3% != on
set reg3 %speech%
global reg3
set regnum3 on
global regnum3
unset reg
say Fatto
end
end
if %reg% == on
if %regnum4% != on
set reg4 %speech%
global reg4
set regnum4 on
global regnum4
unset reg
say Fatto
end
end
if %reg% == on
if %regnum5% != on
set reg5 %speech%
global reg5
set regnum5 on
global regnum5
unset reg
say Fatto
end
end
if %reg% == on
if %regnum6% != on
set reg6 %speech%
global reg6
set regnum6 on
global regnum6
unset reg
say Fatto
end
end
if %reg% == on
if %regnum7% != on
set reg7 %speech%
global reg7
set regnum7 on
global regnum7
unset reg
say Fatto
end
end
if %reg% == on
if %regnum8% != on
set reg8 %speech%
global reg8
set regnum8 on
global regnum8
unset reg
say Fatto
end
end
if %reg% == on
if %regnum9% != on
set reg9 %speech%
global reg9
set regnum9 on
global regnum9
unset reg
say Fatto
end
end
if %reg% == on
if %regnum10% != on
set reg10 %speech%
global reg10
set regnum10 on
global regnum10
unset reg
say Fatto
end
end
if %reg% == on
say Mi spiace sono finiti i posti
end
end

if %actor.name% == Roland || %actor.name% == Rodric
if %speech% == riferisci
if %regnum1% == on
say 1  %reg1% %data1%
end
if %regnum2% == on
say 2  %reg2% %data2%
end
if %regnum3% == on
say 3  %reg3% %data3%
end
if %regnum4% == on
say 4  %reg4% %data4%
end
if %regnum5% == on
say 5  %reg5% %data5%
end
if %regnum6% == on
say 6  %reg6% %data6%
end
if %regnum7% == on
say 7  %reg7% %data7%
end
if %regnum8% == on
say 8  %reg8% %data8%
end
if %regnum9% == on
say 9  %reg9% %data9%
end
if %regnum10% == on
say 10 %reg10% %data10%
end
end
end

if %actor.name% == roland || %actor.name% == Rodric
if %speech% == cancella && %data% != on && %opera% != on
say Quale devo cancellare?
set cance on
global cance
end
end

if %actor.name% == roland || %actor.name% == Rodric
if %speech% < 11 && %speech% > 0
if %cance% == on
unset reg%speech%
unset regnum%speech%
unset data%speech%
unset cance
end
end
end

if %actor.name% == roland || %actor.name% == Rodric
if %opera% == on && %speech% < 11 && %speech% > 0
set data%speech% %datazza%
global data%speech%
unset data
unset datazza
unset opera
end
end


if %actor.name% == roland || %actor.name% == Rodric
if %data% == on && %speech% != ripeti
set datazza &1&x%speech%&0
global datazza
say Di che operazione?
set opera on
global opera
unset data
end
end

if %actor.name% == roland || %actor.name% == Rodric
if %speech% == note && %opera% != on
say Ok inserisci le annotazioni
set data on
global data
end
end

if %actor.name% == roland || %actor.name% == Rodric
if %speech% == ripeti
if %data% == on
say Ok inserisci le annotazioni
end
if %opera% == on
say Di che operazione?
end
if %cance% == on
say Quale devo cancellare?
end
end
end

~


$~
