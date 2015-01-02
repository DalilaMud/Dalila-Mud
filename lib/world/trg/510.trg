#51000
issa bandiera room (zephy)~
2 c 100
bandiera~
* Lista comandi
wisflagged %actor.room% INDOORS cielo_aperto
if %arg% == issa
        switch %bandiera% 
        case posataele
                if %cielo_aperto% == falso
                wsend %actor.name% Issi la bandiera di &3&xElenia&0
                wechoworld %actor.name% issa la bandiera di &3&xElenia&0 a %luogo%.
                end
                set bandiera issataele
                global bandiera
                wpurge astav
                wload obj 51003
                wpurge bandiera
                break
        case posataald
                if %cielo_aperto% == falso
                wsend %actor.name% Issi la bandiera di &4&xAldaria&0.
                wechoworld %actor.name% issa la bandiera di &4&xAldaria&0 a %luogo%.
                end
                set bandiera issataald
                global bandiera
                wpurge bandiera
                wpurge astav
                wload obj 51002
                break
        case posatasen
                if %cielo_aperto% == falso
                wsend %actor.name% Issi la bandiera di &xSendaria&0.
                wechoworld %actor.name% issa la bandiera di &xSendaria&0 a %luogo%.
                end
                set bandiera issatasen
                global bandiera
                wpurge bandiera
                wpurge astav
                wload obj 51009
                break
        case posatatol
                if %cielo_aperto% == falso
                wsend %actor.name% Issi la bandiera di &x&6Tolnedra&0.
                wechoworld %actor.name% issa la bandiera di &x&6Tolnedra&0 a %luogo%.
                end
                set bandiera issatatol
                global bandiera
                wpurge bandiera
                wpurge astav
                wload obj 51010
                break
        case ammainata
                wecho Prima di issare una bandiera devi posarla e legarla sull'asta
                break
        case issatatol
        case issatasen
        case issataele
        case issataald
                wecho C'e' gia' una bandiera issata sull'asta.
                break
        default
                wecho questo non dovresti leggerlo! chiama un creatore
                break
        done
elseif %arg% == ammaina
        switch %bandiera%
        case issataele
                if %cielo_aperto% == falso
                wechoworld La bandiera di &3&xElenia&0 viene ammainata a %luogo%.
                end
                set bandiera ammainata
                global bandiera
                wload obj 51006
                wpurge astaele
                wload obj 51005
                wforce %actor.name% prend bElenia
        break
        case issataald
                if %cielo_aperto% == falso
                wechoworld La bandiera di &4&xAldaria&0 viene ammainata a %luogo%.
                end
                set bandiera ammainata
                global bandiera
                wload obj 51004
                wpurge astaald
                wload obj 51005
                wforce %actor.name% prend bAldaria
        break
        case issatasen
                if %cielo_aperto% == falso
                wechoworld La bandiera di &xSendaria&0 viene ammainata a %luogo%.
                end
                set bandiera ammainata
                global bandiera
                wload obj 51007
                wpurge astasen
                wload obj 51005
                wforce %actor.name% prend bSendaria
        break
        case issatatol
                if %cielo_aperto% == falso
                wechoworld La bandiera di &6&xTolnedra&0 viene ammainata a %luogo%.
                end
                set bandiera ammainata
                global bandiera
                wload obj 51008
                wpurge astatol
                wload obj 51005
                wforce %actor.name% prend btolnedra
        break
        case posataele
        case posataald
        case posatasen
        case posatatol
                wecho ma se non e' stata neppure issata?
        break
        case ammainata
                wecho Se c'era una bandiera gia' l'hanno ammainata
        break
        default
                wecho Questo non dovresti leggerlo! chiama un creatore
        break
        done
else
        wecho Che ci vuoi fare con la bandiera?
end
~
#51001
Cancelli test Nord (Zephy)~
2 d 100
fatemi entrare~
* Lista comandi
   wecho &4Una Guardia dei Cancelli dice, 'Entrate pure, Concittadino! '&0
   wecho Un rumore metallico di ingranaggi e catenacci proviene dalla serratura del portone.
   wdoor 51001 north flags a
   wdoor 51000 south flags a
   wait 10 s
   wecho la Porta viene richiusa con un tonfo.
   wdoor 51001 north flags ab
   wdoor 51000 south flags ab
   wat 51000 wecho la Porta viene richiusa con un tonfo.
