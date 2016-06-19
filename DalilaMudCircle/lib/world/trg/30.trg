#3001
romilda1~
0 e 1
arriva~
wait 10
say Mamma mia, ha proprio un'aria distrutta sa?
wait 10
gasp
wait 17
say Dovrebbe proprio riposare in uno dei nostro comodi e convenienti letti....
wait 25
smile
wait 100
bella giornata oggi eh?
say bella giornata oggi eh?
wait 90
say quasi quasi vado a farmi un giro all'aperto
wait 90
yawn
wait 170
slee
wait100
wait 100
wake
wait 20
say ehm....ops!
wait 200
~
#3002
jagat1~
0 g 55
~
wait 10
Say acc.......ma dove ho messo le chiavi della citta'????
~
#3003
strillone1~
0 g 100
~
wait 10
Say UDITE! UDITE! La Nobile Elhana cerca prodi avventurieri che abbiano voglia di svolgere importanti compiti per Lei!
wait 30
say Chi e' interessato parli con me....
wair 30
smile
wait 20
emote indica il cartello che ha attorno al collo e ti invita a leggerlo.
~
#3004
strillone2~
0 d 1
Regina~
wait 10
say Oh, la Nobile Elhana e' una persona veramente adorabile
wait 30
say Il suo nome e' conosciuto in tutta Dalila, servirla e' un onore per tutti.
wait 20
bounce
~
#3005
strillone 3~
0 d 1
dove~
wait 10
say Beh, arrivare dalla Regina Elhana non e' troppo difficile.
wait 20
say Come ben saprai vive a Chyrellos, e per raggiungere quella citta' dovrai percorrere
say il sentiero che dalle porte ovest di Deira si dirama verso nord
wait 20
emote riprende fiato
wait 30
say a quel punto dovrai vagare un po' per il bosco verso ovest
wait 10
say e dopo un po' dovresti imbatterti in un sentiero che va in direzione nord-ovest
wait 20
say a quel punto non devi fare altro che seguire il sentiero e proseguire poi nord
wait 30
say e sarai a Cyrellos!
wait 25
smile
say Elhana si trova nel Castello di Cyrellos dentro alla citta'
~
#3006
strillone4~
0 d 0
cosa~
wait 10
say La Nobile Elhana e' in grado di offrirti delle vantaggiosissime Quest
wait 40
say Che incrementeranno la tua fama in questo regno, e arricchiranno anche le tue tasche
wait 35
say certo che siete tutti dei venali....
~
#3007
guardia1~
0 g 100
~
wait 20
say Salve messer %actor%, e' un onore incontrarla oggi
wait 30
bow
~
#3009
Cade il piatto (by Helsyn)~
1 h 100
~
if (%self.vnum% != 3063)
   wait 1 s
   %send% %actor% Cerchi di liberarti dal peso di %self.shortdesc%.
   wait 1 s
   %send% %actor% Stanco di tenere %self.shortdesc% cerchi un posto dove poggiarlo.
   %echoaround% %actor% %actor.name% sembra stanco di sorreggere %self.shortdesc%.
   wait 1 s
   %send% %actor% Ma sbadato come sei lasci che il piatto si svuoti.
   %echoaround% %actor% %actor.name% lascia che tutto il cibo esca dal piatto. Che spreco!!!!
   wait 1 s
   %send% %actor% ora non hai altro che un piatto vuoto. Mi dispiace per te.
   %echoaround% %actor% %actor.name% ora possiede un bel piatto vuoto!!.
   otransform 3063
end
~
#3010
simona~
0 d 100
elfo apostolo~
wait 20
say oh, Saluta Roger quando lo vedi!
wait 30
smile
~
#3011
Labirinto del circo1 (Nimrhuin)~
2 g 100
~
wait 1
if %actor.vnum% > -1
 halt
end
eval i 6
eval usc %random.6%
while %i% > 0
 eval cas %random.140%-1
 if %cas% < 2
  eval uscita 75
 elseif %cas% < 5
  eval uscita 74
 elseif %cas% < 30
  eval uscita 73
 elseif %cas% < 45
  eval uscita 72
 elseif %cas% < 70
  eval uscita 71
 elseif %cas% < 100
  eval uscita 70
 else
  eval uscita 0
 end
 if %i% == %usc%
  eval uscita 65
 end
 if %uscita% > 0
  if %i% == 1
   %door% %self.vnum% north purge
   %door% %self.vnum% north room 88%uscita%
  elseif %i% == 2
   %door% %self.vnum% east purge
   %door% %self.vnum% east room 88%uscita%
  elseif %i% == 3
   %door% %self.vnum% south purge
   %door% %self.vnum% south room 88%uscita%
  elseif %i% == 4
   %door% %self.vnum% west purge
   %door% %self.vnum% west room 88%uscita%
  elseif %i% == 5
   %door% %self.vnum% up purge
   %door% %self.vnum% up room 88%uscita%
  else
   %door% %self.vnum% down purge
   %door% %self.vnum% down room 88%uscita%
  end
 end
 eval i %i%-1
