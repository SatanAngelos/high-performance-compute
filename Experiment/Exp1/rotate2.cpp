#include<bits/stdc++.h>
using namespace std;
#define RIDX(i,j,dim) (i*dim+j)
#define COPY(d,s) *(d)=*(s)
typedef struct pixel{
		int red;
		int green;
		int blue;
}pixel;

void rotate2(int dim, pixel *src, pixel *dst)
{
	int i,j,ii,jj;
	for(ii=0; ii < dim; ii+=32)
		for(jj=0; jj < dim; jj+=32)
			for(i=ii; i < ii+32; i+=4)
				for(j=jj; j < jj+32; j+=4)
				{

				dst[RIDX(dim-1-j,i,dim)] = src[RIDX(i,j,dim)];
				dst[RIDX(dim-1-j,i+1,dim)] = src[RIDX(i+1,j,dim)];
				dst[RIDX(dim-1-j,i+2,dim)] = src[RIDX(i+2,j,dim)];
				dst[RIDX(dim-1-j,i+3,dim)] = src[RIDX(i+3,j,dim)];

				dst[RIDX(dim-1-j-1,i,dim)] = src[RIDX(i,j+1,dim)];
				dst[RIDX(dim-1-j-1,i+1,dim)] = src[RIDX(i+1,j+1,dim)];
				dst[RIDX(dim-1-j-1,i+2,dim)] = src[RIDX(i+2,j+1,dim)];
				dst[RIDX(dim-1-j-1,i+3,dim)] = src[RIDX(i+3,j+1,dim)];

				dst[RIDX(dim-1-j-2,i,dim)] = src[RIDX(i,j+2,dim)];
				dst[RIDX(dim-1-j-2,i+1,dim)] = src[RIDX(i+1,j+2,dim)];
				dst[RIDX(dim-1-j-2,i+2,dim)] = src[RIDX(i+2,j+2,dim)];
				dst[RIDX(dim-1-j-2,i+3,dim)] = src[RIDX(i+3,j+2,dim)];

				dst[RIDX(dim-1-j-3,i,dim)] = src[RIDX(i,j+3,dim)];
				dst[RIDX(dim-1-j-3,i+1,dim)] = src[RIDX(i+1,j+3,dim)];
				dst[RIDX(dim-1-j-3,i+2,dim)] = src[RIDX(i+2,j+3,dim)];
				dst[RIDX(dim-1-j-3,i+3,dim)] = src[RIDX(i+3,j+3,dim)];

				}

}

int main(){

	int dim=128;
    pixel *src=(pixel*)malloc(sizeof(pixel));
    pixel *dst=(pixel*)malloc(sizeof(pixel));
    (*src).blue=1,(*src).green=0,(*src).red=1;
    (*dst).blue=0,(*dst).green=1,(*dst).red=0;


		//naive_rotate(dim,src,dst);

		//rotate1(dim,src,dst);

		rotate2(dim,src,dst);

		//rotate3(dim,src,dst);


	free(src);
	free(dst);
	src=NULL;
	dst=NULL;
	return 0;

}
