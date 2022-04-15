using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DzSinx
{

    public class Consumer
    {
        private readonly Mutex mutex = new Mutex();
        private Thread? thread;
        private readonly int number;
        private volatile bool activated = false;

        public Consumer(int number)
        {
            this.number = number;
        }
        public void Stop()
        {
            activated = false;
        }

        public bool Run(Class1 clas)
        {
            mutex.WaitOne();
            try
            {
                if (activated)
                {
                    return false;
                }

                thread = new Thread(() =>
                {
                    while (activated)
                    {                        
                        Console.WriteLine($"C {number} - P {clas.Recieve()}");
                        Thread.Sleep(3000);
                    }
                });
                thread.Name = $"consumer_{number}";
                activated = true;
                thread.Start();
                return activated;
            }
            finally
            {
                mutex.ReleaseMutex();
            }
        }        
    }
}