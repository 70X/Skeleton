-- USO e NOTE GENERALI --

Il viewer carica una mesh che prende da linea di comando (./viewer
meshfile). Si esce dal programma premendo sul bottone "quit" o usando
ESC.

Il progetto e' supportato su Linux e MacOSX.



-- FILE --

Il progetto contiene 3 cartelle: eigen3.0, AntTweakBar, una libreria che permette di creare
delle interfacce molto semplici e veloci ma piu' evolute di quello che
si puo' fare con GLUT da solo; viewer, che contiene il codice sorgente
per la visualizzazione.

La documentazione di Eigen e AntTweakBar si trova in
rete. NDR: Eigen e' identico a quello che scaricate, ma nel Makefile
linux di AntTweakBar c'e' una piccola modifica.

Per occupare meno spazio, dalle cartelle Eigen e AntTweakBar sono
stati tolti esempi, documentazione etc.



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

Potete utilizzare make debug per compilare senza ottimizzationi e con
simboli di debug e assert abilitati.


-- MAKEFILE --

Se c'e' il bisogno di aggiungere dei file vostri, bisogna modificare il
makefile.


-- SORGENTI --

viewer.cc: main(), callback GLUT e interfaccia grafica AntTweakBar.

Mesh.hh: classe per gestire, caricare e disegnare la mesh.

camera.* e trackball.*: classi per gestire rotazione, pan e zoom. Non
	   		dovrebbe esserci bisogno di toccarle, a meno
	   		che non sia qualche bug, cosa assolutamente
	   		possibile :-)

