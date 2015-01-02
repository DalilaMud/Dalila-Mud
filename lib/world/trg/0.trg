#1
Cantante (by Nimrhuin)~
0 b 100
~
eval num %random.12%
global num
if %num% <= 11
 halt
end
wait 3 sec
say Life it seems, will fade away
wait 3 sec
say Drifting further every day 
wait 3 sec
say Getting lost within myself 
wait 3 sec
say Nothing matters no one else 
wait 6 sec
say I have lost the will to live 
wait 3 sec
say Simply nothing more to give 
wait 3 sec
say There is nothing more for me 
wait 3 sec
say Need the end to set me free 
wait 10 sec
say Things not what they used to be 
wait 3 sec
say Missing one inside of me
wait 3 sec
say Deathly lost, this cant be real
wait 3 sec
say Cannot stand this hell I feel 
wait 3 sec
say Emptiness is filling me 
wait 3 sec
say To the point of agony 
wait 3 sec
say Growing darkness taking dawn
wait 3 sec
say I was me, but not hes gone
wait 20 sec
say No one but me can save myself, but its too late
wait 3 sec
say Now I can think, think why I should even try 
wait 10 sec
say Yesterday seems as though it never existed 
wait 3 sec
say Death Greets me warm, now I will just say goodbye
~
#2
Cancamini~
0 b 100
~
eval num %random.12%
global num
if %num% <= 11
halt
end
wait 3 sec
emo canticchia allegramente tra se': '&7 Cam camini, cam camini, spazzacamin... Allegro e felice, pensieri non ho... &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 Cam camini, cam camini, spazzacamin...  La sorte e' con voi se la mano vi do... &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 chi un bacio mi da', felice sara'... &0'
wait 1 sec
emo ammicca sornione.
wait 3 sec
emo canticchia allegramente tra se': '&7 Tu penserai che lo spazzacamin, si trovi nel mondo al piu' basso gradin, &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 io sto fra la cenere eppure non c'e' nessuno quaggiu' piu' felice di me. &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 Cam camini, cam camini, spazzacamin... Allegro e felice, pensieri non ho... &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 Cam camini, cam camini, spazzacamin...  La sorte e' con voi se la mano vi do... &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 Cam camini, cam camini, spazzacamin...  e' allegro e felice pensieri non ha... &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 cam camini, cam camini, spazzacamin... la sorte e' con te  se la mano ti da' &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 Scelgo le spazzole proprio a puntin, &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 con una la canna, con l'altra il camin. &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 La' dove il fumo si perde nel ciel &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 lo spazzacamino ha il suo mondo piu' bel, &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 tra la terra e le stelle di Londra nel cuor &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 rischiara la notte un vago chiaror. &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 Sopra i tetti di Londra, oh!, che splendor! &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 Cam camini, cam camini, spazzacamin... La mano puoi dar alla felicita' &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 e' bello vivere sempre cosi', e insieme cantar cam cam camini... &0'
wait 3 sec
emo canticchia allegramente tra se': '&7 cam camini cam cam lo spazzacamin... &0'
wait 3 sec
emo sorride.
~
#3
Kit-Serrature (by Phantom)~
1 c 100
installa~
%echo% This trigger commandlist is not complete!
~
#4
Pedana Teleport (by Nessuno)~
1 c 100
*~
* Lista comandi
%echo% Una pedana in granito emette un debole suono.
if %cmd% == parola
  set passwd %arg%
  remote passwd %actor.id%
  %echo% passwd settata in %arg%
  halt
end
if %cmd% == portami
  if %arg% != %passwd%
    %echo% sbagliato devi dire portami %passwd%
    halt
  else
    %echo% giusto sei entrato
  end
end
return 0
halt
%send% %actor% &rImprovvisamente ti ritrovi in un altro luogo, circondato da beni di grande valore.&d
wteleport %actor.name% 2822
wait 1
wforce %actor.name% look
 
