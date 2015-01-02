#9280
Uscita Buco~
1 j 100
~
if (%actor.clan%==5)
  if (%actor.rank>=26)
    if (%actor.room%==12613)
      wait 1
      oteleport %actor.name% 12592
      oforce %actor.name% look
    end
  end
end
~
#9281
Consegna Medaglione Fossa~
0 d 100
medaglione~
if (%speech%==consegna medaglione)
  osend %actor.name% va bene, lo prendo
end
~
$~
