==== viewer - visualizzatore per il progetto di CG-1213 =====


-- USO e NOTE GENERALI --

Il viewer carica una mesh che prende da linea di comando (./viewer
meshfile). Si esce dal programma premendo sul bottone "quit" o usando
ESC.

Il progetto e' supportato su Linux e MacOSX. A nessuno e' vietato di
usare Windows, ma non e' offerto supporto per farlo compilare e poi
dovete controllare che giri su UNIX prima di consegnare.



-- FILE --

Il progetto contiene 3 cartelle: eigen3.0 (che contiene Eigen come lo
scaricate dal web); AntTweakBar, una libreria che permette di creare
delle interfacce molto semplici e veloci ma piu' evolute di quello che
si puo' fare con GLUT da solo; viewer, che contiene il codice sorgente
di un visualizzare che e' il punto di partenza del vostro lavoro.

La documentazione di Eigen e AntTweakBar la trovate facilmente in
rete. NDR: Eigen e' identico a quello che scaricate, ma nel Makefile
linux di AntTweakBar c'e' una piccola modifica. Usate la versione che
vi e' stata fornita da noi.

Per occupare meno spazio, dalle cartelle Eigen e AntTweakBar sono
stati tolti esempi, documentazione etc. Se ne sentite il bisogno
riscaricateli.



-- COMPILARE --

COMPILAZIONE LINUX:
cd AntTweakBar/src; make
cd ../..
cd viewer; make

E siete pronti.

COMPILAZIONE MACOSX:
cd AntTweakBar/src; make -f Makefile.osx
sudo cp ../lib/libAntTweakBar.dylib /usr/lib/
cd ../..
cd viewer; make

E siete pronti.

NB: I comandi li ho scritti a memoria per dare un'idea, non li ho
controllati! Non fate copia-incolla senza prima avere acceso il
cervello (in particolare per chi usa sudo).

Potete utilizzare make debug per compilare senza ottimizzationi e con
simboli di debug e assert abilitati.


-- MAKEFILE --

Se avete bisogno di aggiungere dei file vostri, dovrete modificare il
makefile allegato. Sembra complicato ma in realta' e' facilissimo, fa
gia' tutto da solo.

Ricordatevi di aggiungere i file sorgente alla variabile CPPSOURCES
(solo i file .cc, non aggiungete nessun header che quelli vengono
gestiti automaticamente dal calcolo delle dipendenze!!) e di
modificare eventuali variabili FLAGS, INCLUDES, DEFINES, LDFLAGS se ve
ne fosse bisogno (ignorate quelle language-specific che tanto il
progetto e' solo C++). Le variabili RELEASE_FLAGS e DEBUG_FLAGS
servono a definire cosa viene abilitato usando make oppure make debug.

Ricordatevi che il makefile funziona solo con file .cc! Se preferite
cpp, cambiate l'estensione CPP_EXT nel makefile e rinominate tutti i
file.



-- SORGENTI --

viewer.cc: main(), callback GLUT e interfaccia grafica AntTweakBar.

VF.hh: classe per gestire, caricare e disegnare la mesh.
      computa il laplaciano (matrice 'L') con il metodo initL().

camera.* e trackball.*: classi per gestire rotazione, pan e zoom. Non
	   		dovrebbe esserci bisogno di toccarle, a meno
	   		che non sia qualche bug, cosa assolutamente
	   		possibile :-)

Buon lavoro!!!
