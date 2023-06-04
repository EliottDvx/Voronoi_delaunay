# Explications sur les containers list, vector, deque et map


## list

`list` est une structure de données chaînée, avec chaque élément lié au précédent et au suivant. Elle peut stocker les éléments dans des endroits de stockage mémoire différents.

### Avantages :
Les nouveaux éléments peuvent être insérés et supprimés à n'importe quel endroit de la liste. Il n'y a pas besoin de déplacer tous les autres éléments pour faire de la place.

### Inconvénients :
L'accès à un élément spécifique est assez lent, car il faut parcourir toute la liste pour trouver l'élément que l'on cherche. `list` utilise aussi plus de mémoire que d'autres containers pour lier les éléments entre eux.

### Exemple
```cpp
list<Ball> balls;

balls.push_back(Ball("rouge"));
balls.push_back(Ball("vert"));
balls.push_back(Ball("bleu"));


// Affichage de la liste de balles
for (const Ball &ball : balls) {
    cout << "Balle de couleur : " << ball.color << endl;
}
```
---
## vector

`vector` est un container semblable à un tableau `array`, mais avec une taille qui change automatiquement en fonction du nombre d'éléments qu'il contient. Tout comme les `arrays`, les `vectors` stockent les éléments dans des endroits de stockage mémoire les uns à la suite des autres.

### Avantages :

La taille maximale du `vector` évolue automatiquement en fonction du nombre d'éléments qu'il contient. L'accès à un élément spécifique est rapide, car il suffit de connaître le numéro de l'élément.

### Inconvénients :

Les nouveaux éléments ne peuvent être insérés qu'à la fin ou au début du `vector`. Ajouter ou suprimer un élément au milieu du nécessite de déplacer tous les éléments qui suivent, ce qui peut prendre du temps. La réallocation de mémoire automatique pour augmenter la taille max peut aussi prendre du temps.

### Exemple
```cpp
vector<string> recipeSteps;

// Création des étapes
recipeSteps.push_back("Préchauffer le four (à pizza) à 300°C minimum");
recipeSteps.push_back("Étaler la pâte");
recipeSteps.push_back("Ajouter les ingrédients");
recipeSteps.push_back("Surveiller prendant 3 à 5 minutes");

// Affichage
for (size_t i = 0; i < recipeSteps.size(); i++) {
    cout << i + 1 << " - " << recipeSteps[i] << endl;
}

// On modifie la 3ème étape
recipeSteps[2] = "Ajouter les ingrédients et enfourner";
```
---
## deque

`deque` (pour double-ended queue) est une liste de données, mais qui offre la possibilité d'ajouter et supprimer des éléments au début et à la fin grâce à une taille dynamique. `deque` offre beaucoup d'avantages de `vector`. Cependant, les éléments peuvent être stockés dans des endroits de stockage mémoire différents.

### Avantages :
Ajouter des éléments au début et à la fin du conteneur est à la fois facile et rapide.

### Inconvénients :
Nécessite une certaine quantité d'espace mémoire.


### Exemple
```cpp
deque<string> taskList;

// Ajout de tâches à faire
taskList.push_front("Voronoi");
taskList.push_back("Appeler la caf");
taskList.push_back("Courses");

// Affichage de la liste
cout << "Liste des tâches : " << endl;
for (const string &task : taskList) {
    cout << " - " << task << endl;
}
// Suppression de la première tâche
taskList.pop_front();
// Suppression de la dernière tâche
taskList.pop_back();
```
---
## map

`map` est un container dit "associatif", qui permet de stocker des données sous forme de paires clé-valeur. En donnant une clé, on peut récupérer la valeur associée. Les clés sont uniques, mais les valeurs peuvent être utilisées plusieurs fois.

### Avantages :

L'accès à une valeur spécifique est rapide, car il suffit de connaître la clé.

### Inconvénients :

`map` utilise beaucoup de mémoire, et est plus lent que les autres containers.

### Exemple
```cpp
map<string, double> notes;

// Ajout des notes moyennes par matière
notes["Anglais"] = 16.0;
notes["Droit"] = 5;
notes["Prog Algo"] = 17.0;

// Récupération d'une note associée à une matière
string matiere = "Prog Algo";

// Actualisation de la note
notes[matiere] = 19.5;

cout << "La note en " << matiere << " est : " << notes[matiere] << endl;