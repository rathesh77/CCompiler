# Compiler et executer le projet
```make compile run```

# Roadmap

## types de statements:
-declaration de variables / affectation
-boucles
-conditions
-appel de functions

## liste des mot-clés supportés par le langage cible
-tantque
-si

## type de variables:
-entier


stocker les variables
stocker les appels de functions


procedure:

on parcours chaque ligne
on detecte le type de statement de la ligne (declaration | affectation | boucle | condition | appel de function)
on extrait les tokens (regex)
on ajoute à l'AST
