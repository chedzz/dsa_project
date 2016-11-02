/**************************************************************************************************************
 * Copyright (C) 2016 Namyata M. Cheduri (namyata.cheduri@gmail.com)
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Gneral Publlic License as published by the the 
 * Free Software Foundation; either version 3 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even implied warranty of
 * MERCHANTIBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have recieved a GNU Generl Public License along wih this
 * program; If not, see <https://www.gnu.org/licenses/>.
 *************************************************************************************************************/

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

typedef unsigned long long int ull;
#define MAX_DIGIT 1024
#define MIN_LEN 4
typedef struct element {
	ull x;
	int *xx;
	int xLen;
	int xNeg;
}element;
typedef struct keylist {
	ull e, n, totient;
	long long int d;
}keylist;

int *arrz;
static int powerof2;

void convert_to_array(unsigned long long int , int *, int *);
int power_of_2_size(int, int);
void init_array(int *, int *, int, int, int);
void bignumber_multiply_divide_conquer(int *, int *, int *, int);
void print_bignumber(int *, int);
void small_mult(int *, int *, int *, int);
void make_single_digit(int *, int);
void mul_main(long long int, long long int);
void encrypt(ull, ull, int, int);
void decrypt(long long int, ull, int, int);
ull power_mod(element *, ull, ull);
void multiply_bignumber(element *, element *, element *);
ull mod_bignumber(int *, int, ull);
void reconvert_to_num(element *);
ull ran_no_gen();
ull prime_gen();
ull my_pow(ull x, ull y,ull n);
ull extended_euclidean(ull, ull, long long int *, long long int *);
void private_key(long long int *, ull, ull);
int  prime(ull p);
time_t tt;

int main(int argc, char *argv[]) {
	if(argc != 4) {
		printf("usage: ./a.out -e|d <source_filename> <dest filename>\n");
		return EINVAL;
	}
	int fd_source, fd_dest, fd_keys;
	fd_source = open(argv[2], O_RDWR | O_CREAT, S_IRWXU | S_IRWXO | S_IRWXG);
	if(fd_source == -1) {
		perror("Open failed");
		return errno;
	}
	fd_dest = open(argv[3], O_RDWR | O_CREAT, S_IRWXU | S_IRWXO | S_IRWXG);
	if(fd_dest == -1) {
		perror("Open failed");
		return errno;
	}
	fd_keys = open("Keys", O_APPEND | O_RDWR);
	if(fd_keys == -1) {
		perror("open failed");
		return errno;
	}
	ull p, q = 0, totient, e, n, i;
	keylist k, k_ret;
	long long int d;
	int ran_count = 0;
	srandom(time(&tt));
	char choice[3];
	strcpy(choice, argv[1]);
	char cm;
	switch(choice[1]) {
		case 'e':
			
			p = prime_gen(RAND_MAX);
			while(1) {
				q = prime_gen(RAND_MAX);
				ran_count++;
				if(q != p)
				break;
			}

			n = p * q;
			totient = (p - 1) * (q - 1);
			while(1) {
				e = prime_gen(totient - 1);
				if((totient % e) != 0)
					break;
			}
			private_key(&d, e, totient);
			if(d < 0)
				d = d + totient;
			encrypt(e, n, fd_source, fd_dest);
			close(fd_source);
			close(fd_dest);
			free(arrz);
			k.e = e;
			k.n = n;
			k.totient = totient;
			k.d = d;
			write(fd_keys, &k, sizeof(keylist));
			close(fd_keys);
			break;
		case 'd' :
			fd_keys = open("Keys", O_RDONLY);
			read(fd_source, &e, sizeof(ull));
			while(read(fd_keys, &k_ret, sizeof(keylist))) {
				if(k_ret.e == e)
					break;
			}
			d = k_ret.d;
			n = k_ret.n;
			printf("Calling decrypt\n");
			decrypt(d, n, fd_source, fd_dest);
			printf("Decrypted message\n");
			close(fd_dest);
			close(fd_source);
			free(arrz);
			break;
	}
	
	return 0;
}
int  prime(unsigned long long int p) {
	if(p%2==0)
		return 0;		
	long long int k = 0;
	unsigned long long int exp, a;
	time_t tt;
	unsigned long long int d = p - 1;
	while(d % 2 == 0) {
		d /= 2;
		k++;
	}
	a = 2 + random() % (p - 3);
	exp = my_pow(a , d, p);
	if(exp == 1 || exp == p -1) 
		return 1;
	while(d != p - 1) {
		exp = my_pow(exp , 2, p);
		d = d * 2;
		if(exp == 1) 
			return 0;	
		if(exp == p-1) 
			return 1;
	}
	return 0;
}
ull my_pow(ull x, ull y,ull n) {
	unsigned long long int res = 1;
	x = x % n;
	while(y > 0) {
		if(y & 1) {
			res = (res * x) % n;
		}
		y = y / 2;
		x = (x * x) % n;
	}
	return res;
}
unsigned long long int prime_gen(unsigned long long int range) {
	unsigned long long int n, t;
	int type = -1;
	while(1) {
		n = random();
		if(n < range) 
			break;
	}
	t = n / 6;
	n = 6 * t + 1;
	while(type != 1) {
		type = prime(n);
		if(type == 1)
			break;	
		++t;
		n = 6 * t + 1;
	}
	return n;
}
void private_key(long long int *d, ull e, ull totient) {
	ull gcd;
	long long int x, y;
	gcd = extended_euclidean(e, totient, &x, &y);
	*d = x;
}	
ull extended_euclidean(ull e, ull totient, long long int *x, long long int *y) {
	if(e == 0) {
		*x = 0;
		*y = 1;
		return totient;
	}
	long long int  x_next, y_next, gcd;
	gcd = extended_euclidean(totient % e, e, &x_next, &y_next);
	*x = y_next - (totient / e) * x_next;
	*y = x_next;
	return gcd;
}	 

