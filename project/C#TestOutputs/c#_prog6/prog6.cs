using System;

class CSharp {

  static void print_result(int n, int r) {
    Console.Write("fib(");
    Console.Write(n);
    Console.Write(") = ");
    Console.Write(r);
    Console.Write("\n");
  }

  static int fib(int x) {
    if (x <= 1) {
      return x;
    }
    else {
      return fib(x - 2) + fib(x - 1);
    };
  }

  static void Main(string[] args) {
    int n = 0;
    int m = 26;
    while (n < m) {
      print_result(n, fib(n));
      n = n + 1;
    };
  }
}
