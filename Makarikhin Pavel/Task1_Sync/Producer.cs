using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Task1_Sync
{
    /// <summary>
    /// Класс производителя.
    /// </summary>
    public class Producer<T>
    {
        /// <summary>
        /// Помещаемый производителем объект.
        /// </summary>
        public T Obj { get; init; }

        /// <summary>
        /// Флаг завершения потока.
        /// </summary>
        public static bool Stop = false;

        /// <summary>
        /// Базовый конструктор класса Producer.
        /// </summary>
        /// <param name="obj">Объект, который производитель будем помещать в список.</param>
        public Producer(T obj)
        {
            Obj = obj;
        }

        /// <summary>
        /// Помещает в список объект.
        /// </summary>
        /// <typeparam name="T"></typeparam>
        /// <param name="list">Список объектов.</param>
        /// <param name="obj">Помещаемый объект.</param>
        public void Produce(List<T> list)
        {
            while (!Stop)
            {
                lock (list)
                {
                    list.Add(Obj);
                    Console.WriteLine("\"{0}\" was added to list. List: {1}", Obj, list.ListToString());
                }
                Thread.Sleep(1000);
            }

            Console.WriteLine("Thread \"{0}\" is finished!", Thread.CurrentThread.Name);
        }
    }
}