done
~
#3012
Labirinto del circo2 (Nimrhuin)~
2 g 100
~
wait 1
if %actor.vnum% > -1
 halt
end
eval i 6
while %i% > 0
eval cas %random.140%-1
 if %cas% < 2
  eval uscita 75
 elseif %cas% < 5
  eval uscita 74
 elseif %cas% < 29
  eval uscita 73
 elseif %cas% < 43
  eval uscita 72
 elseif %cas% < 67
  eval uscita 71
 elseif %cas% < 95
  eval uscita 70
 elseif %cas% < 100
  eval uscita 69
 else
  eval uscita 0
 end
 if %uscita% > 0
  if %i% == 1
   %door% %self.vnum% north purge
   %door% %self.vnum% north room 88%uscita%
  elseif %i% == 2
   %door% %self.vnum% east purge
   %door% %self.vnum% east room 88%uscita%
  elseif %i% == 3
   %door% %self.vnum% south purge
   %door% %self.vnum% south room 88%uscita%
  elseif %i% == 4
   %door% %self.vnum% west purge
   %door% %self.vnum% west room 88%uscita%
  elseif %i% == 5
   %door% %self.vnum% up purge
   %door% %self.vnum% up room 88%uscita%
  else
   %door% %self.vnum% down purge
   %door% %self.vnum% down room 88%uscita%
  end
 end
 eval i %i%-1
done
~
#3020
prova1~
0 m 100
~
wait 1 s
eval num %random.10%
set prova ciao
global num
global prova
~
#3021
prova2~
0 d 100
zzz~
wait 2 s
say %num%
say %prova%
say prova
~
#3046
filthy~
0 ab 100
parti~
wait 10
n
wait 20
say Venite a provare la slot machine in Poor Alley!
wait 80
say Con sole 100coins potrete vincere fino a 50kcoins!!!
wait 220
e
wait 20
say Venite a provare la slot machine in Poor Alley!
wait 80
say Con sole 100coins potrete vincere fino a 50kcoins!!!
wait 220
n
wait 20
say Venite a provare la slot machine in Poor Alley!
wait 80
say Con sole 100coins potrete vincere fino a 50kcoins!!!
wait 220
n
wait 20
say Venite a provare la slot machine in Poor Alley!
wait 80
say Con sole 100coins potrete vincere fino a 50kcoins!!!
wait 200
n
wait 20
say Venite a provare la slot machine in Poor Alley!
wait 200
say Con sole 100coins potrete vincere fino a 50kcoins!!!
wait 80
s
wait 20
say Non potete non venire nel nuovissimo Casino' di Deira!
wait 150
e
say Venite a provare la slot machine in Poor Alley!
wait 150
say Non potete non venire nel nuovissimo Casino' di Deira!
wait 60
w
wait 40
say Venite a provare la slot machine in Poor Alley!
wait 120
s
wait 30
say Venite a provare la slot machine in Poor Alley!
wait 380
s
wait 40
say Venite a provare la slot machine in Poor Alley!
wait 100
say Non potete non venire nel nuovissimo Casino' di Deira!
wait 150
w
wait 30
say Il nuovo casino' di Deira qui a sud!
s
wait 2000
wait 3000
wait 4000
wait 10000
wait 4000
~
#3061
Trigger1~
0 g 100
~
Say Oh, che vita grama
wait 6
say La gente e' proprio maleducata...
~
#3090
mob3099~
0 g 100
~
wait 5
smile
say Salve giovane eroe!! Daresti qualche spicciolo ad una povera vecchietta??
~
#3091
mob3099~
0 m 100
~
wait 2 s
emote fa un cenno col capo
smile
say Grazie mille mio eroe.
wait 2 s
say Ho sentito dire che Re Rodric sara' il regnante di Deira.
wait 4 s
say Ma corre voce che anche Lamorka abbia un buon monarca in Otak.
wait 4 s
say Tra i suoi progetti ci sono la restaurazione di Lamorka e la costruzione di un maestoso porto.
em tossisce
wait 5 s
say So che e' gia' in cerca di valorosi cittadini pronti a popolare il suo Regno ricco di risorse e possibilita'.
em si gratta l'anca...due pulci saltano via.
wait 6 s
say Chissa' forse puoi lavorare per lui...se hai abbastanza coraggio si intende...
em sbadiglia
wait 5 s
em conta i tuoi denari
grin
say arrivederci.
~
#3092
mob3099~
0 m 1
~
wait 3 s
say Dai,non essere spilorcio !!! Per 100 monete ti daro' utili informazioni.
wait 1 s 
grin
~
#3099
banchiere~
0 k 100
~
wait 1 s
say AH, Ladri! Assassini! GUARDIEEE!
wait 1
~
$~
