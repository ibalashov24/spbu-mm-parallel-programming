using System;
using System.Threading;
using System.Threading.Tasks;

namespace HW1
{
    public class Consumer<T>
    {
        public Consumer(ICustomQueue<T> queue, int id, CancellationToken ct, int sleepTime = 1000)
        {
            Task.Factory.StartNew(() =>
                {
                    while (!ct.IsCancellationRequested)
                    {
                        if (queue.TryDequeue(out var result))
                        {
                            Console.WriteLine($"id: {id}, consumed value: {result}");
                        };
                        Thread.Sleep(sleepTime);
                    }
                }, ct, TaskCreationOptions.LongRunning, TaskScheduler.Default
            );
        }
    }
}