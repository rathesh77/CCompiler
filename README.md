# Compilateur langage ALGO vers Javascript

Compilateur qui génère un code "cible" en Javascript à partir d'un code "source" écrit dans un langage algorithmique, avec une syntaxe précise.
Le fichier "source" doit avoir l'extension ".algo" (ex: **test.algo**). Un exemple est déjà fourni à la racine du projet. (**sample.algo**).
## Fonctionnalités 

- opérateur binaire (+, -, >=, >, <, <=, ==, !=, *, /, ET, OU)
- appel de fonction
- définition de fonction (fonction mafonction (entier a) : entier { } )
- déclaration de variable / initialisation (entier a; entier b = 1;)
- affectation (b = 2)
- branchement (si / sinon si / sinon)
- boucle (tantque () { })
- retourner
- variable
- entier


# Compiler et executer le projet
Pour compiler, il faut posséder un compilateur C (gcc, clang ou autre...) pour compiler le code source du projet, et l'outil **Make** (optionnel ici) qui permet beaucoup de choses, notamment compiler le code source d'un projet en une commande simple. Vous pouvez compiler manuellement le code source, ce qui ne pose pas de réel problème ici, car nous n'avons pas beaucoup de fichiers .c à compiler. Dans un plus gros projet, on rencontrerait des soucis. 

Compilation avec **Make**: ```make compile```

Compilation  "à la main": ```gcc main.c */*.c -g -o main```

Execution: ```./main.exe sample.algo```

**N.B: Ne pas oublier de fournir le nom du fichier "source" en argument de la commande d'exécution**.