void mul_main(long long int a, long long int b) {
	int arrx[MAX_DIGIT], arry[MAX_DIGIT];
	free(arrz);
	arrz = (int *)malloc(sizeof(int) * 6 * MAX_DIGIT);
	int arrxlen = 0, arrylen = 0;
	convert_to_array(a, arrx, &arrxlen);
	if(arrxlen <= 0)
		printf("Error\n");
	convert_to_array(b, arry, &arrylen);
	if(arrylen <= 0)
		printf("Error\n");
	powerof2 = power_of_2_size(arrxlen, arrylen);
	init_array(arrx, arry, arrxlen, arrylen, powerof2);
	bignumber_multiply_divide_conquer(arrx, arry, arrz, powerof2);
	make_single_digit(arrz, 2 * powerof2);
	print_bignumber(arrz, 2 * powerof2);
	return;
}
void convert_to_array(unsigned long long int e, int aa[], int *x) {
	if(*x != 0)
		*x = 0;
	while(e > 0) {
		aa[(*x)] = e % 10;
		(*x)++;
		e = e / 10;
	}
}
int power_of_2_size(int a, int b) {
    	int powerof2 = 1;
   	while(powerof2 <= a || powerof2 <= b) {
        	powerof2 *= 2;
    	}
    	return powerof2;
}
void init_array(int arrx[], int arry[], int arrxlen, int arrylen, int powerof2) {
    	int i = arrxlen;
	while(i < powerof2) {
		arrx[i] = 0;
		i++;
	}
    	i = arrylen;
	while(i < powerof2) {
		arry[i] = 0;
		i++;
	}   
}
void bignumber_multiply_divide_conquer(int *arrx, int *arry, int rr[], int powerof2) {
	int *p = &arrx[powerof2 / 2];
	int *q = &arrx[0];
	int *r = &arry[powerof2 / 2];
	int *s = &arry[0];
	int *wx = &rr[powerof2 * 5]; 
	int *wy = &rr[powerof2 * 5 + powerof2 / 2]; 
	int *V = &rr[powerof2 * 0];  
	int *U = &rr[powerof2 * 1];  
	int *W = &rr[powerof2 * 2];          
 	if(powerof2 <= MIN_LEN) {
 	       small_mult(arrx, arry, rr, powerof2);
 	       return;
	}
	int i;
	for(i = 0; i < powerof2 / 2; i++) {
        wx[i] = p[i] + q[i];
        wy[i] = r[i] + s[i];
	}
	bignumber_multiply_divide_conquer(q, s, V, powerof2 / 2);
	bignumber_multiply_divide_conquer(p, r, U, powerof2 / 2);
	bignumber_multiply_divide_conquer(wx, wy, W, powerof2 / 2);
	for(i = 0; i < powerof2; i++) {
		W[i] = W[i] - U[i] - V[i];
	}
	for(i =0; i < powerof2; i++) {
		rr[i + powerof2 / 2] += W[i];
	} 
} 
void print_bignumber(int aa[], int x) {
	int i;
	for(i = x-1; i > 0; i--) {
		if(aa[i] != 0)
			break;
	}
	while(i >= 0) {
    		printf("%d", aa[i]);
		i--;
	}
	printf("\n");
}
void small_mult(int *arrx, int *arry, int *rr, int powerof2) {
	int i;
	int j;
	for(i = 0; i < 2 * powerof2; i++) {
		rr[i] = 0;
	}
	for(i = 0; i < powerof2; i++) {
		for(j = 0; j < powerof2; j++) {
			rr[i+j] += arrx[i] * arry[j];
		}
	}
}
void make_single_digit(int rr[], int powerof2) {
	int i;
	int carry = 0;
	for(i = 0; i < powerof2; i++) {
		rr[i] += carry;
		if(rr[i] < 0) {
			carry = (-(rr[i] + 1) / 10 + 1); 
		}
		else {
			carry = rr[i] / 10;
        	}
		rr[i] = rr[i] - carry * 10;
	}
	assert(carry == 0);
} 
ull mod_bignumber(int a[], int aLen, ull n) {
	ull temp = 0;
	for(int k = aLen - 1 ; k >= 0 ; k--) {
		temp = 10 * temp + a[k];
		temp %= n;
	}
	return temp;
}
void encrypt(ull e, ull n, int fd, int fd_dest) {
	char ch;
	lseek(fd, 0, SEEK_SET);
	write(fd_dest, &e, sizeof(e));
	while(read(fd, &ch, 1)) {
		element c;
		c.x = ch;
		c.xx = (int *)malloc(sizeof(int) * MAX_DIGIT);
		convert_to_array(c.x, c.xx, &c.xLen);
		ull cipher = power_mod(&c, e, n);
		write(fd_dest, &cipher, sizeof(ull));
	}
}
ull power_mod(element *c, ull e, ull n) {
	element res, tmp;
	res.x = 1;
	res.xx = (int *)malloc(sizeof(int) * 6 * MAX_DIGIT);
	tmp.xx = (int*)malloc(sizeof(int) * 6 * MAX_DIGIT);
	convert_to_array(res.x, res.xx, &res.xLen);
	ull temp = mod_bignumber(c -> xx, c -> xLen, n);
	convert_to_array(temp, c -> xx, &(c -> xLen));
	while(e > 0) {
		if(e & 1) {
			multiply_bignumber(c, &res, &tmp);
			temp = mod_bignumber(tmp.xx, tmp.xLen, n);
			convert_to_array(temp, res.xx, &res.xLen);
		}
		e = e / 2;
		multiply_bignumber(c, c, &tmp);
		temp = mod_bignumber(tmp.xx, tmp.xLen, n);
		convert_to_array(temp, c -> xx, &(c -> xLen));
	}	
	reconvert_to_num(&res);
	ull cipher = res.x;
	free(c -> xx);
	free(res.xx);
	free(tmp.xx);
	return cipher;
}
void multiply_bignumber(element *a, element *b, element *res) {
	int i;	
	int tLen = power_of_2_size(a -> xLen,  b -> xLen);
	init_array(a -> xx, b -> xx, a -> xLen, b -> xLen, tLen);
	bignumber_multiply_divide_conquer(a -> xx, b -> xx, res -> xx, tLen);
	make_single_digit(res -> xx, 2 * tLen);
	for(i = 2 * tLen - 1; i >= 0; i--) {
		if(res -> xx[i] != 0)
			break;
	}
	res -> xLen = (i + 1);
	return;
}
void reconvert_to_num(element *a) {
	int i;
	ull sum = 0;
	for(i = (a -> xLen) - 1; i>=0; i--) {
		sum = sum * 10 + a -> xx[i];
	}
	a -> x = sum;
}	
void decrypt(long long int d, ull n, int fd, int fd_dest) {
	ull cipher;
	int i = 0;
	while(read(fd, &cipher, sizeof(ull))) {
		if(d < 0)
			d = d + n;
		element c;
		c.x = cipher;
		c.xx = (int*)malloc(sizeof(int) * MAX_DIGIT);
		convert_to_array(c.x, c.xx, &c.xLen); 
		ull message = power_mod(&c, d, n);
		write(fd_dest, &message, sizeof(ull));
	}
	return;
}
