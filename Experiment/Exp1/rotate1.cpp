#include<bits/stdc++.h>
using namespace std;
#define RIDX(i,j,dim) (i*dim+j)
#define COPY(d,s) *(d)=*(s)
typedef struct pixel{
		int red;
		int green;
		int blue;
}pixel;

void rotate1(int dim,pixel *src,pixel *dst){
	int i,j,ii,jj;
	for(ii=0;ii<dim;ii+=4){
		for(jj=0;jj<dim;jj+=4){
			for(i=ii;i<ii+4;i++){
				for(j=jj;j<jj+4;j++){

					dst[RIDX(dim-1-j,i,dim)]=src[RIDX(i,j,dim)];
				}
			}
		}
	}

}

int main(){

	int dim=128;
    pixel *src=(pixel*)malloc(sizeof(pixel));
    pixel *dst=(pixel*)malloc(sizeof(pixel));
    (*src).blue=1,(*src).green=0,(*src).red=1;
    (*dst).blue=0,(*dst).green=1,(*dst).red=0;


		//naive_rotate(dim,src,dst);

		rotate1(dim,src,dst);

		//rotate2(dim,src,dst);

		//rotate3(dim,src,dst);


	return 0;

}
