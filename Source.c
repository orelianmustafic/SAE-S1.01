// source.c - Groupe 77 - Orelian MUSTAFIC et Alexis SOLLOGOUB - 07/10/2022

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#pragma warning(disable: 4996)

enum {
	NB_SEMESTRES = 2,
	MIN_UE = 3,
	MAX_UE = 6,
	MAX_MATIERES = 10,
	MAX_EPREUVES = 5,
	MAX_ETUDIANTS = 100,
	MAX_NOTES = NB_SEMESTRES * MAX_MATIERES * MAX_EPREUVES
};
const float MIN_NOTE = 0.f;
const float MAX_NOTE = 20.f;


typedef struct {
    char nom[30];
	float coef[MAX_UE];
} Epreuve;

typedef struct {
	char nom[30];
	unsigned int nbEpreuves;
	Epreuve epreuves[MAX_EPREUVES];
} Matiere;

typedef struct {
	unsigned int nbMatieres;
	Matiere matieres[MAX_MATIERES];
} Semestre;

typedef struct {
	float note;
	unsigned int numSemestre;
	unsigned int numMatiere;
	unsigned int numEpreuve;
} Note;

typedef struct {
	char nom[30];
	unsigned int nbNotes[NB_SEMESTRES];
	Note notes[MAX_NOTES];
} Etudiant;

typedef struct {
	unsigned int nbUE; // nombre de coef, commun a toutes les epreuves
	unsigned int nbEtu;
	Semestre semestres[NB_SEMESTRES];
	Etudiant etudiants[MAX_ETUDIANTS];
} Formation;


void init(Formation* f);
void defFormation(Formation* f);
void ajoutEpreuve(Formation* f);
void verifCoefs(const Formation* f);
void ajoutNote(Formation* f);
void verifNotes(const Formation* f);
void releveNotes(Formation* f);
void decisionJury(const Formation* f);


int verifCoef(unsigned int nbUE, const Semestre* s);
int verifNote(const Formation* f, int numEtu, int numSemestre);

int existeMatiere(const char* t, const Semestre* s);
int existeEpreuve(const char* t, int numMatiere, const Semestre* s);
int existeEtudiant(const char* t, const Formation* f);
int existeNote(const Etudiant* e, unsigned int numSemestre, unsigned int numMatiere, unsigned int numEpreuve);

void moyennesSemestre(const Formation* f, int numEtu, int numSemestre, float moyennesTotal[], float coefsTotal[]);

int main() {
	char cmd[31] = "";
	Formation f;
	init(&f);

	do {
		scanf("%31s", cmd);

		if (strcmp(cmd, "formation") == 0) defFormation(&f);
		else if (f.nbUE < MIN_UE || f.nbUE > MAX_UE) printf("Le nombre d'UE n'est pas defini\n");
		else if (strcmp(cmd, "epreuve") == 0) ajoutEpreuve(&f);
		else if (strcmp(cmd, "coefficients") == 0) verifCoefs(&f);
		else if (strcmp(cmd, "note") == 0) ajoutNote(&f);
		else if (strcmp(cmd, "notes") == 0) verifNotes(&f);
		else if (strcmp(cmd, "releve") == 0) releveNotes(&f);
		else if (strcmp(cmd, "decision") == 0) decisionJury(&f);

		scanf("%*[^\n]");

	} while (strcmp(cmd, "exit") != 0);
}



void init(Formation* f) {
	f->nbEtu = 0;
	for (unsigned int i = 0; i < NB_SEMESTRES; ++i) {
		f->semestres[i].nbMatieres = 0;
		for (unsigned int j = 0; j < MAX_MATIERES; ++j) {
			f->semestres[i].matieres[j].nbEpreuves = 0;
		}
	}
}


