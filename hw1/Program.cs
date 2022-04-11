using System;
using System.Collections.Generic;
using System.Threading;

namespace HW1
{
    internal static class Program
    {
        private static void Main(string[] args)
        {
            using var cts = new CancellationTokenSource();
            var ct = cts.Token;

            var queueWithLock = new QueueWithLock<int>();
            var producers = new List<Producer<int>>
            {
                new(queueWithLock, 1, ct),
                new(queueWithLock, 2, ct),
                new(queueWithLock, 3, ct),
                new(queueWithLock, 4, ct)
            };
            var consumers = new List<Consumer<int>>
            {
                new(queueWithLock, 1, ct),
                new(queueWithLock, 2, ct)
            };

            Console.WriteLine("Press any key to stop...");
            Console.ReadKey();

            Console.WriteLine("\nCancellation requested");
            cts.Cancel();

        }
    }
}