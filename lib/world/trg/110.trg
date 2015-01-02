#11000
Entrata nel covo~
0 d 0
ma chiara la via~
eval caso %random.300%
eval possibilita' %actor.int%+%actor.wis%
if %actor.eq(1).shortdesc% != Un'anello fatto di &wArgento&0 e &rSangue&0
say Hey!? Non Basta, anche io so cantare storielle...
elseif %actor.name% == kylie || %actor.name% == morente || %actor.name% == jiba || %actor.name% == malus
sussurra %actor.name% Spero che non ti abbiano seguito...
%teleport% %actor% 11001
elseif %caso% < %possibilita'%
%teleport% %actor% 11001
else
%purge% %actor.eq(1)%
%send% %actor% &wCon un gesto fulmineo %self.name% ti ruba l'anello.&0
wait 1s
say Non farmi ridere chi pensi di fregare?
end
~
#11001
Entrata nella grotta~
0 g 100
~
say La notte e' buia...
~
#11003
Prigione~
2 c 100
uscita~
if %actor.name% == kylie || %actor.name% == morente || %actor.name% == jiba || %actor.name% == malus
%echoaround% %actor% &c%actor.name% scopare all'improvviso
%send% %actor% ok
%teleport% %actor% 11008
%door% 1531513 east room 11007
%door% 1535514 east purge
~
#11004
Libera prigionieri~
2 c 100
libera~
if %actor.name% == kylie || %actor.name% == morente || %actor.name% == jiba || %actor.name% == malus
%echo% Ok.
%teleport% all 11008
~
#11005
Gassa stanze~
2 c 100
gassa~
if %arg% == east
%send% %actor% &GTiri la leva Gialla e la stanza ad est si riempie di gas&0
%at% 11009 dg_cast 'sleep' %random.char%
%at% 11009 dg_cast 'sleep' %random.char%
%at% 11009 dg_cast 'sleep' %random.char%
%at% 11009 dg_cast 'sleep' %random.char%
%at% 11009 dg_cast 'sleep' %random.char%
else if %arg% == south
%send% %actor% &RTiri la leva Rossa e la stanza ad est si riempie di gas&0
%at% 11010 dg_cast 'sleep' %random.char%
%at% 11010 dg_cast 'sleep' %random.char%
%at% 11010 dg_cast 'sleep' %random.char%
%at% 11010 dg_cast 'sleep' %random.char%
%at% 11010 dg_cast 'sleep' %random.char%
else if %arg% == west
%send% %actor% &VTiri la leva Verde e la stanza ad est si riempie di gas&0
%at% 11011 dg_cast 'sleep' %random.char%
%at% 11011 dg_cast 'sleep' %random.char%
%at% 11011 dg_cast 'sleep' %random.char%
%at% 11011 dg_cast 'sleep' %random.char%
%at% 11011 dg_cast 'sleep' %random.char%
else
%send% %actor% Direzione non valida. Scrivi gassa 'east' o 'west' o 'south'
end
~
#11008
Controlla vnum~
0 i 100
~
eval room %self.room%
eval vnum %room.vnum%
* se non compreso nei vnum indicati imposta il controllo come falso (0) altrimenti come vero (1)
if %vnum% < 9500 || %vnum% > 9599
eval controllo1 0
else
eval controllo1 1
end
if %vnum% < 8500 || %vnum% > 8699
eval controllo2 0
else
eval controllo2 1
end
if %vnum% < 9500 || %vnum% > 9899
eval controllo3 0
else
eval controllo3 1
end
if %vnum% < 2400000 || %vnum% > 2449999
eval controllo4 0
else
eval controllo4 1
end
* se tutti i controlli sono falsi il vnum e' vietato
if %controllo1% == 0
if %controllo2% == 0
if %controllo3% == 0
if %controllo4% == 0
say Qui non posso stare! Torno in caserma!
%asound% %self.name% &edice, 'Qui non posso stare! Torno in caserma!'&0
follow me
%teleport% %self% 8599
end
end
end
end
~
#11010
Guardia elite Tolnedra (1) (Niente copiato da Sheep)~
0 g 100
~
wait 1 s
if %actor.clan% == 3
if %actor.rank% >= 43
emote scatta sull'attenti
wait 1
say Signore! 
msend %actor.name% i comandi per le guardie sono guardie andiamo, guardie riposo, guardie saluto
else
end
end
~
#11011
Guardia elite Tolnedra (2) (Niente copiato da Sheep)~
0 d 1
guardie~
if %actor.clan% != 3
wait 1 s
emote guarda con sprezzo %actor.name%
halt
end
wait 1 s
if %actor.rank% >= 43
if %speech% == guardie andiamo
say Agli ordini, %actor.name%
follow %actor.name%
wait 1 s
say Pronto!!
elseif %speech% == guardie riposo
wait 1 s
say Agli ordini
wait 1 s
follow self
elseif %speech% == guardie saluto
emote si inchina con fierezza
end
else
emote guarda male %actor.name%
end
~
#11012
Trigger antistow~
0 c 100
stow~
if %actor.level% <= 70
%send% %actor% Non puoi caricarti %self.name% sulle spalle
return 1
else
return 0
~
#11013
Save in prigione~
0 b 100
~
%force% %random.char% save
%echo% ok
~
#11014
Purga obj~
0 j 100
~
%purge% %object%
~
#11015
bocca sputademoni~
1 b 10
60~
if %poppaggi% <= 1
%echo% &ZUn'improvviso sbuffo di fumo si leva dalla crepa&0
wait 3s
%echo% &rNon accade nulla&0
halt
end
%echo% &ZUn'improvviso sbuffo di fumo si leva dalla crepa&0
wait 3s
%echo% &rDelle forme terribili si materializzano...&0
eval caso %random.3%
if %caso% == 1
%load% mob 11001
%load% mob 11001
%load% mob 11001
end
if %caso% == 2
%load% mob 11002
%load% mob 11002
end
if %caso% == 3
%load% mob 11003
end
eval poppaggi %poppaggi% - 1
global poppaggi
~
#11016
lacrima di silue~
1 c 1
tira~
%echoaround% %actor% &B%actor.name% lancia la lacrima di silue verso il portale demoniaco.&0
%send% %actor% &BLanci la lacrima di silue verso il portale demoniaco.&0
wait 4s
%echo% &CIn un istante che sembra essere eterno vedi la lacrima volare nell'aria diretta verso il bersaglio...&0
wait 4s
%echo% &GLa Lacrima di Silue e' sulla bocca della crepa...&0
wait 4s
%echo% &ZUno sbuffo di fumo sulfureo devia la traiettoria della Lacrima...&0
%echo% &c...che si infrange sul bordo...&0
wait 4s
%echo% &rLa crepa lentamente si richiude, ma il dubbio che si possa riaprire permane...&0
wait 4s
%echo% &EPer ora hai salvato di nuovo la Daresia!!!&0
%purge%
%purge% %self%
halt
~
#11017
corna del demone~
0 k 1
~
%echoaround% %actor% &WCon un possente colpo %actor.name% stacca le corna del demone&0
%send% %actor% &WCon un possente colpo stacchi le corna del demone&0
%load% obj 5284
~
#11018
ali del demone~
0 k 1
~
%echoaround% %actor% &WCon un possente colpo %actor.name% stacca le ali del demone&0
%send% %actor% &WCon un possente colpo stacchi le ali del demone&0
%load% obj 5283
~
#11019
cuore del demone~
0 f 10
~
say RAAAAAHHH
%echo% &eIl demone muore mentre il suo petto eslpode in un &Rfiotto di sangue&e. &ZIl suo cuore nero cade a terra.&0
%load% obj 5282
~
#11020
porta moria e roccia nascosta~
2 f 100
~
%door% 4104 down room 11012
%door% 11014 south flags abe
%door% 11018 north flags abe
%at% 2444851 %purge%
%at% 2444851 %load% obj 9747
~
#11021
Entrata degli inferi~
2 c 100
tuffati~
%echoaround% %actor% &G%actor.name% coraggiosamente si tuffa nella crepa!&0
%send% %actor% &GCoraggiosamente ti tuffi nella crepa&0
eval entra 0
if %actor.eq(6).shortdesc% == delle &Rrosse&0 corna di un &rdemone&0
eval entra 1
end
if %actor.eq(17).shortdesc% == Un &Znero&0 cuore di &rdemone&0
eval entra 1
end
if %actor.eq(12).shortdesc% == &ZNere&0 ali di &rdemone&0
eval entra 1
end
if %entra% == 0
%teleport% %actor.name% 11036
end
if %entra% == 1
%teleport% %actor.name% 11027
end
~
#11022
Rugginofago mangia eq~
0 k 5
~
eval equip %random.29% -1
eval oggetto %actor.eq(%equip%)%
if %oggetto%
%send% %actor.name% &Z%self.name% si mangia %oggetto.shortdesc%!!&0 
%echoaround% %actor.name% &Z%self.name% si mangia %oggetto.shortdesc% di %actor.name%!!&0 
%purge% %oggetto%
end
~
#11023
freccia ~
1 c 100
shoot tan n~
%echo% senti una sottile freccia entrare nelle tue carni
halt
~
#11024
planetar - quest~
0 n 100
~
wait 5s
%echo% %self.name% si concentra ed entra in un profondo stato di meditazione.
wait 5s
%echo% &wUn lieve suono angelico proviene da un punto imprecisato&0
wait 20s
%echo% &wUn lieve suono angelico proviene da un punto imprecisato&0
wait 20s
%echo% &wUn lieve suono angelico proviene da un punto imprecisato&0
wait 20s
%echo% &wIl suono si fa piu' forte...&0
wait 20s
%echo% &WIl suono e' sempre piu' forte&0
wait 20s
%echo% &BUna potente aura circonda %self.name% che viene avvolto da fiamme &Cazzurrine&0
wait 5s
%echo% &CIl fuoco degli angeli si propaga nella stanza&0
wait 5 s
%echo% &WUn potente flash di luce ti stordisce per un attimo... il rituale e' finito&0
%echo% &b%self.name% ti sorride mentre scompare lentamente nel nulla&0
%purge%
%purge% %self%
halt
~
#11025
bocca dell'inferno 2~
1 n 100
~
eval poppaggi 10
global poppaggi
~
#11026
planetaar resetta rituale~
0 k 100
~
wait 30 s
say A causa dell'interruzione devo cominciare di nuovo il rituale.
%load% mob 11005
%purge% %self%
halt
~
#11027
Demoni dell'abisso - not pass~
0 q 100
~
if %direction% == down
if %actor.align% != -1000
say Tu non puoi passare!
return 0
end
end
~
#11028
resetta crepa~
2 g 25
~
if %actor.vnum% == -1
%purge%
%load% obj 11001
end
~
#11029
Entrata negli inferi~
2 g 100
~
wait 2s
%echo% &ZUna flebile voce maligna dice da un punto imprecisato, ' &rSolo chi viene riconosciuto come demone entrera' negli inferi&Z '&0
wait 2s
%send% %actor.name% &gDigita 'tuffati' per provare, ma se non hai in qualcosa l'aspetto di un demone saranno guai&0
~
#11030
demone - superbia~
0 g 100
~
wait 1 s
look %actor.name%
wait 1 s
grin
wait 1 s
say Sei un essere inutile, come tutti gli altri.
~
#11031
demone - gola~
0 k 5
~
%echoaround% %actor% &V%self.name% tocca %actor.name% che vomita copiosamente.&0
%send% %actor.name% &V%self.name% ti tocca e subito vomiti copiosamente&0
%force% %actor% vomita
wait 1s
%force% %actor% vomita
wait 1s
%force% %actor% vomita
%actor.fame% = 4
%actor.sete% = 4
~
#11032
demone - ira~
0 k 5
~
eval rc %random.char%
if %rc% != %actor%
if %rc% != %self%
%echoaround% %actor% &R%self.name% tocca %actor.name% e un'ira terribile si ipadronisce della sua mente.&0
%send% %actor.name% &R%self.name% ti tocca e un'ira terribile si ipadronisce della tua mente&0
dg_cast 'peace' %actor%
dg_cast 'peace' %actor%
dg_cast 'peace' %actor%
%force% %actor% kill %rc.name%
end
end
~
#11033
demone - avarizia~
0 k 5
~
%echoaround% %actor% &G%self.name% tocca %actor.name% privandolo dei suoi beni.&0
%send% %actor.name% &G%self.name% ti tocca privandoti dei tuoi beni.&0
%actor.gold(-100)%
~
#11034
demone - lussuria~
0 k 5
~
eval char %random.char%
%echoaround% %char% &e%self.name% tocca %char.name% che viene preso da una frenesia incontrollabile&0
%send% %char.name% &e%self.name% ti tocca e subito vieni preso da una frenesia incontrollabile&0
dg_cast 'death dance' %char%
~
#11035
demone - invidia~
0 k 5
~
eval equip %random.29% -1
eval oggetto %actor.eq(%equip%)%
if %oggetto%
%echoaround% %actor% %self.name% &btocca %actor.name% facendogli fare qualcosa che non vuole.&0
%send% %actor.name% %self.name% &bfacendoti fare qualcosa che non vuoi.&0
* %load% obj %oggetto.vnum% %self% inv
* %purge% %oggetto%
%force% %actor% remove %oggetto.alias%
%force% %actor% give %oggetto.alias% demone
end
~
#11036
demone - orgoglio~
0 f 100
~
%load% mob 11015
%load% mob 11016
%load% mob 11016
%load% mob 11017
%load% mob 11017
%echo% &ZIl corpo del demone si smembra e i suoi pezzi animati ti attaccano!&0
~
#11037
collare dell'ira~
1 c 1
furia~
if %cariche% != 1
if %cariche% != 2
if %cariche% != 3
%send% %actor.name% Non accade nulla.
halt
end
end
end
eval c %cariche%
eval cariche %c% - 1
global cariche
dg_affect %actor% blind on 5
dg_affect %actor% deathdance on 5
odamage %actor.name% 50
oechoaround %actor.name% &RGli occhi di %actor.name% diventano rossi per l'ira che lo pervade!&d
%send% %actor.name% &RUn'ira tremenda ti offusca la mente e la vista!&0
wait 1 s
%force% %actor% grin
~
#11038
collare dell'ira - info~
1 j 100
~
wait 1s
%send% %actor% Scrivi 'furia' per usare il collare. Ha ancora %cariche% cariche.
if %cariche% ==
set cariche 3
global cariche
halt
end
~
#11039
collare dell'ira - setta cariche~
1 n 100
~
eval cariche 3
global cariche
~
#11040
Beholder antimagic~
0 p 100
~
eval retval 0
if %spellname% == peace
%send% %actor.name% &WL'occhio del beholder assorbe il tuo incantesimo&d
return 0
halt
end
if %spellname% == chill touch
%send% %actor.name% &WL'occhio del beholder assorbe il tuo incantesimo&0
return 0
halt
end
if %spellname% == burning hands
%send% %actor.name% &WL'occhio del beholder assorbe il tuo incantesimo&0
return 0
halt
end
eval check %random.2%
if %check% == 1
return 0
%send% %actor.name% &WL'occhio del beholder assorbe il tuo incantesimo&0
end
~
#11041
trigger anti identify~
1 p 100
~
eval retval 0
if %spellname% == identify
%send% %actor,name% &wIl tuo incantesimo ti mostra solo cose senza senso...&0
trig-return %retval%
end
~
#11042
Fucina - comandi (NIENTE)~
1 c 100
fucina~
if %actor.prof(fabbro)% <= 0
  %send% %actor.name% Non hai idea di come si faccia.
  halt
