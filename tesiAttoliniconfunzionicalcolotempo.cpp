# include <stdio.h>
# include <stdlib.h>
# include <string.h>
#include <ctype.h> 
#include <sys/time.h>
#include <math.h> 
//# include <iostream>
//# include <fstram>
# define l 50000

// dichiarazione variabili globali  
FILE *f; 
int nparole, nlettere, righen; 
char **S; // matrice che contiene le stringhe lette da file 
char *Q; // array di appoggio che serve per l'ordinamento di B 
char **B; // matrice che contiene le stringhe di S ordinate 
char **N; // matrice che contiene i suffissi di B 
int *Interleave; 
int *End; 
char *BWT;
int i,j;  // indici 
timeval start, stop; 
double elapsedTime; 

// dichiarazione funzioni 
void conteggio (); // funzione che permette di contare quante sono le parole e le lettere del file 
void inizializzazioneS(char **S); 
void riempimentoS (char **S); 
void inizializzazioneN (char **N); 
void inizializzazioneQ (char *Q); 
void inizializzazioneB (char **B); 
void creazioneB (char **B); 
void creazioneN (char **N, char **B); 
void inizializzazioneInterleave (int *Interleave); 
void inizializzazioneEnd (int *End); 
void creazioneInterleave (int *Interleave, int *End); 
void creazioneBWT (char *BWT); 
void skip_line(FILE *f); 

// main 
int main (int argc, char* argv[]) { 
	// registrazione tempo di avvio 
	//gettimeofday (&start,NULL); 
	// chiamata della funzione conteggio 
    	f=fopen(argv[1],"r"); 
	conteggio();  
	//chiusura del file 
	fclose (f); 
	S = (char **) calloc (nparole, sizeof (char *));
	for (i=0; i<nparole; i++){
		S[i] = (char *) calloc (nlettere, sizeof (char)); 
	} 
	inizializzazioneS(S);
    	f=fopen(argv[1],"r"); 
    	riempimentoS (S); 
    	fclose (f); 
    	Q = (char *) calloc (nparole, sizeof (char)); // creazione di Q vettore di lunghezza nparole
    	B = (char **) calloc (nparole, sizeof (char *)); // creazione di B di lunghezza nlettere e largo nparole 
	for (i=0; i<nparole; i++){
		B[i] = (char *) calloc (nlettere, sizeof (char)); 
	} 
	righen=nparole*nlettere; 
	N = (char **) calloc (righen, sizeof (char *)); // creazione di N di lunghezza nlettere e largo nparole 
	for (i=0; i<righen; i++){
		N[i] = (char *) calloc (nlettere, sizeof (char)); 
	} 
	inizializzazioneQ (Q);  
	inizializzazioneB (B); 
	creazioneB (B); 
	creazioneN (N,B); 
	Interleave = (int *) calloc (righen, sizeof (int)); //creazione Interleave di righen 
	inizializzazioneInterleave(Interleave); 
	End = (int *) calloc (righen, sizeof (int));
	inizializzazioneEnd (End); 
	creazioneInterleave(Interleave, End); 
	BWT = (char *) calloc (righen, sizeof (char));
	creazioneBWT (BWT); 
	printf ("trasformazione eseguita con successo.. risultato stampato su file.. \n"); 
	f=fopen("testoTesi.txt","w"); 
	for (i=0; i<righen; i++){
		fprintf (f,"%c ", BWT[i]); 
	}
	fclose(f); 
	
	// tempo finale 
	//gettimeofday (&stop,NULL); 

	//elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	//printf ("\n il tempo in millisecondi e': %f \n", elapsedTime); 
	
	// fine del programma
	return 0; 
}

// FUNZIONI 