/*
* Définition de la formation avec le nombre d'UE la composant
* [in] f, la formation
*/
void defFormation(Formation* f) {
	int nbUE;
    
    char str[30];
    fgets(str, 2, stdin);
    nbUE = strtol(str, NULL, 10);

	if (f->nbUE >= MIN_UE && f->nbUE <= MAX_UE)
		printf("Le nombre d'UE est deja defini\n");
	else if (nbUE < MIN_UE || nbUE > MAX_UE)
		printf("Le nombre d'UE est incorrect\n");
	else {
		f->nbUE = nbUE;
		printf("Le nombre d'UE est defini\n");
	}
}


/*
* Ajout d'une épreuve à une formation
* [in] f, la formation
*/
void ajoutEpreuve(Formation* f) {
	int numSemestre;
    int numMatiere;
    int numEpreuve;
	float coefUE;
	char nomMatiere[30];
    char nomEpreuve[30];

	scanf("%2d %30s %30s", &numSemestre, nomMatiere, nomEpreuve);
	numSemestre--;

	// Vérifie si le semstre est correct (entre 0 et 1)
	if (numSemestre < 0 || numSemestre >= NB_SEMESTRES) {
		printf("Le numero de semestre est incorrect\n");
		return;
	}

	Semestre* s = &f->semestres[numSemestre];

	// Boucle sur toutes les UE pour leur ajouter les coefs
	float tableau_UE[MAX_UE];
	for (unsigned int i = 0; i < f->nbUE; ++i) {
        char str[30];
        fgets(str, 2, stdin);
		coefUE = (float) strtol(str, NULL, 10);
		tableau_UE[i] = coefUE;
		if (coefUE < 0) {
			printf("Au moins un des coefficients est incorrect\n");
			return;
		}
	}

	// Vérifie qu'il y a au moins un coefficient supérieur à zéro
	float verifUE = 0;
	for (unsigned int i = 0; i < f->nbUE; ++i)
		verifUE += tableau_UE[i];

	if (verifUE == 0) {
		printf("Au moins un des coefficients est incorrect\n");
		return;
	}



	numMatiere = existeMatiere(nomMatiere, s);
	// Si la matière n'existe pas (-1) on la crée
	if (numMatiere == -1) {
		strcpy(s->matieres[s->nbMatieres].nom, nomMatiere);
		numMatiere = (int) s->nbMatieres;
		s->nbMatieres++;
		printf("Matiere ajoutee a la formation\n");
	}

	numEpreuve = existeEpreuve(nomEpreuve, numMatiere, s);
	// Vérifie si l'epreuve existe
	if (numEpreuve >= 0) {
		printf("Une meme epreuve existe deja\n");
		return;
	}
	numEpreuve = (int) s->matieres[numMatiere].nbEpreuves;

	// Ajoute le nom de la matière
	strcpy(s->matieres[numMatiere].epreuves[numEpreuve].nom, nomEpreuve);
	s->matieres[numMatiere].nbEpreuves++;


	// Ajoute les valeurs au tableau
    for (unsigned int i = 0; i < f->nbUE; ++i) s->matieres[numMatiere].epreuves[numEpreuve].coef[i] = tableau_UE[i];


    printf("Epreuve ajoutee a la formation\n");
}


/*
* Vérification de la validité des coefficients
* [in] f, la formation
*/
void verifCoefs(const Formation* f) {
	int numSemestre;

    char str[30];
    fgets(str, 2, stdin);
    numSemestre = strtol(str, NULL, 10) - 1;

	if (numSemestre < 0 || numSemestre >= NB_SEMESTRES)
		printf("Le numero de semestre est incorrect\n");
	else if (f->semestres[numSemestre].nbMatieres == 0)
		printf("Le semestre ne contient aucune epreuve\n");
	else if (verifCoef(f->nbUE, &f->semestres[numSemestre]))
		printf("Les coefficients d'au moins une UE de ce semestre sont tous nuls\n");
	else
		printf("Coefficients corrects\n");
}


