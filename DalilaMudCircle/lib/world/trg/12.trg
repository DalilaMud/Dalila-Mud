#1201
quarzo1~
1 g 100
~
* Lista comandi
wait 6
osend %actor.name% Tutti attorno a te si riparano gli occhi, come abbagliati, ma tu raccogli la reliquia senza esitazioni, non del tutto conscio di cio' che stai facendo.
wait 1
oechoaround %actor.name% Una luce abbagliante permane tutto quanto quando %actor.name% raccoglie la sacra reliquia!
~
#1202
quarzo2~
1 j 100
~
* Lista comandi
oload mob 1285
wait 5
oechoaround %actor.name% %actor.name% inizia a sbiadire, una strana aura azzura permane la sua figura, poi, d'un tratto, scompare nel nulla!
osend %actor.name% SENTI IL POTERE DIVINO INVADERE IL TUO CORPO
oteleport %actor.name% 22
wait 6
osend %actor.name% Tutto intorno a te diventa scuro....sempre piu' scuro....finche' non vedi una luce avvicinarsi sempre di piu'....
wait 10
osend %actor.name% ....di piu'.....di piu'.....fino a diventare insopportabile!
wait 10
osend %actor.name% Sei ora nella LUCE: nulla ti ha mai preparato per quello che hai di fronte, eppure, per qualche strana forza arcana, sai chiaramente dove ti trovi e quale immenso potere ti e' stato donato
~
#1203
nebbie2~
0 e 1
smette~
* Lista comandi
if %object.vnum% == 1285
wait 3
mteleport %actor.name% %stanza%
mgoto %stanza%
mforce %actor.name% look
wait 2
msend %actor.name% Come in un sogno, ti ritrovi dov'eri prima di indossare la reliquia sacra
mgoto 2
mpurge
end
~
#1204
nebbie3~
0 e 0
ti guarda.~
* Lista comandi
if %mostrami% != 0
mgoto %mostrami%
mteleport %actor.name% %self.room%
mforce %actor.name% look
mgoto 22
mteleport %actor.name% %self.room%
set mostrami nullo
global mostrami
end
~
#1205
nebbia1~
0 ab 100
~
* Lista comandi
if %stanza% == 0
set stanza %self.room%
global stanza
wait 4
mgoto 22
end
~
#1206
nebbie4~
0 d 100
prova rune mostrami~
* Lista comandi
wait 2
if %speech% == evoca rune
get runa in cont1
get runa in cont2
get runa in cont3
get runa in cont4
get runa in cont5
get runa in cont6
wait 50
drop all
mteleport %actor.name% %self.room%
if %self.room% != 22
mforce %actor.name% look
wait 9
emote si dirada, mostrandoti nuovi orizzonti
mgoto 2
else
emote fa ricadere le rune a terra... non succede piu' nulla
end
end
if %speech% /= mostrami
set mostrami nullo
set %speech%
global mostrami
if %mostrami% == nullo
wait 5
emote iniziano a vorticare intorno a te, una paura irrefrenabile si impadrona di te!
wait 5
msend %actor.name% UN SIBILO INSOPPORTABILE PENETRA NELLA TUA MENTE, INIZI A URLARE MA NON RIESCI A PERCEPIRE LA TUA VOCE
wait 5
emote si squarcia sotto di te facendoti precipitare nel vuoto
wait 7
mforce %actor.name% d
mgoto 12512
mload obj 1285
mgoto 22
mgoto 2
else
wait 4
emote forma una strana immagine davanti a te...sta cercando di mostrarti %mostrami%
end
end
~
#1207
rune1~
0 e 1
prendi~
* Lista comandi
if %object.vnum% > 1519 & %object.vnum% < 1530
eval runa %runa% + 1
global runa
eval numero %object.vnum% - 1520
global numero
set %runa%a %numero%
global %runa%a
if %runa% > 5
eval totale %1a% * 100000 + %2a% * 10000 + %3a% * 1000 + %4a% * 100 + %5a% * 10 + %6a%
global totale
unset 6a
unset 5a
unset 4a
unset 3a
unset 2a
unset 1a
wait 12
emote alza le rune in aria facendole roteare in un vortice multicolore
wait 9
mgoto %totale%
unset totale
unset numero
unset runa
end
end
~
#1208
quarzo3~
1 h 100
~
* Lista comandi
wait 2
oload mob 1284
oforce %actor.name% scream
oechoaround %actor.name% %actor.name% CORRE IMPROVVISAMENTE VERSO LA RELIQUIA, RIPRENDENDONE POSSESSO
~
#1209
PRESA QUARZO~
0 e 0
grida forte!~
* Lista comandi
wait 1
mforce %actor.name% get reliquia
wait 1
mgoto 2
~
#1219
Dogane Trigger Regno (Kize)~
0 d 100
regno~
* Lista comandi
   if %actor.level% > 74 && %actor.vnum% == -1
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
       end
       global regno_id
     end
   end
