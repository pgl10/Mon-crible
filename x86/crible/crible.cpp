/*************************************************************
*  Crible d'Eratosthène par pgl10                            *
*                                                            *
*  Pour gagner de la mémoire et du temps d'exécution on ne   *
*  marque pas dans le crible les multiples de 2, 3, 5 et 7   *
*  Après avoir calculé le crible on a les fonctions :        *
*  - prem(n) : le nombre n est-il premier ?                  *
*  - next(n) : le nombre premier qui suit l'entier n         *
*  - nieme(n) : la valeur du n-ième nombre premier           *
*  - nbprem(n) : le nombre de premiers entre 1 et n          *
*  Dans cette version la limite d'exploration est            *
*  au maximum de : 4 294 311 961                             *
*************************************************************/

#include <iostream>
#include <iomanip>
#include <ctime>

typedef unsigned int ent;

ent nc;
//  limite de l'intervalle [1,nc] de calcul du crible
ent nmax = 4294311961;
//  nmax est la valeur maximum de nc.
//  Limite des unsigned int : (2^32)-1 = 4294967295
//  Pour un crible plus grand, on aurait pour i
//  à partir de : 65521 = 311*210 + 211 les valeurs :
//  65521(prem), 65531, 65533, 65537(prem), 65539(prem), ...
//  imax=65537 est impossible : 65537 x 65537 = 4295098369
//  mais imax=65533 est impossible aussi parce que
//  pour i=11 quand nj=89246071 : j=4294967281 (correct)
//  mais quand nj=89246072 (cycle[nj%48]=2) : j=4294967303 
//  ce qui est impossible pour un entier sur 32 bits.
//  Donc imax = 65531 et nmax = 65531 x 65531 = 4294311961
    
unsigned char* crible;  
//  crible[] contient les bits utiles du crible.
ent seqnc[48];
//  La séquence répétitive des nombres marqués dans le crible.
ent cycle[48];
//  cycle[i] = seqnc[i+1] - seqnc[i]
ent nb[210];
//  Les rangs de la séquence répétitive du crible.
int nk[256];
//  nk[i] est le nombre de bits contenus dans l'octet i (de 0 à 255)
bool done=false;
//  done est mis à la valeur true quand les nk[] sont calculés
unsigned char c0[8]={'\xFE', '\xFD', '\xFB', '\xF7', '\xEF', '\xDF', '\xBF', '\x7F'};
unsigned char c1[8]={'\x01', '\x02', '\x04', '\x08', '\x10', '\x20', '\x40', '\x80'};
     
//   Suivant la méthode du crible d'Eratosthène on va marquer
//   à 1 les bits du crible pour les nombres premiers de 1 à n
//   et à 0 pour les nombres composés.
//   Mais pour gagner du temps et de l'espace mémoire les multiples
//   de 2, 3, 5 et 7 ne seront ni présents ni marqués dans le crible.
//   Chaque élément du crible aura les bits marqueurs pour 8 entiers.
//   crible[0] pour  11,  13,  17,  19,  23,  29,  31,  37
//   crible[1] pour  41,  43,  47,  53,  59,  61,  67,  71
//   crible[2] pour  73,  79,  83,  89,  97, 101, 103, 107
//   crible[3] pour 109, 113, 121, 127, 131, 137, 139, 143
//   crible[4] pour 149, 151, 157, 163, 167, 169, 173, 179
//   crible[5] pour 181, 187, 191, 193, 197, 199, 209, 211
//   crible[6] pour 221, 223, 227, 229, 233, 239, 241, 247
//   etc. 121 est le 1-er nombre composé du crible et 143 le 2-ième.
//   Tous les 2*3*5*7 = 210 valeurs les nombres marqués dans crible
//   recommencent une séquence identique décalée de la valeur 210.
//   Dans le crible elle occupe 48 positions succesives seulement.
//   Après avoir rempli le tableau nb[210] avec les rangs successifs
//   le nombre i sera marqué par le p-ième bit du crible avec 
//   p = ((i-11)/210)*48 + nb[(i-11)%210];  dans l'octet q=p/8 
//   à la position r=p-8*q et inversement le p-ième bit du crible 
//   est le marqueur du nombre i = (p/48)*210 + seqnc[p%48]
     
void pause() {
    std::cout << std::endl;
    system("pause");    
}

void limite() {
    std::cout << std::endl 
              << "L'intervalle de calcul est trop grand" << std::endl;
    pause();
    exit(EXIT_FAILURE);
}
     
//  L'entier i est-il dans le crible ?
bool filtre(ent i) {
    if(i%2==0) return false;
    if(i%3==0) return false;
    if(i%5==0) return false;
    if(i%7==0) return false;
    return true;
}

