#9700
Purga uscita castello~
2 f 100
~
%door% 9703 east purge
%door% 2414852 west purge
~
#9701
Uscita dal castello~
2 d 0
devo uscire~
%echo% &ZTi senti scrutato per un momento&0
wait 1s
if %actor.clan% == 3 && %actor.rank% == 60
%echo% &eQualcuno dice, 'La trasportiamo subito fuori sire.'&0
%teleport% %actor% 2414852
else
%echo% &eQualcuno dice, 'Dannazione! Come sei entrato qui, mh? Fila in cella!'&0
%send% %actor.name% &wUna botola si apre sotto i tuoi piedi.&0
%teleport% %actor% 8578
end
~
#9702
ingresso~
0 d 0
morte ai nemici di tolnedra~
if %actor.clan% == 3 && %actor.rank% == 60
bow %actor.name%
wait 1s
%teleport% %actor% 9703
else
scuoti
wait 1s
say &ZNon ci provare bello. Di qui passa solo chi e' autorizzato.&0
end
~
#9703
tempio 1~
2 g 100
~
if %direction% == north
set target %actor.name%
if %actor.name% /= guardia
else
if %target% != 0
wait 3
wload obj 12638
wforce %target% get santa
wforce %target% quaff santa
wait 2
wsend %target% Senti lo spirito di Fizban dentro di te mentre varchi l'entrata del tempio
wpurge
wforce %target% sit
wait 3
wsend %target% Improvvisamente la testa incomincia a girarti, ti sembra di non ricordare piu' dove ti trovi mentre ti perdi nella contemplazione degli affreschi del tempio
wteleport %target% 12515
wait 5
wforce %target% look
wait 30 s
wteleport %target% 12512
wsend %target% Ritorni nel tuo corpo mortale con tua grande delusione
end
end
end
~
#9704
ammazzapiccioni~
0 g 100
~
if %actor.name% == jimmylee || %actor.name% == raven || %actor.name% == arius || %actor.name% == benjamin || %actor.name% == alorie || %actor.name% == joram || %actor.name% == sasha
wait 2
say AIUTO! %ACTOR.NAME%!
wait 2
emote &7ti guarda con aria terrorizzata!&0
flee
flee
flee
end
~
#9705
ammazza2~
0 i 100
~
if %actor.name% == jimmylee || %actor.name% == raven || %actor.name% == arius || %actor.name% == benjamin || %actor.name% == alorie
wait 2
say AIUTO! %ACTOR.NAME%!
emote &7ti guarda con aria terrorizzata!&n
flee
flee
flee
nd
end
~
#9706
Ingresso castello 2~
0 e 100
scassinare~
 tell smigol Signore, %actor.name% sta cercando di infiltrarsi furtivamente nel palazzo di Tolnedra.
 tell sagat Signore, %actor.name% sta cercando di infiltrarsi furtivamente nel palazzo di Tolnedra.
 tell fasir Signore, %actor.name% sta cercando di infiltrarsi furtivamente nel palazzo di Tolnedra.
 tell thief Signore, %actor.name% sta cercando di infiltrarsi furtivamente nel palazzo di Tolnedra.
 dg_cast 'paralize' %actor.name%
~
#9710
Venditore di Manovaali (Jerold)~
0 m 100
50000~
unset manca
unset inpiu
set costo 100000
if %amount% < %costo%
wait 2 s
say &1&x Spiacente %actor.name%. Questi soldi non bastano. &0
wait 2 S
give %amount% coins %actor.name%
eval manca %costo%-%amount%
wait 2 s
say &2&x La tariffa attuale per un manovale e' di &1 %costo% &2 coin. &0
wait 2 s
say &2&x a Voi mancano &1 %manca% &2 coins. &0
elseif %amount% > %costo%
wait 2 s
emote ridacchia
wait 2
say &1&x Caro %actor.name% mi avete dato troppi soldi! &0
wait 2
give %amount% coins %actor.name%
eval inpiu %amount%-%costo%
wait 2 s
say &2&x La tariffa attuale per un manovale e' di &1 %costo% &2 coin. &0
wait 2 s
say &2&x Voi me ne avete dati &1 %inpiu% &2 in piu'&0
else
wait 2 s
Say &1&x Ottimo! &0
Regno deposita %costo% tolnedra
wait 2 s
emote apre una porta nascosta e fa uscire un manovale
mload mob 9716
wait 2 s
mforce manovale follow %actor.name%
mforce %actor.name% Say Padrone
mforce manovale Say &3&x Sono a vostra disposizione %actor.name% &0
wait 2 s
Say &2&x Ora il manovale e' al vostro servizio.  &0
wait 2 s
Say &2&x Per farlo lavorare bastera' impartire l'ordine &1 ORDINA MANOVALE &2 seguita dall'azione &0
wait 2 s
Say &2&x Buon lavoro! &0
wait 1 s
emote si fa una grossa risata
wait 1 s
Say &2&x Ci vediamo presto. &0
end
~
#9711
Settaggio Padrone del Manovale (Jerold)~
0 bd 1
Padrone~
if %speech% == Padrone
if %mob_lock% == on
if %actor.name% == %Padrone%
wait 1 s
Say &3&x Si %Padrone% sono ai vostri ordini &0
rem anello
junk anello
mload obj 9800
wear anello
follow %Padrone%
else
wait 1 s
Say &1&x Spiacente %actor.name% non sei tu il mio Padrone &0
end
else
eval random_emote %random.5%
global random_emote
if %random_emote% == 5
set tempo_emote 50
global tempo_emote
elseif %random_emote% == 4
set tempo_emote 45
global tempo_emote
elseif %random_emote% == 3
set tempo_emote 40
global tempo_emote
elseif %random_emote% == 2
set tempo_emote 35
global tempo_emote
else
set tempo_emote 30
global tempo_emote
end
mload obj 9800
wear anello
set Padrone %actor.name%
set mob_lock on
global Padrone
global mob_lock
Say &3&x Si %Padrone% sono ai vostri ordini &0
wait 1 s
follow %Padrone%
end
else
end
~
#9712
Settaggio con Charm Padrone del Manovale (Jerold)~
0 d 1
Padrone~
      rem anello
