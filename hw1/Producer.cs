
using System;
using System.Threading;
using System.Threading.Tasks;

namespace HW1
{
    public class Producer<T>
    {
        public Producer(ICustomQueue<T> queue, T value, CancellationToken ct, int sleepTime = 1000)
        {
            Task.Factory.StartNew(() =>
                {
                    while (!ct.IsCancellationRequested)
                    {
                        queue.Enqueue(value);
                        Thread.Sleep(sleepTime);
                    }
                }, ct, TaskCreationOptions.LongRunning, TaskScheduler.Default
            );
        }
    }
}