~
#1220
PORTE DEIRA 1~
0 d 100
devo 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 setta~
* Lista comandi
if %actor.name% == %6% || %actor.name% == %7% || %actor.name% == %8% || %actor.name% == %9% || %actor.name% == %10%
set pass ok
end
if %actor.name% == %1% || %actor.name% == %2% || %actor.name% == %3% || %actor.name% == %4% || %actor.name% == %5%
set pass ok
end
if %actor.name% == %11% || %actor.name% == %12% || %actor.name% == %13% || %actor.name% == %14% || %actor.name% == %15%
set pass ok
end
if %actor.name% == %16% || %actor.name% == %17% || %actor.name% == %18% || %actor.name% == %19% || %actor.name% == %20%
set pass ok
end
if %speech% == devo entrare
if %pass% != ok
wait 5
emote controlla che il nome %actor.name% non compaia sulla lista dei ricercati
wait 8
emote prima di aprire il cancello esamina attentamente la zona circostante in cerca di gente nascosta
wait 10
mload obj 12565
hold chiave
unlock cancell u
open cancell up
mforce %actor% up
close cancell up
lock cancell up
mat 2 drop chiave
elseif %pass% == ok
wait 3
say &1&xALLARME!!!&0
mkill %actor.name%
end
end
if %actor.clan% == %regno_id% && %actor.rank% > 15
if %speech% == setta criminale
wait 5
say Bene...
set setta on
global setta
halt
end
if %setta% == on
if %speech% == 1
set %speech%
global 1
elseif %speech% == 2
set %speech%
global 2
elseif %speech% == 3
set %speech%
global 3
elseif %speech% == 4
set %speech%
global 4
elseif %speech% == 5
set %speech%
global 5
elseif %speech% == 6
set %speech%
global 6
elseif %speech% == 7
set %speech%
global 7
elseif %speech% == 8
set %speech%
global 8
elseif %speech% == 9
set %speech%
global 9
elseif %speech% == 10
set %speech%
global 10
elseif %speech% == 11
set %speech%
global 11
elseif %speech% == 12
set %speech%
global 12
elseif %speech% == 13
set %speech%
global 13
elseif %speech% == 14
set %speech%
global 14
elseif %speech% == 15
set %speech%
global 15
elseif %speech% == 16
set %speech%
global 16
elseif %speech% == 17
set %speech%
global 17
elseif %speech% == 18
set %speech%
global 18
elseif %speech% == 19
set %speech%
global 19
elseif %speech% == 20
set %speech%
global 20
else
end
wait 5
say Il nome e' stato aggiornato
unset setta
end
end
~
#1221
PORTE DEIRA 2~
0 e 0
ti guarda~
* Lista comandi
mecho ELENCO RICERCATI:
mecho 1- %1%
mecho 2- %2%
mecho 3- %3%
mecho 4- %4%
mecho 5- %5%
mecho 6- %6%
mecho 7- %7%
mecho 8- %8%
mecho 9- %9%
mecho 10- %10%
mecho 11- %11%
mecho 12- %12%
mecho 13- %13%
mecho 14- %14%
mecho 15- %15%
mecho 16- %16%
mecho 17- %17%
mecho 18- %18%
mecho 19- %19%
mecho 20- %20%
~
#1222
RICARICA DOGANA~
0 d 1
ricarica~
* Lista comandi
If %speech% == ricarica && %actor.rank% > 15 && %actor.clan% == 5
wait 5
set stanza %self.room%
n
if %stanza% == %self.room%
s
if %stanza% == %self.room%
e
if %stanza% == %self.room%
w
else
mload mob 12567
halt
end
else
mload mob 12567
halt
end
else
mload mob 12567
halt
end
mload mob 12567
end
~
#1223
Guardia palazzo Aldariano (Nimrhuin)~
0 d 100
devo entrare~
* Lista comandi
if %speech% == devo entrare
if %actor.clan% == 5 && %actor.rank% > 9
    wait 2 s
    say Salve a voi.
    wait 1 s
    mload obj 12565
    unlock cancellata up
    open cancellata up
    wait 1 s
    mforce %actor.name% u
    close cancellata up
    lock cancellata up
    mpurge chiave
  else
    wait 2 s
    say Mi dispiace, non vi e' permesso l'accesso al palazzo.
  end