if %speech% == Padrone
   if %mob_lock% == on
      if %actor.name% == %Padrone%
        wait 1 s
        Say &3&x Si %Padrone% sono ai vostri ordini &0
        follow %Padrone%
wear anello
         else
  end
   else
   set Padrone %actor.name%
   set mob_lock on
   global Padrone
   global mob_lock
follow %Padrone%
   mload obj 9800
   wear anello
   end
else
end
~
#9713
Stipendio manovale (Jerold)~
0 bm 1
~
if %stipendio_arretrato% > 0
if %amount% < %stipendio_valore%
wait 2 s
emote non ha la faccia di uno contento.
wait 1 s
say &1&x Spiacente %actor.name%. Questi soldi non bastano. &0
wait 1 s
give %amount% coins %actor.name%
wait 1 s
say &1&x La paga sindacale attuale  di %stipendio_valore% coins. &0
elseif %amount% > %stipendio_valore%
wait 2 s
pa %actor.name%
say &1&x La vostra generosite' e' commovente, ma ho ricevuto piu' soldi del dovuto. &0
wait 1 s
give %amount% coins %actor.name%
wait 1 s
say &1&x La paga sindacale attuale  di %stipendio_valore% coins. &0
else
wait 1 s
say &1&x Grazie %actor.name%. &0
wait 1
eval stipendio_arretrato %stipendio_arretrato% - 1
global stipendio_arretrato
emote mette in un sacchetto le monete e il tutto lo ripone dentro i pantaloni.
end
else
wait 1 s
say &1&x Grazie ma al momento non ne ho bisogno &0
wait 1 s
give %amount% coins %actor.name%
end
~
#9714
Check stipendio manovali (Jerold)~
0 b 100
~
set stipendio_valore 2000
global stipendio_valore
eval check_paga %check_paga% + 1
global check_paga
if %check_paga% > 3600
set check_paga 0
global check_paga
eval stipendio_arretrato %stipendio_arretrato% + 1
global stipendio_arretrato
if %stipendio_arretrato% > 2
emote si autolicenzia visti i mancati pagamenti
mgoto 0
mpurge
else
end
else
end
eval attesa %attesa%+1
global attesa
eval recita %random.10%
global recita
eval recita1 %random.5%
global recita1
eval recita2 %random.3%
global recita2
eval recita3 %random.3%
global recita3
eval recita4 %random.3%
global recita4
eval recita5 %random.3%
global recita5
eval recita6 %random.3%
global recita6
eval recita7 %random.3%
global recita7
eval recita8 %random.3%
global recita8
eval recita9 %random.3%
global recita9
eval recita10 %random.3%
global recita10
if %attesa% > %tempo_emote%
set attesa 0
global attesa
if %recita%==10
emote estrare alcune monete dal suo borsello e inizia a contarle.
wait 2 s
if %recita1%==3
say &3 Oh porc... mi hanno rifilato un altra moneta falsa.&0
wait 2 s
emote ringhia furioso
elseif %recita1%==2
emote osserva con attenzione una moneta molto soddisfatto
wait 2 s
emote lustra sulla manica la moneta.
else
say &3 Dovro' passare in banca un giorno di questi &0
end
wait 2 s
emote rimette le monete nel borsello.
elseif %recita%==9
say &3Ah! che dura vita la nostra!&0
wait 2 s
if %recita2%==3
emote si siede un attimo e si massaggia i piedi gonfi
elseif %recita2%==2
emote ringhia furioso
else
emote sorride con malinconia
end
elseif %recita%==8
emote guarda il cielo pensieroso.
wait 2 s
if %recita3%==3
say &3 Mmm la nuvole non promettono nulla di buono&0
elseif %recita3%==2
emote respira profondamente.
else
say &3 Couf! Couf! mi sa che sto' per ammalarmi.&0
end
elseif %recita%==7
say &3Mi sa che quando stacco dal lavoro andro' a fare una bevutina alla Taverna...&0
wait 2 s
if %recita4%==3
say &3... anche se lo stregone dice che non fa molto bene!&0
elseif %recita4%==2
emote estrae un fiaschettino di vino e lo tracanna.
wait 1
emote rimette via il fiaschettino.
else
say &3 ...no per oggi e' meglio di no.&0
end
elseif %recita%==6
emote estrae un coltellino e inizia a pulirsi le unghie.
wait 2 s
if %recita5%==3
say &3Ecco ora sono pulite!&0
wait 2
emote si guarda le mani soddisfatto.
elseif %recita5%==2
emote si procura un piccolo tagliettino sotto le unghie.
else
say &3 Un giorno di questi dovro' anche tagliarle... ahah!&0
end
wait 1 s
emote mette via il coltellino.
elseif %recita%==5
say &3Oggi sono proprio felice&0
wait 2 s
if %recita6%==3
say &3Devo ricordarmi di fare un regalo alla mia bella.&0
elseif %recita6%==2
emote sorride.
wait 2 s
look %padrone%
else
emote si stiracchia
end
elseif %recita%==4
emote saltella qua e la in preda ad un raptus.
wait 2 s
if %recita7%==3
say &3 Ahh ... mi ha morso un insetto... che male! &0
elseif %recita7%==2
say &3 Ouch! Una storta alla caviglia. &0
else
wait 2 s
emote si calma.
end
elseif %recita%==3
emote fa pipi' in un angolo
wait 2 s
if %recita8%==3
say &3 Ehi non mi guardare cosi'...mi scappava! &0
elseif %recita8%==2
emote si guarda i pantaloni un po' macchiati.
else
emote diventa rosso in viso
end
elseif %recita%==2
say &3Lavoro, lavoro... sempre lavoro &0
way 2
if %recita9%==3
emote si mostra un pochino infastidito.
elseif %recita9%==2
say &3Pero' almeno sono sempre molto allenato &0
wait 1 s
emote mostra i muscoli
else
emote sbuffa.
end
else
emote si guarda attorno.
wait 2 s
if %recita10%==3
say &3 Eh! Credo di essermi perso... ma qui dove siamo? &0
wait 2 s
emote alza le spalle sconsolato.
elseif %recita10%==2
emote fissa il suo sguardo su un oggetto per terra.
else
say &3C'e' poca gente in giro oggi&0
end
end
end
~
#9715
Suicidio del manovale~
0 ab 100
~
eval karakiri %karakiri% + 1
global karakiri
if %karakiri% > 4
emote si autolicenzia visti i mancati pagamenti
mgoto 2
mpurge
end
~
#9716
Stato manovale (Jerold)~
0 be 0
ti guarda~
mload mob 12601
mecho vittima
mecho vittima Padrone:  %padrone%
if %stipendio_arretrato% > 0
if %stipendio_arretrato% == 1
mecho vittima Hai in arretrato da pagare %stipendio_arretrato% stipendo
mecho vittima ogni stipendio ammonta a %stipendio_valore% coins
else
mecho vittima Hai in arretrato da pagare %stipendio_arretrato% stipendi
mecho vittima ogni stipendio ammonta a %stipendio_valore% coins
end
else
mecho vittima nessuno stipendio arretrato
end
mforce vittima mgoto 2
mat 2 mpurge
~
#9717
Funzionario Reale Cimmura (Jerold)~
0 d 1
contratto comandi progetto notifica addestramento stipendio premio~
if %speech% == ferramenta
mload mob 12606
wait 3
say funzionario %actor.name%
wait 6
say segui %actor.name%
end
if %speech% == comandi
wait 5
say Eccoli:  comandi, progetto, notifica, contratto, addestramento, stipendio, premio 
elseif %speech% == progetto
nod
wait 5
mload obj 61
give progetto %actor.name%
wait 5
say ecco a voi un Progetto reale
elseif %speech% == contratto
nod
wait 5
mload obj 90
give progetto %actor.name%
wait 5
say eccovi un Contratto per vendere una abitazione
elseif %speech% == addestramento
wait 5
mload obj 12581
give adde %actor.name%
eval debito %debito% + 25000
global debito
elseif %speech% == premio
wait 5
mload obj 12580
eval debito %debito% + 10000
global debito
elseif %speech% == stipendio
mload obj 12582
eval debito %debito% + 5000
global debito
elseif %speech% == notifica
wait 1 s
mload obj 62
give notifica %actor.name%
elseif %speech% == buono
wait 2
nod
wait 5
mload obj 63
give buono %actor.name%
wait 1 s
say ecco a voi un Buono Reale
eval buoni %buoni% + 1
global buoni
elseif %speech% == credito
wait 1 s
if %buoni% == 0
say non vi sono buoni da riscattare dalle casse del regno, Lord %Actor.name%
else
say Sono stati ritirati %buoni% buoni da 10k
end
wait 5
say Mentre il debito da pagare causato dal mantenimento delle truppe e' di %debito%
end
if %speech% == azzera 
if %actor.name% == Feanor
nod
wait 5
emote strappa una pergamena
unset buoni
unset debito
else
say non puo' ordinarmi questo, sir %actor.name%
end
end
give all %actor.name%
~
#9718
Guardiano della Sala Donazioni di Tolnedra (Jerold)~
2 d 0
Sala Donazioni~
if %actor.clan% == 3 
wteleport %actor.name% 9833
wforce %actor.name% look
else
wecho &1&x La sala donazioni e' disponibile solo per il popolo di Tolnedra &0
end
~
#9719
Comportamento Manovali (Jerold)~
0 b 100
~
eval recita %random.4%
global recita
eval attesa %attesa%+1
global attesa
if %attesa% > 5
set attesa 0
global attesa
if %recita%==4
emote estrare alcune monete dal suo borsello e inizia a contarle.
else
end
if %recita%==3
emote fa il di piu'
else
end
if %recita%==2
say &3Ah! che dura vita la nostra!&x
else
emote si guarda le mani
end
end
~
#9720
Pausa sindacale Manovali (Jerold)~
0 e 0
ordina qualcosa a Un robusto manovale~
eval sciopero %random.100%
global sciopero
if %sciopero% > 90
rem anello
wait 1 s
eval recita11 %random.3%
global recita11
if %recita11%==3
say &1&x Mi Spiace ma sono un pochettino stanco e mi voglio riposare &0
elseif %recita11%==2
say &1&x Non ora... mi sento poco bene. &0
else
say &3&x Un attimo! ho un fitta allo sterno!&0
end
wear anello
else
end
~
#9721
Tic Cerca manovali per Check Suicidio (Jerold)~
0 ab 100
~
emote batte il piede impaziente
~
#9722
Morte del Cerca Manovali (Jerold)~
0 d 0
e' giunta la tua ora~
emote abbassa la testa
mgoto 36
sit
~
#9723
Venditore Manovale - Cerca manovali (Jerold)~
0 d 1
Perfetto Spiacente~
if %actor.name% == robustissimo
follow ricercatore
goto 9829
eval conta_manovali %conta_manovali%+1
global conta_manovali
mgoto %conta_manovali%.robustissimo
emote arriva tutto trafelato
follow manovale
wait 1 s
Say Titolare %Messer%
wait 1 s
Say rientrate
else
end
~
#9724
Cerca Manovali 1 (Jerold)~
0 d 1
messer~
if %actor.name% == L'addetto alla ricerca dei manovali
set ricerca on
global ricerca
set %speech%
global messer
wait 1 s
say &3&x D'accordo andro' a cercare tutti i manovali di %Messer% ... con permesso.&0
wait 1 s
emote esce frettolosamente dal negozio e sparisce tra le strade della citta'
else
end
goto 9829
eval conta_manovali %conta_manovali%+1
global conta_manovali
mgoto %conta_manovali%.robustissimo
emote arriva tutto trafelato
follow manovale
wait 1 s
Say Titolare %Messer%
wait 1 s
Say rientrate
else
end
~
#9725
Settaggio casa manovali (Jerold)~
0 d 1
casa cambiacasa rincasa manager~
if %actor.name% == %padrone% || %actor.name% == Sem Joy || %actor.level% > 70
if %speech% == casa
if %lock_casa% == on
wait 1 s
say &3&xMio signore, ho gia' un domicilio...&0
wait 1 s
say &3&x...ma se vuole che io cambi basta dirmi &1CAMBIACASA&3 nella nuova abitazione&0
else
set lock_casa on
global lock_casa
set casa_padrone %self.room%
global casa_padrone
wait 1 s
say &3&xBene mio signore! Grazie per l'ospitalita' &0
end
elseif %speech% == cambiacasa
unset casa_padrone
set casa_padrone %self.room%
global casa_padrone
wait 1 s
say &3&xBene mio signore! Da oggi questa sara' la mia nuova dimora.&0
elseif %speech% == rincasa
wait 1 s
say  Si mio signore' 
wait 1 s
emote si allontana frettolosamente
mgoto %casa_padrone%
rem anello
follow me
elseif %speech% == manager
'&1&xPadrone &3&B%padrone% &1&x e ho &1&B%stipendio_arretrato% &1&x stipendi arretrati&0
if %stipendio_arretrato% >2
mgoto 36
else end
else
end
else
wait 1 s
say &1&xSpiacente non sei il mio padrone per dirmi questo&0
end
~
#9726
Manovale Titolare (Jerold)~
0 d 1
Titolare~
set %speech%
global Titolare
~
#9727
Cerca manovali 2 (Jerold)~
0 ab 100
~
if %ricerca% == on
mgoto 9829
eval conta_manovali %conta_manovali%+1
global conta_manovali
mgoto %conta_manovali%.robustissimo
emote arriva tutto trafelato
wait 1 s
Say Titolare %Messer%
wait 2 s
Say rientrate
emote batte il piede impaziente
else
end
~
#9728
Manovale per cerca manovali~
0 d 1
rientrate~
if %actor.name% == Il ricercatore di manovali
if %Titolare% == %Padrone%
wait 2 s
say Certamente
wait 1 s
emote si allontana frettolosamente
mgoto %casa_padrone%
else
wait 2 s
say Non mi Interessa
end
else
end
~
#9729
Portale per l'arsenale di Aldaria (Kizeridis)~
0 d 100
Aldur~
if %speech% == Aldur
wait 2 s
wsend %actor.name% Senti una strana forza provenire dal nulla
wait 2 s
wteleport %actor.name% 2822
wforce %actor.name% look
else
end
~
#9730
Portale per l'arsenale di tolnedra (Nulla)~
2 d 100
nullarulez~
if %speech% == nullarulez
if %actor.clan% == 3 && %actor.rank% >= 56
wait 2 s
wsend %actor.name% Senti una strana forza provenire dal nulla
wait 2 s
wteleport %actor.name% 40206
wforce %actor.name% look
else
wait 2 s
Sembra che tu abbia attivato uno strano meccanismo appena pronunci la parola d'ordine...
wait 1 s
wecho &RIL PAVIMENTO SI APRE SOTTO I TUOI PIEDI&d
wteleport %actor.name% 8578
wforce %actor.name% look
end
end
~
#9731
Uccisore del Cerca Manovali (Jerold)~
0 e 0
batte il piede impaziente~
if %conta% > 4
mpurge cerca
set conta 0
global conta
else
eval conta %conta%+1
global conta
end
~
#9732
Test di allarme~
0 h 100
~
'Salute a voi
~
#9735
Prigioni 1 - Generale (Jerold)~
2 ab 100
~
if %unlock% < 3
eval unlock %unlock%+1
global unlock
else
set unlock 0
global unlock
set lock OFF
global lock
end
if %animazione% == on
eval timer %timer% +1
global timer
if %timer% >= 2
wecho &1&x Un enorme guardiano esce dalla cella sbuffando&0
set animazione off
global animazione
unset %libera%
else
end
else
end
~
#9736
Prigioni 2 - Tolnedra (Jerold)~
2 d 1
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
wdoor 9836 north flags a
wait 10 s
wdoor 9836 north flags bcd
else
wecho %actor.name% %recita2a%
end
else
eval percentuale_fuga1 %random.2000%
global percentuale_fuga1
if %percentuale_fuga1% > 1998
wecho %actor.name% %recita3a%
wdoor 9836 north flags a
wait 10 s
wdoor 9836 north flags bcd
else
wecho %actor.name% %recita2a%
end
end
else
wecho %actor.name% %recita4a%
end
~
#9737
Prigioni 3 - Tolnedra (Jerold)~
2 d 0
Devo uscire~
if %speech% == devo uscire
set a %actor.name%
if %a% == talos || %a% == Jerold || %a% == roscellino || %a% == fender || %a% == feanor || %a% == spartakus || %a% == Thief || %a% == Kitiara || %a% == Sarib || %a% == Keli || %a% == Baal || %a% == Melyanna || %a% == Tajmaku
wait 2 s
wsend %actor.name% Ad un tuo cenno una guardia ti fa uscire
wait 2 s
wteleport %actor.name% 9835
wforce %actor.name% look
else
end
else
end
~
#9738
Prigione 4 - Tolnedra (Jerold)~
2 d 1
libera~
set a %actor.name%
if %a% == talos || %a% == Jerold || %a% == roscellino || %a% == fender || %a% == feanor || %a% == spartakus || %a% == Thief || %a% == Kitiara || %a% == Sarib || %a% == Keli || %a% == Baal || %a% == Melyanna || %a% == Tajmaku
set %speech%
global libera
set animazione on
global animazione
set timer 0
global timer
wecho  Un enorme guardiano entra nella cella 
wait 1 s
wecho Un enorme guardiano dice, 'Il prigioniero da liberare mi dica &1&xEccomi&0
else
end
~
#9739
Prigioni 5 - Tolnedra (Jerold)~
2 d 0
devo entrare~
if %speech% == devo entrare
if %actor.name% == talos || %actor.name% == sadric || %actor.name% == roscellino || %actor.name% == fender || %actor.name% == feanor || %actor.name% == spartakus || %actor.name% == Thief || %actor.name% == Kitiara || %actor.name% == Sarib
wait 2 s
wsend %actor.name% Ad un tuo cenno una guardia ti fa entrare
wait 2 s
wteleport %actor.name% 9836
wforce %actor.name% look
else
end
else
end
~
#9740
Prigioni Tolnedra - 6 (Jerold)~
2 d 1
Eccomi~
if %animazione% == on
if %actor.name% == %libera%
wecho Perfetto ora vi trasferisco fuori
wait 2 s
wsend %actor.name% Il guardiano ti fa uscire dalla cella... in malo modo.
wait 1 s
wteleport %actor.name% 9835
set animazione off
global animazione
unset %libera%
else
wecho non ci provare
end
else
end
~
#9741
Prigioni Generale - 6 (Jerold)~
2 ab 100
~
if %animazione% == on
eval timer %timer% +1
global timer
if %timer% >= 2
wecho &1&x Un enorme guardiano esce dalla cella sbuffando&0
set animazione off
global animazione
unset %libera%
else
end
else
end
~
#9745
entrata cittadini cimmura porta est~
0 d 100
*~
if %actor.clan% == 3
if %speech% == fatemi entrare
if %actor.rank% < 50
say Passate pure, Concittadino!
else
emo scatta sull'attenti.
say Sissignore!
end
%teleport% %actor% 8500
end
if %speech% == fatemi uscire
if %actor.rank% < 50
say Passate pure, Concittadino!
else
emo scatta sull'attenti.
say Sissignore!
end
%teleport% %actor% 2404858
end
end
~
#9746
Blocco per no regno~
0 q 100
~
if %citta_chiusa% == 1
if %direction% == west
if %actor.clan% != 3
say Spiacente amico, tu non puoi passare
return 0
end
end
end
~
#9747
Setta citta' chiusa o aperta~
0 c 0
cimmura~
if %actor.clan% == 3 && %actor.rank% >= 50
if %arg% == chiudi
say Si Signore, di qui passano solo Cittadini Tolnar
eval citta_chiusa 1
global citta_chiusa
wait 1s
%load% obj 8500
hold chiave
chiudi cancello
chiudi portone
lock cancello
lock portone
rimuo chiav
emote ripone la chiave al sicuro
mat xxxxx drop chiave
end
if %arg% == apri
say Si Signore, faccio passare chiunque
eval citta_chiusa 0
global citta_chiusa
wait 1 s
%load% obj 8500
hold chiave
unlock portone
unlock cancello
open portone
open cancello
rimuo chiav
emote ripone la chiave al sicuro
mat xxxxx drop chiave
end
end
~
#9748
entrata cittadini cimmura porta nord~
0 d 100
*~
if %actor.clan% == 3
if %speech% == fatemi entrare
if %actor.rank% < 50
say Passate pure, Concittadino!
else
emo scatta sull'attenti.
say Sissignore!
end
%teleport% %actor% 8512
end
if %speech% == fatemi uscire
if %actor.rank% < 50
say Passate pure, Concittadino!
else
emo scatta sull'attenti.
say Sissignore!
end
%teleport% %actor% 2404554
end
end
~
#9749
entrata cittadini cimmura porta ovest~
0 d 100
*~
if %actor.clan% == 3
if %speech% == fatemi entrare
if %actor.rank% < 50
say Passate pure, Concittadino!
else
emo scatta sull'attenti.
say Sissignore!
end
%teleport% %actor% 8522
end
if %speech% == fatemi uscire
if %actor.rank% < 50
say Passate pure, Concittadino!
else
emo scatta sull'attenti.
say Sissignore!
end
%teleport% %actor% 2404850
end
end
~
#9750
Nuovi Manovali 1 (Jerold)~
0 ab 100
~
eval check_paga %check_paga% + 1
global check_paga
if %check_paga% > 3000
set check_paga 0
global check_paga
eval stipendio_arretrato %stipendio_arretrato% + 1
global stipendio_arretrato
if %stipendio_arretrato% > 2
emote si autolicenzia visti i mancati pagamenti
mgoto 36
else
end
else
end
eval attesa %attesa%+1
global attesa
eval recita %random.10%
global recita
eval recita1 %random.5%
global recita1
eval recita2 %random.3%
global recita2
eval recita3 %random.3%
global recita3
eval recita4 %random.3%
global recita4
eval recita5 %random.3%
global recita5
eval recita6 %random.3%
global recita6
eval recita7 %random.3%
global recita7
eval recita8 %random.3%
global recita8
eval recita9 %random.3%
global recita9
eval recita10 %random.3%
global recita10
if %attesa% > %tempo_emote%
set attesa 0
global attesa
if %recita%==10
emote estrare alcune monete dal suo borsello e inizia a contarle.
wait 2 s
if %recita1%==3
say &3 Oh porc... mi hanno rifilato un altra moneta falsa.&0
wait 2 s
emote ringhia furioso
elseif %recita1%==2
emote osserva con attenzione una moneta molto soddisfatto
wait 2 s
emote lustra sulla manica la moneta.
else
say &3 Dovro' passare in banca un giorno di questi &0
end
wait 2 s
emote rimette le monete nel borsello.
elseif %recita%==9
say &3Ah! che dura vita la nostra!&0
wait 2 s
if %recita2%==3
emote si siede un attimo e si massaggia i piedi gonfi
elseif %recita2%==2
emote ringhia furioso
else
emote sorride con malinconia
end
elseif %recita%==8
emote guarda il cielo pensieroso.
wait 2 s
if %recita3%==3
say &3 Mmm la nuvole non promettono nulla di buono&0
elseif %recita3%==2
emote respira profondamente.
else
say &3 Couf! Couf! mi sa che sto' per ammalarmi.&0
end
elseif %recita%==7
say &3Mi sa che quando stacco dal lavoro andro' a fare una bevutina alla Taverna...&0
wait 2 s
if %recita4%==3
say &3... anche se lo stregone dice che non fa molto bene!&0
elseif %recita4%==2
emote estrae un fiaschettino di vino e lo tracanna.
wait 1
emote rimette via il fiaschettino.
else
say &3 ...no per oggi e' meglio di no.&0
end
elseif %recita%==6
emote estrae un coltellino e inizia a pulirsi le unghie.
wait 2 s
if %recita5%==3
say &3Ecco ora sono pulite!&0
wait 2
emote si guarda le mani soddisfatto.
elseif %recita5%==2
emote si procura un piccolo tagliettino sotto le unghie.
else
say &3 Un giorno di questi dovro' anche tagliarle... ahah!&0
end
wait 1 s
emote mette via il coltellino.
elseif %recita%==5
say &3Oggi sono proprio felice&0
wait 2 s
if %recita6%==3
say &3Devo ricordarmi di fare un regalo alla mia bella.&0
elseif %recita6%==2
emote sorride.
wait 2 s
look %padrone%
else
emote si stiracchia
end
elseif %recita%==4
emote saltella qua e la in preda ad un raptus.
wait 2 s
if %recita7%==3
say &3 Ahh ... mi ha morso un insetto... che male! &0
elseif %recita7%==2
say &3 Ouch! Una storta alla caviglia. &0
else
wait 2 s
emote si calma.
end
elseif %recita%==3
emote fa pipi' in un angolo
wait 2 s
if %recita8%==3
say &3 Ehi non mi guardare cosi'...mi scappava! &0
elseif %recita8%==2
emote si guarda i pantaloni un po' macchiati.
else
emote diventa rosso in viso
end
elseif %recita%==2
say &3Lavoro, lavoro... sempre lavoro &0
way 2
if %recita9%==3
emote si mostra un pochino infastidito.
elseif %recita9%==2
say &3Pero' almeno sono sempre molto allenato &0
wait 1 s
emote mostra i muscoli
else
emote sbuffa.
end
else
emote si guarda attorno.
wait 2 s
if %recita10%==3
say &3 Eh! Credo di essermi perso... ma qui dove siamo? &0
wait 2 s
emote alza le spalle sconsolato.
elseif %recita10%==2
emote fissa il suo sguardo su un oggetto per terra.
else
say &3C'e' poca gente in giro oggi&0
end
end
end
~
#9751
Nuovi Manovali 2 (Jerold~
0 m 1
~
if %actor.name% == %Padrone%
if %stipendio_arretrato% > 0
if %amount% < %stipendio_valore%
wait 2 s
emote ti guarda storto.
wait 1 s
say &1&x Spiacente %actor.name%. Questi soldi non bastano. &0
wait 1 s
give %amount% coins %actor.name%
wait 1 s
say &1&x La paga sindacale attuale  di %stipendio_valore% coins. &0
elseif %amount% > %stipendio_valore%
wait 2 s
pa %actor.name%
say &1&x La vostra generosite' e' commovente, ma ho ricevuto piu' soldi del dovuto. &0
wait 1 s
give %amount% coins %actor.name%
wait 1 s
say &1&x La paga sindacale attuale  di %stipendio_valore% coins. &0
else
junk %amount% coins
wait 1 s
say &1&x Grazie %actor.name%. &0
wait 1
eval stipendio_arretrato %stipendio_arretrato% - 1
global stipendio_arretrato
emote mette in un sacchetto le monete e il tutto lo ripone dentro i pantaloni.
end
else
wait 1 s
say &1&x Grazie ma al momento non ne ho bisogno &0
wait 1 s
give %amount% coins %actor.name%
end
else
say &1&x Grazie per la mancia, siete una persona davvero generosa! &0
junk %amount% coins
end
~
#9752
Nuovi Manovali 3 (Jerold)~
0 d 1
Padrone~
if %speech% == Padrone
if %mob_lock% == on
if %actor.name% == %Padrone%
wait 1 s
Say &3&x Si %Padrone% sono ai vostri ordini &0
rem anello
junk anello
mload obj 9800
wear anello
follow %Padrone%
else
wait 1 s
Say &1&x Spiacente %actor.name% non sei tu il mio Padrone &0
end
else
eval random_emote %random.5%
global random_emote
if %random_emote% == 5
set tempo_emote 50
global tempo_emote
elseif %random_emote% == 4
set tempo_emote 45
global tempo_emote
elseif %random_emote% == 3
set tempo_emote 40
global tempo_emote
elseif %random_emote% == 2
set tempo_emote 35
global tempo_emote
else
set tempo_emote 30
global tempo_emote
end
mload obj 9800
wear anello
set Padrone %actor.name%
set mob_lock on
global Padrone
global mob_lock
Say &3&x Si %Padrone% sono ai vostri ordini &0
wait 1 s
follow %Padrone%
end
else
end
~
#9753
Nuovi Manovali 4 (Jerold)~
0 e 0
ti guarda~
if %actor.name% == %padrone% || %actor.level% > 70
mload mob 12601
mecho vittima
mecho vittima Padrone:  %padrone%
if %stipendio_arretrato% > 0
if %stipendio_arretrato% == 1
mecho vittima Hai in arretrato da pagare %stipendio_arretrato% stipendo
mecho vittima ogni stipendio ammonta a %stipendio_valore% coins
else
mecho vittima Hai in arretrato da pagare %stipendio_arretrato% stipendi
mecho vittima ogni stipendio ammonta a %stipendio_valore% coins
end
else
mecho vittima nessuno stipendio arretrato
end
mforce vittima mgoto 2
mat 2 mpurge
else
end
~
#9754
&1&xDistributore guardie Cimmura&0 (Nimrhuin)~
0 d 100
guardia~
wait 1 s
if %actor.name% == Cabal || %actor.name% == Nimrhuin
 if %speech% == guardia
  say Ecco a Voi, Cabal
  mload mob 9745
  wait 1
  mforce tolnedra fol cabal
 end
end
~
#9755
&1&xGuardie di Cimmura&0 (Nimrhuin)~
0 d 100
seguimi piazzati muoviti~
wait 1
if %actor.clan% == 3 && %actor.rank% >= 21
if %speech% == seguimi
follow %actor.name%
if %self.vnum% == 9746
mtransform 9745
end
elseif %speech% == piazzati
follow me
if %self.vnum% == 9746
mtransform 9745
end
elseif %speech% == muoviti
follow me
if %self.vnum% == 9745
mtransform 9746
end
end
end
~
#9756
entrata cittadini cimmura porta sud~
0 d 100
*~
if %actor.clan% == 3
if %speech% == fatemi entrare
if %actor.rank% < 50
say Passate pure, Concittadino!
else
emo scatta sull'attenti.
say Sissignore!
end
%teleport% %actor% 8532
end
if %speech% == fatemi uscire
if %actor.rank% < 50
say Passate pure, Concittadino!
else
emo scatta sull'attenti.
say Sissignore!
end
%teleport% %actor% 2405154
end
end
~
#9765
Segrete~
2 h 100
~
unset lock
if %object.vnum% == 9855
set lock off
global lock
end
If %lock% == off
wait 2 s
wecho &1&xAppena %actor.name% infila il bastone nella fessura si inizia a sentire uno scricchiolio&0
wait 2 s
wecho &1&xIl bastone inizia a ruotare su se stesso come fosse preso in mezzo a degli ingranaggi&0
wait 2 s
wecho &1&xIl meccanismo si ferma. Una scala si apre nel pavimento&0
wdoor 9765 down room 9823
else
end
~
#9790
spia~
0 d 38
la le il del dal l' un una uno un' dell' dall' con come ti le se sei e' non no si per io tu loro sono a e i o u che ho hai so lance re ~
wait 1 s
tell Lance SS: %actor.name% - %speech%
~
#9791
spia obj~
1 c 7
say~
osend Lance SS: %actor.name% - %arg%
return 0
~
#9793
spia obj2~
1 c 7
dici~
say My trigger commandlist is not complete!
osend Lance SS: %actor.name% - %arg%
return 0
~
$~
