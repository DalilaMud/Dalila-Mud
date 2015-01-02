#41500
offerta quest ulexon_1 (zephy)~
0 d 100
fantasmi spettri lavori~
context %actor.id%
if !(%actor.varexists(quest_ulexon_1)% && %actor.level% < 35)
say &6 Fantasmi! SI PROPRIO FANTASMI! SISSIGNORE!&0
say &6 io li ho visti con i miei occhi... C'e' stato un crollo pochi giorni fa mentre cercavamo di smantellare&0
wait 4 s
say &6 una delle strutture piu' recenti, una sorta di camerino per gli attori, che e' stato aggiunto al progetto &0
wait 4 s
say &6 originale solo di recente. Ebbene, dopo questo crollo (dovuto beninteso non all' attivita' dei nostri manovali&0
emote gonfia il petto con serieta'
wait 4 s
say &6 bensi' all'inadeguatezza dei materiali che sono stati utilizzati), ebbene dicevo... dopo questo crollo sono &0
wait 4 s
say &6 iniziate le prime apparizioni, all'inizio non ci credevo neppure io e ho ripreso i miei manovali, accusandoli &0
wait 4 s
say &6 di star inventando sciocche scuse... Ma poi ho dovuto ricredermi...&0
emote &x&9rabbbrrrrrividisce&0
say &6 Proprio mentre li stavo sgridando.. ho visto un ombra proiettarsi davanti a me, non un ombra qualsiasi!&0
wait 4 s
say &6 era un'ombra grigia... come se il corpo che le appartenesse fosse trasparente... mi sono girato e...&0
wait 5 s
piangi
say &6 ...quale orribile visione... -SIGH- non ho resistito -SOB- e me la sono data a gambe..seguito dai miei &0
wait 4 s 
say &6 manovali.. -SIGH--SOB- capite? ora non posso certo pretendere da loro che continuino a lavorare li' dentro&0
wait 4 s
say &6 -SOB- -SIGH-... devo trovare un modo per liberarmi di quelle creature... o perdero' l'appalto -SIGH-&0
wait 4 s
say &6 vi prego, aiutatemi... sarei disposto a pagarvi.. non ho molto perche' ho appena aperto l'attivita' ma..&0
say &6 ecco.. diciamo 5 monete d'oro... e tutta la mia riconoscernza! Vi prego non disprezzatela... non si sa mai&0
set offerta_quest on
global offerta_quest
say &6 allora accettate?&0
wait 10 s
rdelete offerta_quest %self.id%
else
halt
end
~
#41501
assegna quest_ulexon_1 (zephy)~
0 c 100
si~
context %actor.id%
if %offerta_quest% == on
say &6 FANTASTICO! Ve ne sono grato miei signori! per prima cosa vi consiglio di esplorare la zona del crollo&0
wait 4 s
say &6 forse c'e' qualcosa che li ha attirati... non so non mi intendo di queste cose, ma ritengo che  &0
wait 4 s
say &6 le vostre indagini debbano cominciare da li, provate a &3scavare&6 li' intorno&0
say &6 Inutile tentare di scacciarli, sono gia' morti e seppure&0
wait 4 s
say &6 si riescono ad allontanare per un po'... alla fine ritornano! &0
wait 4 s
say &6 ecco, ora vi apro. Fate in fretta tra 10 secondi la barriera tornera' attiva.&0
mload obj 41500
drop polvere
emote estrae della polvere dorata da una sacchetta e la soffia sulla barriera magica
wait 5 s
say &6 Buona fortuna amici, che gli Dei in cui credete vi proteggano!&0
set quest_ulexon_1 attiva
remote quest_ulexon_1 %actor.id%
set ulexon_1_chap_anfiteatro on
remote ulexon_1_chap_anfiteatro %actor.id%
vdelete %offerta_quest% %self.id%
else
say &6 si che? di cosa stai parlando? bah &0
halt
end
~
#41502
accesso anfiteatro (zephy)~
2 h 100
~
*---------------------------------------------------------------------------------------------------------------
* L'anfiteatro e' attualmente protetto da una barriera magica  posta dal fratello pandion di Berg,
* per evitare che ne escano gli spettri. Per aprirla momentaneamente bisogna soffiarli sopra la polverina azzurra.
*------------------------------------------------------------------------------------------------------------
if (%object.vnum% == 41500)
 wpurge %object%
 wpurge barriera
 wdoor %self% north room 6881
 wait 10 s
 wdoor %self% north purge
 wload obj 41501
else 
 halt
end
~
#41503
FEAR (zephy)~
0 k 70
~
if (%actor.name(is_pc)%)
msend %actor.name% %self% ti fissa negli occhi e presto ti senti avvolgere dal freddo e dall'oscurita'.
echo %self% trafigge %actor.name% con il suo sguardo
dg_cast 'fear' %actor.name%
else
halt
end
~
#41504
conclusione anfiteatro (zephy)~
0 i 100
~
context %actor.id%
if (ulexon_1_chap_anfiteatro==on) 
 if ((actor.has_item(41502) && (actor.has_item(41503))
 say &6Ohhh sembra che tu abbia trovato qualcosa di interessante...&0
 emote osserva con attenzione le pietre che hai trovato.
 wait 3 s
 say &6Incredibile.., hai visto che strani disegni che formano le luci? questa e' roba da Pandion...&0
 say &6Forse potrebbe interessare il nostro Capo: Malez, sapete lui e' un Pandion dai grandi poteri.&0
 wait 4 s
 say &6Ad ogni modo, le domande che dobbiamo porci sono: Sono collegate all'apparizione degli spettri?&0
 say &6Come ci sono finite qui? Da dove provengono? E perche' in'ora non han dato problemi?
 wait 4 s
 say &6Se volete un consiglio oltre a parlare con Malez dovrest recarvi al Comune di Lamorka e chiedere a Marzio&0
 say &0di &3consultare&6 i registri catastali, e anche parlare con Malez non sarebbe una cattiva idea&0
 wait 4 s
 set ulexon_1_chap_catasto on
 remote ulexon_1_chap_catasto %actor.id%
 set ulexon_1_chap_anfiteatro off
 remote ulexon_1_chap_anfiteatro %actor.id%
 set ulexon_1_chap_malez on
 remote ulexon_1_chap_malez %actor.id%
 else
 say &6 Allora?!? Avete trovato niente nella zona del crollo?!? &0
 say &6Nulla?! Tornate li' e cercate meglio! Ci sono in ballo un sacco di soldi oltre alla nostra reputazione!&0
 end
else
halt
end
~
#41505
uscita anfiteatro (zephy)~
2 h 100
~
*---------------------------------------------------------------------------------------------------------------
* L'anfiteatro e' attualmente protetto da una barriera magica  posta dal fratello pandion di Berg,
* per evitare che ne escano gli spettri. Per aprirla momentaneamente bisogna soffiarli sopra la polverina azzurra.
*------------------------------------------------------------------------------------------------------------
if (%object.vnum% == 41500)
 wpurge %object%
 wpurge barriera
 wdoor %self% south room 6880
 wait 10 s
 wdoor %self% north purge
 wload obj 41501
else 
 halt
end
~
$~
