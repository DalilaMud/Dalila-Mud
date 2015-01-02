#50001
new trigger~
0 g 100
~
   wecho &4Una Guardia dei Cancelli dice, 'Entrate pure, Concittadino! '&0
   wecho Un rumore metallico di ingranaggi e catenacci proviene dalla serratura del portone.
   wdoor 51001 north flags a
   wdoor 51000 south flags a
   wait 10 s
   wecho la Porta viene richiusa con un tonfo.
   wdoor 51001 north flags ab
   wdoor 51000 south flags ab
   wat wecho la Porta viene richiusa con un tonfo.
~
$~