void conteggio () {
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	// conto le parole e i caratteri e il numero delle lettere corrisponde al numero più alto di caratteri di tutte le stringhe 
	
	int parole, caratteri, max; // dichiarazione varibili locali 
	parole=max=0;  
	caratteri=0; 
	char s='a'; 
	if (f==NULL){
		fprintf (stderr, "errore nell'apertura del file "); 
	}  
	// in questo while si contano le parole quando si incontra il carattere $ e si contano tutte le lettere diverse dal carattere vuoto 
	while (s!=EOF){ 
		s=fgetc(f); 
		if (s!=EOF){
			if (s!='>') {
				caratteri=caratteri+1; 
			}
			else {
				if(max<caratteri){
					max=caratteri; 
				}
				skip_line(f);
				caratteri=0; 
				parole=parole+1;  
			}
		}
				
	}
	// questo if serve per controllare se il numero di caratteri dell'ultima parola corrispondono al numero massimo di caratteri 
	if (caratteri > max){
		max=caratteri;
	}
	
	nparole=parole; 
	nlettere=max; 
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi di conteggio e': %f \n", elapsedTime); 
	
}

void inizializzazioneS (char **S) {
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	for (int i=0; i<nparole; i++){
		for (int j=0; j<nlettere; j++){
			S[i][j]='$'; 
		}
	}
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi dell'inizializzazione di S e': %f \n", elapsedTime); 
}