~
#5
Autoeliminazione Progetti Abbandonati (Jerold)~
1 ab 100
~
  eval elimina %elimina%+1
  global elimina
  if %elimina% > 10
    opurge self
  end
   
~
#6
Autoeliminazione Notifiche Abbandonate (Jerold)~
1 ab 100
~
eval elimina %elimina%+1
global elimina
if %elimina% > 500
end
~
#7
purge chiavi~
1 ab 100
~
eval eli %eli% + 1
global eli
if %eli% > 1000
opurge chiave
end
~
#8
Autoeliminazione~
1 ab 100
~
eval elimina %elimina%+1
global elimina
if %elimina% > 2
oteleport 1 %self%
end
~
#10
Seguace Night 1 (Jerold)~
0 g 100
~
if %actor.name% == Night
set recita %random.4%
global recita
wait 2 s
if %recita% == 4
Say &1&x Ben tornato Capo! &0
elseif %recita% == 3
Say &1&x Ehila! &0 
elseif %recita% == 2
Say &1&x Mio signore! &0 
else
emote ti sorride felice.
end
else
wait 1 s
Say &1&x Ehi tu! Questo non e' luogo per te! O mi dici la parola d'ordine o ti accoppo! &0
set target %actor.name%
global target
set allerta on
global allerta
set conta 0
global conta
end
~
#11
Seguace Night 2 (Jerold)~
0 b 100
~
if %allerta% == on
if %conta% == 0
Say &1&x Di le tue ultime preghiere! &0
wait 1 s
if %self.room% != %target.room%
set allerta off
global allerta
halt
else
set allerta off
global allerta
unset conta
mkill %target%
if %self.room% != %target.room%
halt
else
eval conta %conta%+1
global conta
end
else
end
else
halt
end
end
~
#12
Seguace Night 3 (Jerold)~
0 d 1
darklotus~
if %speech% == darklotus
if %actor.name% == %target%
set allerta off
global allerta
wait 2 s
Say &2&x Si! Puoi restare! &0
unset conta
unset target
else
Say &1&x Non l'ho chiesto a te! &0
end
else
Say &1&x Non credo sia la parola d'ordine giusta. &0
end
~
#13
Setta Capo delle Guardie (by Phantom)~
0 d 1
numero_guardie vnum_guardie ufficio regno caserma cella~
if %actor.level% > 74 && %actor.vnum% == -1
  if %speech% /= ufficio
    set %speech%
    global ufficio
    say Il mio ufficio e' nella stanza %ufficio%
  end  
  if %speech% /= numero_guardie
    set %speech%
    global numero_guardie
    say Numero massimo di guardie disponibili %numero_guardie%
  end 
  if %speech% /= vnum_guardie
    set %speech%
    global vnum_guardie
    say le mie guardie hanno il vnum %vnum_guardie%
  end    
  if %speech% /= caserma
    set %speech%
    global caserma
    say La ronda si riunisce nella stanza %caserma%
  end
  if %speech% /= cella
    set %speech%
    global cella
    say I Prigionieri saranno chiusi nella stanza %cella%
  end     
  if %speech% /= regno
    set %speech%
    global regno
    if %regno% == aldaria
      set regno_id 5
    elseif %regno% == sendaria
      set regno_id 6
    elseif %regno% == elenia
      set regno_id 2
    elseif %regno% == tolnedra
      set regno_id 3
    else
      say Non conosco questo Regno
      unset regno
      unset regno_id
    end
   
    if %regno%
      say Serviro' %regno% fino alla morte. 
      global regno_id
    end
  end
    