end
if %arg% == accendi
  * accensione fucina / manca acciarino e rametti
  if %stato% == accesa
    %send% %actor.name% E' gia' accesa.
    halt
  end
  if %actor.eq(17).shortdesc% != un acciarino
    %send% %actor.name% Devi tenere in mano un acciarino per accendere la fucina
    halt
  end
  eval contenuto %self.contents%
  eval rami 0
  while %contenuto%
    eval tmp %contenuto.next_in_list%
    if %contenuto.vnum% >= 90160 & %contenuto.vnum% <= 90170
      eval tmp2 %rami% + 1
      eval rami %tmp2%
      %purge% %contenuto%
    end
    eval contenuto %tmp%
  done
  %echo% Delle piccole fiamme avviluppano i rami nella fucina...
  wait 5 s
  if %rami% >= 3
    %echo% Il fuoco scoppietta allegramente.
  else
    %echo% Le fiamme si spengono mestamente.
    halt
  end    
  eval stato accesa
  global stato
  %send% %actor.name% Accendi la fucina.
  %echoaround% %actor% %actor.name% accende la fucina.
  halt
end
if %arg% == spegni
  * spegnimento fucina
  if %stato% == spenta
    %send% %actor.name% E' gia' spenta.
    halt
  end
  eval stato spenta
  global stato
  %send% %actor.name% Spegni la fucina.
  %echoaround% %actor% %actor.name% spegne la fucina.
  eval num_carb (%unita_carbone% / 5)
  while %num_carb% > 0
    %load% obj 2215
    eval tmp (%num_carb% - 1)
    eval num_carb %tmp%
  done
  eval unita_carbone 0
  global unita_carbone
  halt
