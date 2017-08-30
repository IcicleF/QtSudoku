#include <iostream>
#include <cstdio>
#define searchnext(x,y) y==9? search(x+1,1):search(x,y+1)

using namespace std;

bool r[10][10],l[10][10],s[10][10];//行，列，小九宫格
int a[10][10],b[10][10];
bool fillin(int x,int y,int k)
{
	if(r[x][k])return 0;
	if(l[y][k])return 0;
	if(s[(x-1)/3*3+(y-1)/3+1][k])return 0;
	b[x][y]=k;
	r[x][k]=l[y][k]=s[(x-1)/3*3+(y-1)/3+1][k]=1;
	return 1;
}
void del(int x,int y,int k)
{
	b[x][y]=0;
	r[x][k]=l[y][k]=s[(x-1)/3*3+(y-1)/3+1][k]=0;
}
void search(int x,int y)
{
	if(x==10&y==1)
	{
		for (int i=1;i<=9;++i){
			for (int j=1;j<=9;++j)
				printf("%d ", b[i][j]);
			puts("");
		}
		puts("");

		for (int i=1;i<=9;++i){
			for (int j=1;j<=9;++j)
				printf("%d ", a[i][j]?1:0);
			puts("");
		}
		return;
	}
	if(b[x][y])searchnext(x,y);
	else
		for(int i=1;i<=9;i++)
		{
			if(fillin(x,y,i))
			{
				searchnext(x,y);
				del(x,y,i);
			}
		}
}
char str[15];
int main()
{
	for(int i=1;i<=9;i++)
	{
		scanf("%s",str);
		for(int j=1;j<=9;j++)
		{
			a[i][j]=str[j-1] - '0';
			if(a[i][j])fillin(i,j,a[i][j]);
		}
	}
    search(1,1);
	return 0;
}
