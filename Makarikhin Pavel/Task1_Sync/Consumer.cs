using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Task1_Sync
{
    /// <summary>
    /// Класс потребителя.
    /// </summary>
    public class Consumer<T>
    {
        /// <summary>
        /// Флаг завершения потока.
        /// </summary>
        private volatile bool _Stop = false;

        /// <summary>
        /// Извлекает последний объект списка.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="list">Список объектов.</param>
        public void Consume(List<T> list)
        {
            while (!_Stop)
            {
                lock (list)
                {
                    if (list.Count > 0)
                    {
                        Console.Write("\"{0}\" was removed from list. ", list[list.Count - 1]);
                        list.RemoveAt(list.Count - 1);
                        Console.WriteLine("List: {0}", list.ListToString());
                    }
                    else
                    {
                        Console.WriteLine("list is empty!");
                    }
                }
                Thread.Sleep(1000);
            }

            Console.WriteLine("Thread \"{0}\" is finished!", Thread.CurrentThread.Name);
        }

        /// <summary>
        /// Останавливает работу потока.
        /// </summary>
        public void Stop()
        {
            _Stop = true;
        }
    }
}
