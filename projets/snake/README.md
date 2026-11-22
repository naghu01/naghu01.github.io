# Snake en C – projet de groupe BTS SIO SLAM

Jeu Snake en console Windows, écrit en C (compatible C90 / Dev-C++), réalisé en équipe de 4 à l'École IRIS (octobre-novembre 2026).

## Équipe NODE et répartition

| Membre | Rôle |
| --- | --- |
| Benediction | Chef de projet : menu, mode ordinateur |
| Lamine | Affichage et commandes clavier |
| Farid | Logique du jeu (déplacements, collisions, croissance) |
| **Najeeb** | **Mission A : 3 pommes · Mission B : sauvegarde et chargement · présentation** |

## Ma contribution (Najeeb)

| Fonction | Lignes | Rôle |
| --- | --- | --- |
| `occupe()` | 76-82 | Vérifie qu'une case n'est prise ni par le serpent ni par une autre pomme |
| `creerPommes()` | 83-97 | Place 3 pommes au hasard sur des cases libres |
| `sauvegarder()` | 134-144 | Écrit l'état de la partie dans `snake_save.txt` (touche P) |
| `charger()` | 145-189 | Relit la sauvegarde et contrôle chaque valeur avant de reprendre la partie |

## Fonctionnalités

- 3 pommes à la fois : +10 points par pomme, le serpent grandit d'une case quand les 3 sont mangées
- Sauvegarde (P) et reprise de partie depuis le menu
- Mode ordinateur lent ou rapide : le serpent choisit seul sa direction et évite les impasses
- Sons et musique activables (M, N)

## Compiler et lancer (Windows)

```
gcc snake.c -o snake.exe
snake.exe
```

Commandes : W A S D pour diriger, P pour sauvegarder, X pour quitter.
