#21001
Chyrellos guardie 3~
0 d 1
guardia avanti stanza follami sfollami stop devo attenti riposo~
if %actor.name% /= capoguardie
if %speech% == guardia libera
unset azione
end
if %speech% /= stanza
set %speech%
if %speech% != 0
global stanza
set piazzata on
global piazzata
end
wait 1
nod
wait 1
mgoto %stanza%
wait 5
emote si posiziona di guardia seguendo gli ordini
end
end
if %piazzata% == on
if %attenti% == on
wait 1 s
if %actor.name% == Mcleod
set pass ok
global pass
end
if %actor.name% == Dolmant || %actor.name% == Yaemon || %actor.name% == Enrique || %actor.name% == Mcleod || %actor.name% == Boccadoro || %actor.name% ==  Sauzer || %actor.name% == Belar || %actor.name% == Kharas || %actor.name% == Azucena
set pass ok
global pass
end
if %pass% == ok
if %speech% == riposo
wait 1 s
nod
wait 1 s
say Come lei desidera, Lord %actor.name%
unset azione
end
if %speech% == attenti
wait 1 s
nod
wait 1 s
emote scatta sull'attenti!
set azione on
global azione
end
if %speech% == follami
say Come lei desidera, Lord %Actor.name%
wait 1 s
foll %actor.name%
end
if %speech% == sfollami 
wait 1 s
foll %self%
end
if %speech% == stop
stop
end
if %speech% == devo entrare
wait 4
if %self.room% == 27000
nod
wait 4
mload obj 27000
unlock cancella
open cancella
mforce %actor% nord
close cancella
lock cancella
mat 2 drop all
end
end
if %speech% == devo uscire
wait 4
if %self.room% == 27001
nod
wait 4
mload obj 27000
unlock cancella
open cancella
mforce %actor% sud
close cancella
lock cancella
mat 2 drop all
end
end
end
else
wait 1 s
emote guarda con aria indifferente Lord %Actor.name%
end
unset pass
end
~
#21002
Prigioni 2 - Elenia (Jerold)~
2 d 100
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
eval percentuale_fuga %random.1000%
global percentuale_fuga
if %percentuale_fuga% > 998
wecho %actor.name% %recita1a%
wdoor 21261 east flags a
wait 10 s
wdoor 21261 east flags bcd
else
wecho %actor.name% %recita2a%
end
else
eval percentuale_fuga1 %random.2000%
global percentuale_fuga1
if %percentuale_fuga1% > 1998
wecho %actor.name% %recita3a%
wdoor 21261 east flags a
wait 10 s
wdoor 21261 east flags bcd
else
wecho %actor.name% %recita2a%
end
end
else
wecho %actor.name% %recita4a%
end
~
#21003
Prigioni 3 - Elenia (Jerold)~
2 d 0
devo uscire~
if %speech% == devo uscire
if %actor.name% == yaemon || %actor.name% == Kharas || %actor.name% == Belar ||  %actor.name% == dolmant || %actor.name% == Enrique ||  %actor.name% == Mcleod || %actor.name%==Wab || %actor.name%==Aldayur
wait 2 s
wsend %actor.name% Ad un tuo cenno una guardia ti fa uscire
wait 2 s
wteleport %actor.name% 21260
wforce %actor.name% look
else
end
else
end
~
#21004
Prigioni 4 - Elenia (Jerold)~
2 d 1
libera~
set a %actor.name%
global a
if %a% == yaemon || %a% == Sauzer || %a% == dolmant || %a% == Enrique || %a% == Mcleod || %a% == Belar || %a% == Kharas || %a%==Wab || %a%==Aldayur
unset %libera%
set %speech%
global libera
set animazione on
global animazione
set timer 0
global timer
wecho &xUn enorme guardiano entra nella cella &0
wait 1 s
wecho  &xUn enorme guardiano dice, 'Il prigioniero da liberare mi dica &1Eccomi&0
~
#21005
Prigioni 5 - Elenia (Jerold)~
2 d 0
devo entrare~
if %speech% == devo entrare
if %actor.name% == yaemon || %actor.name% == Sauzer || %actor.name% == boccadoro || %actor.name% == kurten || %actor.name% == azucena || %actor.name% == dolmant || %actor.name% == Mcleod || %actor.name% == Belar || %actor.name% == Enrique
wait 2 s
wsend %actor.name% Ad un tuo cenno una guardia ti fa entrare
wait 2 s
wteleport %actor.name% 21261
wforce %actor.name% look
else
end
else
end
~
#21006
Prigioni 6 - Elenia (Jerold)~
2 d 1
Eccomi~
   if %animazione% == on
