/**
Matheus Gomes da Silva Horta - 8532321
Lais helena Chiachio de Miranda - 8517032 **/

#include<iostream>
#include<cstdio>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include<cmath>
#include<ctime>

using namespace std;

// Estrutura que representa uma serie temporal com seus valores e sua classe
typedef struct {

    float type;
    vector < float > values;

}TemporalSequence;

float DTWdistance(vector<float>& arr1, vector<float>& arr2, float bandWidth);
float calculateDTW(vector < TemporalSequence >& testeSeq,
                   vector < TemporalSequence >& treinoSeq, float bandWidth);
float sakoeChibaDTW(float bandWidth, vector < TemporalSequence >& testeSeq,
                   vector < TemporalSequence >& treinoSeq);


/**
Função que chama a função que calcula a DTW para as series e retorna a procentagem **/
float sakoeChibaDTW(float bandWidth, vector < TemporalSequence >& testeSeq,
                   vector < TemporalSequence >& treinoSeq) {

    float percentage;
    percentage = calculateDTW(testeSeq,treinoSeq,bandWidth);

    return percentage;
}

/**
Calcula e retorna o menor entre 3 floats **/
float smallest(float x, float y, float z){
    return min(min(x, y), z);
}

/**
Calcula o a distancia entre duas séries utilizando o DTW
@args =: arr1 => vetor de series temporais de teste
         arr2 => vetor de series temporais de treino
 **/
float DTWdistance(vector<float>& arr1, vector<float>& arr2, float bandWidth) {

    // recebem
    unsigned int size1 = arr1.size(),size2 = arr2.size();

    // matriz de memoização e matriz de custos
    float DTW[size1][arr2.size()],cost[size1][arr2.size()];

    // largura da banda
    float band = size2 * bandWidth;
    // proporção de crescimento entre as duas series
    float proportion = (float)size2 / (float)size1;

    // preenche a matriz de custos previamente
    for( unsigned int i =0; i < size1; i ++) {
        for( unsigned int j =0; j < size2; j ++) {
            cost[i][j] = pow(arr1[i] - arr2[j],2);
        }
    }

    // preenche os valores iniciais
    for( unsigned int i = 0; i < size1; i ++) {
        DTW[i][0] = 999999;
    }
    for( unsigned int i = 0; i < size2; i ++) {
        DTW[0][i] = 999999;
    }
    DTW[0][0] = 0;

    // Preenche a matriz de memoização dado a restrição de banda
    for( unsigned int i = 1; i < size1; i ++) {
        for( unsigned int j = max(1, (int)round(i*proportion-band)); j < min((int)size2, (int)round(i*proportion+band)); j ++ ) {
            DTW[i][j] = cost[i][j] + smallest(DTW[i-1][j], DTW[i][j-1], DTW[i-1][j-1] );
        }
    }
    // retorna o valor procurado para as duas series
    return DTW[size1-1][size2-1];
}

/** Chama a funcao DTW para cada serie temporal e calcula o numero de acertos e erros para execução
@args =: testeSeq => estrutura de series temporais de teste
         treinoSeq => estrutura de series temporais de treino
         bandWidth => porcentagem de restrição de banda
**/
float calculateDTW(vector < TemporalSequence >& testeSeq,
                   vector < TemporalSequence >& treinoSeq, float bandWidth) {

    float percentage,type = 0,hits = 0, misses = 0,result,aux;

    for( unsigned int i = 0; i < testeSeq.size(); i ++ ) {
        result = 99999;
        for( unsigned int j = 0; j < treinoSeq.size(); j ++) {

            aux = DTWdistance(testeSeq[i].values,treinoSeq[j].values,bandWidth);
            if( aux < result ) {

                result = aux;
                type = treinoSeq[j].type;
            }
        }
        if( type == testeSeq[i].type) {
            hits++;
        }
        else {
            misses++;
        }
    }

    printf("hits %f misses %f\n",hits,misses);
    percentage = (hits / testeSeq.size() ) * 100 ;
    return percentage;
}

/**
Le o arquivo e organiza os valores em um vetor de series temporais
@args =: myfile : arquivo a ser lido
@return =: vetor de estruturas series temporais contendo seus valores e tipos
**/
vector < TemporalSequence > readFile(ifstream& myfile) {

    // variaveis auxiliares
    string line;
    stringstream temporalSeq;
    TemporalSequence auxSequence;
    float aux;
    // vetor a ser retornado
    vector < TemporalSequence > sequences;
    // le a linha do arquivo
    while( getline (myfile,line) ) {
        // passa para uma stream auxiliar
        temporalSeq << line;
        // le o tipo da serie
        temporalSeq >> auxSequence.type;
        // lçe os valores da serie
        while(!temporalSeq.eof()) {
            temporalSeq >> aux;
            auxSequence.values.push_back(aux);
        }
        // coloca a estrutura auxiliar no vetor de series
        sequences.push_back(auxSequence);

        auxSequence.values.clear();
        temporalSeq.clear();
    }

    return sequences;
}
int main( int argc, char **argv) {

    // arquivos de entrada
    ifstream myfile;
    // holds the temporal series from test file
    vector< TemporalSequence > testeSeq;
    // holds the temporal series from train file
    vector < TemporalSequence > treinoSeq;
    // porcentagem final e largura de banda usada no sakoe chiba
    float percentage,bandWidth[6] = {1.0, 0.5, 0.2, 0.1, 0.01, 0.0};
    // contagem de tempo
    clock_t start, end;
    float result;

    // valida entrada
    if (argc < 3) {
        cerr <<
        "uso: ./P2 arquivo_de_treino arquivo_de_teste"
        << endl;
        return -1;
    }

    string file1Name = argv[1];
    string file2Name = argv[2];



    // abre e le o arquivo teste
    myfile.open(argv[1]);
    testeSeq = readFile(myfile);
    myfile.close();
    // abre e le o arquivo treino
    myfile.open(argv[2]);
    treinoSeq = readFile(myfile);
    myfile.close();

    // realiza o processo e retorna a porcentagem
    for( int i =0; i < 6; i ++ ) {
        // inicia o relogio
        start = clock();
        // calcula o resultado para cada banda
        percentage = sakoeChibaDTW(bandWidth[i],testeSeq,treinoSeq);
        end = clock();
        result = (end - start)/CLOCKS_PER_SEC;
        // imprime as informações relevantes
        printf("Para uma banda de %f %\nTaxa de acerto : %f \nTempo gasto : %f\n\n",bandWidth[i]*100,percentage,result);
    }

    return 0;
}
