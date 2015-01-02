#9000
Shargugh~
0 g 100
~
wait 3
say Attento avventuriero ... questo bosco e' abitato ...
cringe
say da creature demoniache!
~
#9001
Lance~
0 d 2
chiave portafortuna~
if %actor.name% == lance || %actor.name% == arkan
if %speech% == chiave
mload obj 9088
give chiave %actor.name%
say Ecco a lei Sir. %actor.name%!
end
if %speech% == portafortuna
mload obj 9744
give portafortuna %actor.name%
say Ecco a lei Sir. %actor.name%!
end
end
~
#9002
Malviventi~
0 d 1
Attenti Riposo Follami Sfollami Assisti Stop~
  wait 1 s
if %actor.name% == lance
if %speech% == comandi
wait 1 s
say I comandi per ora sono: Attenti, Riposo, Follami, Sfollami, Assisti, Stop
end
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
set attenti on
global attenti
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
if %speech% == assisti
assist %actor.name%
end
if %speech% == stop
stop
end
end
else
wait 1 s
emote guarda con aria indifferente Lord %Actor.name%
end
~
#9003
entrata cvo~
0 d 1
Honor~
wait 1 s
msend %actor% Manolesta ti apre la porta segrata a sud ed entri nel covo.
mtele %actor% 40008
~
#9004
entrata covo 2~
0 m 1
~
if %amount% == 1
mteleport %actor% 40008
wait 1 s
msend %actor% Manolesta ti apre la porta segrata a sud ed entri nel covo.
else
wait 1 s
say Vattene! altrimenti farai una brutta fine
end
return 0
~
#9005
new trigger~
0 d 1
prigione~
if %actor.name% == lance || %actor.name% == arkan
if %speech% == prigione
mload obj 9099
give chiave %actor.name%
say Ecco a lei Sir. %actor.name%!
end
end
~
#9040
kill~
0 d 1
follami sfollami assisti stop attenti riposo comandi~
   wait 1 s
if %actor.name% == talos || %actor.name% == sadric || %actor.name% == roscellino || %actor.name% == fender || %actor.name% == feanor || %actor.name% == spartakus || %actor.name% == sarib
if %speech% == comandi
wait 1 s
say I comandi per ora sono: Attenti, Riposo, Follami, Sfollami, Assisti, Stop
end
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
set attenti on
global attenti
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
if %speech% == assisti
assist %actor.name%
end
if %speech% == stop
stop
end
end
else
wait 1 s
emote guarda con aria indifferente Lord %Actor.name%
end
~
#9041
Comandi Guardie (by Phantom)~
0 d 1
ronda settore grado postazione attenti riposo cella~
if (%actor.name% == kain || %actor.name% == sheep) && %piazzata% != on 
  if %speech% == Oggi sei di ronda
    unset azione
  end
  if %speech% /= settore
    set %speech%
    global settore
  elseif %speech% /= grado
    set %speech%
    global grado
  elseif %speech% /= cella
    set %speech%
    global cella
  elseif %speech% /= postazione
    set %speech%
    global postazione
    set piazzata on
    global piazzata
    wait 1
    nod
    wait 1
    mgoto %postazione%
    wait 5
    emote arriva ed inizia il proprio turno di guardia.
  end
end
if %piazzata% == on
  if %attenti% == on
    wait 1 s
    if %actor.rank% >= %grado% && %actor.clan% == %settore%
      set pass ok
      global pass
    end
    
    if %pass% == ok
      if %speech% == riposo
        wait 1 s
        nod
        wait 1 s
        emo riprende il suo servizio di ronda.
        unset azione
        foll %self%
      elseif %speech% == attenti
        wait 1 s
        nod
        wait 1 s
        emote scatta sull'attenti!
        set azione on
        global azione
        foll %actor.name%
      end     
      
    end
  else
    wait 1 s
    emote accarezza l'elsa della propria spada.
  end
    unset pass