if %actor.name% == %libera%
wecho Perfetto ora vi trasferisco fuori
wait 2 s
wsend %actor.name% Il guardiano ti fa uscire dalla cella... in malo modo.
wait 1 s
wteleport %actor.name% 21260
set animazione off
global animazione
unset %libera%
else
wecho non ci provare
end
else
end
~
#21007
new trigger~
0 g 100
~
wait 25
Emote ti si fa incontro dopo aver riposto un grosso volume.
wait 40
say Salve %actor.name%! Prenda pure tutti i libri di cui necessita.
wait 40
say Li consulti pure, ma si ricordi di rimetterli al loro posto.
wait 40
say Sono della collettivita' e devono restare qui! Mi raccomando.
~
#21008
Prigioni 7 (guardie fuori)~
2 abg 100
~
wait 3 s
wteleport guardia 21260
~
#21009
Menestrello 1 (Elenia)~
0 m 1
~
if %amount% < 10
wait 1 s
say Ehi un po' di rispetto per un artista... 10 monete d'oro che vuoi che sono!
give %amount% coins %actor.name%
elseif %amount% => 1000
wait 1 s
Regno deposita %amount% elenia
say WOW!!! GRAZIE!!! Per una mancia cosi' grande ci vuole una canzone speciale!
wait 1 s
emote estrae una tastiera luminosa da sotto la veste
wait 1 s
emote  si concentra 
say  E 1... 1, 2, 3...  
elseif %amount% > 10
wait 1 s
say Troppi soldi
give %amount% coins %actor.name%
else
wait 1 s
set richiedente %actor.name%
set attiva on
global richiedente
global attiva
Regno deposita %amount% elenia
say Ottimo, scelga una canzone sulla bacheca e mi dica il numero
end
~
#21010
Ambrose, maitre Ostrica Aperta (Sophya-Jerold)~
0 g 100
~
   
   if %actor.vnum% != -1
     halt
   end
   set cliente %actor.name%
   global cliente
   wait 2 s
   smile %actor.name%
   wait 2 s
   say Benvenuto all'Ostrica Aperta!
   wait 2 s
   say Se ha la cortesia di dirmi in quale stanza ha prenotato, la faccio accomodare immediatamente!
   wait 2 s
