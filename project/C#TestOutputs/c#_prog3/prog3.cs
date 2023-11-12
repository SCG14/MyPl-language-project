using System;

class CSharp {

  public struct Car {
    public string model;
    public int year;
    public string make;
    public int price;
  }

  static void Main(string[] args) {
    Car[] cars = new Car[5];
    Car tesla = new Car();
    tesla.make = "Tesla";
    tesla.model = "Model S";
    tesla.year = 2022;
    tesla.price = 79990;
    cars[0] = tesla;
    Car bmw = new Car();
    bmw.make = "BMW";
    bmw.model = "M3";
    bmw.year = 2021;
    bmw.price = 69900;
    cars[1] = bmw;
    Car audi = new Car();
    audi.make = "Audi";
    audi.model = "Q7";
    audi.year = 2022;
    audi.price = 59990;
    cars[2] = audi;
    Car mercedes = new Car();
    mercedes.make = "Mercedes";
    mercedes.model = "C-Class";
    mercedes.year = 2022;
    mercedes.price = 43900;
    cars[3] = mercedes;
    Car ford = new Car();
    ford.make = "Ford";
    ford.model = "Mustang";
    ford.year = 2020;
    ford.price = 27220;
    cars[4] = ford;
    for (int i = 0; i < cars.Length; i = i + 1) {
      Car car = cars[i];
      Console.Write("Make: ");
      Console.Write(car.make);
      Console.Write("\n");
      Console.Write("Model: ");
      Console.Write(car.model);
      Console.Write("\n");
      Console.Write("Year: ");
      Console.Write(car.year);
      Console.Write("\n");
      Console.Write("Price: $");
      Console.Write(car.price);
      Console.Write("\n\n");
    };
  }
}