end
if %arg% == info
  * info fucina
  %send% %actor.name% La fucina e' %stato%.
  wait 1 s
  if %temperatura% >= 1500
    eval ferro fuso
  else
    eval ferro solido
  end
  if %temperatura% >= 950
    eval argento fuso
  else
    eval argento solido
  end
  if %temperatura% >= 1100
    eval rame fuso
  else
    eval rame solido
  end
  if %temperatura% >= 400
    eval zinco fuso
  else
    eval zinco solido
  end
  if %temperatura% >= 200
    eval stagno fuso
  else
    eval stagno solido
  end
  if %temperatura% >= 1000
    eval oro fuso
  else
    eval oro solido
  end
  %send% %actor.name% L'indicatore del   ferro   e' %ferro%.
  wait 1 s
  %send% %actor.name% L'indicatore dell' argento e' %argento%.
  wait 1 s
  %send% %actor.name% L'indicatore del   rame    e' %rame%.
  wait 1 s
  %send% %actor.name% L'indicatore dello zinco   e' %zinco%.
  wait 1 s
  %send% %actor.name% L'indicatore dello stagno  e' %stagno%.
  wait 1 s
  %send% %actor.name% L'indicatore dell' oro     e' %oro%.
  wait 1 s
  eval tempo (%unita_carbone% / 10)
  %send% %actor.name% La fucina contiene carbone sufficiente per circa %tempo% minuti.
  halt
