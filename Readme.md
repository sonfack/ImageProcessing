# Readme 


## Structure du code 
Le code de cett application est structuré comme suit : 
*Le dossier principale : celui qui contient tout le code source de l'application. A l'interieur on y trouve :  
	*Le fichier de gestion de la compilation __CMakeLists.txt__  
	*Le dosser __include__ qui contient le fichier entête __image.hpp__  
	*Le dossier __build__ qui contient les fichiers intermediaires de la compilation et le fichier exécutable de l'application  
	*Le dossier __src__ qui contient les fichier sources *image.cpp*, *main.cpp* et les dossers :   
		*__sources__ ou sont stoquées les images à traiter 
		*__resultats__ ou seront stoquées les resultats apès traitement

## Compilation 
Pour compilier le code il faut:  
1. se placer dans repertoir __build__ 
2. taper la commande __ckmake ..__  
3. taper la commande __make__  
Apres compilation, le fichier executable est créé dans le dossier __build__ et s'appele __run__   

## Execution 
Pour l'execution, il faut mettre les images sources qui seront utilisées and le dossier __src/sources__  
Pour la suite, aller a l'invite de commande et entrer la commande __./run__  _
 

