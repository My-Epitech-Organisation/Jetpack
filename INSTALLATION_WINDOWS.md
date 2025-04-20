# Guide d'installation de Jetpack sur Windows

Ce document explique comment installer et exécuter le jeu Jetpack sur Windows.

## Prérequis

- Un PC sous Windows 10 ou 11
- Droits d'administrateur sur votre ordinateur
- Connexion Internet pour télécharger les dépendances

## Étape 1 : Préparation

1. Copiez l'intégralité du dossier Jetpack de Linux vers votre PC Windows
2. Assurez-vous que le fichier `cross_platform.hpp` est présent dans le dossier `client/network/`
   - S'il n'est pas présent, vous devrez le créer selon les instructions précédentes

## Étape 2 : Installation des dépendances

1. Ouvrez l'Explorateur de fichiers et naviguez jusqu'au dossier Jetpack
2. Double-cliquez sur le fichier `setup_windows.bat`
3. Suivez les instructions à l'écran
   - Le script va télécharger et installer :
     - Visual Studio Build Tools (compilateur C++)
     - CMake (système de construction)
     - SFML 2.5.1 (bibliothèque graphique)
   - Il va également créer deux scripts utiles pour la compilation

## Étape 3 : Configuration du projet

1. Une fois l'installation terminée, double-cliquez sur `prepare_cmake.bat`
   - Ce script va configurer les fichiers CMake pour Windows en utilisant le fichier `CMakeLists_windows.txt` préparé précédemment

## Étape 4 : Compilation du jeu

1. Double-cliquez sur `build_jetpack.bat`
   - Ce script va :
     - Configurer le projet avec CMake
     - Compiler le code source
     - Copier les DLLs de SFML nécessaires
2. Attendez la fin de la compilation

## Étape 5 : Exécution du jeu

1. Le client compilé se trouve dans le dossier `build\client\Release\`
2. Pour lancer le jeu, ouvrez une invite de commande (cmd) et naviguez jusqu'à ce dossier
3. Exécutez la commande :
   ```
   jetpack_client.exe -h <adresse_ip_serveur> -p <port_serveur> [-d]
   ```
   - Remplacez `<adresse_ip_serveur>` par l'adresse IP du serveur Jetpack
   - Remplacez `<port_serveur>` par le port sur lequel le serveur écoute
   - L'option `-d` active le mode débogage (facultatif)

## Résolution des problèmes courants

### Erreur "SFML introuvable"
- Vérifiez que le dossier SFML a bien été extrait dans le dossier principal du projet
- Assurez-vous que le chemin vers SFML est correctement configuré dans la variable `SFML_DIR`

### Erreur de DLL manquante
- Vérifiez que les fichiers .dll de SFML ont bien été copiés dans le dossier contenant l'exécutable
- Si nécessaire, copiez-les manuellement depuis `sfml\SFML-2.5.1\bin\` vers `build\client\Release\`

### Erreur de connexion réseau
- Vérifiez que le serveur est bien en cours d'exécution
- Assurez-vous que le pare-feu Windows n'empêche pas la connexion
- Vérifiez que vous utilisez la bonne adresse IP et le bon port

### Autres erreurs
- Consultez les logs de débogage en exécutant le client avec l'option `-d`
- Les logs seront créés dans un dossier `debug` à côté de l'exécutable