//  Crible d'Eratosthène
void Eratosthene() {
    ent i, i2, j, ni, nj, p, q, r, t;
    //  Pour p=0 à 47 les nombres seqnc[p] sont les
    //  nombres de la séquence répétitive du crible
    p=0;
    for(i=11; i<221; i++) {
        if(filtre(i)) {
            seqnc[p]=i;
            p=p+1;
        }
    }
    //  Pour p=0 à 47 cycle[p] est la différence entre
    //  le (p+1)-ième nombre marqué et le p-ième.
    for(i=0; i<47; i++) cycle[i]=seqnc[i+1]-seqnc[i];
    //  cycle[47]=seqnc[0]+210-seqnc[47]=221-211 
    cycle[47]=10;
    //  Pour i de 0 à 209 le nombre i+11 ( de 11 à 220 )
    //  est marqué dans le crible par le nb[i]-ième bit
    p=0;
    for(i=0; i<210; i++) {
        nb[i]=0;
        if(filtre(i+11)) {
            nb[i]=p;
            p=p+1;
        }
    }
    p=0;
    for(i=0; i<210; i++) {
        if(nb[i]==0) nb[i]=p;
		else p=p+1;
    }
    q=1+((nc-11)/210*48+nb[(nc-11)%210])/8;
    crible=new unsigned char[q];
    if(crible==NULL) limite();
    std::cout << std::endl << "Le crible a : " << q << " octets" << std::endl;
    std::cout << std::endl << "Attendre ...   ";
    // Initialisation de crible[] : tous les marqueurs à 1
    memset(crible, '\xFF', q);
    //  Selon la méthode habituelle, on va marquer par un bit nul les multiples de
    //  chaque nombre premier à partir de 11 en évitant les multiples de 2, 3, 5 et 7
    i=11;
    i2=121;
    ni=nj=0;
    while(i2<=nc) {
        p=(i-11)/210*48+nb[(i-11)%210];
        q=p/8;
        r=p-8*q;
        if((crible[q]&c1[r]) != '\x00') {
            //  La majorité du temps CPU se passe
            //  dans la boucle interne suivante
            for(j=i2; j<=nc;) {
                t=(j-11)/210*48+nb[(j-11)%210];
                q=t/8;
                //  Pour marquer à 0 le nombre composé j multiple de i
                //  possible mais moins bon : crible[q]&=(~(1<<(t-8*q))); 
                crible[q]&=c0[t-8*q];
                //  Le j suivant sera le prochain multiple de i 
                //  non multiple de 2, 3, 5 ou 7
                j=j+cycle[nj%48]*i;
                nj=nj+1;
            }
        }
        i=i+cycle[p%48];
        nj=ni=ni+1;
        i2=i*i;
    }
}

//  Retourner true si le nombre i est premier et false si i est composé
bool prem(ent i) {  
    ent p, q, r;
    if(i<2 || i>nc) return false;
    //  les multiples de 2, 3, 5 et 7 ne sont pas marqués dans crible[]
    if(i%2==0) return i==2;
    if(i%3==0) return i==3;
    if(i%5==0) return i==5;
    if(i%7==0) return i==7;
    p=((i-11)/210)*48+nb[(i-11)%210];
    q=p/8;
    r=p-8*q;
    if((crible[q]&c1[r]) == '\x00') return false;
    return true;
}

//  Retourner le premier nombre premier qui suit le nombre i
ent next(ent i) {
    ent p, q, r, x;
    if(i<2)  return 2;
    if(i==2) return 3;
    if(i<5)  return 5;
    if(i<7)  return 7;
    if(i<11) return 11;
    x=((i-11)/210)*48+nb[(i-11)%210];
    for(p=x+1;; p++) {
        q=p/8;
        r=p-8*q;
        x=(p/48)*210+seqnc[p%48];
        if(x>nc) return 0;
        if((crible[q]&c1[r]) != '\x00') return x;
    }
    return 0;
}

//  Calcul des 256 valeurs nk[i] pour i = 0 à  255
void inink() {
    unsigned char oct;
    int k, n;
    // nk[i] : nombre de bits à 1 dans l'octet i 
    for(k=0; k<256; k++) {
        oct=(unsigned char)k;
        nk[k]=0;
        for(n=0; n<8; n++) if((oct&c1[n]) != '\x00') nk[k]=nk[k]+1;
    }
    done=true;
}

//  Retourner le nombre de nombres premiers entre 1 et i
ent nbprem(ent i) {
    ent k, l, n, p, q, r, z;
    if(i<2)  return 0;
    if(i==2) return 1;
    if(i<5)  return 2;
    if(i<7)  return 3;
    if(i<11) return 4;
    if(i>nc) return 0;
    if(!done) inink();
    n=4;
    z=((i-11)/210)*48+nb[(i-11)%210];
    l=z/8;
    // Pour les octets complets du crible
    // avant celui qui a le marqueur de i
    for(k=0; k<l; k++) {
        n=n+nk[int(crible[k])];
    }
    // Et pour l'octet du crible qui a le marqueur de i
    for(p=8*l; p<=z; p++) {
        q=p/8;
        r=p-8*q;
        if((crible[q]&c1[r]) != '\x00') n=n+1;
    }
    return n;
}

