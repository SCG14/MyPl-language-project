using System;

class CSharp {

  static void Main(string[] args) {
    int a = 5;
    int b = 7;
    int c = a + b;
    int d = c - 3;
    int t = 2;
    int e = 3;
    int f = t * e;
    bool g = true;
    bool h = false;
    bool i = g || h;
    bool j = !(i && (a > b));
    int k = 0;
    if (j) {
      k = 1;
    }
    else {
      k = 0;
    };
    Console.Write(k);
    int[] arr = new int[5];
    arr[0] = 0;
    arr[1] = 2;
    arr[2] = 4;
    arr[3] = 6;
    arr[4] = 8;
    int sum = 0;
    for (int m = 0; m < arr.Length; m = m + 1) {
      sum = sum + arr[m];
    };
    Console.Write(sum);
    Console.Write("\n");
  }
}
