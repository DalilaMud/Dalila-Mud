#33100
Allarme intrusi~
2 g 100
~
if %actor.vnum% == -1
if %direction% == south
wait 3
wecho &3&xAppena entri nell' accampamento le guardie si accorgono di te !!!&0
wait 3
wecho &1Una Guardia urla: Soldati Armatevi !!! Uccidiamo gli INTRUSI!!!&0
end
else end
end
~
#33101
Allarme Intrusi (2)~
2 g 100
~
if %actor.vnum% == -1
if %direction% == east
wait 3
wecho &3&xAppena entri nell' accampamento le guardie si accorgono di te!!!&0
wait 3
wecho &1Una Guardia urla: Soldati Armatevi !!! Uccidiamo gli INTRUSI!!!&0
end
else end
end
~
#33102
Avvertimenti~
0 g 100
~
wait 5
emote ti guarda con fare sprezzante
wait 5
say Questo e' l' accampamento del Principe Jalhaan, primogenito del Sultano
wait 8
say Non posso fare entrare nessuno, cosi' mi e' stato ordinato.
wait 8
say Quindi ... aria !
wait 3
emote sghignazza e mette la mano sull' elsa della sua scimitarra
 
~
#33103
I vortici di sabbia~
2 g 100
~
if %actor.vnum% == -1
  set target %actor.name%
  eval num %random.6%
  wait 5
  wsend %target% La sabbia inizia a muoversi sempre piu' velocemente...
  wait 2
  wsend %target% Ti senti trascinare sempre piu' in basso dai gorghi vortiginosi !
  wait 2
  wsend %target% Cadi in un vortice di sabbia e vieni risucchiato !!!
  wdamage %target% 100
  wait 5
  wsend %target% La caduta e' stata davvero dolorosa ! Sei FERITO ! e ti sei perso !
  wforce %target% bar
  if %num% == 1
    wteleport %target% 33152
    wecho &3&xVedi %target% scivolare dentro un vortice di sabbia e sparire nel nulla!&0
    wforce %target% look
  elseif %num% == 2
    wteleport %target% 33137
    wecho &3&xVedi %target% scivolare dentro un vortice di sabbia e sparire nel nulla!&0
    wforce %target% look
  elseif %num% == 3
    wteleport %target% 33153
    wecho &3&xVedi %target% scivolare dentro un vortice di sabbia e sparire nel nulla!&0
    wforce %target% look
  elseif %num% == 4
    wteleport %target% 33158
    wecho &3&xVedi %target% scivolare dentro un vortice di sabbia e sparire nel nulla!&0
    wforce %target% look
  elseif %num% == 5
    wteleport %target% 33155
    wecho &3&xVedi %target% scivolare dentro un vortice di sabbia e sparire nel nulla!&0
    wforce %target% look
  elseif %num% == 6
    wteleport %target% 33146
    wecho &3&xVedi %target% scivolare dentro un vortice di sabbia e sparire nel nulla!&0
    wforce %target% look
  else
    wecho &3&xUno sbuffo di sabbia viene sollevato dal vento.&0 
  end
end
 
~
#33104
Seguace di Jeen~
0 g 100
~
wait 5
follow jeen
~
#33105
Seguace di Jeen (2)~
0 b 100
~
wait 5
follow jeen
~
#33106
Sveglia Thunk!~
2 g 100
~
if %actor.vnum% == -1
wait 10
wforce guardia say Impossibile gli intrusi sono arrivati fino a qui !
wait 10
wforce guardia emote decide di svegliare il suo capo
wait 10
wforce guardia say Sveglia Thunk ! E' ora di spaccare delle teste !
wforce guardia sveglia thunk
wait 10
wforce thunk say &1&xCHI OSA DISTURBARMI! MORITE!&0
wait 10
wforce guardia grin
end
else end
end
~
#33107
Jub e' pazzo !~
0 g 100
~
wait 10
if %actor.vnum% == -1
say Aiuta! aiuta jub !!! aiuta il pazzo jub !!! Jub e' tonto ! Jub si e' perso !! Aiuta !!
wait 10
say Jub vede tanti mostri ! Tanti tanti !!! Jub vuole uscire ! Aiuta !
wait 10
say ME puo' darti tanti doni ! ma fuori ! ti prego ! aiuta !
follow %actor.name%
wait 10
worship %actor.name%
say Andiamo ! fuori  ! porta jub !
else end
end
~
#33108
Jub e' completamente pazzo !~
0 b 20
~
wait 5
emote ti guarda negli occhi
wait 10
say tutti dicono che sono pazzo ! Ma non vero e' !
wait 10
say io felice tu aiutare me ! promettere che non aiutare mostri ad ucciderti come ultima volta !
wait 10
say prometto !!! Credi a jub ! Jub e' buono ! Jub non farebbe male a una mosca !
wait 10
emote batte violentemente le mani uccidendo una mosca che volava tranquilla
say O no ?
~
#33109
La morte del reietto~
0 f 100
~
emote &1crolla a terra senza vita... ma il sangue ribolle e da esso nascono nuovi fedeli !!! Lunga vita alla stirpe !!!&0
mload mob 33128
mload mob 33128
mforce uomodrago get all cad
mforce uomodrago say VOI AVETE OSATO UCCIDERE NOSTRO PADRE !!! ORA PAGHERETE CON LA VITA !
end
~
#33110
Karim il mercante~
0 g 100
~
wait 5
bow %actor.name%
wait 5
say Salve Avventurieri !
wait 5
say Io sono solo un mercante, non mi interessano queste battaglie... comprate pure sono sicuro che troverete i miei oggetti.... LEGGENDARI !
wait 5
rid
wait 5
emote si sfrega le mani avidamente mentre vi vede stupiti dalla sua mercanzia
~
#33111
I cammelli bevono~
0 b 25
~
wait 10
emote sembra molto stanco dopo la traversata nel deserto
wait 5
emote beve un po' di acqua dall' oasi
~
#33112
Jeen insulta gli avventurieri~
0 g 100
~
if %actor.vnum% == -1
  wait 5
  say Muahahahah ecco altra carne e altri tesori per il mio gruppo !
end
 
~
#33113
Le braccia dalla terra~
2 g 100
~
wait 10
wecho &1&xMORTALE!!! Nessuno puo' scappare da questo luogo maledetto !!! Sara' la stessa roccia su cui cammini a fermarti !!!&0
~
#33114
Le uova si schiudono...~
2 g 100
~
if %direction% == south && %actor.vnum% == -1
  wait 3 s
  wecho Senti dei rumori terribili... qualcosa si rompe e ... forse un ringhio molto esile rimbomba nella grotta. Delle uova di Beholder devono essersi schiuse da qualche parte...
end
   
~
#33115
Le armi false~
1 j 100
~
wait 10
oecho Appena %actor.name% indossa il potente artefatto... viene avvolto da...
wait 20
oecho Una PUZZA Insopportabile !!!
~
#33116
Jeen avverte gli avventurieri~
2 g 100
~
if %actor.vnum% == -1
if %direction% == north
wait 5
wecho &3Jeen il tagliateste urla: Presto finiamo di saccheggiare le rovini prima che ci scoprano!&0
else end
end
end
~
$~