end
~
#9042
Guardie (non toccare)~
0 d 1
seguitemi rompete attenti stop assist prendete~
if %actor.name% == Feanor || %actor.name% == Roscellino || %actor.name% == Spartakus || %actor.name% == Talos || %actor.name% == Fender
if tar == on
kill %speech%
unset tar
end
wait 1
if %speech% == seguitemi
emote scatta sull'attenti
say Si, My Lord come lei desidera!
foll %actor.name%
end
if %speech% == rompete
emote rompe le righe
say Si, Mylord
foll me
end
if %speech% == attenti
emote scatta sull'attenti
end
if %speech% == stop
stop
end
if %speech% == assist
assist %actor.name%
end
if %speech% == prendete
set tar on
global tar
end
else
wait 1
emote ti guarda
say Non puoi darmi ordini!
end
~
#9043
Rincasa guardie~
0 d 1
Ripiazzati~
set a %actor.name%
if %a% == talos || %a% == Jerold || %e% == roscellino || %a% == fender || %a% == feanor || %a% == spartakus || %a% == Thief || %a% == Kitiara || %a% == Sarib || %a% == Baal || %a% == Keli || %a% == Melyanna || %actor.level% > 70
mgoto 9700
else
end
~
#9050
Check Trigger~
2 h 100
~
wteleport %object% 9800
wteleport %object.name% 9800
~
#9051
Sfera blu Barbone (Jerold)~
0 j 100
~
if %object.vnum% == 9898
set rivelato on
global rivelato
wait 2 s
emote si incanta ad osservare, come rapito, la sfera blu!
wait 2 s
say &xAh!, stupendo... allora e' vero!... la profezia si sta avverando!&0
wait 4 s
if %actor.sex% == Maschio
say &xMio signore...!&0
else
say &xMia signora...!&0
end
wait 4 s
say &x...lei non ci credera' ma io un tempo ero un valoroso esploratore.&0
wait 4 s
say &xHo percorso migliaia di leghe, visitato ogni territorio noto e non noto...&0
wait 4 s
say &x...conosciuto miti e leggende ed una di queste ha a che fare con questa sfera.&0
wait 4 s
emote sembra perso nei suoi ricordi.
wait 4 s
emote si scrolla il pensiero e torna alla realta'
wait 4 s
say &xDisgraziatamente la mia memoria non e' piu' quella di un tempo...&0
wait 4 s
say &x...ma sono certo che esiste un libro da qualche parte che puo' dirvi di piu'.&0
wait 7 s
say &xIl libro e' passato in molte mani e chissa' dov e' finito.&0
wait 4 s
say &xCercate il libro e ne saprete di piu'.&0
wait 4 s
give sferablu %actor.name%
drop sferablu
else
wait 2 s
say &xSpiacente non so cosa sia questa cosa.&0
drop all
end
~
#9080
Primo Discepolo (Jerold / Quest1)~
0 d 1
Solja~
mload mob 12601
wait 20
mecho vittima \&7 Ad un tratto un soffio di vento penetra nella fitta foresta \&0
mecho vittima !!SOUND(vento1.wav)
wait 45
mecho vittima \&7 Poi il silenzio avvolge ogni cosa. \&0
wait 50
mecho vittima \&3 un bagliore ... \&0
mecho vittima !!SOUND(magia.wav)
wait 20
mecho vittima \&7 ... la statua davanti a te prende vita parzialemente! \&0
wait 50
mecho vittima \&7 Grazie per aver pronunziato il nome del mio maestro e avermi cosi' liberato. \&0
wait 40
mecho vittima \&7 So che lui e' morto, ma so che avrebbe fatto qualsiasi cosa a sua disposizione per cercare di salvare la \&1\&x principessa. \&0
wait 60
mecho vittima \&7 Un fremito colpisce la statua. \&0
mecho vittima !!SOUND(blessoff.wav)
wait 50
mecho vittima \&7 Si, hai capito bene, una principessa, figlia del sovrano regnante sulle nostre terre oltre l'oceano di acqua. \&0
wait 50
mecho vittima \&7 Un malvagio l'ha rapita per convincere il re ad abdicare e ci riuscira' se la principessa non fara' ritorno al castello in breve tempo... \&0
wait 60
mecho vittima \&7 Un altro fremito invade il volto della statua. \&0
mecho vittima !!SOUND(blessoff.wav)
wait 50
mecho vittima \&7 Ascolta! pultroppo, come il mio signore, giunti in queste terre abbiamo  perso quasi completamente le nostre forze magiche... \&0
wait 60
mecho vittima \&7 ... quindi non ho sufficente enerigia per liberarmi da questo mio eterno incantesimo. \&0
wait 50
mecho vittima  \&7 Pero' posso darti un altro \&1\&x indizio \&0\&7 per aiutari a trovare la nostra amata principessa... \&0
wait 50
mecho vittima  \&7... sono riuscito a contattare telepaticamente un altro dei tre seguaci catturati.\&0
wait 50
mecho vittima  \&7 Ho visto attraverso i suoi occhi... con lui c'e' un \&2\&x grande guerriero \&0\&7, potente ma buono, sono \&2\&x molto in alto \&0\&7... non ti attacchera' se non lo attaccherai tu.\&0
wait 60
mecho vittima  \&7 Li troverai l'altro discepolo, \&2\&xKlain\&0\&7, digli che \&1\&xL'amore e' dolore\&0\&7... capira'\&0
wait 60
mecho vittima \&7Un smorfia di tremendo dolore invade tutta la statua ...\&0
mecho vittima !!SOUND(Paralize.wav)
mecho vittima \&7...in un attimo realizzi che e' ritornata ad essere un semplice sasso con forme umane.\&0
wait 60
mecho vittima \&7Corri \&2\&x %actor.name% \&0\&7, corri... il tempo e' prezioso... cerca il secondo discepolo!.\&0
mforce vittima mgoto 2
mat 2 mpurge
~
#9081
Secondo Discepolo (Jerold / Quest1)~
0 d 0
l'amore e' dolore~
mload mob 12601
wait 20
mecho vittima \&7 Un piccolo \&1\&x alone di luce \&0\&7 blu invade il corpo della statua... \&0
mecho vittima !!SOUND(bark.wav)
wait 20
mecho vittima \&7 ... e prende vita. \&0
mecho vittima !!SOUND(bark.wav)
wait 20
mecho vittima \&7 Non c'e tempo da perdere! C'e' rimasto davvero poco tempo. \&0
wait 30
mecho vittima \&7 Riesco a vedere nel tuo animo, vedo il tuo  glorioso passato e un glorioso futuro... \&0
wait 50
mecho vittima \&2\&x Aiutaci tu! \&0
wait 30
mecho vittima \&7 Ora ti racconto una breve storia. Ricordatela e, alla fine di questa avventura, raccontala a tua volta alla gente che incontri. \&0
wait 70
mecho vittima \&7 La statua si sposta lentamente in un frastuono assordante. \&0
mecho vittima !!SOUND(Distruzione.wav)
wait 50
mecho vittima \&3\&x Ascolta,  un tempo, lontano da queste terre, vivevano due maghi molto potenti e molto amici tra loro, molto buoni e caritatevoli. \&0
mecho vittima !!MUSIC(Melodia1.mid)
wait 60
mecho vittima \&3\&x Erano al servizio del casato reale del nostro regno.\&0
wait 40
mecho vittima \&3\&x Eravamo in pace con tutti i popoli di quelle terre, i due maghi ci proteggevano e consigliavano saggiamente il nostro re. \&0
wait 65
mecho vittima \&3\&x Ma un giorno accadde qualcosa di inaspettato. \&0
wait 40
mecho vittima \&3\&x Arrivo'  al castello reale, sede del Re e dei due maghi, la principessa, figlia del sovrano, che era vissuta e cresciuta fin da piccola in un posto nascosto a tutti.\&0
wait 70
mecho vittima \&3\&x Il re aveva avuto paura di perdere prematuramente la sua amata primogentita a tal punto da aver nascosto a tutti, compresi i due maghi, la nascita della sua figlia. \&0
wait 70
mecho vittima \&3\&x Ora la principessa, ormai adulta e di incantevole bellezza, aveva fatto ritorno al castello per farsi conoscere alla sua gente e per riabbracciare il vecchio padre. \&0
wait 70
mecho vittima \&3\&x Ma questo suo ritorno suscito' dei due maghi un micidiale e eguale colpo di fulmine. \&0
mecho vittima !!MUSIC(dag_10.mid)
wait 45
mecho vittima \&3\&x I due iniziarono a corteggiarla in modo da prima lieve e poi, giorno dopo giorno, sempre con maggiore insistenza, tanto da trascurare la protezione del regno e di ignorare completamente il re. \&0
wait 70
mecho vittima \&3\&x I maghi, come se non bastasse, iniziarono a odiarsi tra loro e a scaglairsi addosso incantesimi sempre piu' potenti e devastanti, finche' un giorno uno dei due rapi' la principessa e scomparve nel cielo. \&0
wait 70
mecho vittima \&3\&x L'altro mago, resosi conto di cio' che era successo, rinsavi', chiese subito perdono al re e si impegno' a ritrovare la principessa e riportarla a casa sana e salva.\&0
wait 70
mecho vittima \&3\&x Prese quattro suoi discepoli e anche lui scomparve nel cielo alla rincorsa del mago e e della principessa. \&0
wait 70
mecho vittima \&3\&x Come avrai capito il mago rinsavito era \&1\&x Solja \&3 e io sono uno dei \&1\&x 4 \&3 seguaci. \&0
wait 45
mecho vittima \&3\&x Intanto il secondo mago riusci' ad arrivare in queste terre che pero', stranamente, prosciugarono  molta della sua energia magica... \&0
wait 70
mecho vittima \&3\&x ... come e' successo a noi, ma non abbastanza per impedirgli di imprigionare la principessa in un posto a me sconosciuto e di eliminare il mago rivale e trasformare noi 4 in stature di piera per l'eternita'. \&0
wait 70
mecho vittima \&3\&x Pero' anch'io posso darti un altro indizio. \&0
wait 45
mecho vittima \&3\&x Prima di essere trasformato in pietra ho sentito che il perfido mago ordinava ai suoi discepoli di portere uno dei miei amici in un posto ben protetto. \&0
wait 50
mecho vittima \&3\&x Ho sentito parlare di \&1 una casa bruciata, \&3 di \&1 ripidi sentieri \&3, di una \&1 citta' \&3 e di un vicino \&1 lago. \&0
wait 50
mecho vittima \&3\&x Trovalo e digli il mio nome, \&1\&x Klain \&3 , sara' sufficente per dargli il coraggio di liberarsi momentaneamente dall'incantesimo e darlgi il modo di aiutarti. \&0
wait 70
mecho vittima \&3\&x Buona fortuna. \&0
wait 20
mecho vittima Un tuono, !!SOUND(Thunder1.wav) un bagliore e la figura torna immobile per l'eternita'.
wait 40
mecho vittima \&3\&x Capito dove si trova il terzo discepolo \&2 %actor.name% \&0 ?
mforce vittima mgoto 2
mat 2 mpurge
~
#9082
Terzo discepolo (Jerold / Quest1)~
0 d 1
Klain~
mload mob 12601
wait 20
mecho vittima \&7 Una minuscola \&2\&x lacrima \&0\&7 scende dall'occhio della statua...\&0
wait 30
mecho vittima \&7 ... nella zona del cuore una luce rossa prende vita.\&0
mecho vittima !!SOUND(fuoco.wav)
wait 45
mecho vittima \&3\&x Klain \&0\&7, fratello di sangue, ora solo sento che anche tu sei stato trasformato in un blocco di pietra.\&0
wait 50
mecho vittima \&1\&x Destino! \&0
wait 50
mecho vittima \&7 E a te nobile amico \&2\&x GRAZIE \&0\&7 per avermi momentaneamente riportato alla vita. \&0
wait 50
mecho vittima \&7 Almeno, per un altro istante, ho la possibilita' di aiutarti a liberare la principessa \&3\&x Sheela. \&0
wait 50
mecho vittima \&7 Klain ti avra' senz'altro raccontato la storia dei due maghi, ma senz'altro non ti ha detto che l'ultimo di noi 4 seguaci... \&0
wait 70
mecho vittima \&7 ...quello che devi ancora trovare non era ancora un mago e che quindi non ha nessun potere magico... \&0
wait 50
mecho vittima \&7 ...ma e dotato di molte qualita' che al fine della tua missione potranno tornarti utili. \&0
wait 50
mecho vittima \&7 Quindi non c'e nessuna parola magica o di tale potenza da risvegliare l'ultimo discepolo dal suo pietrificato sonno. \&0
wait 70
mecho vittima \&7 Ma non e' detto che la magia non possa liberarlo momentaneamente, anzi! \&0
wait 50
mecho vittima \&7 Mentre sorvolavo la vostre terre, prima di essere catturato e trasformato ho lasciato cadere un \&1\&x oggetto \&0\&7 che se consegnato al mio amico potra' aiutarlo a liberarsi... \&0
wait 70
mecho vittima \&7 ... non so per quanto tempo... queste strane terre hanno la caratteristica di prosciugare le energie magiche estranee. \&0
wait 70
mecho vittima \&7 Ora non so con esattezza dove si trova l'oggetto ma piu' o meno te lo posso indicare... \&0
wait 50
mecho vittima \&7 ...l'ho lasciato cadere su della \&2\&x alte cime \&0\&7 molto vicino ai \&2\&x confini est \&0\&7 del tuo mondo... \&0
wait 50
mecho vittima \&7 ... lo so e' generica come indicazione... \&2\&x aspetta \&0\&7, ricordo anche di aver visto un po' piu' a \&2\&x sud \&0\&7 una strana costruzione, una specie di \&1\&x santuario. \&0
wait 70
mecho vittima \&7 La statua alza le braccia al cielo. \&0
wait 30
mecho vittima \&7 Un ultima cosa, l'oggetto che cerchi e' anche lui... \&0
wait 30
mecho vittima \&7 Troppo tardi l'incantesimo ha ripreso la vita del discepolo per sempre. \&0
mecho vittima !!SOUND(Handwind.wav)
wait 50
mecho vittima \&1\&x Cerca l'oggetto... cerca l'ultimo discepolo. \&0
mforce vittima  mgoto 2
mat 2 mpurge
~
#9083
Quarto discepolo (Jerold / Quest1)~
0 j 100
~
mload mob 12601
if %object.vnum% == 9016
wait 1 s
junk sfera
wait 20
mecho vittima !!SOUND(Magic2.wav)
mecho vittima \&7 Appena posi l'oggetto sulla statua la sfera inizia ad \&3\&x illuminarsi \&0\&7 e a proiettare un fascio di luce \&6\&x purpurea \&0\&7 attorno alla sagoma del pietrificato discepolo. \&0
wait 65
mecho vittima \&7 Ad un tratto gli occhi della statua si aprono e un filo di voce esce dalla sua bocca. \&0
wait 50
mecho vittima \&7 Credevo di non aver piu' speranze... grazie. \&0
wait 40
mecho vittima \&7 Immagino che tu sia qui per aiutarci, vedo nel tuo volto i segni della stanchezza, questo mi dimostra che hai percorso molta strada per giungere fino a me. \&0
wait 70
mecho vittima \&7 Pero', grazie al tuo sforzo, la meta e' vicina, ancora \&2\&x un piccolo \&0\&7 sacrificio. \&0
wait 50
mecho vittima \&7 Non molto distante da qui c'e un luogo circondato da \&1\&x salici piangenti \&0\&7 e nel \&1\&x centro \&0\&7 troverai la porta che ti condurra' alla prigione della Principessa. \&0
wait 70
mecho vittima \&7 Anche lei per tornare alle sue terre avra' bisogno di un \&2\&x oggetto. \&0
wait 50
mload obj 9017
drop specchio
wait 20
mecho vittima \&7 Prendi lo specchio dallo alla principessa e salva cosi' il nostro regno dal malvagio mago. \&0
wait 50
mecho vittima \&7 la sfera inizia a spegnersi e la luce attorno alla statua inizia a scemare. \&0
mecho vittima \&7 !!SOUND(Sancoff.wav)
wait 40
mecho vittima \&7 La nostra magia qui non funziona come dovrebbe... \&0
wait 40
mecho vittima \&7 ...la sfera si e' esaurita subito... destino! ho perso la magia ancora prima di poterla usare... \&0
wait 60
mecho vittima \&7 ...comunque grazie a te la morte mia dei miei tre amici e del nostro maestro non e' stata vana... \&2\&x grazie!\&0
wait 60
mecho vittima \&7 La sfera si spegne del tutto la statua perde la vita. \&0
mecho vittima !!SOUND(Magic3.wav)
wait 40
mecho vittima \&1\&x E ORA LA PRINCIPESSA!  \&0
else
wait 15
drop all
mecho vittima \&7 la statua sembra non accettare questo oggetto... tenta con un altro. \&0
end
mforce vittima mgoto 2
mat 2 mpurge
~
#9084
Principessa (Jerold / Quest1)~
0 j 100
~
mload mob 12601
if %object.vnum% == 9017
wait 20
mecho vittima \&7 Oh ma questo e' un miracolo... \&0
mecho vittima !!MUSIC(Avvent.mid)
wait 20
mecho vittima \&7 una lacrima scende dal volto della principessa \&0
wait 20
mecho vittima \&7 Che tu sia benedetto. Finalmente potro' tornare tra le braccia di mio padre. \&0
wait 40
mecho vittima \&7 Il mio nome e' Sheela, ma tu lo sai gia' immagino, e il mio popolo mi attende. \&0
wait 40
mecho vittima \&7 Grazie a questo specchio posso tornare sana e salva al mio castello. \&0
wait 40
\&7 emote ti abbraccia calorosamente \&0
wait 20
mecho vittima \&7 Il perfido \&1\&x Forsyth \&0\&7, il mago maligno che ha scatenato tutto questa baraonda, si e' accorto che in queste terre la nostra magia e' davvero poco utile. \&0
wait 50
\&7 emote sospira \&0
wait 20
mecho vittima \&7 Mi ha lasciato in questo posto, convinto di ritrovarmici, mentre lui trovava il modo di recuperare e conservare la propria magia, e credimi e' davvero potente. \&0
wait 60
mecho vittima \&7 Bene come non posso, oltre che ringraziarti, farti due doni prima di tornare alla mia amata terra. \&0
wait 40
\&7 emote si sfila uno dei suoi anelli. \&0
mload obj 9018
give anello %actor.name%
wait 20
mecho vittima \&7 Prendi! questo' e' un piccolo segno di gratitudine e riconoscimento. \&0
wait 40
mecho vittima \&7 Visto che hai fatto molta strada per liberarmi questo ti permettera' di percorrere molta piu' strada prima di stancarsi. \&0
wait 40
\&7 emote ti sorride \&0
wait 20
mecho vittima \&7 E come secondo regalo uso la mia rimanente energia magica per trasportarti vicino ad una delle vostre grandi e stupende citta'. \&0
wait 60
mecho vittima \&7 Grazie ancora... grazie di cuore. \&0
wait 20
mecho vittima \&1\&x ti senti pervadere da una strana sensazione... ma dove ti trovi adesso? meglio dare un occhiata in giro \&0
mteleport %actor.name% 1498505
junk specchio
else
wait 15
drop all
say Questo non mi e' molto utile amico!
end
mforce vittima mgoto 2
mat 2 mpurge
~
#9085
La sfera della vita (Jerold / Quest1)~
1 g 100
~
wait 20
osend %actor.name% \&7 La sfera inizia a \&1\&x brillare \&0\&7  nelle tue mani.\&0
osend %actor.name% !!SOUND(Magic6.wav)
wait 20
osend %actor.name% \&7 ad un tratto senti una voce nella tua mente e' la sfera che ti contatta telepaticamente. \&0
wait 45
osend %actor.name% \&7Grande \&3\&x %actor.name%!\&0\&7 Sei riuscito a trovarmi. \&0
wait 40
osend %actor.name% \&7 Mi presento sono la \&1\&x sfera della vita \&0\&7, capace di donare l'anima a chi se le vista strappare dalle forze del male. \&0
wait 65
osend %actor.name% \&7Coraggio portami dal discepolo bisognoso. So dove puoi trovarlo! \&0
wait 50
osend %actor.name% \&7Sotto le \&1\&x colline ombros \&0\&7 c'e' una citta' e al suo interno un tempio dedicato ad un grande e \&1\&x benevolo Dio. \&0
wait 65
osend %actor.name% \&7Vai li e mettimi tra le mani del bisognoso.\&0
wait 40
osend %actor.name% \&7La sfera smette di brillare!\&0
osend %actor.name% !!SOUND(Magic4.wav)
wait 20
osend %actor.name% \&3\&x Coraggio %actor.name%, sei a meta' della tua avventura!  \&0
~
#9089
Spada di Shade~
1 j 35
~
wait 1 s
osend %actor.name% La Spada di Shade si illumina improvvisamente e avverti un senso di invincibilita'.
wait 3 s
osend %actor.name% La Spada alimenta il tuo odio e la tua sete di sangue.
wait 3 s
osend %actor.name% La Spada telepaticamente ti impone di uccidere!
wait 3 s
osend %actor.name% Si impossessa per un momento della tua mente!
wait 2 s
oecho %actor.name% esclama: Per il volere del Dio del Male SHADE io avro' il vostro sangue!
wait 3 s
osend %actor.name% Ti riprendi...
~
#9090
spada shade 2~
1 b 10
~
oecho Percepisci una aura di malvagita' provenire dalla SPADA DI SHADE.
wait 2 s
oecho La Spada di SHADE si illumina lievemente quando un senso di paura pervade la tua mente.
~
#9091
Pugnale Shade~
1 j 35
~
wait 1 s
osend %actor.name% Il Pugnale di Shade si illumina improvvisamente e avverti un senso di invincibilita'.
wait 3 s
osend %actor.name% Il Pugnale di Shade alimenta il tuo odio e la tua sete di sangue.
wait 3 s
osend %actor.name% Il Pugnale di Shade telepaticamente ti impone di uccidere!
wait 3 s
osend %actor.name% Si impossessa per un momento della tua mente!
wait 2 s
oecho %actor.name% esclama: Per il volere del Dio del Male SHADE io avro' il vostro sangue!
wait 3 s
osend %actor.name% Ti riprendi...
~
#9092
Pugnale shade 2~
1 j 10
~
oecho Percepisci una aura di malvagita' provenire dal PUGNALE DI SHADE.
wait 2 s
oecho Il Pugnale di SHADE si illumina lievemente quando un senso di paura pervade la tua mente.
~
$~
