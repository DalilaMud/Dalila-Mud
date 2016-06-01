# Dalila Mud
Qui sono raccolti tutti i file sorgenti ed i files del mondo di Dalila + alcuni tools in sviluppo.

## directory
* src = Contiene i sorgenti in C.
* lib/world = Contiene le rooms, i mobs e gli shops del mondo di dalila.
* cnf = file di configurazione usati dal comando "configure".
* bin = Tendenzialmente vuota, necessaria ad accogliere la sorgente compilata ed altri eseguibili.
* export contiene i tools per l'esportazione dei dati in diversi formati.
* doc = doc

### Github html page
* index.html
* javascripts
* stylesheets

### HEROKU
* Procfile
* public

## Export
Per far funzionare gli export

    php inport.php  // importa (al momento solo le rooms) in un unico files rooms.json
    npm install      // installare i moduli aggiuntivi di node 
    node export.js   // esporta il diversi formati, al momenti json, yaml, csv
    
### Happy Mudding ^^