end
~
#1224
Prova (Nimrhuin)~
0 g 100
~
* Lista comandi
say %actor.clan%
~
#1230
tapiro (offar)~
1 gj 100
~
* Lista comandi
oforce %actor% ooc &x&3Sono IO il vero e unico TAPIRONE!!&0
~
#1241
Elmina~
0 fk 75
~
* Lista comandi
wait 1 s
msend %actor% Il corpo di Emina si solleva in aria .... volteggiando vorticosamente
wait 1 s
msend %actor% un forte fento gelido irrompe nalla stanza.... e una voce profonda:
wait 1 s
msend %actor% Chi osa attaccare ELMINA? .....PAGHERA' CON LA PROPRIA MORTE
mload mob 1242
mload mob 1242
msend %actor% I cavalieri di Xaran appaiono dal nulla e ti attaccano......
~
#1250
ziraxn2~
0 d 100
primo secondo~
* Lista comandi
wait 300
say FATE QUALCOSA!
wait 300
cast 'peace' kharas
cast 'peace' giltha
nani elfi
wait 120
say Questo e' troppo, tornatevene ai vostri eserciti, pazzi, io me ne torno nel mio rifugio di pace, fuori da questo mondo in collera con se stesso.;
wait 80
emote si concentra profondamente, fino a lasciare sgombere le menti di tutti presenti da ogni pensiero.
wait 50
say La seduta e' sciolta.
~
#1251
welmar~
0 d 100
primo~
* Lista comandi
wait 40
say NON POTETE DARE RAGIONE A QUEI BIFOLCHI! Siete banditi a vita dal mio regno, stupidi stolti, metre voi....stupidi nani, se attaccherete Lamorka troverete un'accoglienza coi fiocchi! ahahah
~
#1252
kharas~
0 d 100
primo~
* Lista comandi
wait 120
say Non vedo l'ora di sterminare il....glorioso esercito Bretone....sono sicuro che avro' ...l'onore....di ucciderti di persona, mi carissimo Welmar!
~
#1253
giltha2~
0 d 100
primo~
* Lista comandi
wait 200
say non ti preoccupare mi caro Nano, non ti daro' il tempo di uccidere il Reietto....! Il mio esercito entrera' in Lamorka, e TU non me lo impedirai di certo!
wait 40
kill kharas
~
#1254
ziraxn3...~
0 d 100
terzo~
* Lista comandi
wait 200
say FATE QUALCOSA!
wait 250
cast 'peace' welmar
cast 'peace' kharas
wait 120
say Questo e' troppo, tornatevene ai vostri eserciti, pazzi, io me ne torno nel mio rifugio di pace, fuori da questo mondo in collera con se stesso.
wait 80
emote si concentra profondamente, fino a lasciare sgombere le menti di tutti presenti da ogni pensiero.
wait 60
say la seduta e' sciolta.
~
#1255
welmar...~
0 d 100
secondo~
* Lista comandi
wait 40
say NON POTETE DARE RAGIONE A QUEI BIFOLCHI! Siete banditi a vita dal mio regno, stolti, metre voi....stupidi elfi, se attaccherete Lamorka troverete un'accoglienza coi fiocchi! ahahah
wait 80
emote guarda strto Kharas
~
#1256
kharas~
0 d 100
secondo~
* Lista comandi
wait 120
say Stavolta sono d'accordo con te Welmar.....veramente un'accoglienza coi fiocchi.....
wait 10
grin
~
#1257
giltha...~
0 d 100
secondo~
* Lista comandi
wait 200
say ah si? MUORI, NANO ARGHIKLAXIN! (intraducibile anche via mente....)
wait 40
kill kharas
~
#1258
welmar~
0 d 100
terzo~
* Lista comandi
wait 30
wait 140
say  perche' aspettare allora! MUORI!
kill kharas
~
#1259
kharas...~
0 d 100
terzo~
* Lista comandi
wait 40
say Brutti sciocchi! Noi attaccheremo Lamorka e stermineremo anche te, Welmar! Preparati a combatteretra meno di un mese!;
~
#1260
gilthaetc.~
0 d 100
terzo~
* Lista comandi
wait 80
emote pensa e ci saremo pure noi....
wait 10
grin
~
#1270
kharas~
0 e 1
go~
* Lista comandi
wait 10
wait 50
holl CHE LA RIUNIONE ABBIA INIZIO!
say seguitemi
wait 20
d
~
#1271
gilta~
0 e 1
go~
* Lista comandi
wait 10
wait 30
wait 10
wait 30
say seguitemi
wait 30
u
~
#1272
welmar~
0 e 1
go~
* Lista comandi
HOLL ARRIVO ANCHE IO, NON TEMETE, STOLTI
wait 50
say seguitemi
wait 20
w
wait 70
~
#1273
scorte~
0 d 1
seguitemi~
* Lista comandi
wait 1
say Si, Signore!
foll kharas
foll gil
foll welmar
~
#1274
zirax5~
0 d 100
parti~
* Lista comandi
wait 30
nod
wait 30
u
wait 20
d
wait 20
d
wait 20
u
wait 20
e
wait 20
w
te welmar go
te kharas go
te gilt go
wait 50
say Venite a me, o Prodigiosi cavalieri.
wait 60
emote si concentra profondamente
wait 30
emote penetra nelle menti di tutti i presenti nella sala, mettendole misticamente in unione 
wait 30
say ora vi capirete in qualunque lingua parliate
wait 40
smile
wait 40
say A voi, miei nobili eroi, l'onore di scegliere a quale dei tre Comandanti dare la parola
wait 20
sit
~
#1275
welmar3~
0 d 100
kharas gilthanas welmar~
* Lista comandi
wait 120
say Adesso, caro comandante, vorrei proprio sapere chi avete intenzione di attaccare ?
wait 180
say AH, e cosi hai intenzione di attaccare Lamorka miserabile.....come se non lo sapessi, l'alleanza che ci lega con i Lamorkiani vige ormai da piu di due secoli !
wait 120
say COSA ? ? ? quello che state dicendo non ha senso ! A che scopo attacchereste una citta commerciale situata dall'altra parte delle terre conosciute, rispetto alla nostra terra elfica.
wait 460
say Molto francamente, mio nobile Zirax, credo che nessuno di noi sappia di che cosa state parlando....
wait 200
say SI !
~
#1276
kharas~
0 d 100
kharas gilthanas welmar~
* Lista comandi
wait 20
Say E invece prendo io la parola, sono sicuramente il piu' anziano nella stanza, a parte te, Zirax...
wait 20
say Le intenzioni del mio popolo sono ben precise, siamo piu che decisi ad attuare una politica espansionista nei confronti dei nostri piu diretti avversari politici ed economoci.
wait 200
say Credo che questi argomenti vadano trattati unicamente nella mia sala della Guerra, e non certo con un gruppetto di luridi Umani ed Elfi, che per altro non capisco proprio che cosa potrebbero avere in contrario contro un nostro attacco a 
say Lamorka
wait 360
Tutto cio e inaccettabile, noi attaccheremo Lamorka, e se voi Elfi sarete sulla strada,  faremo vedove le vostre mogli!
wait 20
grin
wait 460
say SI!
~
#1277
giltha~
0 d 100
kharas gilthanas welmar~
* Lista comandi
wait 150
say gia'...
wait 10
emote guarda male Welmar
wait 200
say Scusate, ma credo che mi debba proprio intromettere nel discorso, credo che sia tenuto a dirvi che il mio nobile popolo attacchera tra meno di una settimana la citta di cui state parlando !
wait 160
say Credo che questo invece non vi riguardi affatto, misero REietto che non siete altro.......il nostro popolo prendera possesso di Lamorka, con la diplomazia, ma se questo metodo dovesse fallire, non esiteremo a sterminare l'esercito.
say Ci serve un avamposto nell'est  per poter circondare Deira e l'esercito di questi stolti Nani in un colpo solo.
wait 560
say SI !
~
#1278
ziraxn~
0 d 100
kharas gilthanas welmar~
* Lista comandi
wait 660
emote arrossisce fino a diventare di un pallore preoccupante
wait 20
say ADESSO BASTAAAA!
wait 100
say Siete stati riuniti qua per discutere, non per dichiararvi guerra, Sappiamo tutti benissimo che il vero nemico da combattere in questo momento e' il male che sta mettendovi uno contro l'altro !
say Comportandovi cosi poco saggiamente state solo facendo il SUO gioco !
wait 160
say E molto semplice, nobile Re, questi vostri istinti assassini, dopo secoli di pace e di tranquillita, e ovvio che non possono essere nati dal nulla,
say  TUTTO quello che fate, pensate, programmate in questo momento e plagiato dalla terribile influenza maligna che sovrasta il nostro mondo, per la perdita dell'Arpa di cui voi tutti sapete.
wait 80
say VOI tutti siete qui perche' questo coraggioso manipolo di eroi e' stato in grado di riunirvi col mio aiuto, e ora, invece di scannarvi, dovreste cercare di trovare un misero accordo, almeno per meditare sulle mie parole.
say  Tutto quello che vi chiedo e un armistizio, chiedo troppo ?
wait 180
say Fermatevi ! Sentiamo almeno il parere dei nostri eroi qui presenti, come rappresentanti del popolo lucertola; per voi, quale esercito ha ragione?
wait 50
emote indica il primo esericito dei nani, poi il secondo degli elfi, e infine il terzo dei bretoni.
~
#1280
capi~
0 d 1
mediare mediazione allearsi~
* Lista comandi
wait 20
say Mai! Non mi abbassero' mai a parlare una lingua diversa da quella del mio popolo con quei pezzenti dehli altri Comandanti!
wait 40
say Il nostro e' un popolo nobile e antico, non possiamo certo permetterci di abbassarci....
wait 15
say ....al livello di questi altri popoli meschini e inetti.
wait 50
say Puah, l'unico modo in cui potremmo parlare e' attraverso un mediatore
emote si fa pensoso...
wait 40
say un mediatore che conosca tutte e tre le lingue di noi comandanti....
wait 40
emote chiama un uomo della sua scorta per chiedergli un'informazione
wait 30
smile
wait 25
say A questo punto vi prego di non disturbarmi piu' con sta storia delle trattative,etc...
wiat 30
wait 30
say verro' a patti solo se mi porterete codesto mediatore, e con questo ritengo chiuso il discorso.
wait 40
say puah, provate a chiedere a qualche vecchio re se ne sa qualche cosa...
wait 50
say magari persino colui che vi ha mandato qui vi potra' dire di piu' su questo Mediatore
wait 30
wave
say tornate solo quando lo avrete trovato, addio....
~
#1281
jupiter~
0 d 1
mediatore esper~
* Lista comandi
wait 20
say E va bene, credo sia meglio per il mondo che l'ultimo esper venga allo scoperto
wait 40
say un certo Zortax, Zertar, Zoard, Zirax, Zinizof, Zartaz....iniziava per Z....
wait 30
Egli e' l'unica persona ancora in vita a conoscere gli ormai persi segreti della magia dell'ESP
wait 30
say Quell'uomo possiede un gran dono, e' in grado di mettere in contatto le menti delle persone
wait 30
say e' l'unico che possa aiutarvi, ma non si sa piu' dove viva ormai, se e' ancora vivo....
wait 50
say anzi, ora che ci penso bene, avevo sentito di strane forze agenti in un posto sconsacrato da tempo, e ora pervaso da forti forze maligne
wait 30
say puo' darsi che si sia rifugiato li', chi lo andrebbe mai a cercare laggiu'....solo dei pazzi...o degli Eroi
~
#1282
ReGashna~
0 d 1
mediatore~
* Lista comandi
wait 30
say dovete assolutamente trovare questo mediatore...
wait 30
shake
wait 24
say ho paura che i comandanti, con questo discorso, ritengano di aver chiuso il discorso patteggiamenti
wait 30
say tutti sanno che gli Esper sono ormai una categoria estinta da mille anni
wait 25
say mmmm, forse solo un Dio puo' sapere qualche cosa su questo fantomatico Mediatore
wait 40
say la nostra unica speranza e' che uno di loro sia sopravvissuto al massacro di un millennio fa
wait 30
say volete sapere la storia miei giovani? potrebbe aiutarvi nel vostro arduo cammino
wait 25
~
#1283
regashna2~
0 d 1
dgf~
* Lista comandi
wait 30
smile
wait 40
emote si siede comodo
wait 30
say Dovete sapere, miei cari ragazzi, che piu' di mille anni fa esistevano ben 40 grandi scuole di magia,
wait 40
say raggruppate sotto il segno di 8 oracoli differenti.
wait 20
shake
wait 20
say Eh....Quelli si che erano bei tempi, la magia non era solo intesa come pratica bellica,
wait 30
say ma era invece considerata parte integrante della vita di ogni uomo, un dono Divino
wait 20
say da sfruttare nel migliore dei modi; La magia in se' racchiudeva significati di poco lontani da quelli religiosi.
wait 45
say Il potere degli 8 oracoli, col passare del tempo, stava pero' diventando sproporzionato e corrotto,
wait 30
say pochi erano ancora quelli che credevano negli ideali originali, e la Magia veniva 
wait 20
say sempre piu' intesa come fonte di potere e dominio.
wait 40
say Fu allora che molti Re, spinti spesso da non nobili ideali, decisero di allearsi
wait 30
say per rimuovere questa profonda minaccia una volta per tutte.
emote sospira forte
wait 40
say Essi pero' non erano certo degli stolti, decisero cosi' di mettere gli Oracoli uno contro l'altro
wait 30 e ci riuscirono....
wait 60
say e ci riuscirono....
wait 40
say Ma la vera causa del disastro, fu quella di scegliere il giorno che avrebbe dovuto consacrare il mondo proprio agli Oracoli, come giorno della loro fine...
wait 60
say Gli 8 oracoli erano riuniti nell'antica torre della magia, allo scopo di creare 8 talismani, contenenti le 8essenze del loro potere
wait 40
say da tempo giravano ormai voci su alcune rivalita' interne agli Oracoli, e attraverso questo profondo gesto di unione, essi speravano di riunire il loro rapporto
wait 30
say ma si sbagliavano
wait 60
say Gli 8 talismani vennero creati, e si unirono in un lampo di luce, per formare l'essenza del potere UNIVERSALE della magia terrestre
wait 30
say Neanche loro potevano concepire la potenza di CIO' che avevano creato, e di sicuro
wait 30
say non si aspettavano quello che sarebbe venuto dopo, e cio' che l'entita' avrebbe poi causato
wait 40
say il pomeriggio stesso della creazione del prezioso Artefatto, gli Oracoli iniziarono a disgregarsi
wait 30
say La debolezza momentanea del loro potere, dovuta alla creazione dei talismani,
wait 30
say favori' l'attacco mentale dei Re ribelli, che dando sfogo alle varie voci,
wait 40
say riuscirono a convincere i membri dei 7 oracoli che gli ESPER, l'8avo oracolo, stava per impossessarsi dell'artefatto
wait 40
say accecati da una ira cieca e senza senso, gli oracoli iniziarono allora una lotta fratricida
wait 30
say Gli Esper, per dimostrare la loro innocenza, non si difesero, e poco avrebbero comunque potuto fare,
wait 40
say essendo la loro essenza buona e pacifica, volta solo a far comunicare le menti delle persone, ma con pericolose tendenze al plagio, come insinuavano i Re nemici....
wait 40
say e fu cosi' che nella mezzanotte del secondo Giorno di primavera, le forze degli 8oracoli interagirono,
wait 30
say UNA CONTRO L'ALTRA, destabilizzando cosi' l'artefatto, cosi' vicino e giovane nella sua creazione....
wait 40
say come impazzito, questo scappo' dal rifugio magico in cui era stato rinchiuso,
wait 40
say ma solo per dividersi negli 8 talismani che lo formavano
wait 40
say questi pero', non si fermarono, ma partirono verso gli otto angoli piu' remoti del pianeta
wait 40
say mentre la massa dell'artefatto, ormai vuota ed instabile, ESPLOSE
wait 40
say travolgendo foreste, citta', montagne intere, sconvolgendo la Geografia di intere terre.
wait 100
say Molti morirono in quella notte, pochi sopravvissero a lungo ai terrori che seguirono
wait 50
emote rabbrividisce
wait 30
wait 50
say E gli 8 sacri ordini che tanto avevano fatto per il mondo, svanirono nel nulla, forse distrutti da quella immensa forza da loro stessi creati,
wait 80
wait 150
say i pochi adepti rimasti, fondarono i 5ordini sacri di cui voi fate parte
wait 80
say destino della sorte pero', gli Dei avevano risparmiato proprio loro, gli Esper
wait 50
say gli unici veri innocenti,  ma questo il popolo non poteva capirlo, e dopo 2secoli di persecuzione,
wait 100
say vennero tutti lapidati o bruciati vivi, o cosi' pare...
wait 200
say Fine.
5
si
~
#1284
zirax~
0 d 1
mediatore~
* Lista comandi
wait 30
say Finalmente mi avete trovato miei prodigiosi cavalieri.
wait 40
say e' da tempo che spiavo le vostre menti determinate, vi stavo aspettando.
wait 30
say ma sia ben chiara una cosa, io non ho assolutamente piu' niente da condividere con questa gente
wait 40
say Quasi nessuno mi ha aiutato da 1000 anni a questa parte, e poi spuntate qua voi per portarmi a "mediare" per salvare migliaia di misere vite....bleah
wait 50
say vi dovrei paralizzare, ma purtroppo non e' nella mia natura
wait 40
say ci vorrebbe veramente un VALIDO motivo perche' io vi segua dove mi volete portare
wait 50
say vediamo se ne trovate uno che mi convinca, puah, forse solo le lucertole dovrebbero sopravvivere in questo misero mondo
~
#1285
zirax2~
0 d 1
Kajashan~
* Lista comandi
wait 30
say Ho sentito Bene? Quel simpatico vecchio e' ancora vivo??? e per voi rischia di morire per questa stupida guerra?
wait 40
say mmmm, ok, mi avete convinto, ma voglio avere una conferma delle vostre parole, portatemi un suo stendardo e vi seguiro' in capo al mondo!
wait 5
wait 50
wave
~
#1286
ReGashna3~
0 d 1
stendardo~
* Lista comandi
Nothing.
wait 30
say scusate, ma perche' Zirax vuole codesto Stendardo?
wait 30
~
#1287
Regashna4~
0 d 100
conferma~
* Lista comandi
wait 30
drop stendardo
wait 20
say e va bene miei fedeli, vi nomino Ambasciatori di Gashna, andate e fatevi rispettare
~
#1288
zirax3~
0 j 100
~
* Lista comandi
wait 30
inv
~
#1289
zirax4~
0 e 100
Gashna~
* Lista comandi
wait 30
smile
wait 30
emote si sgrossa dalla polvere.
wait 24
say e allora che aspettiamo :)
wait 40
say portatemi nel luogo in cui si dovranno incontrare i comandanti
wait 40
say e, solo a quel punto, ditemi di partire.
wait 40
foll mister
foll jonny
foll rabailak
foll sparta
foll shaolin
foll martel
foll faretto
foll imman
~
#1290
goblin~
0 d 100
andiamo~
* Lista comandi
if %actor.vnum% == 1293
foll comandante
say Si, Signore!!
elseif %actor.vnum% == 1294
follo generale
say Si, Signore!!
else
end
~
#1291
comandante~
0 d 100
raccolta~
* Lista comandi
wait 20
say Si, mio Signore.
wait 20
bow
wait 20
foll cavaliere
wait 30
say presto, follatemi miei soldati!
~
#1292
cavaliere~
0 d 100
strage~
* Lista comandi
wait 20
grin
wait 10
foll death
wait 30
say pronti al massacro
wait 20
say a raccolta soldati!
~
#1293
tagliagole~
0 d 100
foll~
* Lista comandi
wait 1 s
if %actor.vnum% == 1294
follow assassino
else
end
~
#1294
tagliagole2~
0 d 100
vai~
* Lista comandi
wait 1
if %actor.vnum% == 1294
assist %actor.name%
else
end
~
#1295
fantasma~
0 d 100
strage~
* Lista comandi
wait 20
say Si, mio Supremo padrone.
wait 10
foll death
wait 20
bow
wait 30
say A raccolta miei fedeli!
wait 10
grin
~
#1296
fuga~
0 d 100
flee~
* Lista comandi
wait 10
flee
flee
~
#1299
PURGATORE~
1 j 100
~
* Lista comandi
wait 1 s
opurge all
opurge
end
~
$~
