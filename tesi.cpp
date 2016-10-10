# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <ctype.h> 
# include <math.h>
#include <sys/time.h>
//# include <iostream>
//# include <fstram>
# define l 50000

// ATTENZIONE! IL FILE DEVE ESSERE SENZA A CAPO SULL'ULTIMA RIGA. DEVE TERMINARE CON IL PUNTATORE DOPO L'ULTIMA LETTERA 

// dichiarazione variabili globali  
FILE *f; 
int m=0; // variabile delle righe del file 
int k=0; // variabile delle colonne del file 
char **S; // matrice che viene riempita con il file di input. Ha m righe e k+1 colonne 
char **B; // matrice che contiene le permutazioni ordinate della matrice S 
char **Q; // matrice che contiene le permutazioni corrette di B 
int **Interleave; // matrice che tiene in memoria l'ordinamento dei suffissi. La colonna che mi interessa è lultima. I[mX(k+1)][k+1] 
int *C; // è un array di contatori che ha dimensione C[k+1] 
char **Qlp; // è la matrice Q. Ci sono questa matrice e quella Q perchè quella Q viene usata come inizializzatore  
int *E; // viene trattata come un booleano
int righematrice; // tiene memoria di quante righe deve avere la matrice. Viene utilizzata anche per E, Interleave e BWT. Valore: m*(k+1)
int indicecolonnafinaleI; // mi serve per salvare la colonna con la quale mi fermo alla creazione dell'Interleave, cioè quando E[i] ha tutti 1 
char *BWT;  // Vettore contenente le permutazioni finlali di tutti i caratteri 
timeval start, stop; 
double elapsedTime; 

// dichiarazione funzioni 
void Conteggiomk (); // funzione che permette di contare quante sono le parole e le lettere del file  
void InizializzazioneS(); // funzione che inizializza la matrice S a tutti dollari 
void InizializzazioneB(); // funzione che inizializza B a tutti dollari 
void InizializzazioneQ(); // funzione che inizializza Q a tutti dollari 
void RiempimentoS(); //funzione che mette il contenuto del file nella matrice S 
void CostruzioneB_Q(); // funzione che si occupa di creare le matrici B e Q partendo da S usando una matrice di appoggio N[2][m] 
void CreazioneInterleave(); // funzione che crea E e Interleave 
void CreazioneBWT(); // funzione che partendo da E e Interleave fa la permutazione finale dei caratteri generando la BWT 
void skip_line(FILE *f); // funzione che mi fa saltare la linea del file quando leggo il carattere '>' 

// main 
int main (int argc, char* argv[]) { 
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
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
	righematrice = m*(k+1);
	CreazioneInterleave();  
	CreazioneBWT();
	printf ("Trasformazione eseguita con successo. Stampa su file in corso.. ");   
	f = fopen (argv[2], "w"); 
	for (i=0; i<righematrice; i++) { 
		fprintf (f, "%c ", BWT[i]); 
	} 
	fclose(f); 
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\nil tempo in millisecondi e': %f \n", elapsedTime); 
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
		if ((s!='>') && (s!='\n')) {
			caratteri++; 
		}
		else {				
			if (s=='\n'){
				if(max<caratteri)
					max=caratteri; 
				caratteri=0; 
				parole++; 
				}  
			else {	
				if (max<caratteri) 
					max=caratteri; 
				skip_line(f); 
				caratteri=0; 
			}
		}
				
	}
	// questo if serve per controllare se il numero di caratteri dell'ultima parola corrispondono al numero massimo di caratteri 
	if (caratteri > max) 
		max=caratteri;
	
	m=parole; 
	k=max; 
	
// fine funzione 
}

void InizializzazioneS() { // inizializzo S a tutti $ 
	//dichiarazione varibili locali 
	int i, j; 
	for (i=0; i<m; i++) {
		for (j=0; j<k+1; j++) {
			S[i][j] = '$'; 
		}
	}
//fine funzione 
}

void InizializzazioneB() { // inizializzo B a tutti $ 
	//dichiarazione varibili locali 
	int i, j; 
	for (i=0; i<m; i++) {
		for (j=0; j<k+1; j++) {
			B[i][j] = '$'; 
		}
	}
//fine funzione 
}

