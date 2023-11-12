using System;

class CSharp {

  static void Main(string[] args) {
    Console.Write("CAESAR ENCRYPTOR\n");
    Console.Write("key = 13\n");
    int key = 13;
    string text = "Computer science is awesome";
    Console.Write("phrase: Computer science is awesome");
    Console.Write("\n");
    string enc_ch = "";
    for (int i = 0; i < text.Length; i = i + 1) {
      enc_ch = String.Concat(enc_ch, enc(text[i].ToString(), key));
    };
    Console.Write("encrypted phrase: ");
    Console.Write(enc_ch);
    Console.Write("\n");
  }

  static string enc(string ch, int key) {
    string[] letters = array_creation();
    string enc_ch = "";
    int enc_key = 0;
    if (ch == " ") {
      enc_key = 1000;
    }
    else {
      for (int i = 0; i < 25; i = i + 1) {
        if (ch == letters[i]) {
          enc_key = mod((i + key), 26);
        };
      };
    };
    if (enc_key == 1000) {
      enc_ch = String.Concat(enc_ch, " ");
    }
    else {
      for (int i = 0; i < 25; i = i + 1) {
        if (i == enc_key) {
          enc_ch = String.Concat(enc_ch, letters[i]);
        };
      };
    };
    return enc_ch;
  }

  static int div(int num1, int num2) {
    int count = 0;
    while (num1 >= num2) {
      num1 = num1 - num2;
      count = count + 1;
    };
    return count;
  }

  static int mod(int num1, int num2) {
    int divs = div(num1, num2);
    return num1 - (divs * num2);
  }

  static string[]  array_creation() {
    string[] letters = new string[26];
    letters[0] = "a";
    letters[1] = "b";
    letters[2] = "c";
    letters[3] = "d";
    letters[4] = "e";
    letters[5] = "f";
    letters[6] = "g";
    letters[7] = "h";
    letters[8] = "i";
    letters[9] = "j";
    letters[10] = "k";
    letters[11] = "l";
    letters[12] = "m";
    letters[13] = "n";
    letters[14] = "o";
    letters[15] = "p";
    letters[16] = "q";
    letters[17] = "r";
    letters[18] = "s";
    letters[19] = "t";
    letters[20] = "u";
    letters[21] = "v";
    letters[22] = "w";
    letters[23] = "x";
    letters[24] = "y";
    letters[25] = "z";
    return letters;
  }
}
