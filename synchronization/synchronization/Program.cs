using synchronization;

const int consumersCount = 5;
const int producerCount = 5;

var channel = new Channel();
var consumers = new List<Consumer>();
var producers = new List<Producer>();

for (var i = 0; i < consumersCount; i++)
{
    var c = new Consumer(i);
    consumers.Add(c);
    c.Run(channel);
}

for (var i = 0; i < producerCount; i++)
{
    var p = new Producer(i);
    producers.Add(p);
    p.Run(channel);
}

Console.ReadKey();

producers.ForEach(p => p.Stop());
consumers.ForEach(c => c.Stop());