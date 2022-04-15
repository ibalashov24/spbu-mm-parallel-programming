using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DzSinx
{

    public class Class1
    {
        private readonly Mutex mutex = new Mutex();
        private readonly Queue<int> queue = new Queue<int>();

        public void Send(int i)
        {
            mutex.WaitOne();
            try
            {
                queue.Enqueue(i);
            }
            finally
            {
                mutex.ReleaseMutex();
            }
        }

        public int Recieve()
        {
            while (true)
            {
                mutex.WaitOne();
                try
                {
                    if (queue.TryDequeue(out var result))
                    {
                        return result;
                    }
                }
                finally
                {
                    mutex.ReleaseMutex();
                }
                Thread.Sleep(1000);
            }
        }
    }
}