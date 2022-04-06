using Task1_Sync;

int consumerNumber;
int producerNumber;
List<int> listOfObjects = new List<int>();


/// <summary>
/// Создание потребителей и производителей.
/// </summary>
Console.WriteLine("Input number of producers: ");

if (!int.TryParse(Console.ReadLine(), out producerNumber))
{
    Console.WriteLine("Invalid input!");
}

Console.WriteLine("Input number of consumers: ");

if (!int.TryParse(Console.ReadLine(), out consumerNumber))
{
    Console.WriteLine("Invalid input!");
}

/// <summary>
/// Запуск потоков.
/// </summary>

for (int i = 0; i < producerNumber; i++)
{
    var producer = new Producer<int>(i);
    Thread thread = new Thread(() => { producer.Produce(listOfObjects); });
    thread.Name = $"Producer {i}";
    thread.Start();
}

for (int i = 0; i < consumerNumber; i++)
{
    var consumer = new Consumer<int>();
    Thread thread = new Thread(() => { consumer.Consume(listOfObjects); });
    thread.Name = $"Consumer {i}";
    thread.Start();
}

/// <summary>
/// Завершение потоков после нажатия любой кнопки.
/// </summary>
/// 
Console.ReadKey();

Producer<int>.Stop = true;
Consumer<int>.Stop = true;