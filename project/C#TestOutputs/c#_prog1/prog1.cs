using System;

class CSharp {

  static void quick_sort(int[]  xs, int start, int end) {
    if (start < end) {
      int pivot_val = xs[start];
      int end_p1 = start;
      for (int i = (start + 1); i <= end; i = i + 1) {
        if (xs[i] < pivot_val) {
          end_p1 = end_p1 + 1;
          int tmp = xs[i];
          xs[i] = xs[end_p1];
          xs[end_p1] = tmp;
        };
      };
      int tmp2 = xs[start];
      xs[start] = xs[end_p1];
      xs[end_p1] = tmp2;
      quick_sort(xs, start, end_p1 - 1);
      quick_sort(xs, end_p1 + 1, end);
    };
  }

  static void Main(string[] args) {
    Console.Write("quick sort\n");
    int[] xs = new int[100];
    int j = 100;
    for (int i = 0; i < 100; i = i + 1) {
      xs[i] = j;
      j = j - 1;
    };
    Console.Write("unsorted array: ");
    print_array(xs);
    quick_sort(xs, 0, 99);
    Console.Write("sorted array: ");
    print_array(xs);
  }

  static void print_array(int[]  xs) {
    int n = xs.Length;
    Console.Write("[");
    for (int i = 0; i < n; i = i + 1) {
      Console.Write(xs[i]);
      if (i != (n - 1)) {
        Console.Write(", ");
      };
    };
    Console.Write("]\n");
  }
}
