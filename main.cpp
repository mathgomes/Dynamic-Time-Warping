#include<iostream>
#include<cstdio>
#include<string>
#include<fstream>
#include<sstream>
#include<vector>
#include<cmath>
#include<ctime>

using namespace std;

typedef struct {

    float type;
    vector < float > values;

}TemporalSequence;

float DTWdistance(vector<float>& arr1, vector<float>& arr2, float bandWidth);
float calculateDTW(vector < TemporalSequence >& testeSeq,
                   vector < TemporalSequence >& treinoSeq, float bandWidth);
float sakoeChibaDTW(float bandWidth, vector < TemporalSequence >& testeSeq,
                   vector < TemporalSequence >& treinoSeq);


float sakoeChibaDTW(float bandWidth, vector < TemporalSequence >& testeSeq,
                   vector < TemporalSequence >& treinoSeq) {

    float percentage;
    percentage = calculateDTW(testeSeq,treinoSeq,1.0);

    return percentage;
}

float smallest(float x, float y, float z){
    return min(min(x, y), z);
}

float DTWdistance(vector<float>& arr1, vector<float>& arr2, float bandWidth) {

    int size1 = arr1.size(),size2 = arr2.size();

    float DTW[size1][arr2.size()],cost[size1][arr2.size()];

    float band = size2 * bandWidth;
    float proportion = size2 / size1;


    for( unsigned int i =0; i < size1; i ++) {
        for( unsigned int j =0; j < size2; j ++) {
            cost[i][j] = pow(arr1[i] - arr2[j],2);
        }
    }

    for( unsigned int i = 0; i < size1; i ++) {
        DTW[i][0] = 999999;
    }
    for( unsigned int i = 0; i < size2; i ++) {
        DTW[0][i] = 999999;
    }
    DTW[0][0] = 0;

    for( unsigned int i = 1; i < size1; i ++) {
        for( unsigned int j = max(1, (int)round(i*proportion-band)); j < min(size2, (int)round(i*proportion+band)); j ++ ) {
            DTW[i][j] = cost[i][j] + smallest(DTW[i-1][j], DTW[i][j-1], DTW[i-1][j-1] );
            //printf("DTW %d %d  = %f\n",i,j,DTW[i][j]);
        }
    }
    //printf("retorno %f ",DTW[arr1.size()][arr2.size()]);
    return DTW[size1-1][size2-1];
}

float calculateDTW(vector < TemporalSequence >& testeSeq,
                   vector < TemporalSequence >& treinoSeq, float bandWidth) {

    float percentage,type,hits = 0, misses = 0,result,aux;

    for( unsigned int i = 0; i < testeSeq.size(); i ++ ) {
        result = 99999;
        for( unsigned int j = 0; j < treinoSeq.size(); j ++) {

            aux = DTWdistance(testeSeq[i].values,treinoSeq[j].values,bandWidth);
            //printf("resultado para %d %d  = %f\n",i,j,aux);
            if( aux < result ) {
                //printf("%d %d aux %f result %f\n",i,j,aux,result);
                result = aux;
                type = treinoSeq[j].type;
            }
        }
        if( type == testeSeq[i].type) {
            //printf("hit %f %f\n",type,testeSeq[i][0]);
            hits++;
        }
        else {
            //printf("miss %f %f\n",type,testeSeq[i][0]);
            misses++;
        }
    }
    printf("hits %f misses %f\n",hits,misses);
    percentage = (hits / testeSeq.size() ) * 100 ;
    return percentage;
}


vector < TemporalSequence > readFile(ifstream& myfile) {

    string line;
    stringstream temporalSeq;
    vector < TemporalSequence > sequences;
    float aux;
    TemporalSequence auxSequence;

    while( getline (myfile,line) ) {
        temporalSeq << line;
        temporalSeq >> auxSequence.type;
        while(!temporalSeq.eof()) {
            temporalSeq >> aux;
            auxSequence.values.push_back(aux);
        }
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

    float percentage,bandWidth;

    clock_t start, end;

    // valida entrada
    if (argc < 3) {
        cerr <<
        "uso: ./P2 arquivo_de_treino arquivo_de_teste"
        << endl;
        return -1;
    }

    string file1Name = argv[1];
    string file2Name = argv[2];

    start = clock();

    myfile.open(argv[1]);
    testeSeq = readFile(myfile);
    myfile.close();

    myfile.open(argv[2]);
    treinoSeq = readFile(myfile);
    myfile.close();

    percentage = sakoeChibaDTW(bandWidth,testeSeq,treinoSeq);

    end = clock();

    float result = (end - start)/CLOCKS_PER_SEC;

    printf("tempo %f\ntaxa de acerto %f\n",result,percentage);

    return 0;
}