void InizializzazioneQ() { // inizializzo la prima matrice Q a tutti $ 
	//dichiarazione varibili locali 
	int i, j; 
	for (i=0; i<m; i++) {
		for (j=0; j<k+1; j++) {
			Q[i][j] = '$'; 
		}
	}
//fine funzione 
}

void RiempimentoS() { // costruisco S leggendo i caratteri da file 
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
		i++;
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
	int indiceE = 0; // è l'indice che mi permette di tenere traccia della posizione su E 
	int i, j, w, r; // variabili contatore utilizzate nei for e while 
	int indicecolonneI = 0; // mi tiene traccia della posizione di I nelle colonne  
	int indicerigheQlp; // mi dice a quale riga sono in Qlp 
	int indicecolonneQlp; // mi dice a quale colonna devo fare riferimento nella costruzione di Qlp 
	int inizializzatoreI = 0; // mi serve per inizializzare l'Interleave 
	int La[righematrice], Lc[righematrice], Lg[righematrice], Lt[righematrice], L$[righematrice]; // memorizzano il valore di I 
	char Lca[m], Lcc[m], Lcg[m], Lct[m]; // memorizzano i caratteri di Qlp 
	int indiceA, indiceC, indiceG, indiceT, indice$ = 0; // mi servono per memorizzare la posizione delle liste L e Lc 
	int readI; // tiene traccia del valore letto da I 
	char readQ; // tiene traccia del valore letto da Qlp 
	char readB; // tiene traccia del valore letto da B 
	int boolE = 0; // vale 0 se E ha tutti uno 1 altrimenti. Serve per sapere quando posso interrompermi per risparmiare iterazioni    
	indicecolonneI = indicerigheQlp = 0;  
	E = (int *) calloc (righematrice, sizeof (int));
	Interleave = (int **) calloc (righematrice, sizeof (int *));
	for (i=0; i<righematrice; i++){
		Interleave[i] = (int *) calloc (k+1, sizeof (int)); 
	} 
	Qlp = (char **) calloc (righematrice, sizeof (char *));
	for (i=0; i<righematrice; i++){
		Qlp[i] = (char *) calloc (k+1, sizeof (char)); 
	} 
	C = (int *) calloc (k+1, sizeof (int));
	// inizializzazione Interleave 
	for (i=m; i<righematrice; i++) { 
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

	// inizializzazione array C 
	for (j=0; j<k+1; j++) { 
		C[j] = 0; 
	}
	
	//inizializzazione liste L 
	for (i=0; i<righematrice; i++) { 
		La[i] = Lc[i] = Lg[i] = Lt[i] = L$[i] = 0; 
	}  

	// creazione Interleave 
	j=0; 
	while (j<k) { 
		i=0; 
		// inizializzazione a ogni passaggio delle liste, dei contatori e degli indici 
		for (w=0; w<righematrice; w++) { 
			L$[w] = La[w] = Lc[w] = Lg[w] = Lt[w] = 0; 
		} 
		indiceA = indiceC = indiceG = indiceT = indice$ = 0; 
		indiceE = 0; 
		while (i<righematrice) {
			if (E[i]==1) { 
				// svuoto le liste e metto i valori letti (se ci sono) in Interleave per ordinare in maniera lessicografica 
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
				// ho svuotato le liste ora le inizializzo nuovamente a 0 
				for (w=0; w<righematrice; w++) { 
					L$[w] = La[w] = Lc[w] = Lg[w] = Lt[w] = 0; 
				} 
				indiceA = indiceC = indiceG = indiceT = indice$ = 0; 
				// ora vado a leggere il carattere e lo metto nella lista giusta 
				readI = Interleave[i][indicecolonneI];
				readQ = Qlp[C[readI]][readI]; 
				if (readQ == '$') { 
					L$[indice$] = readI;
					indice$++; 
				} 
				else if (readQ == 'A') { 
					La[indiceA] = readI;  
					indiceA++; 
				}
				else if (readQ == 'C') { 
					Lc[indiceC] = readI; 
					indiceC++; 
				}
				else if (readQ == 'G') {  
					Lg[indiceG] = readI; 
					indiceG++; 
				}
				else if (readQ == 'T') { 
					Lt[indiceT] = readI; 
					indiceT++; 
				}
				C[readI]++; 
				i++;
			}// fine if 
			else { 
				// devo solamente mettere il carattere nel posto giusto 
				readI = Interleave[i][indicecolonneI]; 
				readQ = Qlp[C[readI]][readI]; 
				if (readQ == '$') { 
					L$[indice$] = readI; 
					indice$++; 
				} 
				else if (readQ == 'A') { 
					La[indiceA] = readI;
					indiceA++; 
				}
				else if (readQ == 'C') { 
					Lc[indiceC] = readI; 
					indiceC++; 
				}
				else if (readQ == 'G') {  
					Lg[indiceG] = readI; 
					indiceG++; 
				}
				else if (readQ == 'T') { 
					Lt[indiceT] = readI;
					indiceT++; 
				}
				else { 
					printf ("errore nel file.. usare ctrl+c per fermare l'esecuzione.."); 
				} 
				C[readI]++; 
				i++; 
			}// fine else 
		} // fine while righematrice 
		// ora che è finito il while svuoto le ultime liste o, al primo passaggio, tutte le liste 
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
		// costruisco Qlp 
		indicecolonneQlp = j; 
		while (indicecolonneQlp<(k)) { 
			// inizializzo a ogni passaggio le liste e i contatori 
			indice$ = indiceA = indiceC = indiceG = indiceT = 0;   
			for (w=0; w<m; w++) { 
				Lca[w] = Lcc[w] = Lcg[w] = Lct[w] = ' '; 
			}  
			i = indicerigheQlp;
			w=0;  
			while (i < (m+indicerigheQlp)) {
				// leggo Qlp e B e li inserisco nella lista giusta   
				readQ = Qlp[i-m][indicecolonneQlp];  
				readB = B[w][indicecolonneQlp]; 
				if (readB == 'A') { 
					Lca[indiceA] = readQ;
					indiceA++; 
				} 
				if (readB == 'C') { 
					Lcc[indiceC] = readQ;  
					indiceC++; 
				}
				if (readB == 'G') { 
					Lcg[indiceG] = readQ;   
					indiceG++; 
				} 
				if (readB == 'T') { 
					Lct[indiceT] = readQ;   
					indiceT++; 
				}
				w++; 
				i++; 
			}// fine while 
			w=indicerigheQlp; 
			while (w<(m+indicerigheQlp)) { 
				// svuoto le liste e metto in ordine Qlp  
				for (r=0; r<indiceA; r++) { 
					Qlp[w][indicecolonneQlp+1] = Lca[r]; 
					w++; 
				} 
				for (r=0; r<indiceC; r++) { 
					Qlp[w][indicecolonneQlp+1] = Lcc[r]; 
					w++; 
				} 
				for (r=0; r<indiceG; r++) { 
					Qlp[w][indicecolonneQlp+1] = Lcg[r]; 
					w++; 
				} 
				for (r=0; r<indiceT; r++) { 
					Qlp[w][indicecolonneQlp+1] = Lct[r]; 
					w++; 
				} 
			}// fine while 
			indicecolonneQlp++;  
		} // fine while
		indicerigheQlp+=m; 
		indicecolonneI++; 
		// qui metto la condizione perchè possa uscire anticipatamente dopo aver controllato che E abbia tutti 1 al suo interno
		indicecolonnafinaleI = indicecolonneI;  
		boolE = 1; 
		for (w=0; w<righematrice; w++) { 
			if (E[w] == 0) 
				boolE = 0; 
		}
		if (boolE == 1) 
			break; 
		j++;
	} // fine while  
		 
// fine funzione 
}

void CreazioneBWT() { 
	int i; 
	int readI; 
	char readB; 
	BWT = (char *) calloc (righematrice, sizeof (char));
	for (i=0; i<k+1; i++) { 
		C[i] = 0; 
	} 
	i=0; 
	while (i<righematrice) { 
		readI = Interleave[i][indicecolonnafinaleI]; 
		readB = B[C[readI]][readI]; 
		BWT[i] = readB; 
		i++; 
		C[readI]++; 
	}
		
}//fine funzione 
	

void skip_line(FILE *f) {
	while(getc(f)!='\n')
	;
}