void riempimentoS (char **S){
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	char s; // dichiarazione array di appoggio per la lettura da file 
	i=0; 
    	while (i<nparole){ 
		for (j=1; j<=nlettere; j++){ 
			if ((s=fgetc(f))!= EOF){ 
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
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi del riempimento di S e': %f \n", elapsedTime); 
}

void inizializzazioneN (char **N){
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	for (i=0; i<righen; i++){
		for (j=0; j<nlettere; j++){
			N[i][j]='$'; 
		}
	}
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi dell'inizializzazione di N e': %f \n", elapsedTime); 
}

void inizializzazioneQ (char *Q){
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	for (i=0; i<nparole; i++) {
		Q[i]='$'; 
	}
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi dellinizializzazione di Q e': %f \n", elapsedTime); 
}

void inizializzazioneB (char **B){
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	for (i=0; i<nparole; i++){
		for (j=0; j<nlettere; j++){
			B[i][j]='$'; 
		}
	}
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi dell'inizializzazione di B e': %f \n", elapsedTime); 
}

void creazioneB (char **B){
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	int w=0; 
	int k=nlettere-1; 
	int indicearrayn=nparole;  
	char appoggio; 
	int appoggiotraccia;  
	// mi serve un array che mi mantenga traccia delle posizioni che ho spostato 
	int Traccia[nparole]; // dentro ci vanno i valori delle posizioni per questo mi serve di interi e non di caratteri 
	// inizializzo Traccia 
	for (i=0; i<nparole; i++){
		Traccia[i]=i; 
	}
	// ora parto con la costruzione dell'array B e Q con 'aiuto di traccia 
	while (w<nlettere){
		if (k==nlettere-1){
			// metto l'ultima l'ultima colonna delle stringhe lette da file nella prima colonna di B ma solo al primo passaggio 
			for (i=0; i<nparole; i++){ 
				B[i][w]=S[i][k]; 
				Q[i]=B[i][w];
			}
			// metto B in tutta la matrice escluse le righe che corrispondono al numero di parole  
		}
		else {
			// ora devo utilizzare Traccia come memoria per odinare lessicograficamente Q in modo da predere le giuste lettere da S
			// per ordinare lessicofraficamente so che se una lettera viene prima ha valore ASCII minore di quella che viene dopo 
			// ordino il Q preso al passo precedente
			i=0; 
			while (i<nparole-1){ // -1 perche altrimenti supererei la lunghezza dell'array Q 
				if ((int)Q[i]>(int)Q[i+1]){ // allora scambio 
					appoggio=Q[i]; 
					Q[i]=Q[i+1]; 
					Q[i+1]=appoggio; 
					// ora devo tenere traccia delle posizioni scambiate in Traccia 
					appoggiotraccia=Traccia[i]; 
					Traccia[i]=Traccia[i+1]; 
					Traccia[i+1]=appoggiotraccia; 
					//printf ("\n %d \n %d", Traccia[i], Traccia[i+1]); 
					i=0; // lo faccio ripartire da 0 perchè altrimenti non ordina in maniera corretta termina quando è tutto ordinato e quindi l'if non vale più 
				}
				else {
					i++; 
				}
			}
			// adesso ho ordinato tutto Q per cui devo andare a prendere i caratteri di S e in posizione Traccia[i]; k
			for (i=0; i<nparole; i++){ 
				B[i][w]=S[Traccia[i]][k]; 
				Q[i]=B[i][w];
			}
			// ora il procedimento riparte da capo 
		}
		w++; 
		k--; 
		indicearrayn=indicearrayn+nparole; 
	} 
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi dellza creazione di B e': %f \n", elapsedTime); 
}

void creazioneN (char **N, char **B){
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	int indicearrayn=nparole; 
	int w=0; 
	int k=nlettere-1; 
	int y;
	int TracciaN[nparole]; 
	int appoggiotraccian; 
	char VettoreAppoggioN[nlettere]; 
	char QN[nparole]; 
	char appoggioqn; 
	// inizializzo TracciaN 
	for (i=0; i<nparole; i++){
		TracciaN[i]=i; 
	}
	// iniazializzo QN 
	for (i=0; i<nparole; i++) {
		QN[i]='$'; 
	}
	while (indicearrayn<righen){
		// al primo passaggio 
		if (indicearrayn==nparole){
			i=nparole;
			while (i<righen){
				for (j=0; j<nparole; j++){
					N[i][0]=B[j][w]; 
					QN[j]=B[j][w]; 
					i++; 
				}
			}
		}
		else {
			// qui ho fatto il primo passaggio quindi ora utilizzando Q e Traccia come prima vado a mettere in ordine qullo che mi interessa 
			// devo però mettere in ordine QN 
			// faccio lo shift per farci stare la nuova colonna 
			for (i=0; i<nparole; i++){
				QN[i]=B[i][w]; 
			}
			// quando scambio le lettere in Q devo anche scambiare le righe della matrice 
			i=0; 
			while (i<nparole-1){ // -1 perche altrimenti supererei la lunghezza dell'array QN  
				if ((int)QN[i]>(int)QN[i+1]){ // allora scambio 
					appoggioqn=QN[i]; 
					QN[i]=QN[i+1]; 
					QN[i+1]=appoggioqn; 
					for (y=0; y<nlettere; y++){
						VettoreAppoggioN[y]=N[indicearrayn+i][y]; 
						N[indicearrayn+i][y]=N[indicearrayn+i+1][y]; 
						N[indicearrayn+i+1][y]=VettoreAppoggioN[y]; 
					}
					i=0; // lo faccio ripartire da 0 perchè altrimenti non ordina in maniera corretta termina quando è tutto ordinato e quindi l'if non vale più
				}
				else {
					i++;
				}
			}
			// qui sto copiando ogni nparole il contenuto di ogni riga 
			for (y=indicearrayn+nparole; y<righen; y++){
				for (j=0; j<nlettere; j++){
					N[y][j]=N[y-nparole][j]; 
				}
			}
			// qua sto facendo traslare tutte le lettere di una posizione in modo tale che abbia la prima colonna libera da indicearrayn in poi in modo tale
			// che possa inserire la nuova colonna all'inizio 
			for (i=indicearrayn; i<righen; i++){
				for (j=nlettere-1; j>=0; j--){
					N[i][j]=N[i][j-1]; 
				}
			}
			// inserisco la nuova colonna 
			i=indicearrayn; 
			while (i<righen){
				for (j=0; j<nparole; j++){
					N[i][0]=QN[j];
					i++; 
				}
			}	
		}
		w++; 
		k--; 
		indicearrayn=indicearrayn+nparole; 
	}
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi della creazione di N e': %f \n", elapsedTime); 
}

void inizializzazioneInterleave (int *Interleave) {
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	int valoredaassegnare=0; 
	i=0; 
	while (i<righen){
		Interleave[i]=valoredaassegnare; 
		i++; 	
		if (i%nparole==0){
			valoredaassegnare++; 
		}
	} 
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi dell'inizializzazione dell'interleave e': %f \n", elapsedTime); 
}

void inizializzazioneEnd (int *End){
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	for (i=0; i<nparole; i++){
		End[i]=1; 
	}
	for (i=nparole; i<righen-1; i++){
		End[i]=0; 
	}
	End[i]=1; 
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi dell'inizializzazione di End e': %f \n", elapsedTime); 
}

void creazioneInterleave (int *Interleave, int *End){
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	char VettoreAppoggioN[nlettere]; 
	int controlloend=0; 
	int k=0; 
	int appoggioi; 
	while (controlloend==0){
		// qui devo scamabiare le righe della matrice N, scambiare i valori dell'Interleave quando cambiano i valori di N 
		// successivamente controllo la colonna di N che sto prendendo in considerazione e dove cambia la lettera metto un 1 
		i=nparole; 
		while (i<righen-1){
			if (End[i]==1){
				i=i+1;
				continue;  
			}
			if ((int)N[i][k]>(int)N[i+1][k]){
				appoggioi=Interleave[i]; 
				Interleave[i]=Interleave[i+1]; 
				Interleave[i+1]=appoggioi; 
				for (int y=0; y<nlettere; y++){
					VettoreAppoggioN[y]=N[i][y]; 
					N[i][y]=N[i+1][y]; 
					N[i+1][y]=VettoreAppoggioN[y]; 
				}// se c'è il confronto tra dollaro e lettera devo spostare la riga con il dollaro in basso e tenere in alto quella senza dollaro 
				i=nparole; // lo faccio ripartire da 0 perchè altrimenti non ordina in maniera corretta termina quando è tutto ordinato e quindi l'if non vale più
			}
			else {
				i++;
			}
		}
		//ora che ho fatto lo scambio controllo la colonna k di N e metto un uno a tutte le lettere differenti 
		i=nparole; 
		while (i<righen-1){
			if ((int)N[i][k]!=(int)N[i+1][k]){
				End[i]=1; 
			}
			if (N[i][k]=='$'){
				End[i]=1; 
			}
			i++; 
		} 
		// ora faccio il controllo sull'array End e aggiorno la variabile 
		controlloend=1; 
		for (i=0; i<righen; i++){
			if (End[i]==0){
				controlloend=0; 
			}
		}
		k++;
	}
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi della creazione dell'interleave e': %f \n", elapsedTime); 
}

void creazioneBWT (char *BWT){
	// registrazione tempo di avvio 
	gettimeofday (&start,NULL); 
	int appoggiobwt[nlettere]; // è un array di interi che indica la riga a cui andare a prendere le lettere da B 
	// inizializzazione appoggiobwt tutto a zeri 
	for (i=0; i<nlettere; i++){
		appoggiobwt[i]=0; 
	}
	
	// inizio creazione bwt 
	i=0; 
	while (i<righen){
		BWT[i]=B[appoggiobwt[Interleave[i]]][Interleave[i]]; 
		appoggiobwt[Interleave[i]]++; 
		i++; 
	}
	// tempo finale 
	gettimeofday (&stop,NULL); 

	elapsedTime = (stop.tv_sec - start.tv_sec) * 1000.0; 
	printf ("\n il tempo in millisecondi della creazione della BWT e': %f \n", elapsedTime); 
}

void skip_line(FILE *f) {
	while(getc(f)!='\n')
	;
}