end
~
#14
Capo delle guardie Posixiona sentinelle (by Phantom)~
0 d 1
grado posiziona finito completa 1 2 3 4 5 6 7 8 9~
       if %actor.vnum% != -1
         halt
       end
       if %actor.clan% != %regno_id%
         say Fuori di qui!!! O vi metto agli arresti!!!
         halt
       end
       if %actor.rank% < 25
         say Lasciatemi in pace!!! 
         halt
       end
       if %actor.rank% > 28 && %speech% /= grado
         set %speech%
         global grado
         halt
       end
          
       if %azione% == 0        
         if %speech% == posiziona
           set azione 1
           global azione
           wait 2
           foll %actor.name%
           wait 2
           say Ditemi il singolo punto da modificare (1,2,3,etc.)...o ditemi se e' il caso di cambiare la lista completa
           halt
         end
       end
       if %speech% == finito
         if %azione% == 2
           set azione 0
           global azione
           wait 2
           emote torna in caserma
           wait 2
           unset cambio
           mgoto %ufficio%
           foll capog
           halt
         end
       end
        
      if %azione% == 1
        if %speech% == completa
          wait 2
          say Bene, ora portatemi nei vari luoghi di appostamento, per farmi memorizzare una locazione bastera', una volta sul luogo, che di diciate quante guardie posizionare
          wait 2
          say Finite le guardie da appostare o i numeri di appostamenti memorizzabili (9), le restanti guardie saranno adibite a ronde casuali per le vie cittadine
          set azione 2
          global azione
          set cambio 1
          global cambio
          set completa on
          global completa
          halt
        end
      
        if %speech% > 0 & %speech% < 10
          wait 2
          set cambio %speech%
          global cambio
          say Ok, portatemi nel nuovo luogo di appostamento %cambio% e ditemi la quantita' di guardie da posizionare
          set azione 2
          global azione
          set completa off
          global completa
          halt
        end
      end
      
      if %azione% == 2    
        if %speech% > 0 & %speech% < 7
          wait 2
          emote segna il posizionamento di %speech% guardie in questa stanza
          set %cambio% %self.room%
          global %cambio%
          set %cambio%a %speech%
          global %cambio%a
          wait 2
        
          if %completa% == on
            if %cambio% < 9
              eval cambio %cambio% + 1
              global cambio
              emote fa cenno di proseguire verso l'appostamento numero %cambio%
              halt
            end
          end
          if %cambio% > 8 || %completa% != on
            say L'elenco e' completo, sara' operativo al prossimo turno di guardia.
            wait 1 s
            Say Messer %actor.name%, vi lascio per tornare alle mie mansioni
            wait 1 s
            bow
            wait 1 s
            emote torna rapidamente in caserma
            mgoto %ufficio%
            foll capo
            set azione 0
            global azione
            unset cambio
          end
        end
      end
      
~
#15
La Guardia esegue un arresto (by Phantom)~
0 k 100
~
if %actor.vnum% != -1
  halt
end
if %actor.hit% < 0
  say Per ora ti porto in cella... li verrai giudicato!
  wait 1 s
  %echo% %self.name% si carica sulle spalle %actor.name%
  wait 1 s
  %echo% %self.name% si allontana con %actor.name% sulle spalle.
  wait 1 s
  %teleport% %actor% %cella%
  wait 1 s
  %goto% %postazione%
end
~
#20
Prigione 1 (Faymond)~
2 c 100
forza~
%send% %actor% &5Inizi ad accumulare rabbia per concentrarla in un unico, potente colpo.&0
%echoaround% %actor% %actor.name% flette e gonfia ogni muscolo del suo corpo.
wait 13 s
global forza
set caso %random.45%
~
#64
delete me~
1 g 2
assapora~
tele %actor.name% Yhumm! Che boccone prelibato! La tenerezza e il sapore della
%tele% %actor.name% carne unita alla fragranza di un vino novello. E' quasi un
tele %actor.name% peccato masticarlo...
echo buooono
~
#95
go e patrol~
0 d 100
vai controlla~
go %self% mob elementale kill eleme
~
#99
prova deleteme~
1 c 4
*~
%echo% This trigger commandlist is not complete!
~
$~
