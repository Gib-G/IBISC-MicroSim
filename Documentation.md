Au lancement de MicroSim, la console s'ouvre pour demander à l'utilisateur son identifiant pour donner un nom spécifique à son fichier de sauvegarde.
Si aucun identifiant n'est rentré, le fichier de sauvegarde par défaut est pris.

Le menu principal comporte deux boutons pour sélectionner un niveau.
Les boutons de MicroSim fonctionnent de la manière suivante : l'utilisateur doit presser la touche principale du bras haptique (située sur le bras) puis entrer en contact avec le bouton.
Une fois le niveau sélectionné, l'épreuve se lance :

MicroSim :
L'exercice se lance en appuyant sur le bouton Start. Le timer ne s'incrémente que lorsque l'exercice est en cours.
Le bouton Reset réinitialise le canvas et stoppe l'exercice.
Le bouton représenté par un chiffre donne le pattern actuel que l'utilisateur doit compléter, l'utiliser change le pattern.
Le bouton de rotation (la flèche incurvée) permet de changer l'orientation du canvas et de faire varier les postures demandées par l'exercice.

Une fois l'épreuve lancée, les boutons permettant de paramétrer l'exercice disparaissent, l'utilisateur ne peut utiliser que le bouton Reset ou le bouton Save (accessible uniquement durant l'épreuve).
Le bouton Save met fin à l'épreuve et sauvegarde les résultats du joueur.

Entrer en contact avec le canvas à n'importe quel moment permet de dessiner sur celui-ci, si la zone sur laquelle l'utilisateur dessine est celle attendue (représentée par la couleur jaune), la couleur produite sera verte, ou orange si l'utilisateur appuie trop fort, sinon rouge.
Les résultats sauvegardés sont établis via une analyse pixel par pixel du canvas selon la couleur de chaque pixel.

La position des bras haptiques est sauvegardée dans des fichiers temporaires à intervalles réguliers pour obtenir des mesures exploitables.
Le fichier de sauvegarde générés est un .csv nommé selon l'identifiant entré au début et contient :
les positions sauvegardées durant l'exercice, le pourcentage de pixels corrects, de pixels symbolisant une erreur de pression ou les pixels erreurs, ainsi que les paramètres relatifs au pattern et à la rotation du canvas.

Around the Clock : 
L'utilisateur possède 2 pinces représentant les bras haptiques.
La touche principale de chacun des bras haptiques enclenche les pinces et permet d'attraper l'aiguille.
L'objectif est de faire passer l'aiguille au travers de tous les anneaux.
Pour valider le passage au travers de l'anneau, il faut le faire traverser dans le bon sens (au travers du cercle de couleur bleu).
Cela permet de simuler les gestes de suture.
Au moment où l'aiguille est attrapée, l'exercice commence et le timer s'incrémente.
Le bouton Reset apparaît et permet de réinitialiser la scène.


Informations générales :
Les variables permettant de gérer certains paramètres comme la force maximale autorisée sur le canvas (MAX_FORCE) sont localisées dans les headers respectifs des exercices du projet IBISC-Microsim (cGridLevel.h et cAroundTheClock.h).

La carte Arduino utilisée est décrite dans Resources\Arduino, son script et le schéma de branchement y sont disponibles.
Dans toute la simulation, les deux boutons de la carte Arduino permettent de simuler le zoom et dezoom d'un microscope.

MicroSim utilise trois threads distincts : un graphique, un physique (haptique inclus) et un dédié à l'Arduino.
La simulation peut être lancée sans casque VR, la caméra peut être alors controlée via la souris (rotation en cliquant sur la fenêtre puis mouvement de la souris, mouvements horizontaux via ZQSD et verticaux via SPACE/Ctrl).
Le 3ème pattern de MicroGrid peut être changé en utilisant le même nom : customPattern.png. (localisé dans Resources\Images)
Il est nécessaire d'utiliser le même jaune pour la reconnaissance des couleurs. (rgba(255,255,0,255) ou setYellow() par défaut dans Chai3D)