~
#51002
Richiama alce (Zephy)~
1 j 7
~
* Lista comandi
if %caricato% != on
   if %actor.name% == maqroll
   set caricato on
   global caricato
   wait 1 s
   osend %actor.name% Soffi nel richiamo e subito ne esce uno strano fischio.. simile ad un muggito.
   oechoaround %actor.name% %actor.name% porta uno strano fischietto alla bocca e subito si ode uno strano fischio... si direbbe un muggito.
   wait 2 s
   osend %actor.name% Un cucciolo d'alce trotterella silenziosamente fino a te.
   oechoaround %actor.name% Un cucciolo d'alce si avvicina silenziosamente...
   oload mob 64
   oforce cucciolo foll %actor.name%
   else
   end
   else
   end
~
#51003
FREE FOR USE~
1 c 4
lega~
* Lista comandi
if %room.bandiera% == posataele
        %send% %actor.name% Leghi la bandiera alla carrucola dell'asta.
        oechoaround %actor.name% %actor.name% lega la bandiera di Elenia alla carrucola dell'asta.
        setroom bandiera legataele
elseif %room.bandiera% == posataald
        %send% %actor.name% Leghi la bandiera alla carrucola dell'asta.
        oechoaround %actor.name% %actor.name% lega la bandiera di Aldaria alla carrucola dell'asta.
        setroom bandiera legataald
elseif %room.bandiera% == posatasen
        %send% %actor.name% Leghi la bandiera alla carrucola dell'asta.
        oechoaround %actor.name% %actor.name% lega la bandiera di Sendaria alla carrucola dell'asta.
        setroom bandiera legatasen
elseif %room.bandiera% == posatatol
        %send% %actor.name% Leghi la bandiera alla carrucola dell'asta.
        oechoaround %actor.name% %actor.name% lega la bandiera di Tolnedra alla carrucola dell'asta.
        setroom bandiera legatatol
elseif %room.bandiera% == legataele || legataald || legatasen || legatatol
        oecho C'e' gia' una bandiera legata all'asta.
elseif %room.bandiera% == issataald || issataele || issatasen || issatatol
        %echo% La bandiera e' gia' sul pennacchio
elseif %room.bandiera% == ammainata
        %echo% Posa una bandiera vicino all'asta.
else
        %echo% Non c'e' un'asta portabandiera qui.
end
~
#51004
addio alce (zephy)~
1 c 100
addio~
* Lista comandi
wait 1 s
   if %actor.name% != maqroll
   halt
   end
   osend %actor.name%  Il cucciolo muggisce un saluto, poi si allontana...silenziosamente
oechoaround %actor.name% Il cucciolo d'alce rivolge un muggito di commiato a %actor.name% e poi si allontana... silenziosamente
   opurge alce
   set caricato off
   global caricato
~
#51005
salvaVariabileglobale(Zephy)~
1 g 100
~
* Lista comandi
if % == posataele || posataald || posatasen || posatatol
           setroom bandiera ammainata
   else
           halt
   end
~
#51006
puntabandiera (zephy)~
2 h 100
~
* Lista comandi
if %bandiera% != ammainata || issatasen || issatatol || issataele || issataald
        halt
else
        if %object.vnum% == 51006
                set bandiera posataele
                global bandiera
                elseif %object.vnum% == 51004
                set bandiera posataald
                global bandiera
                elseif %object.vnum% == 51007
                set bandiera posatasen
                global bandiera
                elseif %object.vnum% == 51008
                set bandiera posatatol
                global bandiera
        else
                halt
        end
end
~
#51007
load cancello base (Zephy)~
2 g 100
~
* Lista comandi
if checked != si
 set portone  3
 global portone
 set aiutoariete1 0
 global aiutoariete1
 set aiutoariete2 0
 global aiutoariete2
 set aiutoariete3 0
 global aiutoariete3
 set esciar east
 global esciar
 set resport 70
 global resport
 set checked si
 global checked
