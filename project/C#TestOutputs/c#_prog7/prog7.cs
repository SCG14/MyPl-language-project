using System;

class CSharp {

  static void Main(string[] args) {
    int n = 0;
    Console.Write(n);
    Console.Write("\n");
    string s = "hello";
    Console.Write(s.Length);
    Console.Write("\n");
    string t = "world";
    string u = String.Concat(s, t);
    Console.Write(u);
    Console.Write("\n");
    Console.Write(s[1]);
    Console.Write("\n");
    int i = 1;
    string x = i.ToString();
    Console.Write(x);
    Console.Write("\n");
    i = Convert.ToInt32(x);
    Console.Write(x);
    Console.Write("\n");
  }
}