end
if %arg% == carica_carbone
  * carica il carbone
  eval uc_add %self.count(2215)%
  eval tmp (%unita_carbone% + (%uc_add% * 10))
  eval unita_carbone %tmp%
  global unita_carbone
  eval contenuto %self.contents%
  * ciclo che purga il carbone
  while %contenuto%
    eval next %contenuto.next_in_list%
    if %contenuto.vnum% == 2215
      %purge% %contenuto%
    end
    eval contenuto %next%
  done
  %send% %actor.name% Carichi del carbone nella fucina.
  %echoaround% %actor% %actor.name% carica del carbone nella fucina.
  halt
end
if %arg% == stampo
  * fabbrica lo stampo
  if %temperatura% < 2050
    %send% %actor.name% Per poter fabbricare uno stampo la fucina deve essere a piena potenza.
    halt
  end
  eval martello %actor.eq(17)%
  if %martello.vnum% != 14
    %send% %actor.name% Devi impugnare un martello da fabbro.
    halt
  end
  eval lingotto %actor.inventory(2201)%
  eval carbone %actor.inventory(2215)%
  if %lingotto% & %carbone%
    %send% %actor.name% Cominci a lavorare sullo stampo...
    wait 20 s
    %send% %actor.name% Ottieni uno stampo per lingotti!
    %echoaround% %actor% %actor.name% ottiene uno stampo per lingotti!
    %load% obj 11016 %actor% inv
    %purge% %lingotto%
    %purge% %carbone%
  else
    %send% %actor.name% Devi avere un pezzo di ematite e un pezzo di carbone in inventario per fabbricare uno stampo.
    halt
  end
