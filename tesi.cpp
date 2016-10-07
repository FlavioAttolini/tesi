# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h> 
# include <math.h>
//# include <iostream>
//# include <fstram>
# define l 50000

// dichiarazione variabili globali  
FILE *f; 
int m; // variabile delle righe del file 
int k; // variabile delle colonne del file 
char **S; // matrice che viene riempita con il file di input. Ha m righe e k+1 colonne 
char **B; // matrice che contiene le permutazioni ordinate della matrice S 
char **Q; // matrice che contiene le permutazioni corrette di B 
int **Interleave; // matrice che tiene in memoria l'ordinamento dei suffissi. La colonna che mi interessa è lultima. I[mX(k+1)][k+1] 

// dichiarazione funzioni 
void Conteggiomk (); // funzione che permette di contare quante sono le parole e le lettere del file  
void InizializzazioneS(); // funzione che inizializza la matrice S a tutti dollari 
void InizializzazioneB(); // funzione che inizializza B a tutti dollari 
void InizializzazioneQ(); // funzione che inizializza Q a tutti dollari 
void RiempimentoS(); //funzione che mette il contenuto del file nella matrice S 
void CostruzioneB_Q(); // funzione che si occupa di creare le matrici B e Q partendo da S usando una matrice di appoggio N[2][m] 
void CreazioneInterleave(); 
void skip_line(FILE *f); 

// main 
int main (int argc, char* argv[]) { 
	//dichiarazione variabili main 
	int i, j; 
	// chiamata della funzione conteggio 
    	f=fopen(argv[1],"r"); 
	Conteggiomk();  
	//chiusura del file 
	fclose (f); 
	S = (char **) calloc (m, sizeof (char *));
	for (i=0; i<m; i++){
		S[i] = (char *) calloc (k+1, sizeof (char)); 
	} 
	InizializzazioneS(); 
	B = (char **) calloc (m, sizeof (char *));
	for (i=0; i<m; i++){
		B[i] = (char *) calloc (k+1, sizeof (char)); 
	} 
	InizializzazioneB(); 
	Q = (char **) calloc (m, sizeof (char *));
	for (i=0; i<m; i++){
		Q[i] = (char *) calloc (k+1, sizeof (char)); 
	} 
	InizializzazioneQ(); 
	f=fopen(argv[1], "r"); 
	RiempimentoS(); 
	fclose(f); 
	CostruzioneB_Q(); 
	CreazioneInterleave();  
	// fine del programma
	return 0; 
}

// FUNZIONI 

void Conteggiomk () {
	
	// conto le parole e i caratteri e il numero delle lettere corrisponde al numero più alto di caratteri di tutte le stringhe 
	
	int parole, caratteri, max; // dichiarazione varibili locali 
	parole=max=0;  
	caratteri=0; 
	char s='a'; 
	if (f==NULL){
		fprintf (stderr, "errore nell'apertura del file "); 
	}  
	while (s!=EOF){ 
		s=fgetc(f); 
		if (s!=EOF){
			if ((s!='>') && (s!='\n')) {
				caratteri++; 
			}
			else {
				if (s=='\n'){
					if(max<caratteri){
						max=caratteri; 
					}
					caratteri=0; 
					parole++; 
				}  
				else {
					if (max<caratteri) {
						max=caratteri; 
					}
					skip_line(f); 
					caratteri=0; 
				}
			}
		}
				
	}
	// questo if serve per controllare se il numero di caratteri dell'ultima parola corrispondono al numero massimo di caratteri 
	if (caratteri > max){
		max=caratteri;
	}
	
	m=parole; 
	k=max; 
	
// fine funzione 
}

void InizializzazioneS() {
	//dichiarazione varibili locali 
	int i, j; 
	for (i=0; i<m; i++) {
		for (j=0; j<k+1; j++) {
			S[i][j] = '$'; 
		}
	}
//fine funzione 
}

void InizializzazioneB() {
	//dichiarazione varibili locali 
	int i, j; 
	for (i=0; i<m; i++) {
		for (j=0; j<k+1; j++) {
			B[i][j] = '$'; 
		}
	}
//fine funzione 
}

