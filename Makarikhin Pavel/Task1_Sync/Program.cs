using Task1_Sync;


int consumerNumber;
int producerNumber;

var vList = new VolatileList<int>();
var consumers = new List<Consumer<int>>();
var producers = new List<Producer<int>>();

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
    producers.Add(producer);
    var thread = new Thread(() => { producer.Produce(vList.ListOfObjects); });
    thread.Name = $"Producer {i}";
    thread.Start();
}

for (int i = 0; i < consumerNumber; i++)
{
    var consumer = new Consumer<int>();
    consumers.Add(consumer);
    var thread = new Thread(() => { consumer.Consume(vList.ListOfObjects); });
    thread.Name = $"Consumer {i}";
    thread.Start();
}

/// <summary>
/// Завершение потоков после нажатия любой кнопки.
/// </summary>
/// 
Console.ReadKey();

producers.ForEach(x => x.Stop());
consumers.ForEach(x=>x.Stop());