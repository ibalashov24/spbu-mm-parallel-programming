using System;
using System.Threading;
using System.Threading.Tasks;

namespace HW1
{
    public delegate void ConsumerCallback<T>(Guid id, T value);
    public class Consumer<T>
    {
        private Guid _id;
        public Consumer(ICustomQueue<T> queue, ConsumerCallback<T> ?callback, CancellationToken ct, int sleepTime = 1000)
        {
            
            _id = Guid.NewGuid();

            Task.Factory.StartNew(() =>
            {
                while (!ct.IsCancellationRequested)
                {
                    if (queue.TryDequeue(out var result))
                    {
                        if (callback is not null)
                        {
                            callback(_id, result);
                        }
                    };
                    Thread.Sleep(sleepTime);
                }
            }, ct, TaskCreationOptions.LongRunning, TaskScheduler.Default
            );
        }
        public Guid Id
        {
            get { return _id; }
        }
    }
}