void InizializzazioneQ() {
	//dichiarazione varibili locali 
	int i, j; 
	for (i=0; i<m; i++) {
		for (j=0; j<k+1; j++) {
			Q[i][j] = '$'; 
		}
	}
//fine funzione 
}

void RiempimentoS() { 
	// dichiarazione varibili locali 
	char s='a'; 
	int i, j; 
	i=0; 
    	while (i<m) { 
		for (j=1; j<=k+1; j++) { 
			if ((s=fgetc(f)) != EOF) { 
				if (s=='>'){
					skip_line(f);
					j--; 
				}
				else { 
					if (s=='\n'){ // se trovo un a capo devo cambiare di riga per cui esco dal for e incremento i
						continue; 
					}
					else {
						S[i][j]=s; 
					}
				}
			}
		}
		i=i+1;
	}

// fine funzione 
} 

void CostruzioneB_Q() {
	int N[2][m]; 
	int i, j, w; 
	int La[m], Lc[m], Lg[m], Lt[m]; 
	int readN; // mi serve per tenere in memoria il valore letto da N 
	int indiceN; // mi serve per sapere se leggere da N0 o N1 (le righe)
	int indicecolonneB=0; 
	int indicerigheB=0; 
	int indiceA, indiceC, indiceG, indiceT; 
	int indicecolonneS; 
	char readB; // mi serve per tenere in memoria il carattere letto da B 
	char readS; // mi serve per tenere in memoria il carattere che vado a leggere da S 
	indiceN = indiceA = indiceC = indiceG = indiceT = 0;
	indicecolonneS = k;  
	
	// inizializzazione delle L a 0 
	for (i=0; i<m; i++) { 
		La[i]=Lc[i]=Lg[i]=Lt[i]=0; 
	}
	
	// inizializzazione N 
	for (i=0; i<2; i++) {
		for (j=0; j<m; j++) { 
			N[i][j]=j+1; 
		}
	}
	// primo passaggio ho B0 e Q0 (Q0 è già inizializzata con i dollari per cui non lo modifico) 
	// metto l'ultima colonna di S in B 
	for (i=0; i<m; i++) {
		B[i][0] = S[i][indicecolonneS]; 
	}
	indicecolonneS--; 
	j=1; 
	while (j<k+1) { 
		i=0; 
		indicerigheB = 0;
		indiceA = indiceC = indiceG = indiceT = 0;  
		while (i<m) { 
			readN = N[indiceN][i]; 
			readB = B[indicerigheB][indicecolonneB]; 
			if (readB == 'A') { 
				La[indiceA] = readN; 
				indiceA++; // quando userò indiceA successivamente devo ricordarmi di abbassarlo di 1 
			}
			else if (readB == 'C') { 
				Lc[indiceC] = readN; 
				indiceC++; 
			}
			else if (readB == 'G') {  
				Lg[indiceG] = readN; 
				indiceG++; 
			}
			else if (readB == 'T') { 
				Lt[indiceT] = readN; 
				indiceT++; 
			}
			else { 
				printf ("errore nel file.. usare ctrl+c per fermare l'esecuzione.."); 
			}
			i++;
			indicerigheB++;  
		}
		// ora vado a costruire Q e N (che se vale 0 viene messo a 1 e viceversa) 
		if (indiceN == 0) { 
			indiceN = 1; 
		}
		else {
			indiceN = 0; 
		}
		i=0;   
		for (w=0; w<indiceA; w++) { 
			if (La[w] != 0) { 
				Q[i][j] = 'A';
				N[indiceN][i] = La[w]; 
				i++; 
			}
		}
		for (w=0; w<indiceC; w++) {
			if (Lc[w] != 0) {
				Q[i][j] = 'C';
				N[indiceN][i] = Lc[w];  
				i++; 
			}
		}
		for (w=0; w<indiceG; w++) { 
			if (Lg[w] != 0) {
				Q[i][j] = 'G'; 
				N[indiceN][i] = Lg[w]; 
				i++; 
			}
		}
		for (w=0; w<indiceT; w++) {
			if (Lt[w] != 0) {
				Q[i][j] = 'T';
				N[indiceN][i] = Lt[w];  
				i++; 
			}
		} 
		indicecolonneB++; 
		// ho finito di costruire Q 
		// ora vado a costruire B 
		i = 0; 
		indicerigheB = 0;  
		while (i<m) { 
			readN = N[indiceN][i]; 
			readS = S[readN-1][indicecolonneS]; // già decrementato prima, devo ricordarmi di decrementarlo dopo  
			// readN - 1 perchè leggo il numero che va da 1 a 5 ma le mie righe partono da 0  
			B[indicerigheB][indicecolonneB] = readS;  
			i++;
			indicerigheB++;  
		} 
		indicecolonneS--;  
		j++; 
	} // fine while 

// fine funzione 
}

