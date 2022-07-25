# Compilateur langage ALGO vers Javascript

Compilateur écrit en **langage C**, qui génère un code "cible" en **Javascript** à partir d'un code "source" écrit dans un **langage algorithmique** à syntaxe francaise.

Le fichier "source" doit avoir l'extension ".algo" (ex: **test.algo**). Un exemple est déjà fourni à la racine du projet. (**sample.algo**).

## Fonctionnalités prises en charge

**Analyse lexicale/syntaxique :**

- opérateur binaire (+, -, >=, >, <, <=, ==, *, /, ET, OU)

- appel de fonction

- définition de fonction (fonction mafonction (entier a) : entier { } )

- déclaration de variable / initialisation (entier a; entier b = 1;)

- affectation (b = 2)

- branchement (si / sinon si / sinon)

- boucle (tantque () { })

- retourner

- variable de type entier SEULEMENT
<br>

**Analyse sémantique :**

- Vérification des variables (declaration/redeclaration) dans les boucles, conditions, fonctions et appels de fonctions.

- Vérification des fonctions (declaration/appel)

## Ce qui ne marche pas
- Les nombres négatifs (entier a = -1)

- opérateurs +=, ++, --, -=, /=, *= (optez plutôt pour des écritures comme ```a = a + 1```)

-  Il y a un problème de portée de variable dans les blocs "sinon si/sinon"
	```
	sinonsi (...) { 
		let a = 5; 
	} 
	sinon { 
		let a = 6; 
	}
	```
	Ce bout de code ne compilera pas alors qu'il devrait. Utilisez des noms de variable différents dans chaque bloc. En soit, c'est une bonne pratique de développement 🙂.

# Compiler et exécuter le projet

Pour compiler, il faut posséder un compilateur C (gcc, clang ou autre...) pour compiler le code source du projet, et l'outil **Make** (optionnel ici) qui permet de compiler le code source du projet en une commande. Vous pouvez compiler manuellement le code source, ce qui ne pose pas de réel problème ici, car nous n'avons pas beaucoup de fichiers .c à compiler. Dans un plus gros projet, on rencontrerait des soucis.

Compilation avec **Make**: ```make compile```

Compilation "à la main": ```gcc main.c */*.c -g -o main```

Execution: ```./main.exe sample.algo```

# Quelques notes importantes
- Ne pas oublier de fournir le nom du fichier "source" en argument de la commande d'exécution.
- L'analyse sémantique est sujette à contenir des bugs que je n'aurai pas identifié. De ce fait, lors de l'exécution du programme, si un message "analyse sémantique incorrecte" s'affiche dans la console alors que la structure du fichier .algo est bonne, modifiez la fonction main dans le fichier **/main.c** en remplaçant la condition à la **ligne 35** ```if (has_valid_semantic(ast) == true)``` par ```if (true)```. En faisant cela, le programme sautera cette étape, et vous pourrez continuer vos tests sans que ce soit trop encombrant.
- Lorsque vous vous trompez involontairement en faisant une erreur de syntaxe dans le fichier .algo, le compilateur va simplement ecrire en sortie le message "Une ou plusieurs erreurs de syntaxe rencontrées" et ne donnera pas de détail. Il faut donc faire attention à ce que vous écrivez.
