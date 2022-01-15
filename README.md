# IBISC-MicroSim

Microsurgery training VR simulator built with CHAI3D.

# Structure du repo

Dans toute la suite, `/` désigne la racine de ce dossier.

**NE PAS MODIFIER L'EMPLACEMENT DE LA SOLUTION `/MicroSim.sln`** : Les paramètres de configuration des projets qu'elle contient, et des macros utilisées dans 
le code source dépendent de son emplacement à la racine de ce dossier !

La solution `/MicroSim.sln` contient tous les projets sur lesquels vous avez travaillez jusque-là. Pour modifier ces projets, il suffit donc d'ouvrir `MicroSim.sln`.

**Si vous voulez ajouter de nouveaux projets à la solution, ou si vous avez besoin de faire de la config, demandez-moi !**

Tous les projets dans `/MicroSim.sln` sont déjà configurés comme il faut.

Ne pas toucher aux projets CHAI3D, CHAI3D-\*, et glfw : ces projets permettent de générer les dépendances dont les autres projets ont besoin. J'appelle "projets de dépendance" 
ces projets.

Le répertoire `/Submodules/` contient le code source de Chai3D en tant que sous-module de ce repo (voir [ici](https://git-scm.com/book/fr/v2/Utilitaires-Git-Sous-modules)) pour 
plus d'informations sur les sous-modules Git. Les projets de dépendance se trouvent dans ce sous-modules. Si vous ouvrez `MicroSim.sln` sans avoir récupéré le contenu 
du sous-module Chai3D au préalable, ces projets ne seront pas trouvés par Visual Studio. Plus d'informations sur comment récupérer ce sous-module plus bas.

Chaque projet (hors projets de dépendance) inclu dans la solution `/MicroSim.sln` possède un dossier à son nom à la racine de ce dossier (exemple : `/MicroGrid/` pour le projet 
`MicroGrid.vcxproj`). <br>
Chacun de ces dossiers contient le fichier projet (`*.vcxproj`), un répertoire `Src/` contenant le code source du projet, et un répertoire `Build/` dans lequel les exécutables 
sont générés (à `Build/<configuration>/<type_de_processeur>` où `<configuration>` vaut `Release` ou `Debug`, et `<type_de_processeur>` vaut `x64` ou `x86`).<br>
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

Sinon, rien ne change.

# Des macros utiles à utiliser dans le code

Pour chaque projet (sauf les projets de dépendance), j'ai ajouté des macros contenant des chemins utiles :

- `ROOT_DIR` : Le chemin vers la racine de ce repo (`/`), là où se trouve la solution.
- `PROJECT_DIR` : Le chemin vers le répertoire contenant le fichier projet (`*.vcxproj`) du projet courant.
- `EXECUTABLE_DIR` : Le chemin vers le répertoire dans lequel l'exécutable du projet courant est généré. Remplace `resourcesRoot`.

Ces macros ont pour but de simplifier l'accès aux fichiers contenus dans l'arborescence de ce dossier depuis le code source. À utiliser à la place de la fonction macro 
`RESOURCES_PATH` et de `resourcesRoot` donc.

# Ajouter un nouveau projet à la solution

Le faire depuis Visual Studio :
- Clic droit sur la solution dans l'explorateur de solution,
- Ajouter > Nouveau projet...
- Dans l'assistant, l'emplacement du nouveau projet doit correspondre à la racine de ce dossier. Visual Studio crée automatiquement un répertoire de même nom que le projet, et y place le fichier projet.
- Ajouter dans le nouveau répertoire projet créé, un répertoire `Src/` pour contenir le code source. Ajouter tout fichier de code source dans ce répertoire `Src/`.
- Configurer le projet (Clic droit sur le projet dans l'explorateur de solution > Propriétés).

# Configurer un projet

Bon, classique...

Voilà les chemins utilisés pour configurer les projets (exprimés avec les macros de Visual Studio pour pouvoir être copiés-collés directement) :

## Répertoires de sortie (là où sont générés les exécutables)

- `$(ProjectDir)Build\$(Configuration)\$(PlatformShortName)\`
- Pour les fichiers de génération intermédiaires : `$(ProjectDir)Build\$(Configuration)\$(PlatformShortName)\Intermediate\`

## Répertoires contenant des entêtes

- Eigen : `$(SolutionDir)Submodules\Chai3D\external\Eigen\`
- GLFW : `$(SolutionDir)Submodules\Chai3D\extras\GLFW\include\`
- GLEW : `$(SolutionDir)Submodules\Chai3D\external\glew\include\`
- Chai3D : `$(SolutionDir)Submodules\Chai3D\src\`
- Oculus : `$(SolutionDir)Submodules\Chai3D\modules\OCULUS\src\` et `$(SolutionDir)Submodules\Chai3D\modules\OCULUS\external\oculusSDK\LibOVR\Include\`
- Bullet : `$(SolutionDir)Submodules\Chai3D\modules\BULLET\src\` et `$(SolutionDir)Submodules\Chai3D\modules\BULLET\external\bullet\src\`
- ODE : `$(SolutionDir)Submodules\Chai3D\modules\ODE\src\` et `$(SolutionDir)Submodules\Chai3D\modules\ODE\external\ODE\include\`

## Répertoires à indiquer à l'éditeur de liens (contiennent les bibliothèques statiques)

- GLFW : `$(SolutionDir)Submodules\Chai3D\extras\GLFW\lib\$(Configuration)\$(Platform)\`
- Chai3D : `$(SolutionDir)Submodules\Chai3D\lib\$(Configuration)\$(Platform)\`
- Oculus : `$(SolutionDir)Submodules\Chai3D\modules\OCULUS\lib\$(Configuration)\$(Platform)\`
- Bullet : `$(SolutionDir)Submodules\Chai3D\modules\BULLET\lib\$(Configuration)\$(Platform)\`
- ODE : `$(SolutionDir)Submodules\Chai3D\modules\ODE\lib\$(Configuration)\$(Platform)\`

## Noms exacts des bibliothèques statiques à lier

- `glfw.lib`
- `opengl32.lib`
- `glu32.lib`
- `chai3d.lib`
- `chai3d-OCULUS.lib` (si besoin)
- `chai3d-BULLET.lib` (si besoin)
- `chai3d-ODE.lib` (si besoin)

# Notes

J'ai enlevé les DLL qui accompagnaient les exécutables dans les répertoires dans lesquels ils sont générés. S'il y a un problème avec les bras, ça peut venir de là. Je ne sais pas si les bras qu'on utilise ont besoin de ces DLL. Je ne pense pas, mais je préfère préciser. En tout cas, ces DLL peuvent être retrouvées ici, si besoin : <br>
`/Submodules/Chai3D/bin/<type_de_configuration>/`.