end
~
#11043
Fucina - globals (NIENTE)~
1 n 100
~
eval stato spenta
global stato
eval temperatura 0
global temperatura
eval unita_carbone 0
global unita_carbone
~
#11044
Fucina - consumo carbone (NIENTE)~
1 b 100
~
if %stato% == spenta
   if (%temperatura% > 0)
      eval tmp %temperatura%
      eval temperatura (%tmp% - 50)
      global temperatura
   end
halt
end
if %stato% == accesa
   * parte consuma carbone
   if %unita_carbone% >= 1
      eval u_carb %unita_carbone%
      eval unita_carbone %u_carb% - 1
      global unita_carbone
      if %temperatura% <= 2000
         eval tmp %temperatura%
         eval temperatura (%tmp% + 50)
         global temperatura
      end
   end
   if %unita_carbone% == 0
      %echo% La fucina si spegne lentamente per mancanza di carbone.
      eval stato spenta
      global stato
   end
   * parte fusione metallo
   eval stampo %self.contents%
   while %stampo%
   if %stampo.vnum% == 11016
      eval contenuto %stampo.contents%
      eval vnum 0
      * ematite - ferro
      if %contenuto.vnum% == 2201
         if %temperatura% <= 1500
            %echo% La temperatura della fucina non e' sufficiente per scogliere il ferro contenuto nell'ematite.
         else
            eval vnum 2261
         end
      end
      * silvanite - argento
      if %contenuto.vnum% == 2202
         if %temperatura% <= 950
            %echo% La temperatura della fucina non e' sufficiente per scogliere l'argento contenuto nella silvanite.
         else
            eval vnum 2262
         end
      end
      * malachite - rame
      if %contenuto.vnum% == 2203
         if %temperatura% <= 1100
            %echo% La temperatura della fucina non e' sufficiente per scogliere il rame contenuto nella malachite.
         else
            eval vnum 2263
         end
      end
      * cassiterite - zinco
      if %contenuto.vnum% == 2204
         if %temperatura% <= 400
            %echo% La temperatura della fucina non e' sufficiente per scogliere lo zinco contenuto nella cassiterite.
         else
            eval vnum 2264
         end
      end
      * sfalerite - stagno
      if %contenuto.vnum% == 2205
         if %temperatura% <= 200
            %echo% La temperatura della fucina non e' sufficiente per scogliere lo stagno contenuto nella sfalerite.
         else
            eval vnum 2265
         end
      end
      * pepita - oro
      if %contenuto.vnum% == 2206
         if %temperatura% <= 1000
            %echo% La temperatura della fucina non e' sufficiente per scogliere l'oro nella pepita.
         else
            eval vnum 2266
         end
      end
      if %vnum% != 0
         %purge% %contenuto%
         %echo% Il metallo nello stampo comincia a sciogliersi.
         eval fuso si
         %load% obj %vnum% %stampo%
       end
   end
   eval tmp %stampo.next_in_list%
   eval stampo %tmp%
   done
   if %fuso% == si
      wait 6 s
      %echo% Lo stampo e' pieno di metallo fuso... e' ora di tirarlo fuori.
   end