else halt
end
~
#51008
Ariete Base (Zephy)~
1 c 4
schianta~
* Lista comandi
if %arg% == %room.esciar%
  if %room.portone% > 0
   eval #actor.mov# %actor.mov% - 40
   eval vatt  %random.100% + %actor.str% + (%actor.stradd%/20) + %room.aiutoariete1% + %room.aiutoariete2% + %room.aiutoariete3%
   oecho DEBUG %actor.str% %room.aiutoariete1% %aiutoariete2% %aiutoariete3% %vatt% Questo valore non apparir nella versione finale del triggo
   if %vatt% => %room.resport%
      oecho Con un gran fracasso l'ariete si schianta contro il bersaglio che vibra, trema e scricchiola!
      eval porttrans %room.portone% -1
      setroom portone %porttrans%
      oecho %room.portone% %porttrans%
   elseif %vatt% < %room.resport%
      oecho L'impatto dell'ariete sul Portone e' violentissimo, ma questo sembra non averne risentito.
      oecho e resta in piedi mentre gli assalitori barcollano per il contraccolpo.
      * VEDI SOPRA
   else
   end
   if %room.portone% == 0
      oecho  Con uno spaventoso boato la porta viene sfondata!
      oecho Scheggie di legno e cerniere di metallo schizzano da ogni parte!
      odoor %actor.room% %arg% name portone sfondato
      odoor %actor.room% %arg% description Il Portone ha un enorme crepa nel mezzo, non sara' difficile passarci in mezzo, stando un po' attenti alle scheggie.
      odoor %actor.room% %arg% flags 0
   elseif %room.portone% == 1
      oecho SCRICCHIOLA! SCRICCHIOLA! Le assi del Portone devono aver ceduto in piu' punti, ma ancora tengono...
      odoor %actor.room% %arg% description Un portone con qualche crepa di troppo, che denota una certa instabilita'
   elseif %room.portone% == 2
      oecho Dopo l'impatto il portone sembra vibrare per un po'
      odoor %actor.room% %arg% description Un portone un po' ammaccato, ma ancora piuttosto solido
   elseif %room.portone% => 3
   else
   oecho l'unica cosa che puoi fare qui con quell'ariete e'...sedertici sopra.
   end
   else
   oecho non c'e' nulla da schiantare qui!
   end
else 
oecho Ma dove vuoi schiantarlo l'ariete?
end
~
#51009
aiutoariete1~
1 c 100
aiuta1~
* Lista comandi
eval aiuarie1 %actor.str% + %actor.stradd%/20
oecho %actor.name% afferra l'ariete e partecipa all'assalto
setroom aiutoariete1 %aiuarie1%
wait 2 s
oecho %actor.name% lascia l'ariete stremato
setroom aiutoariete1 0
eval #actor.mov# %actor.mov%-40
wait 5 s
~
#51010
aiutoariete2 (zephy)~
1 c 4
aiuta2~
* Lista comandi
eval aiuarie2 %actor.str% + %actor.stradd%/20
oecho %actor.name% afferra l'ariete e partecipa all'assalto
setroom aiutoariete2 %aiuarie2%
wait 2 s
oecho %actor.name% lascia l'ariete stremato
setroom aiutoariete2 0
eval #actor.mov# %actor.mov%-40
wait 5 s
~
#51011
aiutoariete3 (zephy)~
1 c 4
aiuta3~
* Lista comandi
eval aiuarie3 %actor.str% + %actor.stradd%/20
global aiuarie3
oecho %actor.name% afferra l'ariete e partecipa all'assalto
setroom aiutoariete3 %aiuarie3%
wait 2 s
oecho %actor.name% lascia l'ariete stremato
setroom aiutoariete3 0
set aiuarie3 0
global aiuarie
eval #actor.mov# %actor.mov% - 40
wait 5 s
~
#51012
settaroombandiera (zephy)~
2 g 100
~
* Lista comandi
if check != si
  set luogo TESTNOGDR
  global luogo
  set bandiera ammainata
  global bandiera
  *IL VALORE BANDIERA VA MODIFICATO A 
  *SECONDA DEL POSSESSORE DELL'OBBIETTIVO
  *ED EVENTUALMENTE SOSTITUITO COL VALORE 
  *DI UNA BANDIERA DI UN REGNO
  set check si
  else
end
~
#51013
new trigger~
2 c 100
wizrestaura~
* Lista comandi
set checked no
wdoor %actro.room% %esciar% description Un robusto alto portone fatto di assi rinforzate con bande e cerniere di acciaio
~
$~
