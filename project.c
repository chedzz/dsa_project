#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
typedef unsigned long long int ull;
ull ran_no_gen();
ull prime_gen();
ull my_pow(ull x, ull y,ull n);
ull extended_euclidean(ull, ull, long long int *, long long int *);

void private_key(long long int *, ull, ull);
int  prime(ull p);
time_t tt;
int main() {
	ull p, q = 0, totient, e;
	long long int d;
	int ran_count = 0;
	srandom(time(&tt));
	p = prime_gen(RAND_MAX);
	while(1) {
		
		q = prime_gen(RAND_MAX);
		ran_count++;
		if(q != p)
			break;
	}
	
	printf("p = %llu\nq = %llu\n", p, q);
		            
        
	totient = (p - 1) * (q - 1);
	while(1) {
		e = prime_gen(totient - 1);
		if((totient % e) != 0)
			break;
	}
	printf("totient = %llu\n", totient);	
	printf("public key (e) = %llu\n", e);
	printf("Calculating private key\n");
	private_key(&d, e, totient);
	printf("private key(d) =\n%llu\n", d);
	printf("de =\n%llu\n", d * e);
	//printf("de mod totient =  %llu\n", (d * e) % totient);
	
	return 0;
}
int  prime(ull p) {
//	printf("p = %llu\n", p);
	if(p%2==0)
		return 0;		
	long long int k = 0;
	ull exp, a;
	time_t tt;
	ull d = p - 1;
	while(d % 2 == 0) {
		d /= 2;
		k++;
	}
//	printf("k = %lld \t m = %llu\n", k, d);
	srandom(time(&tt));
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
	ull res = 1;
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
ull prime_gen(ull range) {
	ull n, t;
	int type = -1;
        int count = 0;
	while(1) {
		n = random();
		if(n < range) 
			break;
	}
//	printf("n originally = %llu\n", n);
	t = n / 6;
//	printf("t = %llu\n", t);
	n = 6 * t + 1;
//	printf("now n = %llu\n", n);
	while(type != 1) {
		type = prime(n);		
		count++;
		++t;
		n = 6 * t + 1;
	}
//	printf("back to prime_gen after prime() and after making type = 1\n");
	if(type == 1) {
		printf(" prime found after %d loops\n prime is :%llu\n", count, n);
	}	
	return n;
}
void private_key(long long int *d, ull e, ull totient) {
	ull gcd;
	long long int x, y;
	gcd = extended_euclidean(e, totient, &x, &y);
	printf("GCD = %llu\nx = %llu\ny = %llu\n", gcd, x, y);
	printf("xe + y.totient = %llu\n", x * e + y * totient);
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
