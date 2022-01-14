# IBISC-MicroSim

Microsurgery training VR simulator built with CHAI3D.

# Structure du repo

Dans toute la suite, `/` désigne la racine de ce dossier.

**NE PAS MODIFIER L'EMPLACEMENT DE LA SOLUTION `/MicroSim.sln`** : Les paramètres de configuration des projets qu'elle contient, et des macros utilisées dans 
le code source dépendent de son emplacement à la racine de ce dossier !

La solution `/MicroSim.sln` contient tous les projets sur lesquels vous avez travaillez jusque-là. Pour modifier ces projets, il suffit donc d'ouvrir `MicroSim.sln`.

**Si vous voulez ajouter de nouveaux projets à la solution, ou si vous avez besoin de faire de la config, demandez-moi !**

Tous les projets dans `/MicroSim.sln` sont déjà configurés comme il faut.

Ne pas toucher aux projets CHAI3D, CHAI3D-*, et glfw : ces projets permettent de générer les dépendances dont les autres projets ont besoin. J'appelle "projets de dépendance" 
ces projets.

Le répertoire `/Submodules/` contient le code source de Chai3D en tant que sous-module de ce repo (voir [ici](https://git-scm.com/book/fr/v2/Utilitaires-Git-Sous-modules)) pour 
plus d'informations sur les sous-modules Git. Les projets de dépendance se trouvent dans ce sous-modules. Si vous ouvrez `MicroSim.sln` sans avoir récupéré le contenu 
du sous-module Chai3D au préalable, ces projets ne seront pas trouvés par Visual Studio. Plus d'informations sur comment récupérer ce sous-module plus bas.

Chaque projet (hors projets de dépendance) inclu dans la solution `/MicroSim.sln` possède un dossier à son nom à la racine de ce dossier (exemple : `/MicroGrid/` pour le projet 
`MicroGrid.vcxproj`). <br>
Chacun de ces dossiers contient le fichier projet (`*.vcxproj`), un répertoire `Src/` contenant le code source du projet, et un répertoire `Build/` dans lequel les exécutables 
sont générés (à `Build/<configuration>/<type_de_processeur>` où `<configuration>` vaut `Release` ou `Debug`, et `<type_de_processeur>` vaut `x64` ou `x86`). <br>
Ça serait bien de respecter cette structure pour chaque répertoire projet.

J'ai changé les noms des projets. Voilà les correspondances :
- 01-oculus-cube  -> renommé en -> AroundTheClock.
- 02-oculus-space -> renommé en -> MicroGrid.
- 03-oculus-exploration conserve ce nom.
- 06-bullet-exploration conserve ce nom.

J'ai aussi ajouté un projet vierge (`TemplateProject`) dans lequel il y a la possibilité d'utiliser Oculus, Bullet, et ODE.

Le répertoire `/Resources/` contient toutes les resources utilisées par les différents projets (images, shaders, modèles 3D, etc...). Il regroupe tout ce qui était éparpillé 
dans plusieurs répertoires `resources/` dans le repo Git original. <br>
**Toutes les ressources sont à déposer dans ce dossier !** <br>
**Si ce dossier `/Resources/` devient trop volumineux, faites-moi signe ! Il y a aussi la possibilité de le gérer comme un sous-module de ce repo (comme Chai3D).**

# Workflow

**Ce repo possède une branche `backup` qui contient la version initiale de la solution configurée comme il faut. Merci de ne pas y toucher.**

- Cloner le repo sans les sous-modules : utiliser `git clone`.
- Cloner le repo en récupérant le contenu des sous-modules (Chai3D) : utiliser `git clone --recurse-submodule`.
- Récupérer le contenu des sous-modules après avoir cloné le repo : utiliser `git submodule update --init --recursive` (**ATTENTION : SI VOUS N'AVEZ PAS UTILISÉ 
`git clone --recurse-submodule` AU PRÉALABLE, BIEN ATTENDRE QUE LA COMMANDE SE TERMINE ! On peut avoir l'impression qu'elle plante au moment de cloner les sous-modules. Ce n'est pas le cas. 
Le sous-module Chai3D est juste énorme, et Git n'affiche pas la progression du "clônage" après avoir invoqué `git submodule update --init --recursive`.**).
- Récupérer d'éventuelles modifications apportées aux sous-modules : utiliser `git submodule update --remote` (Fetch et merge le contenu de la branche `master` du sous-module).
- Même chose, mais pendant un `pull` : utiliser `git pull --recurse-submodule`.

