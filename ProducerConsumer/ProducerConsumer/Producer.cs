using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HW1
{
    public delegate void ProducerCallback<T>(Guid id, T value);
    public class Producer<T>
    {
        private Guid _id;
        public Producer(ICustomQueue<T> queue, T value, ProducerCallback<T> ?callback, CancellationToken ct, int sleepTime = 1000)
        {
            _id = Guid.NewGuid();

            Task.Factory.StartNew(() =>
            {
                while (!ct.IsCancellationRequested)
                {
                    queue.Enqueue(value);
                    if (callback is not null)
                    {
                        callback(_id, value);
                    }
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