/*
* Ajout d'une note à une formation
* [in] f, la formation
*/
void ajoutNote(Formation* f) {
	int numSemestre;
    int numEtu;
    int numMatiere;
    int numEpreuve;
	char nomEtu[30];
    char nomMatiere[30];
    char nomEpreuve[30];
	float note;

	scanf("%2d %30s %30s %30s %4f", &numSemestre, nomEtu, nomMatiere, nomEpreuve, &note);
	numSemestre--;

	if (numSemestre < 0 || numSemestre >= NB_SEMESTRES) {
		printf("Le numero de semestre est incorrect\n");
		return;
	}

	numMatiere = existeMatiere(nomMatiere, &f->semestres[numSemestre]);
	numEpreuve = existeEpreuve(nomEpreuve, numMatiere, &f->semestres[numSemestre]);

	if (numMatiere == -1)
		printf("Matiere inconnue\n");

	else if (numEpreuve == -1)
		printf("Epreuve inconnue\n");

	else if (note < MIN_NOTE || note > MAX_NOTE)
		printf("Note incorrecte\n");

	else {
		// Vérifie si l'étudiant existe, sinon, on l'ajoute
		numEtu = existeEtudiant(nomEtu, f);
		if (numEtu == -1) {
			numEtu = (int) f->nbEtu;
			f->nbEtu++;
			strcpy(f->etudiants[numEtu].nom, nomEtu);
			for (unsigned int i = 0; i < NB_SEMESTRES; ++i)
				f->etudiants[numEtu].nbNotes[i] = 0;
			printf("Etudiant ajoute a la formation\n");
		}

		Etudiant* e = &f->etudiants[numEtu];

		if (existeNote(e, numSemestre, numMatiere, numEpreuve) >= 0) {
			printf("Une note est deja definie pour cet etudiant\n");
			return;
		}

		unsigned int nbNotes = 0;
		for (unsigned int i = 0; i < NB_SEMESTRES; ++i) {
			nbNotes += e->nbNotes[i];
		}

		e->notes[nbNotes].note = note;
		e->notes[nbNotes].numSemestre = numSemestre;
		e->notes[nbNotes].numEpreuve = numEpreuve;
		e->notes[nbNotes].numMatiere = numMatiere;
		e->nbNotes[numSemestre]++;

		printf("Note ajoutee a l'etudiant\n");
	}
}


/*
* Vérification de la validité des notes
* [in] f, la formation
*/
void verifNotes(const Formation* f) {
	int numSemestre;
    int numEtu;
	char nomEtu[30];

	scanf("%2d %30s", &numSemestre, nomEtu);
	numSemestre--;
	numEtu = existeEtudiant(nomEtu, f);

	if (numSemestre < 0 || numSemestre >= NB_SEMESTRES)
		printf("Le numero de semestre est incorrect\n");
	else if (numEtu == -1)
		printf("Etudiant inconnu\n");
	else if (verifNote(f, numEtu, numSemestre))
		printf("Il manque au moins une note pour cet etudiant\n");
	else
		printf("Notes correctes\n");
}