end
~
#11045
Stampo - Globali (NIENTE)~
1 n 100
~
eval temperatura 0
global temperatura
~
#11046
Stampo - Comandi (NIENTE)~
1 c 100
raffredda~
%send% %actor.name% Raffreddi lo stampo.
%echoaround% %actor% %actor.name% raffredda lo stampo.
eval temperatura 0
global temperatura
~
#11047
Stampo - Raffreddamento lento (NIENTE)~
1 b 100
~
if %temperatura% > 0
evalt tmp %temperatura% - 3
eval temperatura %tmp%
global temperatura
end
~
#11048
Stampo - Get (NIENTE)~
1 g 100
~
%echo% get
~
#11049
Stampo - Drop (NIENTE)~
1 h 100
~
%echo% drop
~
#11050
Venditore Fabbro (Niente)~
0 n 100
~
eval prodotto1 11015
global prodotto1
eval prodotto2 11016
global prodotto2
~
#11051
Venditore Fabbro 2 (Niente)~
0 c 100
dammi~
eval i 1
while %i% < 10
   eval tmp %i% + 1
   switch %i%
      case 1
         eval vnum %prodotto1%
      break
      case 2
         eval vnum %prodotto2%
      break
      case 3
         eval vnum %prodotto3%
      break
      case 4
         eval vnum %prodotto4%
      break
      case 5
         eval vnum %prodotto5%
      break
      case 6
         eval vnum %prodotto6%
      break
      case 7
         eval vnum %prodotto7%
      break
      case 8
         eval vnum %prodotto8%
      break
      case 9
         eval vnum %prodotto9%
      break
   done
   if %vnum%
      %load% obj %vnum% %self% inv
   end
   eval i %tmp%