//  Retourner le i-ième nombre premier
ent nieme(ent i) {
    ent j, k, l, n, p, q, r, z;
    if(i<1)  return 0;
    if(i==1) return 2;
    if(i==2) return 3;
    if(i==3) return 5;
    if(i==4) return 7;
    if(!done) inink();
    n=4;
    z=((nc-11)/210)*48+nb[(nc-11)%210];
    l=z/8;
    // octet par octet
    k=0;
    for(j=0; j<=l; j++) {
        n=n+nk[int(crible[k])];
        if(n>=i) break;
        ++k;
    }
    n=n-nk[int(crible[k])];
    // bit par bit
    for(p=8*k; p<=z; p++) {
        q=p/8;
        r=p-8*q;
        if((crible[q]&c1[r]) != '\x00') n=n+1;
        if(n==i) return (p/48)*210+seqnc[p%48];
    }
    return 0;
}

void menu() {
    std::cout << std::endl << "Les commandes admises ici sont des types suivants : " << std::endl
        << "n 677 : combien y a-t-il de nombres premiers entre 1 et 677 ?" << std::endl
        << "p 677 : le nombre 677 est-il premier ?" << std::endl
        << "r 677 : quel est le rang du nombre 677 ?" << std::endl
        << "s 677 : quel est le nombre premier qui suit 677 ?" << std::endl
        << "? 123 : quel est le nombre premier de rang 123 ?" << std::endl
        << "q : quitter l'application" << std::endl;
}

void dialogue() {
    char cde[256], car;
    ent i, n;
    ent nbp=nbprem(nc);
    ent pgp=nieme(nbp);
    bool good;
    for(;;) {
        do {
            good = true;
            std::cout << std::endl << "> ";
            std::cin.getline(cde, 256);
            car=cde[0];
            if(car == 'q') break;
            for(i=0; i<int(strlen(cde)); i++) cde[i]=cde[i+1];
            n = 1; // pour le cas où sscanf_s() ne peut pas lire n
            i=sscanf_s(cde, "%d", &n);
            if(!(i>0) || n < 1 || n > pgp) good=false;
            if(car=='?' && n>nbp) good=false;
            if(!good) menu();
        }while(!good);
        if(car=='n') {
            i = nbprem(n);
            if(n==1) std::cout << "De 1 \205 1 il n'y a aucun nombre premier" << std::endl;
            else if(n==2) {
                std::cout << "De 1 \205 2 il y a 1 nombre premier " << std::endl;
                std::cout << "et le dernier est 2" << std::endl;
            }
            else {
                std::cout << "De 1 \205 " << n << " il y a " << i << " nombres premiers" << std::endl;
                std::cout << "et le dernier est " << nieme(i) << std::endl;
            }
        }
        else if(car=='p') {
            if(prem(n)) std::cout << n << " est un nombre premier" << std::endl;
            else if(n == 1) std::cout <<  "1 n'est pas un nombre premier"  << std::endl; 
            else std::cout << n << " est un nombre compos\202" << std::endl;
        }
        else if(car=='q') break;
        else if(car=='r') {
            i = nbprem(n);
            if(n==1) std::cout << "1 n'est pas un nombre premier"  << std::endl; 
            else if(n==2) std::cout << "2 est le 1-er nombre premier"  << std::endl; 
            else if(prem(n)) std::cout << n << " est le " << i << "-i\212me nombre premier" << std::endl;
            else std::cout << n << " est un nombre compos\202 qui suit le " << i << "-i\212me premier"  << std::endl;
        }
        else if(car=='s') {
            std::cout << "Le nombre premier qui suit " << n << " est " << next(n) << std::endl;
        }
        else if(car=='?') {
            if(n==1) std::cout << "Le 1-er nombre premier est : " << nieme(n) << std::endl;
            else std::cout << "Le " << n << "-i\212me nombre premier est : " << nieme(n) << std::endl;
        }
        else menu();
    }
}

void erreur() {
    std::cout << std::endl << "Pour calculer les nombres premiers de 1 \205 n" << std::endl;
    std::cout << std::endl << "lancez ce programme en faisant : crible n" << std::endl;
    pause();
    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) { 
    ent nbp, pgp;
    if(argc!=2) erreur();
    if(!(sscanf_s(argv[1], "%u", &nc)>0)) erreur();
    std::cout << std::endl << "---------------------------------------------------" << std::endl;
    std::cout << std::endl << "Crible d'Eratosth\212ne" << std::endl;
    std::cout << std::endl << "Calcul des nombres premiers entre 1 et " << nc << std::endl;
    if(nc>nmax) limite();
    clock_t start = clock();
    Eratosthene();
    std::cout << std::fixed << "\rCalcul du crible en : " << std::setprecision(3) 
              << double(clock()-start)/double(CLOCKS_PER_SEC) << " s" << std::endl;
    nbp=nbprem(nc);
    pgp=nieme(nbp);
    std::cout << std::endl << "Nombre de nombres premiers calcul\202s : " << nbp << std::endl;
    std::cout << std::endl << "Le premier nombre premier est : " << 2 << std::endl;
    std::cout << std::endl << "Le " << nbp << "-i\212me nombre premier est : " << pgp << std::endl;
    // Pour supprimer la phase conversationnelle il suffit de mettre
    // en commentaire l'instruction : dialogue(); et de recompiler
    dialogue();
    delete [] crible;
    return 0;
}