/*
* Affichage d'un relevé de notes
* [in] f, la formation
*/
void releveNotes(Formation* f) {
	int numSemestre;
    int numEtu;
    int numNote;
	char nomEtu[30];


	scanf("%2d %30s", &numSemestre, nomEtu);
	numSemestre--;
	numEtu = existeEtudiant(nomEtu, f);


	if (numSemestre < 0 || numSemestre >= NB_SEMESTRES) {
        printf("Le numero de semestre est incorrect\n");
        return;
    }
	else if (numEtu == -1)  {
		printf("Etudiant inconnu\n");
        return;
    }
	else if (f->semestres[numSemestre].nbMatieres == 0 || verifCoef(f->nbUE, &f->semestres[numSemestre])) {
		printf("Les coefficients de ce semestre sont incorrects\n");
        return;
    }
	else if (verifNote(f, numEtu, numSemestre)) {
		printf("Il manque au moins une note pour cet etudiant\n");
        return;
    }

    Semestre* s = &f->semestres[numSemestre];
    Etudiant const* e = &f->etudiants[numEtu];
    unsigned int nbEspacement = 8;

    for (unsigned int i = 0; i < s->nbMatieres; ++i) {
        if (strlen(s->matieres[i].nom) > 8)
            nbEspacement = (int) strlen(s->matieres[i].nom);
    }

    for (unsigned int i = 0; i < nbEspacement; ++i) printf(" ");
    for (unsigned int i = 1; i <= f->nbUE; ++i) printf("  UE%u ", i);
    printf("\n");

    float notesMatieres[MAX_UE];
    float coefMatieres[MAX_UE];

    for (unsigned int i = 0; i < s->nbMatieres; ++i) {
        for (unsigned int l = 0; l < MAX_UE; ++l) {
            notesMatieres[l] = 0;
            coefMatieres[l] = 0;
        }

        for (unsigned int j = 0; j < s->matieres[i].nbEpreuves; ++j) {
            numNote = existeNote(e, numSemestre, i, j);
            for (unsigned int k = 0; k < f->nbUE; ++k) {
                notesMatieres[k] += s->matieres[i].epreuves[j].coef[k] * e->notes[numNote].note;
                coefMatieres[k] += s->matieres[i].epreuves[j].coef[k];
            }
        }

        printf("%s", s->matieres[i].nom);
        for (unsigned int w = (unsigned int) strlen(s->matieres[i].nom); w < nbEspacement; ++w) printf(" ");

        for (unsigned int m = 0; m < f->nbUE; ++m) {
            if (coefMatieres[m] == 0) printf("  ND  ");
            else printf(" %.1f ", floorf(notesMatieres[m] / coefMatieres[m] * 10.f) / 10.f);
        }
        printf("\n");
    }

    printf("--\n");
    printf("Moyennes");
    for (unsigned int i = 8; i < nbEspacement; ++i) printf(" ");

    float moyennesTotal[MAX_UE];
    float coefsTotal[MAX_UE];
    moyennesSemestre(f, numEtu, numSemestre, moyennesTotal, coefsTotal);

    for (unsigned int i = 0; i < f->nbUE; ++i) {
        printf(" %.1f ", floorf(moyennesTotal[i] * 10.f) / 10.f);
    }

    printf("\n");
}



/*
* Affichage de la décision de jury
* [in] f, la formation
*/
void decisionJury(const Formation* f) {
	int numEtu;
	char nomEtu[30];

	scanf("%30s", nomEtu);
	numEtu = existeEtudiant(nomEtu, f);

	if (numEtu == -1) {
		printf("Etudiant inconnu\n");
		return;
	}

	for (int numSemestre = 0; numSemestre < NB_SEMESTRES; ++numSemestre) {
		if (verifCoef(f->nbUE, &f->semestres[numSemestre])) {
			printf("Les coefficients d'au moins un semestre sont incorrects\n");
			return;
		}
		else if (verifNote(f, numEtu, numSemestre)) {
			printf("Il manque au moins une note pour cet etudiant\n");
			return;
		}
	}


	unsigned int nbEspacement = 18;
	for (unsigned int i = 0; i < nbEspacement; ++i) printf(" ");
	for (unsigned int i = 1; i <= f->nbUE; ++i) printf("  UE%u ", i);
	printf("\n");


	float moyennesAnnee[MAX_UE];
    float coefsAnnee[MAX_UE];
	for (unsigned int i = 0; i < MAX_UE; ++i) {
		moyennesAnnee[i] = 0;
		coefsAnnee[i] = 0;
	}

	for (int numSemestre = 0; numSemestre < NB_SEMESTRES; ++numSemestre) {
		float moyennesTotal[MAX_UE];
        float coefsTotal[MAX_UE];
		moyennesSemestre(f, numEtu, numSemestre, moyennesTotal, coefsTotal);

		printf("S%d", numSemestre + 1);
		for (unsigned int i = 2; i < nbEspacement; ++i) printf(" ");

		for (unsigned int i = 0; i < f->nbUE; ++i) {
			printf(" %.1f ", floorf(moyennesTotal[i] * 10.f) / 10.f);
			moyennesAnnee[i] += moyennesTotal[i];
			coefsAnnee[i] += coefsTotal[i];
		}
		printf("\n");
	}

	printf("--\n");
	printf("Moyennes annuelles");
	for (unsigned int i = 16; i < nbEspacement; ++i) printf(" ");

	for (unsigned int i = 0; i < f->nbUE; ++i) printf(" %.1f", floorf(moyennesAnnee[i] / NB_SEMESTRES * 10.f) / 10.f);
	printf("\nAcquisition ");
	for (unsigned int i = 11; i < nbEspacement; ++i) printf(" ");



	unsigned int passageUE[MAX_UE];
	for (unsigned int i = 0; i < f->nbUE; ++i) {
		if (floorf(moyennesAnnee[i] / NB_SEMESTRES * 10.f) / 10.f >= 10.) passageUE[i] = 1;
		else passageUE[i] = 0;
	}

	unsigned int compteurPassage = 0;
	for (unsigned int i = 0; i < f->nbUE; ++i)
		if (passageUE[i] == 1) compteurPassage++;

	if (compteurPassage == 0) printf("Aucune ");
	else {
		for (unsigned int i = 0, compteurVirgule = 0; i < f->nbUE; ++i) {
			if (passageUE[i] != 1) continue;

            printf("UE%d", i + 1);
            compteurVirgule++;

            if (compteurVirgule < compteurPassage) printf(", ");
		}
	}

	printf("\nDevenir ");
	for (unsigned int i = 7; i < nbEspacement; ++i) printf(" ");


	if (compteurPassage > floor(f->nbUE / 2.)) printf("Passage ");
	else printf("Redoublement ");

	printf("\n");
}




