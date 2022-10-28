
Veloce tutorial su git:

Video Tutorial: https://www.youtube.com/watch?v=8JJ101D3knE&t=9s
per le cose essenziali potete fermarvi a 30:21


(Do per scontato che siamo su ambiente linux)
Dopo che avete installato git e fatto tutta la configurazione iniziale che vi fa vedere nel tutorial
da terminale posizionatevi nella directory che volete e 
scrivete:

## Introduzione ##

git clone https://github.com/Angelo-Pio/HomeWork_TLP

questo comando scaricherà una cartella HomeWork_TLP nella directory in cui siete posizionati

di base il branch di partenza sarà master e gli altri branch presenti non verranno scaricati
 per creare il proprio bisogna scrivere in terminale:

git branch <nome_branch>

con 

git branch 

sarà possibile vedere la lista di branch presenti, il terminale evidenzierà con un asterisco al lato il branch corrente

mentre con git checkout <nomebranch> ci si potrà spostare tra un branch e l'altro

per ottenere un altro branch presente su github invece bisognare scrivere

git pull origin <branchname> ; git checkout <branchname>

## Aggiungere file al repository online ##

(questi comandi funzionano con i file relativi alla branch in cui si è posizionati)

con git add <nomeFile> oppure <nomeDirectory> aggiugnete i file alla staging area (tendenzialmente scrivete git add . e vi aggiunge tutto)

poi fate 

git commit -m "Commento per illustrare cosa avete fatto" 

questo mette i file che avete aggiunto nella staging area nella commit area, qui ci sono tutti i commit
il commento è obbligatorio cercate di scrivere poco ma che si capisca cosa avete cambiato rispetto all'ultimo commit.

Potete avere più commit in quest'area, questa cosa è comoda per una serie di ragioni.
quando dovete pushare tutti i commit locali su github scrivete

git push 

infine

git pull 

questo comando prende dalla repository online tutti i file nuovi e li unisce con quelli locali in caso
di cambiamenti

## Esempio ##

Utente 1 crea un file Esempio.txt, ci scrive dentro "Hello" e scrive 

git add . ; git commit -m "Aggiunto Esempio.txt" ; git push 

Utente 2 fa:

git pull ; modifica il contentuto di Esempio.txt in "Hello World" e lancia gli stessi comandi di Utente1

Utente 1 scrive git pull e si ritroverà il file Esempio.txt cambiato