~
#21011
Ambrose2, maitre Ostrica Aperta (Sophya-Jerold)~
0 d 1
sala~
if %actor.name% == %cliente%
if %speech% == Sala bianca
wait 1 s
emote porta %cliente% all'interno del ristorante
mteleport %cliente% 21218
mgoto %actor.name%
mforce %actor.name% look
'Eccoci! Buon proseguimento.
mgoto 21214
elseif %speech% == Sala blu
emote porta %cliente% all'interno del ristorante
mteleport %cliente% 21217
mgoto %actor.name%
mforce %actor.name% look
'Eccoci! Buon proseguimento.
mgoto 21214
elseif %speech% == Sala marrone
emote porta %cliente% all'interno del ristorante
mteleport %cliente% 21221
mgoto %actor.name%
mforce %actor.name% look
'Eccoci! Buon proseguimento.
mgoto 21214
elseif %speech% == Sala privata
emote porta %cliente% all'interno del ristorante
mteleport %cliente% 21219
mgoto %actor.name%
mforce %actor.name% look
'Eccoci! Buon proseguimento.
mgoto 21214
elseif %speech% == Sala rossa
emote porta %cliente% all'interno del ristorante
mteleport %cliente% 21215
mgoto %actor.name%
mforce %actor.name% look
'Eccoci! Buon proseguimento.
mgoto 21214
elseif %speech% == Sala trasparente
emote porta %cliente% all'interno del ristorante
mteleport %cliente% 21216
mgoto %actor.name%
mforce %actor.name% look
'Eccoci! Buon proseguimento.
mgoto 21214
elseif %speech% == Sala verde
emote porta %cliente% all'interno del ristorante
mteleport %cliente% 21220
mgoto %actor.name%
mforce %actor.name% look
'Eccoci! Buon proseguimento.
mgoto 21214
else
end
else
wait 1 s
'Spiacente non e' e' lei che sto servendo!
end
unset %cliente%
~
#21012
La morte del Drago verde (Sophya)~
0 f 100
~
emote &2&xstramazza al suolo e dalle sue fauci, ormai chiuse per sempre, fuoriesce un'ultima esalazione di gas, stavolta innocuo.&0
emote &2&xe' morto e tu sei ancora in piedi per narrarlo!&0
~
#21013
La morte del drago rosso (Sophya-Ilweran)~
0 f 100
~
emote &1&xcrolla al suolo con un boato tremendo.&0
emote &1&xe' morto! CONGRATULAZIONI!&0
~
#21014
Suono del liuto (Sophya)~
1 c 3
suona~
oecho Suono
~
#21015
Entrata primo magazzino di Chyrellos~
2 d 1
Magazzino Regno Primo~
if %actor.clan% ==2 && %actor.rank% >= 7
wteleport %actor.name% 21235
wforce %actor.name% look
else
wecho &7&xL'accesso e' consentito solo ai funzionari Eleni!&0
wait 5
wteleport %actor.name% 2134863
wforce %actor.name% look
end
~
#21016
Entrata secondo Magazzino di Chyrellos~
2 d 1
Secondo~
if %actor.clan% ==2 && %actor.rank% >= 7
wteleport %actor.name% 21259
wforce %actor.name% look
else
wecho &7&xL'accesso e' consentito solo ai funzionari Eleni!&0
wait 5
wteleport %actor.name% 2134863
wforce %actor.name% look
end
~
#21017
Entrata deposito beni eleni (Sophya)~
2 d 1
deposito beni~
if %actor.clan% ==2 && %actor.rank% >= 7
wteleport %actor.name% 21247
wforce %actor.name% look
else
wecho &7&xL'accesso e' consentito solo ai funzionari Eleni!&0
wait 5
wteleport %actor.name% 2134863
wforce %actor.name% look
end
~
#21018
Festa Ashmet - Emiro (Sophya)~
0 g 100
%actor.name% arriva.~
wait 115
emote alza gli occhi e sospira
wait 10
'Profeta, chiudi gli occhi!
wait 10
emote beve vino.
wait 120
'Non c'e' animale piu' nobile del cammello
wait 120
emote si guarda attorno con aria divertita
wait 10
'Qualcuna di queste signorine vorrebbe diventare la mia
sesta moglie?
~
#21019
Festa Ashmet - Regina degli Elfi (Sophya)~
0 g 100
%actor.name% arriva.~
wait 100
emote arrosisce sotto la maschera.
wait 10
'Non posso concedere un ballo ad uno sconosciuto
wait 100
'Noi elfi sappiamo guardare i sentimenti, lo sapete?
wait 100
'Le spade forgiate dagli elfi sono ottime, ma troppo
leggere
~
#21020
Visionario Ceco - Enter (Chardhros)~
0 g 100
~
if %actor.name%==Fostervold || %actor.name%==Lutz || %actor.name%==Chardhros
    wait 1 
    msend %actor.name% %self.name% si gira verso di te e ti accorgi che i suoi occhi sono bianchi.
    mechoaround %actor.name% %self.name% si gira verso %actor.name%.  
    wait 2
    mecho Qualcosa ti dice di non toccare %self.name%, i poteri dei morti sembrano essergli affianco.
endif 
~
#21021
Pazzo Ceco - Lamentele (Chardhros)~
0 g 100
~
* Prova
*scrittura trigger
say So paxxo so cieco!
bug non vedo un tubo accendete la luce!!!
~
$~