/*
* Recherche de l'emplacement d'une matière
* [in] t, le nom de la matière
* [in] s, le semestre en question
* [out] i ou -1, l'indice ou -1 pour « non trouvé »
*/
int existeMatiere(const char* t, const Semestre* s) {
	for (unsigned int i = 0; i < s->nbMatieres; ++i)
		if (strcmp(t, s->matieres[i].nom) == 0) return (int) i;
	return -1;
}

/*
* Recherche de l'emplacement d'une épreuve
* [in] t, le nom de l'épreuve
* [in] numMatiere, l'indice de la matière
* [in] s, le semestre en question
* [out] i ou -1, l'indice ou -1 pour « non trouvé »
*/
int existeEpreuve(const char* t, const int numMatiere, const Semestre* s) {
	for (unsigned int i = 0; i < s->matieres[numMatiere].nbEpreuves; ++i)
		if (strcmp(t, s->matieres[numMatiere].epreuves[i].nom) == 0) return (int) i;
	return -1;
}

/*
* Recherche de l'emplacement d'un étudiant
* [in] t, le nom de l'étudiant
* [in] f, la formation
* [out] i ou -1, l'indice ou -1 pour « non trouvé »
*/
int existeEtudiant(const char* t, const Formation* f) {
	for (unsigned int i = 0; i < f->nbEtu; ++i)
		if (strcmp(t, f->etudiants[i].nom) == 0) return (int) i;
	return -1;
}
/*
* Recherche de l'emplacement d'une note
* [in] e, l'étudiant
* [in] numSemestre, l'indice du semestre
* [in] numMatiere, l'indice de la matière
* [in] numEpreuve, l'indice de l'épreuve
* [out] i ou -1, l'indice ou -1 pour « non trouvé »
*/
int existeNote(const Etudiant* e, const unsigned int numSemestre, const unsigned int numMatiere, const unsigned int numEpreuve) {
	for (unsigned int i = 0; i < MAX_NOTES; ++i)
		if (e->notes[i].numSemestre == numSemestre && e->notes[i].numMatiere == numMatiere && e->notes[i].numEpreuve == numEpreuve)
			return (int) i;
	return -1;
}

