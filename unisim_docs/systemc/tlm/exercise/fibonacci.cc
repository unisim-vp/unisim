#include<iostream>

using namespace std;

unsigned int fibonacci(unsigned int n) {
	if (1 == n || 2 == n) {
        return 1;
    } else {
		unsigned int t = (fibonacci(n-1) + fibonacci(n-2));
		return t;
    }
}

uint64_t fib2 (uint64_t n)
{
  // POST: return value is the n-th Fibonacci number F_n
  if (n == 0) return 0;
  if (n <= 2) return 1;
  uint64_t a = 1, b = 1;  // F_1, F_2 
  for (uint64_t i = 3; i <= n; ++i) {
    uint64_t a_prev = a;  // F_{i-2} 
    a = b;                    // F_{i-1}
    b += a_prev;              // F_{i-1} + F_{i-2} = F_i  
  }  
  return b;
}

int main() {
	unsigned int n = 50;
	cerr << n << endl;
//	cerr << fibonacci(n) << endl;
	cerr << fib2(n) << endl;
}
