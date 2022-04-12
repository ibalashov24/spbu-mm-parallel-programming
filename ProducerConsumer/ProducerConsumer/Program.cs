using System;
using System.Collections.Generic;
using System.Threading;

namespace HW1
{
    internal static class Program
    {
        private static void producerCallback(Guid id, int value)
        {
            Console.WriteLine($"Produced {value} by {id}");
        }

        private static void consumerCallback(Guid id, int value)
        {
            Console.WriteLine($"Consumed {value} by {id}");
        }
        private static void Main(string[] args)
        {
            using var cts = new CancellationTokenSource();
            var ct = cts.Token;

            var queueWithLock = new QueueWithLock<int>();
            var producers = new List<Producer<int>>
            {
                new(queueWithLock, 1, producerCallback, ct),
                new(queueWithLock, 2, producerCallback, ct),
                new(queueWithLock, 3, producerCallback, ct),
                new(queueWithLock, 4, producerCallback, ct)
            };
            var consumers = new List<Consumer<int>>
            {
                new(queueWithLock, consumerCallback,  ct),
                new(queueWithLock, consumerCallback, ct)
            };

            Console.WriteLine("Press any key to stop...");
            Console.ReadKey();

            Console.WriteLine("\nCancellation requested");
            cts.Cancel();

        }
    }
}