/*
* Vérifie si on a au moins un coefficient positif par UE
* [in] nbUE, le nombre total d'UE de notre formation
* [in] s, le semestre en question
* [out] 1 ou 0, pour True ou False
*/
int verifCoef(const unsigned int nbUE, const Semestre* s) {
	for (unsigned int i = 0; i < nbUE; ++i) {
		float Coef = 0;
		for (unsigned int j = 0; j < s->nbMatieres; ++j) {
			for (unsigned int k = 0; k < s->matieres[j].nbEpreuves; ++k) {
				Coef += s->matieres[j].epreuves[k].coef[i];
			}
		}
		if (Coef == 0)
			return 1;
	}
	return 0;
}

/*
* Vérifie si le nombre de notes de l'étudiant correspond au nombre total d'épreuves du semestre
* [in] f, la formation
* [in] numEtu, l'indice de l'étudiant
* [in] numSemestre, le numéro du semestre
* [out] 1 ou 0, pour True ou False
*/
int verifNote(const Formation* f, const int numEtu, const int numSemestre) {
	unsigned int nbTotEpreuve = 0;
	for (unsigned int i = 0; i < f->semestres[numSemestre].nbMatieres; ++i)
		nbTotEpreuve += f->semestres[numSemestre].matieres[i].nbEpreuves;
	if (f->etudiants[numEtu].nbNotes[numSemestre] == nbTotEpreuve)
		return 0;
	return 1;
}


/*
* Calcule les moyennes et les coefficients totaux pour chaque UE d'un semestre donné
* [in] f, la formation
* [in] numEtu, l'indice de l'étudiant
* [in] numSemestre, le numéro du semestre
* [in-out] moyennesTotal, utilisé pour stocker les moyennes du semestre
* [in-out] coefsTotal, utilisé pour stocker la somme des coefs du semestre
*/
void moyennesSemestre(const Formation* f, const int numEtu, const int numSemestre, float moyennesTotal[], float coefsTotal[]) {

	Semestre const* s = &f->semestres[numSemestre];
	Etudiant const* e = &f->etudiants[numEtu];

	float moyennes[MAX_MATIERES][MAX_UE];
    float coefs[MAX_MATIERES][MAX_UE];
    float moyennesS[MAX_UE];
    float coefsS[MAX_UE];
	int numNote;

	for (unsigned int i = 0; i < MAX_UE; ++i) {
		moyennesTotal[i] = 0;
		coefsTotal[i] = 0;
	}

	for (unsigned int i = 0; i < MAX_UE; ++i) {
		moyennesS[i] = 0;
		coefsS[i] = 0;

	}

	for (unsigned int numMatiere = 0; numMatiere < s->nbMatieres; ++numMatiere) {
		for (unsigned int numUE = 0; numUE < MAX_UE; ++numUE) {
			moyennes[numMatiere][numUE] = 0;
			coefs[numMatiere][numUE] = 0;
		}

		for (unsigned int numEpreuve = 0; numEpreuve < s->matieres[numMatiere].nbEpreuves; ++numEpreuve) {
			numNote = existeNote(e, numSemestre, numMatiere, numEpreuve);
			for (unsigned int numUE = 0; numUE < f->nbUE; ++numUE) {
				moyennes[numMatiere][numUE] += e->notes[numNote].note * s->matieres[numMatiere].epreuves[numEpreuve].coef[numUE];
				coefs[numMatiere][numUE] += s->matieres[numMatiere].epreuves[numEpreuve].coef[numUE];
			}
		}

		for (unsigned int numUE = 0; numUE < f->nbUE; ++numUE) {
			if (coefs[numMatiere][numUE] != 0) {
				moyennes[numMatiere][numUE] = moyennes[numMatiere][numUE] / coefs[numMatiere][numUE];
				moyennesS[numUE] += moyennes[numMatiere][numUE] * coefs[numMatiere][numUE];
				coefsS[numUE] += coefs[numMatiere][numUE];
			}
		}
	}

	for (unsigned int numUE = 0; numUE < f->nbUE; ++numUE) {
		moyennesTotal[numUE] = moyennesS[numUE] / coefsS[numUE];
		coefsTotal[numUE] += coefsS[numUE];
	}
}