done
if %arg% == lista
* mostro la lista
eval ogg %self.inventory%
   while %ogg%
      eval temp2 %ogg.next_in_list%
      %echo% &e%self.name% dice, 'Posso venderti %ogg.shortdesc% per %ogg.cost% monete'&0
      %purge% %ogg%
      wait 1 s
      eval ogg %temp2%
   done
halt
end
* vende l'oggetto se c'e' in inventario
eval nosell vero
eval prodotto %self.inventory%
while %prodotto%
   eval temp3 %prodotto.next_in_list%
   if %prodotto.shortdesc% /= %arg%
      if %actor.gold% < %prodotto.cost%
         %send% %actor.name% &r%self.name% ti dice, 'Ti servono %prodotto.cost% monete e tu ne hai solo %actor.gold%.'&0
         eval nosell falso
      else
         %send% %actor.name% &r%self.name% ti dice, 'Affare fatto.'&0
         %actor.gold(-%prodotto.cost%)%
         %load% obj %prodotto.vnum% %actor% inv
         eval nosell falso
      end
   end
   %purge% %prodotto%
   eval prodotto %temp3%
done
if %nosell% == vero
   %send% %actor.name% &r%self.name% ti dice, 'Non vendo quella roba.'&0
end
~
#11052
Venditore Fabbro 3~
0 j 100
~
%send% %actor.name% &r%self.name% ti dice, 'Non mi interessa. Tienitelo.'&0
return 0
~
#11098
prova di settaggio prof~
0 d 100
*~
set actor.prof(%speech%) 1
%send% %actor.name% %speech% settata a 1
~
#11099
triggo praccone~
0 d 100
*~
%actor.skillset("%speech%" 100)%
~
$~