void CreazioneInterleave() {
	// dichiarazione variabili locali 
	int righematrice; 
	righematrice = m*(k+1);
	char **Qlp; 
	int *E; // viene trattata come un booleano
	int indiceE = 0;  
	int i, j, w, r; 
	int indicecolonneI = 0; 
	int indicerigheI = 0; 
	int indicerigheQlp; 
	int indicecolonneQlp; 
	int inizializzatoreI = 0; 
	int **C; 
	int La[righematrice], Lc[righematrice], Lg[righematrice], Lt[righematrice], L$[righematrice];
	int indiceA, indiceC, indiceG, indiceT, indice$ = 0; 
	int indicecolonneC = 0;
	int indicerigheC = 0; 
	int readI; 
	char readQ;
	char readB; 
	int boolE = 0; // vale 0 se E ha tutti uno 1 altrimenti. Serve per sapere quando posso interrompermi per risparmiare iterazioni    
	indicecolonneI = indicerigheI = indicerigheQlp = 0;  
	E = (int *) calloc (righematrice, sizeof (int));
	Interleave = (int **) calloc (righematrice, sizeof (int *));
	for (i=0; i<righematrice; i++){
		Interleave[i] = (int *) calloc (k+1, sizeof (int)); 
	} 
	Qlp = (char **) calloc (righematrice, sizeof (char *));
	for (i=0; i<righematrice; i++){
		Qlp[i] = (char *) calloc (k+1, sizeof (char)); 
	} 
	C = (int **) calloc (m, sizeof (int *));
	for (i=0; i<righematrice; i++){
		C[i] = (int *) calloc (k+1, sizeof (int)); 
	}
	// inizializzazione Interleave 
	for (i=5; i<righematrice; i++) { 
		if ((i%m) == 0) { 
			inizializzatoreI++; 
		} 
		for (j=0; j<k+1; j++) { 
			Interleave[i][j] = inizializzatoreI; 
		}
	}
	// inizializzazione E 
	for (i=0; i<righematrice; i++) { 
		E[i] = 0; 
	}

	for (i=0; i<righematrice; i++) { 
		for (j=0; j<k+1; j++) { 
			Qlp[i][j] = '$'; 
		} 
	}
	// inizializzazione delle prime righe della matrice Qlp che corrispondono alla matrice Q creata prima 
	while (indicerigheQlp < m) { 
		for (j=0; j<k+1; j++) { 
			Qlp[indicerigheQlp][j] = Q[indicerigheQlp][j]; // ricordarsi che Q è in verticale per cui devo mantenere l'ordine 
			// verticale per ogni cosa 
		}
		indicerigheQlp++; 
	} 	

	for (i=0; i<m; i++) { 
		for (j=0; j<k+1; j++) { 
			C[i][j] = 0; 
		}
	} 
	
	for (i=0; i<righematrice; i++) { 
		La[i] = Lc[i] = Lg[i] = Lt[i] = L$[i] = 0; 
	}  

	j=0; 
	while (j<k+1) { 
		i=0; 
		for (w=0; w<righematrice; w++) { 
			L$[w] = La[w] = Lc[w] = Lg[w] = Lt[w] = 0; 
		} 
		for (w=0; w<m; w++) { 
			for (r=0; r<k+1; r++) { 
				C[i][r] = 0; 
			}
		}
		indiceA = indiceC = indiceG = indiceT = indice$ = 0; 
		indiceE = 0; 
		while (i<righematrice) {
			if (E[i] == 1)  
				i++; 
			else { 
				while (E[i] == 0) { 
					readI = Interleave[i][indicecolonneI];
					indicecolonneC = readI; 
					readQ = Qlp[indicerigheC][indicecolonneC]; 
					if (readQ == '$') { 
						L$[indice$] = readI; 
						printf ("questo e' $: %d \n", L$[indice$]); 
						indice$++; 
					} 
					else if (readQ == 'A') { 
						La[indiceA] = readI;
						printf ("questo e' A: %d \n", La[indiceA]);  
						indiceA++; 
					}
					else if (readQ == 'C') { 
						Lc[indiceC] = readI; 
						printf ("questo e' C: %d \n", Lc[indiceC]); 
						indiceC++; 
					}
					else if (readQ == 'G') {  
						Lg[indiceG] = readI; 
						printf ("questo e' G: %d \n", Lg[indiceG]); 
						indiceG++; 
					}
					else if (readQ == 'T') { 
						Lt[indiceT] = readI; 
						printf ("questo e' T: %d \n", Lt[indiceT]); 
						indiceT++; 
					}
					else { 
						printf ("errore nel file.. usare ctrl+c per fermare l'esecuzione.."); 
					} 
					C[indicerigheC][indicecolonneC]++; 
					i++;
					indicerigheC = i%m; 
				}// fine while 
			}// fine else 
			for (w=0; w<indice$; w++) {
				E[indiceE] = 1;  
				Interleave[indiceE][indicecolonneI+1] = L$[w]; 
				indiceE++; 
			}
			for (w=0; w<indiceA; w++) { 
				if (La[w]!=0) {
					if (w==0) 
						E[indiceE] = 1; 
					Interleave[indiceE][indicecolonneI+1] = La[w]; 
					indiceE++; 
				} 
			}
			for (w=0; w<indiceC; w++) { 
				if (Lc[w]!=0) {
					if (w==0) 
						E[indiceE] = 1; 
					Interleave[indiceE][indicecolonneI+1] = Lc[w]; 
					indiceE++; 
				} 
			}
			for (w=0; w<indiceG; w++) { 
				if (Lg[w]!=0) {
					if (w==0)  
						E[indiceE] = 1; 
					Interleave[indiceE][indicecolonneI+1] = Lg[w]; 
					indiceE++; 
				} 
			}
			for (w=0; w<indiceT; w++) { 
				if (Lt[w]!=0) {
					if (w==0) 
						E[indiceE] = 1; 
					Interleave[indiceE][indicecolonneI+1] = Lt[w]; 
					indiceE++; 
				} 
			}
		} // fine while righematrice 
		for (w=0; w<righematrice; w++) { 
			printf ("%d ", E[w]); 
		}
		printf ("\n"); 
		indice$ = indiceA = indiceC = indiceG = indiceT = 0;   
		for (w=0; w<righematrice; w++) { 
			L$[w] = La[w] = Lc[w] = Lg[w] = Lt[w] = 0; 
		}  
		indicecolonneQlp = j; 
		while (indicecolonneQlp<k+1) { 
			while ((indicerigheQlp%m) < m) {  // non funziona questo while 
				printf ("%d ", indicerigheQlp-m); 
				/*readQ = Qlp[indicerigheQlp-m][indicecolonneQlp];// sbagliata   
				printf ("%c ", readQ); 
				getchar(); */
				indicerigheQlp++; 
			}// fine while  
		indicecolonneQlp++; 
		} // fine while
		indicecolonneI++; 
		j++;
	} // fine while 
		 
// fine funzione 
}
	

void skip_line(FILE *f) {
	while(getc(f)!='\n')
	;
}


