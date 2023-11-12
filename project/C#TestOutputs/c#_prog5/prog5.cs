using System;

class CSharp {

  static void Main(string[] args) {
    int x = 10;
    if (x == 10) {
      Console.Write("x is equal to 10 \n");
    };
    int[] arr = new int[5];
    for (int i = 0; i < 5; i = i + 1) {
      arr[i] = i * 2;
    };
    int j = 0;
    while (j < 5) {
      Console.Write("arr[");
      Console.Write(j);
      Console.Write("] = ");
      Console.Write(arr[j]);
      Console.Write("\n");
      j = j + 1;
    };
    if (x > 5) {
      Console.Write("x is greater than 5 \n");
    }
    else if (x < 5) {
      Console.Write("x is less than 5 \n");
    }
    else {
      Console.Write("x is equal to 5 \n");
    };
  }
}
