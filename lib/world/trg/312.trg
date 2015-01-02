#31202
Campo Energetico (Faymond)~
2 g 100
~
if %actor.level% <= 30
wait 1 s
%echo% Senti il potere impossessarsi del tuo corpo!! Per un attimo i tuoi occhi brillano di mille colori!!
wait 2 s
%echo% Sei miracolosamente sopravvissuto alla fortissima energia
wait 4 s
%echo% un varco si apre nel campo energetico...
elseif %actor.level% <= 52
wait 1 s
%echo% Senti il potere impossessarsi del tuo corpo!! Per un attimo i tuoi occhi brillano di mille colori!!
wait 2 s
%echo% NO!! L'ENERGIA E' TROPPO FORTE!! TI STA BRUCIANDO FURIOSA!!!
wait 2 s
%echo% con un gesto disperato, gravemente ferito ti lanci fuori dal campo di energia...
%damage% %actor% 200
wteleport %actor% 31201
else
wait 1 s
%echo% Non appena metti piede nella stanza ti si annebbia la vista e un dolore atroce ti squarcia la schiena..
wait 2 s
%echo% la tua energia e' troppo forte e il tuo corpo crolla sotto la tua stessa potenza micidiale...
wait 2 s
%echo% Addio, potente avventuriero!
%damage% %actor% 2000
end
~
#31233
Crollo (Faymond)~
2 g 100
~
wait 6 s
%echo% &7&xSenti che il suolo si sbriciola sotto di te inghiottendoti dapprima le braccia, poi le gambe...&0
wait 4 s
%echo% &x&7...fino a farti sprofondare in una frana di terriccio e sassi.. prega di restare tutto intero!!&0
wteleport all 31234
wait 2 s
%force% %actor% look
~
#31238
Radice viscida (Fay)~
2 g 100
~
wait 3 s
%echo% &2Scivoli lungo la superficie unta e squamosa di questa grossa radice rampicante per diversi metri&0
%echo% &2prima che essa cominci disgustosamente a dimenarsi tra le tue braccia!&0
wait 8 s
%echo% &2Per lunghi tragici secondi temi il peggio, stretto con tutte le tue forze all'infame vegetale...&0
wait 4 s
%echo% &2finche' con un sussulto la radice ti si scrolla di dosso, lasciandoti fortunosamente cadere sulla sponda rocciosa!&0
wteleport all 31239
wait 5 s
%force% %actor% look
~
#31239
Yugoloth (Faymond)~
0 k 100
~
wait 7 s
set punto %random.40%
global punto
if %punto% >= %actor.dex%
%send% %actor% &6La disgustosa lingua dello Yugoloth si avvinghia attorno alla tua caviglia scaraventandoti a terra!&0
%echoaround% %actor% &6La disgustosa lingua dello Yugoloth si avvinghia attorno alla caviglia di %actor.name% facendolo crollare a terra!&0
dg_cast %actor.name% 'disintegrate'
%force% %actor% siedi
else
%echo% &6L'orrida lingua dello Yugoloth scatta nell'aria ma non riesce ad avvinghiarsi a nulla!&0
end
~
#31242
Ponte dei sospiri 1 (Faymond)~
2 g 100
~
wait 7 s
%echo% &7Un flebile lamento si solleva soffocato dall'abisso sotto i tuoi piedi, seguito da un altro, ed un altro ancora..&0
wait 4 s
%echo% &7la tua mente rischia di scoppiare.. gli angoscianti, mugolanti lamenti giungono come una pugnalata al tuo cuore.. devi muoverti..&0
wteleport all 31243
~
#31243
Ponte dei sospiri 2 (Faymond)~
2 g 100
~
wait 10 s
if %actor.room% == 31244
%send% %actor% &7I lamenti si stanno facendo assordanti... e' terribile... devi affrettarti!!&0
wait 15 s
if %actor.room% == 31244
%send% %actor% &1Le urla di angoscia ti echeggiano nella testa tanto da farti impazzire... la luce sparisce, mentre precipiti nell'abisso&0
%send% %actor% &1Questa e' la tragica morte che aspetta chi viola il Ponte dei Sospiri.&0
%echoaround% %actor% &1%actor.name% con un ultimo urlo sprofonda nell'abisso dei sospiri.. per sempre..&0
%damage% %actor% 5000
wait 1 s
purge corps
else
end
else
end
~
#31245
Ponte 3 (Faymond)~
2 g 100
~
wait 10 s
if %actor.room% == 31245
%send% %actor% &7I lamenti si stanno facendo assordanti... e' terribile... devi affrettarti!!&0
wait 15 s
if %actor.room% == 31245
%send% %actor% &1Le urla di angoscia ti echeggiano nella testa tanto da farti impazzire... la luce sparisce, mentre precipiti nell'abisso&0
%send% %actor% &1Questa e' la tragica morte che aspetta chi viola il Ponte dei Sospiri.&0
%echoaround% %actor% &1%actor.name% con un ultimo urlo sprofonda nell'abisso dei sospiri.. per sempre..&0
%damage% %actor% 5000
wait 1 s
purge corps
else
end
else
end
~
#31246
Ponte 4 (Faymond)~
2 g 100
~
wait 10 s
if %actor.room% == 31246
%send% %actor% &7I lamenti si stanno facendo assordanti... e' terribile... devi affrettarti!!&0
wait 15 s
if %actor.room% == 31246
%send% %actor% &1Le urla di angoscia ti echeggiano nella testa tanto da farti impazzire... la luce sparisce, mentre precipiti nell'abisso&0
%send% %actor% &1Questa e' la tragica morte che aspetta chi viola il Ponte dei Sospiri.&0
%echoaround% %actor% &1%actor.name% con un ultimo urlo sprofonda nell'abisso dei sospiri.. per sempre..&0
wteleport %actor.name% 31247
%damage% %actor% 5000
else
end
else
end
~
#31250
Empatia dell'odio (Faymond)~
2 g 100
~
wait 6 s
%echo% &1Prende forma una tua immagine speculare, rossa e cupa come sangue, col volto contratto in un'espressione d'ira furibonda&0
wait 4 s
if %actor.level% < 10
wload mob 2620%actor.level%
elseif %actor.level% < 71
wload mob 262%actor.level%
wforce empatia hit %actor.name%
else
end
~
#31251
Porta parlante (Faymond)~
2 g 100
~
wait 2 s
%echo% &8la tetra cancellata sembra prendere vita,e lentamente le sue ante si modellano fino a formare un viso privo di occhi.&0
wait 4 s
%echo% &8Quando l'enorme bocca si apre per interrogarti a malapena riesci a distinguere il suono metallico delle sue parole dall'assordante rimbombare delle pareti.&0
wait 7 s
%echo% La porta echeggia: '&5Straniero, che metti piede in questa terra di nessuno...&0'.
wait 5 s
%echo% La porta echeggia: '&5Sappi che tre porte dietro di me ti sbarrano la strada&0'.
wait 4 s
%echo% La porta echeggia: '&5La strada verso Lach-Nadar... la capitale del sottosuolo.&0'.
wait 5 s
%echo% La porta echeggia: '&5Le tre porte delle tue paure piu' recondite, che piu' scacci e piu' vigorosamente riemergono..&0'.
wait 5 s
%echo% La porta echeggia: '&5Sconfiggile, e le avrai fatte tue.. e sarai degno delle estasi di Lach-nadar&0'.
wait 5 s
%echo% La porta echeggia: '&5Soccombi, e avrai anticipato di poco la tua fine..&0'.
wait 5 s
%echo% &8L'enorme bocca si spalanca per lasciarti passare oltre..&0
wteleport all 31252
~
#31253
Prova morte Odio (faymond)~
2 c 100
kick~
wait 1 s
%echo% BRAVOOOOOOOOOOOOOOOOOOOOOOOOOOO
~
#31257
Chiave (Faymond)~
2 g 100
~
wait 10 s
wload obj 31253
~
#31258
Ponte di Anime (Faymond)~
2 g 100
~
wait 10 s
%echo% &7Per poco non perdi fatalmente l'equilibrio quando una delle anime striscia via da sotto il tuo piede destro&0
%echo% &7per andare a prender posto sulla cima del ponte...ti conviene affrettarti&0
wait 9 s
%echo% &7Il ponte continua nella sua tetra danza d'anime verso l'ignoto, ma tu non sarai con lui...&0
%echo% &7sei ancora cosciente quando i dannati del Lago di Sangue ti dilaniano ferocemente.&0
%damage% all 5000
~
$~
